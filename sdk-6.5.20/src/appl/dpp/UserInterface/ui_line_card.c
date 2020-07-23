/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
/*
 * Basic_include_file.
 */

#if LINK_PSS_LIBRARIES
/*
 * General include file for reference design.
 */
#include <appl/diag/dpp/ref_sys.h>
/*
 * INCLUDE FILES:
 */

/* { */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
/*
 * Utilities include file.
 */
#include <appl/diag/dpp/utils_defx.h>
/*
 * User interface external include file.
 */
#include <appl/diag/dpp/ui_defx.h>
/*
 * User interface internal include file.
 */
#include <appl/dpp/UserInterface/ui_defi.h>
/*
 * Dune chips include file.7
 */
#include <appl/diag/dpp/dune_chips.h>
/* } */

#include <appl/diag/dpp/dune_rpc.h>

#include <appl/diag/dpp/utils_line_card.h>
#include <appl/diag/dpp/utils_fap_testing.h>
#include <appl/diag/dpp/utils_dune_fpga_download.h>
#include <appl/diag/dpp/utils_fap10m_pub.h>
#include <appl/diag/dpp/utils_i2c_mem.h>
/*****************************************************
*NAME
*  display_uplink_err_counters
*TYPE: PROC
*DATE: 22/JULY/2003
*FUNCTION:
*  Display all uplink errors as collected by
*  update_uplink_misc_int_cause_reg_counters() in
*  idle_proc().
*CALLING SEQUENCE:
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    Uplink_misc_int_cause_reg_counters.
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
void
  display_uplink_err_counters(
    void
  )
{
  char
    msg[8 * 80] ;
  sal_sprintf(msg,
    "\r\n"
    "        C2C/UPLINK error counters\r\n"
    "==========================================\r\n"
    ) ;
  send_string_to_screen(msg,FALSE) ;
  if (line_card_fap10m_polling_access() == FALSE)
  {
    sal_sprintf(
      msg,
      "NOTE: The UpLink error counter is not enabled.\n\r"
      "      The displayed counters maybe NOT upto-date.\n\r"
    ) ;
    send_string_to_screen(msg,TRUE) ;
  }
  sal_sprintf(msg,
    "c2c_narrow_ecc_err_on_header_counter = %lu\r\n"
    "c2c_narrow_ecc_err_on_data_counter   = %lu\r\n"
    "c2c_wide_ecc_err_on_header_counter   = %lu\r\n"
    "c2c_wide_ecc_err_on_data_counter     = %lu\r\n"
    "illegal_address_counter              = %lu\r\n"
    "msg_time_out_counter                 = %lu\r\n"
    "c2c_data_error_counter               = %lu\r\n",
    get_c2c_narrow_ecc_err_on_header_counter(),
    get_c2c_narrow_ecc_err_on_data_counter(),
    get_c2c_wide_ecc_err_on_header_counter(),
    get_c2c_wide_ecc_err_on_data_counter(),
    get_illegal_address_counter(),
    get_msg_time_out_counter(),
    get_c2c_data_error_counter()
  ) ;
  send_string_to_screen(msg,TRUE) ;
  sal_sprintf(msg,
    "==========================================\r\n"
    ) ;
  send_string_to_screen(msg,FALSE) ;
  return ;
}
/*
 */
/*****************************************************
*NAME
*  subject_line_card
*TYPE: PROC
*DATE: 29/DEC/2002
*FUNCTION:
*  Process input line which has an 'line_card' subject.
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
int subject_line_card(CURRENT_LINE *current_line, CURRENT_LINE **current_line_ptr)
{
  int           err,ui_ret ;
  PARAM_VAL     *param_val ;
  char          err_msg[80*4] = "";
  unsigned int  ui;
  char          *proc_name;
  char          *soc_sand_proc_name;
  BOOL          line_card_connected = 0 ;
  BOOL          get_val_of_err ;
  unsigned int match_index;
  enum
  {
    NO_ERR = 0,
    ERR_001, ERR_002, ERR_003, ERR_004,
    ERR_005, ERR_006, ERR_007, ERR_008,
    ERR_009, ERR_010, ERR_011, ERR_012,
    ERR_013, ERR_014, ERR_015, ERR_016,
    ERR_017, ERR_018, ERR_019, ERR_020,
    ERR_021, ERR_022, ERR_023, ERR_024,
    NUM_EXIT_CODES
  } ret ;

  soc_sand_proc_name = "Need to init 'soc_sand_proc_name' variable";
  proc_name = "subject_line_card";
  ret = NO_ERR ;
  get_val_of_err = FALSE;

  send_string_to_screen("",TRUE);

  if (get_slot_id())
  {
    /*
     * Enter if this is not a stand alone card.
     */
#if LINK_PSS_LIBRARIES
    line_card_connected = is_line_card_01_connected();
