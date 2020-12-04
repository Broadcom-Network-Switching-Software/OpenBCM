/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#include <appl/diag/dpp/ref_sys.h>
#include <bcm_app/dpp/../H/usrApp.h>

#include <soc/dpp/SOC_SAND_FAP21V/fap21v_framework.h>
#include <soc/dpp/SOC_SAND_FAP21V/fap21v_api_reg_access.h>
#include <soc/dpp/SOC_SAND_FAP21V/fap21v_chip_regs.h>
#include <soc/dpp/SOC_SAND_FAP21V/fap21v_reg_access.h>
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
  ui_fap21v_acc_fcr_interrupt_masks_reg_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    buffer,
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fcr");
  soc_sand_proc_name = "ui_fap21v_acc_fcr_interrupt_masks_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_INTERRUPT_MASKS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FCR_INTERRUPT_MASKS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FCR_INTERRUPT_MASKS_REG_REGISTER_ID:
      regis = &(regs->fcr.interrupt_masks_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_INTERRUPT_MASKS_REG_SRC_DV_CNG_LINK_EV_MASK_ID:
      field = &(regs->fcr.interrupt_masks_reg.src_dv_cng_link_ev_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_INTERRUPT_MASKS_REG_CPUCNT_CELL_FNEMASK_ID:
      field = &(regs->fcr.interrupt_masks_reg.cpucnt_cell_fnemask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_INTERRUPT_MASKS_REG_LOCAL_ROUT_FSOVF_ID:
      field = &(regs->fcr.interrupt_masks_reg.local_rout_fsovf);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_INTERRUPT_MASKS_REG_LOCAL_ROUT_CREDIT_OVF_ID:
      field = &(regs->fcr.interrupt_masks_reg.local_rout_credit_ovf);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_INTERRUPT_MASKS_REG_REACH_FIFO_OVF_ID:
      field = &(regs->fcr.interrupt_masks_reg.reach_fifo_ovf);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_INTERRUPT_MASKS_REG_FSFIFO_OVF_ID:
      field = &(regs->fcr.interrupt_masks_reg.fsfifo_ovf);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_INTERRUPT_MASKS_REG_CREDIT_FIFO_OVF_ID:
      field = &(regs->fcr.interrupt_masks_reg.credit_fifo_ovf);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "fcr.interrupt_masks_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fcr_interrupt_masks_reg_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  uint32
    ret;
  uint32
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fcr");
  soc_sand_proc_name = "ui_fap21v_acc_fcr_interrupt_masks_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_INTERRUPT_MASKS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FCR_INTERRUPT_MASKS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FCR_INTERRUPT_MASKS_REG_REGISTER_ID:
      regis = &(regs->fcr.interrupt_masks_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_INTERRUPT_MASKS_REG_SRC_DV_CNG_LINK_EV_MASK_ID:
      field = &(regs->fcr.interrupt_masks_reg.src_dv_cng_link_ev_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_INTERRUPT_MASKS_REG_CPUCNT_CELL_FNEMASK_ID:
      field = &(regs->fcr.interrupt_masks_reg.cpucnt_cell_fnemask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_INTERRUPT_MASKS_REG_LOCAL_ROUT_FSOVF_ID:
      field = &(regs->fcr.interrupt_masks_reg.local_rout_fsovf);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_INTERRUPT_MASKS_REG_LOCAL_ROUT_CREDIT_OVF_ID:
      field = &(regs->fcr.interrupt_masks_reg.local_rout_credit_ovf);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_INTERRUPT_MASKS_REG_REACH_FIFO_OVF_ID:
      field = &(regs->fcr.interrupt_masks_reg.reach_fifo_ovf);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_INTERRUPT_MASKS_REG_FSFIFO_OVF_ID:
      field = &(regs->fcr.interrupt_masks_reg.fsfifo_ovf);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_INTERRUPT_MASKS_REG_CREDIT_FIFO_OVF_ID:
      field = &(regs->fcr.interrupt_masks_reg.credit_fifo_ovf);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fcr_flow_status_cells_counter_reg_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    buffer,
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fcr");
  soc_sand_proc_name = "ui_fap21v_acc_fcr_flow_status_cells_counter_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_FLOW_STATUS_CELLS_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FCR_FLOW_STATUS_CELLS_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FCR_FLOW_STATUS_CELLS_COUNTER_REG_REGISTER_ID:
      regis = &(regs->fcr.flow_status_cells_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_FLOW_STATUS_CELLS_COUNTER_REG_FSCELL_COUNT_ID:
      field = &(regs->fcr.flow_status_cells_counter_reg.fscell_count);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_FLOW_STATUS_CELLS_COUNTER_REG_FSCELL_COUNT_OVF_ID:
      field = &(regs->fcr.flow_status_cells_counter_reg.fscell_count_ovf);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "fcr.flow_status_cells_counter_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fcr_flow_status_cells_counter_reg_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  uint32
    ret;
  uint32
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fcr");
  soc_sand_proc_name = "ui_fap21v_acc_fcr_flow_status_cells_counter_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_FLOW_STATUS_CELLS_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FCR_FLOW_STATUS_CELLS_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FCR_FLOW_STATUS_CELLS_COUNTER_REG_REGISTER_ID:
      regis = &(regs->fcr.flow_status_cells_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_FLOW_STATUS_CELLS_COUNTER_REG_FSCELL_COUNT_ID:
      field = &(regs->fcr.flow_status_cells_counter_reg.fscell_count);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_FLOW_STATUS_CELLS_COUNTER_REG_FSCELL_COUNT_OVF_ID:
      field = &(regs->fcr.flow_status_cells_counter_reg.fscell_count_ovf);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fcr_gtimer_configuration_reg_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    buffer,
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fcr");
  soc_sand_proc_name = "ui_fap21v_acc_fcr_gtimer_configuration_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_GTIMER_CONFIGURATION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FCR_GTIMER_CONFIGURATION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FCR_GTIMER_CONFIGURATION_REG_REGISTER_ID:
      regis = &(regs->fcr.gtimer_configuration_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_GTIMER_CONFIGURATION_REG_TIME_COUNT_CONFIG_ID:
      field = &(regs->fcr.gtimer_configuration_reg.time_count_config);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_GTIMER_CONFIGURATION_REG_COUNTER_RESET_ID:
      field = &(regs->fcr.gtimer_configuration_reg.counter_reset);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "fcr.gtimer_configuration_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fcr_gtimer_configuration_reg_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  uint32
    ret;
  uint32
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fcr");
  soc_sand_proc_name = "ui_fap21v_acc_fcr_gtimer_configuration_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_GTIMER_CONFIGURATION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FCR_GTIMER_CONFIGURATION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FCR_GTIMER_CONFIGURATION_REG_REGISTER_ID:
      regis = &(regs->fcr.gtimer_configuration_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_GTIMER_CONFIGURATION_REG_TIME_COUNT_CONFIG_ID:
      field = &(regs->fcr.gtimer_configuration_reg.time_count_config);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_GTIMER_CONFIGURATION_REG_COUNTER_RESET_ID:
      field = &(regs->fcr.gtimer_configuration_reg.counter_reset);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fcr_credit_cells_counter_reg_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    buffer,
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fcr");
  soc_sand_proc_name = "ui_fap21v_acc_fcr_credit_cells_counter_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_CREDIT_CELLS_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FCR_CREDIT_CELLS_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FCR_CREDIT_CELLS_COUNTER_REG_REGISTER_ID:
      regis = &(regs->fcr.credit_cells_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_CREDIT_CELLS_COUNTER_REG_CREDIT_CELL_COUNT_ID:
      field = &(regs->fcr.credit_cells_counter_reg.credit_cell_count);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_CREDIT_CELLS_COUNTER_REG_CREDIT_CELL_COUNT_OVF_ID:
      field = &(regs->fcr.credit_cells_counter_reg.credit_cell_count_ovf);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "fcr.credit_cells_counter_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fcr_credit_cells_counter_reg_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  uint32
    ret;
  uint32
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fcr");
  soc_sand_proc_name = "ui_fap21v_acc_fcr_credit_cells_counter_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_CREDIT_CELLS_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FCR_CREDIT_CELLS_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FCR_CREDIT_CELLS_COUNTER_REG_REGISTER_ID:
      regis = &(regs->fcr.credit_cells_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_CREDIT_CELLS_COUNTER_REG_CREDIT_CELL_COUNT_ID:
      field = &(regs->fcr.credit_cells_counter_reg.credit_cell_count);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_CREDIT_CELLS_COUNTER_REG_CREDIT_CELL_COUNT_OVF_ID:
      field = &(regs->fcr.credit_cells_counter_reg.credit_cell_count_ovf);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fcr_fcr_enablers_and_filter_match_input_link_reg_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    buffer,
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fcr");
  soc_sand_proc_name = "ui_fap21v_acc_fcr_fcr_enablers_and_filter_match_input_link_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_FCR_ENABLERS_AND_FILTER_MATCH_INPUT_LINK_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FCR_FCR_ENABLERS_AND_FILTER_MATCH_INPUT_LINK_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FCR_FCR_ENABLERS_AND_FILTER_MATCH_INPUT_LINK_REG_REGISTER_ID:
      regis = &(regs->fcr.fcr_enablers_and_filter_match_input_link_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_FCR_ENABLERS_AND_FILTER_MATCH_INPUT_LINK_REG_DIS_FL_STS_ID:
      field = &(regs->fcr.fcr_enablers_and_filter_match_input_link_reg.dis_fl_sts);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_FCR_ENABLERS_AND_FILTER_MATCH_INPUT_LINK_REG_DIS_CRD_FCR_ID:
      field = &(regs->fcr.fcr_enablers_and_filter_match_input_link_reg.dis_crd_fcr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_FCR_ENABLERS_AND_FILTER_MATCH_INPUT_LINK_REG_DIS_SR_ID:
      field = &(regs->fcr.fcr_enablers_and_filter_match_input_link_reg.dis_sr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_FCR_ENABLERS_AND_FILTER_MATCH_INPUT_LINK_REG_TRAP_ALL_CNT_ID:
      field = &(regs->fcr.fcr_enablers_and_filter_match_input_link_reg.trap_all_cnt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_FCR_ENABLERS_AND_FILTER_MATCH_INPUT_LINK_REG_MTCH_ACT_ID:
      field = &(regs->fcr.fcr_enablers_and_filter_match_input_link_reg.mtch_act);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_FCR_ENABLERS_AND_FILTER_MATCH_INPUT_LINK_REG_FCRMTCH_LOGIC_NOT_ID:
      field = &(regs->fcr.fcr_enablers_and_filter_match_input_link_reg.fcrmtch_logic_not);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_FCR_ENABLERS_AND_FILTER_MATCH_INPUT_LINK_REG_PC_MTCH_LINK_ID:
      field = &(regs->fcr.fcr_enablers_and_filter_match_input_link_reg.pc_mtch_link);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_FCR_ENABLERS_AND_FILTER_MATCH_INPUT_LINK_REG_PC_MTCH_LINK_ON_ID:
      field = &(regs->fcr.fcr_enablers_and_filter_match_input_link_reg.pc_mtch_link_on);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "fcr.fcr_enablers_and_filter_match_input_link_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fcr_fcr_enablers_and_filter_match_input_link_reg_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  uint32
    ret;
  uint32
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fcr");
  soc_sand_proc_name = "ui_fap21v_acc_fcr_fcr_enablers_and_filter_match_input_link_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_FCR_ENABLERS_AND_FILTER_MATCH_INPUT_LINK_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FCR_FCR_ENABLERS_AND_FILTER_MATCH_INPUT_LINK_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FCR_FCR_ENABLERS_AND_FILTER_MATCH_INPUT_LINK_REG_REGISTER_ID:
      regis = &(regs->fcr.fcr_enablers_and_filter_match_input_link_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_FCR_ENABLERS_AND_FILTER_MATCH_INPUT_LINK_REG_DIS_FL_STS_ID:
      field = &(regs->fcr.fcr_enablers_and_filter_match_input_link_reg.dis_fl_sts);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_FCR_ENABLERS_AND_FILTER_MATCH_INPUT_LINK_REG_DIS_CRD_FCR_ID:
      field = &(regs->fcr.fcr_enablers_and_filter_match_input_link_reg.dis_crd_fcr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_FCR_ENABLERS_AND_FILTER_MATCH_INPUT_LINK_REG_DIS_SR_ID:
      field = &(regs->fcr.fcr_enablers_and_filter_match_input_link_reg.dis_sr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_FCR_ENABLERS_AND_FILTER_MATCH_INPUT_LINK_REG_TRAP_ALL_CNT_ID:
      field = &(regs->fcr.fcr_enablers_and_filter_match_input_link_reg.trap_all_cnt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_FCR_ENABLERS_AND_FILTER_MATCH_INPUT_LINK_REG_MTCH_ACT_ID:
      field = &(regs->fcr.fcr_enablers_and_filter_match_input_link_reg.mtch_act);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_FCR_ENABLERS_AND_FILTER_MATCH_INPUT_LINK_REG_FCRMTCH_LOGIC_NOT_ID:
      field = &(regs->fcr.fcr_enablers_and_filter_match_input_link_reg.fcrmtch_logic_not);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_FCR_ENABLERS_AND_FILTER_MATCH_INPUT_LINK_REG_PC_MTCH_LINK_ID:
      field = &(regs->fcr.fcr_enablers_and_filter_match_input_link_reg.pc_mtch_link);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_FCR_ENABLERS_AND_FILTER_MATCH_INPUT_LINK_REG_PC_MTCH_LINK_ON_ID:
      field = &(regs->fcr.fcr_enablers_and_filter_match_input_link_reg.pc_mtch_link_on);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fcr_programmable_control_cell_counter_reg_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    buffer,
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fcr");
  soc_sand_proc_name = "ui_fap21v_acc_fcr_programmable_control_cell_counter_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_PROGRAMMABLE_CONTROL_CELL_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FCR_PROGRAMMABLE_CONTROL_CELL_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FCR_PROGRAMMABLE_CONTROL_CELL_COUNTER_REG_REGISTER_ID:
      regis = &(regs->fcr.programmable_control_cell_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_PROGRAMMABLE_CONTROL_CELL_COUNTER_REG_PCMTCH_CTRL_LB2_ID:
      field = &(regs->fcr.programmable_control_cell_counter_reg.pcmtch_ctrl_lb2);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "fcr.programmable_control_cell_counter_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fcr_programmable_control_cell_counter_reg_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  uint32
    ret;
  uint32
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fcr");
  soc_sand_proc_name = "ui_fap21v_acc_fcr_programmable_control_cell_counter_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_PROGRAMMABLE_CONTROL_CELL_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FCR_PROGRAMMABLE_CONTROL_CELL_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FCR_PROGRAMMABLE_CONTROL_CELL_COUNTER_REG_REGISTER_ID:
      regis = &(regs->fcr.programmable_control_cell_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_PROGRAMMABLE_CONTROL_CELL_COUNTER_REG_PCMTCH_CTRL_LB2_ID:
      field = &(regs->fcr.programmable_control_cell_counter_reg.pcmtch_ctrl_lb2);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fcr_interrupt_sources_reg_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    buffer,
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fcr");
  soc_sand_proc_name = "ui_fap21v_acc_fcr_interrupt_sources_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_INTERRUPT_SOURCES_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FCR_INTERRUPT_SOURCES_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FCR_INTERRUPT_SOURCES_REG_REGISTER_ID:
      regis = &(regs->fcr.interrupt_sources_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_INTERRUPT_SOURCES_REG_SRC_DV_CNG_LINK_EV_ID:
      field = &(regs->fcr.interrupt_sources_reg.src_dv_cng_link_ev);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_INTERRUPT_SOURCES_REG_CPUCNT_CELL_FNE_ID:
      field = &(regs->fcr.interrupt_sources_reg.cpucnt_cell_fne);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_INTERRUPT_SOURCES_REG_LOCAL_ROUT_FSOVF_ID:
      field = &(regs->fcr.interrupt_sources_reg.local_rout_fsovf);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_INTERRUPT_SOURCES_REG_LOCAL_ROUT_CREDIT_OVF_ID:
      field = &(regs->fcr.interrupt_sources_reg.local_rout_credit_ovf);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_INTERRUPT_SOURCES_REG_REACH_FIFO_OVF_ID:
      field = &(regs->fcr.interrupt_sources_reg.reach_fifo_ovf);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_INTERRUPT_SOURCES_REG_FSFIFO_OVF_ID:
      field = &(regs->fcr.interrupt_sources_reg.fsfifo_ovf);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_INTERRUPT_SOURCES_REG_CREDIT_FIFO_OVF_ID:
      field = &(regs->fcr.interrupt_sources_reg.credit_fifo_ovf);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "fcr.interrupt_sources_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fcr_interrupt_sources_reg_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  uint32
    ret;
  uint32
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fcr");
  soc_sand_proc_name = "ui_fap21v_acc_fcr_interrupt_sources_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_INTERRUPT_SOURCES_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FCR_INTERRUPT_SOURCES_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FCR_INTERRUPT_SOURCES_REG_REGISTER_ID:
      regis = &(regs->fcr.interrupt_sources_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_INTERRUPT_SOURCES_REG_SRC_DV_CNG_LINK_EV_ID:
      field = &(regs->fcr.interrupt_sources_reg.src_dv_cng_link_ev);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_INTERRUPT_SOURCES_REG_CPUCNT_CELL_FNE_ID:
      field = &(regs->fcr.interrupt_sources_reg.cpucnt_cell_fne);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_INTERRUPT_SOURCES_REG_LOCAL_ROUT_FSOVF_ID:
      field = &(regs->fcr.interrupt_sources_reg.local_rout_fsovf);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_INTERRUPT_SOURCES_REG_LOCAL_ROUT_CREDIT_OVF_ID:
      field = &(regs->fcr.interrupt_sources_reg.local_rout_credit_ovf);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_INTERRUPT_SOURCES_REG_REACH_FIFO_OVF_ID:
      field = &(regs->fcr.interrupt_sources_reg.reach_fifo_ovf);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_INTERRUPT_SOURCES_REG_FSFIFO_OVF_ID:
      field = &(regs->fcr.interrupt_sources_reg.fsfifo_ovf);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_INTERRUPT_SOURCES_REG_CREDIT_FIFO_OVF_ID:
      field = &(regs->fcr.interrupt_sources_reg.credit_fifo_ovf);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fcr_programmable_control_cell_counter_mask_reg_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    buffer,
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fcr");
  soc_sand_proc_name = "ui_fap21v_acc_fcr_programmable_control_cell_counter_mask_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_PROGRAMMABLE_CONTROL_CELL_COUNTER_MASK_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FCR_PROGRAMMABLE_CONTROL_CELL_COUNTER_MASK_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FCR_PROGRAMMABLE_CONTROL_CELL_COUNTER_MASK_REG_REGISTER_ID:
      regis = &(regs->fcr.programmable_control_cell_counter_mask_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_PROGRAMMABLE_CONTROL_CELL_COUNTER_MASK_REG_PCMTCH_CTRL_MSK2_ID:
      field = &(regs->fcr.programmable_control_cell_counter_mask_reg.pcmtch_ctrl_msk2);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "fcr.programmable_control_cell_counter_mask_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fcr_programmable_control_cell_counter_mask_reg_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  uint32
    ret;
  uint32
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fcr");
  soc_sand_proc_name = "ui_fap21v_acc_fcr_programmable_control_cell_counter_mask_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_PROGRAMMABLE_CONTROL_CELL_COUNTER_MASK_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FCR_PROGRAMMABLE_CONTROL_CELL_COUNTER_MASK_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FCR_PROGRAMMABLE_CONTROL_CELL_COUNTER_MASK_REG_REGISTER_ID:
      regis = &(regs->fcr.programmable_control_cell_counter_mask_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_PROGRAMMABLE_CONTROL_CELL_COUNTER_MASK_REG_PCMTCH_CTRL_MSK2_ID:
      field = &(regs->fcr.programmable_control_cell_counter_mask_reg.pcmtch_ctrl_msk2);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fcr_total_cells_counter_reg_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    buffer,
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fcr");
  soc_sand_proc_name = "ui_fap21v_acc_fcr_total_cells_counter_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_TOTAL_CELLS_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FCR_TOTAL_CELLS_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FCR_TOTAL_CELLS_COUNTER_REG_REGISTER_ID:
      regis = &(regs->fcr.total_cells_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_TOTAL_CELLS_COUNTER_REG_TOTAL_CELL_COUNT_ID:
      field = &(regs->fcr.total_cells_counter_reg.total_cell_count);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_TOTAL_CELLS_COUNTER_REG_TOTAL_CELL_COUNT_OVF_ID:
      field = &(regs->fcr.total_cells_counter_reg.total_cell_count_ovf);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "fcr.total_cells_counter_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fcr_total_cells_counter_reg_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  uint32
    ret;
  uint32
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fcr");
  soc_sand_proc_name = "ui_fap21v_acc_fcr_total_cells_counter_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_TOTAL_CELLS_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FCR_TOTAL_CELLS_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FCR_TOTAL_CELLS_COUNTER_REG_REGISTER_ID:
      regis = &(regs->fcr.total_cells_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_TOTAL_CELLS_COUNTER_REG_TOTAL_CELL_COUNT_ID:
      field = &(regs->fcr.total_cells_counter_reg.total_cell_count);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_TOTAL_CELLS_COUNTER_REG_TOTAL_CELL_COUNT_OVF_ID:
      field = &(regs->fcr.total_cells_counter_reg.total_cell_count_ovf);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fcr_connectivity_map_registers_reg_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    offset = 0,
    buffer,
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fcr");
  soc_sand_proc_name = "ui_fap21v_acc_fcr_connectivity_map_registers_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_CONNECTIVITY_MAP_REGISTERS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FCR_CONNECTIVITY_MAP_REGISTERS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_CONNECTIVITY_MAP_REGISTERS_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_FCR_CONNECTIVITY_MAP_REGISTERS_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FCR_CONNECTIVITY_MAP_REGISTERS_REG_REGISTER_ID:
      regis = &(regs->fcr.connectivity_map_registers_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_CONNECTIVITY_MAP_REGISTERS_REG_SRC_DV_IDLNK_ID:
      field = &(regs->fcr.connectivity_map_registers_reg[offset].src_dv_idlnk);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_CONNECTIVITY_MAP_REGISTERS_REG_SRC_DV_LVL_LNK_ID:
      field = &(regs->fcr.connectivity_map_registers_reg[offset].src_dv_lvl_lnk);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_CONNECTIVITY_MAP_REGISTERS_REG_SRC_DV_LNK_NUM_ID:
      field = &(regs->fcr.connectivity_map_registers_reg[offset].src_dv_lnk_num);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "fcr.connectivity_map_registers_reg***: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fcr_connectivity_map_registers_reg_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  uint32
    ret;
  uint32
    offset = 0,
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fcr");
  soc_sand_proc_name = "ui_fap21v_acc_fcr_connectivity_map_registers_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_CONNECTIVITY_MAP_REGISTERS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FCR_CONNECTIVITY_MAP_REGISTERS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_CONNECTIVITY_MAP_REGISTERS_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_FCR_CONNECTIVITY_MAP_REGISTERS_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FCR_CONNECTIVITY_MAP_REGISTERS_REG_REGISTER_ID:
      regis = &(regs->fcr.connectivity_map_registers_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_CONNECTIVITY_MAP_REGISTERS_REG_SRC_DV_IDLNK_ID:
      field = &(regs->fcr.connectivity_map_registers_reg[offset].src_dv_idlnk);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_CONNECTIVITY_MAP_REGISTERS_REG_SRC_DV_LVL_LNK_ID:
      field = &(regs->fcr.connectivity_map_registers_reg[offset].src_dv_lvl_lnk);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_CONNECTIVITY_MAP_REGISTERS_REG_SRC_DV_LNK_NUM_ID:
      field = &(regs->fcr.connectivity_map_registers_reg[offset].src_dv_lnk_num);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fcr_gtimer_trigger_reg_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    buffer,
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fcr");
  soc_sand_proc_name = "ui_fap21v_acc_fcr_gtimer_trigger_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_GTIMER_TRIGGER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FCR_GTIMER_TRIGGER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FCR_GTIMER_TRIGGER_REG_REGISTER_ID:
      regis = &(regs->fcr.gtimer_trigger_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_GTIMER_TRIGGER_REG_ACTIVATE_GTIMER_ID:
      field = &(regs->fcr.gtimer_trigger_reg.activate_gtimer);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "fcr.gtimer_trigger_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fcr_gtimer_trigger_reg_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  uint32
    ret;
  uint32
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fcr");
  soc_sand_proc_name = "ui_fap21v_acc_fcr_gtimer_trigger_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_GTIMER_TRIGGER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FCR_GTIMER_TRIGGER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FCR_GTIMER_TRIGGER_REG_REGISTER_ID:
      regis = &(regs->fcr.gtimer_trigger_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_GTIMER_TRIGGER_REG_ACTIVATE_GTIMER_ID:
      field = &(regs->fcr.gtimer_trigger_reg.activate_gtimer);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fcr_enable_petra_cell_formats_reg_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    offset = 0,
    buffer,
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fcr");
  soc_sand_proc_name = "ui_fap21v_acc_fcr_enable_petra_cell_formats_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_ENABLE_PETRA_CELL_FORMATS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FCR_ENABLE_PETRA_CELL_FORMATS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_ENABLE_PETRA_CELL_FORMATS_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_FCR_ENABLE_PETRA_CELL_FORMATS_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FCR_ENABLE_PETRA_CELL_FORMATS_REG_REGISTER_ID:
      regis = &(regs->fcr.enable_petra_cell_formats_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_ENABLE_PETRA_CELL_FORMATS_REG_FAP20B_EN_ID:
      field = &(regs->fcr.enable_petra_cell_formats_reg[offset].fap20b_en);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "fcr.enable_petra_cell_formats_reg***: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fcr_enable_petra_cell_formats_reg_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  uint32
    ret;
  uint32
    offset = 0,
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fcr");
  soc_sand_proc_name = "ui_fap21v_acc_fcr_enable_petra_cell_formats_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_ENABLE_PETRA_CELL_FORMATS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FCR_ENABLE_PETRA_CELL_FORMATS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_ENABLE_PETRA_CELL_FORMATS_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_FCR_ENABLE_PETRA_CELL_FORMATS_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FCR_ENABLE_PETRA_CELL_FORMATS_REG_REGISTER_ID:
      regis = &(regs->fcr.enable_petra_cell_formats_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_ENABLE_PETRA_CELL_FORMATS_REG_FAP20B_EN_ID:
      field = &(regs->fcr.enable_petra_cell_formats_reg[offset].fap20b_en);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fcr_programmable_cells_counter_reg_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    buffer,
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fcr");
  soc_sand_proc_name = "ui_fap21v_acc_fcr_programmable_cells_counter_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_PROGRAMMABLE_CELLS_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FCR_PROGRAMMABLE_CELLS_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FCR_PROGRAMMABLE_CELLS_COUNTER_REG_REGISTER_ID:
      regis = &(regs->fcr.programmable_cells_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_PROGRAMMABLE_CELLS_COUNTER_REG_PRG_CTRL_CELL_CNT_ID:
      field = &(regs->fcr.programmable_cells_counter_reg.prg_ctrl_cell_cnt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_PROGRAMMABLE_CELLS_COUNTER_REG_PRG_CTRL_CELLCNT_CO_ID:
      field = &(regs->fcr.programmable_cells_counter_reg.prg_ctrl_cellcnt_co);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "fcr.programmable_cells_counter_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fcr_programmable_cells_counter_reg_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  uint32
    ret;
  uint32
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fcr");
  soc_sand_proc_name = "ui_fap21v_acc_fcr_programmable_cells_counter_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_PROGRAMMABLE_CELLS_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FCR_PROGRAMMABLE_CELLS_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FCR_PROGRAMMABLE_CELLS_COUNTER_REG_REGISTER_ID:
      regis = &(regs->fcr.programmable_cells_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_PROGRAMMABLE_CELLS_COUNTER_REG_PRG_CTRL_CELL_CNT_ID:
      field = &(regs->fcr.programmable_cells_counter_reg.prg_ctrl_cell_cnt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_PROGRAMMABLE_CELLS_COUNTER_REG_PRG_CTRL_CELLCNT_CO_ID:
      field = &(regs->fcr.programmable_cells_counter_reg.prg_ctrl_cellcnt_co);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fcr_connectivity_map_change_event_registers_reg_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    buffer,
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fcr");
  soc_sand_proc_name = "ui_fap21v_acc_fcr_connectivity_map_change_event_registers_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_CONNECTIVITY_MAP_CHANGE_EVENT_REGISTERS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FCR_CONNECTIVITY_MAP_CHANGE_EVENT_REGISTERS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FCR_CONNECTIVITY_MAP_CHANGE_EVENT_REGISTERS_REG_REGISTER_ID:
      regis = &(regs->fcr.connectivity_map_change_event_registers_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_CONNECTIVITY_MAP_CHANGE_EVENT_REGISTERS_REG_SRC_DV_CNG_LINK_ID:
      field = &(regs->fcr.connectivity_map_change_event_registers_reg.src_dv_cng_link);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "fcr.connectivity_map_change_event_registers_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fcr_connectivity_map_change_event_registers_reg_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  uint32
    ret;
  uint32
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fcr");
  soc_sand_proc_name = "ui_fap21v_acc_fcr_connectivity_map_change_event_registers_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_CONNECTIVITY_MAP_CHANGE_EVENT_REGISTERS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FCR_CONNECTIVITY_MAP_CHANGE_EVENT_REGISTERS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FCR_CONNECTIVITY_MAP_CHANGE_EVENT_REGISTERS_REG_REGISTER_ID:
      regis = &(regs->fcr.connectivity_map_change_event_registers_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_CONNECTIVITY_MAP_CHANGE_EVENT_REGISTERS_REG_SRC_DV_CNG_LINK_ID:
      field = &(regs->fcr.connectivity_map_change_event_registers_reg.src_dv_cng_link);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fcr_reach_cells_counter_reg_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    buffer,
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fcr");
  soc_sand_proc_name = "ui_fap21v_acc_fcr_reach_cells_counter_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_REACH_CELLS_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FCR_REACH_CELLS_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FCR_REACH_CELLS_COUNTER_REG_REGISTER_ID:
      regis = &(regs->fcr.reach_cells_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_REACH_CELLS_COUNTER_REG_REACH_CELL_COUNT_ID:
      field = &(regs->fcr.reach_cells_counter_reg.reach_cell_count);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_REACH_CELLS_COUNTER_REG_REACH_CELL_COUNT_OVF_ID:
      field = &(regs->fcr.reach_cells_counter_reg.reach_cell_count_ovf);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "fcr.reach_cells_counter_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fcr_reach_cells_counter_reg_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  uint32
    ret;
  uint32
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fcr");
  soc_sand_proc_name = "ui_fap21v_acc_fcr_reach_cells_counter_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_REACH_CELLS_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FCR_REACH_CELLS_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FCR_REACH_CELLS_COUNTER_REG_REGISTER_ID:
      regis = &(regs->fcr.reach_cells_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_REACH_CELLS_COUNTER_REG_REACH_CELL_COUNT_ID:
      field = &(regs->fcr.reach_cells_counter_reg.reach_cell_count);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCR_REACH_CELLS_COUNTER_REG_REACH_CELL_COUNT_OVF_ID:
      field = &(regs->fcr.reach_cells_counter_reg.reach_cell_count_ovf);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fcr_get(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fcr_get");

  if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_INTERRUPT_MASKS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fcr_interrupt_masks_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_FLOW_STATUS_CELLS_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fcr_flow_status_cells_counter_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_GTIMER_CONFIGURATION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fcr_gtimer_configuration_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_CREDIT_CELLS_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fcr_credit_cells_counter_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_FCR_ENABLERS_AND_FILTER_MATCH_INPUT_LINK_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fcr_fcr_enablers_and_filter_match_input_link_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_PROGRAMMABLE_CONTROL_CELL_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fcr_programmable_control_cell_counter_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_INTERRUPT_SOURCES_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fcr_interrupt_sources_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_PROGRAMMABLE_CONTROL_CELL_COUNTER_MASK_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fcr_programmable_control_cell_counter_mask_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_TOTAL_CELLS_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fcr_total_cells_counter_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_CONNECTIVITY_MAP_REGISTERS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fcr_connectivity_map_registers_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_GTIMER_TRIGGER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fcr_gtimer_trigger_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_ENABLE_PETRA_CELL_FORMATS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fcr_enable_petra_cell_formats_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_PROGRAMMABLE_CELLS_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fcr_programmable_cells_counter_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_CONNECTIVITY_MAP_CHANGE_EVENT_REGISTERS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fcr_connectivity_map_change_event_registers_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_REACH_CELLS_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fcr_reach_cells_counter_reg_get(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after fcr***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fcr_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fcr_set");

  if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_INTERRUPT_MASKS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fcr_interrupt_masks_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_FLOW_STATUS_CELLS_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fcr_flow_status_cells_counter_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_GTIMER_CONFIGURATION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fcr_gtimer_configuration_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_CREDIT_CELLS_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fcr_credit_cells_counter_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_FCR_ENABLERS_AND_FILTER_MATCH_INPUT_LINK_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fcr_fcr_enablers_and_filter_match_input_link_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_PROGRAMMABLE_CONTROL_CELL_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fcr_programmable_control_cell_counter_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_INTERRUPT_SOURCES_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fcr_interrupt_sources_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_PROGRAMMABLE_CONTROL_CELL_COUNTER_MASK_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fcr_programmable_control_cell_counter_mask_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_TOTAL_CELLS_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fcr_total_cells_counter_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_CONNECTIVITY_MAP_REGISTERS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fcr_connectivity_map_registers_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_GTIMER_TRIGGER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fcr_gtimer_trigger_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_ENABLE_PETRA_CELL_FORMATS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fcr_enable_petra_cell_formats_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_PROGRAMMABLE_CELLS_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fcr_programmable_cells_counter_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_CONNECTIVITY_MAP_CHANGE_EVENT_REGISTERS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fcr_connectivity_map_change_event_registers_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCR_REACH_CELLS_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fcr_reach_cells_counter_reg_set(current_line, value);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after fcr***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}
