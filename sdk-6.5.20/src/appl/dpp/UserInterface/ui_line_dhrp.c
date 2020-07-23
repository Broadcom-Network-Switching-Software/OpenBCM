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

 /*
 #include <appl/diag/dpp/ref_sys.h>
*/
#if LINK_FAP20V_LIBRARIES
/* { */
/*
#include <bcm_app/dpp/../h/usrApp.h>
*/

/*
 * INCLUDE FILES:
 */

#include <appl/dpp/UserInterface/ui_defi.h>
#include <appl/dpp/UserInterface/ui_pure_defi_dhrp.h>

#include <appl/diag/dpp/utils_fap20v_calibrate.h>
#include <appl/diag/dpp/utils_fap20v_tests.h>
#include <appl/diag/dpp/utils_line_bringup.h>

#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_workload_status.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>
#include <soc/dpp/SOC_SAND_FAP20V/fap20v_api_diagnostics.h>

#include <DHRP/dhrp_management.h>
#include <DHRP/dhrp_gfa.h>

/*
 */

int ui_dhrp_mem_access(CURRENT_LINE *current_line)
{
  PARAM_VAL
    *param_val ;
  char
    err_msg[80*4] = "";
  char
    *proc_name;
  BOOL
    get_val_of_err;
  unsigned int
    match_index;
  enum
  {
    NO_ERR = 0,
    ERR_001, ERR_002, ERR_003, ERR_004, ERR_005,
    ERR_006,
    NUM_EXIT_CODES
  } ret ;
  unsigned long
    driver_ret;
  unsigned int
    dhrp_unit=0,
    i;
  unsigned long
    addr,
    nof_addrs,
    data;

  proc_name = "ui_dhrp_mem_access";
  ret = NO_ERR ;
  get_val_of_err = FALSE;
  driver_ret = SOC_SAND_OK;

  send_string_to_screen("",TRUE);

  if (get_val_of(
          current_line,(int *)&match_index,PARAM_DHRP_DEVICE_ID_ID,1,
          &param_val,VAL_NUMERIC,err_msg))
  {
    dhrp_unit = 0 ;
  }
  else
  {
    dhrp_unit = (unsigned long)param_val->value.ulong_value ;
  }
  if(dhrp_unit > 1)
  {
    soc_sand_os_printf( "Error: device ID should be 0 or 1\n");
    goto exit;
  }

    if (get_val_of(
            current_line,(int *)&match_index,PARAM_DHRP_MEM_NOF_ADDR_ID,1,
            &param_val,VAL_NUMERIC,err_msg))
    {
      nof_addrs = 1;
    }
    else
    {
      nof_addrs = (unsigned long)param_val->value.ulong_value ;
    }

  if ((!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_MEM_READ_ID,1)))
  {

    /*
     */
    if (get_val_of(
            current_line,(int *)&match_index,PARAM_DHRP_MEM_READ_ID,1,
            &param_val,VAL_NUMERIC,err_msg))
    {
      get_val_of_err = TRUE ;
      goto exit;
    }
    else
    {
      addr = (unsigned long)param_val->value.ulong_value ;
    }
    for(i=0;i<nof_addrs;i++)
    {
      SOC_SAND_PHYSICAL_ACCESS physical_access;
      soc_sand_get_physical_access_hook(&physical_access);
      physical_access.physical_read(
        &data,
        (unsigned long*)(0xd0000000 + (dhrp_unit*0x80000) + i*4),
        addr,
        4
      );
      soc_sand_os_printf( "0X%08x: 0X%08x\n\r", addr+(4*i), data);
    }
  }
  else if ((!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_MEM_WRITE_ID,1)))
  {
     /*
     */
    if (get_val_of(
            current_line,(int *)&match_index,PARAM_DHRP_MEM_WRITE_ID,1,
            &param_val,VAL_NUMERIC,err_msg))
    {
      get_val_of_err = TRUE ;
      goto exit;
    }
    else
    {
      addr = (unsigned long)param_val->value.ulong_value ;
    }


    if (get_val_of(
            current_line,(int *)&match_index,PARAM_DHRP_MEM_WRITE_DATA_ID,1,
            &param_val,VAL_NUMERIC,err_msg))
    {
      get_val_of_err = TRUE ;
      goto exit;
    }
    else
    {
      data = (unsigned long)param_val->value.ulong_value ;
    }

    for(i=0;i<nof_addrs;i++)
    {
      SOC_SAND_PHYSICAL_ACCESS physical_access;
      soc_sand_get_physical_access_hook(&physical_access);
      physical_access.physical_write(
        &data,
        (unsigned long*)(0xd0000000 + (dhrp_unit*0x80000) + i*4),
        addr,
        4
      );
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
    ret = ERR_001 ;
    goto exit ;
  }
exit:
  return ret;
}