#endif
  }
  if (!line_card_connected)
  {
    send_string_to_screen("\n\rLine-Card-01 is NOT connected to this CPU-card.\n\r", FALSE);
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
    send_string_to_screen("\n\rLine-Card is connected to this CPU-card.\n\r", FALSE);
    goto exit ;
  }
  /*
   * Else, there are parameters on the line (not just 'line_card').
   */
  if ( (!search_param_val_pairs(current_line,&match_index,PARAM_LINE_CARD_FAP10M_1_I2C_ID,1))    ||
       (!search_param_val_pairs(current_line,&match_index,PARAM_LINE_CARD_FAP10M_1_E2PROM_ID,1)) ||
       (!search_param_val_pairs(current_line,&match_index,PARAM_LINE_CARD_PP_1_I2C_ID,1))        ||
       (!search_param_val_pairs(current_line,&match_index,PARAM_LINE_CARD_PP_1_PCI_ID,1))        ||
       (!search_param_val_pairs(current_line,&match_index,PARAM_LINE_CARD_PP_1_E2PROM_ID,1))     ||
       (!search_param_val_pairs(current_line,&match_index,PARAM_LINE_CARD_FPGA_DOWNLOAD_ID,1))
     )
  {
    /*
     * FAP10M_1_I2C, FAP10M_1_E2PROM, PP_1_E2PROM
     */
    LINE_CARD_DEV_TYPE
      dev_type;
    const char
      *dev_type_string;
    BOOL
      silent_mode;

    silent_mode = FALSE;
    dev_type_string = NULL;
    if (!search_param_val_pairs(current_line,&match_index,PARAM_LINE_CARD_FAP10M_1_I2C_ID,1))
    {
      dev_type = LINE_CARD_FAP10M_1_I2C;
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_LINE_CARD_FAP10M_1_E2PROM_ID,1))
    {
      dev_type = LINE_CARD_FAP10M_1_E2PROM;
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_LINE_CARD_PP_1_I2C_ID,1))
    {
      dev_type = LINE_CARD_PP_1_I2C;
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_LINE_CARD_PP_1_PCI_ID,1))
    {
      dev_type = LINE_CARD_PP_1_PCI;
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_LINE_CARD_PP_1_E2PROM_ID,1))
    {
      dev_type = LINE_CARD_PP_1_E2PROM;
    }
    else if(!search_param_val_pairs(current_line,&match_index,PARAM_LINE_CARD_FPGA_DOWNLOAD_ID,1))
    {
      unsigned long
        board_ver_id;

      if(line_card_fap10m_is_board_ver_b())
      {
        board_ver_id = UTILS_DUNE_FPGA_DOWNLOAD_V_2;
      }
      else
      {
        board_ver_id = UTILS_DUNE_FPGA_DOWNLOAD_V_1;
      }

      ui = utils_dune_download_and_load_fpga_rbf_file(board_ver_id);
      switch(ui)
      {
        case (0):  /* success */
        {
          send_string_to_screen("\r\n Download fpga file succeeded", TRUE);
          goto exit;
        }
        case(100): /* failed to download file */
        {
          send_string_to_screen("\r\n*** Download fpga file failed: download from host", TRUE);
          goto exit;
        }
        case(200): /* failed to get downloaded file handle */
        {
          send_string_to_screen("\r\n*** Download fpga file failed: SW err (failed to get file handle)", TRUE);
          goto exit;
        }
        case(300): /* failed to get downloaded file base or size */
        {
          send_string_to_screen("\r\n*** Download fpga file failed: SW err (failed to get file addr or size)", TRUE);
          goto exit;
        }
        case(410): /* failed to get downloaded file base or size */
        {
          send_string_to_screen("\r\n*** Download fpga file failed: FPGA init (jumper plugged in ?)", TRUE);
          goto exit;
        }
        case(420): /* failed to get downloaded file base or size */
        {
          send_string_to_screen("\r\n*** Download fpga file failed: FPGA serial download", TRUE);
          goto exit;
        }
        case(430): /* failed to get downloaded file base or size */
        {
          send_string_to_screen("\r\n*** Download fpga file failed: FPGA reports download failure", TRUE);
          goto exit;
        }
        case(500): /* failed to get downloaded file base or size */
        {
          send_string_to_screen("\r\n*** Download fpga file failed: SW err (failed to erase file after success)", TRUE);
          goto exit;
        }
        default:
        {
          sal_sprintf(err_msg, "\r\n*** Download fpga file failed: unknown error code %d from method", ui);
          send_string_to_screen(err_msg, TRUE);
          goto exit;
        }
      }
    }
    else
    {
      send_string_to_screen(" 1. Can not be here ....", TRUE);
      send_string_to_screen(__FILE__, TRUE);
      ret= TRUE;
      goto exit;
    }
    dev_type_string = line_card_dev_type_to_string(dev_type);

    if ((!search_param_val_pairs(current_line,&match_index,PARAM_LINE_CARD_SILENT_ID,1)))
    {
      /*
       * Reduced printouts.
       */
      silent_mode = TRUE;
    }

    if ((!search_param_val_pairs(current_line,&match_index,PARAM_LINE_CARD_READ_ID,1)))
    {
      /*
       * Read request.
       */
      unsigned long
        read_offset,
        *read_data,
        nof_reads,
        data_i,
        pp_dev_num;
      /*
       */
      read_data = NULL;
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_LINE_CARD_READ_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        /*
         * Value of read offset could not be retrieved.
         */
        get_val_of_err = TRUE;
        err = TRUE ;
        goto exit ;
      }
      read_offset = (unsigned long)param_val->value.ulong_value ;
      if ( (read_offset & 0x3) != 0)
      {
        sal_sprintf(err_msg,
                "-->> ERROR: read_offset = 0x%08lX is not LONG aligned ... \n\r",
                read_offset
                );
        send_string_to_screen(err_msg, FALSE);
        ret = ERR_001 ;
        goto exit;
      }

      if (get_val_of(
              current_line,(int *)&match_index,PARAM_LINE_CARD_NOF_READ_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        /*
         * Value of read offset could not be retrieved.
         */
        get_val_of_err = TRUE;
        err = TRUE ;
        goto exit ;
      }
      nof_reads = (unsigned long)param_val->value.ulong_value ;
      /*
       */
      read_data = malloc(sizeof(unsigned long)*nof_reads);
      if (read_data == NULL)
      {
        send_string_to_screen("-->> READ -- malloc failed.\n\r", TRUE);
        err = TRUE ;
        goto exit ;
      }
      /*
       */
      if (!silent_mode)
      {
        sal_sprintf(err_msg,
                "Trying to read from '%s',\n\r"
                "from offset 0x%08lX %lu longs\n\r",
                dev_type_string,
                read_offset,
                nof_reads
                );
        send_string_to_screen(err_msg, FALSE);
      }
      pp_dev_num = dune_rpc_get_remote_pp_id(0);

      /*
       */
      switch (dev_type)
      {
      case LINE_CARD_FAP10M_1_I2C:
      {
        err = line_card_fap10m_i2c_read(read_offset, nof_reads, read_data,silent_mode);
        break;
      }

      case LINE_CARD_FAP10M_1_E2PROM:
      {
        err = line_card_fap10m_e2prom_read(read_offset, nof_reads, read_data,silent_mode);
        break;
      }

      case LINE_CARD_PP_1_I2C:
      {
        err = line_card_pp_i2c_read(read_offset, nof_reads, read_data);
        break;
      }

      case LINE_CARD_PP_1_PCI:
      {
        err = line_card_pp_pci_read(pp_dev_num, read_offset, nof_reads, read_data);
        break;
      }

      case LINE_CARD_PP_1_E2PROM:
      {
        err = line_card_pp_e2prom_read(read_offset, nof_reads, read_data, silent_mode);
        break;
      }

      default:
      {
        send_string_to_screen("Not imple yet.", TRUE);
        ret= TRUE;
        free(read_data);
        goto exit;
        break;
      }
      }

      if (err)
      {
        /*
         * Read failed.
         */
        send_string_to_screen("-->> Read failed.", TRUE);
        ret= TRUE;
        free(read_data);
        goto exit;
      }

      if (silent_mode)
      {
        sal_sprintf(err_msg, "$ ");
      }
      else
      {
        sal_sprintf(err_msg, "Read Value: ");
      }
      send_string_to_screen(err_msg, FALSE);

      for (data_i=0; data_i < nof_reads; ++data_i)
      {
        if ( ( data_i    != 0)  &&
             ((data_i%4) == 0)
           )
        {
          send_string_to_screen("", TRUE);
        }
        if (silent_mode)
        {
          sal_sprintf(err_msg, "%08lX ", read_data[data_i]);
        }
        else
        {
          sal_sprintf(err_msg, "0x%08lX  ", read_data[data_i]);
        }
        send_string_to_screen(err_msg, FALSE);
      }
      free(read_data);
    }
    else if ((!search_param_val_pairs(current_line,&match_index,PARAM_LINE_CARD_VAL_READ_ID,1)))
    {
      /*
       * Read-Val compare request.
       */
      unsigned long
        read_offset,
        mask,
        timeout,
        read_data,
        read_val,
        operation;
      int
        fail;
      PARAM_VAL
        *operation_param_val ;

      /*
       */
      read_data = 0;
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_LINE_CARD_VAL_READ_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        /*
         * Value of read offset could not be retrieved.
         */
        err = TRUE ;
        goto exit ;
      }
      read_offset = (unsigned long)param_val->value.ulong_value ;
      if ( (read_offset & 0x3) != 0)
      {
        sal_sprintf(err_msg,
                "-->> ERROR: read_offset = 0x%08lX is not LONG aligned ... \n\r",
                read_offset
                );
        send_string_to_screen(err_msg, FALSE);
        ret = ERR_002 ;
        goto exit;
      }
      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_LINE_CARD_MASK_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        /*
         * Value of format could not be retrieved.
         */
        ret = ERR_003 ;
        goto exit ;
      }
      mask = (unsigned int)param_val->value.ulong_value ;
      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_LINE_CARD_TIMEOUT_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        /*
         * Value of format could not be retrieved.
         */
        ret = ERR_004 ;
        goto exit ;
      }
      timeout = (unsigned long)param_val->value.ulong_value ;
      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_LINE_CARD_VAL_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        /*
         * Value of format could not be retrieved.
         */
        ret = ERR_005 ;
        goto exit ;
      }
      read_val = (unsigned long)param_val->value.ulong_value ;
      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_LINE_CARD_OPERATION_ID,1,
              &operation_param_val,VAL_SYMBOL,err_msg))
      {
        /*
         * Value of format could not be retrieved.
         */
        ret = ERR_006 ;
        goto exit ;
      }
      operation = (unsigned long)operation_param_val->numeric_equivalent ;
      /*
       */
      if (!silent_mode)
      {
        sal_sprintf(err_msg,
                "Trying to read val from '%s',\n\r"
                "from offset 0x%08lX,\n\r"
                "Mask with 0x%08lX and compare with 0x%08lX (%s)\n\r",
                dev_type_string,
                read_offset,
                mask,
                read_val,
                operation_param_val->value.string_value
                );
        send_string_to_screen(err_msg, FALSE);
      }
      /*
       */
      fail = line_card_read_compare(
               dev_type,
               read_offset,
               read_val,
               mask,
               operation,
               timeout,
               silent_mode,
               &read_data
            );
      /*
       */
      if (silent_mode)
      {
        if (fail)
        {
          sal_sprintf(err_msg, "$ Fail %08lX", read_data);
        }
        else
        {
          sal_sprintf(err_msg, "$ Pass %08lX", read_data);
        }
        send_string_to_screen(err_msg, TRUE);
      }
      else
      {
        if (!silent_mode)
        {
          if (fail)
          {
            send_string_to_screen(
              "\r\n"
              "*** \'val_read\' comparison has failed!!!",
              TRUE) ;
          }
        }
      }
    }
    else if ((!search_param_val_pairs(current_line,&match_index,PARAM_LINE_CARD_WRITE_ID,1)))
    {
      /*
       * Read request.
       */
      unsigned long
        write_start_offset,
        data_to_write[MAX_REPEATED_PARAM_VAL],
        nof_data_to_write,
        data_i,
        pp_dev_num;
      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_LINE_CARD_WRITE_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        /*
         * Value of write offset could not be retrieved.
         */
        err = TRUE ;
        goto exit ;
      }
      write_start_offset = (unsigned long)param_val->value.ulong_value ;
      if ( (write_start_offset & 0x3) != 0)
      {
        sal_sprintf(err_msg,
                "-->> ERROR: write_start_offset = 0x%08lX is not LONG aligned ... \n\r",
                write_start_offset
                );
        send_string_to_screen(err_msg, FALSE);
        ret = ERR_007 ;
        goto exit;
      }

      for (nof_data_to_write=0; nof_data_to_write<MAX_REPEATED_PARAM_VAL; ++nof_data_to_write)
      {
        if (get_val_of(
              current_line,(int *)&match_index,PARAM_LINE_CARD_DATA_ID,nof_data_to_write+1,
              &param_val,VAL_NUMERIC,err_msg))
        {
          /*
           * If there are no more data values then quit loop.
           */
          break ;
        }
        data_to_write[nof_data_to_write] = param_val->value.ulong_value ;
      }
      if (0 == nof_data_to_write)
      {
        /*
         * 0 data to write has been entered.
         */
        send_string_to_screen(
            "\r\n"
            "*** 97. Should not reach here.\r\n",
            FALSE) ;
        ret = ERR_008 ;
        goto exit ;
      }

      if (!silent_mode)
      {
        sal_sprintf(err_msg,
                "Trying to write to '%s',\n\r"
                "  offset 0x%08lX, the %lu values:\n\r",
                dev_type_string,
                write_start_offset,
                nof_data_to_write
                );
        send_string_to_screen(err_msg, FALSE);
        for (data_i=0; data_i < nof_data_to_write; ++data_i)
        {
          if ((data_i%5 == 0) && (data_i != 0))
          {
            send_string_to_screen("", TRUE);
          }
          sal_sprintf(err_msg,
                  "0x%08lX  ",
                  data_to_write[data_i]
                  );
          send_string_to_screen(err_msg, FALSE);
        }
        send_string_to_screen("", TRUE);
      }
      pp_dev_num = dune_rpc_get_remote_pp_id(0);

      /*
       */
      switch (dev_type)
      {
      case LINE_CARD_FAP10M_1_I2C:
      {
        line_card_fap10m_i2c_write(
          write_start_offset, nof_data_to_write, data_to_write, silent_mode
        );
        break;
      }

      case LINE_CARD_FAP10M_1_E2PROM:
      {
        line_card_fap10m_e2prom_write(
          write_start_offset, nof_data_to_write, data_to_write, silent_mode
         );
        break;
      }

      case LINE_CARD_PP_1_I2C:
      {
        line_card_pp_i2c_write(
          write_start_offset, nof_data_to_write, data_to_write
        );
        break;
      }

      case LINE_CARD_PP_1_PCI:
      {
        line_card_pp_pci_write(
          pp_dev_num, write_start_offset, nof_data_to_write, data_to_write
        );
        break;
      }

      case LINE_CARD_PP_1_E2PROM:
      {
        line_card_pp_e2prom_write(
          write_start_offset, nof_data_to_write, data_to_write, silent_mode
         );
        break;
      }

      default:
      {
        send_string_to_screen("Not imple yet.", TRUE);
        ret= TRUE;
        goto exit;
        break;
      }
      }
    }
    else if ((!search_param_val_pairs(current_line,&match_index,PARAM_LINE_CARD_ERASE_ID,1)))
    {
      /*
       * Erase request.
       */
      unsigned long
        erase_offset,
        *erase_data,
        nof_reads;
      /*
       */
      erase_data = NULL;
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_LINE_CARD_ERASE_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        /*
         * Value of erase offset could not be retrieved.
         */
        get_val_of_err = TRUE;
        err = TRUE ;
        goto exit ;
      }
      erase_offset = (unsigned long)param_val->value.ulong_value ;
      if ( (erase_offset & 0x3) != 0)
      {
        sal_sprintf(err_msg,
                "-->> ERROR: erase_offset = 0x%08lX is not LONG aligned ... \n\r",
                erase_offset
                );
        send_string_to_screen(err_msg, FALSE);
        ret = ERR_009 ;
        goto exit;
      }

      if (get_val_of(
              current_line,(int *)&match_index,PARAM_LINE_CARD_NOF_READ_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        /*
         * Value of read offset could not be retrieved.
         */
        get_val_of_err = TRUE;
        err = TRUE ;
        goto exit ;
      }
      nof_reads = (unsigned long)param_val->value.ulong_value ;

      erase_data = malloc(sizeof(unsigned long)*nof_reads);
      if (erase_data == NULL)
      {
        send_string_to_screen("-->> ERASE -- malloc failed.\n\r", TRUE);
        err = TRUE ;
        goto exit ;
      }
      memset(erase_data, 0xFF, sizeof(unsigned long)*nof_reads);

      if (!silent_mode)
      {
        sal_sprintf(err_msg,
                "Trying to erase from '%s',\n\r"
                " via the I2C from offset 0x%08lX %lu longs.\n\r",
                dev_type_string,
                erase_offset,
                nof_reads
                );
        send_string_to_screen(err_msg, FALSE);
      }

      /*
       */
      switch (dev_type)
      {
      case LINE_CARD_FAP10M_1_E2PROM:
      {
        err = line_card_fap10m_e2prom_write(
                erase_offset, nof_reads, erase_data, silent_mode
              );
        break;
      }

      case LINE_CARD_PP_1_E2PROM:
      {
        err = line_card_pp_e2prom_write(
                erase_offset, nof_reads, erase_data, silent_mode
              );
        break;
      }

      default:
      {
        send_string_to_screen("Not imple yet.", TRUE);
        ret= TRUE;
        free(erase_data);
        goto exit;
        break;
      }
      }

      if (err)
      {
        /*
         * Read failed.
         */
        send_string_to_screen("-->> Erase failed.", TRUE);
        ret= TRUE;
        free(erase_data);
        goto exit;
      }

      free(erase_data);
    }
    else
    {
      /*
       * No valid operation variable has been entered.
       */
      send_string_to_screen(
          "\r\n"
          "*** 98. Should not reach here.\r\n",
          FALSE) ;
      ret = ERR_010 ;
      goto exit ;
    }
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_LINE_TRIMMERS_READ_ID,1))
  {
    unsigned int
      byte_i;
    unsigned char
      device_addr,
      nof_bytes,
      internal_addr,
      i2c_buff[I2C_MEM_MAX_IC2_BUFF_SIZE];

    if (get_val_of(
            current_line,(int *)&match_index,PARAM_LINE_TRIMMERS_READ_ID,1,
            &param_val,VAL_SYMBOL,err_msg))
    {
      /*
       * Value of read offset could not be retrieved.
       */
      get_val_of_err = TRUE;
      err = TRUE ;
      goto exit ;
    }
    device_addr =   (unsigned char)param_val->numeric_equivalent ;

    if (get_val_of(
            current_line,(int *)&match_index,PARAM_LINE_TRIMMERS_INTERNAL_ADDR_ID,1,
            &param_val,VAL_NUMERIC,err_msg))
    {
      /*
       * Value of read offset could not be retrieved.
       */
      get_val_of_err = TRUE;
      err = TRUE ;
      goto exit ;
    }
    internal_addr =   (unsigned char)param_val->value.ulong_value ;

    if (get_val_of(
            current_line,(int *)&match_index,PARAM_LINE_TRIMMERS_READ_BYTE_NUM_ID,1,
            &param_val,VAL_NUMERIC,err_msg))
    {
      /*
       * Value of read offset could not be retrieved.
       */
      get_val_of_err = TRUE;
      err = TRUE ;
      goto exit ;
    }
    nof_bytes = (unsigned long)param_val->value.ulong_value ;

    line_card_trimmer_read(
      device_addr,
      internal_addr,
      nof_bytes,
      i2c_buff,
      FALSE
    );
/*TODO!!*/
    d_printf("utils_i2c_mem_read read the following:\n\r");
    for(byte_i=0;byte_i<nof_bytes;byte_i++)
    {
      d_printf("BYTE %d:, Value 0x%x.\n\r", byte_i, i2c_buff[byte_i]);
    }
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_LINE_TRIMMERS_WRITE_ID,1))
  {
    unsigned char
      internal_addr,
      device_addr;
    unsigned char
      i2c_buff[I2C_MEM_MAX_IC2_BUFF_SIZE];

    if (get_val_of(
            current_line,(int *)&match_index,PARAM_LINE_TRIMMERS_WRITE_ID,1,
            &param_val,VAL_SYMBOL,err_msg))
    {
      /*
       * Value of read offset could not be retrieved.
       */
      get_val_of_err = TRUE;
      err = TRUE ;
      goto exit ;
    }
    device_addr = (unsigned char)param_val->numeric_equivalent ;

    if (get_val_of(
            current_line,(int *)&match_index,PARAM_LINE_TRIMMERS_INTERNAL_ADDR_ID,1,
            &param_val,VAL_NUMERIC,err_msg))
    {
      /*
       * Value of read offset could not be retrieved.
       */
      get_val_of_err = TRUE;
      err = TRUE ;
      goto exit ;
    }
    internal_addr =   (unsigned char)param_val->value.ulong_value ;

    if (get_val_of(
            current_line,(int *)&match_index,PARAM_LINE_TRIMMERS_WRITE_DATA_ID,1,
            &param_val,VAL_NUMERIC,err_msg))
    {
      /*
       * Value of read offset could not be retrieved.
       */
      get_val_of_err = TRUE;
      err = TRUE ;
      goto exit ;
    }
    i2c_buff[0] = (unsigned char)param_val->value.ulong_value;


    line_card_trimmer_write(
      device_addr,
      internal_addr,
      1,
      i2c_buff,
      FALSE
    );
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_LINE_FPGA_WRITE_ID,1))
  {
    unsigned char
      addr,
      data;

    soc_sand_proc_name = "fpga write";
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_LINE_FPGA_WRITE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    addr = (unsigned char)(param_val->value.ulong_value);


    UI_MACROS_GET_NUMMERIC_VAL(PARAM_LINE_FPGA_WRITE_DATA_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data = (unsigned char)(param_val->value.ulong_value);

    line_card_fpga_write(
      addr,
      data
    );
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_LINE_FPGA_READ_ID,1))
  {
    unsigned char
      addr,
      data;

    soc_sand_proc_name = "fpga read";
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_LINE_FPGA_READ_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    addr = (unsigned char)(param_val->value.ulong_value);

    data =
      line_card_fpga_read(
        addr
      );
    sal_sprintf(err_msg, "FPGA Offset 0x%x: Data: 0x%x.", addr, data);
    send_string_to_screen(err_msg, TRUE);


  }
  else if ( (!search_param_val_pairs(current_line,&match_index,PARAM_LINE_CARD_FAP10M_1_ID,1)) ||
            (!search_param_val_pairs(current_line,&match_index,PARAM_LINE_CARD_PP_1_ID,1))
          )
  {
    /*
     * FAP10M_1_I2C, FAP10M_1_E2PROM, PP_1_E2PROM
     */
    LINE_CARD_DEV_TYPE
      dev_type;
    const char
      *dev_type_string;

    dev_type_string = NULL;
    if (!search_param_val_pairs(current_line,&match_index,PARAM_LINE_CARD_FAP10M_1_ID,1))
    {
      dev_type = LINE_CARD_FAP10M_1;
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_LINE_CARD_PP_1_ID,1))
    {
      dev_type = LINE_CARD_PP_1;
    }
    else
    {
      send_string_to_screen(" 2. Can not be here ....", TRUE);
      send_string_to_screen(__FILE__, TRUE);
      ret= TRUE;
      goto exit;
    }
    dev_type_string = line_card_dev_type_to_string(dev_type);

    if ((!search_param_val_pairs(current_line,&match_index,PARAM_LINE_CARD_RESET_ID,1)))
    {
      BOOL
        stay_down;
      unsigned long
        down_milisec;
      /*
 */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_LINE_CARD_MILISEC_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        /*
         * Value of down-time could not be retrieved set default.
         */
        down_milisec = 200 ;
      }
      else
      {
        down_milisec = (unsigned long)param_val->value.ulong_value ;
      }
      /*
       */
      if (search_param_val_pairs(current_line,&match_index,PARAM_LINE_CARD_STAY_DOWN_ID,1))
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
      ret = line_card_reset_device(
              dev_type,
              down_milisec * 1000,
              stay_down
            );
      if (ret)
      {
        sal_sprintf(
          err_msg,
          "\n\r-->> line_card_reset_device(%s, %lu, %u)-- failed.",
          line_card_dev_type_to_string(dev_type),
          down_milisec,
          stay_down
        );
        send_string_to_screen(err_msg, TRUE);
      }
      goto exit;

    }
    else if ((!search_param_val_pairs(current_line,&match_index,PARAM_LINE_CARD_FAP10M_CHECK_EDDR_ID,1)))
    {
      BOOL
        test_pass;
      /*
       */
      err = line_card_fap10m_eddr_check(
              &test_pass
            );
      if (err)
      {
        sal_sprintf(
          err_msg,
          "*** line_card_fap10m_eddr_check retuned with error\n\r"
        );
        send_string_to_screen(err_msg,TRUE);
        goto exit;
      }
    }
    else if ((!search_param_val_pairs(current_line,&match_index,PARAM_LINE_CARD_FAP10M_CHECK_IDDR_ID,1)))
    {
      BOOL
        test_pass;
      /*
       */
      err = line_card_fap10m_iddr_check(
              &test_pass
            );
      if (err)
      {
        sal_sprintf(
          err_msg,
          "*** line_card_fap10m_iddr_check retuned with error\n\r"
        );
        send_string_to_screen(err_msg,TRUE);
        goto exit;
      }
    }
    else if ((!search_param_val_pairs(current_line,&match_index,PARAM_LINE_CARD_FAP10M_LOAD_EDDR_DFCDL_ID,1)))
    {
      unsigned long
        rx_clk,
        tx_clk,
        dqs;
      BOOL
        auto_poll;
      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_LINE_CARD_DFCDL_DQS_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit ;
      }
      dqs = param_val->value.ulong_value ;
      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_LINE_CARD_DFCDL_RX_CLK_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit ;
      }
      rx_clk = param_val->value.ulong_value ;
      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_LINE_CARD_DFCDL_TX_CLK_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit ;
      }
      tx_clk = param_val->value.ulong_value ;
      /*
       */
      if (!search_param_val_pairs(current_line,&match_index,PARAM_LINE_CARD_DFCDL_AUTO_POLL_ID,1))
      {
        auto_poll = TRUE;
      }
      else
      {
        auto_poll = FALSE;
      }

      /*
       */
      sal_sprintf(
        err_msg,
        "-->>%s\n\r"
        "    line_card_fap10m_load_eddr_dfcdl_auto_poll(rx_clk-%lu, tx_clk-%lu, dqs-%lu, auto_poll-%u)",
        line_card_dev_type_to_string(dev_type),
        rx_clk, tx_clk, dqs, auto_poll
      );
      send_string_to_screen(err_msg,TRUE);

      /*
       */
      err = line_card_fap10m_load_eddr_dfcdl_auto_poll(
              rx_clk, tx_clk, dqs, auto_poll
            );
      if (err)
      {
        sal_sprintf(
          err_msg,
          "*** line_card_fap10m_load_eddr_dfcdl_auto_poll retuned with error\n\r"
        );
        send_string_to_screen(err_msg,TRUE);
        goto exit;
      }

    }
    else if ((!search_param_val_pairs(current_line,&match_index,PARAM_LINE_CARD_FAP10M_LOAD_C2C_DFCDL_ID,1)))
    {
      unsigned long
        wide,
        narrow;
      BOOL
        auto_poll;
      /*
       */
      auto_poll = FALSE;
      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_LINE_CARD_C2C_DFCDL_NARROW_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit ;
      }
      narrow = param_val->value.ulong_value ;
      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_LINE_CARD_C2C_DFCDL_WIDE_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit ;
      }
      wide = param_val->value.ulong_value ;
      /*
       */
      if (!search_param_val_pairs(current_line,&match_index,PARAM_LINE_CARD_DFCDL_AUTO_POLL_ID,1))
      {
        auto_poll = TRUE;
      }
      else
      {
        auto_poll = FALSE;
      }

      /*
       */
      sal_sprintf(
        err_msg,
        "-->>%s\n\r"
        "    line_card_fap10m_load_c2c_dfcdl_auto_poll(wide-%lu, narrow-%lu, auto_poll-%u)",
        line_card_dev_type_to_string(dev_type),
        wide, narrow, auto_poll
      );
      send_string_to_screen(err_msg,TRUE);

      /*
       */
      err = line_card_fap10m_load_c2c_dfcdl_auto_poll(
              wide, narrow, auto_poll
            );
      if (err)
      {
        sal_sprintf(
          err_msg,
          "*** line_card_fap10m_load_c2c_dfcdl_auto_poll retuned with error\n\r"
        );
        send_string_to_screen(err_msg,TRUE);
        goto exit;
      }

    }
    else if ((!search_param_val_pairs(current_line,&match_index,PARAM_LINE_CARD_FAP10M_LOAD_IDDR_DFCDL_ID,1)))
    {
      unsigned long
        rx_clk,
        tx_clk,
        dqs;
      BOOL
        auto_poll;
      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_LINE_CARD_DFCDL_DQS_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit ;
      }
      dqs = param_val->value.ulong_value ;
      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_LINE_CARD_DFCDL_RX_CLK_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit ;
      }
      rx_clk = param_val->value.ulong_value ;
      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_LINE_CARD_DFCDL_TX_CLK_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit ;
      }
      tx_clk = param_val->value.ulong_value ;
      /*
       */
      if (!search_param_val_pairs(current_line,&match_index,PARAM_LINE_CARD_DFCDL_AUTO_POLL_ID,1))
      {
        auto_poll = TRUE;
      }
      else
      {
        auto_poll = FALSE;
      }

      /*
       */
      sal_sprintf(
        err_msg,
        "-->>%s\n\r"
        "    line_card_fap10m_load_iddr_dfcdl_auto_poll(rx_clk-%lu, tx_clk-%lu, dqs-%lu, auto_poll-%u)",
        line_card_dev_type_to_string(dev_type),
        rx_clk, tx_clk, dqs, auto_poll
      );
      send_string_to_screen(err_msg,TRUE);

      /*
       */
      err = line_card_fap10m_load_iddr_dfcdl_auto_poll(
              rx_clk, tx_clk, dqs, auto_poll
            );
      if (err)
      {
        sal_sprintf(
          err_msg,
          "*** line_card_fap10m_load_iddr_dfcdl_auto_poll retuned with error\n\r"
        );
        send_string_to_screen(err_msg,TRUE);
        goto exit;
      }
    }
    else if ((!search_param_val_pairs(current_line,&match_index,PARAM_LINE_CARD_FAP10M_LOAD_EDDR_FTDLL_ID,1)))
    {
      unsigned long
        start_burst,
        clk_out,
        dqs;
      BOOL
        auto_poll;
      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_LINE_CARD_FTDLL_DQS_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit ;
      }
      dqs = param_val->value.ulong_value ;
      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_LINE_CARD_FTDLL_START_BURST_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit ;
      }
      start_burst = param_val->value.ulong_value ;
      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_LINE_CARD_FTDLL_CLK_OUT_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit ;
      }
      clk_out = param_val->value.ulong_value ;
      /*
       */
      if (!search_param_val_pairs(current_line,&match_index,PARAM_LINE_CARD_FTDLL_AUTO_POLL_ID,1))
      {
        auto_poll = TRUE;
      }
      else
      {
        auto_poll = FALSE;
      }

      /*
       */
      sal_sprintf(
        err_msg,
        "-->>%s\n\r"
        "    line_card_fap10m_load_eddr_ftdll_auto_poll(start_burst-%lu, clk_out-%lu, dqs-%lu, auto_poll-%u)",
        line_card_dev_type_to_string(dev_type),
        start_burst, clk_out, dqs, auto_poll
      );
      send_string_to_screen(err_msg,TRUE);

      /*
       */
      err = line_card_fap10m_load_eddr_ftdll_auto_poll(
              start_burst, clk_out, dqs, auto_poll
            );
      if (err)
      {
        sal_sprintf(
          err_msg,
          "*** line_card_fap10m_load_eddr_ftdll_auto_poll retuned with error\n\r"
        );
        send_string_to_screen(err_msg,TRUE);
        goto exit;
      }

    }
    else if ((!search_param_val_pairs(current_line,&match_index,PARAM_LINE_CARD_FAP10M_LOAD_C2C_FTDLL_ID,1)))
    {
      unsigned long
        wide,
        narrow;
      BOOL
        auto_poll;
      /*
       */
      auto_poll = FALSE;
      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_LINE_CARD_C2C_FTDLL_NARROW_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit ;
      }
      narrow = param_val->value.ulong_value ;
      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_LINE_CARD_C2C_FTDLL_WIDE_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit ;
      }
      wide = param_val->value.ulong_value ;
      /*
       */
      if (!search_param_val_pairs(current_line,&match_index,PARAM_LINE_CARD_FTDLL_AUTO_POLL_ID,1))
      {
        auto_poll = TRUE;
      }
      else
      {
        auto_poll = FALSE;
      }

      /*
       */
      sal_sprintf(
        err_msg,
        "-->>%s\n\r"
        "    line_card_fap10m_load_c2c_ftdll_auto_poll(wide-%lu, narrow-%lu, auto_poll-%u)",
        line_card_dev_type_to_string(dev_type),
        wide, narrow, auto_poll
      );
      send_string_to_screen(err_msg,TRUE);

      /*
       */
      err = line_card_fap10m_load_c2c_ftdll_auto_poll(
              wide, narrow, auto_poll
            );
      if (err)
      {
        sal_sprintf(
          err_msg,
          "*** line_card_fap10m_load_c2c_ftdll_auto_poll retuned with error\n\r"
        );
        send_string_to_screen(err_msg,TRUE);
        goto exit;
      }

    }
    else if ((!search_param_val_pairs(current_line,&match_index,PARAM_LINE_CARD_PP_LOAD_C2C_FTDLL_ID,1)))
    {
      unsigned long
        wide,
        narrow;
      BOOL
        auto_poll;
      /*
       */
      auto_poll = FALSE;
      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_LINE_CARD_C2C_FTDLL_NARROW_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit ;
      }
      narrow = param_val->value.ulong_value ;
      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_LINE_CARD_C2C_FTDLL_WIDE_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit ;
      }
      wide = param_val->value.ulong_value ;

      if (!search_param_val_pairs(current_line,&match_index,PARAM_LINE_CARD_FTDLL_AUTO_POLL_ID,1))
      {
        auto_poll = TRUE;
      }
      else
      {
        auto_poll = FALSE;
      }
      /*
       */
      sal_sprintf(
        err_msg,
        "-->>%s\n\r"
        "    line_card_set_pp_c2c_ftdll (wide-%lu, narrow-%lu, auto_poll = %d)",
        line_card_dev_type_to_string(dev_type),
        wide, narrow, auto_poll
      );
      send_string_to_screen(err_msg,TRUE);

      /*
       */
      err = line_card_set_pp_c2c_ftdll(
              wide, narrow, auto_poll
            );
      if (err)
      {
        sal_sprintf(
          err_msg,
          "*** line_card_set_pp_c2c_ftdll retuned with error\n\r"
        );
        send_string_to_screen(err_msg,TRUE);
        goto exit;
      }

    }
    else if ((!search_param_val_pairs(current_line,&match_index,PARAM_LINE_CARD_FAP10M_LOAD_IDDR_FTDLL_ID,1)))
    {
      unsigned long
        start_burst,
        clk_out,
        dqs;
      BOOL
        auto_poll;
      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_LINE_CARD_FTDLL_DQS_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit ;
      }
      dqs = param_val->value.ulong_value ;
      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_LINE_CARD_FTDLL_START_BURST_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit ;
      }
      start_burst = param_val->value.ulong_value ;
      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_LINE_CARD_FTDLL_CLK_OUT_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit ;
      }
      clk_out = param_val->value.ulong_value ;
      /*
       */
      if (!search_param_val_pairs(current_line,&match_index,PARAM_LINE_CARD_FTDLL_AUTO_POLL_ID,1))
      {
        auto_poll = TRUE;
      }
      else
      {
        auto_poll = FALSE;
      }

      /*
       */
      sal_sprintf(
        err_msg,
        "-->>%s\n\r"
        "    line_card_fap10m_load_iddr_ftdll_auto_poll(start_burst-%lu, clk_out-%lu, dqs-%lu, auto_poll-%u)",
        line_card_dev_type_to_string(dev_type),
        start_burst, clk_out, dqs, auto_poll
      );
      send_string_to_screen(err_msg,TRUE);

      /*
       */
      err = line_card_fap10m_load_iddr_ftdll_auto_poll(
              start_burst, clk_out, dqs, auto_poll
            );
      if (err)
      {
        sal_sprintf(
          err_msg,
          "*** line_card_fap10m_load_iddr_ftdll_auto_poll retuned with error\n\r"
        );
        send_string_to_screen(err_msg,TRUE);
        goto exit;
      }
    }
    else if ((!search_param_val_pairs(current_line,&match_index,PARAM_LINE_CARD_PP_LOAD_IDDR_DFCDL_ID,1)))
    {
      unsigned long
        rx_clk,
        tx_clk,
        dqs;
      BOOL
        auto_poll;
      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_LINE_CARD_DFCDL_DQS_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit ;
      }
      dqs = param_val->value.ulong_value ;
      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_LINE_CARD_DFCDL_RX_CLK_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit ;
      }
      rx_clk = param_val->value.ulong_value ;
      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_LINE_CARD_DFCDL_TX_CLK_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit ;
      }
      tx_clk = param_val->value.ulong_value ;
      /*
       */
      if (!search_param_val_pairs(current_line,&match_index,PARAM_LINE_CARD_DFCDL_AUTO_POLL_ID,1))
      {
        auto_poll = TRUE;
      }
      else
      {
        auto_poll = FALSE;
      }

      /*
       */
      sal_sprintf(
        err_msg,
        "-->>%s\n\r"
        "    line_card_pp_load_iddr_dfcdl_auto_poll(rx_clk-%lu, tx_clk-%lu, dqs-%lu, auto_poll-%u)",
        line_card_dev_type_to_string(dev_type),
        rx_clk, tx_clk, dqs, auto_poll
      );
      send_string_to_screen(err_msg,TRUE);

      /*
       */
      err = line_card_pp_load_iddr_dfcdl_auto_poll(
              rx_clk, tx_clk, dqs, auto_poll
            );
      if (err)
      {
        sal_sprintf(
          err_msg,
          "*** line_card_pp_load_iddr_dfcdl_auto_poll retuned with error\n\r"
        );
        send_string_to_screen(err_msg,TRUE);
        goto exit;
      }

    }
    else if ((!search_param_val_pairs(current_line,&match_index,PARAM_LINE_CARD_PP_LOAD_DDR_FTDLL_ID,1)))
    {
      unsigned long
        start_burst,
        clk_out,
        dqs;
      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_LINE_CARD_FTDLL_DQS_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit ;
      }
      dqs = param_val->value.ulong_value ;
      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_LINE_CARD_FTDLL_START_BURST_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit ;
      }
      start_burst = param_val->value.ulong_value ;
      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_LINE_CARD_FTDLL_CLK_OUT_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit ;
      }
      clk_out = param_val->value.ulong_value ;

      /*
       */
      sal_sprintf(
        err_msg,
        "-->>%s\n\r"
        "    line_card_pp_set_ftdll(start_burst-%lu, clk_out-%lu, dqs-%lu)",
        line_card_dev_type_to_string(dev_type),
        start_burst, clk_out, dqs
      );
      send_string_to_screen(err_msg,TRUE);

      /*
       */
      err = line_card_pp_set_ftdll(
              start_burst, clk_out, dqs
            );
      if (err)
      {
        sal_sprintf(
          err_msg,
          "*** line_card_pp_set_ftdll retuned with error\n\r"
        );
        send_string_to_screen(err_msg,TRUE);
        goto exit;
      }

    }
    else if ((!search_param_val_pairs(current_line,&match_index,PARAM_LINE_CARD_PP_LOAD_C2C_DFCDL_ID,1)))
    {
      unsigned long
        wide,
        narrow,
        pp_dev_num;
      BOOL
        auto_poll;
      /*
       */
      auto_poll = FALSE;
      pp_dev_num = dune_rpc_get_remote_pp_id(0);
      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_LINE_CARD_C2C_DFCDL_NARROW_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit ;
      }
      narrow = param_val->value.ulong_value ;
      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_LINE_CARD_C2C_DFCDL_WIDE_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit ;
      }
      wide = param_val->value.ulong_value ;
      /*
       */
      if (!search_param_val_pairs(current_line,&match_index,PARAM_LINE_CARD_DFCDL_AUTO_POLL_ID,1))
      {
        auto_poll = TRUE;
      }
      else
      {
        auto_poll = FALSE;
      }

      /*
       */
      sal_sprintf(
        err_msg,
        "-->>%s\n\r"
        "    line_card_pp_load_c2c_dfcdl_auto_poll(wide-%lu, narrow-%lu, auto_poll-%u)",
        line_card_dev_type_to_string(dev_type),
        wide, narrow, auto_poll
      );
      send_string_to_screen(err_msg,TRUE);

      /*
       */
      err = line_card_pp_load_c2c_dfcdl_auto_poll(
              pp_dev_num, wide, narrow, auto_poll
            );
      if (err)
      {
        sal_sprintf(
          err_msg,
          "*** line_card_pp_load_c2c_dfcdl_auto_poll retuned with error\n\r"
        );
        send_string_to_screen(err_msg,TRUE);
        goto exit;
      }

    }
    else if ((!search_param_val_pairs(current_line,&match_index,PARAM_LINE_CARD_PP_CHECK_IDDR_ID,1)))
    {
      BOOL
        test_pass;
      /*
       */
      err = line_card_pp_iddr_check(
              &test_pass
            );
      if (err)
      {
        sal_sprintf(
          err_msg,
          "*** line_card_pp_iddr_check retuned with error\n\r"
        );
        send_string_to_screen(err_msg,TRUE);
        goto exit;
      }

    }
    else if ((!search_param_val_pairs(current_line,&match_index,PARAM_LINE_CARD_C2C_RESET_ID,1)))
    {
      /*
       * RESET c2c.
       */
      C2C_INTERFACE
        c2c_interface;

      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_LINE_CARD_C2C_RESET_ID,1,
              &param_val,VAL_SYMBOL,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit ;
      }
      c2c_interface = (C2C_INTERFACE)param_val->numeric_equivalent ;
      /*
       */
      sal_sprintf(
        err_msg,
        "-->> %s\n\r"
        "     c2c_interface=%u\n\r",
        line_card_dev_type_to_string(dev_type),
        c2c_interface
      );
      send_string_to_screen(err_msg,TRUE);
      /*
       */
      err = line_card_reset_c2c(
              dev_type,
              c2c_interface
            );
      if (err)
      {
        sal_sprintf(
          err_msg,
          "*** line_card_reset_c2c retuned with error\n\r"
        );
        send_string_to_screen(err_msg,TRUE);
        goto exit;
      }

    }
    else if ((!search_param_val_pairs(current_line,&match_index,PARAM_RUN_BIST_TEST_ID,1)))
    {
      line_card_run_bist(0);
    }
    else if ((!search_param_val_pairs(current_line,&match_index,PARAM_RUN_BIST_TEST_1_ID,1)))
    {
      line_card_run_bist_1(0);
    }
    else if ((!search_param_val_pairs(current_line,&match_index,PARAM_LINE_CARD_CLOCK_SYNTHESIZERS_ID,1)))
    {
      LINE_CARD_FAP10M_PLL
        pll_type;

      /*
 */
      if (dev_type != LINE_CARD_FAP10M_1)
      {
        send_string_to_screen("-->Setting the clock is only valid for FAP10M_1", TRUE);
        ret=TRUE;
        goto exit;
      }
      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_LINE_CARD_CLOCK_SYNTHESIZERS_ID,1,
              &param_val,VAL_SYMBOL,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit ;
      }
      pll_type = (LINE_CARD_FAP10M_PLL) param_val->numeric_equivalent ;

      if ((!search_param_val_pairs(current_line,&match_index,PARAM_LINE_CARD_CLOCK_SYNTHESIZERS_VDW_ID,1)))
      {
        unsigned long
          vdw,
          rdw,
          od,
          exact_given_clock_in_mhz;
        /*
         */
        if (get_val_of(
                current_line,(int *)&match_index,PARAM_LINE_CARD_CLOCK_SYNTHESIZERS_VDW_ID,1,
                &param_val,VAL_NUMERIC,err_msg))
        {
          get_val_of_err = TRUE ;
          goto exit ;
        }
        vdw = param_val->value.ulong_value ;
        /*
         */
        if (get_val_of(
                current_line,(int *)&match_index,PARAM_LINE_CARD_CLOCK_SYNTHESIZERS_RDW_ID,1,
                &param_val,VAL_NUMERIC,err_msg))
        {
          get_val_of_err = TRUE ;
          goto exit ;
        }
        rdw = param_val->value.ulong_value ;
        /*
         */
        if (get_val_of(
                current_line,(int *)&match_index,PARAM_LINE_CARD_CLOCK_SYNTHESIZERS_OD_ID,1,
                &param_val,VAL_NUMERIC,err_msg))
        {
          get_val_of_err = TRUE ;
          goto exit ;
        }
        od = param_val->value.ulong_value ;
        /*
         */
        ret = line_card_clock_synthesizers(
                pll_type,
                vdw,
                rdw,
                od,
                &exact_given_clock_in_mhz
              );



        if (ret)
        {
          sal_sprintf(
            err_msg,
            "\n\r-->> ERROR\n\r"
                "     line_card_clock_synthesizers(%u, %lu, %lu, %lu)-- failed.",
            pll_type,
            vdw,
            rdw,
            od
          );
          ret = ERR_011 ;
          send_string_to_screen(err_msg, TRUE);
        }
        else
        {
          sal_sprintf(
            err_msg,
            "\n\r-->> line_card_clock_synthesizers(%u, %lu, %lu, %lu)-- Success.\n\r"
                "     exact_given_clock_in_mhz-%lu MHZ\n\r",
            pll_type,
            vdw,
            rdw,
            od,
            exact_given_clock_in_mhz
          );
          send_string_to_screen(err_msg, TRUE);
        }
        goto exit;

      }
      else if ((!search_param_val_pairs(current_line,&match_index,PARAM_LINE_CARD_CLOCK_SYNTHESIZER_OSCILLATOR_ID,1)))
      {
        LINE_CARD_FAP10M_PLL_SOURCE
          pll_source;
        /*
         */
        if (get_val_of(
                current_line,(int *)&match_index,PARAM_LINE_CARD_CLOCK_SYNTHESIZER_OSCILLATOR_ID,1,
                &param_val,VAL_SYMBOL,err_msg))
        {
          get_val_of_err = TRUE ;
          goto exit ;
        }
        pll_source = (LINE_CARD_FAP10M_PLL_SOURCE) param_val->numeric_equivalent ;

        ret = line_card_clock_synthesizers_set_source(
                pll_type,
                pll_source
              );

        if (ret)
        {
          sal_sprintf(
            err_msg,
            "\n\r-->> ERROR\n\r"
                "     line_card_clock_synthesizers_set_source(%u, %u)-- failed.",
            pll_type,
            pll_source
          );
          ret = ERR_012 ;
          send_string_to_screen(err_msg, TRUE);
        }
        else
        {
          sal_sprintf(
            err_msg,
            "\n\r-->> PLL source was set to %s.\n\r",
            (pll_source==LINE_CARD_OSCILLATOR? "OSCILLATOR":"SYNTHESIZER")
          );
          send_string_to_screen(err_msg, TRUE);
        }
        goto exit;
      }
      else if ((!search_param_val_pairs(current_line,&match_index,PARAM_LINE_CARD_CLOCK_CONFIGURE_WITH_FREQ_ID,1)))
      {
        unsigned long
          mhz_freq,
          add_half;
        /*
         */
        if (get_val_of(
                current_line,(int *)&match_index,PARAM_LINE_CARD_CLOCK_CONFIGURE_WITH_FREQ_ID,1,
                &param_val,VAL_NUMERIC,err_msg))
        {
          get_val_of_err = TRUE ;
          goto exit ;
        }
        mhz_freq = param_val->value.ulong_value ;
        /*
         */
        if (!search_param_val_pairs(current_line,&match_index,PARAM_LINE_CARD_CLOCK_ADD_HALF_ID,1))
        {
          add_half = TRUE;
        }
        else
        {
          add_half = FALSE;
        }

        ret = line_card_clock_synthesizers_set(
                pll_type,
                mhz_freq,
                add_half
             );
        if (ret)
        {
          sal_sprintf(
            err_msg,
            "\n\r-->> ERROR\n\r"
                "     line_card_clock_synthesizers_set(%u, %lu.%d)-- failed.",
            pll_type,
            mhz_freq,
            (add_half)?5:0
          );
          ret = ERR_011 ;
          send_string_to_screen(err_msg, TRUE);
        }
        else
        {
          sal_sprintf(
            err_msg,
            "\n\r-->> line_card_clock_synthesizers_set(%u, %lu.%d)-- Success.\n\r",
            pll_type,
            mhz_freq,
            (add_half)?5:0
          );
          send_string_to_screen(err_msg, TRUE);
        }
        goto exit;
      }
      else if ((!search_param_val_pairs(current_line,&match_index,PARAM_LINE_CARD_CLOCK_GET_ID,1)))
      {
        unsigned long
          mhz_freq;
        unsigned int
          add_half;
        /*
         */
        line_card_clock_synthesizers_get(
          pll_type,
          &mhz_freq,
          &add_half
        );

        {
          sal_sprintf(
            err_msg,
            "\n\r-->> line_card_clock_synthesizers_get(%u, %lu.%d)-- Success.\n\r",
            pll_type,
            mhz_freq,
            (add_half)?5:0
          );
          send_string_to_screen(err_msg, TRUE);
        }
        goto exit;
      }
    }
    else if ((!search_param_val_pairs(current_line,&match_index,PARAM_IDDR_PAD_CALIBRATION_ID,1)))
    {
      FAP10M_DDR_PADS
        iddr_pad;
      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_PAD_CALIBRATION_DATA_P_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit ;
      }
      iddr_pad.data_p = param_val->value.ulong_value ;
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_PAD_CALIBRATION_DATA_N_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit ;
      }
      iddr_pad.data_n = param_val->value.ulong_value ;
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_PAD_CALIBRATION_ADDR_P_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit ;
      }
      iddr_pad.addr_p = param_val->value.ulong_value ;
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_PAD_CALIBRATION_ADDR_N_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit ;
      }
      iddr_pad.addr_n = param_val->value.ulong_value ;

      fap10m_set_ddr_pads(0,FAP10M_DDR_IDDR,&iddr_pad,FALSE);
      goto exit;
    }
    else if ((!search_param_val_pairs(current_line,&match_index,PARAM_EDDR_PAD_CALIBRATION_ID,1)))
    {
      FAP10M_DDR_PADS
        eddr_pad;
      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_PAD_CALIBRATION_DATA_P_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit ;
      }
      eddr_pad.data_p = param_val->value.ulong_value ;
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_PAD_CALIBRATION_DATA_N_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit ;
      }
      eddr_pad.data_n = param_val->value.ulong_value ;
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_PAD_CALIBRATION_ADDR_P_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit ;
      }
      eddr_pad.addr_p = param_val->value.ulong_value ;
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_PAD_CALIBRATION_ADDR_N_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit ;
      }
      eddr_pad.addr_n = param_val->value.ulong_value ;

      fap10m_set_ddr_pads(0,FAP10M_DDR_EDDR,&eddr_pad,FALSE);
      goto exit;
    }
    else if ((!search_param_val_pairs(current_line,&match_index,PARAM_C2C_PAD_CALIBRATION_ID,1)))
    {
      FAP10M_C2C_PADS
        c2c_pad;
      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_PAD_CALIBRATION_DATA_P_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit ;
      }
      c2c_pad.data_p = param_val->value.ulong_value ;
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_PAD_CALIBRATION_DATA_N_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit ;
      }
      c2c_pad.data_n = param_val->value.ulong_value ;

      fap10m_set_c2c_pads(0,&c2c_pad,FALSE);
      goto exit;
    }
    else if ((!search_param_val_pairs(current_line,&match_index,PARAM_IO_PAD_CALIBRATION_GET_ID,1)))
    {
      FAP10M_C2C_PADS
        c2c_pad;
      FAP10M_DDR_PADS
        iddr_pad,
        eddr_pad;

      fap10m_get_c2c_pads(0, &c2c_pad);
      fap10m_get_ddr_pads(0, FAP10M_DDR_IDDR, &iddr_pad);
      fap10m_get_ddr_pads(0, FAP10M_DDR_EDDR, &eddr_pad);

      d_printf(
        "c2c_pad: data_p = %d, data_n = %d\n\r",
        c2c_pad.data_p,
        c2c_pad.data_n
      );
      d_printf(
        "iddr_pad: data_p = %d, data_n = %d, addr_p = %d, addr_n = %d\n\r",
        iddr_pad.data_p,
        iddr_pad.data_n,
        iddr_pad.addr_p,
        iddr_pad.addr_n
      );
      d_printf(
        "eddr_pad: data_p = %d, data_n = %d, addr_p = %d, addr_n = %d\n\r",
        eddr_pad.data_p,
        eddr_pad.data_n,
        eddr_pad.addr_p,
        eddr_pad.addr_n
      );
      goto exit;
    }
    else
    {
      /*
       * No valid operation variable has been entered.
       */
      send_string_to_screen(
          "\r\n"
          "*** 97. Should not reach here.\r\n",
          FALSE) ;
      ret = ERR_014 ;
      goto exit ;
    }

  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_LINE_CARD_UTILS_ID,1))
  {
    /*
     * Enter for line starting with "line_card utilities"
     */
    if (!search_param_val_pairs(current_line,&match_index,PARAM_LINE_CARD_DISP_ERR_COUNTERS_ID,1))
    {
      /*
       * Enter for line starting with "line_card utilities display_uplink_err_counters"
       */
      unsigned
        int
          clear_after_read ;
      if (!search_param_val_pairs(current_line,&match_index,PARAM_LINE_CARD_CLEAR_AFTER_TREAD_ID,1))
      {
        /*
         * Enter for line starting with
         * "line_card utilities display_uplink_err_counters clear_after_read"
         */
        clear_after_read = 1 ;
      }
      else
      {
        /*
         * Enter for line starting with
         * "line_card utilities display_uplink_err_counters"
         * (NO  clear_after_read).
         */
        clear_after_read = 0 ;
      }
      display_uplink_err_counters() ;
      if (clear_after_read)
      {
        assert_clear_counters_flag() ;
      }
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_LINE_CARD_PACKET_INJECT_TEST_ID,1))
    {
      /*
       * Enter for line starting with "line_card utilities packet_inject_test"
       */
      unsigned
        int
          error_indication,
          manual_break,
          trace_print,
          num_packets,
          action ;
      unsigned
        long
          pattern[PATTERN_SIZE/4] ;
      unsigned
        int
          ui,
          num_pattern_items,
          packet_size,
          pattern_type ;
      char
        *result_text,
        *trace_print_text ;
      static
        unsigned
          int
            Test_stopped = TRUE ;
      unsigned
        int
          cycles_actually_done ;

      trace_print = 0;
      num_packets = 0;


      if (!search_param_val_pairs(
                  current_line,&match_index,PARAM_LINE_CARD_DISPLAY_ALL_COUNTERS_ID,1))
      {
        action = ACTION_DISPLAY_ALL_COUNTERS ;
      }
      else if (!search_param_val_pairs(
                  current_line,&match_index,PARAM_LINE_CARD_PACKET_INJECT_START_ID,1))
      {
        action = ACTION_START ;
      }
      else
      {
        action = ACTION_START ;
      }
      if (action == ACTION_START)
      {
        if (!search_param_val_pairs(
                    current_line,&match_index,PARAM_LINE_CARD_REPEATED_PATTERN_ID,1))
        {
          pattern_type = REPEATED_PATTERN ;
        }
        else if (!search_param_val_pairs(
                    current_line,&match_index,PARAM_LINE_CARD_RANDOM_PATTERN_ID,1))
        {
          pattern_type = RANDOM_PATTERN ;
        }
        else
        {
          pattern_type = REPEATED_PATTERN ;
        }
        if (pattern_type == RANDOM_PATTERN)
        {
          /*
           * Not even one value of write data could not be retrieved.
           */
          sal_sprintf(err_msg,
              "\r\n\n"
              "*** RANDOM_PATTERN option has not yet been implemented!"
              ) ;
          send_string_to_screen(err_msg,TRUE) ;
          ret = ERR_023 ;
          goto exit ;
        }
        if (pattern_type == REPEATED_PATTERN)
        {
          unsigned
            long
              default_pattern ;
          /*
           * Value to fill into unspecified pattern data items.
           */
          default_pattern = 0xFF00AA55 ;
          /*
           * Get data for patter. There must be at least one data item.
           */
          num_pattern_items = (sizeof(pattern)/sizeof(pattern[0])) ;
          /*
           * The following 'for' MUST make at least one loop!
           */
          for (ui = 1 ; ui <= (sizeof(pattern)/sizeof(pattern[0])) ; ui++)
          {
            if (get_val_of(
                  current_line,(int *)&match_index,PARAM_LINE_CARD_PATTERN_DATA_ID,ui,
                  &param_val,VAL_NUMERIC,err_msg))
            {
              /*
               * If there are no more data values then quit loop.
               */
              num_pattern_items = ui - 1 ;
              break ;
            }
            pattern[ui - 1] = param_val->value.ulong_value ;
          }
          if (num_pattern_items == 0)
          {
            /*
             * Not even one value of write data could not be retrieved.
             */
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** Not even one value of pattern data could not be retrieved!"
                ) ;
            send_string_to_screen(err_msg,TRUE) ;
            ret = ERR_021 ;
            goto exit ;
          }
          /*
           * Just precaution...
           */
          if (num_pattern_items > (sizeof(pattern)/sizeof(pattern[0])))
          {
            /*
             * Too many write data values have been retrieved.
             */
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** Too many values of write data (%lu) have been retrieved!",
                (unsigned long)num_pattern_items
            ) ;
            send_string_to_screen(err_msg,TRUE) ;
            ret = ERR_022 ;
            goto exit ;
          }
          /*
           * Fill the rest with default_pattern.
           */
          for (ui = num_pattern_items ;
                    ui < (sizeof(pattern)/sizeof(pattern[0])) ; ui++)
          {
            pattern[ui] = default_pattern ;
          }
        }
        if (get_val_of(
                current_line,(int *)&match_index,PARAM_LINE_CARD_PACKET_SIZE_ID,1,
                &param_val,VAL_NUMERIC,err_msg))
        {
          get_val_of_err = TRUE ;
          goto exit ;
        }
        packet_size = param_val->value.ulong_value ;
        if (get_val_of(
                current_line,(int *)&match_index,PARAM_LINE_CARD_PACKET_INJECT_NUM_PKTS_ID,1,
                &param_val,VAL_SYMBOL | VAL_NUMERIC,err_msg))
        {
          get_val_of_err = TRUE ;
          goto exit ;
        }
        switch (param_val->val_type)
        {
          case VAL_NUMERIC:
          {
            num_packets = param_val->value.ulong_value ;
            break ;
          }
          default:
          case VAL_SYMBOL:
          {
            num_packets = param_val->numeric_equivalent ;
            break ;
          }
        }
        if (num_packets == NUM_PACKETS_INFINITE)
        {
          num_packets = (unsigned long)(-1) ;
        }
        if (get_val_of(
                current_line,(int *)&match_index,PARAM_LINE_CARD_TRACE_PRINT_ID,1,
                &param_val,VAL_SYMBOL,err_msg))
        {
          get_val_of_err = TRUE ;
          goto exit ;
        }
        trace_print = param_val->numeric_equivalent ;
        if (trace_print == ON_EQUIVALENT)
        {
          trace_print = TRUE ;
          trace_print_text = "ON" ;
        }
        else
        {
          trace_print = FALSE ;
          trace_print_text = "OFF" ;
        }
      }
      if (action == ACTION_START)
      {
        if (!Test_stopped)
        {
          sal_sprintf(err_msg,
                "=== Test is already active. Stop test first...\n\r"
                );
          send_string_to_screen(err_msg, FALSE);
          goto exit ;
        }
        Test_stopped = FALSE ;
        sal_sprintf(err_msg,"\r\n"
                "=== Start test. Send %u packets. Trace print is %s\n\r"
                "    To stop test manually, hit CTRL-c\n\r",
                num_packets,trace_print_text
                );
        send_string_to_screen(err_msg, FALSE);
        err =
          line_card_packet_inject_test(
            action,num_packets,trace_print,
            &cycles_actually_done,&manual_break,&error_indication,
            pattern,pattern_type,packet_size) ;
        /*
         * For now, assume test has been stopped if control returned here.
         */
        Test_stopped = TRUE ;
        if (err)
        {
          sal_sprintf(err_msg,
                "*** ERROR: line_card_packet_inject_test() failed (code %d).\n\r",
                err
                );
          send_string_to_screen(err_msg, FALSE);
          ret = ERR_015 ;
          goto exit ;
        }
        if (manual_break)
        {
          result_text = "Test has been MANUALLY stopped!!!\r\n" ;
        }
        else
        {
          result_text = "" ;
        }
        sal_sprintf(err_msg,"\r\n"
                "=== Test stopped or completed.\n\r"
                "    Required to send: %u packets ; Actually sent %u packets.\n\r"
                "    %s",
                num_packets,cycles_actually_done,result_text
                ) ;
        send_string_to_screen(err_msg, FALSE) ;
        if (error_indication == NO_FAILURE)
        {
          result_text = "SUCCEEDED" ;
        }
        else
        {
          result_text = "FAILED" ;
        }
        sal_sprintf(err_msg,"\r\n"
                "=== Test has %s!!! Code: %d\r\n",
                result_text,error_indication
                ) ;
        send_string_to_screen(err_msg, FALSE);
      }
      else if (action == ACTION_DISPLAY_ALL_COUNTERS)
      {
        if (!Test_stopped)
        {
          sal_sprintf(err_msg,
                "=== Test is still running while counters are requested...\n\r\a"
                );
          send_string_to_screen(err_msg, FALSE);
        }
        err =
          line_card_packet_inject_test(
            action,num_packets,trace_print,&cycles_actually_done,
            &manual_break,&error_indication,0,0,0) ;
        if (err)
        {
          sal_sprintf(err_msg,
                "*** ERROR: line_card_packet_inject_test() failed (code %d).\n\r",
                err
                );
          send_string_to_screen(err_msg, FALSE);
          ret = ERR_020 ;
          goto exit ;
        }
      }
      else
      {
        /*
         * No valid operation variable has been entered.
         */
        send_string_to_screen(
            "\r\n"
            "*** 81. No valid action identifier. Should not reach here.\r\n",
            FALSE) ;
        ret = ERR_016 ;
        goto exit ;
      }
    }
    else
    {
      /*
       * No valid operation variable has been entered.
       */
      send_string_to_screen(
          "\r\n"
          "*** 80. Should not reach here.\r\n",
          FALSE) ;
      ret = ERR_017 ;
      goto exit ;
    }
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_LINE_CARD_ENABLE_JUMBO_ID,1))
  {
    unsigned int
      enable_jumbo_packets;
    PARAM_VAL
      *enable_jumbo_val ;
    if (get_val_of(
            current_line,(int *)&match_index,PARAM_LINE_CARD_ENABLE_JUMBO_ID,1,
            &enable_jumbo_val,VAL_SYMBOL,err_msg))
    {
      /*
       * Value of format could not be retrieved.
       */
      ret = ERR_024 ;
      goto exit ;
    }
    enable_jumbo_packets = (unsigned long)enable_jumbo_val->numeric_equivalent ;

    sal_sprintf(err_msg,
          "=== Jumbo packets: %s\n\r"
          "    Note: This setting must be done before any traffic was\r\n"
          "     streamed between the FAP and the PP!!!.",
          enable_jumbo_packets?"enabled":"disabled"
    );
    send_string_to_screen(err_msg, FALSE);

    err =
      line_card_enable_jumbo_packets(
        enable_jumbo_packets
      );
    if (err)
    {
      sal_sprintf(err_msg,
            "*** ERROR: line_card_enable_jumbo_packets() failed (code %d).\n\r",
            err
            );
      send_string_to_screen(err_msg, FALSE);
      ret = ERR_020 ;
      goto exit ;
    }

  }
  else
  {
    /*
     * No valid LINE_CARD variable has been entered.
     * There must be at least one!
     */
    send_string_to_screen(
        "\r\n"
        "*** No valid parameter has been indicated for handling.\r\n"
        "    At least one must be entered.\r\n",
        FALSE) ;
    ret = ERR_018 ;
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

#endif
