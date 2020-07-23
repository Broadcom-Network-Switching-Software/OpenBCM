/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

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
  ui_fap21v_acc_spi_rsclk_edge_select_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_rsclk_edge_select_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_RSCLK_EDGE_SELECT_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_RSCLK_EDGE_SELECT_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_RSCLK_EDGE_SELECT_REG_REGISTER_ID:
      regis = &(regs->spi.rsclk_edge_select_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_RSCLK_EDGE_SELECT_REG_RSCLKEDGE_SEL_ID:
      field = &(regs->spi.rsclk_edge_select_reg.rsclkedge_sel);
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
  soc_sand_os_printf( "spi.rsclk_edge_select_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_spi_rsclk_edge_select_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_rsclk_edge_select_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_RSCLK_EDGE_SELECT_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_RSCLK_EDGE_SELECT_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_RSCLK_EDGE_SELECT_REG_REGISTER_ID:
      regis = &(regs->spi.rsclk_edge_select_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_RSCLK_EDGE_SELECT_REG_RSCLKEDGE_SEL_ID:
      field = &(regs->spi.rsclk_edge_select_reg.rsclkedge_sel);
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
  ui_fap21v_acc_spi_data_error_cnt_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_data_error_cnt_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_DATA_ERROR_CNT_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_DATA_ERROR_CNT_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_DATA_ERROR_CNT_REG_REGISTER_ID:
      regis = &(regs->spi.data_error_cnt_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_DATA_ERROR_CNT_REG_DATA_ERR_CNT_ID:
      field = &(regs->spi.data_error_cnt_reg.data_err_cnt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_DATA_ERROR_CNT_REG_DATA_ERR_CNT_O_ID:
      field = &(regs->spi.data_error_cnt_reg.data_err_cnt_o);
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
  soc_sand_os_printf( "spi.data_error_cnt_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_spi_data_error_cnt_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_data_error_cnt_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_DATA_ERROR_CNT_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_DATA_ERROR_CNT_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_DATA_ERROR_CNT_REG_REGISTER_ID:
      regis = &(regs->spi.data_error_cnt_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_DATA_ERROR_CNT_REG_DATA_ERR_CNT_ID:
      field = &(regs->spi.data_error_cnt_reg.data_err_cnt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_DATA_ERROR_CNT_REG_DATA_ERR_CNT_O_ID:
      field = &(regs->spi.data_error_cnt_reg.data_err_cnt_o);
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
  ui_fap21v_acc_spi_dip4_error_cnt_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_dip4_error_cnt_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_DIP4_ERROR_CNT_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_DIP4_ERROR_CNT_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_DIP4_ERROR_CNT_REG_REGISTER_ID:
      regis = &(regs->spi.dip4_error_cnt_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_DIP4_ERROR_CNT_REG_DIP4_ERR_CNT_ID:
      field = &(regs->spi.dip4_error_cnt_reg.dip4_err_cnt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_DIP4_ERROR_CNT_REG_DIP4_ERR_CNT_O_ID:
      field = &(regs->spi.dip4_error_cnt_reg.dip4_err_cnt_o);
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
  soc_sand_os_printf( "spi.dip4_error_cnt_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_spi_dip4_error_cnt_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_dip4_error_cnt_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_DIP4_ERROR_CNT_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_DIP4_ERROR_CNT_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_DIP4_ERROR_CNT_REG_REGISTER_ID:
      regis = &(regs->spi.dip4_error_cnt_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_DIP4_ERROR_CNT_REG_DIP4_ERR_CNT_ID:
      field = &(regs->spi.dip4_error_cnt_reg.dip4_err_cnt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_DIP4_ERROR_CNT_REG_DIP4_ERR_CNT_O_ID:
      field = &(regs->spi.dip4_error_cnt_reg.dip4_err_cnt_o);
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
  ui_fap21v_acc_spi_dip4_alarm_threshold_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_dip4_alarm_threshold_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_DIP4_ALARM_THRESHOLD_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_DIP4_ALARM_THRESHOLD_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_DIP4_ALARM_THRESHOLD_REG_REGISTER_ID:
      regis = &(regs->spi.dip4_alarm_threshold_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_DIP4_ALARM_THRESHOLD_REG_DIP4_ALARM_TH_ID:
      field = &(regs->spi.dip4_alarm_threshold_reg.dip4_alarm_th);
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
  soc_sand_os_printf( "spi.dip4_alarm_threshold_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_spi_dip4_alarm_threshold_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_dip4_alarm_threshold_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_DIP4_ALARM_THRESHOLD_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_DIP4_ALARM_THRESHOLD_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_DIP4_ALARM_THRESHOLD_REG_REGISTER_ID:
      regis = &(regs->spi.dip4_alarm_threshold_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_DIP4_ALARM_THRESHOLD_REG_DIP4_ALARM_TH_ID:
      field = &(regs->spi.dip4_alarm_threshold_reg.dip4_alarm_th);
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
  ui_fap21v_acc_spi_general_sink_configuration_register_for_status_channel_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_general_sink_configuration_register_for_status_channel_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_GENERAL_SINK_CONFIGURATION_REGISTER_FOR_STATUS_CHANNEL_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_GENERAL_SINK_CONFIGURATION_REGISTER_FOR_STATUS_CHANNEL_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_GENERAL_SINK_CONFIGURATION_REGISTER_FOR_STATUS_CHANNEL_REG_REGISTER_ID:
      regis = &(regs->spi.general_sink_configuration_register_for_status_channel_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_GENERAL_SINK_CONFIGURATION_REGISTER_FOR_STATUS_CHANNEL_REG_SINK_STATUS_ENA_ID:
      field = &(regs->spi.general_sink_configuration_register_for_status_channel_reg.sink_status_ena);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_GENERAL_SINK_CONFIGURATION_REGISTER_FOR_STATUS_CHANNEL_REG_SINK_STATUS_ASYNC_RSTN_ID:
      field = &(regs->spi.general_sink_configuration_register_for_status_channel_reg.sink_status_async_rstn);
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
  soc_sand_os_printf( "spi.general_sink_configuration_register_for_status_channel_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_spi_general_sink_configuration_register_for_status_channel_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_general_sink_configuration_register_for_status_channel_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_GENERAL_SINK_CONFIGURATION_REGISTER_FOR_STATUS_CHANNEL_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_GENERAL_SINK_CONFIGURATION_REGISTER_FOR_STATUS_CHANNEL_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_GENERAL_SINK_CONFIGURATION_REGISTER_FOR_STATUS_CHANNEL_REG_REGISTER_ID:
      regis = &(regs->spi.general_sink_configuration_register_for_status_channel_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_GENERAL_SINK_CONFIGURATION_REGISTER_FOR_STATUS_CHANNEL_REG_SINK_STATUS_ENA_ID:
      field = &(regs->spi.general_sink_configuration_register_for_status_channel_reg.sink_status_ena);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_GENERAL_SINK_CONFIGURATION_REGISTER_FOR_STATUS_CHANNEL_REG_SINK_STATUS_ASYNC_RSTN_ID:
      field = &(regs->spi.general_sink_configuration_register_for_status_channel_reg.sink_status_async_rstn);
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
  ui_fap21v_acc_spi_source_data_train_len_m_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_source_data_train_len_m_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_SOURCE_DATA_TRAIN_LEN_M_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_SOURCE_DATA_TRAIN_LEN_M_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_SOURCE_DATA_TRAIN_LEN_M_REG_REGISTER_ID:
      regis = &(regs->spi.source_data_train_len_m_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_SOURCE_DATA_TRAIN_LEN_M_REG_SOURCE_DATA_TRAIN_LEN_M_ID:
      field = &(regs->spi.source_data_train_len_m_reg.source_data_train_len_m);
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
  soc_sand_os_printf( "spi.source_data_train_len_m_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_spi_source_data_train_len_m_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_source_data_train_len_m_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_SOURCE_DATA_TRAIN_LEN_M_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_SOURCE_DATA_TRAIN_LEN_M_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_SOURCE_DATA_TRAIN_LEN_M_REG_REGISTER_ID:
      regis = &(regs->spi.source_data_train_len_m_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_SOURCE_DATA_TRAIN_LEN_M_REG_SOURCE_DATA_TRAIN_LEN_M_ID:
      field = &(regs->spi.source_data_train_len_m_reg.source_data_train_len_m);
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
  ui_fap21v_acc_spi_under_flow_error_cnt_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_under_flow_error_cnt_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_UNDER_FLOW_ERROR_CNT_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_UNDER_FLOW_ERROR_CNT_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_UNDER_FLOW_ERROR_CNT_REG_REGISTER_ID:
      regis = &(regs->spi.under_flow_error_cnt_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_UNDER_FLOW_ERROR_CNT_REG_UNDER_FLOW_ERR_CNT_ID:
      field = &(regs->spi.under_flow_error_cnt_reg.under_flow_err_cnt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_UNDER_FLOW_ERROR_CNT_REG_OVERFLOW_ID:
      field = &(regs->spi.under_flow_error_cnt_reg.overflow);
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
  soc_sand_os_printf( "spi.under_flow_error_cnt_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_spi_under_flow_error_cnt_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_under_flow_error_cnt_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_UNDER_FLOW_ERROR_CNT_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_UNDER_FLOW_ERROR_CNT_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_UNDER_FLOW_ERROR_CNT_REG_REGISTER_ID:
      regis = &(regs->spi.under_flow_error_cnt_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_UNDER_FLOW_ERROR_CNT_REG_UNDER_FLOW_ERR_CNT_ID:
      field = &(regs->spi.under_flow_error_cnt_reg.under_flow_err_cnt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_UNDER_FLOW_ERROR_CNT_REG_OVERFLOW_ID:
      field = &(regs->spi.under_flow_error_cnt_reg.overflow);
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
  ui_fap21v_acc_spi_sink_status_calendar_m_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_sink_status_calendar_m_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_SINK_STATUS_CALENDAR_M_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_SINK_STATUS_CALENDAR_M_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_SINK_STATUS_CALENDAR_M_REG_REGISTER_ID:
      regis = &(regs->spi.sink_status_calendar_m_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_SINK_STATUS_CALENDAR_M_REG_SINK_STATUS_CALENDAR_M_ID:
      field = &(regs->spi.sink_status_calendar_m_reg.sink_status_calendar_m);
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
  soc_sand_os_printf( "spi.sink_status_calendar_m_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_spi_sink_status_calendar_m_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_sink_status_calendar_m_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_SINK_STATUS_CALENDAR_M_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_SINK_STATUS_CALENDAR_M_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_SINK_STATUS_CALENDAR_M_REG_REGISTER_ID:
      regis = &(regs->spi.sink_status_calendar_m_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_SINK_STATUS_CALENDAR_M_REG_SINK_STATUS_CALENDAR_M_ID:
      field = &(regs->spi.sink_status_calendar_m_reg.sink_status_calendar_m);
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
  ui_fap21v_acc_spi_indirect_command_wr_data_reg_0_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_indirect_command_wr_data_reg_0_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_INDIRECT_COMMAND_WR_DATA_REG_0_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_INDIRECT_COMMAND_WR_DATA_REG_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_INDIRECT_COMMAND_WR_DATA_REG_0_REGISTER_ID:
      regis = &(regs->spi.indirect_command_wr_data_reg_0.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_INDIRECT_COMMAND_WR_DATA_REG_0_INDIRECT_COMMAND_WR_DATA_ID:
      field = &(regs->spi.indirect_command_wr_data_reg_0.indirect_command_wr_data);
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
  soc_sand_os_printf( "spi.indirect_command_wr_data_reg_0: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_spi_indirect_command_wr_data_reg_0_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_indirect_command_wr_data_reg_0_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_INDIRECT_COMMAND_WR_DATA_REG_0_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_INDIRECT_COMMAND_WR_DATA_REG_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_INDIRECT_COMMAND_WR_DATA_REG_0_REGISTER_ID:
      regis = &(regs->spi.indirect_command_wr_data_reg_0.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_INDIRECT_COMMAND_WR_DATA_REG_0_INDIRECT_COMMAND_WR_DATA_ID:
      field = &(regs->spi.indirect_command_wr_data_reg_0.indirect_command_wr_data);
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
  ui_fap21v_acc_spi_general_sink_data_configuration_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_general_sink_data_configuration_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_GENERAL_SINK_DATA_CONFIGURATION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_GENERAL_SINK_DATA_CONFIGURATION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_GENERAL_SINK_DATA_CONFIGURATION_REG_REGISTER_ID:
      regis = &(regs->spi.general_sink_data_configuration_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_GENERAL_SINK_DATA_CONFIGURATION_REG_SINK_DATA_ENA_ID:
      field = &(regs->spi.general_sink_data_configuration_reg.sink_data_ena);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_GENERAL_SINK_DATA_CONFIGURATION_REG_SINK_DATA_LBENA_ID:
      field = &(regs->spi.general_sink_data_configuration_reg.sink_data_lbena);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_GENERAL_SINK_DATA_CONFIGURATION_REG_STAMP_CHANNEL_IDENA_ID:
      field = &(regs->spi.general_sink_data_configuration_reg.stamp_channel_idena);
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
  soc_sand_os_printf( "spi.general_sink_data_configuration_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_spi_general_sink_data_configuration_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_general_sink_data_configuration_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_GENERAL_SINK_DATA_CONFIGURATION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_GENERAL_SINK_DATA_CONFIGURATION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_GENERAL_SINK_DATA_CONFIGURATION_REG_REGISTER_ID:
      regis = &(regs->spi.general_sink_data_configuration_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_GENERAL_SINK_DATA_CONFIGURATION_REG_SINK_DATA_ENA_ID:
      field = &(regs->spi.general_sink_data_configuration_reg.sink_data_ena);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_GENERAL_SINK_DATA_CONFIGURATION_REG_SINK_DATA_LBENA_ID:
      field = &(regs->spi.general_sink_data_configuration_reg.sink_data_lbena);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_GENERAL_SINK_DATA_CONFIGURATION_REG_STAMP_CHANNEL_IDENA_ID:
      field = &(regs->spi.general_sink_data_configuration_reg.stamp_channel_idena);
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
  ui_fap21v_acc_spi_source_status_calendar_m_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_source_status_calendar_m_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_SOURCE_STATUS_CALENDAR_M_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_SOURCE_STATUS_CALENDAR_M_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_SOURCE_STATUS_CALENDAR_M_REG_REGISTER_ID:
      regis = &(regs->spi.source_status_calendar_m_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_SOURCE_STATUS_CALENDAR_M_REG_SOURCE_STATUS_CALENDAR_M_ID:
      field = &(regs->spi.source_status_calendar_m_reg.source_status_calendar_m);
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
  soc_sand_os_printf( "spi.source_status_calendar_m_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_spi_source_status_calendar_m_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_source_status_calendar_m_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_SOURCE_STATUS_CALENDAR_M_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_SOURCE_STATUS_CALENDAR_M_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_SOURCE_STATUS_CALENDAR_M_REG_REGISTER_ID:
      regis = &(regs->spi.source_status_calendar_m_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_SOURCE_STATUS_CALENDAR_M_REG_SOURCE_STATUS_CALENDAR_M_ID:
      field = &(regs->spi.source_status_calendar_m_reg.source_status_calendar_m);
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
  ui_fap21v_acc_spi_source_status_calendar_len_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_source_status_calendar_len_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_SOURCE_STATUS_CALENDAR_LEN_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_SOURCE_STATUS_CALENDAR_LEN_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_SOURCE_STATUS_CALENDAR_LEN_REG_REGISTER_ID:
      regis = &(regs->spi.source_status_calendar_len_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_SOURCE_STATUS_CALENDAR_LEN_REG_SOURCE_STATUS_CALENDER_LEN_ID:
      field = &(regs->spi.source_status_calendar_len_reg.source_status_calender_len);
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
  soc_sand_os_printf( "spi.source_status_calendar_len_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_spi_source_status_calendar_len_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_source_status_calendar_len_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_SOURCE_STATUS_CALENDAR_LEN_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_SOURCE_STATUS_CALENDAR_LEN_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_SOURCE_STATUS_CALENDAR_LEN_REG_REGISTER_ID:
      regis = &(regs->spi.source_status_calendar_len_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_SOURCE_STATUS_CALENDAR_LEN_REG_SOURCE_STATUS_CALENDER_LEN_ID:
      field = &(regs->spi.source_status_calendar_len_reg.source_status_calender_len);
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
  ui_fap21v_acc_spi_pcw_error_cnt_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_pcw_error_cnt_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_PCW_ERROR_CNT_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_PCW_ERROR_CNT_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_PCW_ERROR_CNT_REG_REGISTER_ID:
      regis = &(regs->spi.pcw_error_cnt_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_PCW_ERROR_CNT_REG_PCWERR_CNT_ID:
      field = &(regs->spi.pcw_error_cnt_reg.pcwerr_cnt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_PCW_ERROR_CNT_REG_PCWERR_CNT_O_ID:
      field = &(regs->spi.pcw_error_cnt_reg.pcwerr_cnt_o);
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
  soc_sand_os_printf( "spi.pcw_error_cnt_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_spi_pcw_error_cnt_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_pcw_error_cnt_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_PCW_ERROR_CNT_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_PCW_ERROR_CNT_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_PCW_ERROR_CNT_REG_REGISTER_ID:
      regis = &(regs->spi.pcw_error_cnt_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_PCW_ERROR_CNT_REG_PCWERR_CNT_ID:
      field = &(regs->spi.pcw_error_cnt_reg.pcwerr_cnt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_PCW_ERROR_CNT_REG_PCWERR_CNT_O_ID:
      field = &(regs->spi.pcw_error_cnt_reg.pcwerr_cnt_o);
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
  ui_fap21v_acc_spi_clear_sink_state_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_clear_sink_state_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_CLEAR_SINK_STATE_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_CLEAR_SINK_STATE_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_CLEAR_SINK_STATE_REG_REGISTER_ID:
      regis = &(regs->spi.clear_sink_state_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_CLEAR_SINK_STATE_REG_CLEAR_SINK_STATE_ID:
      field = &(regs->spi.clear_sink_state_reg.clear_sink_state);
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
  soc_sand_os_printf( "spi.clear_sink_state_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_spi_clear_sink_state_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_clear_sink_state_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_CLEAR_SINK_STATE_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_CLEAR_SINK_STATE_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_CLEAR_SINK_STATE_REG_REGISTER_ID:
      regis = &(regs->spi.clear_sink_state_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_CLEAR_SINK_STATE_REG_CLEAR_SINK_STATE_ID:
      field = &(regs->spi.clear_sink_state_reg.clear_sink_state);
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
  ui_fap21v_acc_spi_dip2_error_cnt_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_dip2_error_cnt_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_DIP2_ERROR_CNT_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_DIP2_ERROR_CNT_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_DIP2_ERROR_CNT_REG_REGISTER_ID:
      regis = &(regs->spi.dip2_error_cnt_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_DIP2_ERROR_CNT_REG_DIP2_ERR_CNT_ID:
      field = &(regs->spi.dip2_error_cnt_reg.dip2_err_cnt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_DIP2_ERROR_CNT_REG_DIP2_ERR_CNT_O_ID:
      field = &(regs->spi.dip2_error_cnt_reg.dip2_err_cnt_o);
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
  soc_sand_os_printf( "spi.dip2_error_cnt_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_spi_dip2_error_cnt_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_dip2_error_cnt_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_DIP2_ERROR_CNT_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_DIP2_ERROR_CNT_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_DIP2_ERROR_CNT_REG_REGISTER_ID:
      regis = &(regs->spi.dip2_error_cnt_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_DIP2_ERROR_CNT_REG_DIP2_ERR_CNT_ID:
      field = &(regs->spi.dip2_error_cnt_reg.dip2_err_cnt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_DIP2_ERROR_CNT_REG_DIP2_ERR_CNT_O_ID:
      field = &(regs->spi.dip2_error_cnt_reg.dip2_err_cnt_o);
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
  ui_fap21v_acc_spi_indirect_command_rd_data_reg_0_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_indirect_command_rd_data_reg_0_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_INDIRECT_COMMAND_RD_DATA_REG_0_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_INDIRECT_COMMAND_RD_DATA_REG_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_INDIRECT_COMMAND_RD_DATA_REG_0_REGISTER_ID:
      regis = &(regs->spi.indirect_command_rd_data_reg_0.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_INDIRECT_COMMAND_RD_DATA_REG_0_INDIRECT_COMMAND_RD_DATA_ID:
      field = &(regs->spi.indirect_command_rd_data_reg_0.indirect_command_rd_data);
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
  soc_sand_os_printf( "spi.indirect_command_rd_data_reg_0: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_spi_indirect_command_rd_data_reg_0_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_indirect_command_rd_data_reg_0_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_INDIRECT_COMMAND_RD_DATA_REG_0_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_INDIRECT_COMMAND_RD_DATA_REG_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_INDIRECT_COMMAND_RD_DATA_REG_0_REGISTER_ID:
      regis = &(regs->spi.indirect_command_rd_data_reg_0.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_INDIRECT_COMMAND_RD_DATA_REG_0_INDIRECT_COMMAND_RD_DATA_ID:
      field = &(regs->spi.indirect_command_rd_data_reg_0.indirect_command_rd_data);
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
  ui_fap21v_acc_spi_dip2_good_to_bad_ratio_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_dip2_good_to_bad_ratio_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_DIP2_GOOD_TO_BAD_RATIO_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_DIP2_GOOD_TO_BAD_RATIO_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_DIP2_GOOD_TO_BAD_RATIO_REG_REGISTER_ID:
      regis = &(regs->spi.dip2_good_to_bad_ratio_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_DIP2_GOOD_TO_BAD_RATIO_REG_DIP2_GOOD_TO_BAD_RATIO_ID:
      field = &(regs->spi.dip2_good_to_bad_ratio_reg.dip2_good_to_bad_ratio);
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
  soc_sand_os_printf( "spi.dip2_good_to_bad_ratio_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_spi_dip2_good_to_bad_ratio_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_dip2_good_to_bad_ratio_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_DIP2_GOOD_TO_BAD_RATIO_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_DIP2_GOOD_TO_BAD_RATIO_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_DIP2_GOOD_TO_BAD_RATIO_REG_REGISTER_ID:
      regis = &(regs->spi.dip2_good_to_bad_ratio_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_DIP2_GOOD_TO_BAD_RATIO_REG_DIP2_GOOD_TO_BAD_RATIO_ID:
      field = &(regs->spi.dip2_good_to_bad_ratio_reg.dip2_good_to_bad_ratio);
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
  ui_fap21v_acc_spi_sink_data_train_len_m_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_sink_data_train_len_m_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_SINK_DATA_TRAIN_LEN_M_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_SINK_DATA_TRAIN_LEN_M_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_SINK_DATA_TRAIN_LEN_M_REG_REGISTER_ID:
      regis = &(regs->spi.sink_data_train_len_m_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_SINK_DATA_TRAIN_LEN_M_REG_SINK_DATA_TAIN_LEN_M_ID:
      field = &(regs->spi.sink_data_train_len_m_reg.sink_data_tain_len_m);
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
  soc_sand_os_printf( "spi.sink_data_train_len_m_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_spi_sink_data_train_len_m_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_sink_data_train_len_m_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_SINK_DATA_TRAIN_LEN_M_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_SINK_DATA_TRAIN_LEN_M_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_SINK_DATA_TRAIN_LEN_M_REG_REGISTER_ID:
      regis = &(regs->spi.sink_data_train_len_m_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_SINK_DATA_TRAIN_LEN_M_REG_SINK_DATA_TAIN_LEN_M_ID:
      field = &(regs->spi.sink_data_train_len_m_reg.sink_data_tain_len_m);
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
  ui_fap21v_acc_spi_max_framing_pattern_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_max_framing_pattern_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_MAX_FRAMING_PATTERN_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_MAX_FRAMING_PATTERN_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_MAX_FRAMING_PATTERN_REG_REGISTER_ID:
      regis = &(regs->spi.max_framing_pattern_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_MAX_FRAMING_PATTERN_REG_MAX_FRM_PATTERN_ID:
      field = &(regs->spi.max_framing_pattern_reg.max_frm_pattern);
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
  soc_sand_os_printf( "spi.max_framing_pattern_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_spi_max_framing_pattern_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_max_framing_pattern_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_MAX_FRAMING_PATTERN_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_MAX_FRAMING_PATTERN_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_MAX_FRAMING_PATTERN_REG_REGISTER_ID:
      regis = &(regs->spi.max_framing_pattern_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_MAX_FRAMING_PATTERN_REG_MAX_FRM_PATTERN_ID:
      field = &(regs->spi.max_framing_pattern_reg.max_frm_pattern);
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
  ui_fap21v_acc_spi_sink_status_calendar_len_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_sink_status_calendar_len_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_SINK_STATUS_CALENDAR_LEN_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_SINK_STATUS_CALENDAR_LEN_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_SINK_STATUS_CALENDAR_LEN_REG_REGISTER_ID:
      regis = &(regs->spi.sink_status_calendar_len_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_SINK_STATUS_CALENDAR_LEN_REG_SINK_STATUS_CALENDAR_LEN_ID:
      field = &(regs->spi.sink_status_calendar_len_reg.sink_status_calendar_len);
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
  soc_sand_os_printf( "spi.sink_status_calendar_len_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_spi_sink_status_calendar_len_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_sink_status_calendar_len_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_SINK_STATUS_CALENDAR_LEN_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_SINK_STATUS_CALENDAR_LEN_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_SINK_STATUS_CALENDAR_LEN_REG_REGISTER_ID:
      regis = &(regs->spi.sink_status_calendar_len_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_SINK_STATUS_CALENDAR_LEN_REG_SINK_STATUS_CALENDAR_LEN_ID:
      field = &(regs->spi.sink_status_calendar_len_reg.sink_status_calendar_len);
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
  ui_fap21v_acc_spi_interrupt_sources_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_interrupt_sources_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_INTERRUPT_SOURCES_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_INTERRUPT_SOURCES_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_INTERRUPT_SOURCES_REG_REGISTER_ID:
      regis = &(regs->spi.interrupt_sources_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_INTERRUPT_SOURCES_REG_VANISHED_DVW_ID:
      field = &(regs->spi.interrupt_sources_reg.vanished_dvw);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_INTERRUPT_SOURCES_REG_TRAIN_ERROR_ID:
      field = &(regs->spi.interrupt_sources_reg.train_error);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_INTERRUPT_SOURCES_REG_SOURCE_STAT_NO_LOCK_ID:
      field = &(regs->spi.interrupt_sources_reg.source_stat_no_lock);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_INTERRUPT_SOURCES_REG_OUT_OF_FRAME_ID:
      field = &(regs->spi.interrupt_sources_reg.out_of_frame);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_INTERRUPT_SOURCES_REG_DIP2_ALARM_ID:
      field = &(regs->spi.interrupt_sources_reg.dip2_alarm);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_INTERRUPT_SOURCES_REG_LODS_ID:
      field = &(regs->spi.interrupt_sources_reg.lods);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_INTERRUPT_SOURCES_REG_SOURCE_UNDERFLOW_ERR_ID:
      field = &(regs->spi.interrupt_sources_reg.source_underflow_err);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_INTERRUPT_SOURCES_REG_SOURCE_FRM_ERROR_ID:
      field = &(regs->spi.interrupt_sources_reg.source_frm_error);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_INTERRUPT_SOURCES_REG_SOURCE_DIP2_ERROR_ID:
      field = &(regs->spi.interrupt_sources_reg.source_dip2_error);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_INTERRUPT_SOURCES_REG_SINK_DATA_ERROR_ID:
      field = &(regs->spi.interrupt_sources_reg.sink_data_error);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_INTERRUPT_SOURCES_REG_SINK_SOPERROR_ID:
      field = &(regs->spi.interrupt_sources_reg.sink_soperror);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_INTERRUPT_SOURCES_REG_SINK_EOPERROR_ID:
      field = &(regs->spi.interrupt_sources_reg.sink_eoperror);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_INTERRUPT_SOURCES_REG_SINK_PCWERROR_ID:
      field = &(regs->spi.interrupt_sources_reg.sink_pcwerror);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_INTERRUPT_SOURCES_REG_SINK_DIP4_ERROR_ID:
      field = &(regs->spi.interrupt_sources_reg.sink_dip4_error);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_INTERRUPT_SOURCES_REG_SINK_OVERFLOW_ERROR_ID:
      field = &(regs->spi.interrupt_sources_reg.sink_overflow_error);
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
  soc_sand_os_printf( "spi.interrupt_sources_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_spi_interrupt_sources_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_interrupt_sources_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_INTERRUPT_SOURCES_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_INTERRUPT_SOURCES_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_INTERRUPT_SOURCES_REG_REGISTER_ID:
      regis = &(regs->spi.interrupt_sources_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_INTERRUPT_SOURCES_REG_VANISHED_DVW_ID:
      field = &(regs->spi.interrupt_sources_reg.vanished_dvw);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_INTERRUPT_SOURCES_REG_TRAIN_ERROR_ID:
      field = &(regs->spi.interrupt_sources_reg.train_error);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_INTERRUPT_SOURCES_REG_SOURCE_STAT_NO_LOCK_ID:
      field = &(regs->spi.interrupt_sources_reg.source_stat_no_lock);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_INTERRUPT_SOURCES_REG_OUT_OF_FRAME_ID:
      field = &(regs->spi.interrupt_sources_reg.out_of_frame);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_INTERRUPT_SOURCES_REG_DIP2_ALARM_ID:
      field = &(regs->spi.interrupt_sources_reg.dip2_alarm);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_INTERRUPT_SOURCES_REG_LODS_ID:
      field = &(regs->spi.interrupt_sources_reg.lods);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_INTERRUPT_SOURCES_REG_SOURCE_UNDERFLOW_ERR_ID:
      field = &(regs->spi.interrupt_sources_reg.source_underflow_err);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_INTERRUPT_SOURCES_REG_SOURCE_FRM_ERROR_ID:
      field = &(regs->spi.interrupt_sources_reg.source_frm_error);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_INTERRUPT_SOURCES_REG_SOURCE_DIP2_ERROR_ID:
      field = &(regs->spi.interrupt_sources_reg.source_dip2_error);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_INTERRUPT_SOURCES_REG_SINK_DATA_ERROR_ID:
      field = &(regs->spi.interrupt_sources_reg.sink_data_error);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_INTERRUPT_SOURCES_REG_SINK_SOPERROR_ID:
      field = &(regs->spi.interrupt_sources_reg.sink_soperror);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_INTERRUPT_SOURCES_REG_SINK_EOPERROR_ID:
      field = &(regs->spi.interrupt_sources_reg.sink_eoperror);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_INTERRUPT_SOURCES_REG_SINK_PCWERROR_ID:
      field = &(regs->spi.interrupt_sources_reg.sink_pcwerror);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_INTERRUPT_SOURCES_REG_SINK_DIP4_ERROR_ID:
      field = &(regs->spi.interrupt_sources_reg.sink_dip4_error);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_INTERRUPT_SOURCES_REG_SINK_OVERFLOW_ERROR_ID:
      field = &(regs->spi.interrupt_sources_reg.sink_overflow_error);
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
  ui_fap21v_acc_spi_tsclk_edge_select_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_tsclk_edge_select_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_TSCLK_EDGE_SELECT_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_TSCLK_EDGE_SELECT_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_TSCLK_EDGE_SELECT_REG_REGISTER_ID:
      regis = &(regs->spi.tsclk_edge_select_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_TSCLK_EDGE_SELECT_REG_TSCLKEDGE_SEL_ID:
      field = &(regs->spi.tsclk_edge_select_reg.tsclkedge_sel);
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
  soc_sand_os_printf( "spi.tsclk_edge_select_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_spi_tsclk_edge_select_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_tsclk_edge_select_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_TSCLK_EDGE_SELECT_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_TSCLK_EDGE_SELECT_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_TSCLK_EDGE_SELECT_REG_REGISTER_ID:
      regis = &(regs->spi.tsclk_edge_select_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_TSCLK_EDGE_SELECT_REG_TSCLKEDGE_SEL_ID:
      field = &(regs->spi.tsclk_edge_select_reg.tsclkedge_sel);
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
  ui_fap21v_acc_spi_dip2_alarm_threshold_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_dip2_alarm_threshold_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_DIP2_ALARM_THRESHOLD_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_DIP2_ALARM_THRESHOLD_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_DIP2_ALARM_THRESHOLD_REG_REGISTER_ID:
      regis = &(regs->spi.dip2_alarm_threshold_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_DIP2_ALARM_THRESHOLD_REG_DIP2_ALARM_TH_ID:
      field = &(regs->spi.dip2_alarm_threshold_reg.dip2_alarm_th);
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
  soc_sand_os_printf( "spi.dip2_alarm_threshold_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_spi_dip2_alarm_threshold_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_dip2_alarm_threshold_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_DIP2_ALARM_THRESHOLD_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_DIP2_ALARM_THRESHOLD_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_DIP2_ALARM_THRESHOLD_REG_REGISTER_ID:
      regis = &(regs->spi.dip2_alarm_threshold_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_DIP2_ALARM_THRESHOLD_REG_DIP2_ALARM_TH_ID:
      field = &(regs->spi.dip2_alarm_threshold_reg.dip2_alarm_th);
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
  ui_fap21v_acc_spi_source_data_max_t_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_source_data_max_t_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_SOURCE_DATA_MAX_T_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_SOURCE_DATA_MAX_T_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_SOURCE_DATA_MAX_T_REG_REGISTER_ID:
      regis = &(regs->spi.source_data_max_t_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_SOURCE_DATA_MAX_T_REG_INITIAL_TRAINING_SEQ_ID:
      field = &(regs->spi.source_data_max_t_reg.source_data_max_t);
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
  soc_sand_os_printf( "spi.source_data_max_t_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_spi_source_data_max_t_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_source_data_max_t_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_SOURCE_DATA_MAX_T_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_SOURCE_DATA_MAX_T_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_SOURCE_DATA_MAX_T_REG_REGISTER_ID:
      regis = &(regs->spi.source_data_max_t_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_SOURCE_DATA_MAX_T_REG_INITIAL_TRAINING_SEQ_ID:
      field = &(regs->spi.source_data_max_t_reg.source_data_max_t);
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
  ui_fap21v_acc_spi_in_frame_threshold_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_in_frame_threshold_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_IN_FRAME_THRESHOLD_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_IN_FRAME_THRESHOLD_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_IN_FRAME_THRESHOLD_REG_REGISTER_ID:
      regis = &(regs->spi.in_frame_threshold_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_IN_FRAME_THRESHOLD_REG_IN_FRM_TH_ID:
      field = &(regs->spi.in_frame_threshold_reg.in_frm_th);
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
  soc_sand_os_printf( "spi.in_frame_threshold_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_spi_in_frame_threshold_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_in_frame_threshold_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_IN_FRAME_THRESHOLD_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_IN_FRAME_THRESHOLD_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_IN_FRAME_THRESHOLD_REG_REGISTER_ID:
      regis = &(regs->spi.in_frame_threshold_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_IN_FRAME_THRESHOLD_REG_IN_FRM_TH_ID:
      field = &(regs->spi.in_frame_threshold_reg.in_frm_th);
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
  ui_fap21v_acc_spi_out_of_frame_threshold_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_out_of_frame_threshold_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_OUT_OF_FRAME_THRESHOLD_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_OUT_OF_FRAME_THRESHOLD_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_OUT_OF_FRAME_THRESHOLD_REG_REGISTER_ID:
      regis = &(regs->spi.out_of_frame_threshold_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_OUT_OF_FRAME_THRESHOLD_REG_OUT_OF_FRM_TH_ID:
      field = &(regs->spi.out_of_frame_threshold_reg.out_of_frm_th);
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
  soc_sand_os_printf( "spi.out_of_frame_threshold_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_spi_out_of_frame_threshold_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_out_of_frame_threshold_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_OUT_OF_FRAME_THRESHOLD_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_OUT_OF_FRAME_THRESHOLD_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_OUT_OF_FRAME_THRESHOLD_REG_REGISTER_ID:
      regis = &(regs->spi.out_of_frame_threshold_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_OUT_OF_FRAME_THRESHOLD_REG_OUT_OF_FRM_TH_ID:
      field = &(regs->spi.out_of_frame_threshold_reg.out_of_frm_th);
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
  ui_fap21v_acc_spi_sync_good_dip4_threshold_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_sync_good_dip4_threshold_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_SYNC_GOOD_DIP4_THRESHOLD_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_SYNC_GOOD_DIP4_THRESHOLD_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_SYNC_GOOD_DIP4_THRESHOLD_REG_REGISTER_ID:
      regis = &(regs->spi.sync_good_dip4_threshold_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_SYNC_GOOD_DIP4_THRESHOLD_REG_SYNC_GOOD_DIP4_TH_ID:
      field = &(regs->spi.sync_good_dip4_threshold_reg.sync_good_dip4_th);
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
  soc_sand_os_printf( "spi.sync_good_dip4_threshold_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_spi_sync_good_dip4_threshold_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_sync_good_dip4_threshold_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_SYNC_GOOD_DIP4_THRESHOLD_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_SYNC_GOOD_DIP4_THRESHOLD_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_SYNC_GOOD_DIP4_THRESHOLD_REG_REGISTER_ID:
      regis = &(regs->spi.sync_good_dip4_threshold_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_SYNC_GOOD_DIP4_THRESHOLD_REG_SYNC_GOOD_DIP4_TH_ID:
      field = &(regs->spi.sync_good_dip4_threshold_reg.sync_good_dip4_th);
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
  ui_fap21v_acc_spi_interrupt_masks_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_interrupt_masks_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_INTERRUPT_MASKS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_INTERRUPT_MASKS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_INTERRUPT_MASKS_REG_REGISTER_ID:
      regis = &(regs->spi.interrupt_masks_reg.addr);
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
  soc_sand_os_printf( "spi.interrupt_masks_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_spi_interrupt_masks_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_interrupt_masks_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_INTERRUPT_MASKS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_INTERRUPT_MASKS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_INTERRUPT_MASKS_REG_REGISTER_ID:
      regis = &(regs->spi.interrupt_masks_reg.addr);
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
  ui_fap21v_acc_spi_indirect_command_address_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_indirect_command_address_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_INDIRECT_COMMAND_ADDRESS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_INDIRECT_COMMAND_ADDRESS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_INDIRECT_COMMAND_ADDRESS_REG_REGISTER_ID:
      regis = &(regs->spi.indirect_command_address_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_INDIRECT_COMMAND_ADDRESS_REG_INDIRECT_COMMAND_ADDRESS_ID:
      field = &(regs->spi.indirect_command_address_reg.indirect_command_address);
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
  soc_sand_os_printf( "spi.indirect_command_address_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_spi_indirect_command_address_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_indirect_command_address_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_INDIRECT_COMMAND_ADDRESS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_INDIRECT_COMMAND_ADDRESS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_INDIRECT_COMMAND_ADDRESS_REG_REGISTER_ID:
      regis = &(regs->spi.indirect_command_address_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_INDIRECT_COMMAND_ADDRESS_REG_INDIRECT_COMMAND_ADDRESS_ID:
      field = &(regs->spi.indirect_command_address_reg.indirect_command_address);
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
  ui_fap21v_acc_spi_general_source_data_configuration_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_general_source_data_configuration_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_GENERAL_SOURCE_DATA_CONFIGURATION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_GENERAL_SOURCE_DATA_CONFIGURATION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_GENERAL_SOURCE_DATA_CONFIGURATION_REG_REGISTER_ID:
      regis = &(regs->spi.general_source_data_configuration_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_GENERAL_SOURCE_DATA_CONFIGURATION_REG_SOURCE_DATA_ENA_ID:
      field = &(regs->spi.general_source_data_configuration_reg.source_data_ena);
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
  soc_sand_os_printf( "spi.general_source_data_configuration_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_spi_general_source_data_configuration_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_general_source_data_configuration_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_GENERAL_SOURCE_DATA_CONFIGURATION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_GENERAL_SOURCE_DATA_CONFIGURATION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_GENERAL_SOURCE_DATA_CONFIGURATION_REG_REGISTER_ID:
      regis = &(regs->spi.general_source_data_configuration_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_GENERAL_SOURCE_DATA_CONFIGURATION_REG_SOURCE_DATA_ENA_ID:
      field = &(regs->spi.general_source_data_configuration_reg.source_data_ena);
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
  ui_fap21v_acc_spi_general_source_fcs_configuration_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_general_source_fcs_configuration_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_GENERAL_SOURCE_FCS_CONFIGURATION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_GENERAL_SOURCE_FCS_CONFIGURATION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_GENERAL_SOURCE_FCS_CONFIGURATION_REG_REGISTER_ID:
      regis = &(regs->spi.general_source_fcs_configuration_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_GENERAL_SOURCE_FCS_CONFIGURATION_REG_SOURCE_STATUS_ENA_ID:
      field = &(regs->spi.general_source_fcs_configuration_reg.source_status_ena);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_GENERAL_SOURCE_FCS_CONFIGURATION_REG_SOURCE_STATUS_LBENA_ID:
      field = &(regs->spi.general_source_fcs_configuration_reg.source_status_lbena);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_GENERAL_SOURCE_FCS_CONFIGURATION_REG_SOURCE_STATUS_ASYNC_RSTN_ID:
      field = &(regs->spi.general_source_fcs_configuration_reg.source_status_async_rstn);
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
  soc_sand_os_printf( "spi.general_source_fcs_configuration_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_spi_general_source_fcs_configuration_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_general_source_fcs_configuration_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_GENERAL_SOURCE_FCS_CONFIGURATION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_GENERAL_SOURCE_FCS_CONFIGURATION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_GENERAL_SOURCE_FCS_CONFIGURATION_REG_REGISTER_ID:
      regis = &(regs->spi.general_source_fcs_configuration_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_GENERAL_SOURCE_FCS_CONFIGURATION_REG_SOURCE_STATUS_ENA_ID:
      field = &(regs->spi.general_source_fcs_configuration_reg.source_status_ena);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_GENERAL_SOURCE_FCS_CONFIGURATION_REG_SOURCE_STATUS_LBENA_ID:
      field = &(regs->spi.general_source_fcs_configuration_reg.source_status_lbena);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_GENERAL_SOURCE_FCS_CONFIGURATION_REG_SOURCE_STATUS_ASYNC_RSTN_ID:
      field = &(regs->spi.general_source_fcs_configuration_reg.source_status_async_rstn);
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
  ui_fap21v_acc_spi_sop_error_cnt_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_sop_error_cnt_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_SOP_ERROR_CNT_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_SOP_ERROR_CNT_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_SOP_ERROR_CNT_REG_REGISTER_ID:
      regis = &(regs->spi.sop_error_cnt_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_SOP_ERROR_CNT_REG_SOPERR_CNT_ID:
      field = &(regs->spi.sop_error_cnt_reg.soperr_cnt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_SOP_ERROR_CNT_REG_SOPERR_CNT_O_ID:
      field = &(regs->spi.sop_error_cnt_reg.soperr_cnt_o);
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
  soc_sand_os_printf( "spi.sop_error_cnt_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_spi_sop_error_cnt_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_sop_error_cnt_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_SOP_ERROR_CNT_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_SOP_ERROR_CNT_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_SOP_ERROR_CNT_REG_REGISTER_ID:
      regis = &(regs->spi.sop_error_cnt_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_SOP_ERROR_CNT_REG_SOPERR_CNT_ID:
      field = &(regs->spi.sop_error_cnt_reg.soperr_cnt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_SOP_ERROR_CNT_REG_SOPERR_CNT_O_ID:
      field = &(regs->spi.sop_error_cnt_reg.soperr_cnt_o);
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
  ui_fap21v_acc_spi_framing_error_cnt_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_framing_error_cnt_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_FRAMING_ERROR_CNT_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_FRAMING_ERROR_CNT_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_FRAMING_ERROR_CNT_REG_REGISTER_ID:
      regis = &(regs->spi.framing_error_cnt_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_FRAMING_ERROR_CNT_REG_FRM_ERR_CNT_ID:
      field = &(regs->spi.framing_error_cnt_reg.frm_err_cnt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_FRAMING_ERROR_CNT_REG_FRM_ERR_CNT_O_ID:
      field = &(regs->spi.framing_error_cnt_reg.frm_err_cnt_o);
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
  soc_sand_os_printf( "spi.framing_error_cnt_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_spi_framing_error_cnt_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_framing_error_cnt_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_FRAMING_ERROR_CNT_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_FRAMING_ERROR_CNT_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_FRAMING_ERROR_CNT_REG_REGISTER_ID:
      regis = &(regs->spi.framing_error_cnt_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_FRAMING_ERROR_CNT_REG_FRM_ERR_CNT_ID:
      field = &(regs->spi.framing_error_cnt_reg.frm_err_cnt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_FRAMING_ERROR_CNT_REG_FRM_ERR_CNT_O_ID:
      field = &(regs->spi.framing_error_cnt_reg.frm_err_cnt_o);
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
  ui_fap21v_acc_spi_eop_error_cnt_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_eop_error_cnt_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_EOP_ERROR_CNT_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_EOP_ERROR_CNT_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_EOP_ERROR_CNT_REG_REGISTER_ID:
      regis = &(regs->spi.eop_error_cnt_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_EOP_ERROR_CNT_REG_EOPERR_CNT_ID:
      field = &(regs->spi.eop_error_cnt_reg.eoperr_cnt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_EOP_ERROR_CNT_REG_EOPERR_CNT_O_ID:
      field = &(regs->spi.eop_error_cnt_reg.eoperr_cnt_o);
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
  soc_sand_os_printf( "spi.eop_error_cnt_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_spi_eop_error_cnt_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_eop_error_cnt_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_EOP_ERROR_CNT_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_EOP_ERROR_CNT_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_EOP_ERROR_CNT_REG_REGISTER_ID:
      regis = &(regs->spi.eop_error_cnt_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_EOP_ERROR_CNT_REG_EOPERR_CNT_ID:
      field = &(regs->spi.eop_error_cnt_reg.eoperr_cnt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_EOP_ERROR_CNT_REG_EOPERR_CNT_O_ID:
      field = &(regs->spi.eop_error_cnt_reg.eoperr_cnt_o);
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
  ui_fap21v_acc_spi_indirect_command_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_indirect_command_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_INDIRECT_COMMAND_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_INDIRECT_COMMAND_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_INDIRECT_COMMAND_REG_REGISTER_ID:
      regis = &(regs->spi.indirect_command_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_INDIRECT_COMMAND_REG_INDIRECT_COMMAND_ID:
      field = &(regs->spi.indirect_command_reg.indirect_command);
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
  soc_sand_os_printf( "spi.indirect_command_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_spi_indirect_command_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_indirect_command_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_INDIRECT_COMMAND_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_INDIRECT_COMMAND_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_INDIRECT_COMMAND_REG_REGISTER_ID:
      regis = &(regs->spi.indirect_command_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_INDIRECT_COMMAND_REG_INDIRECT_COMMAND_ID:
      field = &(regs->spi.indirect_command_reg.indirect_command);
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
  ui_fap21v_acc_spi_tss_rdy_position_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_tss_rdy_position_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_TSS_RDY_POSITION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_TSS_RDY_POSITION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_TSS_RDY_POSITION_REG_REGISTER_ID:
      regis = &(regs->spi.tss_rdy_position_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_TSS_RDY_POSITION_REG_TSS_RDY_POSITION_ID:
      field = &(regs->spi.tss_rdy_position_reg.tss_rdy_position);
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
  soc_sand_os_printf( "spi.tss_rdy_position_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_spi_tss_rdy_position_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_tss_rdy_position_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_TSS_RDY_POSITION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_TSS_RDY_POSITION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_TSS_RDY_POSITION_REG_REGISTER_ID:
      regis = &(regs->spi.tss_rdy_position_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_TSS_RDY_POSITION_REG_TSS_RDY_POSITION_ID:
      field = &(regs->spi.tss_rdy_position_reg.tss_rdy_position);
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
  ui_fap21v_acc_spi_data_link_state_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_data_link_state_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_DATA_LINK_STATE_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_DATA_LINK_STATE_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_DATA_LINK_STATE_REG_REGISTER_ID:
      regis = &(regs->spi.data_link_state_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_DATA_LINK_STATE_REG_LODS_ID:
      field = &(regs->spi.data_link_state_reg.lods);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_DATA_LINK_STATE_REG_DATA_SYNC_ID:
      field = &(regs->spi.data_link_state_reg.data_sync);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_DATA_LINK_STATE_REG_CHANNEL_ACTIVE_ID:
      field = &(regs->spi.data_link_state_reg.channel_active);
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
  soc_sand_os_printf( "spi.data_link_state_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_spi_data_link_state_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_data_link_state_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_DATA_LINK_STATE_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_DATA_LINK_STATE_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_DATA_LINK_STATE_REG_REGISTER_ID:
      regis = &(regs->spi.data_link_state_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_DATA_LINK_STATE_REG_LODS_ID:
      field = &(regs->spi.data_link_state_reg.lods);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_DATA_LINK_STATE_REG_DATA_SYNC_ID:
      field = &(regs->spi.data_link_state_reg.data_sync);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_DATA_LINK_STATE_REG_CHANNEL_ACTIVE_ID:
      field = &(regs->spi.data_link_state_reg.channel_active);
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
  ui_fap21v_acc_spi_dip4_good_to_bad_ratio_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_dip4_good_to_bad_ratio_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_DIP4_GOOD_TO_BAD_RATIO_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_DIP4_GOOD_TO_BAD_RATIO_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_DIP4_GOOD_TO_BAD_RATIO_REG_REGISTER_ID:
      regis = &(regs->spi.dip4_good_to_bad_ratio_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_DIP4_GOOD_TO_BAD_RATIO_REG_DIP4_GOOD_TO_BAD_RATIO_ID:
      field = &(regs->spi.dip4_good_to_bad_ratio_reg.dip4_good_to_bad_ratio);
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
  soc_sand_os_printf( "spi.dip4_good_to_bad_ratio_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_spi_dip4_good_to_bad_ratio_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_dip4_good_to_bad_ratio_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_DIP4_GOOD_TO_BAD_RATIO_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_DIP4_GOOD_TO_BAD_RATIO_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_DIP4_GOOD_TO_BAD_RATIO_REG_REGISTER_ID:
      regis = &(regs->spi.dip4_good_to_bad_ratio_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_DIP4_GOOD_TO_BAD_RATIO_REG_DIP4_GOOD_TO_BAD_RATIO_ID:
      field = &(regs->spi.dip4_good_to_bad_ratio_reg.dip4_good_to_bad_ratio);
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
  ui_fap21v_acc_spi_overflow_error_cnt_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_overflow_error_cnt_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_OVERFLOW_ERROR_CNT_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_OVERFLOW_ERROR_CNT_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_OVERFLOW_ERROR_CNT_REG_REGISTER_ID:
      regis = &(regs->spi.overflow_error_cnt_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_OVERFLOW_ERROR_CNT_REG_OVERFLOW_ERR_CNT_ID:
      field = &(regs->spi.overflow_error_cnt_reg.overflow_err_cnt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_OVERFLOW_ERROR_CNT_REG_OVERFLOW_ERR_CNT_O_ID:
      field = &(regs->spi.overflow_error_cnt_reg.overflow_err_cnt_o);
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
  soc_sand_os_printf( "spi.overflow_error_cnt_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_spi_overflow_error_cnt_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi");
  soc_sand_proc_name = "ui_fap21v_acc_spi_overflow_error_cnt_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_OVERFLOW_ERROR_CNT_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SPI_OVERFLOW_ERROR_CNT_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SPI_OVERFLOW_ERROR_CNT_REG_REGISTER_ID:
      regis = &(regs->spi.overflow_error_cnt_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_OVERFLOW_ERROR_CNT_REG_OVERFLOW_ERR_CNT_ID:
      field = &(regs->spi.overflow_error_cnt_reg.overflow_err_cnt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SPI_OVERFLOW_ERROR_CNT_REG_OVERFLOW_ERR_CNT_O_ID:
      field = &(regs->spi.overflow_error_cnt_reg.overflow_err_cnt_o);
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
  ui_fap21v_acc_spi_get(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi_get");

  if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_RSCLK_EDGE_SELECT_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_rsclk_edge_select_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_DATA_ERROR_CNT_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_data_error_cnt_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_DIP4_ERROR_CNT_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_dip4_error_cnt_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_DIP4_ALARM_THRESHOLD_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_dip4_alarm_threshold_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_GENERAL_SINK_CONFIGURATION_REGISTER_FOR_STATUS_CHANNEL_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_general_sink_configuration_register_for_status_channel_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_SOURCE_DATA_TRAIN_LEN_M_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_source_data_train_len_m_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_UNDER_FLOW_ERROR_CNT_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_under_flow_error_cnt_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_SINK_STATUS_CALENDAR_M_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_sink_status_calendar_m_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_INDIRECT_COMMAND_WR_DATA_REG_0_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_indirect_command_wr_data_reg_0_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_GENERAL_SINK_DATA_CONFIGURATION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_general_sink_data_configuration_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_SOURCE_STATUS_CALENDAR_M_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_source_status_calendar_m_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_SOURCE_STATUS_CALENDAR_LEN_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_source_status_calendar_len_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_PCW_ERROR_CNT_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_pcw_error_cnt_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_CLEAR_SINK_STATE_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_clear_sink_state_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_DIP2_ERROR_CNT_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_dip2_error_cnt_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_INDIRECT_COMMAND_RD_DATA_REG_0_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_indirect_command_rd_data_reg_0_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_DIP2_GOOD_TO_BAD_RATIO_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_dip2_good_to_bad_ratio_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_SINK_DATA_TRAIN_LEN_M_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_sink_data_train_len_m_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_MAX_FRAMING_PATTERN_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_max_framing_pattern_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_SINK_STATUS_CALENDAR_LEN_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_sink_status_calendar_len_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_INTERRUPT_SOURCES_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_interrupt_sources_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_TSCLK_EDGE_SELECT_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_tsclk_edge_select_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_DIP2_ALARM_THRESHOLD_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_dip2_alarm_threshold_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_SOURCE_DATA_MAX_T_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_source_data_max_t_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_IN_FRAME_THRESHOLD_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_in_frame_threshold_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_OUT_OF_FRAME_THRESHOLD_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_out_of_frame_threshold_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_SYNC_GOOD_DIP4_THRESHOLD_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_sync_good_dip4_threshold_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_INTERRUPT_MASKS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_interrupt_masks_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_INDIRECT_COMMAND_ADDRESS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_indirect_command_address_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_GENERAL_SOURCE_DATA_CONFIGURATION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_general_source_data_configuration_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_GENERAL_SOURCE_FCS_CONFIGURATION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_general_source_fcs_configuration_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_SOP_ERROR_CNT_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_sop_error_cnt_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_FRAMING_ERROR_CNT_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_framing_error_cnt_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_EOP_ERROR_CNT_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_eop_error_cnt_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_INDIRECT_COMMAND_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_indirect_command_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_TSS_RDY_POSITION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_tss_rdy_position_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_DATA_LINK_STATE_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_data_link_state_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_DIP4_GOOD_TO_BAD_RATIO_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_dip4_good_to_bad_ratio_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_OVERFLOW_ERROR_CNT_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_overflow_error_cnt_reg_get(current_line);
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
  ui_fap21v_acc_spi_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_spi_set");

  if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_RSCLK_EDGE_SELECT_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_rsclk_edge_select_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_DATA_ERROR_CNT_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_data_error_cnt_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_DIP4_ERROR_CNT_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_dip4_error_cnt_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_DIP4_ALARM_THRESHOLD_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_dip4_alarm_threshold_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_GENERAL_SINK_CONFIGURATION_REGISTER_FOR_STATUS_CHANNEL_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_general_sink_configuration_register_for_status_channel_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_SOURCE_DATA_TRAIN_LEN_M_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_source_data_train_len_m_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_UNDER_FLOW_ERROR_CNT_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_under_flow_error_cnt_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_SINK_STATUS_CALENDAR_M_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_sink_status_calendar_m_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_INDIRECT_COMMAND_WR_DATA_REG_0_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_indirect_command_wr_data_reg_0_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_GENERAL_SINK_DATA_CONFIGURATION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_general_sink_data_configuration_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_SOURCE_STATUS_CALENDAR_M_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_source_status_calendar_m_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_SOURCE_STATUS_CALENDAR_LEN_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_source_status_calendar_len_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_PCW_ERROR_CNT_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_pcw_error_cnt_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_CLEAR_SINK_STATE_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_clear_sink_state_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_DIP2_ERROR_CNT_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_dip2_error_cnt_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_INDIRECT_COMMAND_RD_DATA_REG_0_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_indirect_command_rd_data_reg_0_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_DIP2_GOOD_TO_BAD_RATIO_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_dip2_good_to_bad_ratio_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_SINK_DATA_TRAIN_LEN_M_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_sink_data_train_len_m_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_MAX_FRAMING_PATTERN_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_max_framing_pattern_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_SINK_STATUS_CALENDAR_LEN_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_sink_status_calendar_len_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_INTERRUPT_SOURCES_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_interrupt_sources_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_TSCLK_EDGE_SELECT_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_tsclk_edge_select_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_DIP2_ALARM_THRESHOLD_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_dip2_alarm_threshold_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_SOURCE_DATA_MAX_T_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_source_data_max_t_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_IN_FRAME_THRESHOLD_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_in_frame_threshold_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_OUT_OF_FRAME_THRESHOLD_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_out_of_frame_threshold_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_SYNC_GOOD_DIP4_THRESHOLD_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_sync_good_dip4_threshold_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_INTERRUPT_MASKS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_interrupt_masks_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_INDIRECT_COMMAND_ADDRESS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_indirect_command_address_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_GENERAL_SOURCE_DATA_CONFIGURATION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_general_source_data_configuration_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_GENERAL_SOURCE_FCS_CONFIGURATION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_general_source_fcs_configuration_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_SOP_ERROR_CNT_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_sop_error_cnt_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_FRAMING_ERROR_CNT_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_framing_error_cnt_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_EOP_ERROR_CNT_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_eop_error_cnt_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_INDIRECT_COMMAND_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_indirect_command_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_TSS_RDY_POSITION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_tss_rdy_position_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_DATA_LINK_STATE_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_data_link_state_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_DIP4_GOOD_TO_BAD_RATIO_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_dip4_good_to_bad_ratio_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SPI_OVERFLOW_ERROR_CNT_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_spi_overflow_error_cnt_reg_set(current_line, value);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after spi***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}
