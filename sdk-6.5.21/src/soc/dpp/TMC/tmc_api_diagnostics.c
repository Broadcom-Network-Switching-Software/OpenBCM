/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/







#include <shared/bsl.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Management/sand_general_params.h>
#include <soc/dpp/SAND/Management/sand_callback_handles.h>

#include <soc/dpp/SAND/Management/sand_device_management.h>

#include <soc/dpp/SAND/SAND_FM/sand_trigger.h>
#include <soc/dpp/SAND/SAND_FM/sand_mem_access.h>

#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>

#include <soc/dpp/TMC/tmc_api_diagnostics.h>
#include <soc/dpp/TMC/tmc_api_statistics.h>
#include <soc/dpp/TMC/tmc_api_general.h>

#include <soc/dpp/TMC/tmc_api_ingress_packet_queuing.h>



























void
  SOC_TMC_DIAG_SOFT_ERR_RESULT_clear(
    SOC_SAND_OUT SOC_TMC_DIAG_SOFT_ERR_RESULT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_DIAG_SOFT_ERR_RESULT));
  info->err_sp = 0;
  info->err_dp = 0;
  info->err_rf = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_DIAG_SOFT_SMS_RESULT_clear(
    SOC_SAND_OUT SOC_TMC_DIAG_SOFT_SMS_RESULT *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_DIAG_SOFT_SMS_RESULT));
  for (ind = 0; ind < SOC_TMC_DIAG_CHAIN_LENGTH_MAX_IN_UINT32S; ++ind)
  {
    info->diag_chain[ind] = 0;
  }
  SOC_TMC_DIAG_SOFT_ERR_RESULT_clear(&(info->nof_errs));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
#if SOC_TMC_DEBUG_IS_LVL1

