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
#include <appl/diag/dpp/utils_memory.h>

#ifdef SAND_LOW_LEVEL_SIMULATION
/* { */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
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
 * Dune chips include file.
 */
#include <appl/diag/dpp/dune_chips.h>
#include <bcm_app/dpp/../H/usrApp.h>
/* } */
#else
/* { */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <taskLib.h>
#include <errnoLib.h>
#include <usrLib.h>
#include <tickLib.h>
#include <ioLib.h>
#include <iosLib.h>
#include <logLib.h>
#include <pipeDrv.h>
#include <timers.h>
#include <sigLib.h>
#include <usrApp.h>
/*
 * This file is required to complete definitions
 * related to timers and clocks. For display/debug
 * purposes only.
 */
#include <private\timerLibP.h>
#include <shellLib.h>
#include <dbgLib.h>
#include <flash28.h>
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
 * Dune chips include file.
 */
#include <appl/diag/dpp/dune_chips.h>
/* } */
#endif

#include <FMC/fmc_common_agent.h>
#include <FMC/fmc_transport_layer_dcl.h>

#include <appl/diag/dpp/dune_rpc.h>
#include <appl/diag/dpp/utils_line_TEVB.h>
#include <appl/diag/dpp/utils_memory.h>

/*****************************************************
*NAME
*  subject_mem
*TYPE: PROC
*DATE: 19/FEB/2002
*FUNCTION:
*  Process input line which has a 'mem' subject.
*  Input line is assumed to have been checked and
*  found to be of right format.
*CALLING SEQUENCE:
*  subject_mem(current_line,current_line_ptr)
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
  subject_mem(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  )
{
  int
    ret ;
  PARAM_VAL
    *param_val ;
  char
    *block_name,
    err_msg[80*4] ;
  unsigned
    int
      get_val_of_err ;
  int
    status=0 ;
  ret = FALSE ;
  get_val_of_err = FALSE ;
  if (current_line->num_param_names == 0)
  {
    /*
     * Enter if there are no parameters on the line (just 'mem').
     */
    print_mem_banks_info() ;
  }
  else
  {
    /*
     * Enter if there are parameters on the line (not just 'mem').
     */
    unsigned
      int
        match_index ;
    if (!search_param_val_pairs(current_line,&match_index,PARAM_MEM_READ_ID,1))
    {
      /*
       * Enter if this is a 'mem read' request.
       */
      unsigned
        long
          system_tick_mic,
          read_address,
          pause_after_read_time ;
      unsigned
        int
          offset_flag,
          override_flag,
          type,
          disp,
          len,
          format,
          use_mem_buff,
          mem_buff_offset,
          silent,
          address_in_longs,
          nof_read_times,
          low_resolution_pause,
          read_number_i;
      unsigned
        short
          before_read_time,
          after_read_time;
      PARAM_VAL
        *format_param_val,
        *disp_param_val ;

      mem_buff_offset = 0;
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_MEM_NUM_OF_TIMES_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        /*
         * Value of read address could not be retrieved.
         */
        get_val_of_err = TRUE ;
        goto sume_exit ;
      }
      nof_read_times = param_val->value.ulong_value ;
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_MEM_PAUSE_AFTER_READ_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        /*
         * Value of read address could not be retrieved.
         */
        get_val_of_err = TRUE ;
        goto sume_exit ;
      }
      pause_after_read_time = param_val->value.ulong_value ;
      system_tick_mic = 1000000 / CLOCKS_PER_SEC ;
      if (pause_after_read_time > (2 * system_tick_mic))
      {
        low_resolution_pause = TRUE ;
      }
      else
      {
        low_resolution_pause = FALSE ;
      }
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_MEM_READ_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        /*
         * Value of read address could not be retrieved.
         */
        get_val_of_err = TRUE ;
        goto sume_exit ;
      }
      read_address = param_val->value.ulong_value ;

      /*
       * Prameter 'address_in_longs'
       */
      if (search_param_val_pairs(
              current_line,&match_index,PARAM_MEM_ADDRESS_IN_LONGS_ID,1) == TRUE)
      {
        /*
         * Parameter 'address_in_longs' is not on this line. Set value of
         * address_in_longs flag to 'false'.
         */
        address_in_longs = FALSE ;
      }
      else
      {
        /*
         * address_in_longs parameter has been detected.
         */
        address_in_longs = TRUE ;
      }

      if (address_in_longs)
      {
        /*
         * If address in long indicator appears,
         * when user asks for 0x100, we take it as 0x400.
         * All other calulation should be with the 0x400 - as the CPU works with.
         */
        read_address *= 0x4;
      }

      if (get_val_of(
              current_line,(int *)&match_index,PARAM_MEM_LEN_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        /*
         * Value of len could not be retrieved.
         */
        get_val_of_err = TRUE ;
        goto sume_exit ;
      }
      len = (unsigned int)param_val->value.ulong_value ;
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_MEM_FORMAT_ID,1,
              &format_param_val,VAL_SYMBOL,err_msg))
      {
        /*
         * Value of format could not be retrieved.
         */
        get_val_of_err = TRUE ;
        goto sume_exit ;
      }
      format = (unsigned int)format_param_val->numeric_equivalent ;
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_MEM_DISP_ID,1,
              &disp_param_val,VAL_SYMBOL,err_msg))
      {
        /*
         * Value of display format could not be retrieved.
         */
        get_val_of_err = TRUE ;
        goto sume_exit ;
      }
      disp = (unsigned int)disp_param_val->numeric_equivalent ;
      /*
       * If offset is specified then follow 'offset'
       * logic (not absolute address).
       */
      if (search_param_val_pairs(
              current_line,&match_index,PARAM_MEM_OFFSET_ID,1) == TRUE)
      {
        /*
         * Parameter 'offset' is not on this line. Set value of
         * offset flag to 'false'.
         */
        offset_flag = FALSE ;
      }
      else
      {
        /*
         * Offset parameter has been detected.
         */
        offset_flag = TRUE ;
      }
      /*
       * Prameter 'silent'
       */
      if (search_param_val_pairs(
              current_line,&match_index,PARAM_MEM_SILENT_ID,1) == TRUE)
      {
        /*
         * Parameter 'silent' is not on this line. Set value of
         * silent flag to 'false'.
         */
        silent = FALSE ;
      }
      else
      {
        /*
         * Silent parameter has been detected.
         */
        silent = TRUE ;
      }

      /*
       * If override is specified then follow 'override'
       * logic (no memory block limits check).
       */
      if (search_param_val_pairs(
              current_line,&match_index,PARAM_MEM_OVERRIDE_ID,1) == TRUE)
      {
        /*
         * Parameter 'override' is not on this line. Set value of
         * override flag to 'false'.
         */
        override_flag = FALSE ;
      }
      else
      {
        /*
         * Override parameter has been detected.
         */
        override_flag = TRUE ;
      }
      if (offset_flag)
      {
        MEMORY_BLOCK
          *mem_block ;
        char
          *start_address ;
        /*
         * If 'offset' has been specified (explicitly or implicitly)
         * then the value of the 'read' parameter is offset within
         * the indicated memory block.
         */
        /*
         * If 'offset' has been specified (explicitly or implicitly)
         * then 'type' must be there as well, specifying the
         * memory block to access.
         */
        if (get_val_of(
                current_line,(int *)&match_index,PARAM_MEM_TYPE_ID,1,
                &param_val,VAL_SYMBOL,err_msg))
        {
          /*
           * Value of memory block type could not be retrieved.
           */
          get_val_of_err = TRUE ;
          goto sume_exit ;
        }
        type = (unsigned int)param_val->numeric_equivalent ;
        mem_block = block_id_to_ptr((unsigned int)type) ;
        if (mem_block == (MEMORY_BLOCK *)0)
        {
          sal_sprintf(err_msg,
              "\r\n\n"
              "*** This memory block identifier (%u) is unknown within memory map.",
              (unsigned short)type) ;
          send_string_to_screen(err_msg,TRUE) ;
          ret = TRUE ;
          goto sume_exit ;
        }
        if (!override_flag)
        {
          if (read_address >= mem_block->block_size)
          {
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** This offset (%lu) is beyond the end address of\r\n"
                "    this block (%s).",
                (unsigned long)read_address,mem_block->block_name) ;
            send_string_to_screen(err_msg,TRUE) ;
            ret = TRUE ;
            goto sume_exit ;
          }
        }
        start_address = mem_block->start_address + read_address ;
        read_address = (unsigned long)start_address ;
      }
      {
        MEMORY_BLOCK
          *memory_block[2] ;
        char
          *address[2] ;
        address[0] = (char *)read_address ;
        memory_block[0] = is_ptr_in_blocks(address[0]) ;
        if (!override_flag)
        {
          if (memory_block[0] == (MEMORY_BLOCK *)0)
          {
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** This base memory address (0x%08lX) is not within memory map.",
                (unsigned long)address[0]) ;
            send_string_to_screen(err_msg,TRUE) ;
            ret = TRUE ;
            goto sume_exit ;
          }
        }
        address[1] = (char *)(read_address + (len*format) - 1) ;
        memory_block[1] = is_ptr_in_blocks(address[1]) ;
        if (!override_flag)
        {
          if (memory_block[1] == (MEMORY_BLOCK *)0)
          {
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** This top memory address (0x%08lX) is not within memory map.",
                (unsigned long)address[1]) ;
            send_string_to_screen(err_msg,TRUE) ;
            ret = TRUE ;
            goto sume_exit ;
          }
          if (memory_block[1] != memory_block[0])
          {
            sal_sprintf(err_msg,
              "\r\n\n"
              "*** This memory range (0x%08lX,0x%08lX) crosses from one memory\r\n"
              "    block into another.",
              (unsigned long)address[0],(unsigned long)address[1]) ;
            send_string_to_screen(err_msg,TRUE) ;
            ret = TRUE ;
            goto sume_exit ;
          }
          if (memory_block[0]->attributes & BYTE_ACCESS_ONLY)
          {
            /*
             * If this memory block allows byte access only, make sure
             * 'format' complies.
             */
            if (format != CHAR_EQUIVALENT)
            {
              sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** This memory block (%s) allows byte access only.",
                  memory_block[0]->block_name) ;
              send_string_to_screen(err_msg,TRUE) ;
              ret = TRUE ;
              goto sume_exit ;
            }
          }
        }
        if (memory_block[0] == (MEMORY_BLOCK *)0)
        {
          /*
           * This could be, at this point, if 'override' has been
           * activated.
           */
          block_name = "Any (via override)" ;
        }
        else
        {
          block_name = memory_block[0]->block_name ;
        }
        {
          /*
           * Prameter 'mem_buff_offset'
           */
          unsigned
            int
              mem_buff_len ;
          use_mem_buff = 0 ;
          if (search_param_val_pairs(
                  current_line,&match_index,PARAM_MEM_BUFF_OFFSET_ID,1) == FALSE)
          {
            /*
             * 'mem_buff_offset' parameter has been detected.
             */
            if (get_val_of(
                    current_line,(int *)&match_index,PARAM_MEM_BUFF_OFFSET_ID,1,
                    &param_val,VAL_NUMERIC,err_msg))
            {
              /*
               * Value of len could not be retrieved.
               */
              get_val_of_err = TRUE ;
              goto sume_exit ;
            }
            mem_buff_offset = (unsigned int)param_val->value.ulong_value ;
            if (format == CHAR_EQUIVALENT)
            {
              mem_buff_len = len ;
            }
            else if (format == SHORT_EQUIVALENT)
            {
              mem_buff_len = len * sizeof(short) ;
            }
            else if (format == LONG_EQUIVALENT)
            {
              mem_buff_len = len * sizeof(long) ;
            }
            else
            {
              mem_buff_len = len * sizeof(long) ;
            }
            if (is_buff_mem_offset_legit(mem_buff_offset,mem_buff_len))
            {
              sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** This mem buff offset (%u) and size (%u) do not fit into size\r\n"
                  "    of mem buff (%u).",
                  mem_buff_offset,mem_buff_len,get_buff_mem_size()) ;
              send_string_to_screen(err_msg,TRUE) ;
              ret = TRUE ;
              goto sume_exit ;
            }
            use_mem_buff = 1 ;
          }
        }
        if (!silent)
        {
          sal_sprintf(err_msg,
              "\r\n\n"
              "Memory block: \'%s\'. \'%s\' display. \'%s\' access.\r\n"
              "=======================================================================",
              block_name,
              disp_param_val->value.string_value,
              format_param_val->value.string_value) ;
          send_string_to_screen(err_msg,FALSE) ;
        }
      }

      for (read_number_i = 0; read_number_i < nof_read_times; ++read_number_i)
      {
        unsigned long
          mem_buff_offset_adder;
        before_read_time = read_timer_2();
        /*
         * The adding need to add to mem_buff_offset
         * in case we use 'use_mem_buff'.
         */
        mem_buff_offset_adder = read_number_i*len*format;
        if (nof_read_times > 1)
        {
            if (!silent)
            {
                if (read_number_i == 0)
                {
                    send_string_to_screen("",TRUE) ;
                }
                sal_sprintf(err_msg,
                        "Read number #%u, Pause after read %lu micro-sec.",
                        read_number_i+1,
                        pause_after_read_time);
                send_string_to_screen(err_msg,TRUE) ;
            }
        }
        /*
         * implementing remote operation for mem read
         */
        if(!search_param_val_pairs(current_line,&match_index,PARAM_MEM_REMOTE_OPERATION_ID,1))
        {
          unsigned int unit_to_send;
          OTHER_SERVICES_IN_STRUCT in_struct;
          unsigned char* recv_buff;
          /* get the unit numner */
          if (get_val_of( current_line,(int *)&match_index,PARAM_MEM_REMOTE_OPERATION_ID,1,
                          &param_val,VAL_NUMERIC,err_msg)
             )
          {
            get_val_of_err = TRUE;
            goto sume_exit;
          }
          unit_to_send = (unsigned int)param_val->value.ulong_value;
          /* fill the input params */
          in_struct.proc_id = 0; /*Read Operation*/
          in_struct.data.mem_read_data.addr = read_address ;
          in_struct.data.mem_read_data.len  = len ;
          in_struct.data.mem_read_data.format  = format ;
          recv_buff = NULL;
          recv_buff = (unsigned char*)malloc(len * format) ;
          if (NULL == recv_buff)
          {
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** This failed to allocate buffer for sendBuffToCore reply") ;
            send_string_to_screen(err_msg,TRUE) ;
            ret = TRUE;
            goto sume_exit;
          }
          status = fmc_send_buffer_to_device_relative_id(
#if INCLUDE_DUNE_RPC
            dune_rpc_get_mockup_unit_from_unit(unit_to_send),
#else
            0,
#endif
            sizeof(in_struct),
            (unsigned char *)&in_struct,
            len*format,
            recv_buff
          );
          if (0 != status)
          {
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** fmc_send_buffer_to_device_relative_id failed to send command to remote unit") ;
            send_string_to_screen(err_msg,TRUE) ;
            ret = TRUE;
            free (recv_buff);
            goto sume_exit;
          }
          display_memory(
              (char *)recv_buff,
              len,
              format,
              disp,
              silent,
              use_mem_buff,
              mem_buff_offset + mem_buff_offset_adder
          );
        }
        else
        {
          display_memory(
              (char *)read_address,
              len,
              format,
              disp,
              silent,
              use_mem_buff,
              mem_buff_offset + mem_buff_offset_adder
          ) ;
        }
        if (nof_read_times > 1)
        {
            /*
             * Wait 'pause_after_read_time' micro-sec.
             */
          unsigned short
            time_diff_short;
          unsigned long
            time_diff;
          after_read_time = read_timer_2();
          time_diff_short = after_read_time - before_read_time;
          time_diff = timer_2_to_micro(time_diff_short);
          while( time_diff < pause_after_read_time)
          {
            if (low_resolution_pause)
            {
              d_taskUnsafe() ;
              d_taskDelay(1) ;
              d_taskSafe() ;
            }
            after_read_time = read_timer_2();
            time_diff_short = after_read_time - before_read_time;
            time_diff = timer_2_to_micro(time_diff_short);
          }
        }
      }
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_MEM_WRITE_ID,1) ||
             !search_param_val_pairs(current_line,&match_index,PARAM_MEM_XOR_ID,1)   ||
             !search_param_val_pairs(current_line,&match_index,PARAM_MEM_AND_ID,1)   ||
             !search_param_val_pairs(current_line,&match_index,PARAM_MEM_OR_ID,1)
             )
    {
      /*
       * Enter if this is a 'mem write' request.
       */
      unsigned
        long
          write_data[MAX_REPEATED_PARAM_VAL],
          write_address ;
      unsigned
        int
          ui,
          num_to_write,
          offset_flag,
          override_flag,
          repeat_count,
          type,
          format,
          silent,
          address_in_longs,
          eeprom_access_flag ;
      PARAM_VAL
        *format_param_val ;
      int operation_on_data; /*0-non, 1-and, 2-or, 3-xor*/

      if(!search_param_val_pairs(current_line,&match_index,PARAM_MEM_XOR_ID,1))
      {
        operation_on_data = XOR_OP ;
      }
      else if(!search_param_val_pairs(current_line,&match_index,PARAM_MEM_OR_ID,1))
      {
        operation_on_data = OR_OP ;
      }
      else if(!search_param_val_pairs(current_line,&match_index,PARAM_MEM_AND_ID,1))
      {
        operation_on_data = AND_OP ;
      }
      else
      {
        operation_on_data = NO_OPERATION ;
      }

      if (get_val_of(
              current_line,(int *)&match_index,PARAM_MEM_WRITE_ID,1,
              &param_val,VAL_NUMERIC,err_msg) &&
          get_val_of(
              current_line,(int *)&match_index,PARAM_MEM_XOR_ID,1,
              &param_val,VAL_NUMERIC,err_msg) &&
          get_val_of(
              current_line,(int *)&match_index,PARAM_MEM_AND_ID,1,
              &param_val,VAL_NUMERIC,err_msg) &&
          get_val_of(
              current_line,(int *)&match_index,PARAM_MEM_OR_ID,1,
              &param_val,VAL_NUMERIC,err_msg)
          )
      {
        /*
         * Value of write address could not be retrieved.
         */
        get_val_of_err = TRUE ;
        goto sume_exit ;
      }
      write_address = param_val->value.ulong_value ;


      /*
       * Prameter 'address_in_longs'
       */
      if (search_param_val_pairs(
              current_line,&match_index,PARAM_MEM_ADDRESS_IN_LONGS_ID,1) == TRUE)
      {
        /*
         * Parameter 'address_in_longs' is not on this line. Set value of
         * address_in_longs flag to 'false'.
         */
        address_in_longs = FALSE ;
      }
      else
      {
        /*
         * address_in_longs parameter has been detected.
         */
        address_in_longs = TRUE ;
      }

      if (address_in_longs)
      {
        /*
         * If address in long indicator appears,
         * when user asks for 0x100, we take it as 0x400.
         * All other calulation should be with the 0x400 - as the CPU works with.
         */
        write_address *= 0x4;
      }



      if (get_val_of(
              current_line,(int *)&match_index,PARAM_MEM_FORMAT_ID,1,
              &format_param_val,VAL_SYMBOL,err_msg))
      {
        /*
         * Value of format could not be retrieved.
         */
        get_val_of_err = TRUE ;
        goto sume_exit ;
      }
      format = (unsigned int)format_param_val->numeric_equivalent ;
      /*
       * If offset is specified then follow 'offset'
       * logic (not absolute address).
       */
      if (search_param_val_pairs(
              current_line,&match_index,PARAM_MEM_OFFSET_ID,1) == TRUE)
      {
        /*
         * Parameter 'offset' is not on this line. Set value of
         * offset flag to 'false'.
         */
        offset_flag = FALSE ;
      }
      else
      {
        /*
         * Offset parameter has been detected.
         */
        offset_flag = TRUE ;
      }
      /*
       * Prameter 'silent'
       */
      if (search_param_val_pairs(
              current_line,&match_index,PARAM_MEM_SILENT_ID,1) == TRUE)
      {
        /*
         * Parameter 'silent' is not on this line. Set value of
         * silent flag to 'false'.
         */
        silent = FALSE ;
      }
      else
      {
        /*
         * Silent parameter has been detected.
         */
        silent = TRUE ;
      }

      /*
       * If override is specified then follow 'override'
       * logic (no memory block limits check).
       */
      if (search_param_val_pairs(
              current_line,&match_index,PARAM_MEM_OVERRIDE_ID,1) == TRUE)
      {
        /*
         * Parameter 'override' is not on this line. Set value of
         * override flag to 'false'.
         */
        override_flag = FALSE ;
      }
      else
      {
        /*
         * Override parameter has been detected.
         */
        override_flag = TRUE ;
      }
      if (offset_flag)
      {
        MEMORY_BLOCK
          *mem_block ;
        char
          *start_address ;
        /*
         * If 'offset' has been specified (explicitly or implicitly)
         * then the value of the 'read' parameter is offset within
         * the indicated memory block.
         */
        /*
         * If 'offset' has been specified (explicitly or implicitly)
         * then 'type' must be there as well, specifying the
         * memory block to access.
         */
        if (get_val_of(
                current_line,(int *)&match_index,PARAM_MEM_TYPE_ID,1,
                &param_val,VAL_SYMBOL,err_msg))
        {
          /*
           * Value of memory block type could not be retrieved.
           */
          get_val_of_err = TRUE ;
          goto sume_exit ;
        }
        type = (unsigned int)param_val->numeric_equivalent ;
        mem_block = block_id_to_ptr((unsigned int)type) ;
        if (mem_block == (MEMORY_BLOCK *)0)
        {
          sal_sprintf(err_msg,
              "\r\n\n"
              "*** This memory block identifier (%u) is unknown within memory map.",
              (unsigned short)type) ;
          send_string_to_screen(err_msg,TRUE) ;
          ret = TRUE ;
          goto sume_exit ;
        }
        if (!override_flag)
        {
          if (write_address >= mem_block->block_size)
          {
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** This offset (%lu) is beyond the end address of\r\n"
                "    this block (%s).",
                (unsigned long)write_address,mem_block->block_name) ;
            send_string_to_screen(err_msg,TRUE) ;
            ret = TRUE ;
            goto sume_exit ;
          }
        }
        start_address = mem_block->start_address + write_address ;
        write_address = (unsigned long)start_address ;
      }
      {
        /*
         * Get data to write. There must be at least one data item.
         */
        num_to_write = (sizeof(write_data)/sizeof(write_data[0])) ;
        /*
         * The following 'for' MUST make at least one loop!
         */
        for (ui = 1 ; ui <= (sizeof(write_data)/sizeof(write_data[0])) ; ui++)
        {
          if (get_val_of(
                current_line,(int *)&match_index,PARAM_MEM_DATA_ID,ui,
                &param_val,VAL_NUMERIC,err_msg))
          {
            /*
             * If there are no more data values then quit loop.
             */
            num_to_write = ui - 1 ;
            break ;
          }
          write_data[ui - 1] = param_val->value.ulong_value ;
        }
        if (num_to_write == 0)
        {
          /*
           * Not even one value of write data could not be retrieved.
           */
          sal_sprintf(err_msg,
              "\r\n\n"
              "*** Not even one value of write data could not be retrieved!"
              ) ;
          send_string_to_screen(err_msg,TRUE) ;
          ret = TRUE ;
          goto sume_exit ;
        }
        /*
         * Just precaution...
         */
        if (num_to_write > (sizeof(write_data)/sizeof(write_data[0])))
        {
          /*
           * Too many write data values have been retrieved.
           */
          sal_sprintf(err_msg,
              "\r\n\n"
              "*** Too many values of write data (%u) have been retrieved!",
              (unsigned int)num_to_write) ;
          send_string_to_screen(err_msg,TRUE) ;
          ret = TRUE ;
          goto sume_exit ;
        }
      }
      {
        /*
         * Get write repeat count.
         */
        if (get_val_of(
                current_line,(int *)&match_index,PARAM_MEM_REPEAT_ID,1,
                &param_val,VAL_NUMERIC,err_msg))
        {
          /*
           * Value of repeat count could not be retrieved.
           */
          get_val_of_err = TRUE ;
          goto sume_exit ;
        }
        repeat_count = (unsigned int)param_val->value.ulong_value ;
      }
      {
        MEMORY_BLOCK
          *memory_block[2] ;
        char
          *address[2] ;
        address[0] = (char *)write_address ;
        memory_block[0] = is_ptr_in_blocks(address[0]) ;
        if (!override_flag)
        {
          if (memory_block[0] == (MEMORY_BLOCK *)0)
          {
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** This base memory address (0x%08lX) is not within memory map.",
                (unsigned long)address[0]) ;
            send_string_to_screen(err_msg,TRUE) ;
            ret = TRUE ;
            goto sume_exit ;
          }
        }
        address[1] = (char *)(write_address + (repeat_count*num_to_write*format) - 1) ;
        memory_block[1] = is_ptr_in_blocks(address[1]) ;
        if (!override_flag)
        {
          /*
           * Enter if this is NOT the 'override' case
           */
          if (memory_block[1] == (MEMORY_BLOCK *)0)
          {
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** This top memory address (0x%08lX) is not within memory map.",
                (unsigned long)address[1]) ;
            send_string_to_screen(err_msg,TRUE) ;
            ret = TRUE ;
            goto sume_exit ;
          }
          if (memory_block[1] != memory_block[0])
          {
            sal_sprintf(err_msg,
              "\r\n\n"
              "*** This memory range (0x%08lX,0x%08lX) crosses from one memory\r\n"
              "    block into another.",
              (unsigned long)address[0],(unsigned long)address[1]) ;
            send_string_to_screen(err_msg,TRUE) ;
            ret = TRUE ;
            goto sume_exit ;
          }
          if (memory_block[0]->attributes & BLOCK_READ_ONLY)
          {
            /*
             * If this memory block allows read only, reject request.
             */
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** This memory block (%s) allows READ access only.",
                memory_block[0]->block_name) ;
            send_string_to_screen(err_msg,TRUE) ;
            ret = TRUE ;
            goto sume_exit ;
          }
          if (memory_block[0]->attributes & BYTE_ACCESS_ONLY)
          {
            /*
             * If this memory block allows byte access only, make sure
             * 'format' complies.
             */
            if (format != CHAR_EQUIVALENT)
            {
              sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** This memory block (%s) allows byte access only.",
                  memory_block[0]->block_name) ;
              send_string_to_screen(err_msg,TRUE) ;
              ret = TRUE ;
              goto sume_exit ;
            }
          }
          if (memory_block[0]->attributes & LONG_ALIGNED_WRITE_ONLY)
          {
            /*
             * If this memory block allows long and aaligned access
             * only, make sure 'format' and addresses comply.
             */
            if (format != LONG_EQUIVALENT)
            {
              sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** This memory block (%s) allows long access only.",
                  memory_block[0]->block_name) ;
              send_string_to_screen(err_msg,TRUE) ;
              ret = TRUE ;
              goto sume_exit ;
            }
            if ((unsigned long)address[0] & (BIT(0) | BIT(1)))
            {
              sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** This memory block (%s) allows long-aligned access only.",
                  memory_block[0]->block_name) ;
              send_string_to_screen(err_msg,TRUE) ;
              ret = TRUE ;
              goto sume_exit ;
            }
          }
          if (memory_block[0]->attributes & EEPROM_ACCESS)
          {
            eeprom_access_flag = TRUE ;
          }
          else
          {
            eeprom_access_flag = FALSE ;
          }
        }
        else
        {
          /*
           * Enter if this is the 'override' case
           */
          eeprom_access_flag = FALSE ;
          /*
           * In this case, unlock flash memory. Everything is allowed.
           */
          unlock_flash() ;
        }
        if (memory_block[0] == (MEMORY_BLOCK *)0)
        {
          /*
           * This could be, at this point, if 'override' has been
           * activated.
           */
          block_name = "Any (via override)" ;
        }
        else
        {
          block_name = memory_block[0]->block_name ;
        }
        if (!silent)
        {
          sal_sprintf(err_msg,
                "\r\n\n"
                "Memory block: \'%s\'. %u data items. \'%s\' access.\r\n"
                "=======================================================================",
                block_name,
                num_to_write*repeat_count,
                format_param_val->value.string_value) ;
          send_string_to_screen(err_msg,FALSE) ;
        }
      }
      if(!search_param_val_pairs(current_line,&match_index,PARAM_MEM_REMOTE_OPERATION_ID,1))
      {
        unsigned int             unit_to_send;
        unsigned long            out_struct;
        OTHER_SERVICES_IN_STRUCT in_struct;
        /* get the unit numner */
        if (get_val_of( current_line,(int *)&match_index,PARAM_MEM_REMOTE_OPERATION_ID,1,
                        &param_val,VAL_NUMERIC,err_msg)
           )
        {
          get_val_of_err = TRUE;
          goto sume_exit;
        }
        unit_to_send = (unsigned int)param_val->value.ulong_value;
        /* fill the input params */
        in_struct.proc_id = 1; /*Write Operation*/
        in_struct.data.mem_write_data.addr              = write_address;
        in_struct.data.mem_write_data.eeprom_access     = eeprom_access_flag;
        in_struct.data.mem_write_data.format            = format;
        in_struct.data.mem_write_data.len               = num_to_write;
        in_struct.data.mem_write_data.operation_on_data = operation_on_data;
        in_struct.data.mem_write_data.repeat_count      = repeat_count;
        for(ui = 0 ; ui < MAX_REPEATED_PARAM_VAL ; ui++)
        {
          in_struct.data.mem_write_data.write_data[ui] =  write_data[ui];
        }
        status = fmc_send_buffer_to_device_relative_id(
#if INCLUDE_DUNE_RPC
                      dune_rpc_get_mockup_unit_from_unit(unit_to_send),
#else
                      0,
#endif
                      sizeof(in_struct),
                      (unsigned char *)&in_struct,
                      sizeof(out_struct),
                      (unsigned char *)&out_struct
                    );

        if (0 != status)
        {
          sal_sprintf(err_msg,
              "\r\n\n"
              "*** fmc_send_buffer_to_device_relative_id failed to send command to remote unit") ;
          send_string_to_screen(err_msg,TRUE) ;
          ret = TRUE;
          goto sume_exit;
        }
      }
      else
      {
        for (ui = 0 ; ui < repeat_count ; ui++)
        {
          if (operation_on_data != NO_OPERATION)
          {
            write_memory_do_op(
              (char *)write_address,num_to_write,
              format,write_data,eeprom_access_flag,
              operation_on_data, silent) ;
              write_address += (num_to_write * format) ;
          }
          else
          {
            write_memory(
              (char *)write_address,num_to_write,
              format,write_data,eeprom_access_flag,
              silent) ;
              write_address += (num_to_write*format) ;
          }
        }
      }
      if (override_flag)
      {
        /*
         * Lock flash memory. It has been unlocked for the 'override'
         * case where everything is allowed.
         */
        lock_flash() ;
      }
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_MEM_MUL_WRITE_ID,1))
    {
      /*
       * Enter if this is a 'mem mul_write' request.
       */
      unsigned
        long
          write_data[MAX_REPEATED_PARAM_VAL],
          write_address[MAX_REPEATED_PARAM_VAL],
          multiply_fcator ;
      unsigned
        int
          ui,
          num_to_write,
          num_addresses,
          offset_flag,
          override_flag,
          type,
          format,
          silent,
          address_in_longs,
          eeprom_access_flag ;
      PARAM_VAL
        *format_param_val ;
      num_addresses = (sizeof(write_address)/sizeof(write_address[0])) ;

      /*
       * Prameter 'address_in_longs'
       */
      if (search_param_val_pairs(
              current_line,&match_index,PARAM_MEM_ADDRESS_IN_LONGS_ID,1) == TRUE)
      {
        /*
         * Parameter 'address_in_longs' is not on this line. Set value of
         * address_in_longs flag to 'false'.
         */
        address_in_longs = FALSE ;
      }
      else
      {
        /*
         * address_in_longs parameter has been detected.
         */
        address_in_longs = TRUE ;
      }

      if (address_in_longs)
      {
        /*
         * If address in long indicator appears,
         * when user asks for 0x100, we take it as 0x400.
         * All other calulation should be with the 0x400 - as the CPU works with.
         */
        multiply_fcator = 0x4;
      }
      else
      {
        multiply_fcator = 0x1;
      }

      /*
       * The following 'for' MUST make at least one loop!
       */
      for (ui = 1 ; ui <= (sizeof(write_address)/sizeof(write_address[0])) ; ui++)
      {
        if (get_val_of(
                current_line,(int *)&match_index,PARAM_MEM_MUL_WRITE_ID,ui,
                &param_val,VAL_NUMERIC,err_msg)
            )
        {
          /*
           * If there are no more address values then quit loop.
           */
          num_addresses = ui - 1 ;
          break ;
        }
        write_address[ui - 1] = multiply_fcator * param_val->value.ulong_value ;
      }
      if (num_addresses == 0)
      {
        /*
         * Not even one value of write address could not be retrieved.
         */
        sal_sprintf(err_msg,
            "\r\n\n"
            "*** Not even one value of write address could not be retrieved!"
            ) ;
        send_string_to_screen(err_msg,TRUE) ;
        ret = TRUE ;
        goto sume_exit ;
      }
      /*
       * Just precaution...
       */
      if (num_addresses > (sizeof(write_address)/sizeof(write_address[0])))
      {
        /*
         * Too many write data values have been retrieved.
         */
        sal_sprintf(err_msg,
            "\r\n\n"
            "*** Too many values of write addresses (%u) have been retrieved!",
            (unsigned int)num_addresses) ;
        send_string_to_screen(err_msg,TRUE) ;
        ret = TRUE ;
        goto sume_exit ;
      }
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_MEM_FORMAT_ID,1,
              &format_param_val,VAL_SYMBOL,err_msg))
      {
        /*
         * Value of format could not be retrieved.
         */
        get_val_of_err = TRUE ;
        goto sume_exit ;
      }
      format = (unsigned int)format_param_val->numeric_equivalent ;
      /*
       * If offset is specified then follow 'offset'
       * logic (not absolute address).
       */
      if (search_param_val_pairs(
              current_line,&match_index,PARAM_MEM_OFFSET_ID,1) == TRUE)
      {
        /*
         * Parameter 'offset' is not on this line. Set value of
         * offset flag to 'false'.
         */
        offset_flag = FALSE ;
      }
      else
      {
        /*
         * Offset parameter has been detected.
         */
        offset_flag = TRUE ;
      }
      /*
       * Prameter 'silent'
       */
      if (search_param_val_pairs(
              current_line,&match_index,PARAM_MEM_SILENT_ID,1) == TRUE)
      {
        /*
         * Parameter 'silent' is not on this line. Set value of
         * silent flag to 'false'.
         */
        silent = FALSE ;
      }
      else
      {
        /*
         * Silent parameter has been detected.
         */
        silent = TRUE ;
      }


      /*
       * If override is specified then follow 'override'
       * logic (no memory block limits check).
       */
      if (search_param_val_pairs(
              current_line,&match_index,PARAM_MEM_OVERRIDE_ID,1) == TRUE)
      {
        /*
         * Parameter 'override' is not on this line. Set value of
         * override flag to 'false'.
         */
        override_flag = FALSE ;
      }
      else
      {
        /*
         * Override parameter has been detected.
         */
        override_flag = TRUE ;
      }
      if (offset_flag)
      {
        MEMORY_BLOCK
          *mem_block ;
        char
          *start_address ;
        /*
         * If 'offset' has been specified (explicitly or implicitly)
         * then the value of the 'read' parameter is offset within
         * the indicated memory block.
         */
        /*
         * If 'offset' has been specified (explicitly or implicitly)
         * then 'type' must be there as well, specifying the
         * memory block to access.
         */
        if (get_val_of(
                current_line,(int *)&match_index,PARAM_MEM_TYPE_ID,1,
                &param_val,VAL_SYMBOL,err_msg))
        {
          /*
           * Value of memory block type could not be retrieved.
           */
          get_val_of_err = TRUE ;
          goto sume_exit ;
        }
        type = (unsigned int)param_val->numeric_equivalent ;
        mem_block = block_id_to_ptr((unsigned int)type) ;
        if (mem_block == (MEMORY_BLOCK *)0)
        {
          sal_sprintf(err_msg,
              "\r\n\n"
              "*** This memory block identifier (%u) is unknown within memory map.",
              (unsigned short)type) ;
          send_string_to_screen(err_msg,TRUE) ;
          ret = TRUE ;
          goto sume_exit ;
        }
        for (ui = 0 ; ui < num_addresses ; ui++)
        {
          if (!override_flag)
          {
            if (write_address[ui] >= mem_block->block_size)
            {
              sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** This offset (%lu) is beyond the end address of\r\n"
                  "    this block (%s).",
                  (unsigned long)write_address[ui],mem_block->block_name) ;
              send_string_to_screen(err_msg,TRUE) ;
              ret = TRUE ;
              goto sume_exit ;
            }
          }
          start_address = mem_block->start_address + write_address[ui] ;
          write_address[ui] = (unsigned long)start_address ;
        }
      }
      {
        /*
         * Get data to write. There must be at least one data item.
         */
        num_to_write = (sizeof(write_data)/sizeof(write_data[0])) ;
        /*
         * The following 'for' MUST make at least one loop!
         */
        for (ui = 1 ; ui <= (sizeof(write_data)/sizeof(write_data[0])) ; ui++)
        {
          if (get_val_of(
                current_line,(int *)&match_index,PARAM_MEM_MUL_DATA_ID,ui,
                &param_val,VAL_NUMERIC,err_msg))
          {
            /*
             * If there are no more data values then quit loop.
             */
            num_to_write = ui - 1 ;
            break ;
          }
          write_data[ui - 1] = param_val->value.ulong_value ;
        }
        if (num_to_write == 0)
        {
          /*
           * Not even one value of write data could not be retrieved.
           */
          sal_sprintf(err_msg,
              "\r\n\n"
              "*** Not even one value of write data could not be retrieved!"
              ) ;
          send_string_to_screen(err_msg,TRUE) ;
          ret = TRUE ;
          goto sume_exit ;
        }
        /*
         * Just precaution...
         */
        if (num_to_write > (sizeof(write_data)/sizeof(write_data[0])))
        {
          /*
           * Too many write data values have been retrieved.
           */
          sal_sprintf(err_msg,
              "\r\n\n"
              "*** Too many values of write data (%u) have been retrieved!",
              (unsigned int)num_to_write) ;
          send_string_to_screen(err_msg,TRUE) ;
          ret = TRUE ;
          goto sume_exit ;
        }
      }
      {
        /*
         * Note: Number of addresses must match number of data items:
         * Exactly ONE data item is used with ONE address.
         */
        if (num_to_write != num_addresses)
        {
          sal_sprintf(err_msg,
              "\r\n\n"
              "*** Number of addresses (%u) is not equal to number of data items (%u).",
              (unsigned short)num_addresses,(unsigned short)num_to_write) ;
          send_string_to_screen(err_msg,TRUE) ;
          ret = TRUE ;
          goto sume_exit ;
        }
      }
      for (ui = 0 ; ui < num_addresses ; ui++)
      {
        MEMORY_BLOCK
          *memory_block[2] ;
        char
          *address[2] ;
        address[0] = (char *)write_address[ui] ;
        memory_block[0] = is_ptr_in_blocks(address[0]) ;
        if (!override_flag)
        {
          if (memory_block[0] == (MEMORY_BLOCK *)0)
          {
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** This base memory address (0x%08lX) is not within memory map.",
                (unsigned long)address[0]) ;
            send_string_to_screen(err_msg,TRUE) ;
            ret = TRUE ;
            goto sume_exit ;
          }
        }
        address[1] = (char *)(write_address[ui] + format - 1) ;
        memory_block[1] = is_ptr_in_blocks(address[1]) ;
        if (!override_flag)
        {
          /*
           * Enter if this is NOT the 'override' case
           */
          if (memory_block[1] == (MEMORY_BLOCK *)0)
          {
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** This top memory address (0x%08lX) is not within memory map.",
                (unsigned long)address[1]) ;
            send_string_to_screen(err_msg,TRUE) ;
            ret = TRUE ;
            goto sume_exit ;
          }
          if (memory_block[1] != memory_block[0])
          {
            sal_sprintf(err_msg,
              "\r\n\n"
              "*** This memory range (0x%08lX,0x%08lX) crosses from one memory\r\n"
              "    block into another.",
              (unsigned long)address[0],(unsigned long)address[1]) ;
            send_string_to_screen(err_msg,TRUE) ;
            ret = TRUE ;
            goto sume_exit ;
          }
          if (memory_block[0]->attributes & BLOCK_READ_ONLY)
          {
            /*
             * If this memory block allows read only, reject request.
             */
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** This memory block (%s) allows READ access only.",
                memory_block[0]->block_name) ;
            send_string_to_screen(err_msg,TRUE) ;
            ret = TRUE ;
            goto sume_exit ;
          }
          if (memory_block[0]->attributes & BYTE_ACCESS_ONLY)
          {
            /*
             * If this memory block allows byte access only, make sure
             * 'format' complies.
             */
            if (format != CHAR_EQUIVALENT)
            {
              sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** This memory block (%s) allows byte access only.",
                  memory_block[0]->block_name) ;
              send_string_to_screen(err_msg,TRUE) ;
              ret = TRUE ;
              goto sume_exit ;
            }
          }
          if (memory_block[0]->attributes & LONG_ALIGNED_WRITE_ONLY)
          {
            /*
             * If this memory block allows long and aaligned access
             * only, make sure 'format' and addresses comply.
             */
            if (format != LONG_EQUIVALENT)
            {
              sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** This memory block (%s) allows long access only.",
                  memory_block[0]->block_name) ;
              send_string_to_screen(err_msg,TRUE) ;
              ret = TRUE ;
              goto sume_exit ;
            }
            if ((unsigned long)address[0] & (BIT(0) | BIT(1)))
            {
              sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** This memory block (%s) allows long-aligned access only.",
                  memory_block[0]->block_name) ;
              send_string_to_screen(err_msg,TRUE) ;
              ret = TRUE ;
              goto sume_exit ;
            }
          }
          if (memory_block[0]->attributes & EEPROM_ACCESS)
          {
            eeprom_access_flag = TRUE ;
          }
          else
          {
            eeprom_access_flag = FALSE ;
          }
        }
        else
        {
          /*
           * Enter if this is the 'override' case
           */
          eeprom_access_flag = FALSE ;
          /*
           * In this case, unlock flash memory. Everything is allowed.
           */
          unlock_flash() ;
        }
        if (memory_block[0] == (MEMORY_BLOCK *)0)
        {
          /*
           * This could be, at this point, if 'override' has been
           * activated.
           */
          block_name = "Any (via override)" ;
        }
        else
        {
          block_name = memory_block[0]->block_name ;
        }
        if (!silent)
        {
          sal_sprintf(err_msg,
                "\r\n\n"
                "Memory block: \'%s\'. %u data items. \'%s\' access.\r\n"
                "=======================================================================",
                block_name,
                1,
                format_param_val->value.string_value) ;
          send_string_to_screen(err_msg,FALSE) ;
        }
        {
          write_memory(
              (char *)write_address[ui],1,
              format,&write_data[ui],eeprom_access_flag,
              silent) ;
        }
        if (override_flag)
        {
          /*
           * Lock flash memory. It has been unlocked for the 'override'
           * case where everything is allowed.
           */
          lock_flash() ;
        }
      }
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_MEM_VAL_READ_ID,1))
    {
      /*
       * Enter if this is a 'mem val_read' request.
       */
      unsigned
        long
          timeout,
          mask,
          val,
          read_address ;
      unsigned
        int
          offset_flag,
          override_flag,
          type,
          format,
          len,
          operation,
          silent,
          address_in_longs;
      int
        fail ;
      PARAM_VAL
        *format_param_val,
        *operation_param_val ;
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_MEM_VAL_READ_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        /*
         * Value of read address could not be retrieved.
         */
        get_val_of_err = TRUE ;
        goto sume_exit ;
      }
      read_address = param_val->value.ulong_value ;

      /*
       * Prameter 'address_in_longs'
       */
      if (search_param_val_pairs(
              current_line,&match_index,PARAM_MEM_ADDRESS_IN_LONGS_ID,1) == TRUE)
      {
        /*
         * Parameter 'address_in_longs' is not on this line. Set value of
         * address_in_longs flag to 'false'.
         */
        address_in_longs = FALSE ;
      }
      else
      {
        /*
         * address_in_longs parameter has been detected.
         */
        address_in_longs = TRUE ;
      }

      if (address_in_longs)
      {
        /*
         * If address in long indicator appears,
         * when user asks for 0x100, we take it as 0x400.
         * All other calulation should be with the 0x400 - as the CPU works with.
         */
        read_address *= 0x4;
      }



      /*
       * Always handle one memory object.
       */
      len = 1 ;
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_MEM_FORMAT_ID,1,
              &format_param_val,VAL_SYMBOL,err_msg))
      {
        /*
         * Value of format could not be retrieved.
         */
        get_val_of_err = TRUE ;
        goto sume_exit ;
      }
      format = (unsigned int)format_param_val->numeric_equivalent ;
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_MEM_VAL_MASK_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        /*
         * Value of format could not be retrieved.
         */
        get_val_of_err = TRUE ;
        goto sume_exit ;
      }
      mask = (unsigned int)param_val->value.ulong_value ;
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_MEM_VAL_TIMEOUT_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        /*
         * Value of format could not be retrieved.
         */
        get_val_of_err = TRUE ;
        goto sume_exit ;
      }
      timeout = (unsigned long)param_val->value.ulong_value ;
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_MEM_VAL_VAL_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        /*
         * Value of format could not be retrieved.
         */
        get_val_of_err = TRUE ;
        goto sume_exit ;
      }
      val = (unsigned int)param_val->value.ulong_value ;
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_MEM_VAL_OPERATION_ID,1,
              &operation_param_val,VAL_SYMBOL,err_msg))
      {
        /*
         * Value of display format could not be retrieved.
         */
        get_val_of_err = TRUE ;
        goto sume_exit ;
      }
      operation = (unsigned int)operation_param_val->numeric_equivalent ;
      /*
       * If offset is specified then follow 'offset'
       * logic (not absolute address).
       */
      if (search_param_val_pairs(
              current_line,&match_index,PARAM_MEM_OFFSET_ID,1) == TRUE)
      {
        /*
         * Parameter 'offset' is not on this line. Set value of
         * offset flag to 'false'.
         */
        offset_flag = FALSE ;
      }
      else
      {
        /*
         * Offset parameter has been detected.
         */
        offset_flag = TRUE ;
      }
      /*
       * Prameter 'silent'
       */
      if (search_param_val_pairs(
              current_line,&match_index,PARAM_MEM_SILENT_ID,1) == TRUE)
      {
        /*
         * Parameter 'silent' is not on this line. Set value of
         * silent flag to 'false'.
         */
        silent = FALSE ;
      }
      else
      {
        /*
         * Silent parameter has been detected.
         */
        silent = TRUE ;
      }


      /*
       * Prameter 'address_in_longs'
       */
      if (search_param_val_pairs(
              current_line,&match_index,PARAM_MEM_ADDRESS_IN_LONGS_ID,1) == TRUE)
      {
        /*
         * Parameter 'address_in_longs' is not on this line. Set value of
         * address_in_longs flag to 'false'.
         */
        address_in_longs = FALSE ;
      }
      else
      {
        /*
         * address_in_longs parameter has been detected.
         */
        address_in_longs = TRUE ;
      }

      /*
       * If override is specified then follow 'override'
       * logic (no memory block limits check).
       */
      if (search_param_val_pairs(
              current_line,&match_index,PARAM_MEM_OVERRIDE_ID,1) == TRUE)
      {
        /*
         * Parameter 'override' is not on this line. Set value of
         * override flag to 'false'.
         */
        override_flag = FALSE ;
      }
      else
      {
        /*
         * Override parameter has been detected.
         */
        override_flag = TRUE ;
      }
      if (offset_flag)
      {
        MEMORY_BLOCK
          *mem_block ;
        char
          *start_address ;
        /*
         * If 'offset' has been specified (explicitly or implicitly)
         * then the value of the 'read' parameter is offset within
         * the indicated memory block.
         */
        /*
         * If 'offset' has been specified (explicitly or implicitly)
         * then 'type' must be there as well, specifying the
         * memory block to access.
         */
        if (get_val_of(
                current_line,(int *)&match_index,PARAM_MEM_TYPE_ID,1,
                &param_val,VAL_SYMBOL,err_msg))
        {
          /*
           * Value of memory block type could not be retrieved.
           */
          get_val_of_err = TRUE ;
          goto sume_exit ;
        }
        type = (unsigned int)param_val->numeric_equivalent ;
        mem_block = block_id_to_ptr((unsigned int)type) ;
        if (mem_block == (MEMORY_BLOCK *)0)
        {
          sal_sprintf(err_msg,
              "\r\n\n"
              "*** This memory block identifier (%u) is unknown within memory map.",
              (unsigned short)type) ;
          send_string_to_screen(err_msg,TRUE) ;
          ret = TRUE ;
          goto sume_exit ;
        }
        if (!override_flag)
        {
          if (read_address >= mem_block->block_size)
          {
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** This offset (%lu) is beyond the end address of\r\n"
                "    this block (%s).",
                (unsigned long)read_address,mem_block->block_name) ;
            send_string_to_screen(err_msg,TRUE) ;
            ret = TRUE ;
            goto sume_exit ;
          }
        }
        start_address = mem_block->start_address + read_address ;
        read_address = (unsigned long)start_address ;
      }
      {
        MEMORY_BLOCK
          *memory_block[2] ;
        char
          *address[2] ;
        address[0] = (char *)read_address ;
        memory_block[0] = is_ptr_in_blocks(address[0]) ;
        if (!override_flag)
        {
          if (memory_block[0] == (MEMORY_BLOCK *)0)
          {
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** This base memory address (0x%08lX) is not within memory map.",
                (unsigned long)address[0]) ;
            send_string_to_screen(err_msg,TRUE) ;
            ret = TRUE ;
            goto sume_exit ;
          }
        }
        address[1] = (char *)(read_address + (len*format) - 1) ;
        memory_block[1] = is_ptr_in_blocks(address[1]) ;
        if (!override_flag)
        {
          if (memory_block[1] == (MEMORY_BLOCK *)0)
          {
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** This top memory address (0x%08lX) is not within memory map.",
                (unsigned long)address[1]) ;
            send_string_to_screen(err_msg,TRUE) ;
            ret = TRUE ;
            goto sume_exit ;
          }
          if (memory_block[1] != memory_block[0])
          {
            sal_sprintf(err_msg,
              "\r\n\n"
              "*** This memory range (0x%08lX,0x%08lX) crosses from one memory\r\n"
              "    block into another.",
              (unsigned long)address[0],(unsigned long)address[1]) ;
            send_string_to_screen(err_msg,TRUE) ;
            ret = TRUE ;
            goto sume_exit ;
          }
          if (memory_block[0]->attributes & BYTE_ACCESS_ONLY)
          {
            /*
             * If this memory block allows byte access only, make sure
             * 'format' complies.
             */
            if (format != CHAR_EQUIVALENT)
            {
              sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** This memory block (%s) allows byte access only.",
                  memory_block[0]->block_name) ;
              send_string_to_screen(err_msg,TRUE) ;
              ret = TRUE ;
              goto sume_exit ;
            }
          }
        }
        if (memory_block[0] == (MEMORY_BLOCK *)0)
        {
          /*
           * This could be, at this point, if 'override' has been
           * activated.
           */
          block_name = "Any (via override)" ;
        }
        else
        {
          block_name = memory_block[0]->block_name ;
        }
        if (!silent)
        {
          char
            *mask_string,
            *val_string ;
          unsigned
            short
              val_short,
              mask_short ;
          unsigned
            char
              val_char,
              mask_char ;
          char
            disp[240] ;

          val_char  = 0;
          val_short = 0;
          mask_short= 0;
          mask_char = 0;

          switch (format)
          {
            case SHORT_EQUIVALENT:
            {
              mask_string = val_string = "0x%04X" ;
              val_short = (unsigned short)val ;
              mask_short = (unsigned short)mask ;
              break ;
            }
            case CHAR_EQUIVALENT:
            {
              mask_string = val_string = "0x%02X" ;
              val_char = (unsigned char)val ;
              mask_char = (unsigned char)mask ;
              break ;
            }
            default:
            {
              mask_string = val_string = "0x%08X" ;
              break ;
            }
          }
          sal_sprintf(disp,
              "\r\n\n"
              "Memory block: \'%s\'. \'%s\' access.\r\n"
              "Read, mask with %s and compare with %s (%s)\r\n"
              "=======================================================================",
              block_name,
              format_param_val->value.string_value,
              mask_string,val_string,
              operation_param_val->value.string_value
              ) ;
          switch (format)
          {
            case SHORT_EQUIVALENT:
            {
              sal_sprintf(err_msg,
                  disp,
                  mask_short,val_short
                  ) ;
              break ;
            }
            case CHAR_EQUIVALENT:
            {
              sal_sprintf(err_msg,
                  disp,
                  mask_char,val_char
                  ) ;
              break ;
            }
            default:
            {
              sal_sprintf(err_msg,
                  disp,
                  mask,val
                  ) ;
              break ;
            }
          }
          send_string_to_screen(err_msg,TRUE) ;
        }
      }
      fail =
        read_compare_memory(
            (char *)read_address,
            format,val,mask,operation,timeout,silent) ;
      if (fail)
      {
        if (!silent)
        {
          send_string_to_screen(
            "\r\n"
            "*** \'val_read\' comparison has failed!!!",
            TRUE) ;
        }
      }
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_MEM_COUNTERS_ID,1))
    {
      /*
       * Enter if this is a 'mem counters' request.
       */
      unsigned
        int
          counter_id,
          silent;
      PARAM_VAL
        *counter_param_val ;
      /*
       * Prameter 'silent'
       */
      if (search_param_val_pairs(
              current_line,&match_index,PARAM_MEM_SILENT_ID,1) == TRUE)
      {
        /*
         * Parameter 'silent' is not on this line. Set value of
         * silent flag to 'false'.
         */
        silent = FALSE ;
      }
      else
      {
        /*
         * Silent parameter has been detected.
         */
        silent = TRUE ;
      }

      if (!search_param_val_pairs(current_line,&match_index,PARAM_MEM_COUNTERS_GET_ID,1))
      {
        /*
         * Enter if this is a 'mem counters get' request.
         */
        if (get_val_of(
                current_line,(int *)&match_index,PARAM_MEM_COUNTERS_GET_ID,1,
                &counter_param_val,VAL_SYMBOL,err_msg))
        {
          /*
           * Value of format could not be retrieved.
           */
          get_val_of_err = TRUE ;
          goto sume_exit ;
        }
        counter_id = (unsigned int)counter_param_val->numeric_equivalent ;
        display_get_mem_counter(counter_id,silent) ;
      }
      else if (!search_param_val_pairs(current_line,&match_index,PARAM_MEM_COUNTERS_CLEAR_ID,1))
      {
        /*
         * Enter if this is a 'mem counters get' request.
         */
        if (get_val_of(
                current_line,(int *)&match_index,PARAM_MEM_COUNTERS_CLEAR_ID,1,
                &counter_param_val,VAL_SYMBOL,err_msg))
        {
          /*
           * Value of format could not be retrieved.
           */
          get_val_of_err = TRUE ;
          goto sume_exit ;
        }
        counter_id = (unsigned int)counter_param_val->numeric_equivalent ;
        display_clear_mem_counter(counter_id,silent) ;
      }
      else
      {
        sal_sprintf(err_msg,
            "\r\n\n"
            "*** No known operation specified for \'mem counters\'.\r\n"
            "    Probably software error.") ;
        send_string_to_screen(err_msg,TRUE) ;
        ret = TRUE ;
        goto sume_exit ;
      }
    }
    else
    {
      /*
       * Enter if an unknown request.
       */
      send_string_to_screen(
        "\r\n"
        "*** Memory command without \'read\', \'write\', \'val_read\' or any other known\r\n"
        "    operations. Probably syntax error...\r\n",
        TRUE) ;
      ret = TRUE ;
      goto sume_exit ;
    }
  }
sume_exit:
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