int ui_dhrp_bsp(CURRENT_LINE *current_line)
{
  PARAM_VAL
    *param_val ;
  char
    err_msg[80*4] = "";
  char
    *proc_name;
  BOOL
    get_val_of_err;
  unsigned int
    match_index;
  enum
  {
    NO_ERR = 0,
    ERR_001, ERR_002, ERR_003, ERR_004, ERR_005,
    ERR_006,
    NUM_EXIT_CODES
  } ret ;
  unsigned long
    driver_ret;
  unsigned long
    verbose,
    silent;
  unsigned int
    dhrp_unit=0;
  PARAM_VAL
    *param_val_general_purpose = NULL;

  proc_name = "ui_dhrp_bsp";
  ret = NO_ERR ;
  get_val_of_err = FALSE;
  driver_ret = SOC_SAND_OK;

  send_string_to_screen("",TRUE);

  if (get_val_of(
          current_line,(int *)&match_index,PARAM_DHRP_DEVICE_ID_ID,1,
          &param_val,VAL_NUMERIC,err_msg))
  {
    dhrp_unit = 0 ;
  }
  else
  {
    dhrp_unit = (unsigned long)param_val->value.ulong_value ;
  }

  if(dhrp_unit > 1)
  {
    soc_sand_os_printf( "Error: device ID should be 0 or 1\n");
    goto exit;
  }
  if ((!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_CALIBRATE_CLEAR_NV_ID,1)))
  {
    ret = dhrp_gfa_nv_CLBRATE_PARAMS_clear(DHRP_FAP_ID_A);
    if (ret)
    {
      get_val_of_err = TRUE ;
      goto exit;
    }

    ret = dhrp_gfa_nv_CLBRATE_PARAMS_clear(DHRP_FAP_ID_B);
    if (ret)
    {
      get_val_of_err = TRUE ;
      goto exit;
    }

    goto exit;
  }
  else if ((!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_CALIBRATE_LOAD_ID,1)))
  {
    unsigned long
      ddr_i;
    FAP20V_CLBRATE_PARAMS
      clbrate_params;


    fap20v_clear_CLBRATE_PARAMS(&clbrate_params);

    /*
     */
    ret = get_val_of( current_line, (int *)&match_index,
                      PARAM_DHRP_NOF_DDR_ID, 1,
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
                      PARAM_DHRP_SAMPLE_DELAY_REG_ID, 1,
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
                        PARAM_DHRP_END_WIN_ID, ddr_i,
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
                        PARAM_DHRP_AVRG_ID, ddr_i,
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
      dhrp_unit,
      &clbrate_params,
      FALSE
    );

  }
  else if ((!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_CALIBRATE_DDR_FULL_ID,1)))
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


    if (!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_VERBOSE_ID,1))
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
                      PARAM_DHRP_NOF_DDR_ID, 1,
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
                        PARAM_DHRP_OFFSET_CORRECT_ID, offset_i,
                        &param_val, VAL_NUMERIC, err_msg
                      ) ;
      if (ret)
      {
        break;
      }
      offset_correct[offset_i-1] = (unsigned long)param_val->value.ulong_value;
    }
    ret = 0;

    if (!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_DO_EXHAUSTIVE_SEARCH_ID,1))
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
    fap20v_calibrate_ddr_nof_scan_set(FAP20V_CALIBRATE_DDR2_NOF_SCAN);

    fap20v_calibrate_ddr_full(
      dhrp_unit,
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
  else if ((!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_CALIBRATE_DDR_ID,1)))
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
            current_line,(int *)&match_index,PARAM_DHRP_MILISEC_ID,1,
            &param_val,VAL_NUMERIC,err_msg))
    {
      get_val_of_err = TRUE ;
      goto exit;
    }
    else
    {
      delay_in_milisec = (unsigned long)param_val->value.ulong_value ;
    }



    if (!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_VERBOSE_ID,1))
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
            current_line,(int *)&match_index,PARAM_DHRP_NOF_PKTS_ID,1,
            &param_val,VAL_NUMERIC,err_msg))
    {
      get_val_of_err = TRUE ;
      goto exit;
    }
    else
    {
      max_nof_send_pkts = (unsigned long)param_val->value.ulong_value ;
    }


    fap20v_calibrate_ddr(
      dhrp_unit,
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
  else if ((!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_DRAM_CALIBRATE_AUTO_ID,1)))
  {
    dhrp_dram_auto_calibration(dhrp_unit);
  }
  else if ((!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_SCAN_DRAM_DELAY_AUTO_B_ID,1)))
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
            current_line,(int *)&match_index,PARAM_DHRP_MILISEC_ID,1,
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
            current_line,(int *)&match_index,PARAM_DHRP_FIX_FACTOR_ID,1,
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
            current_line,(int *)&match_index,PARAM_DHRP_SAMPLE_NUM_ID,1,
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
            current_line,(int *)&match_index,PARAM_DHRP_SAMPLE_ADD_ID,1,
            &param_val,VAL_NUMERIC,err_msg))
    {
      get_val_of_err = TRUE ;
      goto exit;
    }
    else
    {
      sample_add = (unsigned long)param_val->value.ulong_value ;
    }

    if (!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_VERBOSE_ID,1))
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
            current_line,(int *)&match_index,PARAM_DHRP_START_DDR_ID,1,
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
            current_line,(int *)&match_index,PARAM_DHRP_END_DDR_ID,1,
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
            current_line,(int *)&match_index,PARAM_DHRP_PACKET_SCHEME_ID,1,
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
            current_line,(int *)&match_index,PARAM_DHRP_NOF_PKTS_ID,1,
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
      dhrp_unit,
      fix_factor, delay_in_milisec,
      sample_num, sample_add,
      start_ddr, end_ddr,
      packet_scheme, max_nof_send_pkts,
      &sample_delay_reg,
      end_win,
      silent
    );
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_LBG_ID,1))
  {
    static FAP20V_LOCAL_BW_GEN
      orig_lbg;

    if (!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_VERBOSE_ID,1))
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
    if (!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_LBG_START_CONF_ID,1))
    {
      driver_ret = fap20v_lbg_start_conf(dhrp_unit, &orig_lbg, silent);
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_LBG_SEND_PKT_ID,1))
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
              current_line,(int *)&match_index,PARAM_DHRP_NOF_PKTS_ID,1,
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
              current_line,(int *)&match_index,PARAM_DHRP_PKT_BYTE_SIZE_ID,1,
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
                          PARAM_DHRP_PAD_DATA_ID, byte_i,
                          &param_val, VAL_NUMERIC, err_msg
                        ) ;
        if (ret)
        {
          break;
        }
        pad_data[byte_i-1] = ((unsigned char)param_val->value.ulong_value)&0xFF;
      }
      ret = 0;


      driver_ret = fap20v_lbg_send_packet(dhrp_unit, pkt_byte_size, nof_pkt, pad_data, silent);
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_GET_RATE_ID,1))
    {
      unsigned long
        pkt_byte_size;
      FAP20V_LBG_RATE_REPORT
        report;


      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_DHRP_PKT_BYTE_SIZE_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit;
      }
      else
      {
        pkt_byte_size = (unsigned long)param_val->value.ulong_value ;
      }

      driver_ret = fap20v_lbg_get_rate(
                     dhrp_unit, pkt_byte_size, FAP20V_LBG_MEASUREMENT_DELAY_MILIS,
                     &report
                   );
      fap20v_print_FAP20V_LBG_RATE_REPORT(&report, ui_printing_policy_get());
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_LBG_RESTORE_CONF_ID,1))
    {
      driver_ret = fap20v_lbg_restore_conf(dhrp_unit, &orig_lbg, silent);
    }
    else
    {
      /*
       * No valid DHRP variable has been entered.
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
    disp_fap20v_result(driver_ret, "LBG function:");
  }
  else if ((!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_IDDR_TEST_RW_ID,1)))
  {
    unsigned int
      data_i;
    unsigned long
      dram_offset,
      data_to_test[FAP20V_IDRAM_WORD_NOF_LONGS],
      nof_iteration;


    /*
     */
    if (get_val_of(
            current_line,(int *)&match_index,PARAM_DHRP_IDDR_OFFSET_ID,1,
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
            current_line,(int *)&match_index,PARAM_DHRP_NOF_ITERATION_ID,1,
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
                        PARAM_DHRP_PAD_DATA_ID, data_i,
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
      dhrp_unit,
      dram_offset,
      data_to_test,
      nof_iteration,
      TRUE,
      FALSE
    );
  }
  else if ((!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_ZBT_TEST_RW_ID,1)))
  {
    fap20v_zbt_test_rw(dhrp_unit, FALSE);
  }
  else if ((!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_INQ_TEST_RW_ID,1)))
  {
    fap20v_inq_test_rw(dhrp_unit, FALSE);
  }
  else if ((!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_CPU_TEST_RW_ID,1)))
  {
    fap20v_cpu_test_rw(dhrp_unit, FALSE);
  }
  else if ((!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_CLR_CNT_INT_ID,1)))
    {
      gfa_clear_counters_and_indications(dhrp_unit);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_RESET_DEVICE_ID,1))
  {
    BOOL
      remain_active = TRUE;
    unsigned long
      new_fap_id;

    new_fap_id = 0;

    sal_sprintf(err_msg,
            "\r\n"
            "*** \'dhrp_gfa_reset' error \r\n"
            "*** or \'unknown parameter \'.\r\n"
            "    Probably SW error\r\n"
    );

    /*
     */
    if (!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_REMAIN_OFF_ID,1))
    {
      remain_active = FALSE;
    }

   ret = dhrp_gfa_reset(
           dhrp_unit,
           remain_active
         );
    if (0 != ret)
    {
      send_string_to_screen(err_msg,TRUE);
      get_val_of_err = TRUE;
      goto exit;
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
    ret = ERR_001 ;
    goto exit ;
  }
