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
  ui_fap21v_acc_sla_obfcb_dip2_alarm_threshold_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_obfcb_dip2_alarm_threshold_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_DIP2_ALARM_THRESHOLD_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_DIP2_ALARM_THRESHOLD_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_DIP2_ALARM_THRESHOLD_REG_REGISTER_ID:
      regis = &(regs->sla.obfcb_dip2_alarm_threshold_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_DIP2_ALARM_THRESHOLD_REG_OBFCB_DIP2_ALARM_TH_ID:
      field = &(regs->sla.obfcb_dip2_alarm_threshold_reg.obfcb_dip2_alarm_th);
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
  soc_sand_os_printf( "sla.obfcb_dip2_alarm_threshold_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sla_obfcb_dip2_alarm_threshold_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_obfcb_dip2_alarm_threshold_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_DIP2_ALARM_THRESHOLD_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_DIP2_ALARM_THRESHOLD_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_DIP2_ALARM_THRESHOLD_REG_REGISTER_ID:
      regis = &(regs->sla.obfcb_dip2_alarm_threshold_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_DIP2_ALARM_THRESHOLD_REG_OBFCB_DIP2_ALARM_TH_ID:
      field = &(regs->sla.obfcb_dip2_alarm_threshold_reg.obfcb_dip2_alarm_th);
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
  ui_fap21v_acc_sla_obfcb_max_framing_pattern_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_obfcb_max_framing_pattern_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_MAX_FRAMING_PATTERN_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_MAX_FRAMING_PATTERN_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_MAX_FRAMING_PATTERN_REG_REGISTER_ID:
      regis = &(regs->sla.obfcb_max_framing_pattern_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_MAX_FRAMING_PATTERN_REG_OBFCB_MAX_FRM_PATTERN_ID:
      field = &(regs->sla.obfcb_max_framing_pattern_reg.obfcb_max_frm_pattern);
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
  soc_sand_os_printf( "sla.obfcb_max_framing_pattern_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sla_obfcb_max_framing_pattern_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_obfcb_max_framing_pattern_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_MAX_FRAMING_PATTERN_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_MAX_FRAMING_PATTERN_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_MAX_FRAMING_PATTERN_REG_REGISTER_ID:
      regis = &(regs->sla.obfcb_max_framing_pattern_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_MAX_FRAMING_PATTERN_REG_OBFCB_MAX_FRM_PATTERN_ID:
      field = &(regs->sla.obfcb_max_framing_pattern_reg.obfcb_max_frm_pattern);
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
  ui_fap21v_acc_sla_fifo_status_0_from_primary_spi4_fcs_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_fifo_status_0_from_primary_spi4_fcs_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_FIFO_STATUS_0_FROM_PRIMARY_SPI4_FCS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_FIFO_STATUS_0_FROM_PRIMARY_SPI4_FCS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_FIFO_STATUS_0_FROM_PRIMARY_SPI4_FCS_REG_REGISTER_ID:
      regis = &(regs->sla.fifo_status_0_from_primary_spi4_fcs_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_FIFO_STATUS_0_FROM_PRIMARY_SPI4_FCS_REG_FFSTAT_FROM_PRIM0_ID:
      field = &(regs->sla.fifo_status_0_from_primary_spi4_fcs_reg.ffstat_from_prim0);
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
  soc_sand_os_printf( "sla.fifo_status_0_from_primary_spi4_fcs_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sla_fifo_status_0_from_primary_spi4_fcs_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_fifo_status_0_from_primary_spi4_fcs_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_FIFO_STATUS_0_FROM_PRIMARY_SPI4_FCS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_FIFO_STATUS_0_FROM_PRIMARY_SPI4_FCS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_FIFO_STATUS_0_FROM_PRIMARY_SPI4_FCS_REG_REGISTER_ID:
      regis = &(regs->sla.fifo_status_0_from_primary_spi4_fcs_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_FIFO_STATUS_0_FROM_PRIMARY_SPI4_FCS_REG_FFSTAT_FROM_PRIM0_ID:
      field = &(regs->sla.fifo_status_0_from_primary_spi4_fcs_reg.ffstat_from_prim0);
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
  ui_fap21v_acc_sla_obfc_calendar_m_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_obfc_calendar_m_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFC_CALENDAR_M_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_OBFC_CALENDAR_M_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFC_CALENDAR_M_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_OBFC_CALENDAR_M_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_OBFC_CALENDAR_M_REG_REGISTER_ID:
      regis = &(regs->sla.obfc_calendar_m_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_OBFC_CALENDAR_M_REG_OBFC_CALENDAR_M_ID:
      field = &(regs->sla.obfc_calendar_m_reg[offset].obfc_calendar_m);
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
  soc_sand_os_printf( "sla.obfc_calendar_m_reg***: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sla_obfc_calendar_m_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_obfc_calendar_m_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFC_CALENDAR_M_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_OBFC_CALENDAR_M_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFC_CALENDAR_M_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_OBFC_CALENDAR_M_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_OBFC_CALENDAR_M_REG_REGISTER_ID:
      regis = &(regs->sla.obfc_calendar_m_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_OBFC_CALENDAR_M_REG_OBFC_CALENDAR_M_ID:
      field = &(regs->sla.obfc_calendar_m_reg[offset].obfc_calendar_m);
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
  ui_fap21v_acc_sla_spi4_transfer_control_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_spi4_transfer_control_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_SPI4_TRANSFER_CONTROL_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_SPI4_TRANSFER_CONTROL_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_SPI4_TRANSFER_CONTROL_REG_REGISTER_ID:
      regis = &(regs->sla.spi4_transfer_control_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_SPI4_TRANSFER_CONTROL_REG_BURST_SIZE_ID:
      field = &(regs->sla.spi4_transfer_control_reg.burst_size);
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
  soc_sand_os_printf( "sla.spi4_transfer_control_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sla_spi4_transfer_control_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_spi4_transfer_control_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_SPI4_TRANSFER_CONTROL_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_SPI4_TRANSFER_CONTROL_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_SPI4_TRANSFER_CONTROL_REG_REGISTER_ID:
      regis = &(regs->sla.spi4_transfer_control_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_SPI4_TRANSFER_CONTROL_REG_BURST_SIZE_ID:
      field = &(regs->sla.spi4_transfer_control_reg.burst_size);
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
  ui_fap21v_acc_sla_dip2_error_cnt_out_of_band_flow_control_b_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_dip2_error_cnt_out_of_band_flow_control_b_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_DIP2_ERROR_CNT_OUT_OF_BAND_FLOW_CONTROL_B_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_DIP2_ERROR_CNT_OUT_OF_BAND_FLOW_CONTROL_B_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_DIP2_ERROR_CNT_OUT_OF_BAND_FLOW_CONTROL_B_REG_REGISTER_ID:
      regis = &(regs->sla.dip2_error_cnt_out_of_band_flow_control_b_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_DIP2_ERROR_CNT_OUT_OF_BAND_FLOW_CONTROL_B_REG_OBFCB_DIP2_ERR_CNT_ID:
      field = &(regs->sla.dip2_error_cnt_out_of_band_flow_control_b_reg.obfcb_dip2_err_cnt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_DIP2_ERROR_CNT_OUT_OF_BAND_FLOW_CONTROL_B_REG_OBFCB_DIP2_ERR_CNT_O_ID:
      field = &(regs->sla.dip2_error_cnt_out_of_band_flow_control_b_reg.obfcb_dip2_err_cnt_o);
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
  soc_sand_os_printf( "sla.dip2_error_cnt_out_of_band_flow_control_b_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sla_dip2_error_cnt_out_of_band_flow_control_b_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_dip2_error_cnt_out_of_band_flow_control_b_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_DIP2_ERROR_CNT_OUT_OF_BAND_FLOW_CONTROL_B_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_DIP2_ERROR_CNT_OUT_OF_BAND_FLOW_CONTROL_B_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_DIP2_ERROR_CNT_OUT_OF_BAND_FLOW_CONTROL_B_REG_REGISTER_ID:
      regis = &(regs->sla.dip2_error_cnt_out_of_band_flow_control_b_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_DIP2_ERROR_CNT_OUT_OF_BAND_FLOW_CONTROL_B_REG_OBFCB_DIP2_ERR_CNT_ID:
      field = &(regs->sla.dip2_error_cnt_out_of_band_flow_control_b_reg.obfcb_dip2_err_cnt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_DIP2_ERROR_CNT_OUT_OF_BAND_FLOW_CONTROL_B_REG_OBFCB_DIP2_ERR_CNT_O_ID:
      field = &(regs->sla.dip2_error_cnt_out_of_band_flow_control_b_reg.obfcb_dip2_err_cnt_o);
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
  ui_fap21v_acc_sla_obfca_tsclk_edge_select_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_obfca_tsclk_edge_select_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_TSCLK_EDGE_SELECT_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_TSCLK_EDGE_SELECT_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_TSCLK_EDGE_SELECT_REG_REGISTER_ID:
      regis = &(regs->sla.obfca_tsclk_edge_select_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_TSCLK_EDGE_SELECT_REG_OBFCA_TSCLKEDGE_SEL_ID:
      field = &(regs->sla.obfca_tsclk_edge_select_reg.obfca_tsclkedge_sel);
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
  soc_sand_os_printf( "sla.obfca_tsclk_edge_select_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sla_obfca_tsclk_edge_select_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_obfca_tsclk_edge_select_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_TSCLK_EDGE_SELECT_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_TSCLK_EDGE_SELECT_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_TSCLK_EDGE_SELECT_REG_REGISTER_ID:
      regis = &(regs->sla.obfca_tsclk_edge_select_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_TSCLK_EDGE_SELECT_REG_OBFCA_TSCLKEDGE_SEL_ID:
      field = &(regs->sla.obfca_tsclk_edge_select_reg.obfca_tsclkedge_sel);
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
  ui_fap21v_acc_sla_obfca_out_of_frame_threshold_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_obfca_out_of_frame_threshold_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_OUT_OF_FRAME_THRESHOLD_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_OUT_OF_FRAME_THRESHOLD_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_OUT_OF_FRAME_THRESHOLD_REG_REGISTER_ID:
      regis = &(regs->sla.obfca_out_of_frame_threshold_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_OUT_OF_FRAME_THRESHOLD_REG_OBFCA_OUT_OF_FRAME_TH_ID:
      field = &(regs->sla.obfca_out_of_frame_threshold_reg.obfca_out_of_frame_th);
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
  soc_sand_os_printf( "sla.obfca_out_of_frame_threshold_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sla_obfca_out_of_frame_threshold_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_obfca_out_of_frame_threshold_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_OUT_OF_FRAME_THRESHOLD_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_OUT_OF_FRAME_THRESHOLD_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_OUT_OF_FRAME_THRESHOLD_REG_REGISTER_ID:
      regis = &(regs->sla.obfca_out_of_frame_threshold_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_OUT_OF_FRAME_THRESHOLD_REG_OBFCA_OUT_OF_FRAME_TH_ID:
      field = &(regs->sla.obfca_out_of_frame_threshold_reg.obfca_out_of_frame_th);
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
  ui_fap21v_acc_sla_indirect_command_wr_data_reg_0_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_indirect_command_wr_data_reg_0_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_INDIRECT_COMMAND_WR_DATA_REG_0_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_INDIRECT_COMMAND_WR_DATA_REG_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_INDIRECT_COMMAND_WR_DATA_REG_0_REGISTER_ID:
      regis = &(regs->sla.indirect_command_wr_data_reg_0.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_INDIRECT_COMMAND_WR_DATA_REG_0_INDIRECT_COMMAND_WR_DATA_ID:
      field = &(regs->sla.indirect_command_wr_data_reg_0.indirect_command_wr_data);
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
  soc_sand_os_printf( "sla.indirect_command_wr_data_reg_0: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sla_indirect_command_wr_data_reg_0_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_indirect_command_wr_data_reg_0_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_INDIRECT_COMMAND_WR_DATA_REG_0_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_INDIRECT_COMMAND_WR_DATA_REG_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_INDIRECT_COMMAND_WR_DATA_REG_0_REGISTER_ID:
      regis = &(regs->sla.indirect_command_wr_data_reg_0.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_INDIRECT_COMMAND_WR_DATA_REG_0_INDIRECT_COMMAND_WR_DATA_ID:
      field = &(regs->sla.indirect_command_wr_data_reg_0.indirect_command_wr_data);
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
  ui_fap21v_acc_sla_obfca_dip2_alarm_threshold_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_obfca_dip2_alarm_threshold_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_DIP2_ALARM_THRESHOLD_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_DIP2_ALARM_THRESHOLD_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_DIP2_ALARM_THRESHOLD_REG_REGISTER_ID:
      regis = &(regs->sla.obfca_dip2_alarm_threshold_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_DIP2_ALARM_THRESHOLD_REG_OBFCA_DIP2_ALARM_TH_ID:
      field = &(regs->sla.obfca_dip2_alarm_threshold_reg.obfca_dip2_alarm_th);
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
  soc_sand_os_printf( "sla.obfca_dip2_alarm_threshold_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sla_obfca_dip2_alarm_threshold_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_obfca_dip2_alarm_threshold_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_DIP2_ALARM_THRESHOLD_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_DIP2_ALARM_THRESHOLD_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_DIP2_ALARM_THRESHOLD_REG_REGISTER_ID:
      regis = &(regs->sla.obfca_dip2_alarm_threshold_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_DIP2_ALARM_THRESHOLD_REG_OBFCA_DIP2_ALARM_TH_ID:
      field = &(regs->sla.obfca_dip2_alarm_threshold_reg.obfca_dip2_alarm_th);
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
  ui_fap21v_acc_sla_sla_interrupt_mask_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_sla_interrupt_mask_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_SLA_INTERRUPT_MASK_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_SLA_INTERRUPT_MASK_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_SLA_INTERRUPT_MASK_REG_REGISTER_ID:
      regis = &(regs->sla.sla_interrupt_mask_reg.addr);
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
  soc_sand_os_printf( "sla.sla_interrupt_mask_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sla_sla_interrupt_mask_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_sla_interrupt_mask_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_SLA_INTERRUPT_MASK_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_SLA_INTERRUPT_MASK_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_SLA_INTERRUPT_MASK_REG_REGISTER_ID:
      regis = &(regs->sla.sla_interrupt_mask_reg.addr);
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
  ui_fap21v_acc_sla_dip2_error_cnt_out_of_band_flow_control_a_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_dip2_error_cnt_out_of_band_flow_control_a_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_DIP2_ERROR_CNT_OUT_OF_BAND_FLOW_CONTROL_A_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_DIP2_ERROR_CNT_OUT_OF_BAND_FLOW_CONTROL_A_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_DIP2_ERROR_CNT_OUT_OF_BAND_FLOW_CONTROL_A_REG_REGISTER_ID:
      regis = &(regs->sla.dip2_error_cnt_out_of_band_flow_control_a_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_DIP2_ERROR_CNT_OUT_OF_BAND_FLOW_CONTROL_A_REG_OBFCA_DIP2_ERR_CNT_ID:
      field = &(regs->sla.dip2_error_cnt_out_of_band_flow_control_a_reg.obfca_dip2_err_cnt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_DIP2_ERROR_CNT_OUT_OF_BAND_FLOW_CONTROL_A_REG_OBFCA_DIP2_ERR_CNT_O_ID:
      field = &(regs->sla.dip2_error_cnt_out_of_band_flow_control_a_reg.obfca_dip2_err_cnt_o);
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
  soc_sand_os_printf( "sla.dip2_error_cnt_out_of_band_flow_control_a_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sla_dip2_error_cnt_out_of_band_flow_control_a_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_dip2_error_cnt_out_of_band_flow_control_a_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_DIP2_ERROR_CNT_OUT_OF_BAND_FLOW_CONTROL_A_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_DIP2_ERROR_CNT_OUT_OF_BAND_FLOW_CONTROL_A_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_DIP2_ERROR_CNT_OUT_OF_BAND_FLOW_CONTROL_A_REG_REGISTER_ID:
      regis = &(regs->sla.dip2_error_cnt_out_of_band_flow_control_a_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_DIP2_ERROR_CNT_OUT_OF_BAND_FLOW_CONTROL_A_REG_OBFCA_DIP2_ERR_CNT_ID:
      field = &(regs->sla.dip2_error_cnt_out_of_band_flow_control_a_reg.obfca_dip2_err_cnt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_DIP2_ERROR_CNT_OUT_OF_BAND_FLOW_CONTROL_A_REG_OBFCA_DIP2_ERR_CNT_O_ID:
      field = &(regs->sla.dip2_error_cnt_out_of_band_flow_control_a_reg.obfca_dip2_err_cnt_o);
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
  ui_fap21v_acc_sla_indirect_command_rd_data_reg_0_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_indirect_command_rd_data_reg_0_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_INDIRECT_COMMAND_RD_DATA_REG_0_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_INDIRECT_COMMAND_RD_DATA_REG_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_INDIRECT_COMMAND_RD_DATA_REG_0_REGISTER_ID:
      regis = &(regs->sla.indirect_command_rd_data_reg_0.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_INDIRECT_COMMAND_RD_DATA_REG_0_INDIRECT_COMMAND_RD_DATA_ID:
      field = &(regs->sla.indirect_command_rd_data_reg_0.indirect_command_rd_data);
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
  soc_sand_os_printf( "sla.indirect_command_rd_data_reg_0: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sla_indirect_command_rd_data_reg_0_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_indirect_command_rd_data_reg_0_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_INDIRECT_COMMAND_RD_DATA_REG_0_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_INDIRECT_COMMAND_RD_DATA_REG_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_INDIRECT_COMMAND_RD_DATA_REG_0_REGISTER_ID:
      regis = &(regs->sla.indirect_command_rd_data_reg_0.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_INDIRECT_COMMAND_RD_DATA_REG_0_INDIRECT_COMMAND_RD_DATA_ID:
      field = &(regs->sla.indirect_command_rd_data_reg_0.indirect_command_rd_data);
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
  ui_fap21v_acc_sla_obfca_max_consecutive_framing_pattern_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_obfca_max_consecutive_framing_pattern_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_MAX_CONSECUTIVE_FRAMING_PATTERN_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_MAX_CONSECUTIVE_FRAMING_PATTERN_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_MAX_CONSECUTIVE_FRAMING_PATTERN_REG_REGISTER_ID:
      regis = &(regs->sla.obfca_max_consecutive_framing_pattern_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_MAX_CONSECUTIVE_FRAMING_PATTERN_REG_OBFCA_MAX_FRM_PATTERN_ID:
      field = &(regs->sla.obfca_max_consecutive_framing_pattern_reg.obfca_max_frm_pattern);
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
  soc_sand_os_printf( "sla.obfca_max_consecutive_framing_pattern_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sla_obfca_max_consecutive_framing_pattern_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_obfca_max_consecutive_framing_pattern_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_MAX_CONSECUTIVE_FRAMING_PATTERN_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_MAX_CONSECUTIVE_FRAMING_PATTERN_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_MAX_CONSECUTIVE_FRAMING_PATTERN_REG_REGISTER_ID:
      regis = &(regs->sla.obfca_max_consecutive_framing_pattern_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_MAX_CONSECUTIVE_FRAMING_PATTERN_REG_OBFCA_MAX_FRM_PATTERN_ID:
      field = &(regs->sla.obfca_max_consecutive_framing_pattern_reg.obfca_max_frm_pattern);
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
  ui_fap21v_acc_sla_obfcb_dip2_good_to_bad_ratio_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_obfcb_dip2_good_to_bad_ratio_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_DIP2_GOOD_TO_BAD_RATIO_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_DIP2_GOOD_TO_BAD_RATIO_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_DIP2_GOOD_TO_BAD_RATIO_REG_REGISTER_ID:
      regis = &(regs->sla.obfcb_dip2_good_to_bad_ratio_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_DIP2_GOOD_TO_BAD_RATIO_REG_OBFCB_DIP2_GOOD_TO_BAD_RATIO_ID:
      field = &(regs->sla.obfcb_dip2_good_to_bad_ratio_reg.obfcb_dip2_good_to_bad_ratio);
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
  soc_sand_os_printf( "sla.obfcb_dip2_good_to_bad_ratio_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sla_obfcb_dip2_good_to_bad_ratio_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_obfcb_dip2_good_to_bad_ratio_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_DIP2_GOOD_TO_BAD_RATIO_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_DIP2_GOOD_TO_BAD_RATIO_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_DIP2_GOOD_TO_BAD_RATIO_REG_REGISTER_ID:
      regis = &(regs->sla.obfcb_dip2_good_to_bad_ratio_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_DIP2_GOOD_TO_BAD_RATIO_REG_OBFCB_DIP2_GOOD_TO_BAD_RATIO_ID:
      field = &(regs->sla.obfcb_dip2_good_to_bad_ratio_reg.obfcb_dip2_good_to_bad_ratio);
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
  ui_fap21v_acc_sla_sla_initialization_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_sla_initialization_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_SLA_INITIALIZATION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_SLA_INITIALIZATION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_SLA_INITIALIZATION_REG_REGISTER_ID:
      regis = &(regs->sla.sla_initialization_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_SLA_INITIALIZATION_REG_SLABLOCK_INIT_ID:
      field = &(regs->sla.sla_initialization_reg.slablock_init);
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
  soc_sand_os_printf( "sla.sla_initialization_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sla_sla_initialization_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_sla_initialization_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_SLA_INITIALIZATION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_SLA_INITIALIZATION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_SLA_INITIALIZATION_REG_REGISTER_ID:
      regis = &(regs->sla.sla_initialization_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_SLA_INITIALIZATION_REG_SLABLOCK_INIT_ID:
      field = &(regs->sla.sla_initialization_reg.slablock_init);
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
  ui_fap21v_acc_sla_spi4_sel_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_spi4_sel_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_SPI4_SEL_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_SPI4_SEL_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_SPI4_SEL_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_SPI4_SEL_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_SPI4_SEL_REG_REGISTER_ID:
      regis = &(regs->sla.spi4_sel_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_SPI4_SEL_REG_SPI4_SEL_ID:
      field = &(regs->sla.spi4_sel_reg[offset].spi4_sel);
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
  soc_sand_os_printf( "sla.spi4_sel_reg***: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sla_spi4_sel_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_spi4_sel_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_SPI4_SEL_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_SPI4_SEL_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_SPI4_SEL_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_SPI4_SEL_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_SPI4_SEL_REG_REGISTER_ID:
      regis = &(regs->sla.spi4_sel_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_SPI4_SEL_REG_SPI4_SEL_ID:
      field = &(regs->sla.spi4_sel_reg[offset].spi4_sel);
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
  ui_fap21v_acc_sla_flow_control_configurations_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_flow_control_configurations_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_FLOW_CONTROL_CONFIGURATIONS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_FLOW_CONTROL_CONFIGURATIONS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_FLOW_CONTROL_CONFIGURATIONS_REG_REGISTER_ID:
      regis = &(regs->sla.flow_control_configurations_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_FLOW_CONTROL_CONFIGURATIONS_REG_IBF_CA_ENA_ID:
      field = &(regs->sla.flow_control_configurations_reg.ibf_ca_ena);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_FLOW_CONTROL_CONFIGURATIONS_REG_IBF_CB_ENA_ID:
      field = &(regs->sla.flow_control_configurations_reg.ibf_cb_ena);
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
  soc_sand_os_printf( "sla.flow_control_configurations_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sla_flow_control_configurations_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_flow_control_configurations_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_FLOW_CONTROL_CONFIGURATIONS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_FLOW_CONTROL_CONFIGURATIONS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_FLOW_CONTROL_CONFIGURATIONS_REG_REGISTER_ID:
      regis = &(regs->sla.flow_control_configurations_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_FLOW_CONTROL_CONFIGURATIONS_REG_IBF_CA_ENA_ID:
      field = &(regs->sla.flow_control_configurations_reg.ibf_ca_ena);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_FLOW_CONTROL_CONFIGURATIONS_REG_IBF_CB_ENA_ID:
      field = &(regs->sla.flow_control_configurations_reg.ibf_cb_ena);
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
  ui_fap21v_acc_sla_fifo_status_0_from_auxiliary_flow_control_channel_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_fifo_status_0_from_auxiliary_flow_control_channel_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_FIFO_STATUS_0_FROM_AUXILIARY_FLOW_CONTROL_CHANNEL_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_FIFO_STATUS_0_FROM_AUXILIARY_FLOW_CONTROL_CHANNEL_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_FIFO_STATUS_0_FROM_AUXILIARY_FLOW_CONTROL_CHANNEL_REG_REGISTER_ID:
      regis = &(regs->sla.fifo_status_0_from_auxiliary_flow_control_channel_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_FIFO_STATUS_0_FROM_AUXILIARY_FLOW_CONTROL_CHANNEL_REG_FFSTAT_FROM_AUX_0_ID:
      field = &(regs->sla.fifo_status_0_from_auxiliary_flow_control_channel_reg.ffstat_from_aux_0);
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
  soc_sand_os_printf( "sla.fifo_status_0_from_auxiliary_flow_control_channel_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sla_fifo_status_0_from_auxiliary_flow_control_channel_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_fifo_status_0_from_auxiliary_flow_control_channel_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_FIFO_STATUS_0_FROM_AUXILIARY_FLOW_CONTROL_CHANNEL_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_FIFO_STATUS_0_FROM_AUXILIARY_FLOW_CONTROL_CHANNEL_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_FIFO_STATUS_0_FROM_AUXILIARY_FLOW_CONTROL_CHANNEL_REG_REGISTER_ID:
      regis = &(regs->sla.fifo_status_0_from_auxiliary_flow_control_channel_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_FIFO_STATUS_0_FROM_AUXILIARY_FLOW_CONTROL_CHANNEL_REG_FFSTAT_FROM_AUX_0_ID:
      field = &(regs->sla.fifo_status_0_from_auxiliary_flow_control_channel_reg.ffstat_from_aux_0);
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
  ui_fap21v_acc_sla_fifo_status_1_from_auxiliary_flow_control_channel_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_fifo_status_1_from_auxiliary_flow_control_channel_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_FIFO_STATUS_1_FROM_AUXILIARY_FLOW_CONTROL_CHANNEL_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_FIFO_STATUS_1_FROM_AUXILIARY_FLOW_CONTROL_CHANNEL_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_FIFO_STATUS_1_FROM_AUXILIARY_FLOW_CONTROL_CHANNEL_REG_REGISTER_ID:
      regis = &(regs->sla.fifo_status_1_from_auxiliary_flow_control_channel_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_FIFO_STATUS_1_FROM_AUXILIARY_FLOW_CONTROL_CHANNEL_REG_FFSTAT_FROM_AUX_1_ID:
      field = &(regs->sla.fifo_status_1_from_auxiliary_flow_control_channel_reg.ffstat_from_aux_1);
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
  soc_sand_os_printf( "sla.fifo_status_1_from_auxiliary_flow_control_channel_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sla_fifo_status_1_from_auxiliary_flow_control_channel_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_fifo_status_1_from_auxiliary_flow_control_channel_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_FIFO_STATUS_1_FROM_AUXILIARY_FLOW_CONTROL_CHANNEL_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_FIFO_STATUS_1_FROM_AUXILIARY_FLOW_CONTROL_CHANNEL_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_FIFO_STATUS_1_FROM_AUXILIARY_FLOW_CONTROL_CHANNEL_REG_REGISTER_ID:
      regis = &(regs->sla.fifo_status_1_from_auxiliary_flow_control_channel_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_FIFO_STATUS_1_FROM_AUXILIARY_FLOW_CONTROL_CHANNEL_REG_FFSTAT_FROM_AUX_1_ID:
      field = &(regs->sla.fifo_status_1_from_auxiliary_flow_control_channel_reg.ffstat_from_aux_1);
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
  ui_fap21v_acc_sla_general_configuration_register_out_of_band_flow_control_for_spi4_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_general_configuration_register_out_of_band_flow_control_for_spi4_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_GENERAL_CONFIGURATION_REGISTER_OUT_OF_BAND_FLOW_CONTROL_FOR_SPI4_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_GENERAL_CONFIGURATION_REGISTER_OUT_OF_BAND_FLOW_CONTROL_FOR_SPI4_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_GENERAL_CONFIGURATION_REGISTER_OUT_OF_BAND_FLOW_CONTROL_FOR_SPI4_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_GENERAL_CONFIGURATION_REGISTER_OUT_OF_BAND_FLOW_CONTROL_FOR_SPI4_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_GENERAL_CONFIGURATION_REGISTER_OUT_OF_BAND_FLOW_CONTROL_FOR_SPI4_REG_REGISTER_ID:
      regis = &(regs->sla.general_configuration_register_out_of_band_flow_control_for_spi4_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_GENERAL_CONFIGURATION_REGISTER_OUT_OF_BAND_FLOW_CONTROL_FOR_SPI4_REG_OBFC_ENA_ID:
      field = &(regs->sla.general_configuration_register_out_of_band_flow_control_for_spi4_reg[offset].obfc_ena);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_GENERAL_CONFIGURATION_REGISTER_OUT_OF_BAND_FLOW_CONTROL_FOR_SPI4_REG_OBFC_LBENA_ID:
      field = &(regs->sla.general_configuration_register_out_of_band_flow_control_for_spi4_reg[offset].obfc_lbena);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_GENERAL_CONFIGURATION_REGISTER_OUT_OF_BAND_FLOW_CONTROL_FOR_SPI4_REG_OBFC_ASYNC_RSTN_ID:
      field = &(regs->sla.general_configuration_register_out_of_band_flow_control_for_spi4_reg[offset].obfc_async_rstn);
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
  soc_sand_os_printf( "sla.general_configuration_register_out_of_band_flow_control_for_spi4_reg***: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sla_general_configuration_register_out_of_band_flow_control_for_spi4_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_general_configuration_register_out_of_band_flow_control_for_spi4_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_GENERAL_CONFIGURATION_REGISTER_OUT_OF_BAND_FLOW_CONTROL_FOR_SPI4_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_GENERAL_CONFIGURATION_REGISTER_OUT_OF_BAND_FLOW_CONTROL_FOR_SPI4_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_GENERAL_CONFIGURATION_REGISTER_OUT_OF_BAND_FLOW_CONTROL_FOR_SPI4_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_GENERAL_CONFIGURATION_REGISTER_OUT_OF_BAND_FLOW_CONTROL_FOR_SPI4_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_GENERAL_CONFIGURATION_REGISTER_OUT_OF_BAND_FLOW_CONTROL_FOR_SPI4_REG_REGISTER_ID:
      regis = &(regs->sla.general_configuration_register_out_of_band_flow_control_for_spi4_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_GENERAL_CONFIGURATION_REGISTER_OUT_OF_BAND_FLOW_CONTROL_FOR_SPI4_REG_OBFC_ENA_ID:
      field = &(regs->sla.general_configuration_register_out_of_band_flow_control_for_spi4_reg[offset].obfc_ena);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_GENERAL_CONFIGURATION_REGISTER_OUT_OF_BAND_FLOW_CONTROL_FOR_SPI4_REG_OBFC_LBENA_ID:
      field = &(regs->sla.general_configuration_register_out_of_band_flow_control_for_spi4_reg[offset].obfc_lbena);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_GENERAL_CONFIGURATION_REGISTER_OUT_OF_BAND_FLOW_CONTROL_FOR_SPI4_REG_OBFC_ASYNC_RSTN_ID:
      field = &(regs->sla.general_configuration_register_out_of_band_flow_control_for_spi4_reg[offset].obfc_async_rstn);
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
  ui_fap21v_acc_sla_otm_headers_in_petra_mode_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_otm_headers_in_petra_mode_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OTM_HEADERS_IN_PETRA_MODE_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_OTM_HEADERS_IN_PETRA_MODE_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_OTM_HEADERS_IN_PETRA_MODE_REG_REGISTER_ID:
      regis = &(regs->sla.otm_headers_in_petra_mode_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_OTM_HEADERS_IN_PETRA_MODE_REG_OTMPETRAEN_ID:
      field = &(regs->sla.otm_headers_in_petra_mode_reg.otmpetraen);
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
  soc_sand_os_printf( "sla.otm_headers_in_petra_mode_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sla_otm_headers_in_petra_mode_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_otm_headers_in_petra_mode_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OTM_HEADERS_IN_PETRA_MODE_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_OTM_HEADERS_IN_PETRA_MODE_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_OTM_HEADERS_IN_PETRA_MODE_REG_REGISTER_ID:
      regis = &(regs->sla.otm_headers_in_petra_mode_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_OTM_HEADERS_IN_PETRA_MODE_REG_OTMPETRAEN_ID:
      field = &(regs->sla.otm_headers_in_petra_mode_reg.otmpetraen);
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
  ui_fap21v_acc_sla_fabric_header_in_fap20b_mode_and_otm_header_in_petra_mode_configuration_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_fabric_header_in_fap20b_mode_and_otm_header_in_petra_mode_configuration_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_FABRIC_HEADER_IN_FAP20B_MODE_AND_OTM_HEADER_IN_PETRA_MODE_CONFIGURATION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_FABRIC_HEADER_IN_FAP20B_MODE_AND_OTM_HEADER_IN_PETRA_MODE_CONFIGURATION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_FABRIC_HEADER_IN_FAP20B_MODE_AND_OTM_HEADER_IN_PETRA_MODE_CONFIGURATION_REG_REGISTER_ID:
      regis = &(regs->sla.fabric_header_in_fap20b_mode_and_otm_header_in_petra_mode_configuration_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_FABRIC_HEADER_IN_FAP20B_MODE_AND_OTM_HEADER_IN_PETRA_MODE_CONFIGURATION_REG_FAP20OTM_PETRA_MASK_SRC_IF_ID:
      field = &(regs->sla.fabric_header_in_fap20b_mode_and_otm_header_in_petra_mode_configuration_reg.fap20otm_petra_mask_src_if);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_FABRIC_HEADER_IN_FAP20B_MODE_AND_OTM_HEADER_IN_PETRA_MODE_CONFIGURATION_REG_FAP20OTM_PETRA_FAP_ID_SEL_ID:
      field = &(regs->sla.fabric_header_in_fap20b_mode_and_otm_header_in_petra_mode_configuration_reg.fap20otm_petra_fap_id_sel);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_FABRIC_HEADER_IN_FAP20B_MODE_AND_OTM_HEADER_IN_PETRA_MODE_CONFIGURATION_REG_FAP20OTM_PETRA_UNI_SRC_TYPE_ID:
      field = &(regs->sla.fabric_header_in_fap20b_mode_and_otm_header_in_petra_mode_configuration_reg.fap20otm_petra_uni_src_type);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_FABRIC_HEADER_IN_FAP20B_MODE_AND_OTM_HEADER_IN_PETRA_MODE_CONFIGURATION_REG_FAP20_OTM_PETRA_MUL_CLASS_8_TO_4_ID:
      field = &(regs->sla.fabric_header_in_fap20b_mode_and_otm_header_in_petra_mode_configuration_reg.Fap20_Otm_Petra_Mul_Class_8_to_4);
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
  soc_sand_os_printf( "sla.fabric_header_in_fap20b_mode_and_otm_header_in_petra_mode_configuration_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sla_fabric_header_in_fap20b_mode_and_otm_header_in_petra_mode_configuration_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_fabric_header_in_fap20b_mode_and_otm_header_in_petra_mode_configuration_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_FABRIC_HEADER_IN_FAP20B_MODE_AND_OTM_HEADER_IN_PETRA_MODE_CONFIGURATION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_FABRIC_HEADER_IN_FAP20B_MODE_AND_OTM_HEADER_IN_PETRA_MODE_CONFIGURATION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_FABRIC_HEADER_IN_FAP20B_MODE_AND_OTM_HEADER_IN_PETRA_MODE_CONFIGURATION_REG_REGISTER_ID:
      regis = &(regs->sla.fabric_header_in_fap20b_mode_and_otm_header_in_petra_mode_configuration_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_FABRIC_HEADER_IN_FAP20B_MODE_AND_OTM_HEADER_IN_PETRA_MODE_CONFIGURATION_REG_FAP20OTM_PETRA_MASK_SRC_IF_ID:
      field = &(regs->sla.fabric_header_in_fap20b_mode_and_otm_header_in_petra_mode_configuration_reg.fap20otm_petra_mask_src_if);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_FABRIC_HEADER_IN_FAP20B_MODE_AND_OTM_HEADER_IN_PETRA_MODE_CONFIGURATION_REG_FAP20OTM_PETRA_FAP_ID_SEL_ID:
      field = &(regs->sla.fabric_header_in_fap20b_mode_and_otm_header_in_petra_mode_configuration_reg.fap20otm_petra_fap_id_sel);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_FABRIC_HEADER_IN_FAP20B_MODE_AND_OTM_HEADER_IN_PETRA_MODE_CONFIGURATION_REG_FAP20OTM_PETRA_UNI_SRC_TYPE_ID:
      field = &(regs->sla.fabric_header_in_fap20b_mode_and_otm_header_in_petra_mode_configuration_reg.fap20otm_petra_uni_src_type);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_FABRIC_HEADER_IN_FAP20B_MODE_AND_OTM_HEADER_IN_PETRA_MODE_CONFIGURATION_REG_FAP20_OTM_PETRA_MUL_CLASS_8_TO_4_ID:
      field = &(regs->sla.fabric_header_in_fap20b_mode_and_otm_header_in_petra_mode_configuration_reg.Fap20_Otm_Petra_Mul_Class_8_to_4);
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
  ui_fap21v_acc_sla_cpu_and_lbp_valid_flags_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_cpu_and_lbp_valid_flags_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_CPU_AND_LBP_VALID_FLAGS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_CPU_AND_LBP_VALID_FLAGS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_CPU_AND_LBP_VALID_FLAGS_REG_REGISTER_ID:
      regis = &(regs->sla.cpu_and_lbp_valid_flags_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_CPU_AND_LBP_VALID_FLAGS_REG_CPUVALID_ID:
      field = &(regs->sla.cpu_and_lbp_valid_flags_reg.cpuvalid);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_CPU_AND_LBP_VALID_FLAGS_REG_LBPVALID_ID:
      field = &(regs->sla.cpu_and_lbp_valid_flags_reg.lbpvalid);
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
  soc_sand_os_printf( "sla.cpu_and_lbp_valid_flags_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sla_cpu_and_lbp_valid_flags_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_cpu_and_lbp_valid_flags_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_CPU_AND_LBP_VALID_FLAGS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_CPU_AND_LBP_VALID_FLAGS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_CPU_AND_LBP_VALID_FLAGS_REG_REGISTER_ID:
      regis = &(regs->sla.cpu_and_lbp_valid_flags_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_CPU_AND_LBP_VALID_FLAGS_REG_CPUVALID_ID:
      field = &(regs->sla.cpu_and_lbp_valid_flags_reg.cpuvalid);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_CPU_AND_LBP_VALID_FLAGS_REG_LBPVALID_ID:
      field = &(regs->sla.cpu_and_lbp_valid_flags_reg.lbpvalid);
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
  ui_fap21v_acc_sla_cpu_packet_control_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_cpu_packet_control_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_CPU_PACKET_CONTROL_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_CPU_PACKET_CONTROL_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_CPU_PACKET_CONTROL_REG_REGISTER_ID:
      regis = &(regs->sla.cpu_packet_control_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_CPU_PACKET_CONTROL_REG_START_OF_PACKET_ID:
      field = &(regs->sla.cpu_packet_control_reg.start_of_packet);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_CPU_PACKET_CONTROL_REG_END_OF_PACKET_ID:
      field = &(regs->sla.cpu_packet_control_reg.end_of_packet);
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
  soc_sand_os_printf( "sla.cpu_packet_control_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sla_cpu_packet_control_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_cpu_packet_control_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_CPU_PACKET_CONTROL_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_CPU_PACKET_CONTROL_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_CPU_PACKET_CONTROL_REG_REGISTER_ID:
      regis = &(regs->sla.cpu_packet_control_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_CPU_PACKET_CONTROL_REG_START_OF_PACKET_ID:
      field = &(regs->sla.cpu_packet_control_reg.start_of_packet);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_CPU_PACKET_CONTROL_REG_END_OF_PACKET_ID:
      field = &(regs->sla.cpu_packet_control_reg.end_of_packet);
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
  ui_fap21v_acc_sla_number_of_lost_tdm_cells_in_interface_b_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_number_of_lost_tdm_cells_in_interface_b_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_NUMBER_OF_LOST_TDM_CELLS_IN_INTERFACE_B_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_NUMBER_OF_LOST_TDM_CELLS_IN_INTERFACE_B_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_NUMBER_OF_LOST_TDM_CELLS_IN_INTERFACE_B_REG_REGISTER_ID:
      regis = &(regs->sla.number_of_lost_tdm_cells_in_interface_b_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_NUMBER_OF_LOST_TDM_CELLS_IN_INTERFACE_B_REG_TDMFFB_LOST_CNT_ID:
      field = &(regs->sla.number_of_lost_tdm_cells_in_interface_b_reg.tdmffb_lost_cnt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_NUMBER_OF_LOST_TDM_CELLS_IN_INTERFACE_B_REG_TDMFFB_LOST_CNT_O_ID:
      field = &(regs->sla.number_of_lost_tdm_cells_in_interface_b_reg.tdmffb_lost_cnt_o);
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
  soc_sand_os_printf( "sla.number_of_lost_tdm_cells_in_interface_b_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sla_number_of_lost_tdm_cells_in_interface_b_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_number_of_lost_tdm_cells_in_interface_b_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_NUMBER_OF_LOST_TDM_CELLS_IN_INTERFACE_B_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_NUMBER_OF_LOST_TDM_CELLS_IN_INTERFACE_B_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_NUMBER_OF_LOST_TDM_CELLS_IN_INTERFACE_B_REG_REGISTER_ID:
      regis = &(regs->sla.number_of_lost_tdm_cells_in_interface_b_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_NUMBER_OF_LOST_TDM_CELLS_IN_INTERFACE_B_REG_TDMFFB_LOST_CNT_ID:
      field = &(regs->sla.number_of_lost_tdm_cells_in_interface_b_reg.tdmffb_lost_cnt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_NUMBER_OF_LOST_TDM_CELLS_IN_INTERFACE_B_REG_TDMFFB_LOST_CNT_O_ID:
      field = &(regs->sla.number_of_lost_tdm_cells_in_interface_b_reg.tdmffb_lost_cnt_o);
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
  ui_fap21v_acc_sla_obfcb_out_of_frame_threshold_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_obfcb_out_of_frame_threshold_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_OUT_OF_FRAME_THRESHOLD_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_OUT_OF_FRAME_THRESHOLD_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_OUT_OF_FRAME_THRESHOLD_REG_REGISTER_ID:
      regis = &(regs->sla.obfcb_out_of_frame_threshold_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_OUT_OF_FRAME_THRESHOLD_REG_OBFCB_OUT_OF_FRM_TH_ID:
      field = &(regs->sla.obfcb_out_of_frame_threshold_reg.obfcb_out_of_frm_th);
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
  soc_sand_os_printf( "sla.obfcb_out_of_frame_threshold_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sla_obfcb_out_of_frame_threshold_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_obfcb_out_of_frame_threshold_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_OUT_OF_FRAME_THRESHOLD_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_OUT_OF_FRAME_THRESHOLD_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_OUT_OF_FRAME_THRESHOLD_REG_REGISTER_ID:
      regis = &(regs->sla.obfcb_out_of_frame_threshold_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_OUT_OF_FRAME_THRESHOLD_REG_OBFCB_OUT_OF_FRM_TH_ID:
      field = &(regs->sla.obfcb_out_of_frame_threshold_reg.obfcb_out_of_frm_th);
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
  ui_fap21v_acc_sla_indirect_command_address_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_indirect_command_address_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_INDIRECT_COMMAND_ADDRESS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_INDIRECT_COMMAND_ADDRESS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_INDIRECT_COMMAND_ADDRESS_REG_REGISTER_ID:
      regis = &(regs->sla.indirect_command_address_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_INDIRECT_COMMAND_ADDRESS_REG_INDIRECT_COMMAND_ADDRESS_ID:
      field = &(regs->sla.indirect_command_address_reg.indirect_command_address);
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
  soc_sand_os_printf( "sla.indirect_command_address_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sla_indirect_command_address_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_indirect_command_address_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_INDIRECT_COMMAND_ADDRESS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_INDIRECT_COMMAND_ADDRESS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_INDIRECT_COMMAND_ADDRESS_REG_REGISTER_ID:
      regis = &(regs->sla.indirect_command_address_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_INDIRECT_COMMAND_ADDRESS_REG_INDIRECT_COMMAND_ADDRESS_ID:
      field = &(regs->sla.indirect_command_address_reg.indirect_command_address);
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
  ui_fap21v_acc_sla_fifo_status_1_from_primary_spi4_fcs_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_fifo_status_1_from_primary_spi4_fcs_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_FIFO_STATUS_1_FROM_PRIMARY_SPI4_FCS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_FIFO_STATUS_1_FROM_PRIMARY_SPI4_FCS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_FIFO_STATUS_1_FROM_PRIMARY_SPI4_FCS_REG_REGISTER_ID:
      regis = &(regs->sla.fifo_status_1_from_primary_spi4_fcs_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_FIFO_STATUS_1_FROM_PRIMARY_SPI4_FCS_REG_FFSTAT_FROM_PRIM1_ID:
      field = &(regs->sla.fifo_status_1_from_primary_spi4_fcs_reg.ffstat_from_prim1);
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
  soc_sand_os_printf( "sla.fifo_status_1_from_primary_spi4_fcs_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sla_fifo_status_1_from_primary_spi4_fcs_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_fifo_status_1_from_primary_spi4_fcs_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_FIFO_STATUS_1_FROM_PRIMARY_SPI4_FCS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_FIFO_STATUS_1_FROM_PRIMARY_SPI4_FCS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_FIFO_STATUS_1_FROM_PRIMARY_SPI4_FCS_REG_REGISTER_ID:
      regis = &(regs->sla.fifo_status_1_from_primary_spi4_fcs_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_FIFO_STATUS_1_FROM_PRIMARY_SPI4_FCS_REG_FFSTAT_FROM_PRIM1_ID:
      field = &(regs->sla.fifo_status_1_from_primary_spi4_fcs_reg.ffstat_from_prim1);
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
  ui_fap21v_acc_sla_number_of_lost_tdm_cells_in_interface_a_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_number_of_lost_tdm_cells_in_interface_a_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_NUMBER_OF_LOST_TDM_CELLS_IN_INTERFACE_A_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_NUMBER_OF_LOST_TDM_CELLS_IN_INTERFACE_A_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_NUMBER_OF_LOST_TDM_CELLS_IN_INTERFACE_A_REG_REGISTER_ID:
      regis = &(regs->sla.number_of_lost_tdm_cells_in_interface_a_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_NUMBER_OF_LOST_TDM_CELLS_IN_INTERFACE_A_REG_TDMFFA_LOST_CNT_ID:
      field = &(regs->sla.number_of_lost_tdm_cells_in_interface_a_reg.tdmffa_lost_cnt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_NUMBER_OF_LOST_TDM_CELLS_IN_INTERFACE_A_REG_TDMFFA_LOST_CNT_O_ID:
      field = &(regs->sla.number_of_lost_tdm_cells_in_interface_a_reg.tdmffa_lost_cnt_o);
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
  soc_sand_os_printf( "sla.number_of_lost_tdm_cells_in_interface_a_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sla_number_of_lost_tdm_cells_in_interface_a_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_number_of_lost_tdm_cells_in_interface_a_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_NUMBER_OF_LOST_TDM_CELLS_IN_INTERFACE_A_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_NUMBER_OF_LOST_TDM_CELLS_IN_INTERFACE_A_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_NUMBER_OF_LOST_TDM_CELLS_IN_INTERFACE_A_REG_REGISTER_ID:
      regis = &(regs->sla.number_of_lost_tdm_cells_in_interface_a_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_NUMBER_OF_LOST_TDM_CELLS_IN_INTERFACE_A_REG_TDMFFA_LOST_CNT_ID:
      field = &(regs->sla.number_of_lost_tdm_cells_in_interface_a_reg.tdmffa_lost_cnt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_NUMBER_OF_LOST_TDM_CELLS_IN_INTERFACE_A_REG_TDMFFA_LOST_CNT_O_ID:
      field = &(regs->sla.number_of_lost_tdm_cells_in_interface_a_reg.tdmffa_lost_cnt_o);
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
  ui_fap21v_acc_sla_fabric_header_and_otm_header_in_fap20b_mode_configuration_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_fabric_header_and_otm_header_in_fap20b_mode_configuration_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_FABRIC_HEADER_AND_OTM_HEADER_IN_FAP20B_MODE_CONFIGURATION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_FABRIC_HEADER_AND_OTM_HEADER_IN_FAP20B_MODE_CONFIGURATION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_FABRIC_HEADER_AND_OTM_HEADER_IN_FAP20B_MODE_CONFIGURATION_REG_REGISTER_ID:
      regis = &(regs->sla.fabric_header_and_otm_header_in_fap20b_mode_configuration_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_FABRIC_HEADER_AND_OTM_HEADER_IN_FAP20B_MODE_CONFIGURATION_REG_FAP20_OTMMASK_MID13_ID:
      field = &(regs->sla.fabric_header_and_otm_header_in_fap20b_mode_configuration_reg.fap20_otmmask_mid13);
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
  soc_sand_os_printf( "sla.fabric_header_and_otm_header_in_fap20b_mode_configuration_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sla_fabric_header_and_otm_header_in_fap20b_mode_configuration_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_fabric_header_and_otm_header_in_fap20b_mode_configuration_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_FABRIC_HEADER_AND_OTM_HEADER_IN_FAP20B_MODE_CONFIGURATION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_FABRIC_HEADER_AND_OTM_HEADER_IN_FAP20B_MODE_CONFIGURATION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_FABRIC_HEADER_AND_OTM_HEADER_IN_FAP20B_MODE_CONFIGURATION_REG_REGISTER_ID:
      regis = &(regs->sla.fabric_header_and_otm_header_in_fap20b_mode_configuration_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_FABRIC_HEADER_AND_OTM_HEADER_IN_FAP20B_MODE_CONFIGURATION_REG_FAP20_OTMMASK_MID13_ID:
      field = &(regs->sla.fabric_header_and_otm_header_in_fap20b_mode_configuration_reg.fap20_otmmask_mid13);
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
  ui_fap21v_acc_sla_framing_error_cnt_out_of_frame_flow_control_a_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_framing_error_cnt_out_of_frame_flow_control_a_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_FRAMING_ERROR_CNT_OUT_OF_FRAME_FLOW_CONTROL_A_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_FRAMING_ERROR_CNT_OUT_OF_FRAME_FLOW_CONTROL_A_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_FRAMING_ERROR_CNT_OUT_OF_FRAME_FLOW_CONTROL_A_REG_REGISTER_ID:
      regis = &(regs->sla.framing_error_cnt_out_of_frame_flow_control_a_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_FRAMING_ERROR_CNT_OUT_OF_FRAME_FLOW_CONTROL_A_REG_OBFCA_FRM_ERR_CNT_ID:
      field = &(regs->sla.framing_error_cnt_out_of_frame_flow_control_a_reg.obfca_frm_err_cnt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_FRAMING_ERROR_CNT_OUT_OF_FRAME_FLOW_CONTROL_A_REG_OBFCA_FRM_ERR_CNT_O_ID:
      field = &(regs->sla.framing_error_cnt_out_of_frame_flow_control_a_reg.obfca_frm_err_cnt_o);
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
  soc_sand_os_printf( "sla.framing_error_cnt_out_of_frame_flow_control_a_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sla_framing_error_cnt_out_of_frame_flow_control_a_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_framing_error_cnt_out_of_frame_flow_control_a_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_FRAMING_ERROR_CNT_OUT_OF_FRAME_FLOW_CONTROL_A_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_FRAMING_ERROR_CNT_OUT_OF_FRAME_FLOW_CONTROL_A_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_FRAMING_ERROR_CNT_OUT_OF_FRAME_FLOW_CONTROL_A_REG_REGISTER_ID:
      regis = &(regs->sla.framing_error_cnt_out_of_frame_flow_control_a_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_FRAMING_ERROR_CNT_OUT_OF_FRAME_FLOW_CONTROL_A_REG_OBFCA_FRM_ERR_CNT_ID:
      field = &(regs->sla.framing_error_cnt_out_of_frame_flow_control_a_reg.obfca_frm_err_cnt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_FRAMING_ERROR_CNT_OUT_OF_FRAME_FLOW_CONTROL_A_REG_OBFCA_FRM_ERR_CNT_O_ID:
      field = &(regs->sla.framing_error_cnt_out_of_frame_flow_control_a_reg.obfca_frm_err_cnt_o);
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
  ui_fap21v_acc_sla_indirect_command_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_indirect_command_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_INDIRECT_COMMAND_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_INDIRECT_COMMAND_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_INDIRECT_COMMAND_REG_REGISTER_ID:
      regis = &(regs->sla.indirect_command_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_INDIRECT_COMMAND_REG_INDIRECT_COMMAND_ID:
      field = &(regs->sla.indirect_command_reg.indirect_command);
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
  soc_sand_os_printf( "sla.indirect_command_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sla_indirect_command_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_indirect_command_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_INDIRECT_COMMAND_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_INDIRECT_COMMAND_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_INDIRECT_COMMAND_REG_REGISTER_ID:
      regis = &(regs->sla.indirect_command_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_INDIRECT_COMMAND_REG_INDIRECT_COMMAND_ID:
      field = &(regs->sla.indirect_command_reg.indirect_command);
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
  ui_fap21v_acc_sla_obfca_consecutive_error_free_frames_threshold_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_obfca_consecutive_error_free_frames_threshold_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_CONSECUTIVE_ERROR_FREE_FRAMES_THRESHOLD_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_CONSECUTIVE_ERROR_FREE_FRAMES_THRESHOLD_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_CONSECUTIVE_ERROR_FREE_FRAMES_THRESHOLD_REG_REGISTER_ID:
      regis = &(regs->sla.obfca_consecutive_error_free_frames_threshold_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_CONSECUTIVE_ERROR_FREE_FRAMES_THRESHOLD_REG_OBFCA_IN_FRAME_TH_ID:
      field = &(regs->sla.obfca_consecutive_error_free_frames_threshold_reg.obfca_in_frame_th);
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
  soc_sand_os_printf( "sla.obfca_consecutive_error_free_frames_threshold_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sla_obfca_consecutive_error_free_frames_threshold_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_obfca_consecutive_error_free_frames_threshold_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_CONSECUTIVE_ERROR_FREE_FRAMES_THRESHOLD_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_CONSECUTIVE_ERROR_FREE_FRAMES_THRESHOLD_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_CONSECUTIVE_ERROR_FREE_FRAMES_THRESHOLD_REG_REGISTER_ID:
      regis = &(regs->sla.obfca_consecutive_error_free_frames_threshold_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_CONSECUTIVE_ERROR_FREE_FRAMES_THRESHOLD_REG_OBFCA_IN_FRAME_TH_ID:
      field = &(regs->sla.obfca_consecutive_error_free_frames_threshold_reg.obfca_in_frame_th);
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
  ui_fap21v_acc_sla_packet_fragment_word_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_packet_fragment_word_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_PACKET_FRAGMENT_WORD_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_PACKET_FRAGMENT_WORD_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_PACKET_FRAGMENT_WORD_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_PACKET_FRAGMENT_WORD_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_PACKET_FRAGMENT_WORD_REG_REGISTER_ID:
      regis = &(regs->sla.packet_fragment_word_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_PACKET_FRAGMENT_WORD_REG_PACKET_FRAGMENT_WORD_ID:
      field = &(regs->sla.packet_fragment_word_reg[offset].packet_fragment_word);
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
  soc_sand_os_printf( "sla.packet_fragment_word_reg***: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sla_packet_fragment_word_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_packet_fragment_word_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_PACKET_FRAGMENT_WORD_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_PACKET_FRAGMENT_WORD_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_PACKET_FRAGMENT_WORD_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_PACKET_FRAGMENT_WORD_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_PACKET_FRAGMENT_WORD_REG_REGISTER_ID:
      regis = &(regs->sla.packet_fragment_word_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_PACKET_FRAGMENT_WORD_REG_PACKET_FRAGMENT_WORD_ID:
      field = &(regs->sla.packet_fragment_word_reg[offset].packet_fragment_word);
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
  ui_fap21v_acc_sla_obfc_calendar_len_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_obfc_calendar_len_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFC_CALENDAR_LEN_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_OBFC_CALENDAR_LEN_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFC_CALENDAR_LEN_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_OBFC_CALENDAR_LEN_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_OBFC_CALENDAR_LEN_REG_REGISTER_ID:
      regis = &(regs->sla.obfc_calendar_len_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_OBFC_CALENDAR_LEN_REG_OBFC_CALENDAR_LEN_ID:
      field = &(regs->sla.obfc_calendar_len_reg[offset].obfc_calendar_len);
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
  soc_sand_os_printf( "sla.obfc_calendar_len_reg***: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sla_obfc_calendar_len_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_obfc_calendar_len_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFC_CALENDAR_LEN_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_OBFC_CALENDAR_LEN_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFC_CALENDAR_LEN_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_OBFC_CALENDAR_LEN_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_OBFC_CALENDAR_LEN_REG_REGISTER_ID:
      regis = &(regs->sla.obfc_calendar_len_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_OBFC_CALENDAR_LEN_REG_OBFC_CALENDAR_LEN_ID:
      field = &(regs->sla.obfc_calendar_len_reg[offset].obfc_calendar_len);
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
  ui_fap21v_acc_sla_framing_error_cnt_out_of_frame_flow_control_b_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_framing_error_cnt_out_of_frame_flow_control_b_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_FRAMING_ERROR_CNT_OUT_OF_FRAME_FLOW_CONTROL_B_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_FRAMING_ERROR_CNT_OUT_OF_FRAME_FLOW_CONTROL_B_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_FRAMING_ERROR_CNT_OUT_OF_FRAME_FLOW_CONTROL_B_REG_REGISTER_ID:
      regis = &(regs->sla.framing_error_cnt_out_of_frame_flow_control_b_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_FRAMING_ERROR_CNT_OUT_OF_FRAME_FLOW_CONTROL_B_REG_OBFCB_FRM_ERR_CNT_ID:
      field = &(regs->sla.framing_error_cnt_out_of_frame_flow_control_b_reg.obfcb_frm_err_cnt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_FRAMING_ERROR_CNT_OUT_OF_FRAME_FLOW_CONTROL_B_REG_OBFCB_FRM_ERR_CNT_O_ID:
      field = &(regs->sla.framing_error_cnt_out_of_frame_flow_control_b_reg.obfcb_frm_err_cnt_o);
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
  soc_sand_os_printf( "sla.framing_error_cnt_out_of_frame_flow_control_b_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sla_framing_error_cnt_out_of_frame_flow_control_b_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_framing_error_cnt_out_of_frame_flow_control_b_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_FRAMING_ERROR_CNT_OUT_OF_FRAME_FLOW_CONTROL_B_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_FRAMING_ERROR_CNT_OUT_OF_FRAME_FLOW_CONTROL_B_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_FRAMING_ERROR_CNT_OUT_OF_FRAME_FLOW_CONTROL_B_REG_REGISTER_ID:
      regis = &(regs->sla.framing_error_cnt_out_of_frame_flow_control_b_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_FRAMING_ERROR_CNT_OUT_OF_FRAME_FLOW_CONTROL_B_REG_OBFCB_FRM_ERR_CNT_ID:
      field = &(regs->sla.framing_error_cnt_out_of_frame_flow_control_b_reg.obfcb_frm_err_cnt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_FRAMING_ERROR_CNT_OUT_OF_FRAME_FLOW_CONTROL_B_REG_OBFCB_FRM_ERR_CNT_O_ID:
      field = &(regs->sla.framing_error_cnt_out_of_frame_flow_control_b_reg.obfcb_frm_err_cnt_o);
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
  ui_fap21v_acc_sla_obfcb_tsclk_edge_select_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_obfcb_tsclk_edge_select_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_TSCLK_EDGE_SELECT_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_TSCLK_EDGE_SELECT_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_TSCLK_EDGE_SELECT_REG_REGISTER_ID:
      regis = &(regs->sla.obfcb_tsclk_edge_select_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_TSCLK_EDGE_SELECT_REG_OBFCB_TSCLKEDGE_SEL_ID:
      field = &(regs->sla.obfcb_tsclk_edge_select_reg.obfcb_tsclkedge_sel);
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
  soc_sand_os_printf( "sla.obfcb_tsclk_edge_select_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sla_obfcb_tsclk_edge_select_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_obfcb_tsclk_edge_select_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_TSCLK_EDGE_SELECT_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_TSCLK_EDGE_SELECT_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_TSCLK_EDGE_SELECT_REG_REGISTER_ID:
      regis = &(regs->sla.obfcb_tsclk_edge_select_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_TSCLK_EDGE_SELECT_REG_OBFCB_TSCLKEDGE_SEL_ID:
      field = &(regs->sla.obfcb_tsclk_edge_select_reg.obfcb_tsclkedge_sel);
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
  ui_fap21v_acc_sla_obfcb_in_frame_threshold_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_obfcb_in_frame_threshold_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_IN_FRAME_THRESHOLD_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_IN_FRAME_THRESHOLD_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_IN_FRAME_THRESHOLD_REG_REGISTER_ID:
      regis = &(regs->sla.obfcb_in_frame_threshold_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_IN_FRAME_THRESHOLD_REG_OBFCB_IN_FRM_TH_ID:
      field = &(regs->sla.obfcb_in_frame_threshold_reg.obfcb_in_frm_th);
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
  soc_sand_os_printf( "sla.obfcb_in_frame_threshold_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sla_obfcb_in_frame_threshold_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_obfcb_in_frame_threshold_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_IN_FRAME_THRESHOLD_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_IN_FRAME_THRESHOLD_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_IN_FRAME_THRESHOLD_REG_REGISTER_ID:
      regis = &(regs->sla.obfcb_in_frame_threshold_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_IN_FRAME_THRESHOLD_REG_OBFCB_IN_FRM_TH_ID:
      field = &(regs->sla.obfcb_in_frame_threshold_reg.obfcb_in_frm_th);
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
  ui_fap21v_acc_sla_sla_interrupt_sources_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_sla_interrupt_sources_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_SLA_INTERRUPT_SOURCES_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_SLA_INTERRUPT_SOURCES_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_SLA_INTERRUPT_SOURCES_REG_REGISTER_ID:
      regis = &(regs->sla.sla_interrupt_sources_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_SLA_INTERRUPT_SOURCES_REG_PRIMA_OUT_OF_FRAME_ID:
      field = &(regs->sla.sla_interrupt_sources_reg.prima_out_of_frame);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_SLA_INTERRUPT_SOURCES_REG_PRIMB_OUT_OF_FRAME_ID:
      field = &(regs->sla.sla_interrupt_sources_reg.primb_out_of_frame);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_SLA_INTERRUPT_SOURCES_REG_SLADATA_READY_FOR_CPU_ID:
      field = &(regs->sla.sla_interrupt_sources_reg.sladata_ready_for_cpu);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_SLA_INTERRUPT_SOURCES_REG_TDMA_FFLOST_ID:
      field = &(regs->sla.sla_interrupt_sources_reg.tdma_fflost);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_SLA_INTERRUPT_SOURCES_REG_TDMB_FFLOST_ID:
      field = &(regs->sla.sla_interrupt_sources_reg.tdmb_fflost);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_SLA_INTERRUPT_SOURCES_REG_OBFCA_NO_LOCK_ID:
      field = &(regs->sla.sla_interrupt_sources_reg.obfca_no_lock);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_SLA_INTERRUPT_SOURCES_REG_OBFCA_OUT_OF_FRAME_ID:
      field = &(regs->sla.sla_interrupt_sources_reg.obfca_out_of_frame);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_SLA_INTERRUPT_SOURCES_REG_OBFCA_DIP2_ALARM_ID:
      field = &(regs->sla.sla_interrupt_sources_reg.obfca_dip2_alarm);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_SLA_INTERRUPT_SOURCES_REG_OBFCA_FRM_ERROR_ID:
      field = &(regs->sla.sla_interrupt_sources_reg.obfca_frm_error);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_SLA_INTERRUPT_SOURCES_REG_OBFCA_DIP2_ERROR_ID:
      field = &(regs->sla.sla_interrupt_sources_reg.obfca_dip2_error);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_SLA_INTERRUPT_SOURCES_REG_OBFCB_NO_LOCK_ID:
      field = &(regs->sla.sla_interrupt_sources_reg.obfcb_no_lock);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_SLA_INTERRUPT_SOURCES_REG_OBFCB_OUT_OF_FRAME_ID:
      field = &(regs->sla.sla_interrupt_sources_reg.obfcb_out_of_frame);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_SLA_INTERRUPT_SOURCES_REG_OBFCB_DIP2_ALARM_ID:
      field = &(regs->sla.sla_interrupt_sources_reg.obfcb_dip2_alarm);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_SLA_INTERRUPT_SOURCES_REG_OBFCB_FRM_ERROR_ID:
      field = &(regs->sla.sla_interrupt_sources_reg.obfcb_frm_error);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_SLA_INTERRUPT_SOURCES_REG_OBFCB_DIP2_ERROR_ID:
      field = &(regs->sla.sla_interrupt_sources_reg.obfcb_dip2_error);
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
  soc_sand_os_printf( "sla.sla_interrupt_sources_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sla_sla_interrupt_sources_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_sla_interrupt_sources_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_SLA_INTERRUPT_SOURCES_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_SLA_INTERRUPT_SOURCES_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_SLA_INTERRUPT_SOURCES_REG_REGISTER_ID:
      regis = &(regs->sla.sla_interrupt_sources_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_SLA_INTERRUPT_SOURCES_REG_PRIMA_OUT_OF_FRAME_ID:
      field = &(regs->sla.sla_interrupt_sources_reg.prima_out_of_frame);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_SLA_INTERRUPT_SOURCES_REG_PRIMB_OUT_OF_FRAME_ID:
      field = &(regs->sla.sla_interrupt_sources_reg.primb_out_of_frame);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_SLA_INTERRUPT_SOURCES_REG_SLADATA_READY_FOR_CPU_ID:
      field = &(regs->sla.sla_interrupt_sources_reg.sladata_ready_for_cpu);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_SLA_INTERRUPT_SOURCES_REG_TDMA_FFLOST_ID:
      field = &(regs->sla.sla_interrupt_sources_reg.tdma_fflost);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_SLA_INTERRUPT_SOURCES_REG_TDMB_FFLOST_ID:
      field = &(regs->sla.sla_interrupt_sources_reg.tdmb_fflost);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_SLA_INTERRUPT_SOURCES_REG_OBFCA_NO_LOCK_ID:
      field = &(regs->sla.sla_interrupt_sources_reg.obfca_no_lock);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_SLA_INTERRUPT_SOURCES_REG_OBFCA_OUT_OF_FRAME_ID:
      field = &(regs->sla.sla_interrupt_sources_reg.obfca_out_of_frame);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_SLA_INTERRUPT_SOURCES_REG_OBFCA_DIP2_ALARM_ID:
      field = &(regs->sla.sla_interrupt_sources_reg.obfca_dip2_alarm);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_SLA_INTERRUPT_SOURCES_REG_OBFCA_FRM_ERROR_ID:
      field = &(regs->sla.sla_interrupt_sources_reg.obfca_frm_error);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_SLA_INTERRUPT_SOURCES_REG_OBFCA_DIP2_ERROR_ID:
      field = &(regs->sla.sla_interrupt_sources_reg.obfca_dip2_error);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_SLA_INTERRUPT_SOURCES_REG_OBFCB_NO_LOCK_ID:
      field = &(regs->sla.sla_interrupt_sources_reg.obfcb_no_lock);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_SLA_INTERRUPT_SOURCES_REG_OBFCB_OUT_OF_FRAME_ID:
      field = &(regs->sla.sla_interrupt_sources_reg.obfcb_out_of_frame);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_SLA_INTERRUPT_SOURCES_REG_OBFCB_DIP2_ALARM_ID:
      field = &(regs->sla.sla_interrupt_sources_reg.obfcb_dip2_alarm);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_SLA_INTERRUPT_SOURCES_REG_OBFCB_FRM_ERROR_ID:
      field = &(regs->sla.sla_interrupt_sources_reg.obfcb_frm_error);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_SLA_INTERRUPT_SOURCES_REG_OBFCB_DIP2_ERROR_ID:
      field = &(regs->sla.sla_interrupt_sources_reg.obfcb_dip2_error);
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
  ui_fap21v_acc_sla_obfca_dip2_good_to_bad_ratio_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_obfca_dip2_good_to_bad_ratio_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_DIP2_GOOD_TO_BAD_RATIO_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_DIP2_GOOD_TO_BAD_RATIO_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_DIP2_GOOD_TO_BAD_RATIO_REG_REGISTER_ID:
      regis = &(regs->sla.obfca_dip2_good_to_bad_ratio_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_DIP2_GOOD_TO_BAD_RATIO_REG_OBFCA_DIP2_GOOD_TO_BAD_RATIO_ID:
      field = &(regs->sla.obfca_dip2_good_to_bad_ratio_reg.obfca_dip2_good_to_bad_ratio);
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
  soc_sand_os_printf( "sla.obfca_dip2_good_to_bad_ratio_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sla_obfca_dip2_good_to_bad_ratio_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla");
  soc_sand_proc_name = "ui_fap21v_acc_sla_obfca_dip2_good_to_bad_ratio_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_DIP2_GOOD_TO_BAD_RATIO_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_DIP2_GOOD_TO_BAD_RATIO_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_DIP2_GOOD_TO_BAD_RATIO_REG_REGISTER_ID:
      regis = &(regs->sla.obfca_dip2_good_to_bad_ratio_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_DIP2_GOOD_TO_BAD_RATIO_REG_OBFCA_DIP2_GOOD_TO_BAD_RATIO_ID:
      field = &(regs->sla.obfca_dip2_good_to_bad_ratio_reg.obfca_dip2_good_to_bad_ratio);
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
  ui_fap21v_acc_sla_get(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla_get");

  if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_DIP2_ALARM_THRESHOLD_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_obfcb_dip2_alarm_threshold_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_MAX_FRAMING_PATTERN_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_obfcb_max_framing_pattern_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_FIFO_STATUS_0_FROM_PRIMARY_SPI4_FCS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_fifo_status_0_from_primary_spi4_fcs_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFC_CALENDAR_M_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_obfc_calendar_m_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_SPI4_TRANSFER_CONTROL_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_spi4_transfer_control_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_DIP2_ERROR_CNT_OUT_OF_BAND_FLOW_CONTROL_B_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_dip2_error_cnt_out_of_band_flow_control_b_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_TSCLK_EDGE_SELECT_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_obfca_tsclk_edge_select_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_OUT_OF_FRAME_THRESHOLD_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_obfca_out_of_frame_threshold_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_INDIRECT_COMMAND_WR_DATA_REG_0_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_indirect_command_wr_data_reg_0_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_DIP2_ALARM_THRESHOLD_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_obfca_dip2_alarm_threshold_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_SLA_INTERRUPT_MASK_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_sla_interrupt_mask_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_DIP2_ERROR_CNT_OUT_OF_BAND_FLOW_CONTROL_A_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_dip2_error_cnt_out_of_band_flow_control_a_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_INDIRECT_COMMAND_RD_DATA_REG_0_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_indirect_command_rd_data_reg_0_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_MAX_CONSECUTIVE_FRAMING_PATTERN_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_obfca_max_consecutive_framing_pattern_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_DIP2_GOOD_TO_BAD_RATIO_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_obfcb_dip2_good_to_bad_ratio_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_SLA_INITIALIZATION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_sla_initialization_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_SPI4_SEL_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_spi4_sel_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_FLOW_CONTROL_CONFIGURATIONS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_flow_control_configurations_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_FIFO_STATUS_0_FROM_AUXILIARY_FLOW_CONTROL_CHANNEL_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_fifo_status_0_from_auxiliary_flow_control_channel_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_FIFO_STATUS_1_FROM_AUXILIARY_FLOW_CONTROL_CHANNEL_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_fifo_status_1_from_auxiliary_flow_control_channel_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_GENERAL_CONFIGURATION_REGISTER_OUT_OF_BAND_FLOW_CONTROL_FOR_SPI4_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_general_configuration_register_out_of_band_flow_control_for_spi4_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OTM_HEADERS_IN_PETRA_MODE_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_otm_headers_in_petra_mode_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_FABRIC_HEADER_IN_FAP20B_MODE_AND_OTM_HEADER_IN_PETRA_MODE_CONFIGURATION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_fabric_header_in_fap20b_mode_and_otm_header_in_petra_mode_configuration_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_CPU_AND_LBP_VALID_FLAGS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_cpu_and_lbp_valid_flags_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_CPU_PACKET_CONTROL_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_cpu_packet_control_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_NUMBER_OF_LOST_TDM_CELLS_IN_INTERFACE_B_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_number_of_lost_tdm_cells_in_interface_b_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_OUT_OF_FRAME_THRESHOLD_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_obfcb_out_of_frame_threshold_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_INDIRECT_COMMAND_ADDRESS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_indirect_command_address_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_FIFO_STATUS_1_FROM_PRIMARY_SPI4_FCS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_fifo_status_1_from_primary_spi4_fcs_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_NUMBER_OF_LOST_TDM_CELLS_IN_INTERFACE_A_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_number_of_lost_tdm_cells_in_interface_a_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_FABRIC_HEADER_AND_OTM_HEADER_IN_FAP20B_MODE_CONFIGURATION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_fabric_header_and_otm_header_in_fap20b_mode_configuration_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_FRAMING_ERROR_CNT_OUT_OF_FRAME_FLOW_CONTROL_A_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_framing_error_cnt_out_of_frame_flow_control_a_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_INDIRECT_COMMAND_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_indirect_command_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_CONSECUTIVE_ERROR_FREE_FRAMES_THRESHOLD_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_obfca_consecutive_error_free_frames_threshold_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_PACKET_FRAGMENT_WORD_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_packet_fragment_word_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFC_CALENDAR_LEN_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_obfc_calendar_len_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_FRAMING_ERROR_CNT_OUT_OF_FRAME_FLOW_CONTROL_B_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_framing_error_cnt_out_of_frame_flow_control_b_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_TSCLK_EDGE_SELECT_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_obfcb_tsclk_edge_select_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_IN_FRAME_THRESHOLD_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_obfcb_in_frame_threshold_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_SLA_INTERRUPT_SOURCES_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_sla_interrupt_sources_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_DIP2_GOOD_TO_BAD_RATIO_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_obfca_dip2_good_to_bad_ratio_reg_get(current_line);
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
  ui_fap21v_acc_sla_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sla_set");

  if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_DIP2_ALARM_THRESHOLD_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_obfcb_dip2_alarm_threshold_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_MAX_FRAMING_PATTERN_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_obfcb_max_framing_pattern_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_FIFO_STATUS_0_FROM_PRIMARY_SPI4_FCS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_fifo_status_0_from_primary_spi4_fcs_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFC_CALENDAR_M_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_obfc_calendar_m_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_SPI4_TRANSFER_CONTROL_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_spi4_transfer_control_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_DIP2_ERROR_CNT_OUT_OF_BAND_FLOW_CONTROL_B_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_dip2_error_cnt_out_of_band_flow_control_b_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_TSCLK_EDGE_SELECT_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_obfca_tsclk_edge_select_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_OUT_OF_FRAME_THRESHOLD_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_obfca_out_of_frame_threshold_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_INDIRECT_COMMAND_WR_DATA_REG_0_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_indirect_command_wr_data_reg_0_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_DIP2_ALARM_THRESHOLD_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_obfca_dip2_alarm_threshold_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_SLA_INTERRUPT_MASK_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_sla_interrupt_mask_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_DIP2_ERROR_CNT_OUT_OF_BAND_FLOW_CONTROL_A_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_dip2_error_cnt_out_of_band_flow_control_a_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_INDIRECT_COMMAND_RD_DATA_REG_0_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_indirect_command_rd_data_reg_0_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_MAX_CONSECUTIVE_FRAMING_PATTERN_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_obfca_max_consecutive_framing_pattern_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_DIP2_GOOD_TO_BAD_RATIO_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_obfcb_dip2_good_to_bad_ratio_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_SLA_INITIALIZATION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_sla_initialization_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_SPI4_SEL_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_spi4_sel_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_FLOW_CONTROL_CONFIGURATIONS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_flow_control_configurations_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_FIFO_STATUS_0_FROM_AUXILIARY_FLOW_CONTROL_CHANNEL_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_fifo_status_0_from_auxiliary_flow_control_channel_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_FIFO_STATUS_1_FROM_AUXILIARY_FLOW_CONTROL_CHANNEL_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_fifo_status_1_from_auxiliary_flow_control_channel_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_GENERAL_CONFIGURATION_REGISTER_OUT_OF_BAND_FLOW_CONTROL_FOR_SPI4_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_general_configuration_register_out_of_band_flow_control_for_spi4_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OTM_HEADERS_IN_PETRA_MODE_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_otm_headers_in_petra_mode_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_FABRIC_HEADER_IN_FAP20B_MODE_AND_OTM_HEADER_IN_PETRA_MODE_CONFIGURATION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_fabric_header_in_fap20b_mode_and_otm_header_in_petra_mode_configuration_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_CPU_AND_LBP_VALID_FLAGS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_cpu_and_lbp_valid_flags_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_CPU_PACKET_CONTROL_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_cpu_packet_control_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_NUMBER_OF_LOST_TDM_CELLS_IN_INTERFACE_B_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_number_of_lost_tdm_cells_in_interface_b_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_OUT_OF_FRAME_THRESHOLD_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_obfcb_out_of_frame_threshold_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_INDIRECT_COMMAND_ADDRESS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_indirect_command_address_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_FIFO_STATUS_1_FROM_PRIMARY_SPI4_FCS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_fifo_status_1_from_primary_spi4_fcs_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_NUMBER_OF_LOST_TDM_CELLS_IN_INTERFACE_A_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_number_of_lost_tdm_cells_in_interface_a_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_FABRIC_HEADER_AND_OTM_HEADER_IN_FAP20B_MODE_CONFIGURATION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_fabric_header_and_otm_header_in_fap20b_mode_configuration_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_FRAMING_ERROR_CNT_OUT_OF_FRAME_FLOW_CONTROL_A_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_framing_error_cnt_out_of_frame_flow_control_a_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_INDIRECT_COMMAND_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_indirect_command_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_CONSECUTIVE_ERROR_FREE_FRAMES_THRESHOLD_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_obfca_consecutive_error_free_frames_threshold_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_PACKET_FRAGMENT_WORD_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_packet_fragment_word_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFC_CALENDAR_LEN_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_obfc_calendar_len_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_FRAMING_ERROR_CNT_OUT_OF_FRAME_FLOW_CONTROL_B_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_framing_error_cnt_out_of_frame_flow_control_b_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_TSCLK_EDGE_SELECT_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_obfcb_tsclk_edge_select_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFCB_IN_FRAME_THRESHOLD_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_obfcb_in_frame_threshold_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_SLA_INTERRUPT_SOURCES_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_sla_interrupt_sources_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SLA_OBFCA_DIP2_GOOD_TO_BAD_RATIO_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sla_obfca_dip2_good_to_bad_ratio_reg_set(current_line, value);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after sla***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}