void
  SOC_TMC_DIAG_LBG_PACKET_PATTERN_print(
    SOC_SAND_IN SOC_TMC_DIAG_LBG_PACKET_PATTERN *info
  )
{
  uint32 ind=0;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind=0; ind<info->data_byte_size; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "Data[%u]:       %02x\n\r"), ind,info->data[ind]));
  }
  LOG_CLI((BSL_META_U(unit,
                      "Data_byte_size: %u\n\r"),info->data_byte_size));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_DIAG_LBG_TRAFFIC_INFO_print(
    SOC_SAND_IN SOC_TMC_DIAG_LBG_TRAFFIC_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Nof_packets: %u[Packets]\n\r"),info->nof_packets));
  LOG_CLI((BSL_META_U(unit,
                      "Packet_size: %u[Bytes]\n\r"),info->packet_size));
  SOC_TMC_DIAG_LBG_PACKET_PATTERN_print(&info->pattern);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_DIAG_LBG_PATH_INFO_print(
    SOC_SAND_IN SOC_TMC_DIAG_LBG_PATH_INFO *info
  )
{
  uint32
    indx;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (indx = 0; indx < info->nof_ports; ++indx)
  {
    LOG_CLI((BSL_META_U(unit,
                        "%u \n\r"), info->ports[indx] ));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_DIAG_LBG_INFO_print(
    SOC_SAND_IN SOC_TMC_DIAG_LBG_INFO *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_DIAG_LBG_PATH_INFO_print(&info->path);

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

const char*
  SOC_TMC_DIAG_QDR_BIST_ADDR_MODE_to_string(
    SOC_SAND_IN SOC_TMC_DIAG_QDR_BIST_ADDR_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_DIAG_QDR_BIST_ADDR_MODE_NORMAL:
    str = "QDR_BIST_ADDR_MODE_NORMAL";
  break;

  case SOC_TMC_DIAG_QDR_BIST_ADDR_MODE_ADDRESS_SHIFT:
    str = "QDR_BIST_ADDR_MODE_ADDRESS_SHIFT";
  break;

  case SOC_TMC_DIAG_QDR_BIST_ADDR_MODE_ADDRESS_TEST:
    str = "QDR_BIST_ADDR_MODE_ADDRESS_TEST";
  break;

  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_DIAG_QDR_BIST_DATA_MODE_to_string(
    SOC_SAND_IN SOC_TMC_DIAG_QDR_BIST_DATA_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_DIAG_QDR_BIST_DATA_MODE_NORMAL:
    str = "QDR_BIST_DATA_MODE_NORMAL";
  break;

  case SOC_TMC_DIAG_QDR_BIST_DATA_MODE_PATTERN_BIT:
    str = "QDR_BIST_DATA_MODE_PATTERN_BIT";
  break;

  case SOC_TMC_DIAG_QDR_BIST_DATA_MODE_RANDOM:
    str = "QDR_BIST_DATA_MODE_RANDOM";
  break;

  case SOC_TMC_DIAG_QDR_BIST_DATA_MODE_DATA_SHIFT:
    str = "QDR_BIST_DATA_MODE_DATA_SHIFT";
  break;

  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_DIAG_BIST_DATA_PATTERN_to_string(
    SOC_SAND_IN SOC_TMC_DIAG_BIST_DATA_PATTERN enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_DIAG_BIST_DATA_PATTERN_DIFF:
    str = "BIST_DATA_PATTERN_DIFF";
  break;

  case SOC_TMC_DIAG_BIST_DATA_PATTERN_ONE:
    str = "BIST_DATA_PATTERN_ONE";
    break;

  case SOC_TMC_DIAG_BIST_DATA_PATTERN_ZERO:
    str = "BIST_DATA_PATTERN_ZERO";
    break;

  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_DIAG_DRAM_BIST_DATA_MODE_to_string(
    SOC_SAND_IN SOC_TMC_DIAG_DRAM_BIST_DATA_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_DIAG_DRAM_BIST_DATA_MODE_NORMAL:
    str = "DRAM_BIST_DATA_MODE_NORMAL";
  break;

  case SOC_TMC_DIAG_DRAM_BIST_DATA_MODE_PATTERN_BIT:
    str = "DRAM_BIST_DATA_MODE_PATTERN_BIT";
  break;

  case SOC_TMC_DIAG_DRAM_DATA_MODE_RANDOM:
    str = "DRAM_DATA_MODE_RANDOM";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_DIAG_SOFT_ERROR_PATTERN_to_string(
    SOC_SAND_IN  SOC_TMC_DIAG_SOFT_ERROR_PATTERN enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_DIAG_SOFT_ERROR_PATTERN_ONE:
    str = "one";
  break;
  case SOC_TMC_DIAG_SOFT_ERROR_PATTERN_ZERO:
    str = "zero";
  break;
  case SOC_TMC_DIAG_SOFT_ERROR_PATTERN_DIFF1:
    str = "diff1";
  break;
  case SOC_TMC_DIAG_SOFT_ERROR_PATTERN_DIFF2:
    str = "diff2";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_DIAG_SOFT_COUNT_TYPE_to_string(
    SOC_SAND_IN  SOC_TMC_DIAG_SOFT_COUNT_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_DIAG_SOFT_COUNT_TYPE_FAST:
    str = "fast";
  break;
  case SOC_TMC_DIAG_SOFT_COUNT_TYPE_COMPLETE:
    str = "complete";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_TMC_DIAG_QDR_BIST_TEST_RUN_INFO_print(
    SOC_SAND_IN SOC_TMC_DIAG_QDR_BIST_TEST_RUN_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Nof Write Commands: %u[Commands]\n\r"),info->nof_cmnds_write));
  LOG_CLI((BSL_META_U(unit,
                      "Nof Write Commands: %u[Commands]\n\r"),info->nof_cmnds_read));
  LOG_CLI((BSL_META_U(unit,
                      "Start_addr: %u\n\r"),info->start_addr));
  LOG_CLI((BSL_META_U(unit,
                      "End_addr: %u\n\r"),info->end_addr));
  LOG_CLI((BSL_META_U(unit,
                      "Read Offset: %u[Commands]\n\r"),info->read_offset));
  LOG_CLI((BSL_META_U(unit,
                      "Data_mode %s \n\r"),
           SOC_TMC_DIAG_QDR_BIST_DATA_MODE_to_string(info->data_mode)
           ));
  LOG_CLI((BSL_META_U(unit,
                      "Address_mode %s \n\r"),
           SOC_TMC_DIAG_QDR_BIST_ADDR_MODE_to_string(info->address_mode)
           ));
  LOG_CLI((BSL_META_U(unit,
                      "Data_pattern %s \n\r"),
           SOC_TMC_DIAG_BIST_DATA_PATTERN_to_string(info->data_pattern)
           ));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_DIAG_QDR_BIST_TEST_RES_INFO_print(
    SOC_SAND_IN SOC_TMC_DIAG_QDR_BIST_TEST_RES_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  if (info->is_test_finished == FALSE)
  {
    LOG_CLI((BSL_META_U(unit,
                        "Test not finished...\n\r")));
  }
  else
  {
    if (info->is_qdr_up == FALSE)
    {
      LOG_CLI((BSL_META_U(unit,
                          "QDR interface is down!\n\r")));
    }
    else
    {
      LOG_CLI((BSL_META_U(unit,
                          "QDR interface is up: \n\r")));
      LOG_CLI((BSL_META_U(unit,
                          "Bit_err_counter: %u[Bits]\n\r"),info->bit_err_counter));
      LOG_CLI((BSL_META_U(unit,
                          "Reply_err_counter: %u[Errors]\n\r"),info->reply_err_counter));
      LOG_CLI((BSL_META_U(unit,
                          "Bits_error_bitmap: %u\n\r"),info->bits_error_bitmap));
      LOG_CLI((BSL_META_U(unit,
                          "Last_addr_err: %u\n\r"),info->last_addr_err));
      LOG_CLI((BSL_META_U(unit,
                          "Last_data_err: %u\n\r"),info->last_data_err));
    }
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_DIAG_DRAM_BIST_TEST_RUN_INFO_print(
    SOC_SAND_IN SOC_TMC_DIAG_DRAM_BIST_TEST_RUN_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Finite_nof_cmnds: %u[Commands]\n\r"),info->is_finite_nof_cmnds));
  LOG_CLI((BSL_META_U(unit,
                      "Writes_per_cycle: %u[Write commands]\n\r"),info->writes_per_cycle));
  LOG_CLI((BSL_META_U(unit,
                      "Reads_per_cycle: %u[Read commands]\n\r"),info->reads_per_cycle));
  LOG_CLI((BSL_META_U(unit,
                      "Start_addr: %u\n\r"),info->start_addr));
  LOG_CLI((BSL_META_U(unit,
                      "End_addr: %u\n\r"),info->end_addr));
  LOG_CLI((BSL_META_U(unit,
                      "data_mode %s \n\r"),
           SOC_TMC_DIAG_DRAM_BIST_DATA_MODE_to_string(info->data_mode)
           ));
  LOG_CLI((BSL_META_U(unit,
                      "data_pattern %s \n\r"),
           SOC_TMC_DIAG_BIST_DATA_PATTERN_to_string(info->data_pattern)
           ));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_DIAG_DRAM_BIST_TEST_RES_INFO_print(
    SOC_SAND_IN SOC_TMC_DIAG_DRAM_BIST_TEST_RES_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  if (info->is_test_finished == FALSE)
  {
    LOG_CLI((BSL_META_U(unit,
                        "Test not finished...\n\r")));
  }
  else
  {
    if (info->is_dram_up == FALSE)
    {
      LOG_CLI((BSL_META_U(unit,
                          "DRAM interface is down!\n\r")));
    }
    else
    {
      LOG_CLI((BSL_META_U(unit,
                          "DRAM interface is up. ")));
      LOG_CLI((BSL_META_U(unit,
                          "Error counter: %u[Errors]\n\r"),info->reply_err_counter));
    }
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_DIAG_DRAM_ACCESS_INFO_print(
    SOC_SAND_IN SOC_TMC_DIAG_DRAM_ACCESS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "address: %u\n\r"),info->address));
  LOG_CLI((BSL_META_U(unit,
                      "is_data_size_bits_256_not_32: %u\n\r"),info->is_data_size_bits_256_not_32));
  LOG_CLI((BSL_META_U(unit,
                      "is_infinite_nof_actions: %u\n\r"),info->is_infinite_nof_actions));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_DIAG_DRAM_READ_COMPARE_STATUS_print(
    SOC_SAND_IN SOC_TMC_DIAG_DRAM_READ_COMPARE_STATUS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "success: %u\n\r"),info->success));
  LOG_CLI((BSL_META_U(unit,
                      "error_bits_global: %u\n\r"),info->error_bits_global));
  LOG_CLI((BSL_META_U(unit,
                      "nof_addr_with_errors: %u\n\r"),info->nof_addr_with_errors));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_DIAG_DLL_STATUS_INFO_print(
    SOC_SAND_IN SOC_TMC_DIAG_DLL_STATUS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "DDL control 0                         : 0x%08X\n\r"),info->ddl_control_0));
  LOG_CLI((BSL_META_U(unit,
                      "DDL control 1                         : 0x%08X\n\r"),info->ddl_control_1));
  LOG_CLI((BSL_META_U(unit,
                      "DDL control 2                         : 0x%08X\n\r"),info->ddl_control_2));
  LOG_CLI((BSL_META_U(unit,
                      "DLL finished initialization           : %u\n\r"),info->dll_init_done));
  LOG_CLI((BSL_META_U(unit,
                      "Round trip delay value                : 0x%04X\n\r"), info->rnd_trp));
  LOG_CLI((BSL_META_U(unit,
                      "Round trip diff delay value           : 0x%04X\n\r"), info->rnd_trp_diff));
  LOG_CLI((BSL_META_U(unit,
                      "Phase selection down indication       : 0x%04X\n\r"), info->dll_ph_dn));
  LOG_CLI((BSL_META_U(unit,
                      "Phase selection up indication         : 0x%04X\n\r"), info->dll_ph_up));
  LOG_CLI((BSL_META_U(unit,
                      "Output selected phase                 : 0x%04X\n\r"), info->main_ph_sel));
  LOG_CLI((BSL_META_U(unit,
                      "Phase selected for sync clock 2       : 0x%04X\n\r"), info->ph2sel));
  LOG_CLI((BSL_META_U(unit,
                      "Half cycle count vector               : 0x%04X\n\r"), info->hc_sel_vec));
  LOG_CLI((BSL_META_U(unit,
                      "Quarter cycle count vector            : 0x%04X\n\r"), info->qc_sel_vec));
  LOG_CLI((BSL_META_U(unit,
                      "Count vector                          : 0x%04X\n\r"), info->sel_vec));
  LOG_CLI((BSL_META_U(unit,
                      "Half granularity indication           : %u\n\r"),info->sel_hg));
  LOG_CLI((BSL_META_U(unit,
                      "Phase selected for half cycle up      : %u\n\r"),info->ph_sel_hc_up));
  LOG_CLI((BSL_META_U(unit,
                      "Insertion delay compensation vector   : 0x%04X\n\r"), info->ins_dly_min_vec));
  LOG_CLI((BSL_META_U(unit,
                      "Phase selection offset                : %u\n\r"),info->ddl_init_main_ph_sel_ofst));
  LOG_CLI((BSL_META_U(unit,
                      "DDL phase selected for half cycle up  : %u\n\r"),info->ddl_ph_sel_hc_up));
  LOG_CLI((BSL_META_U(unit,
                      "DDL train trigger up limit            : %u\n\r"),info->ddl_train_trig_up_limit));
  LOG_CLI((BSL_META_U(unit,
                      "DDL train trigger down limit          : %u\n\r"),info->ddl_train_trig_dn_limit));
  LOG_CLI((BSL_META_U(unit,
                      "Phase selection error                 : %u\n\r"),info->ph_sel_err));
  LOG_CLI((BSL_META_U(unit,
                      "Delay max min mode                    : %u\n\r"),info->dly_max_min_mode));
  LOG_CLI((BSL_META_U(unit,
                      "Phase selected                        : 0x%04X\n\r"), info->ph_sel));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_DIAG_DRAM_STATUS_INFO_print(
    SOC_SAND_IN SOC_TMC_DIAG_DRAM_STATUS_INFO *info
  )
{
  uint32
    ind,
    ind2;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "\n\r")));
  LOG_CLI((BSL_META_U(unit,
                      "DRAM training sequence control        : 0x%08X\n\r"),info->training_seq));
  LOG_CLI((BSL_META_U(unit,
                      "Io calibration status                 : 0x%08X\n\r"),info->calibration_st));
  LOG_CLI((BSL_META_U(unit,
                      "DDL periodic training                 : 0x%08X\n\r"),info->ddl_periodic_training));
  LOG_CLI((BSL_META_U(unit,
                      "DLL master control vector             : 0x%04X\n\r"), info->dll_mstr_s));
  LOG_CLI((BSL_META_U(unit,
                      "\n\r")));

  for (ind=0; ind<SOC_TMC_DIAG_NOF_DDR_TRAIN_SEQS; ++ind)
  {
    ind2 = SOC_TMC_DIAG_NOF_DDR_TRAIN_SEQS - ind - 1;
    LOG_CLI((BSL_META_U(unit,
                        "DDR training sequence[%u]              : 0x%08X\n\r"),ind2,info->ddr_training_sequence[ind2]));
  }

  LOG_CLI((BSL_META_U(unit,
                      "\n\r")));
  LOG_CLI((BSL_META_U(unit,
                      "--PER-DLL STATUS--\n\r")));
  LOG_CLI((BSL_META_U(unit,
                      "\n\r")));

  for (ind=0; ind<SOC_TMC_DIAG_NOF_DLLS_PER_DRAM; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "--DLL %u--\n\r"),ind));
    SOC_TMC_DIAG_DLL_STATUS_INFO_print(&(info->dll_status[ind]));
    LOG_CLI((BSL_META_U(unit,
                        "\n\r")));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_DIAG_DRAM_ERR_INFO_print(
    SOC_SAND_IN  SOC_TMC_DIAG_DRAM_ERR_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "bit_err_bitmap: %u\n\r"),info->bit_err_bitmap));
  LOG_CLI((BSL_META_U(unit,
                      "is_clocking_err: %u\n\r"),info->is_clocking_err));
  for (ind = 0; ind < DIAG_DRAM_NOF_DQSS; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "is_dqs_con_err[%u]: %u\n\r"),ind,info->is_dqs_con_err[ind]));
  }
  LOG_CLI((BSL_META_U(unit,
                      "is_phy_ready_err: %u\n\r"),info->is_phy_ready_err));
  LOG_CLI((BSL_META_U(unit,
                      "is_rtt_avg_min_err: %u\n\r"),info->is_rtt_avg_min_err));
  LOG_CLI((BSL_META_U(unit,
                      "is_rtt_avg_max_err: %u\n\r"),info->is_rtt_avg_max_err));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_DIAG_SOFT_ERR_INFO_print(
    SOC_SAND_IN  SOC_TMC_DIAG_SOFT_ERR_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "pattern %s "), SOC_TMC_DIAG_SOFT_ERROR_PATTERN_to_string(info->pattern)));
  LOG_CLI((BSL_META_U(unit,
                      "sms: %u\n\r"),info->sms));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_DIAG_SOFT_ERR_RESULT_print(
    SOC_SAND_IN  SOC_TMC_DIAG_SOFT_ERR_RESULT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "err_sp: %u, "),info->err_sp));
  LOG_CLI((BSL_META_U(unit,
                      "err_dp: %u, "),info->err_dp));
  LOG_CLI((BSL_META_U(unit,
                      "err_rf: %u\n\r"),info->err_rf));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_DIAG_SOFT_SMS_RESULT_print(
    SOC_SAND_IN  uint32                  sms_ndx,
    SOC_SAND_IN  uint32                  sone_ndx,
    SOC_SAND_IN  SOC_TMC_DIAG_SOFT_SMS_RESULT *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);


exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 



void
  SOC_TMC_DIAG_LAST_PACKET_INFO_clear(
    SOC_SAND_OUT SOC_TMC_DIAG_LAST_PACKET_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_DIAG_LAST_PACKET_INFO));
  info->tm_port = 0;
  info->pp_port = 0;
  info->is_valid = FALSE;
  for (ind = 0; ind < SOC_TMC_DIAG_LAST_PCKT_SNAPSHOT_LEN_BYTES_MAX; ++ind)
  {
    info->buffer[ind] = 0;
  }
  info->packet_size = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_TMC_DEBUG_IS_LVL1

void
  SOC_TMC_DIAG_LAST_PACKET_INFO_print(
    SOC_SAND_IN  SOC_TMC_DIAG_LAST_PACKET_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "tm_port: %u\n\r"),info->tm_port));
  LOG_CLI((BSL_META_U(unit,
                      "pp_port: %u\n\r"),info->pp_port));
  for (ind = 0; ind < SOC_TMC_DIAG_LAST_PCKT_SNAPSHOT_LEN_BYTES_MAX; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "buffer[%u]: %08x\n\r"), ind,info->buffer[ind]));
  }
  LOG_CLI((BSL_META_U(unit,
                      "packet_size: %d\n\r"), info->packet_size));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 

#include <soc/dpp/SAND/Utils/sand_footer.h>

