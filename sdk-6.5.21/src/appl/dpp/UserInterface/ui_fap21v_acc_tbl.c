/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#include <appl/diag/dpp/ref_sys.h>
#include <bcm_app/dpp/../H/usrApp.h>

#include <soc/dpp/SOC_SAND_FAP21V/fap21v_framework.h>
#include <soc/dpp/SOC_SAND_FAP21V/fap21v_chip_tbls.h>
#include <soc/dpp/SOC_SAND_FAP21V/fap21v_tbl_access.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/SAND/SAND_FM/sand_mem_access.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>
#include <appl/diag/dpp/utils_defi.h>
#include <appl/dpp/UserInterface/ui_defi.h>
#include <appl/dpp/UserInterface/ui_pure_defi_fap21v_acc.h>

extern
  uint32
    Default_unit;

STATIC uint32
  get_default_unit()
{
  return Default_unit;
}

int
  ui_fap21v_acc_ind_ips_queue_type_lookup_table_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ind_ips_queue_type_lookup_table_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_ips_queue_type_lookup_table_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_QUEUE_TYPE_LOOKUP_TABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_QUEUE_TYPE_LOOKUP_TABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.queue_type_lookup_table = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_ips_queue_type_lookup_table_tbl_set_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_ips_queue_type_lookup_table_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ind_ips_queue_type_lookup_table_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_ips_queue_type_lookup_table_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "queue_type_lookup_table: %x\n", data.queue_type_lookup_table);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_ips_queue_priority_map_select_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ind_ips_queue_priority_map_select_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_ips_queue_priority_map_select_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_QUEUE_PRIORITY_MAP_SELECT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_QUEUE_PRIORITY_MAP_SELECT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.queue_priority_map_select = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_ips_queue_priority_map_select_tbl_set_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_ips_queue_priority_map_select_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ind_ips_queue_priority_map_select_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_ips_queue_priority_map_select_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "queue_priority_map_select: %x\n", data.queue_priority_map_select);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_ips_credit_balance_based_thresholds_table_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ind_ips_credit_balance_based_thresholds_table_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_ips_credit_balance_based_thresholds_table_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_BACKOFF_ENTER_QCR_BAL_TH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_BACKOFF_ENTER_QCR_BAL_TH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.backoff_enter_qcr_bal_th = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_BACKOFF_EXIT_QCR_BAL_TH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_BACKOFF_EXIT_QCR_BAL_TH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.backoff_exit_qcr_bal_th = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_BACKLOG_ENTER_QCR_BAL_TH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_BACKLOG_ENTER_QCR_BAL_TH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.backlog_enter_qcr_bal_th = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_BACKLOG_EXIT_QCR_BAL_TH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_BACKLOG_EXIT_QCR_BAL_TH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.backlog_exit_qcr_bal_th = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_ips_credit_balance_based_thresholds_table_tbl_set_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_ips_credit_balance_based_thresholds_table_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ind_ips_credit_balance_based_thresholds_table_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_ips_credit_balance_based_thresholds_table_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "backoff_enter_qcr_bal_th: %x\n", data.backoff_enter_qcr_bal_th);
  soc_sand_os_printf( "backoff_exit_qcr_bal_th: %x\n", data.backoff_exit_qcr_bal_th);
  soc_sand_os_printf( "backlog_enter_qcr_bal_th: %x\n", data.backlog_enter_qcr_bal_th);
  soc_sand_os_printf( "backlog_exit_qcr_bal_th: %x\n", data.backlog_exit_qcr_bal_th);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_ips_queue_descriptor_table_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ind_ips_queue_descriptor_table_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_ips_queue_descriptor_table_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_CR_BAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_CR_BAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.cr_bal = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_CRS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_CRS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.crs = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_IN_DQCQ_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_IN_DQCQ_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.in_dqcq = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_ONE_PKT_DEQ_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_ONE_PKT_DEQ_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.one_pkt_deq = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_ips_queue_descriptor_table_tbl_set_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_ips_queue_descriptor_table_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ind_ips_queue_descriptor_table_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_ips_queue_descriptor_table_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "cr_bal: %x\n", data.cr_bal);
  soc_sand_os_printf( "crs: %x\n", data.crs);
  soc_sand_os_printf( "in_dqcq: %x\n", data.in_dqcq);
  soc_sand_os_printf( "one_pkt_deq: %x\n", data.one_pkt_deq);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_ips_destination_device_and_port_lookup_table_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ind_ips_destination_device_and_port_lookup_table_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_ips_destination_device_and_port_lookup_table_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_DEST_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_DEST_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.dest_port = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_DEST_DEV_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_DEST_DEV_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.dest_dev = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_ips_destination_device_and_port_lookup_table_tbl_set_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_ips_destination_device_and_port_lookup_table_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ind_ips_destination_device_and_port_lookup_table_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_ips_destination_device_and_port_lookup_table_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "dest_port: %x\n", data.dest_port);
  soc_sand_os_printf( "dest_dev: %x\n", data.dest_dev);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_ips_queue_size_table_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_IPS_QUEUE_SIZE_TABLE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ind_ips_queue_size_table_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_SIZE_TABLE_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_SIZE_TABLE_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_ips_queue_size_table_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_SIZE_TABLE_TBL_EXPONENT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_SIZE_TABLE_TBL_EXPONENT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.exponent = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_SIZE_TABLE_TBL_MANTISSA_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_SIZE_TABLE_TBL_MANTISSA_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.mantissa = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_SIZE_TABLE_TBL_QSIZE_4B_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_SIZE_TABLE_TBL_QSIZE_4B_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.qsize_4b = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_ips_queue_size_table_tbl_set_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_ips_queue_size_table_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_IPS_QUEUE_SIZE_TABLE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ind_ips_queue_size_table_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_SIZE_TABLE_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_SIZE_TABLE_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_ips_queue_size_table_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "exponent: %x\n", data.exponent);
  soc_sand_os_printf( "mantissa: %x\n", data.mantissa);
  soc_sand_os_printf( "qsize_4b: %x\n", data.qsize_4b);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_ips_empty_queue_credit_balance_table_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ind_ips_empty_queue_credit_balance_table_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_ips_empty_queue_credit_balance_table_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_EMPTY_QSATISFIED_CR_BAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_EMPTY_QSATISFIED_CR_BAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.empty_qsatisfied_cr_bal = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_MAX_EMPTY_QCR_BAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_MAX_EMPTY_QCR_BAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.max_empty_qcr_bal = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_EXCEED_MAX_EMPTY_QCR_BAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_EXCEED_MAX_EMPTY_QCR_BAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.exceed_max_empty_qcr_bal = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_ips_empty_queue_credit_balance_table_tbl_set_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_ips_empty_queue_credit_balance_table_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ind_ips_empty_queue_credit_balance_table_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_ips_empty_queue_credit_balance_table_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "empty_qsatisfied_cr_bal: %x\n", data.empty_qsatisfied_cr_bal);
  soc_sand_os_printf( "max_empty_qcr_bal: %x\n", data.max_empty_qcr_bal);
  soc_sand_os_printf( "exceed_max_empty_qcr_bal: %x\n", data.exceed_max_empty_qcr_bal);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_ips_system_red_max_queue_size_table_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ind_ips_system_red_max_queue_size_table_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_ips_system_red_max_queue_size_table_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_MAXQSZ_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_MAXQSZ_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.maxqsz = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_MAXQSZ_AGE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_MAXQSZ_AGE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.maxqsz_age = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_ips_system_red_max_queue_size_table_tbl_set_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_ips_system_red_max_queue_size_table_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ind_ips_system_red_max_queue_size_table_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_ips_system_red_max_queue_size_table_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "maxqsz: %x\n", data.maxqsz);
  soc_sand_os_printf( "maxqsz_age: %x\n", data.maxqsz_age);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_ips_queue_size_based_thresholds_table_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ind_ips_queue_size_based_thresholds_table_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_ips_queue_size_based_thresholds_table_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_OFF_TO_SLOW_MSG_TH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_OFF_TO_SLOW_MSG_TH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.off_to_slow_msg_th = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_OFF_TO_NORM_MSG_TH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_OFF_TO_NORM_MSG_TH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.off_to_norm_msg_th = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_SLOW_TO_NORM_MSG_TH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_SLOW_TO_NORM_MSG_TH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.slow_to_norm_msg_th = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_NORM_TO_SLOW_MSG_TH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_NORM_TO_SLOW_MSG_TH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.norm_to_slow_msg_th = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_FSM_TH_MUL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_FSM_TH_MUL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.fsm_th_mul = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_ips_queue_size_based_thresholds_table_tbl_set_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_ips_queue_size_based_thresholds_table_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ind_ips_queue_size_based_thresholds_table_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_ips_queue_size_based_thresholds_table_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "off_to_slow_msg_th: %x\n", data.off_to_slow_msg_th);
  soc_sand_os_printf( "off_to_norm_msg_th: %x\n", data.off_to_norm_msg_th);
  soc_sand_os_printf( "slow_to_norm_msg_th: %x\n", data.slow_to_norm_msg_th);
  soc_sand_os_printf( "norm_to_slow_msg_th: %x\n", data.norm_to_slow_msg_th);
  soc_sand_os_printf( "fsm_th_mul: %x\n", data.fsm_th_mul);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_ips_system_physical_port_lookup_table_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ind_ips_system_physical_port_lookup_table_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_ips_system_physical_port_lookup_table_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_SYS_PHY_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_SYS_PHY_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.sys_phy_port = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_ips_system_physical_port_lookup_table_tbl_set_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_ips_system_physical_port_lookup_table_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ind_ips_system_physical_port_lookup_table_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_ips_system_physical_port_lookup_table_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "sys_phy_port: %x\n", data.sys_phy_port);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_ips_queue_priority_maps_table_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ind_ips_queue_priority_maps_table_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_ips_queue_priority_maps_table_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_QUEUE_PRIORITY_MAPS_TABLE_0_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_QUEUE_PRIORITY_MAPS_TABLE_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.queue_priority_maps_table[0] = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_QUEUE_PRIORITY_MAPS_TABLE_1_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_QUEUE_PRIORITY_MAPS_TABLE_1_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.queue_priority_maps_table[1] = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_ips_queue_priority_maps_table_tbl_set_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_ips_queue_priority_maps_table_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ind_ips_queue_priority_maps_table_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_ips_queue_priority_maps_table_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "queue_priority_maps_table: %x\n", data.queue_priority_maps_table);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_ips_flow_status_message_request_queue_control_table_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_IPS_FLOW_STATUS_MESSAGE_REQUEST_QUEUE_CONTROL_TABLE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ind_ips_flow_status_message_request_queue_control_table_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_FLOW_STATUS_MESSAGE_REQUEST_QUEUE_CONTROL_TABLE_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_IPS_FLOW_STATUS_MESSAGE_REQUEST_QUEUE_CONTROL_TABLE_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_ips_flow_status_message_request_queue_control_table_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_FLOW_STATUS_MESSAGE_REQUEST_QUEUE_CONTROL_TABLE_TBL_FSMRQ_CTRL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_IPS_FLOW_STATUS_MESSAGE_REQUEST_QUEUE_CONTROL_TABLE_TBL_FSMRQ_CTRL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.fsmrq_ctrl = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_ips_flow_status_message_request_queue_control_table_tbl_set_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_ips_flow_status_message_request_queue_control_table_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_IPS_FLOW_STATUS_MESSAGE_REQUEST_QUEUE_CONTROL_TABLE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ind_ips_flow_status_message_request_queue_control_table_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_FLOW_STATUS_MESSAGE_REQUEST_QUEUE_CONTROL_TABLE_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_IPS_FLOW_STATUS_MESSAGE_REQUEST_QUEUE_CONTROL_TABLE_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_ips_flow_status_message_request_queue_control_table_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "fsmrq_ctrl: %x\n", data.fsmrq_ctrl);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_ips_flow_id_lookup_table_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_IPS_FLOW_ID_LOOKUP_TABLE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ind_ips_flow_id_lookup_table_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_FLOW_ID_LOOKUP_TABLE_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_IPS_FLOW_ID_LOOKUP_TABLE_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_ips_flow_id_lookup_table_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_FLOW_ID_LOOKUP_TABLE_TBL_BASE_FLOW_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_IPS_FLOW_ID_LOOKUP_TABLE_TBL_BASE_FLOW_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.base_flow = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_FLOW_ID_LOOKUP_TABLE_TBL_SUB_FLOW_MODE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_IPS_FLOW_ID_LOOKUP_TABLE_TBL_SUB_FLOW_MODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.sub_flow_mode = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_ips_flow_id_lookup_table_tbl_set_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_ips_flow_id_lookup_table_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_IPS_FLOW_ID_LOOKUP_TABLE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ind_ips_flow_id_lookup_table_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_FLOW_ID_LOOKUP_TABLE_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_IPS_FLOW_ID_LOOKUP_TABLE_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_ips_flow_id_lookup_table_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "base_flow: %x\n", data.base_flow);
  soc_sand_os_printf( "sub_flow_mode: %x\n", data.sub_flow_mode);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_sch_scheduler_enable_memory_sem_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_SCH_SCHEDULER_ENABLE_MEMORY_SEM_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_sch");
  soc_sand_proc_name = "ui_fap21v_acc_ind_sch_scheduler_enable_memory_sem_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_SCHEDULER_ENABLE_MEMORY_SEM_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_SCHEDULER_ENABLE_MEMORY_SEM_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_sch_scheduler_enable_memory_sem_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_SCHEDULER_ENABLE_MEMORY_SEM_TBL_SCH_ENA_8_ELEMENTS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_SCHEDULER_ENABLE_MEMORY_SEM_TBL_SCH_ENA_8_ELEMENTS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.sch_ena_8_elements = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_sch_scheduler_enable_memory_sem_tbl_set_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_sch_scheduler_enable_memory_sem_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_SCH_SCHEDULER_ENABLE_MEMORY_SEM_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_sch");
  soc_sand_proc_name = "ui_fap21v_acc_ind_sch_scheduler_enable_memory_sem_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_SCHEDULER_ENABLE_MEMORY_SEM_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_SCHEDULER_ENABLE_MEMORY_SEM_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_sch_scheduler_enable_memory_sem_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "sch_ena_8_elements: %x\n", data.sch_ena_8_elements);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_sch_port_queue_size_pqs_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_SCH_PORT_QUEUE_SIZE_PQS_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_sch");
  soc_sand_proc_name = "ui_fap21v_acc_ind_sch_port_queue_size_pqs_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_PORT_QUEUE_SIZE_PQS_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_PORT_QUEUE_SIZE_PQS_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_sch_port_queue_size_pqs_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_PORT_QUEUE_SIZE_PQS_TBL_MAX_QSZ_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_PORT_QUEUE_SIZE_PQS_TBL_MAX_QSZ_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.max_qsz = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_PORT_QUEUE_SIZE_PQS_TBL_FLOW_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_PORT_QUEUE_SIZE_PQS_TBL_FLOW_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.flow_id = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_PORT_QUEUE_SIZE_PQS_TBL_AGING_BIT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_PORT_QUEUE_SIZE_PQS_TBL_AGING_BIT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.aging_bit = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_sch_port_queue_size_pqs_tbl_set_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_sch_port_queue_size_pqs_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_SCH_PORT_QUEUE_SIZE_PQS_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_sch");
  soc_sand_proc_name = "ui_fap21v_acc_ind_sch_port_queue_size_pqs_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_PORT_QUEUE_SIZE_PQS_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_PORT_QUEUE_SIZE_PQS_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_sch_port_queue_size_pqs_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "max_qsz: %x\n", data.max_qsz);
  soc_sand_os_printf( "flow_id: %x\n", data.flow_id);
  soc_sand_os_printf( "aging_bit: %x\n", data.aging_bit);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_sch_flow_sub_flow_fsf_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_SCH_FLOW_SUB_FLOW_FSF_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_sch");
  soc_sand_proc_name = "ui_fap21v_acc_ind_sch_flow_sub_flow_fsf_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_FLOW_SUB_FLOW_FSF_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_FLOW_SUB_FLOW_FSF_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_sch_flow_sub_flow_fsf_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_FLOW_SUB_FLOW_FSF_TBL_SFENA_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_FLOW_SUB_FLOW_FSF_TBL_SFENA_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.sfena = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_sch_flow_sub_flow_fsf_tbl_set_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_sch_flow_sub_flow_fsf_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_SCH_FLOW_SUB_FLOW_FSF_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_sch");
  soc_sand_proc_name = "ui_fap21v_acc_ind_sch_flow_sub_flow_fsf_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_FLOW_SUB_FLOW_FSF_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_FLOW_SUB_FLOW_FSF_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_sch_flow_sub_flow_fsf_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "sfena: %x\n", data.sfena);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_sch_flow_group_memory_fgm_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_SCH_GROUP_MEMORY_FGM_TBL_DATA
    data;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_sch");
  soc_sand_proc_name = "ui_fap21v_acc_ind_sch_flow_group_memory_fgm_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_FLOW_GROUP_MEMORY_FGM_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_FLOW_GROUP_MEMORY_FGM_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_sch_group_memory_fgm_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_FLOW_GROUP_MEMORY_FGM_TBL_FLOW_GROUP_0_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_FLOW_GROUP_MEMORY_FGM_TBL_FLOW_GROUP_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.flow_group[0] = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_FLOW_GROUP_MEMORY_FGM_TBL_FLOW_GROUP_1_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_FLOW_GROUP_MEMORY_FGM_TBL_FLOW_GROUP_1_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.flow_group[1] = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_FLOW_GROUP_MEMORY_FGM_TBL_FLOW_GROUP_2_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_FLOW_GROUP_MEMORY_FGM_TBL_FLOW_GROUP_2_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.flow_group[2] = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_FLOW_GROUP_MEMORY_FGM_TBL_FLOW_GROUP_3_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_FLOW_GROUP_MEMORY_FGM_TBL_FLOW_GROUP_3_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.flow_group[3] = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_FLOW_GROUP_MEMORY_FGM_TBL_FLOW_GROUP_4_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_FLOW_GROUP_MEMORY_FGM_TBL_FLOW_GROUP_4_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.flow_group[4] = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_FLOW_GROUP_MEMORY_FGM_TBL_FLOW_GROUP_5_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_FLOW_GROUP_MEMORY_FGM_TBL_FLOW_GROUP_5_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.flow_group[5] = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_FLOW_GROUP_MEMORY_FGM_TBL_FLOW_GROUP_6_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_FLOW_GROUP_MEMORY_FGM_TBL_FLOW_GROUP_6_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.flow_group[6] = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_FLOW_GROUP_MEMORY_FGM_TBL_FLOW_GROUP_7_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_FLOW_GROUP_MEMORY_FGM_TBL_FLOW_GROUP_7_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.flow_group[7] = (uint32)param_val->value.ulong_value;
  }

  ret = fap21v_sch_group_memory_fgm_tbl_set_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_sch_flow_group_memory_fgm_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_SCH_GROUP_MEMORY_FGM_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_sch");
  soc_sand_proc_name = "ui_fap21v_acc_ind_sch_flow_group_memory_fgm_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_FLOW_GROUP_MEMORY_FGM_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_FLOW_GROUP_MEMORY_FGM_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_sch_group_memory_fgm_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "flow_group[0]: %x\n", data.flow_group[0]);
  soc_sand_os_printf( "flow_group[1]: %x\n", data.flow_group[1]);
  soc_sand_os_printf( "flow_group[2]: %x\n", data.flow_group[2]);
  soc_sand_os_printf( "flow_group[3]: %x\n", data.flow_group[3]);
  soc_sand_os_printf( "flow_group[4]: %x\n", data.flow_group[4]);
  soc_sand_os_printf( "flow_group[5]: %x\n", data.flow_group[5]);
  soc_sand_os_printf( "flow_group[6]: %x\n", data.flow_group[6]);
  soc_sand_os_printf( "flow_group[7]: %x\n", data.flow_group[7]);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_sch_force_status_message_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_SCH_FORCE_STATUS_MESSAGE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_sch");
  soc_sand_proc_name = "ui_fap21v_acc_ind_sch_force_status_message_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_FORCE_STATUS_MESSAGE_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_FORCE_STATUS_MESSAGE_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_sch_force_status_message_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_FORCE_STATUS_MESSAGE_TBL_MESSAGE_FLOW_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_FORCE_STATUS_MESSAGE_TBL_MESSAGE_FLOW_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.message_flow_id = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_FORCE_STATUS_MESSAGE_TBL_MESSAGE_TYPE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_FORCE_STATUS_MESSAGE_TBL_MESSAGE_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.message_type = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_FORCE_STATUS_MESSAGE_TBL_MESSAGE_GROUP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_FORCE_STATUS_MESSAGE_TBL_MESSAGE_GROUP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.message_group = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_FORCE_STATUS_MESSAGE_TBL_MESSAGE_TO_SHAPER_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_FORCE_STATUS_MESSAGE_TBL_MESSAGE_TO_SHAPER_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.message_to_shaper = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_sch_force_status_message_tbl_set_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_sch_force_status_message_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_SCH_FORCE_STATUS_MESSAGE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_sch");
  soc_sand_proc_name = "ui_fap21v_acc_ind_sch_force_status_message_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_FORCE_STATUS_MESSAGE_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_FORCE_STATUS_MESSAGE_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_sch_force_status_message_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "message_flow_id: %x\n", data.message_flow_id);
  soc_sand_os_printf( "message_type: %x\n", data.message_type);
  soc_sand_os_printf( "message_group: %x\n", data.message_group);
  soc_sand_os_printf( "message_to_shaper: %x\n", data.message_to_shaper);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_sch_cl_schedulers_configuration_scc_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_SCH_CL_SCHEDULERS_CONFIGURATION_SCC_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_sch");
  soc_sand_proc_name = "ui_fap21v_acc_ind_sch_cl_schedulers_configuration_scc_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_CL_SCHEDULERS_CONFIGURATION_SCC_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_CL_SCHEDULERS_CONFIGURATION_SCC_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_sch_cl_schedulers_configuration_scc_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_CL_SCHEDULERS_CONFIGURATION_SCC_TBL_CLSCH_TYPE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_CL_SCHEDULERS_CONFIGURATION_SCC_TBL_CLSCH_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.clsch_type = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_sch_cl_schedulers_configuration_scc_tbl_set_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_sch_cl_schedulers_configuration_scc_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_SCH_CL_SCHEDULERS_CONFIGURATION_SCC_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_sch");
  soc_sand_proc_name = "ui_fap21v_acc_ind_sch_cl_schedulers_configuration_scc_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_CL_SCHEDULERS_CONFIGURATION_SCC_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_CL_SCHEDULERS_CONFIGURATION_SCC_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_sch_cl_schedulers_configuration_scc_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "clsch_type: %x\n", data.clsch_type);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_sch_dsm_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_SCH_DSM_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_sch");
  soc_sand_proc_name = "ui_fap21v_acc_ind_sch_dsm_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_DSM_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_DSM_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_sch_dsm_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_DSM_TBL_DUAL_SHAPER_ENA_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_DSM_TBL_DUAL_SHAPER_ENA_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.dual_shaper_ena = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_sch_dsm_tbl_set_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_sch_dsm_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_SCH_DSM_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_sch");
  soc_sand_proc_name = "ui_fap21v_acc_ind_sch_dsm_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_DSM_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_DSM_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_sch_dsm_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "dual_shaper_ena: %x\n", data.dual_shaper_ena);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_sch_device_rate_memory_drm_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_SCH_DEVICE_RATE_MEMORY_DRM_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_sch");
  soc_sand_proc_name = "ui_fap21v_acc_ind_sch_device_rate_memory_drm_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_DEVICE_RATE_MEMORY_DRM_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_DEVICE_RATE_MEMORY_DRM_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_sch_device_rate_memory_drm_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_DEVICE_RATE_MEMORY_DRM_TBL_DEVICE_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_DEVICE_RATE_MEMORY_DRM_TBL_DEVICE_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.device_rate = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_sch_device_rate_memory_drm_tbl_set_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_sch_device_rate_memory_drm_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_SCH_DEVICE_RATE_MEMORY_DRM_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_sch");
  soc_sand_proc_name = "ui_fap21v_acc_ind_sch_device_rate_memory_drm_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_DEVICE_RATE_MEMORY_DRM_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_DEVICE_RATE_MEMORY_DRM_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_sch_device_rate_memory_drm_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "device_rate: %x\n", data.device_rate);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_sch_token_memory_controller_tmc_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_SCH_TOKEN_MEMORY_CONTROLLER_TMC_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_sch");
  soc_sand_proc_name = "ui_fap21v_acc_ind_sch_token_memory_controller_tmc_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_TOKEN_MEMORY_CONTROLLER_TMC_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_TOKEN_MEMORY_CONTROLLER_TMC_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_sch_token_memory_controller_tmc_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_TOKEN_MEMORY_CONTROLLER_TMC_TBL_TOKEN_COUNT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_TOKEN_MEMORY_CONTROLLER_TMC_TBL_TOKEN_COUNT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.token_count = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_TOKEN_MEMORY_CONTROLLER_TMC_TBL_SLOW_STATUS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_TOKEN_MEMORY_CONTROLLER_TMC_TBL_SLOW_STATUS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.slow_status = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_sch_token_memory_controller_tmc_tbl_set_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_sch_token_memory_controller_tmc_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_SCH_TOKEN_MEMORY_CONTROLLER_TMC_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_sch");
  soc_sand_proc_name = "ui_fap21v_acc_ind_sch_token_memory_controller_tmc_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_TOKEN_MEMORY_CONTROLLER_TMC_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_TOKEN_MEMORY_CONTROLLER_TMC_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_sch_token_memory_controller_tmc_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "token_count: %x\n", data.token_count);
  soc_sand_os_printf( "slow_status: %x\n", data.slow_status);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_sch_hr_scheduler_configuration_shc_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_SCH_HR_SCHEDULER_CONFIGURATION_SHC_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_sch");
  soc_sand_proc_name = "ui_fap21v_acc_ind_sch_hr_scheduler_configuration_shc_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_HR_SCHEDULER_CONFIGURATION_SHC_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_HR_SCHEDULER_CONFIGURATION_SHC_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_sch_hr_scheduler_configuration_shc_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_HR_SCHEDULER_CONFIGURATION_SHC_TBL_HRMODE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_HR_SCHEDULER_CONFIGURATION_SHC_TBL_HRMODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.hrmode = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_HR_SCHEDULER_CONFIGURATION_SHC_TBL_HRMASK_TYPE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_HR_SCHEDULER_CONFIGURATION_SHC_TBL_HRMASK_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.hrmask_type = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_sch_hr_scheduler_configuration_shc_tbl_set_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_sch_hr_scheduler_configuration_shc_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_SCH_HR_SCHEDULER_CONFIGURATION_SHC_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_sch");
  soc_sand_proc_name = "ui_fap21v_acc_ind_sch_hr_scheduler_configuration_shc_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_HR_SCHEDULER_CONFIGURATION_SHC_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_HR_SCHEDULER_CONFIGURATION_SHC_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_sch_hr_scheduler_configuration_shc_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "hrmode: %x\n", data.hrmode);
  soc_sand_os_printf( "hrmask_type: %x\n", data.hrmask_type);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_sch_flow_to_queue_mapping_fqm_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_SCH_FLOW_TO_QUEUE_MAPPING_FQM_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_sch");
  soc_sand_proc_name = "ui_fap21v_acc_ind_sch_flow_to_queue_mapping_fqm_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_FLOW_TO_QUEUE_MAPPING_FQM_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_FLOW_TO_QUEUE_MAPPING_FQM_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_sch_flow_to_queue_mapping_fqm_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_FLOW_TO_QUEUE_MAPPING_FQM_TBL_BASE_QUEUE_NUM_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_FLOW_TO_QUEUE_MAPPING_FQM_TBL_BASE_QUEUE_NUM_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.base_queue_num = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_FLOW_TO_QUEUE_MAPPING_FQM_TBL_SUB_FLOW_MODE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_FLOW_TO_QUEUE_MAPPING_FQM_TBL_SUB_FLOW_MODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.sub_flow_mode = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_FLOW_TO_QUEUE_MAPPING_FQM_TBL_FLOW_SLOW_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_FLOW_TO_QUEUE_MAPPING_FQM_TBL_FLOW_SLOW_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.flow_slow_enable = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_sch_flow_to_queue_mapping_fqm_tbl_set_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_sch_flow_to_queue_mapping_fqm_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_SCH_FLOW_TO_QUEUE_MAPPING_FQM_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_sch");
  soc_sand_proc_name = "ui_fap21v_acc_ind_sch_flow_to_queue_mapping_fqm_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_FLOW_TO_QUEUE_MAPPING_FQM_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_FLOW_TO_QUEUE_MAPPING_FQM_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_sch_flow_to_queue_mapping_fqm_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "base_queue_num: %x\n", data.base_queue_num);
  soc_sand_os_printf( "sub_flow_mode: %x\n", data.sub_flow_mode);
  soc_sand_os_printf( "flow_slow_enable: %x\n", data.flow_slow_enable);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_sch_fdms_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_SCH_FDMS_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_sch");
  soc_sand_proc_name = "ui_fap21v_acc_ind_sch_fdms_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_FDMS_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_FDMS_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_sch_fdms_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_FDMS_TBL_SCH_NUMBER_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_FDMS_TBL_SCH_NUMBER_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.sch_number = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_FDMS_TBL_COS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_FDMS_TBL_COS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.cos = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_FDMS_TBL_HRSEL_DUAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_FDMS_TBL_HRSEL_DUAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.hrsel_dual = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_sch_fdms_tbl_set_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_sch_fdms_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_SCH_FDMS_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_sch");
  soc_sand_proc_name = "ui_fap21v_acc_ind_sch_fdms_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_FDMS_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_FDMS_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_sch_fdms_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "sch_number: %x\n", data.sch_number);
  soc_sand_os_printf( "cos: %x\n", data.cos);
  soc_sand_os_printf( "hrsel_dual: %x\n", data.hrsel_dual);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_sch_scheduler_credit_generation_calendar_cal_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_sch");
  soc_sand_proc_name = "ui_fap21v_acc_ind_sch_scheduler_credit_generation_calendar_cal_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_sch_scheduler_credit_generation_calendar_cal_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_TBL_HRSEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_TBL_HRSEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.hrsel = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_sch_scheduler_credit_generation_calendar_cal_tbl_set_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_sch_scheduler_credit_generation_calendar_cal_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_sch");
  soc_sand_proc_name = "ui_fap21v_acc_ind_sch_scheduler_credit_generation_calendar_cal_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_sch_scheduler_credit_generation_calendar_cal_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "hrsel: %x\n", data.hrsel);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_sch_cl_schedulers_type_sct_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_SCH_CL_SCHEDULERS_TYPE_SCT_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_sch");
  soc_sand_proc_name = "ui_fap21v_acc_ind_sch_cl_schedulers_type_sct_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_CL_SCHEDULERS_TYPE_SCT_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_CL_SCHEDULERS_TYPE_SCT_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_sch_cl_schedulers_type_sct_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_CL_SCHEDULERS_TYPE_SCT_TBL_CLCONFIG_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_CL_SCHEDULERS_TYPE_SCT_TBL_CLCONFIG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.clconfig = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_CL_SCHEDULERS_TYPE_SCT_TBL_AF0_INV_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_CL_SCHEDULERS_TYPE_SCT_TBL_AF0_INV_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.af0_inv_weight = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_CL_SCHEDULERS_TYPE_SCT_TBL_AF1_INV_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_CL_SCHEDULERS_TYPE_SCT_TBL_AF1_INV_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.af1_inv_weight = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_CL_SCHEDULERS_TYPE_SCT_TBL_AF2_INV_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_CL_SCHEDULERS_TYPE_SCT_TBL_AF2_INV_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.af2_inv_weight = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_CL_SCHEDULERS_TYPE_SCT_TBL_AF3_INV_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_CL_SCHEDULERS_TYPE_SCT_TBL_AF3_INV_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.af3_inv_weight = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_CL_SCHEDULERS_TYPE_SCT_TBL_BINARY_FLLMODE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_CL_SCHEDULERS_TYPE_SCT_TBL_BINARY_FLLMODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.binary_fllmode = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_CL_SCHEDULERS_TYPE_SCT_TBL_CLASS_MODE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_CL_SCHEDULERS_TYPE_SCT_TBL_CLASS_MODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.class_mode = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_CL_SCHEDULERS_TYPE_SCT_TBL_ENH_CLEN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_CL_SCHEDULERS_TYPE_SCT_TBL_ENH_CLEN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.enh_clen = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_CL_SCHEDULERS_TYPE_SCT_TBL_ENH_CLSPHIGH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_CL_SCHEDULERS_TYPE_SCT_TBL_ENH_CLSPHIGH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.enh_clsphigh = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_sch_cl_schedulers_type_sct_tbl_set_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_sch_cl_schedulers_type_sct_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_SCH_CL_SCHEDULERS_TYPE_SCT_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_sch");
  soc_sand_proc_name = "ui_fap21v_acc_ind_sch_cl_schedulers_type_sct_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_CL_SCHEDULERS_TYPE_SCT_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_CL_SCHEDULERS_TYPE_SCT_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_sch_cl_schedulers_type_sct_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "clconfig: %x\n", data.clconfig);
  soc_sand_os_printf( "af0_inv_weight: %x\n", data.af0_inv_weight);
  soc_sand_os_printf( "af1_inv_weight: %x\n", data.af1_inv_weight);
  soc_sand_os_printf( "af2_inv_weight: %x\n", data.af2_inv_weight);
  soc_sand_os_printf( "af3_inv_weight: %x\n", data.af3_inv_weight);
  soc_sand_os_printf( "binary_fllmode: %x\n", data.binary_fllmode);
  soc_sand_os_printf( "class_mode: %x\n", data.class_mode);
  soc_sand_os_printf( "enh_clen: %x\n", data.enh_clen);
  soc_sand_os_printf( "enh_clsphigh: %x\n", data.enh_clsphigh);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_sch_shaper_descriptor_memory_static_shds_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDS_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_sch");
  soc_sand_proc_name = "ui_fap21v_acc_ind_sch_shaper_descriptor_memory_static_shds_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDS_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDS_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_sch_shaper_descriptor_memory_static_shds_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDS_TBL_PEAK_RATE_MAN_EVEN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDS_TBL_PEAK_RATE_MAN_EVEN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.peak_rate_man_even = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDS_TBL_PEAK_RATE_EXP_EVEN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDS_TBL_PEAK_RATE_EXP_EVEN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.peak_rate_exp_even = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDS_TBL_MAX_BURST_EVEN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDS_TBL_MAX_BURST_EVEN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.max_burst_even = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDS_TBL_SLOW_RATE2_SEL_EVEN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDS_TBL_SLOW_RATE2_SEL_EVEN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.slow_rate2_sel_even = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDS_TBL_PEAK_RATE_MAN_ODD_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDS_TBL_PEAK_RATE_MAN_ODD_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.peak_rate_man_odd = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDS_TBL_PEAK_RATE_EXP_ODD_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDS_TBL_PEAK_RATE_EXP_ODD_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.peak_rate_exp_odd = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDS_TBL_MAX_BURST_ODD_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDS_TBL_MAX_BURST_ODD_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.max_burst_odd = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDS_TBL_SLOW_RATE2_SEL_ODD_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDS_TBL_SLOW_RATE2_SEL_ODD_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.slow_rate2_sel_odd = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDS_TBL_MAX_BURST_UPDATE_EVEN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDS_TBL_MAX_BURST_UPDATE_EVEN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.max_burst_update_even = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDS_TBL_MAX_BURST_UPDATE_ODD_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDS_TBL_MAX_BURST_UPDATE_ODD_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.max_burst_update_odd = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_sch_shaper_descriptor_memory_static_shds_tbl_set_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_sch_shaper_descriptor_memory_static_shds_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDS_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_sch");
  soc_sand_proc_name = "ui_fap21v_acc_ind_sch_shaper_descriptor_memory_static_shds_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDS_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDS_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_sch_shaper_descriptor_memory_static_shds_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "peak_rate_man_even: %x\n", data.peak_rate_man_even);
  soc_sand_os_printf( "peak_rate_exp_even: %x\n", data.peak_rate_exp_even);
  soc_sand_os_printf( "max_burst_even: %x\n", data.max_burst_even);
  soc_sand_os_printf( "slow_rate2_sel_even: %x\n", data.slow_rate2_sel_even);
  soc_sand_os_printf( "peak_rate_man_odd: %x\n", data.peak_rate_man_odd);
  soc_sand_os_printf( "peak_rate_exp_odd: %x\n", data.peak_rate_exp_odd);
  soc_sand_os_printf( "max_burst_odd: %x\n", data.max_burst_odd);
  soc_sand_os_printf( "slow_rate2_sel_odd: %x\n", data.slow_rate2_sel_odd);
  soc_sand_os_printf( "max_burst_update_even: %x\n", data.max_burst_update_even);
  soc_sand_os_printf( "max_burst_update_odd: %x\n", data.max_burst_update_odd);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_sch_flow_to_fip_mapping_ffm_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_SCH_FLOW_TO_FIP_MAPPING_FFM_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_sch");
  soc_sand_proc_name = "ui_fap21v_acc_ind_sch_flow_to_fip_mapping_ffm_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_FLOW_TO_FIP_MAPPING_FFM_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_FLOW_TO_FIP_MAPPING_FFM_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_sch_flow_to_fip_mapping_ffm_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_FLOW_TO_FIP_MAPPING_FFM_TBL_DEVICE_NUMBER_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_FLOW_TO_FIP_MAPPING_FFM_TBL_DEVICE_NUMBER_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.device_number = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_sch_flow_to_fip_mapping_ffm_tbl_set_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_sch_flow_to_fip_mapping_ffm_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_SCH_FLOW_TO_FIP_MAPPING_FFM_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_sch");
  soc_sand_proc_name = "ui_fap21v_acc_ind_sch_flow_to_fip_mapping_ffm_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_FLOW_TO_FIP_MAPPING_FFM_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_FLOW_TO_FIP_MAPPING_FFM_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_sch_flow_to_fip_mapping_ffm_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "device_number: %x\n", data.device_number);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_sch_scheduler_init_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_SCH_SCHEDULER_INIT_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_sch");
  soc_sand_proc_name = "ui_fap21v_acc_ind_sch_scheduler_init_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_SCHEDULER_INIT_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_SCHEDULER_INIT_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_sch_scheduler_init_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_SCHEDULER_INIT_TBL_SCHINIT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_SCHEDULER_INIT_TBL_SCHINIT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.schinit = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_sch_scheduler_init_tbl_set_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_sch_scheduler_init_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_SCH_SCHEDULER_INIT_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_sch");
  soc_sand_proc_name = "ui_fap21v_acc_ind_sch_scheduler_init_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_SCHEDULER_INIT_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SCH_SCHEDULER_INIT_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_sch_scheduler_init_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "schinit: %x\n", data.schinit);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_mmu_sfm_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_MMU_SFM_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_mmu");
  soc_sand_proc_name = "ui_fap21v_acc_ind_mmu_sfm_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_MMU_SFM_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_MMU_SFM_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_mmu_sfm_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_MMU_SFM_TBL_MC_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_MMU_SFM_TBL_MC_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.mc_id = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_MMU_SFM_TBL_TC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_MMU_SFM_TBL_TC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.tc = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_MMU_SFM_TBL_TC_STAMP_ENA_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_MMU_SFM_TBL_TC_STAMP_ENA_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.tc_stamp_ena = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_MMU_SFM_TBL_DP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_MMU_SFM_TBL_DP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.dp = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_MMU_SFM_TBL_DP_STAMP_ENA_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_MMU_SFM_TBL_DP_STAMP_ENA_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.dp_stamp_ena = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_mmu_sfm_tbl_set_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_mmu_sfm_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_MMU_SFM_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_mmu");
  soc_sand_proc_name = "ui_fap21v_acc_ind_mmu_sfm_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_MMU_SFM_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_MMU_SFM_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_mmu_sfm_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "mc_id: %x\n", data.mc_id);
  soc_sand_os_printf( "tc: %x\n", data.tc);
  soc_sand_os_printf( "tc_stamp_ena: %x\n", data.tc_stamp_ena);
  soc_sand_os_printf( "dp: %x\n", data.dp);
  soc_sand_os_printf( "dp_stamp_ena: %x\n", data.dp_stamp_ena);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_mmu_iqs_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_MMU_IQS_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_mmu");
  soc_sand_proc_name = "ui_fap21v_acc_ind_mmu_iqs_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_MMU_IQS_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_MMU_IQS_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_mmu_iqs_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_MMU_IQS_TBL_ING_QSIG_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_MMU_IQS_TBL_ING_QSIG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.ing_qsig = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_mmu_iqs_tbl_set_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_mmu_iqs_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_MMU_IQS_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_mmu");
  soc_sand_proc_name = "ui_fap21v_acc_ind_mmu_iqs_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_MMU_IQS_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_MMU_IQS_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_mmu_iqs_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "ing_qsig: %x\n", data.ing_qsig);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_mmu_dram_addresses_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_MMU_DRAM_ADDRESSES_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_mmu");
  soc_sand_proc_name = "ui_fap21v_acc_ind_mmu_dram_addresses_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_MMU_DRAM_ADDRESSES_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_MMU_DRAM_ADDRESSES_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_mmu_dram_addresses_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_MMU_DRAM_ADDRESSES_TBL_DATA_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_MMU_DRAM_ADDRESSES_TBL_DATA_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.data = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_mmu_dram_addresses_tbl_set_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_mmu_dram_addresses_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_MMU_DRAM_ADDRESSES_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_mmu");
  soc_sand_proc_name = "ui_fap21v_acc_ind_mmu_dram_addresses_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_MMU_DRAM_ADDRESSES_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_MMU_DRAM_ADDRESSES_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_mmu_dram_addresses_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "data: %x\n", data.data);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_sla_out_of_band_flow_control_auxiliary_calendar_table_for_interface_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  FAP21V_SPI4_2_INTERFACE
    spi;
  uint32
    entry_offset;
  FAP21V_SLA_OUT_OF_BAND_FLOW_CONTROL_AUXILIARY_CALENDAR_TABLE_FOR_INTERFACE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_sla");
  soc_sand_proc_name = "ui_fap21v_acc_ind_sla_out_of_band_flow_control_auxiliary_calendar_table_for_interface_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SLA_OUT_OF_BAND_FLOW_CONTROL_AUXILIARY_CALENDAR_TABLE_FOR_INTERFACE_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SLA_OUT_OF_BAND_FLOW_CONTROL_AUXILIARY_CALENDAR_TABLE_FOR_INTERFACE_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SLA_OUT_OF_BAND_FLOW_CONTROL_AUXILIARY_CALENDAR_TABLE_FOR_INTERFACE_TBL_SPI_W_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_INDIRECT_SLA_OUT_OF_BAND_FLOW_CONTROL_AUXILIARY_CALENDAR_TABLE_FOR_INTERFACE_TBL_SPI_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    spi = (FAP21V_SPI4_2_INTERFACE)param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_sla_out_of_band_flow_control_auxiliary_calendar_table_for_interface_tbl_get_unsafe(
          unit,
          spi,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SLA_OUT_OF_BAND_FLOW_CONTROL_AUXILIARY_CALENDAR_TABLE_FOR_INTERFACE_TBL_OBFC_CAL_TBL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SLA_OUT_OF_BAND_FLOW_CONTROL_AUXILIARY_CALENDAR_TABLE_FOR_INTERFACE_TBL_OBFC_CAL_TBL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.obfc_cal_tbl = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_sla_out_of_band_flow_control_auxiliary_calendar_table_for_interface_tbl_set_unsafe(
          unit,
          spi,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_sla_out_of_band_flow_control_auxiliary_calendar_table_for_interface_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  FAP21V_SPI4_2_INTERFACE
    spi;
  uint32
    entry_offset;
  FAP21V_SLA_OUT_OF_BAND_FLOW_CONTROL_AUXILIARY_CALENDAR_TABLE_FOR_INTERFACE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_sla");
  soc_sand_proc_name = "ui_fap21v_acc_ind_sla_out_of_band_flow_control_auxiliary_calendar_table_for_interface_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SLA_OUT_OF_BAND_FLOW_CONTROL_AUXILIARY_CALENDAR_TABLE_FOR_INTERFACE_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SLA_OUT_OF_BAND_FLOW_CONTROL_AUXILIARY_CALENDAR_TABLE_FOR_INTERFACE_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SLA_OUT_OF_BAND_FLOW_CONTROL_AUXILIARY_CALENDAR_TABLE_FOR_INTERFACE_TBL_SPI_R_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_INDIRECT_SLA_OUT_OF_BAND_FLOW_CONTROL_AUXILIARY_CALENDAR_TABLE_FOR_INTERFACE_TBL_SPI_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    spi = (FAP21V_SPI4_2_INTERFACE)param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_sla_out_of_band_flow_control_auxiliary_calendar_table_for_interface_tbl_get_unsafe(
          unit,
          spi,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "obfc_cal_tbl: %x\n", data.obfc_cal_tbl);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_sla_destination_system_port_lut_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_SLA_DESTINATION_SYSTEM_PORT_LUT_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_sla");
  soc_sand_proc_name = "ui_fap21v_acc_ind_sla_destination_system_port_lut_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SLA_DESTINATION_SYSTEM_PORT_LUT_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SLA_DESTINATION_SYSTEM_PORT_LUT_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_sla_destination_system_port_lut_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SLA_DESTINATION_SYSTEM_PORT_LUT_DEST_SYS_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SLA_DESTINATION_SYSTEM_PORT_LUT_DEST_SYS_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.dest_sys_port = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_sla_destination_system_port_lut_tbl_set_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_sla_destination_system_port_lut_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_SLA_DESTINATION_SYSTEM_PORT_LUT_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_sla");
  soc_sand_proc_name = "ui_fap21v_acc_ind_sla_destination_system_port_lut_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SLA_DESTINATION_SYSTEM_PORT_LUT_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SLA_DESTINATION_SYSTEM_PORT_LUT_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_sla_destination_system_port_lut_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "dest_sys_port: %x\n", data.dest_sys_port);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_sla_tdm_multicast_table_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_SLA_TDM_MULTICAST_TABLE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_sla");
  soc_sand_proc_name = "ui_fap21v_acc_ind_sla_tdm_multicast_table_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SLA_TDM_MULTICAST_TABLE_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SLA_TDM_MULTICAST_TABLE_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_sla_tdm_multicast_table_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SLA_TDM_MULTICAST_TABLE_TBL_TMITBL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SLA_TDM_MULTICAST_TABLE_TBL_TMITBL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.tmitbl = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_sla_tdm_multicast_table_tbl_set_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_sla_tdm_multicast_table_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_SLA_TDM_MULTICAST_TABLE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_sla");
  soc_sand_proc_name = "ui_fap21v_acc_ind_sla_tdm_multicast_table_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SLA_TDM_MULTICAST_TABLE_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SLA_TDM_MULTICAST_TABLE_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_sla_tdm_multicast_table_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "tmitbl: %x\n", data.tmitbl);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_sla_egress_port_to_spi4_channel_table_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_SLA_EGRESS_PORT_TO_SPI4_CHANNEL_TABLE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_sla");
  soc_sand_proc_name = "ui_fap21v_acc_ind_sla_egress_port_to_spi4_channel_table_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SLA_EGRESS_PORT_TO_SPI4_CHANNEL_TABLE_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SLA_EGRESS_PORT_TO_SPI4_CHANNEL_TABLE_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_sla_egress_port_to_spi4_channel_table_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SLA_EGRESS_PORT_TO_SPI4_CHANNEL_TABLE_TBL_P2_CTBL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SLA_EGRESS_PORT_TO_SPI4_CHANNEL_TABLE_TBL_P2_CTBL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.p2_ctbl = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_sla_egress_port_to_spi4_channel_table_tbl_set_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_sla_egress_port_to_spi4_channel_table_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_SLA_EGRESS_PORT_TO_SPI4_CHANNEL_TABLE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_sla");
  soc_sand_proc_name = "ui_fap21v_acc_ind_sla_egress_port_to_spi4_channel_table_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SLA_EGRESS_PORT_TO_SPI4_CHANNEL_TABLE_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SLA_EGRESS_PORT_TO_SPI4_CHANNEL_TABLE_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_sla_egress_port_to_spi4_channel_table_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "p2_ctbl: %x\n", data.p2_ctbl);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_sla_spi4_channel_to_egress_port_table_for_interface_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  FAP21V_SPI4_2_INTERFACE
    spi;
  uint32
    entry_offset;
  FAP21V_SLA_SPI4_CHANNEL_TO_EGRESS_PORT_TABLE_FOR_INTERFACE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_sla");
  soc_sand_proc_name = "ui_fap21v_acc_ind_sla_spi4_channel_to_egress_port_table_for_interface_tbl***_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SLA_SPI4_CHANNEL_TO_EGRESS_PORT_TABLE_FOR_INTERFACE_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SLA_SPI4_CHANNEL_TO_EGRESS_PORT_TABLE_FOR_INTERFACE_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SLA_SPI4_CHANNEL_TO_EGRESS_PORT_TABLE_FOR_INTERFACE_TBL_SPI_W_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_INDIRECT_SLA_SPI4_CHANNEL_TO_EGRESS_PORT_TABLE_FOR_INTERFACE_TBL_SPI_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    spi = (FAP21V_SPI4_2_INTERFACE)param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_sla_spi4_channel_to_egress_port_table_for_interface_tbl_get_unsafe(
          unit,
          spi,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SLA_SPI4_CHANNEL_TO_EGRESS_PORT_TABLE_FOR_INTERFACE_TBL_C2_P_TBL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SLA_SPI4_CHANNEL_TO_EGRESS_PORT_TABLE_FOR_INTERFACE_TBL_C2_P_TBL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.c2_p_tbl = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_sla_spi4_channel_to_egress_port_table_for_interface_tbl_set_unsafe(
          unit,
          spi,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_sla_spi4_channel_to_egress_port_table_for_interface_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  FAP21V_SPI4_2_INTERFACE
    spi;
  uint32
    entry_offset;
  FAP21V_SLA_SPI4_CHANNEL_TO_EGRESS_PORT_TABLE_FOR_INTERFACE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_sla");
  soc_sand_proc_name = "ui_fap21v_acc_ind_sla_spi4_channel_to_egress_port_table_for_interface_tbl***_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SLA_SPI4_CHANNEL_TO_EGRESS_PORT_TABLE_FOR_INTERFACE_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SLA_SPI4_CHANNEL_TO_EGRESS_PORT_TABLE_FOR_INTERFACE_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SLA_SPI4_CHANNEL_TO_EGRESS_PORT_TABLE_FOR_INTERFACE_TBL_SPI_R_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_INDIRECT_SLA_SPI4_CHANNEL_TO_EGRESS_PORT_TABLE_FOR_INTERFACE_TBL_SPI_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    spi = (FAP21V_SPI4_2_INTERFACE)param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_sla_spi4_channel_to_egress_port_table_for_interface_tbl_get_unsafe(
          unit,
          spi,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "c2_p_tbl: %x\n", data.c2_p_tbl);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_spi_sink_status_channel_calendar_table_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  FAP21V_SPI4_2_INTERFACE
    spi;
  uint32
    entry_offset;
  FAP21V_SPI_SINK_STATUS_CHANNEL_CALENDAR_TABLE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_spi");
  soc_sand_proc_name = "ui_fap21v_acc_ind_spi_sink_status_channel_calendar_table_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SPI_SINK_STATUS_CHANNEL_CALENDAR_TABLE_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SPI_SINK_STATUS_CHANNEL_CALENDAR_TABLE_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SPI_SINK_STATUS_CHANNEL_CALENDAR_TABLE_TBL_SPI_W_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_INDIRECT_SPI_SINK_STATUS_CHANNEL_CALENDAR_TABLE_TBL_SPI_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    spi = (FAP21V_SPI4_2_INTERFACE)param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_spi_sink_status_channel_calendar_table_tbl_get_unsafe(
          unit,
          spi,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SPI_SINK_STATUS_CHANNEL_CALENDAR_TABLE_TBL_SNK_CAL_TBL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SPI_SINK_STATUS_CHANNEL_CALENDAR_TABLE_TBL_SNK_CAL_TBL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.snk_cal_tbl = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_spi_sink_status_channel_calendar_table_tbl_set_unsafe(
          unit,
          spi,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_spi_sink_status_channel_calendar_table_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  FAP21V_SPI4_2_INTERFACE
    spi;
  uint32
    entry_offset;
  FAP21V_SPI_SINK_STATUS_CHANNEL_CALENDAR_TABLE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_spi");
  soc_sand_proc_name = "ui_fap21v_acc_ind_spi_sink_status_channel_calendar_table_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SPI_SINK_STATUS_CHANNEL_CALENDAR_TABLE_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SPI_SINK_STATUS_CHANNEL_CALENDAR_TABLE_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SPI_SINK_STATUS_CHANNEL_CALENDAR_TABLE_TBL_SPI_R_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_INDIRECT_SPI_SINK_STATUS_CHANNEL_CALENDAR_TABLE_TBL_SPI_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    spi = (FAP21V_SPI4_2_INTERFACE)param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_spi_sink_status_channel_calendar_table_tbl_get_unsafe(
          unit,
          spi,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "snk_cal_tbl: %x\n", data.snk_cal_tbl);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_spi_source_status_channel_calendar_table_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  FAP21V_SPI4_2_INTERFACE
    spi;
  uint32
    entry_offset;
  FAP21V_SPI_SOURCE_STATUS_CHANNEL_CALENDAR_TABLE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_spi");
  soc_sand_proc_name = "ui_fap21v_acc_ind_spi_source_status_channel_calendar_table_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SPI_SOURCE_STATUS_CHANNEL_CALENDAR_TABLE_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SPI_SOURCE_STATUS_CHANNEL_CALENDAR_TABLE_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SPI_SOURCE_STATUS_CHANNEL_CALENDAR_TABLE_TBL_SPI_W_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_INDIRECT_SPI_SOURCE_STATUS_CHANNEL_CALENDAR_TABLE_TBL_SPI_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    spi = (FAP21V_SPI4_2_INTERFACE)param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_spi_source_status_channel_calendar_table_tbl_get_unsafe(
          unit,
          spi,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SPI_SOURCE_STATUS_CHANNEL_CALENDAR_TABLE_TBL_SRC_CAL_TBL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SPI_SOURCE_STATUS_CHANNEL_CALENDAR_TABLE_TBL_SRC_CAL_TBL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.src_cal_tbl = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_spi_source_status_channel_calendar_table_tbl_set_unsafe(
          unit,
          spi,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_spi_source_status_channel_calendar_table_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  FAP21V_SPI4_2_INTERFACE
    spi;
  uint32
    entry_offset;
  FAP21V_SPI_SOURCE_STATUS_CHANNEL_CALENDAR_TABLE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_spi");
  soc_sand_proc_name = "ui_fap21v_acc_ind_spi_source_status_channel_calendar_table_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SPI_SOURCE_STATUS_CHANNEL_CALENDAR_TABLE_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_SPI_SOURCE_STATUS_CHANNEL_CALENDAR_TABLE_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SPI_SOURCE_STATUS_CHANNEL_CALENDAR_TABLE_TBL_SPI_R_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_INDIRECT_SPI_SOURCE_STATUS_CHANNEL_CALENDAR_TABLE_TBL_SPI_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    spi = (FAP21V_SPI4_2_INTERFACE)param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_spi_source_status_channel_calendar_table_tbl_get_unsafe(
          unit,
          spi,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "src_cal_tbl: %x\n", data.src_cal_tbl);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_fdt_mesh_satial_multicast_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_FDT_MESH_SATIAL_MULTICAST_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_fdt");
  soc_sand_proc_name = "ui_fap21v_acc_ind_fdt_mesh_satial_multicast_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_FDT_MESH_SATIAL_MULTICAST_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_FDT_MESH_SATIAL_MULTICAST_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_fdt_mesh_satial_multicast_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_FDT_MESH_SATIAL_MULTICAST_TBL_CONTEXT_DUPLICATION_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_FDT_MESH_SATIAL_MULTICAST_TBL_CONTEXT_DUPLICATION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.context_duplication = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_FDT_MESH_SATIAL_MULTICAST_TBL_LOCAL_DUPLICATION_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_FDT_MESH_SATIAL_MULTICAST_TBL_LOCAL_DUPLICATION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.local_duplication = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_fdt_mesh_satial_multicast_tbl_set_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_fdt_mesh_satial_multicast_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_FDT_MESH_SATIAL_MULTICAST_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_fdt");
  soc_sand_proc_name = "ui_fap21v_acc_ind_fdt_mesh_satial_multicast_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_FDT_MESH_SATIAL_MULTICAST_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_FDT_MESH_SATIAL_MULTICAST_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_fdt_mesh_satial_multicast_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "context_duplication: %x\n", data.context_duplication);
  soc_sand_os_printf( "local_duplication: %x\n", data.local_duplication);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_lbp_unicast_lookup_table_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_LBP_UNICAST_LOOKUP_TABLE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_ind_lbp_unicast_lookup_table_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_UNICAST_LOOKUP_TABLE_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_LBP_UNICAST_LOOKUP_TABLE_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_lbp_unicast_lookup_table_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_UNICAST_LOOKUP_TABLE_TBL_BASE_QNUM_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_LBP_UNICAST_LOOKUP_TABLE_TBL_BASE_QNUM_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.base_qnum_val = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_UNICAST_LOOKUP_TABLE_TBL_BASE_QNUM_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_LBP_UNICAST_LOOKUP_TABLE_TBL_BASE_QNUM_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.base_qnum = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_lbp_unicast_lookup_table_tbl_set_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_lbp_unicast_lookup_table_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_LBP_UNICAST_LOOKUP_TABLE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_ind_lbp_unicast_lookup_table_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_UNICAST_LOOKUP_TABLE_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_LBP_UNICAST_LOOKUP_TABLE_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_lbp_unicast_lookup_table_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "base_qnum_val: %x\n", data.base_qnum_val);
  soc_sand_os_printf( "base_qnum: %x\n", data.base_qnum);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_lbp_lag_membership_lookup_table_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    lag_ndx,
    port_ndx;
  FAP21V_LBP_LAG_MEMBERSHIP_LOOKUP_TABLE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_ind_lbp_lag_membership_lookup_table_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_LAG_MEMBERSHIP_LOOKUP_TABLE_TBL_LAG_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_LBP_LAG_MEMBERSHIP_LOOKUP_TABLE_TBL_LAG_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    lag_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_LAG_MEMBERSHIP_LOOKUP_TABLE_TBL_PORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_LBP_LAG_MEMBERSHIP_LOOKUP_TABLE_TBL_PORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    port_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_lbp_lag_membership_lookup_table_tbl_get_unsafe(
          unit,
          lag_ndx,
          port_ndx,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_LAG_MEMBERSHIP_LOOKUP_TABLE_TBL_LAG_DESTINATION_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_LBP_LAG_MEMBERSHIP_LOOKUP_TABLE_TBL_LAG_DESTINATION_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.lag_destination_id = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_lbp_lag_membership_lookup_table_tbl_set_unsafe(
          unit,
          lag_ndx,
          port_ndx,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_lbp_lag_membership_lookup_table_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    lag_ndx,
    port_ndx;
  FAP21V_LBP_LAG_MEMBERSHIP_LOOKUP_TABLE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_ind_lbp_lag_membership_lookup_table_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_LAG_MEMBERSHIP_LOOKUP_TABLE_TBL_LAG_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_LBP_LAG_MEMBERSHIP_LOOKUP_TABLE_TBL_LAG_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    lag_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_LAG_MEMBERSHIP_LOOKUP_TABLE_TBL_PORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_LBP_LAG_MEMBERSHIP_LOOKUP_TABLE_TBL_PORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    port_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_lbp_lag_membership_lookup_table_tbl_get_unsafe(
          unit,
          lag_ndx,
          port_ndx,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "lag_destination_id: %x\n", data.lag_destination_id);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_lbp_lag_size_lookup_table_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_LBP_LAG_SIZE_LOOKUP_TABLE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_ind_lbp_lag_size_lookup_table_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_LAG_SIZE_LOOKUP_TABLE_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_LBP_LAG_SIZE_LOOKUP_TABLE_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_lbp_lag_size_lookup_table_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_LAG_SIZE_LOOKUP_TABLE_TBL_INIT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_LBP_LAG_SIZE_LOOKUP_TABLE_TBL_INIT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.init = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_LAG_SIZE_LOOKUP_TABLE_TBL_LAG_SIZE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_LBP_LAG_SIZE_LOOKUP_TABLE_TBL_LAG_SIZE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.lag_size = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_lbp_lag_size_lookup_table_tbl_set_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_lbp_lag_size_lookup_table_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_LBP_LAG_SIZE_LOOKUP_TABLE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_ind_lbp_lag_size_lookup_table_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_LAG_SIZE_LOOKUP_TABLE_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_LBP_LAG_SIZE_LOOKUP_TABLE_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_lbp_lag_size_lookup_table_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "init: %x\n", data.init);
  soc_sand_os_printf( "lag_size: %x\n", data.lag_size);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_lbp_outbound_mirror_destination_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_LBP_MIRROR_DESTINATION_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_ind_lbp_outbound_mirror_destination_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_OUTBOUND_MIRROR_DESTINATION_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_LBP_OUTBOUND_MIRROR_DESTINATION_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_lbp_mirror_destination_tbl_get_unsafe(
          unit,
          FAP21V_MIRROR_TYPE_OUTBOUND,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_OUTBOUND_MIRROR_DESTINATION_MODE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_LBP_OUTBOUND_MIRROR_DESTINATION_MODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.mode = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting mode***", TRUE);
    goto exit;
  }
  if (data.mode == 0)
  {
    if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_OUTBOUND_MIRROR_DESTINATION_DESTINATION_ID,1))
    {
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_LBP_OUTBOUND_MIRROR_DESTINATION_DESTINATION_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      data.mode_union.mode_is_0.destination = (uint32)param_val->value.ulong_value;
    }
  }
  else
  {
    if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_OUTBOUND_MIRROR_DESTINATION_QUEUE_NUM_VAL_ID,1))
    {
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_LBP_OUTBOUND_MIRROR_DESTINATION_QUEUE_NUM_VAL_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      data.mode_union.mode_is_1.queue_num_val = (uint32)param_val->value.ulong_value;
    }
    if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_OUTBOUND_MIRROR_DESTINATION_QUEUE_NUM_ID,1))
    {
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_LBP_OUTBOUND_MIRROR_DESTINATION_QUEUE_NUM_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      data.mode_union.mode_is_1.queue_num = (uint32)param_val->value.ulong_value;
    }
  }
  ret = fap21v_lbp_mirror_destination_tbl_set_unsafe(
          unit,
          FAP21V_MIRROR_TYPE_OUTBOUND,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_lbp_outbound_mirror_destination_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_LBP_MIRROR_DESTINATION_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_ind_lbp_outbound_mirror_destination_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_OUTBOUND_MIRROR_DESTINATION_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_LBP_OUTBOUND_MIRROR_DESTINATION_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_lbp_mirror_destination_tbl_get_unsafe(
          unit,
          FAP21V_MIRROR_TYPE_OUTBOUND,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "mode: %x\n", data.mode);
  if (data.mode == 0)
  {
    soc_sand_os_printf( "destination: %x\n", data.mode_union.mode_is_0.destination);
  }
  else
  {
    soc_sand_os_printf( "queue_num_val: %x\n", data.mode_union.mode_is_1.queue_num_val);
    soc_sand_os_printf( "queue_num: %x\n", data.mode_union.mode_is_1.queue_num);
  }

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_lbp_recycling_queues_lookup_table_recycle_fap_table_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_LBP_RECYCLING_QUEUES_LOOKUP_TABLE_RECYCLE_FAP_TABLE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_ind_lbp_recycling_queues_lookup_table_recycle_fap_table_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_RECYCLING_QUEUES_LOOKUP_TABLE_RECYCLE_FAP_TABLE_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_LBP_RECYCLING_QUEUES_LOOKUP_TABLE_RECYCLE_FAP_TABLE_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_lbp_recycling_queues_lookup_table_recycle_fap_table_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_RECYCLING_QUEUES_LOOKUP_TABLE_RECYCLE_FAP_TABLE_TBL_BASE_QNUM_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_LBP_RECYCLING_QUEUES_LOOKUP_TABLE_RECYCLE_FAP_TABLE_TBL_BASE_QNUM_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.base_qnum_val = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_RECYCLING_QUEUES_LOOKUP_TABLE_RECYCLE_FAP_TABLE_TBL_BASE_QNUM_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_LBP_RECYCLING_QUEUES_LOOKUP_TABLE_RECYCLE_FAP_TABLE_TBL_BASE_QNUM_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.base_qnum = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_lbp_recycling_queues_lookup_table_recycle_fap_table_tbl_set_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_lbp_recycling_queues_lookup_table_recycle_fap_table_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_LBP_RECYCLING_QUEUES_LOOKUP_TABLE_RECYCLE_FAP_TABLE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_ind_lbp_recycling_queues_lookup_table_recycle_fap_table_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_RECYCLING_QUEUES_LOOKUP_TABLE_RECYCLE_FAP_TABLE_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_LBP_RECYCLING_QUEUES_LOOKUP_TABLE_RECYCLE_FAP_TABLE_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_lbp_recycling_queues_lookup_table_recycle_fap_table_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "base_qnum_val: %x\n", data.base_qnum_val);
  soc_sand_os_printf( "base_qnum: %x\n", data.base_qnum);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_lbp_incoming_fap_port_lookup_table_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  FAP21V_SPI4_2_INTERFACE
    spi;
  uint32
    entry_offset;
  FAP21V_LBP_INCOMING_FAP_PORT_LOOKUP_TABLE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_ind_lbp_incoming_fap_port_lookup_table_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_INCOMING_FAP_PORT_LOOKUP_TABLE_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_LBP_INCOMING_FAP_PORT_LOOKUP_TABLE_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_INCOMING_FAP_PORT_LOOKUP_TABLE_TBL_SPI_W_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_INDIRECT_LBP_INCOMING_FAP_PORT_LOOKUP_TABLE_TBL_SPI_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    spi = (FAP21V_SPI4_2_INTERFACE)param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_lbp_incoming_fap_port_lookup_table_tbl_get_unsafe(
          unit,
          spi,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_INCOMING_FAP_PORT_LOOKUP_TABLE_TBL_INCOMING_FAP_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_LBP_INCOMING_FAP_PORT_LOOKUP_TABLE_TBL_INCOMING_FAP_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.incoming_fap_port = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_lbp_incoming_fap_port_lookup_table_tbl_set_unsafe(
          unit,
          spi,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_lbp_incoming_fap_port_lookup_table_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  FAP21V_SPI4_2_INTERFACE
    spi;
  uint32
    entry_offset;
  FAP21V_LBP_INCOMING_FAP_PORT_LOOKUP_TABLE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_ind_lbp_incoming_fap_port_lookup_table_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_INCOMING_FAP_PORT_LOOKUP_TABLE_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_LBP_INCOMING_FAP_PORT_LOOKUP_TABLE_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_INCOMING_FAP_PORT_LOOKUP_TABLE_TBL_SPI_R_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_INDIRECT_LBP_INCOMING_FAP_PORT_LOOKUP_TABLE_TBL_SPI_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    spi = (FAP21V_SPI4_2_INTERFACE)param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_lbp_incoming_fap_port_lookup_table_tbl_get_unsafe(
          unit,
          spi,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "incoming_fap_port: %x\n", data.incoming_fap_port);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_lbp_system_fap_port_lookup_table_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_LBP_SYSTEM_FAP_PORT_LOOKUP_TABLE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_ind_lbp_system_fap_port_lookup_table_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_SYSTEM_FAP_PORT_LOOKUP_TABLE_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_LBP_SYSTEM_FAP_PORT_LOOKUP_TABLE_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_lbp_system_fap_port_lookup_table_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_SYSTEM_FAP_PORT_LOOKUP_TABLE_TBL_SYSTEM_FAP_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_LBP_SYSTEM_FAP_PORT_LOOKUP_TABLE_TBL_SYSTEM_FAP_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.system_fap_port = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_lbp_system_fap_port_lookup_table_tbl_set_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_lbp_system_fap_port_lookup_table_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_LBP_SYSTEM_FAP_PORT_LOOKUP_TABLE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_ind_lbp_system_fap_port_lookup_table_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_SYSTEM_FAP_PORT_LOOKUP_TABLE_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_LBP_SYSTEM_FAP_PORT_LOOKUP_TABLE_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_lbp_system_fap_port_lookup_table_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "system_fap_port: %x\n", data.system_fap_port);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_lbp_lag_modulo_divide_lookup_table_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_LBP_LAG_MODULO_DIVIDE_LOOKUP_TABLE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_ind_lbp_lag_modulo_divide_lookup_table_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_LAG_MODULO_DIVIDE_LOOKUP_TABLE_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_LBP_LAG_MODULO_DIVIDE_LOOKUP_TABLE_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_lbp_lag_modulo_divide_lookup_table_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_LAG_MODULO_DIVIDE_LOOKUP_TABLE_TBL_MODULO_DEVIDE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_LBP_LAG_MODULO_DIVIDE_LOOKUP_TABLE_TBL_MODULO_DEVIDE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.modulo_devide = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_lbp_lag_modulo_divide_lookup_table_tbl_set_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_lbp_lag_modulo_divide_lookup_table_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_LBP_LAG_MODULO_DIVIDE_LOOKUP_TABLE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_ind_lbp_lag_modulo_divide_lookup_table_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_LAG_MODULO_DIVIDE_LOOKUP_TABLE_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_LBP_LAG_MODULO_DIVIDE_LOOKUP_TABLE_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_lbp_lag_modulo_divide_lookup_table_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "modulo_devide: %x\n", data.modulo_devide);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_lbp_tree_routing_id_lookup_table_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_LBP_TREE_ROUTING_ID_MULTICAST_ID_LOOKUP_TABLE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_ind_lbp_tree_routing_id_lookup_table_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_TREE_ROUTING_ID_LOOKUP_TABLE_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_LBP_TREE_ROUTING_ID_LOOKUP_TABLE_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_lbp_tree_routing_id_lookup_table_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_TREE_ROUTING_ID_LOOKUP_TABLE_TBL_NEXT_DESCENDANT_LEAF_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_LBP_TREE_ROUTING_ID_LOOKUP_TABLE_TBL_NEXT_DESCENDANT_LEAF_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.tree_routing_lookup_table.next_descendant_leaf = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_TREE_ROUTING_ID_LOOKUP_TABLE_TBL_TREE_ROUTING_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_LBP_TREE_ROUTING_ID_LOOKUP_TABLE_TBL_TREE_ROUTING_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.tree_routing_lookup_table.tree_routing_id = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_TREE_ROUTING_ID_LOOKUP_TABLE_TBL_LEFT_FAP_POINTER_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_LBP_TREE_ROUTING_ID_LOOKUP_TABLE_TBL_LEFT_FAP_POINTER_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.tree_routing_lookup_table.left_fap_pointer = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_TREE_ROUTING_ID_LOOKUP_TABLE_TBL_RIGHT_FAP_POINTER_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_LBP_TREE_ROUTING_ID_LOOKUP_TABLE_TBL_RIGHT_FAP_POINTER_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.tree_routing_lookup_table.right_fap_pointer = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_lbp_tree_routing_id_lookup_table_tbl_set_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_lbp_tree_routing_id_lookup_table_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_LBP_TREE_ROUTING_ID_MULTICAST_ID_LOOKUP_TABLE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_ind_lbp_tree_routing_id_lookup_table_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_TREE_ROUTING_ID_LOOKUP_TABLE_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_LBP_TREE_ROUTING_ID_LOOKUP_TABLE_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_lbp_tree_routing_id_lookup_table_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "next_descendant_leaf: %x\n", data.tree_routing_lookup_table.next_descendant_leaf);
  soc_sand_os_printf( "tree_routing_id: %x\n", data.tree_routing_lookup_table.tree_routing_id);
  soc_sand_os_printf( "left_fap_pointer: %x\n", data.tree_routing_lookup_table.left_fap_pointer);
  soc_sand_os_printf( "right_fap_pointer: %x\n", data.tree_routing_lookup_table.right_fap_pointer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_lbp_inbound_mirror_destination_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_LBP_MIRROR_DESTINATION_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_ind_lbp_inbound_mirror_destination_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_INBOUND_MIRROR_DESTINATION_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_LBP_INBOUND_MIRROR_DESTINATION_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_lbp_mirror_destination_tbl_get_unsafe(
          unit,
          FAP21V_MIRROR_TYPE_INBOUND,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_INBOUND_MIRROR_DESTINATION_MODE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_LBP_INBOUND_MIRROR_DESTINATION_MODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.mode = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting mode***", TRUE);
    goto exit;
  }
  if (data.mode == 0)
  {
    if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_INBOUND_MIRROR_DESTINATION_DESTINATION_ID,1))
    {
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_LBP_INBOUND_MIRROR_DESTINATION_DESTINATION_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      data.mode_union.mode_is_0.destination = (uint32)param_val->value.ulong_value;
    }
  }
  else
  {
    if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_INBOUND_MIRROR_DESTINATION_QUEUE_NUM_VAL_ID,1))
    {
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_LBP_INBOUND_MIRROR_DESTINATION_QUEUE_NUM_VAL_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      data.mode_union.mode_is_1.queue_num_val = (uint32)param_val->value.ulong_value;
    }
    if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_INBOUND_MIRROR_DESTINATION_QUEUE_NUM_ID,1))
    {
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_LBP_INBOUND_MIRROR_DESTINATION_QUEUE_NUM_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      data.mode_union.mode_is_1.queue_num = (uint32)param_val->value.ulong_value;
    }
  }
  ret = fap21v_lbp_mirror_destination_tbl_set_unsafe(
          unit,
          FAP21V_MIRROR_TYPE_INBOUND,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_lbp_inbound_mirror_destination_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_LBP_MIRROR_DESTINATION_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_ind_lbp_inbound_mirror_destination_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_INBOUND_MIRROR_DESTINATION_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_LBP_INBOUND_MIRROR_DESTINATION_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_lbp_mirror_destination_tbl_get_unsafe(
          unit,
          FAP21V_MIRROR_TYPE_INBOUND,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "mode: %x\n", data.mode);
  if (data.mode == 0)
  {
    soc_sand_os_printf( "destination: %x\n", data.mode_union.mode_is_0.destination);
  }
  else
  {
    soc_sand_os_printf( "queue_num_val: %x\n", data.mode_union.mode_is_1.queue_num_val);
    soc_sand_os_printf( "queue_num: %x\n", data.mode_union.mode_is_1.queue_num);
  }

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_lbp_multicast_id_lookup_table_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_LBP_TREE_ROUTING_ID_MULTICAST_ID_LOOKUP_TABLE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_ind_lbp_multicast_id_lookup_table_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_MULTICAST_ID_LOOKUP_TABLE_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_LBP_MULTICAST_ID_LOOKUP_TABLE_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_lbp_multicast_id_lookup_table_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_MULTICAST_ID_LOOKUP_TABLE_TBL_NEXT_DESCENDANT_LEAF_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_LBP_MULTICAST_ID_LOOKUP_TABLE_TBL_NEXT_DESCENDANT_LEAF_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.multicast_id_lookup_table.next_descendant_leaf = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_MULTICAST_ID_LOOKUP_TABLE_TBL_RIGHT_BASE_Q_NUM_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_LBP_MULTICAST_ID_LOOKUP_TABLE_TBL_RIGHT_BASE_Q_NUM_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.multicast_id_lookup_table.right_base_q_num_val = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_MULTICAST_ID_LOOKUP_TABLE_TBL_RIGHT_BASE_Q_NUM_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_LBP_MULTICAST_ID_LOOKUP_TABLE_TBL_RIGHT_BASE_Q_NUM_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.multicast_id_lookup_table.right_base_q_num = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_MULTICAST_ID_LOOKUP_TABLE_TBL_LEFT_BASE_Q_NUMBER_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_LBP_MULTICAST_ID_LOOKUP_TABLE_TBL_LEFT_BASE_Q_NUMBER_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.multicast_id_lookup_table.left_base_q_number_val = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_MULTICAST_ID_LOOKUP_TABLE_TBL_LEFT_BASE_Q_NUMBER_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_LBP_MULTICAST_ID_LOOKUP_TABLE_TBL_LEFT_BASE_Q_NUMBER_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.multicast_id_lookup_table.left_base_q_number = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_lbp_multicast_id_lookup_table_tbl_set_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_lbp_multicast_id_lookup_table_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_LBP_TREE_ROUTING_ID_MULTICAST_ID_LOOKUP_TABLE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_ind_lbp_multicast_id_lookup_table_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_MULTICAST_ID_LOOKUP_TABLE_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_LBP_MULTICAST_ID_LOOKUP_TABLE_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_lbp_multicast_id_lookup_table_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "next_descendant_leaf: %x\n", data.multicast_id_lookup_table.next_descendant_leaf);
  soc_sand_os_printf( "right_base_q_num_val: %x\n", data.multicast_id_lookup_table.right_base_q_num_val);
  soc_sand_os_printf( "right_base_q_num: %x\n", data.multicast_id_lookup_table.right_base_q_num);
  soc_sand_os_printf( "left_base_q_number_val: %x\n", data.multicast_id_lookup_table.left_base_q_number_val);
  soc_sand_os_printf( "left_base_q_number: %x\n", data.multicast_id_lookup_table.left_base_q_number);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_rtp_control_route_memory_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_RTP_CONTROL_ROUTE_MEMORY_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_rtp");
  soc_sand_proc_name = "ui_fap21v_acc_ind_rtp_control_route_memory_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_RTP_CONTROL_ROUTE_MEMORY_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_RTP_CONTROL_ROUTE_MEMORY_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_rtp_control_route_memory_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_RTP_CONTROL_ROUTE_MEMORY_TBL_CRMA_0_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_RTP_CONTROL_ROUTE_MEMORY_TBL_CRMA_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.crma[0] = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_RTP_CONTROL_ROUTE_MEMORY_TBL_CRMA_1_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_RTP_CONTROL_ROUTE_MEMORY_TBL_CRMA_1_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.crma[1] = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_rtp_control_route_memory_tbl_set_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_rtp_control_route_memory_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_RTP_CONTROL_ROUTE_MEMORY_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_rtp");
  soc_sand_proc_name = "ui_fap21v_acc_ind_rtp_control_route_memory_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_RTP_CONTROL_ROUTE_MEMORY_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_RTP_CONTROL_ROUTE_MEMORY_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_rtp_control_route_memory_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "crma[0]: %x\n", data.crma[0]);
  soc_sand_os_printf( "crma[1]: %x\n", data.crma[1]);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_rtp_data_route_memory_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_RTP_DATA_ROUTE_MEMORY_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_rtp");
  soc_sand_proc_name = "ui_fap21v_acc_ind_rtp_data_route_memory_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_RTP_DATA_ROUTE_MEMORY_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_RTP_DATA_ROUTE_MEMORY_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_rtp_data_route_memory_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_RTP_DATA_ROUTE_MEMORY_TBL_DRMA_0_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_RTP_DATA_ROUTE_MEMORY_TBL_DRMA_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.drma[0] = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_RTP_DATA_ROUTE_MEMORY_TBL_DRMA_1_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_RTP_DATA_ROUTE_MEMORY_TBL_DRMA_1_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.drma[1] = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_rtp_data_route_memory_tbl_set_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_rtp_data_route_memory_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_RTP_DATA_ROUTE_MEMORY_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_rtp");
  soc_sand_proc_name = "ui_fap21v_acc_ind_rtp_data_route_memory_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_RTP_DATA_ROUTE_MEMORY_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_RTP_DATA_ROUTE_MEMORY_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_rtp_data_route_memory_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "drma[0]: %x\n", data.drma[0]);
  soc_sand_os_printf( "drma[1]: %x\n", data.drma[1]);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_inq_ipbsize_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_INQ_IPBSIZE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_inq");
  soc_sand_proc_name = "ui_fap21v_acc_ind_inq_ipbsize_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_IPBSIZE_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_INQ_IPBSIZE_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_inq_ipbsize_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_IPBSIZE_TBL_SIZE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_INQ_IPBSIZE_TBL_SIZE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.size = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_IPBSIZE_TBL_SRC_CHAN_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_INQ_IPBSIZE_TBL_SRC_CHAN_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.src_chan_id = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_inq_ipbsize_tbl_set_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_inq_ipbsize_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_INQ_IPBSIZE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_inq");
  soc_sand_proc_name = "ui_fap21v_acc_ind_inq_ipbsize_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_IPBSIZE_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_INQ_IPBSIZE_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_inq_ipbsize_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "size: %x\n", data.size);
  soc_sand_os_printf( "src_chan_id: %x\n", data.src_chan_id);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_inq_ipasize_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_INQ_IPASIZE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_inq");
  soc_sand_proc_name = "ui_fap21v_acc_ind_inq_ipasize_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_IPASIZE_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_INQ_IPASIZE_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_inq_ipasize_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_IPASIZE_TBL_SIZE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_INQ_IPASIZE_TBL_SIZE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.size = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_IPASIZE_TBL_SRC_CHAN_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_INQ_IPASIZE_TBL_SRC_CHAN_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.src_chan_id = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_inq_ipasize_tbl_set_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_inq_ipasize_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_INQ_IPASIZE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_inq");
  soc_sand_proc_name = "ui_fap21v_acc_ind_inq_ipasize_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_IPASIZE_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_INQ_IPASIZE_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_inq_ipasize_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "size: %x\n", data.size);
  soc_sand_os_printf( "src_chan_id: %x\n", data.src_chan_id);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_inq_tdma_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_INQ_TDMA_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_inq");
  soc_sand_proc_name = "ui_fap21v_acc_ind_inq_tdma_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_TDMA_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_INQ_TDMA_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_inq_tdma_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_TDMA_TBL_CELL_DATA_0_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_INQ_TDMA_TBL_CELL_DATA_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.cell_data[0] = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_TDMA_TBL_CELL_DATA_1_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_INQ_TDMA_TBL_CELL_DATA_1_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.cell_data[1] = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_TDMA_TBL_CELL_DATA_2_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_INQ_TDMA_TBL_CELL_DATA_2_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.cell_data[2] = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_TDMA_TBL_CELL_DATA_3_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_INQ_TDMA_TBL_CELL_DATA_3_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.cell_data[3] = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_TDMA_TBL_CELL_DATA_4_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_INQ_TDMA_TBL_CELL_DATA_4_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.cell_data[4] = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_TDMA_TBL_CELL_DATA_5_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_INQ_TDMA_TBL_CELL_DATA_5_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.cell_data[5] = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_TDMA_TBL_CELL_DATA_6_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_INQ_TDMA_TBL_CELL_DATA_6_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.cell_data[6] = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_TDMA_TBL_CELL_DATA_7_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_INQ_TDMA_TBL_CELL_DATA_7_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.cell_data[7] = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_inq_tdma_tbl_set_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_inq_tdma_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_INQ_TDMA_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_inq");
  soc_sand_proc_name = "ui_fap21v_acc_ind_inq_tdma_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_TDMA_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_INQ_TDMA_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_inq_tdma_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "cell_data: %x\n", data.cell_data);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_inq_ipb_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_INQ_IPB_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_inq");
  soc_sand_proc_name = "ui_fap21v_acc_ind_inq_ipb_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_IPB_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_INQ_IPB_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_inq_ipb_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_IPB_TBL_CELL_DATA_0_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_INQ_IPB_TBL_CELL_DATA_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.cell_data[0] = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_IPB_TBL_CELL_DATA_1_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_INQ_IPB_TBL_CELL_DATA_1_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.cell_data[1] = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_IPB_TBL_CELL_DATA_2_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_INQ_IPB_TBL_CELL_DATA_2_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.cell_data[2] = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_IPB_TBL_CELL_DATA_3_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_INQ_IPB_TBL_CELL_DATA_3_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.cell_data[3] = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_IPB_TBL_CELL_DATA_4_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_INQ_IPB_TBL_CELL_DATA_4_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.cell_data[4] = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_IPB_TBL_CELL_DATA_5_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_INQ_IPB_TBL_CELL_DATA_5_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.cell_data[5] = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_IPB_TBL_CELL_DATA_6_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_INQ_IPB_TBL_CELL_DATA_6_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.cell_data[6] = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_IPB_TBL_CELL_DATA_7_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_INQ_IPB_TBL_CELL_DATA_7_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.cell_data[7] = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_inq_ipb_tbl_set_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_inq_ipb_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_INQ_IPB_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_inq");
  soc_sand_proc_name = "ui_fap21v_acc_ind_inq_ipb_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_IPB_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_INQ_IPB_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_inq_ipb_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "cell_data: %x\n", data.cell_data);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_inq_ipa_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_INQ_IPA_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_inq");
  soc_sand_proc_name = "ui_fap21v_acc_ind_inq_ipa_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_IPA_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_INQ_IPA_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_inq_ipa_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_IPA_TBL_CELL_DATA_0_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_INQ_IPA_TBL_CELL_DATA_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.cell_data[0] = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_IPA_TBL_CELL_DATA_1_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_INQ_IPA_TBL_CELL_DATA_1_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.cell_data[1] = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_IPA_TBL_CELL_DATA_2_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_INQ_IPA_TBL_CELL_DATA_2_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.cell_data[2] = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_IPA_TBL_CELL_DATA_3_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_INQ_IPA_TBL_CELL_DATA_3_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.cell_data[3] = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_IPA_TBL_CELL_DATA_4_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_INQ_IPA_TBL_CELL_DATA_4_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.cell_data[4] = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_IPA_TBL_CELL_DATA_5_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_INQ_IPA_TBL_CELL_DATA_5_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.cell_data[5] = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_IPA_TBL_CELL_DATA_6_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_INQ_IPA_TBL_CELL_DATA_6_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.cell_data[6] = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_IPA_TBL_CELL_DATA_7_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_INQ_IPA_TBL_CELL_DATA_7_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.cell_data[7] = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_inq_ipa_tbl_set_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_inq_ipa_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_INQ_IPA_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_inq");
  soc_sand_proc_name = "ui_fap21v_acc_ind_inq_ipa_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_IPA_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_INQ_IPA_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_inq_ipa_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "cell_data: %x\n", data.cell_data);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_inq_tdmb_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_INQ_TDMB_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_inq");
  soc_sand_proc_name = "ui_fap21v_acc_ind_inq_tdmb_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_TDMB_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_INQ_TDMB_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_inq_tdmb_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_TDMB_TBL_CELL_DATA_0_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_INQ_TDMB_TBL_CELL_DATA_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.cell_data[0] = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_TDMB_TBL_CELL_DATA_1_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_INQ_TDMB_TBL_CELL_DATA_1_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.cell_data[1] = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_TDMB_TBL_CELL_DATA_2_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_INQ_TDMB_TBL_CELL_DATA_2_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.cell_data[2] = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_TDMB_TBL_CELL_DATA_3_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_INQ_TDMB_TBL_CELL_DATA_3_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.cell_data[3] = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_TDMB_TBL_CELL_DATA_4_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_INQ_TDMB_TBL_CELL_DATA_4_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.cell_data[4] = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_TDMB_TBL_CELL_DATA_5_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_INQ_TDMB_TBL_CELL_DATA_5_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.cell_data[5] = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_TDMB_TBL_CELL_DATA_6_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_INQ_TDMB_TBL_CELL_DATA_6_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.cell_data[6] = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_TDMB_TBL_CELL_DATA_7_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_INQ_TDMB_TBL_CELL_DATA_7_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.cell_data[7] = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_inq_tdmb_tbl_set_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_inq_tdmb_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_INQ_TDMB_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_inq");
  soc_sand_proc_name = "ui_fap21v_acc_ind_inq_tdmb_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_TDMB_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_INQ_TDMB_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_inq_tdmb_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "cell_data: %x\n", data.cell_data);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_egq_fop_multicast_lut_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_EGQ_FOP_MULTICAST_LUT0_TBL_DATA
    data0;
  FAP21V_EGQ_FOP_MULTICAST_LUT1_TBL_DATA
    data1;
  FAP21V_EGQ_FOP_MULTICAST_LUT2_TBL_DATA
    data2;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_egq");
  soc_sand_proc_name = "ui_fap21v_acc_ind_egq_fop_multicast_lut_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_EGQ_FOP_MULTICAST_LUT_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_EGQ_FOP_MULTICAST_LUT_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_egq_fop_multicast_lut0_tbl_get_unsafe(
          unit,
          entry_offset,
          &data0
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  ret = fap21v_egq_fop_multicast_lut1_tbl_get_unsafe(
          unit,
          entry_offset,
          &data1
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  ret = fap21v_egq_fop_multicast_lut2_tbl_get_unsafe(
          unit,
          entry_offset,
          &data2
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_EGQ_FOP_MULTICAST_LUT_TBL_FOPTO_BMP_LLST_TBL0_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_EGQ_FOP_MULTICAST_LUT_TBL_FOPTO_BMP_LLST_TBL0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data0.fopto_bmp_llst_tbl0 = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_EGQ_FOP_MULTICAST_LUT_TBL_FOPTO_BMP_LLST_TBL1_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_EGQ_FOP_MULTICAST_LUT_TBL_FOPTO_BMP_LLST_TBL1_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data1.fopto_bmp_llst_tbl1 = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_EGQ_FOP_MULTICAST_LUT_TBL_FOPTO_BMP_LLST_TBL2_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_EGQ_FOP_MULTICAST_LUT_TBL_FOPTO_BMP_LLST_TBL2_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data2.fopto_bmp_llst_tbl2 = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_egq_fop_multicast_lut0_tbl_set_unsafe(
          unit,
          entry_offset,
          &data0
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  ret = fap21v_egq_fop_multicast_lut1_tbl_set_unsafe(
          unit,
          entry_offset,
          &data1
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  ret = fap21v_egq_fop_multicast_lut2_tbl_set_unsafe(
          unit,
          entry_offset,
          &data2
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_egq_fop_multicast_lut_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_EGQ_FOP_MULTICAST_LUT0_TBL_DATA
    data0;
  FAP21V_EGQ_FOP_MULTICAST_LUT1_TBL_DATA
    data1;
  FAP21V_EGQ_FOP_MULTICAST_LUT2_TBL_DATA
    data2;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_egq");
  soc_sand_proc_name = "ui_fap21v_acc_ind_egq_fop_multicast_lut_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_EGQ_FOP_MULTICAST_LUT_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_EGQ_FOP_MULTICAST_LUT_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_egq_fop_multicast_lut0_tbl_get_unsafe(
          unit,
          entry_offset,
          &data0
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  ret = fap21v_egq_fop_multicast_lut1_tbl_get_unsafe(
          unit,
          entry_offset,
          &data1
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  ret = fap21v_egq_fop_multicast_lut2_tbl_get_unsafe(
          unit,
          entry_offset,
          &data2
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "multicast group %x: %lx %lx %lx\n", entry_offset, data0.fopto_bmp_llst_tbl0, data1.fopto_bmp_llst_tbl1, data2.fopto_bmp_llst_tbl2);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_egq_packet_descriptor_memory_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_EGQ_PACKET_DESCRIPTOR_MEMORY_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_egq");
  soc_sand_proc_name = "ui_fap21v_acc_ind_egq_packet_descriptor_memory_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_EGQ_PACKET_DESCRIPTOR_MEMORY_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_EGQ_PACKET_DESCRIPTOR_MEMORY_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_egq_packet_descriptor_memory_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "free_read_cnt: %x\n", data.free_read_cnt);
  soc_sand_os_printf( "req_tx_cnt: %x\n", data.req_tx_cnt);
  soc_sand_os_printf( "pkt_size: %x\n", data.pkt_size);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_egq_shaper_calendar_table_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_EGQ_SHAPER_CALENDAR_TABLE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_egq");
  soc_sand_proc_name = "ui_fap21v_acc_ind_egq_shaper_calendar_table_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_EGQ_SHAPER_CALENDAR_TABLE_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_EGQ_SHAPER_CALENDAR_TABLE_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_egq_shaper_calendar_table_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_EGQ_SHAPER_CALENDAR_TABLE_TBL_PORT_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_EGQ_SHAPER_CALENDAR_TABLE_TBL_PORT_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.port_id = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_EGQ_SHAPER_CALENDAR_TABLE_TBL_CREDIT_TO_ADD_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_EGQ_SHAPER_CALENDAR_TABLE_TBL_CREDIT_TO_ADD_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.credit_to_add = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_egq_shaper_calendar_table_tbl_set_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_egq_shaper_calendar_table_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_EGQ_SHAPER_CALENDAR_TABLE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_egq");
  soc_sand_proc_name = "ui_fap21v_acc_ind_egq_shaper_calendar_table_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_EGQ_SHAPER_CALENDAR_TABLE_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_EGQ_SHAPER_CALENDAR_TABLE_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_egq_shaper_calendar_table_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "port_id: %x\n", data.port_id);
  soc_sand_os_printf( "credit_to_add: %x\n", data.credit_to_add);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_egq_credit_balance_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_EGQ_CREDIT_BALANCE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_egq");
  soc_sand_proc_name = "ui_fap21v_acc_ind_egq_credit_balance_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_EGQ_CREDIT_BALANCE_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_EGQ_CREDIT_BALANCE_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_egq_credit_balance_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "credit_balance: %x\n", data.credit_balance);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_egq_scheduled_queues_vs_unscheduled_queues_weight_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_EGQ_SCHEDULED_QUEUES_VS_UNSCHEDULED_QUEUES_WEIGHT_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_egq");
  soc_sand_proc_name = "ui_fap21v_acc_ind_egq_scheduled_queues_vs_unscheduled_queues_weight_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_EGQ_SCHEDULED_QUEUES_VS_UNSCHEDULED_QUEUES_WEIGHT_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_EGQ_SCHEDULED_QUEUES_VS_UNSCHEDULED_QUEUES_WEIGHT_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_egq_scheduled_queues_vs_unscheduled_queues_weight_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_EGQ_SCHEDULED_QUEUES_VS_UNSCHEDULED_QUEUES_WEIGHT_TBL_SCHD_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_EGQ_SCHEDULED_QUEUES_VS_UNSCHEDULED_QUEUES_WEIGHT_TBL_SCHD_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.schd_weight = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_EGQ_SCHEDULED_QUEUES_VS_UNSCHEDULED_QUEUES_WEIGHT_TBL_UN_SCH_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_EGQ_SCHEDULED_QUEUES_VS_UNSCHEDULED_QUEUES_WEIGHT_TBL_UN_SCH_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.un_sch_weight = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_egq_scheduled_queues_vs_unscheduled_queues_weight_tbl_set_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_egq_scheduled_queues_vs_unscheduled_queues_weight_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_EGQ_SCHEDULED_QUEUES_VS_UNSCHEDULED_QUEUES_WEIGHT_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_egq");
  soc_sand_proc_name = "ui_fap21v_acc_ind_egq_scheduled_queues_vs_unscheduled_queues_weight_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_EGQ_SCHEDULED_QUEUES_VS_UNSCHEDULED_QUEUES_WEIGHT_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_EGQ_SCHEDULED_QUEUES_VS_UNSCHEDULED_QUEUES_WEIGHT_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_egq_scheduled_queues_vs_unscheduled_queues_weight_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "schd_weight: %x\n", data.schd_weight);
  soc_sand_os_printf( "un_sch_weight: %x\n", data.un_sch_weight);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_egq_queue_size_desc_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_EGQ_QUEUE_SIZE_DESC_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_egq");
  soc_sand_proc_name = "ui_fap21v_acc_ind_egq_queue_size_desc_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_EGQ_QUEUE_SIZE_DESC_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_EGQ_QUEUE_SIZE_DESC_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_egq_queue_size_desc_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "queue_size_desc: %x\n", data.queue_size_desc);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_egq_multicast_pruning_lut_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  FAP21V_SPI4_2_INTERFACE
    spi;
  uint32
    entry_offset;
  FAP21V_EGQ_MULTICAST_PRUNING_LUT_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_egq");
  soc_sand_proc_name = "ui_fap21v_acc_ind_egq_multicast_pruning_lut_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_EGQ_MULTICAST_PRUNING_LUT_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_EGQ_MULTICAST_PRUNING_LUT_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_EGQ_MULTICAST_PRUNING_LUT_TBL_SPI_W_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_INDIRECT_EGQ_MULTICAST_PRUNING_LUT_TBL_SPI_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    spi = (FAP21V_SPI4_2_INTERFACE)param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_egq_multicast_pruning_lut_tbl_get_unsafe(
          unit,
          spi,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_EGQ_MULTICAST_PRUNING_LUT_TBL_CHNL_TO_PORT_TBL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_EGQ_MULTICAST_PRUNING_LUT_TBL_CHNL_TO_PORT_TBL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.chnl_to_port_tbl = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_egq_multicast_pruning_lut_tbl_set_unsafe(
          unit,
          spi,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_egq_multicast_pruning_lut_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  FAP21V_SPI4_2_INTERFACE
    spi;
  uint32
    entry_offset;
  FAP21V_EGQ_MULTICAST_PRUNING_LUT_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_egq");
  soc_sand_proc_name = "ui_fap21v_acc_ind_egq_multicast_pruning_lut_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_EGQ_MULTICAST_PRUNING_LUT_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_EGQ_MULTICAST_PRUNING_LUT_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_EGQ_MULTICAST_PRUNING_LUT_TBL_SPI_R_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_INDIRECT_EGQ_MULTICAST_PRUNING_LUT_TBL_SPI_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    spi = (FAP21V_SPI4_2_INTERFACE)param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_egq_multicast_pruning_lut_tbl_get_unsafe(
          unit,
          spi,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "chnl_to_port_tbl: %x\n", data.chnl_to_port_tbl);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_egq_destination_system_port_lut_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_EGQ_DESTINATION_SYSTEM_PORT_LUT_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_egq");
  soc_sand_proc_name = "ui_fap21v_acc_ind_egq_destination_system_port_lut_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_EGQ_DESTINATION_SYSTEM_PORT_LUT_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_EGQ_DESTINATION_SYSTEM_PORT_LUT_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_egq_destination_system_port_lut_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_EGQ_DESTINATION_SYSTEM_PORT_LUT_DEST_SYS_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_EGQ_DESTINATION_SYSTEM_PORT_LUT_DEST_SYS_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.dest_sys_port = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_egq_destination_system_port_lut_tbl_set_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_egq_destination_system_port_lut_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_EGQ_DESTINATION_SYSTEM_PORT_LUT_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_egq");
  soc_sand_proc_name = "ui_fap21v_acc_ind_egq_destination_system_port_lut_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_EGQ_DESTINATION_SYSTEM_PORT_LUT_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_EGQ_DESTINATION_SYSTEM_PORT_LUT_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_egq_destination_system_port_lut_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "dest_sys_port: %x\n", data.dest_sys_port);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_egq_queue_size_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_EGQ_QUEUE_SIZE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_egq");
  soc_sand_proc_name = "ui_fap21v_acc_ind_egq_queue_size_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_EGQ_QUEUE_SIZE_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_EGQ_QUEUE_SIZE_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_egq_queue_size_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "queue_size: %x\n", data.queue_size);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_qdp_queue_size_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_QDP_QUEUE_SIZE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_ind_qdp_queue_size_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_SIZE_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_SIZE_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_qdp_queue_size_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "avg_qsize: %x\n", data.qsize);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_qdp_queue_type_parameters_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_QDP_QUEUE_TYPE_PARAMETERS_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_ind_qdp_queue_type_parameters_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_PARAMETERS_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_PARAMETERS_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_qdp_queue_type_parameters_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_PARAMETERS_TBL_WREDENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_PARAMETERS_TBL_WREDENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.wredenable = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_PARAMETERS_TBL_WREDEXP_WQT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_PARAMETERS_TBL_WREDEXP_WQT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.wredexp_wqt = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_PARAMETERS_TBL_WREDPACKET_DIS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_PARAMETERS_TBL_WREDPACKET_DIS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.wredpacket_dis = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_PARAMETERS_TBL_MAX_INS_QSIZ_EXP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_PARAMETERS_TBL_MAX_INS_QSIZ_EXP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.max_ins_qsiz_exp = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_PARAMETERS_TBL_MAX_INS_QSIZ_MNT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_PARAMETERS_TBL_MAX_INS_QSIZ_MNT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.max_ins_qsiz_mnt = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_PARAMETERS_TBL_HDR_COMPENSATION_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_PARAMETERS_TBL_HDR_COMPENSATION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.hdr_compensation = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_PARAMETERS_TBL_SUB_HDR_COMPENSATION_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_PARAMETERS_TBL_SUB_HDR_COMPENSATION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.sub_hdr_compensation = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_PARAMETERS_TBL_SREDENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_PARAMETERS_TBL_SREDENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.sredenable = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_PARAMETERS_TBL_SREDQSIZE_RANGE_TH_00_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_PARAMETERS_TBL_SREDQSIZE_RANGE_TH_00_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.sredqsize_range_th[0] = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_PARAMETERS_TBL_SREDQSIZE_RANGE_TH_01_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_PARAMETERS_TBL_SREDQSIZE_RANGE_TH_01_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.sredqsize_range_th[1] = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_PARAMETERS_TBL_SREDQSIZE_RANGE_TH_02_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_PARAMETERS_TBL_SREDQSIZE_RANGE_TH_02_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.sredqsize_range_th[2] = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_PARAMETERS_TBL_SREDQSIZE_RANGE_TH_03_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_PARAMETERS_TBL_SREDQSIZE_RANGE_TH_03_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.sredqsize_range_th[3] = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_PARAMETERS_TBL_SREDQSIZE_RANGE_TH_04_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_PARAMETERS_TBL_SREDQSIZE_RANGE_TH_04_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.sredqsize_range_th[4] = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_PARAMETERS_TBL_SREDQSIZE_RANGE_TH_05_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_PARAMETERS_TBL_SREDQSIZE_RANGE_TH_05_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.sredqsize_range_th[5] = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_PARAMETERS_TBL_SREDQSIZE_RANGE_TH_06_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_PARAMETERS_TBL_SREDQSIZE_RANGE_TH_06_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.sredqsize_range_th[6] = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_PARAMETERS_TBL_SREDQSIZE_RANGE_TH_07_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_PARAMETERS_TBL_SREDQSIZE_RANGE_TH_07_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.sredqsize_range_th[7] = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_PARAMETERS_TBL_SREDQSIZE_RANGE_TH_08_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_PARAMETERS_TBL_SREDQSIZE_RANGE_TH_08_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.sredqsize_range_th[8] = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_PARAMETERS_TBL_SREDQSIZE_RANGE_TH_09_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_PARAMETERS_TBL_SREDQSIZE_RANGE_TH_09_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.sredqsize_range_th[9] = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_PARAMETERS_TBL_SREDQSIZE_RANGE_TH_10_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_PARAMETERS_TBL_SREDQSIZE_RANGE_TH_10_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.sredqsize_range_th[10] = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_PARAMETERS_TBL_SREDQSIZE_RANGE_TH_11_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_PARAMETERS_TBL_SREDQSIZE_RANGE_TH_11_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.sredqsize_range_th[11] = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_PARAMETERS_TBL_SREDQSIZE_RANGE_TH_12_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_PARAMETERS_TBL_SREDQSIZE_RANGE_TH_12_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.sredqsize_range_th[12] = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_PARAMETERS_TBL_SREDQSIZE_RANGE_TH_13_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_PARAMETERS_TBL_SREDQSIZE_RANGE_TH_13_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.sredqsize_range_th[13] = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_PARAMETERS_TBL_SREDQSIZE_RANGE_TH_14_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_PARAMETERS_TBL_SREDQSIZE_RANGE_TH_14_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.sredqsize_range_th[14] = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_qdp_queue_type_parameters_tbl_set_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_qdp_queue_type_parameters_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    ind;
  uint32
    entry_offset;
  FAP21V_QDP_QUEUE_TYPE_PARAMETERS_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_ind_qdp_queue_type_parameters_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_PARAMETERS_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_PARAMETERS_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_qdp_queue_type_parameters_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "wredenable: %x\n", data.wredenable);
  soc_sand_os_printf( "wredexp_wqt: %x\n", data.wredexp_wqt);
  soc_sand_os_printf( "wredpacket_dis: %x\n", data.wredpacket_dis);
  soc_sand_os_printf( "max_ins_qsiz_exp: %x\n", data.max_ins_qsiz_exp);
  soc_sand_os_printf( "max_ins_qsiz_mnt: %x\n", data.max_ins_qsiz_mnt);
  soc_sand_os_printf( "hdr_compensation: %x\n", data.hdr_compensation);
  soc_sand_os_printf( "sub_hdr_compensation: %x\n", data.sub_hdr_compensation);
  soc_sand_os_printf( "sredenable: %x\n", data.sredenable);
  for (ind = 0; ind < FAP21V_SYS_RED_CONST_NOF_RANGES - 1; ++ind)
  {
    soc_sand_os_printf( "sredqsize_range_th[%d]: %x\n", ind, data.sredqsize_range_th[ind]);
  }

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_qdp_queue_wred_parameters_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    drop_precedence_ndx,
    queue_type_qdp_ndx;
  FAP21V_QDP_QUEUE_WRED_PARAMETERS_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_ind_qdp_queue_wred_parameters_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_WRED_PARAMETERS_TBL_QUEUE_TYPE_QDP_NDX_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_WRED_PARAMETERS_TBL_QUEUE_TYPE_QDP_NDX_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    queue_type_qdp_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_WRED_PARAMETERS_TBL_DROP_PRECEDENCE_NDX_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_WRED_PARAMETERS_TBL_DROP_PRECEDENCE_NDX_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    drop_precedence_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_qdp_queue_wred_parameters_tbl_get_unsafe(
          unit,
          queue_type_qdp_ndx,
          drop_precedence_ndx,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_WRED_PARAMETERS_TBL_C2_QTNDROP_M_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_WRED_PARAMETERS_TBL_C2_QTNDROP_M_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.c2_qtndrop_m = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_WRED_PARAMETERS_TBL_C3_QTNDROP_M_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_WRED_PARAMETERS_TBL_C3_QTNDROP_M_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.c3_qtndrop_m = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_WRED_PARAMETERS_TBL_C1_QTNDROP_M_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_WRED_PARAMETERS_TBL_C1_QTNDROP_M_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.c1_qtndrop_m = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_WRED_PARAMETERS_TBL_MAX_AVRG_TRH_Q_TNDROP_M_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_WRED_PARAMETERS_TBL_MAX_AVRG_TRH_Q_TNDROP_M_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.max_avrg_trh_q_tndrop_m = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_WRED_PARAMETERS_TBL_MIN_AVRG_TRH_QT_NDROP_M_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_WRED_PARAMETERS_TBL_MIN_AVRG_TRH_QT_NDROP_M_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.min_avrg_trh_qt_ndrop_m = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_WRED_PARAMETERS_TBL_SREDADMIT_TH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_WRED_PARAMETERS_TBL_SREDADMIT_TH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.sredadmit_th = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_WRED_PARAMETERS_TBL_SREDADMIT_DROP_TH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_WRED_PARAMETERS_TBL_SREDADMIT_DROP_TH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.sredadmit_drop_th = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_WRED_PARAMETERS_TBL_SREDDROP_TH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_WRED_PARAMETERS_TBL_SREDDROP_TH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.sreddrop_th = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_WRED_PARAMETERS_TBL_SREDDROP_PROB1_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_WRED_PARAMETERS_TBL_SREDDROP_PROB1_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.sreddrop_prob1 = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_WRED_PARAMETERS_TBL_SREDDROP_PROB2_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_WRED_PARAMETERS_TBL_SREDDROP_PROB2_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.sreddrop_prob2 = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_qdp_queue_wred_parameters_tbl_set_unsafe(
          unit,
          queue_type_qdp_ndx,
          drop_precedence_ndx,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_qdp_queue_wred_parameters_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    drop_precedence_ndx,
    queue_type_qdp_ndx;
  FAP21V_QDP_QUEUE_WRED_PARAMETERS_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_ind_qdp_queue_wred_parameters_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_WRED_PARAMETERS_TBL_QUEUE_TYPE_QDP_NDX_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_WRED_PARAMETERS_TBL_QUEUE_TYPE_QDP_NDX_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    queue_type_qdp_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_WRED_PARAMETERS_TBL_DROP_PRECEDENCE_NDX_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_WRED_PARAMETERS_TBL_DROP_PRECEDENCE_NDX_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    drop_precedence_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_qdp_queue_wred_parameters_tbl_get_unsafe(
          unit,
          queue_type_qdp_ndx,
          drop_precedence_ndx,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "c2_qtndrop_m: %x\n", data.c2_qtndrop_m);
  soc_sand_os_printf( "c3_qtndrop_m: %x\n", data.c3_qtndrop_m);
  soc_sand_os_printf( "c1_qtndrop_m: %x\n", data.c1_qtndrop_m);
  soc_sand_os_printf( "max_avrg_trh_q_tndrop_m: %x\n", data.max_avrg_trh_q_tndrop_m);
  soc_sand_os_printf( "min_avrg_trh_qt_ndrop_m: %x\n", data.min_avrg_trh_qt_ndrop_m);
  soc_sand_os_printf( "sredadmit_th: %x\n", data.sredadmit_th);
  soc_sand_os_printf( "sredadmit_drop_th: %x\n", data.sredadmit_drop_th);
  soc_sand_os_printf( "sreddrop_th: %x\n", data.sreddrop_th);
  soc_sand_os_printf( "sreddrop_prob1: %x\n", data.sreddrop_prob1);
  soc_sand_os_printf( "sreddrop_prob2: %x\n", data.sreddrop_prob2);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_qdp_global_statistics_report_thresholds_system_red_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    buffer_pool_ndx,
    boundary_ndx;
  FAP21V_QDP_GLOBAL_STATISTICS_REPORT_THRESHOLDS_SYSTEM_RED_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_ind_qdp_global_statistics_report_thresholds_system_red_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_GLOBAL_STATISTICS_REPORT_THRESHOLDS_SYSTEM_RED_TBL_BUFFER_POOL_NDX_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_QDP_GLOBAL_STATISTICS_REPORT_THRESHOLDS_SYSTEM_RED_TBL_BUFFER_POOL_NDX_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    buffer_pool_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_GLOBAL_STATISTICS_REPORT_THRESHOLDS_SYSTEM_RED_TBL_BOUNDARY_NDX_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_QDP_GLOBAL_STATISTICS_REPORT_THRESHOLDS_SYSTEM_RED_TBL_BOUNDARY_NDX_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    boundary_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_qdp_global_statistics_report_thresholds_system_red_tbl_get_unsafe(
          unit,
          buffer_pool_ndx,
          boundary_ndx,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_GLOBAL_STATISTICS_REPORT_THRESHOLDS_SYSTEM_RED_TBL_THRESHOLD_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_QDP_GLOBAL_STATISTICS_REPORT_THRESHOLDS_SYSTEM_RED_TBL_THRESHOLD_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.threshold = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_qdp_global_statistics_report_thresholds_system_red_tbl_set_unsafe(
          unit,
          buffer_pool_ndx,
          boundary_ndx,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_qdp_global_statistics_report_thresholds_system_red_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    buffer_pool_ndx,
    boundary_ndx;
  FAP21V_QDP_GLOBAL_STATISTICS_REPORT_THRESHOLDS_SYSTEM_RED_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_ind_qdp_global_statistics_report_thresholds_system_red_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_GLOBAL_STATISTICS_REPORT_THRESHOLDS_SYSTEM_RED_TBL_BUFFER_POOL_NDX_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_QDP_GLOBAL_STATISTICS_REPORT_THRESHOLDS_SYSTEM_RED_TBL_BUFFER_POOL_NDX_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    buffer_pool_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_GLOBAL_STATISTICS_REPORT_THRESHOLDS_SYSTEM_RED_TBL_BOUNDARY_NDX_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_QDP_GLOBAL_STATISTICS_REPORT_THRESHOLDS_SYSTEM_RED_TBL_BOUNDARY_NDX_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    boundary_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_qdp_global_statistics_report_thresholds_system_red_tbl_get_unsafe(
          unit,
          buffer_pool_ndx,
          boundary_ndx,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "threshold: %x\n", data.threshold);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_qdp_global_statistics_reject_thresholds_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_QDP_GLOBAL_STATISTICS_REJECT_THRESHOLDS_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_ind_qdp_global_statistics_reject_thresholds_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_GLOBAL_STATISTICS_REJECT_THRESHOLDS_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_QDP_GLOBAL_STATISTICS_REJECT_THRESHOLDS_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_qdp_global_statistics_reject_thresholds_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_GLOBAL_STATISTICS_REJECT_THRESHOLDS_TBL_THRESHOLD_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_QDP_GLOBAL_STATISTICS_REJECT_THRESHOLDS_TBL_THRESHOLD_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.threshold = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_qdp_global_statistics_reject_thresholds_tbl_set_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_qdp_global_statistics_reject_thresholds_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_QDP_GLOBAL_STATISTICS_REJECT_THRESHOLDS_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_ind_qdp_global_statistics_reject_thresholds_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_GLOBAL_STATISTICS_REJECT_THRESHOLDS_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_QDP_GLOBAL_STATISTICS_REJECT_THRESHOLDS_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_qdp_global_statistics_reject_thresholds_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "threshold: %x\n", data.threshold);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_qdp_not_empty_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_QDP_NOT_EMPTY_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_ind_qdp_not_empty_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_NOT_EMPTY_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_QDP_NOT_EMPTY_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_qdp_not_empty_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "not_empty_queue: %x\n", data.not_empty_queue);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_qdp_queue_type_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_QDP_QUEUE_TYPE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_ind_qdp_queue_type_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_TBL_W_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_TBL_W_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_qdp_queue_type_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_TBL_QUEUE_TYPE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_TBL_QUEUE_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.queue_type = (uint32)param_val->value.ulong_value;
  }
  ret = fap21v_qdp_queue_type_tbl_set_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" ***  - FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_qdp_queue_type_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    entry_offset;
  FAP21V_QDP_QUEUE_TYPE_TBL_DATA
    data;
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_ind_qdp_queue_type_tbl_get";
  unit = get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_TBL_R_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_TBL_R_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    entry_offset = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter offset***", TRUE);
    goto exit;
  }
  ret = fap21v_qdp_queue_type_tbl_get_unsafe(
          unit,
          entry_offset,
          &data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }
  send_string_to_screen(" *** - SUCCEEDED", TRUE);
  soc_sand_os_printf( "queue_type: %x\n", data.queue_type);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_egq_get(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_egq_get");

  if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_EGQ_FOP_MULTICAST_LUT_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_egq_fop_multicast_lut_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_EGQ_PACKET_DESCRIPTOR_MEMORY_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_egq_packet_descriptor_memory_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_EGQ_SHAPER_CALENDAR_TABLE_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_egq_shaper_calendar_table_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_EGQ_CREDIT_BALANCE_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_egq_credit_balance_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_EGQ_SCHEDULED_QUEUES_VS_UNSCHEDULED_QUEUES_WEIGHT_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_egq_scheduled_queues_vs_unscheduled_queues_weight_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_EGQ_QUEUE_SIZE_DESC_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_egq_queue_size_desc_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_EGQ_MULTICAST_PRUNING_LUT_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_egq_multicast_pruning_lut_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_EGQ_DESTINATION_SYSTEM_PORT_LUT_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_egq_destination_system_port_lut_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_EGQ_QUEUE_SIZE_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_egq_queue_size_tbl_get(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after egq***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_egq_set(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_egq_set");

  if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_EGQ_FOP_MULTICAST_LUT_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_egq_fop_multicast_lut_tbl_set(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_EGQ_SHAPER_CALENDAR_TABLE_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_egq_shaper_calendar_table_tbl_set(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_EGQ_SCHEDULED_QUEUES_VS_UNSCHEDULED_QUEUES_WEIGHT_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_egq_scheduled_queues_vs_unscheduled_queues_weight_tbl_set(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_EGQ_MULTICAST_PRUNING_LUT_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_egq_multicast_pruning_lut_tbl_set(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_EGQ_DESTINATION_SYSTEM_PORT_LUT_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_egq_destination_system_port_lut_set(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after egq***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_fdt_get(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_fdt_get");

  if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_FDT_MESH_SATIAL_MULTICAST_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_fdt_mesh_satial_multicast_tbl_get(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after fdt***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_fdt_set(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_fdt_set");

  if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_FDT_MESH_SATIAL_MULTICAST_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_fdt_mesh_satial_multicast_tbl_set(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after fdt***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_inq_get(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_inq_get");

  if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_IPBSIZE_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_inq_ipbsize_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_IPASIZE_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_inq_ipasize_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_TDMA_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_inq_tdma_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_IPB_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_inq_ipb_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_IPA_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_inq_ipa_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_TDMB_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_inq_tdmb_tbl_get(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after inq***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_inq_set(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_inq_set");

  if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_IPBSIZE_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_inq_ipbsize_tbl_set(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_IPASIZE_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_inq_ipasize_tbl_set(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_TDMA_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_inq_tdma_tbl_set(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_IPB_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_inq_ipb_tbl_set(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_IPA_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_inq_ipa_tbl_set(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_INQ_TDMB_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_inq_tdmb_tbl_set(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after inq***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_ips_get(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_ips_get");

  if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_ips_queue_type_lookup_table_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_ips_queue_priority_map_select_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_ips_credit_balance_based_thresholds_table_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_ips_queue_descriptor_table_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_ips_destination_device_and_port_lookup_table_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_SIZE_TABLE_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_ips_queue_size_table_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_ips_empty_queue_credit_balance_table_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_ips_system_red_max_queue_size_table_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_ips_queue_size_based_thresholds_table_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_ips_system_physical_port_lookup_table_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_ips_queue_priority_maps_table_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_FLOW_STATUS_MESSAGE_REQUEST_QUEUE_CONTROL_TABLE_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_ips_flow_status_message_request_queue_control_table_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_FLOW_ID_LOOKUP_TABLE_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_ips_flow_id_lookup_table_tbl_get(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after ips***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_ips_set(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_ips_set");

  if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_ips_queue_type_lookup_table_tbl_set(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_ips_queue_priority_map_select_tbl_set(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_ips_credit_balance_based_thresholds_table_tbl_set(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_ips_queue_descriptor_table_tbl_set(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_ips_destination_device_and_port_lookup_table_tbl_set(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_SIZE_TABLE_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_ips_queue_size_table_tbl_set(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_ips_empty_queue_credit_balance_table_tbl_set(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_ips_system_red_max_queue_size_table_tbl_set(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_ips_queue_size_based_thresholds_table_tbl_set(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_ips_system_physical_port_lookup_table_tbl_set(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_ips_queue_priority_maps_table_tbl_set(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_FLOW_STATUS_MESSAGE_REQUEST_QUEUE_CONTROL_TABLE_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_ips_flow_status_message_request_queue_control_table_tbl_set(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_IPS_FLOW_ID_LOOKUP_TABLE_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_ips_flow_id_lookup_table_tbl_set(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after ips***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_lbp_get(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_lbp_get");

  if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_UNICAST_LOOKUP_TABLE_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_lbp_unicast_lookup_table_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_LAG_MEMBERSHIP_LOOKUP_TABLE_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_lbp_lag_membership_lookup_table_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_LAG_SIZE_LOOKUP_TABLE_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_lbp_lag_size_lookup_table_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_OUTBOUND_MIRROR_DESTINATION_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_lbp_outbound_mirror_destination_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_RECYCLING_QUEUES_LOOKUP_TABLE_RECYCLE_FAP_TABLE_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_lbp_recycling_queues_lookup_table_recycle_fap_table_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_INCOMING_FAP_PORT_LOOKUP_TABLE_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_lbp_incoming_fap_port_lookup_table_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_SYSTEM_FAP_PORT_LOOKUP_TABLE_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_lbp_system_fap_port_lookup_table_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_LAG_MODULO_DIVIDE_LOOKUP_TABLE_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_lbp_lag_modulo_divide_lookup_table_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_TREE_ROUTING_ID_LOOKUP_TABLE_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_lbp_tree_routing_id_lookup_table_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_INBOUND_MIRROR_DESTINATION_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_lbp_inbound_mirror_destination_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_MULTICAST_ID_LOOKUP_TABLE_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_lbp_multicast_id_lookup_table_tbl_get(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after lbp***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_lbp_set(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_lbp_set");

  if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_UNICAST_LOOKUP_TABLE_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_lbp_unicast_lookup_table_tbl_set(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_LAG_MEMBERSHIP_LOOKUP_TABLE_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_lbp_lag_membership_lookup_table_tbl_set(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_LAG_SIZE_LOOKUP_TABLE_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_lbp_lag_size_lookup_table_tbl_set(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_OUTBOUND_MIRROR_DESTINATION_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_lbp_outbound_mirror_destination_set(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_RECYCLING_QUEUES_LOOKUP_TABLE_RECYCLE_FAP_TABLE_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_lbp_recycling_queues_lookup_table_recycle_fap_table_tbl_set(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_INCOMING_FAP_PORT_LOOKUP_TABLE_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_lbp_incoming_fap_port_lookup_table_tbl_set(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_SYSTEM_FAP_PORT_LOOKUP_TABLE_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_lbp_system_fap_port_lookup_table_tbl_set(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_LAG_MODULO_DIVIDE_LOOKUP_TABLE_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_lbp_lag_modulo_divide_lookup_table_tbl_set(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_TREE_ROUTING_ID_LOOKUP_TABLE_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_lbp_tree_routing_id_lookup_table_tbl_set(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_INBOUND_MIRROR_DESTINATION_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_lbp_inbound_mirror_destination_set(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_LBP_MULTICAST_ID_LOOKUP_TABLE_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_lbp_multicast_id_lookup_table_tbl_set(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after lbp***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_mmu_get(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_mmu_get");

  if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_MMU_SFM_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_mmu_sfm_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_MMU_IQS_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_mmu_iqs_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_MMU_DRAM_ADDRESSES_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_mmu_dram_addresses_tbl_get(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after mmu***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_mmu_set(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_mmu_set");

  if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_MMU_SFM_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_mmu_sfm_tbl_set(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_MMU_IQS_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_mmu_iqs_tbl_set(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_MMU_DRAM_ADDRESSES_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_mmu_dram_addresses_tbl_set(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after mmu***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_qdp_get(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_qdp_get");

  if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_SIZE_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_qdp_queue_size_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_PARAMETERS_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_qdp_queue_type_parameters_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_WRED_PARAMETERS_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_qdp_queue_wred_parameters_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_GLOBAL_STATISTICS_REPORT_THRESHOLDS_SYSTEM_RED_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_qdp_global_statistics_report_thresholds_system_red_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_GLOBAL_STATISTICS_REJECT_THRESHOLDS_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_qdp_global_statistics_reject_thresholds_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_NOT_EMPTY_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_qdp_not_empty_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_qdp_queue_type_tbl_get(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after qdp***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_qdp_set(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_qdp_set");

  if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_PARAMETERS_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_qdp_queue_type_parameters_tbl_set(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_WRED_PARAMETERS_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_qdp_queue_wred_parameters_tbl_set(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_GLOBAL_STATISTICS_REPORT_THRESHOLDS_SYSTEM_RED_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_qdp_global_statistics_report_thresholds_system_red_tbl_set(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_GLOBAL_STATISTICS_REJECT_THRESHOLDS_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_qdp_global_statistics_reject_thresholds_tbl_set(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_QDP_QUEUE_TYPE_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_qdp_queue_type_tbl_set(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after qdp***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_rtp_get(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_rtp_get");

  if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_RTP_CONTROL_ROUTE_MEMORY_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_rtp_control_route_memory_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_RTP_DATA_ROUTE_MEMORY_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_rtp_data_route_memory_tbl_get(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after rtp***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_rtp_set(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_rtp_set");

  if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_RTP_CONTROL_ROUTE_MEMORY_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_rtp_control_route_memory_tbl_set(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_RTP_DATA_ROUTE_MEMORY_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_rtp_data_route_memory_tbl_set(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after rtp***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_sch_get(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_sch_get");

  if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_SCHEDULER_ENABLE_MEMORY_SEM_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_sch_scheduler_enable_memory_sem_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_PORT_QUEUE_SIZE_PQS_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_sch_port_queue_size_pqs_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_FLOW_SUB_FLOW_FSF_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_sch_flow_sub_flow_fsf_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_FLOW_GROUP_MEMORY_FGM_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_sch_flow_group_memory_fgm_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_FORCE_STATUS_MESSAGE_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_sch_force_status_message_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_CL_SCHEDULERS_CONFIGURATION_SCC_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_sch_cl_schedulers_configuration_scc_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_DSM_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_sch_dsm_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_DEVICE_RATE_MEMORY_DRM_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_sch_device_rate_memory_drm_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_TOKEN_MEMORY_CONTROLLER_TMC_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_sch_token_memory_controller_tmc_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_HR_SCHEDULER_CONFIGURATION_SHC_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_sch_hr_scheduler_configuration_shc_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_FLOW_TO_QUEUE_MAPPING_FQM_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_sch_flow_to_queue_mapping_fqm_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_FDMS_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_sch_fdms_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_sch_scheduler_credit_generation_calendar_cal_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_CL_SCHEDULERS_TYPE_SCT_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_sch_cl_schedulers_type_sct_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDS_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_sch_shaper_descriptor_memory_static_shds_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_FLOW_TO_FIP_MAPPING_FFM_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_sch_flow_to_fip_mapping_ffm_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_SCHEDULER_INIT_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_sch_scheduler_init_tbl_get(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after sch***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_sch_set(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_sch_set");

  if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_SCHEDULER_ENABLE_MEMORY_SEM_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_sch_scheduler_enable_memory_sem_tbl_set(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_PORT_QUEUE_SIZE_PQS_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_sch_port_queue_size_pqs_tbl_set(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_FLOW_SUB_FLOW_FSF_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_sch_flow_sub_flow_fsf_tbl_set(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_FLOW_GROUP_MEMORY_FGM_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_sch_flow_group_memory_fgm_tbl_set(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_FORCE_STATUS_MESSAGE_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_sch_force_status_message_tbl_set(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_CL_SCHEDULERS_CONFIGURATION_SCC_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_sch_cl_schedulers_configuration_scc_tbl_set(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_DSM_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_sch_dsm_tbl_set(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_DEVICE_RATE_MEMORY_DRM_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_sch_device_rate_memory_drm_tbl_set(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_TOKEN_MEMORY_CONTROLLER_TMC_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_sch_token_memory_controller_tmc_tbl_set(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_HR_SCHEDULER_CONFIGURATION_SHC_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_sch_hr_scheduler_configuration_shc_tbl_set(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_FLOW_TO_QUEUE_MAPPING_FQM_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_sch_flow_to_queue_mapping_fqm_tbl_set(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_FDMS_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_sch_fdms_tbl_set(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_sch_scheduler_credit_generation_calendar_cal_tbl_set(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_CL_SCHEDULERS_TYPE_SCT_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_sch_cl_schedulers_type_sct_tbl_set(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDS_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_sch_shaper_descriptor_memory_static_shds_tbl_set(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_FLOW_TO_FIP_MAPPING_FFM_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_sch_flow_to_fip_mapping_ffm_tbl_set(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SCH_SCHEDULER_INIT_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_sch_scheduler_init_tbl_set(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after sch***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_sla_get(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_sla_get");

  if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SLA_OUT_OF_BAND_FLOW_CONTROL_AUXILIARY_CALENDAR_TABLE_FOR_INTERFACE_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_sla_out_of_band_flow_control_auxiliary_calendar_table_for_interface_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SLA_DESTINATION_SYSTEM_PORT_LUT_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_sla_destination_system_port_lut_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SLA_TDM_MULTICAST_TABLE_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_sla_tdm_multicast_table_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SLA_EGRESS_PORT_TO_SPI4_CHANNEL_TABLE_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_sla_egress_port_to_spi4_channel_table_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SLA_SPI4_CHANNEL_TO_EGRESS_PORT_TABLE_FOR_INTERFACE_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_sla_spi4_channel_to_egress_port_table_for_interface_tbl_get(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after sla***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_sla_set(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_sla_set");

  if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SLA_OUT_OF_BAND_FLOW_CONTROL_AUXILIARY_CALENDAR_TABLE_FOR_INTERFACE_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_sla_out_of_band_flow_control_auxiliary_calendar_table_for_interface_tbl_set(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SLA_DESTINATION_SYSTEM_PORT_LUT_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_sla_destination_system_port_lut_set(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SLA_TDM_MULTICAST_TABLE_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_sla_tdm_multicast_table_tbl_set(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SLA_EGRESS_PORT_TO_SPI4_CHANNEL_TABLE_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_sla_egress_port_to_spi4_channel_table_tbl_set(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SLA_SPI4_CHANNEL_TO_EGRESS_PORT_TABLE_FOR_INTERFACE_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_sla_spi4_channel_to_egress_port_table_for_interface_tbl_set(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after sla***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_spi_get(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_spi_get");

  if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SPI_SINK_STATUS_CHANNEL_CALENDAR_TABLE_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_spi_sink_status_channel_calendar_table_tbl_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SPI_SOURCE_STATUS_CHANNEL_CALENDAR_TABLE_TBL_R_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_spi_source_status_channel_calendar_table_tbl_get(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after spi***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ind_spi_set(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ind_spi_set");

  if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SPI_SINK_STATUS_CHANNEL_CALENDAR_TABLE_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_spi_sink_status_channel_calendar_table_tbl_set(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_INDIRECT_SPI_SOURCE_STATUS_CHANNEL_CALENDAR_TABLE_TBL_W_ID,1))
  {
    ui_ret = ui_fap21v_acc_ind_spi_source_status_channel_calendar_table_tbl_set(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after spi***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}