exit:
  return ret;
}
/*****************************************************
*NAME
*  subject_line_dhrp
*TYPE: PROC
*DATE: 29/DEC/2002
*FUNCTION:
*  Process input line which has an 'dhrp' subject.
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
int subject_dhrp(CURRENT_LINE *current_line, CURRENT_LINE **current_line_ptr)
{
  char
    err_msg[80*4] = "";
  char
    *proc_name;
  BOOL
    get_val_of_err ;
  unsigned int
    match_index;
  enum
  {
    NO_ERR = 0,
    ERR_001, ERR_002, ERR_003, ERR_004, ERR_005,
    ERR_006,
    NUM_EXIT_CODES
  } ret ;
  unsigned long
    driver_ret;


  proc_name = "subject_line_dhrp";
  ret = NO_ERR ;
  get_val_of_err = FALSE;
  driver_ret = SOC_SAND_OK;

  send_string_to_screen("",TRUE);

  /*
   * the rest ot the line is empty
   */
  if (current_line->num_param_names == 0)
  {
    /*
     * Enter if there are no parameters on the line (just 'line_card').
     */
    send_string_to_screen("Line-Card-DHRP is connected to this CPU-card.\n\r", FALSE);
    goto exit ;
  }
  /*
   * Else, there are parameters on the line (not just 'dhrp').
   */
  else if ( !search_param_val_pairs(current_line,&match_index,PARAM_DHRP_BSP_ID,1))
  {
    ret = ui_dhrp_bsp(current_line);
  }
  else if (
          (!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_MEM_READ_ID,1))  ||
          (!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_MEM_WRITE_ID,1))
          )
  {
    ui_dhrp_mem_access(current_line);
  }

  else
  {
    /*
     * No valid DHRP variable has been entered.
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


/* } */
#endif /*LINK_FAP20V_LIBRARIES*/


