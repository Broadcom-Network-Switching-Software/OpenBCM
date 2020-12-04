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

/*
 * INCLUDE FILES:
 */

#include <stdio.h>
/*
 * User interface internal include file.
 */
#include <appl/dpp/UserInterface/ui_defi.h>
#include <appl/dpp/UserInterface/ui_pure_defi_diag.h>

#include <appl/diag/dpp/utils_hw_diagnostics.h>

#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_workload_status.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

/*
 */

/*
 * Error Handling
 * {
 */

void ui_diag_reoprt_sw_error_to_user(unsigned int err_id)
{
  char
    err_msg[80*2] = "";

  sal_sprintf(err_msg, "process_tevb_app_line SW error %u\r\n", err_id) ;
  send_string_to_screen(err_msg,TRUE) ;
}





void
  diag_ui_report_driver_err(
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
*  subject_diag
*TYPE: PROC
*DATE: 3/Mar/2007
*FUNCTION:
*  Process input line which has an 'generic_tests' subject.
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
int subject_diag(CURRENT_LINE *current_line, CURRENT_LINE **current_line_ptr)
{
  char
    err_msg[80*4] = "";
  char
    *proc_name;
  BOOL
    get_val_of_err ;
  unsigned int
    match_index,
    short_format,
    verbose,
    silent;
  UTILS_HW_DIAG_MEM_TEST_REVERT_DATA
    revert_data = UTILS_HW_DIAG_MEM_TEST_REVERT_DATA_NO;
  UTILS_HW_DIAG_MEM_TEST_DATA_TYPE
    data_type = UTILS_HW_DIAG_MEM_TEST_DATA_TYPE_RUNNING_INDEX;
  UTILS_HW_DIAG_MEM_TEST_ADDR_WALK_TYPE
    addr_walk_type = UTILS_HW_DIAG_MEM_TEST_ADDR_WALK_TYPE_INC_ONE;
  UTILS_HW_DIAG_MEM_TEST_ADDRESS_ALIGNMENT
    addr_alignment = UTILS_HW_DIAG_MEM_TEST_ADDRESS_BYTE_ALIGNMENT;
  UTILS_HW_DIAG_MEM_TEST_REVERT_ADDRS
    revert_addr = UTILS_HW_DIAG_MEM_TEST_REVERT_ADDRS_NO;
  UTILS_HW_DIAG_MEM_TEST_MEMORY_SPACE
    mem_space;
  PARAM_VAL
    *param_val,
    *param_val_device;
  unsigned long
    base_addr,
    size,
    pattern = 0;
  enum
  {
    NO_ERR = 0,
    ERR_001, ERR_002, ERR_003, ERR_004, ERR_005,
    ERR_006, ERR_007, ERR_008, ERR_009, ERR_010,
    ERR_011, ERR_012, ERR_013, ERR_014, ERR_015,
    NUM_EXIT_CODES
  } ui_ret ;

  proc_name = "subject_diag";
  ui_ret = NO_ERR ;
  get_val_of_err = FALSE;
  short_format = TRUE;

  send_string_to_screen("",TRUE);


  /*
   * the rest of the line is empty
   */
  if (current_line->num_param_names == 0)
  {
    /*
     * Enter if there are no parameters on the line (just 'line_card').
     */
    goto exit ;
  }


  if (!search_param_val_pairs(current_line,&match_index,PARAM_DIAG_VERBOSE_ID,1))
  {
    verbose = TRUE ;
  }
  else
  {
    verbose = FALSE ;
  }
  silent = !verbose;


  if ( !search_param_val_pairs(current_line,&match_index,PARAM_DIAG_HW_ID,1))
  {
    /*
     * Operation related to diag->hw
     */

    if (!search_param_val_pairs(current_line,&match_index,PARAM_DIAG_HW_MEM_ID,1))
    {
      /*
       * Operation related to diag->hw->mem_test
       */

      if (!search_param_val_pairs(current_line,&match_index,PARAM_DIAG_HW_MEM_SET_SPACE_ID,1))
      {
        /*
         * Operation related to diag->hw->mem_test->set_space
         */

        if (!search_param_val_pairs(current_line,&match_index,PARAM_DIAG_HW_MEM_SPACE_CPU_ID,1))
        {
          /*
           * diag->hw->mem_test->set_space->cpu
           */
          utils_hw_diag_memory_test_set_mem_space(
            UTILS_HW_DIAG_MEM_TEST_MEMORY_SPACE_CPU,
            0);

        } /* PARAM_DIAG_HW_MEM_SPACE_CPU_ID */

        else if (!search_param_val_pairs(current_line,&match_index,PARAM_DIAG_HW_MEM_SPACE_DRAM_ID,1))
        {
          /*
           * diag->hw->mem_test->set_space->dram
           */
          sal_sprintf(err_msg,
            "\r\n"
            "*** \'utils_hw_diag_memory_test_set_mem_space\' error \r\n"
            "*** or \'unknown parameter \'.\r\n"
            "    Probably SW error\r\n"
           );

          ui_ret = get_val_of( current_line, (int *)&match_index,
                      PARAM_DIAG_HW_MEM_SPACE_DRAM_ID, 1,
                      &param_val_device, VAL_NUMERIC, err_msg
                    );
          if (0 != ui_ret)
          {
            send_string_to_screen(err_msg,TRUE);
            ui_ret = ERR_001;
            goto exit;
          }

          utils_hw_diag_memory_test_set_mem_space(
            UTILS_HW_DIAG_MEM_TEST_MEMORY_SPACE_DRAM,
            param_val_device->value.ulong_value
            );

        } /* PARAM_DIAG_HW_MEM_SPACE_DRAM_ID */

        else if (!search_param_val_pairs(current_line,&match_index,PARAM_DIAG_HW_MEM_SPACE_ZBT_ID,1))
        {
          /*
           * diag->hw->mem_test->set_space->zbt
           */
          sal_sprintf(err_msg,
            "\r\n"
            "*** \'utils_hw_diag_memory_test_set_mem_space\' error \r\n"
            "*** or \'unknown parameter \'.\r\n"
            "    Probably SW error\r\n"
           );

          ui_ret = get_val_of( current_line, (int *)&match_index,
                      PARAM_DIAG_HW_MEM_SPACE_ZBT_ID, 1,
                      &param_val_device, VAL_NUMERIC, err_msg
                    );
          if (0 != ui_ret)
          {
            send_string_to_screen(err_msg,TRUE);
            ui_ret = ERR_001;
            goto exit;
          }

          utils_hw_diag_memory_test_set_mem_space(
            UTILS_HW_DIAG_MEM_TEST_MEMORY_SPACE_ZBT,
            param_val_device->value.ulong_value
            );


        } /* PARAM_DIAG_HW_MEM_SPACE_ZBT_ID */

        else
        {
          /*
           * No valid operation variable has been entered.
           */
          send_string_to_screen(
              "*** Should not reach here.\r\n",
              FALSE) ;
          ui_ret = ERR_002 ;
          goto exit ;
        }

      } /* diag->hw->mem_test->set_space */

      else if (!search_param_val_pairs(current_line,&match_index,PARAM_DIAG_HW_MEM_BASE_ADDR_ID,1))
      {
        /*
         * Operation related to diag->hw->mem_test->base_addr
         */

        if (!search_param_val_pairs(current_line,&match_index,PARAM_DIAG_HW_MEM_SIZE_ID,1))
        {
          /*
           * Operation related to diag->hw->mem_test->base_addr->size
           */


          ui_ret = get_val_of( current_line, (int *)&match_index,
                      PARAM_DIAG_HW_MEM_BASE_ADDR_ID, 1,
                      &param_val, VAL_NUMERIC, err_msg
                    );
          if (0 != ui_ret)
          {
            send_string_to_screen(err_msg,TRUE);
            ui_ret = ERR_003;
            goto exit;
          }

          base_addr = param_val->value.ulong_value;

          ui_ret = get_val_of( current_line, (int *)&match_index,
                      PARAM_DIAG_HW_MEM_SIZE_ID, 1,
                      &param_val, VAL_NUMERIC, err_msg
                    );
          if (0 != ui_ret)
          {
            send_string_to_screen(err_msg,TRUE);
            ui_ret = ERR_003;
            goto exit;
          }

          size = param_val->value.ulong_value;

          /* default alignment value for ZBT is long */
          mem_space = utils_hw_diag_memory_test_get_mem_space();
          if ((mem_space==UTILS_HW_DIAG_MEM_TEST_MEMORY_SPACE_ZBT)||
            (mem_space==UTILS_HW_DIAG_MEM_TEST_MEMORY_SPACE_DRAM))
          {
            addr_alignment = UTILS_HW_DIAG_MEM_TEST_ADDRESS_LONG_ALIGNMENT;
          }


          /* Get optional parameters */

          if (!search_param_val_pairs(current_line,&match_index,PARAM_DIAG_HW_MEM_REVERT_DATA_ID,1))
          {
            ui_ret = get_val_of( current_line, (int *)&match_index,
              PARAM_DIAG_HW_MEM_REVERT_DATA_ID, 1,
              &param_val, VAL_SYMBOL, err_msg
            );
            if (0 != ui_ret)
            {
              send_string_to_screen(err_msg,TRUE);
              ui_ret = ERR_004;
              goto exit;
            }
            revert_data = param_val->numeric_equivalent;
          }

          if (!search_param_val_pairs(current_line,&match_index,PARAM_DIAG_HW_MEM_REVERT_OFFSET_ID,1))
          {
            ui_ret = get_val_of( current_line, (int *)&match_index,
             PARAM_DIAG_HW_MEM_REVERT_OFFSET_ID, 1,
              &param_val, VAL_SYMBOL, err_msg
            );
            if (0 != ui_ret)
            {
              send_string_to_screen(err_msg,TRUE);
              ui_ret = ERR_005;
              goto exit;
            }
            revert_addr = param_val->numeric_equivalent;
          }


          if (!search_param_val_pairs(current_line,&match_index,PARAM_DIAG_HW_MEM_DATA_WALK_ID,1))
          {
            ui_ret = get_val_of( current_line, (int *)&match_index,
             PARAM_DIAG_HW_MEM_DATA_WALK_ID, 1,
              &param_val, VAL_SYMBOL, err_msg
            );
            if (0 != ui_ret)
            {
              send_string_to_screen(err_msg,TRUE);
              ui_ret = ERR_006;
              goto exit;
            }
            data_type = param_val->numeric_equivalent;
          }

          if (!search_param_val_pairs(current_line,&match_index,PARAM_DIAG_HW_MEM_OFFSET_WALK_ID,1))
          {
            ui_ret = get_val_of( current_line, (int *)&match_index,
             PARAM_DIAG_HW_MEM_OFFSET_WALK_ID, 1,
              &param_val, VAL_SYMBOL, err_msg
            );
            if (0 != ui_ret)
            {
              send_string_to_screen(err_msg,TRUE);
              ui_ret = ERR_007;
              goto exit;
            }
            addr_walk_type = param_val->numeric_equivalent;
          }

          if (!search_param_val_pairs(current_line,&match_index,PARAM_DIAG_HW_MEM_ALIGNMENT_ID,1))
          {
            ui_ret = get_val_of( current_line, (int *)&match_index,
             PARAM_DIAG_HW_MEM_ALIGNMENT_ID, 1,
              &param_val, VAL_SYMBOL, err_msg
            );
            if (0 != ui_ret)
            {
              send_string_to_screen(err_msg,TRUE);
              ui_ret = ERR_008;
              goto exit;
            }
            addr_alignment = param_val->numeric_equivalent;
          }

          if (!search_param_val_pairs(current_line,&match_index,PARAM_DIAG_HW_MEM_PATTERN_ID,1))
          {
            if (data_type!=UTILS_HW_DIAG_MEM_TEST_DATA_TYPE_PATTERN)
            {
              sal_sprintf(err_msg,
                "\r\n"
                "*** error - cannot use pattern when data walk type is not \'pattern\'\r\n"
               );
              send_string_to_screen(err_msg,TRUE);
              ui_ret = ERR_009;
              goto exit;
            }


            ui_ret = get_val_of( current_line, (int *)&match_index,
               PARAM_DIAG_HW_MEM_PATTERN_ID, 1,
                &param_val, VAL_NUMERIC, err_msg
              );
            if (0 != ui_ret)
            {
              send_string_to_screen(err_msg,TRUE);
              ui_ret = ERR_010;
              goto exit;
            }
            pattern = param_val->value.ulong_value;
          }

          utils_hw_diag_memory_test_execute_one_test(
            base_addr,
            size,
            verbose,
            revert_data,
            data_type,
            addr_walk_type,
            addr_alignment,
            revert_addr,
            pattern
            );

        } /* diag->hw->mem_test->base_addr->size */

      } /* diag->hw->mem_test->base_addr */

    } /* diag->hw->mem_test */
  }

  /*
   * Else, there are parameters on the line (not just 'diag').
   */
  else
  {
    /*
     * No valid TEVB variable has been entered.
     * There must be at least one!
     */
    send_string_to_screen(
        "\r\n"
        "*** No valid parameter has been indicated for handling.\r\n"
        "    At least one must be entered.\r\n",
        FALSE) ;
    ui_ret = ERR_004 ;
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
#endif /*LINK_FAP20V_LIBRARIES*/


