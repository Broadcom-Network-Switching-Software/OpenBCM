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
  ui_fap21v_acc_mac_tx_pattern_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mac");
  soc_sand_proc_name = "ui_fap21v_acc_mac_tx_pattern_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_TX_PATTERN_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_TX_PATTERN_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_TX_PATTERN_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_TX_PATTERN_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MAC_TX_PATTERN_REG_REGISTER_ID:
      regis = &(regs->mac.tx_pattern_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_TX_PATTERN_REG_FMACTX_PTRN_ID:
      field = &(regs->mac.tx_pattern_reg[offset].fmactx_ptrn);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_TX_PATTERN_REG_PTRN20BIT_ID:
      field = &(regs->mac.tx_pattern_reg[offset].ptrn20bit);
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
  soc_sand_os_printf( "mac.tx_pattern_reg***: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_mac_tx_pattern_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mac");
  soc_sand_proc_name = "ui_fap21v_acc_mac_tx_pattern_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_TX_PATTERN_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_TX_PATTERN_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_TX_PATTERN_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_TX_PATTERN_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MAC_TX_PATTERN_REG_REGISTER_ID:
      regis = &(regs->mac.tx_pattern_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_TX_PATTERN_REG_FMACTX_PTRN_ID:
      field = &(regs->mac.tx_pattern_reg[offset].fmactx_ptrn);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_TX_PATTERN_REG_PTRN20BIT_ID:
      field = &(regs->mac.tx_pattern_reg[offset].ptrn20bit);
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
  ui_fap21v_acc_mac_ser_des_power_down_and_reset_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mac");
  soc_sand_proc_name = "ui_fap21v_acc_mac_ser_des_power_down_and_reset_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_SER_DES_POWER_DOWN_AND_RESET_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_SER_DES_POWER_DOWN_AND_RESET_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_SER_DES_POWER_DOWN_AND_RESET_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_SER_DES_POWER_DOWN_AND_RESET_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MAC_SER_DES_POWER_DOWN_AND_RESET_REG_REGISTER_ID:
      regis = &(regs->mac.ser_des_power_down_and_reset_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_SER_DES_POWER_DOWN_AND_RESET_REG_PDSN_ID:
      field = &(regs->mac.ser_des_power_down_and_reset_reg[offset].pdsn);
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
  soc_sand_os_printf( "mac.ser_des_power_down_and_reset_reg***: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_mac_ser_des_power_down_and_reset_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mac");
  soc_sand_proc_name = "ui_fap21v_acc_mac_ser_des_power_down_and_reset_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_SER_DES_POWER_DOWN_AND_RESET_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_SER_DES_POWER_DOWN_AND_RESET_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_SER_DES_POWER_DOWN_AND_RESET_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_SER_DES_POWER_DOWN_AND_RESET_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MAC_SER_DES_POWER_DOWN_AND_RESET_REG_REGISTER_ID:
      regis = &(regs->mac.ser_des_power_down_and_reset_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_SER_DES_POWER_DOWN_AND_RESET_REG_PDSN_ID:
      field = &(regs->mac.ser_des_power_down_and_reset_reg[offset].pdsn);
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
  ui_fap21v_acc_mac_interrupt_2_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mac");
  soc_sand_proc_name = "ui_fap21v_acc_mac_interrupt_2_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_2_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_2_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_2_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_2_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_2_REG_REGISTER_ID:
      regis = &(regs->mac.interrupt_2_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_2_REG_LOS_ID:
      field = &(regs->mac.interrupt_2_reg[offset].los);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_2_REG_RX_MIS_AERR_N_INT_ID:
      field = &(regs->mac.interrupt_2_reg[offset].rx_mis_aerr_n_int);
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
  soc_sand_os_printf( "mac.interrupt_2_reg***: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_mac_interrupt_2_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mac");
  soc_sand_proc_name = "ui_fap21v_acc_mac_interrupt_2_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_2_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_2_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_2_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_2_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_2_REG_REGISTER_ID:
      regis = &(regs->mac.interrupt_2_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_2_REG_LOS_ID:
      field = &(regs->mac.interrupt_2_reg[offset].los);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_2_REG_RX_MIS_AERR_N_INT_ID:
      field = &(regs->mac.interrupt_2_reg[offset].rx_mis_aerr_n_int);
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
  ui_fap21v_acc_mac_ser_des_pll_reset_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mac");
  soc_sand_proc_name = "ui_fap21v_acc_mac_ser_des_pll_reset_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_SER_DES_PLL_RESET_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_SER_DES_PLL_RESET_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_SER_DES_PLL_RESET_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_SER_DES_PLL_RESET_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MAC_SER_DES_PLL_RESET_REG_REGISTER_ID:
      regis = &(regs->mac.ser_des_pll_reset_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_SER_DES_PLL_RESET_REG_RST_PLL_ID:
      field = &(regs->mac.ser_des_pll_reset_reg[offset].rst_pll);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_SER_DES_PLL_RESET_REG_RST_SN_ID:
      field = &(regs->mac.ser_des_pll_reset_reg[offset].rst_sn);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_SER_DES_PLL_RESET_REG_SER_DES_RST_LOAD_ID:
      field = &(regs->mac.ser_des_pll_reset_reg[offset].ser_des_rst_load);
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
  soc_sand_os_printf( "mac.ser_des_pll_reset_reg***: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_mac_ser_des_pll_reset_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mac");
  soc_sand_proc_name = "ui_fap21v_acc_mac_ser_des_pll_reset_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_SER_DES_PLL_RESET_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_SER_DES_PLL_RESET_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_SER_DES_PLL_RESET_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_SER_DES_PLL_RESET_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MAC_SER_DES_PLL_RESET_REG_REGISTER_ID:
      regis = &(regs->mac.ser_des_pll_reset_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_SER_DES_PLL_RESET_REG_RST_PLL_ID:
      field = &(regs->mac.ser_des_pll_reset_reg[offset].rst_pll);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_SER_DES_PLL_RESET_REG_RST_SN_ID:
      field = &(regs->mac.ser_des_pll_reset_reg[offset].rst_sn);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_SER_DES_PLL_RESET_REG_SER_DES_RST_LOAD_ID:
      field = &(regs->mac.ser_des_pll_reset_reg[offset].ser_des_rst_load);
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
  ui_fap21v_acc_mac_serdes_configuration_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mac");
  soc_sand_proc_name = "ui_fap21v_acc_mac_serdes_configuration_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_SERDES_CONFIGURATION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_SERDES_CONFIGURATION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_SERDES_CONFIGURATION_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_SERDES_CONFIGURATION_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MAC_SERDES_CONFIGURATION_REG_REGISTER_ID:
      regis = &(regs->mac.serdes_configuration_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_SERDES_CONFIGURATION_REG_DRV_CRRNT_LVL_ID:
      field = &(regs->mac.serdes_configuration_reg[offset].drv_crrnt_lvl);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_SERDES_CONFIGURATION_REG_DRV_EQ_ID:
      field = &(regs->mac.serdes_configuration_reg[offset].drv_eq);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_SERDES_CONFIGURATION_REG_HI_DRV_ID:
      field = &(regs->mac.serdes_configuration_reg[offset].hi_drv);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_SERDES_CONFIGURATION_REG_LO_DRV_ID:
      field = &(regs->mac.serdes_configuration_reg[offset].lo_drv);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_SERDES_CONFIGURATION_REG_LPBK_ID:
      field = &(regs->mac.serdes_configuration_reg[offset].lpbk);
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
  soc_sand_os_printf( "mac.serdes_configuration_reg***: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_mac_serdes_configuration_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mac");
  soc_sand_proc_name = "ui_fap21v_acc_mac_serdes_configuration_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_SERDES_CONFIGURATION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_SERDES_CONFIGURATION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_SERDES_CONFIGURATION_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_SERDES_CONFIGURATION_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MAC_SERDES_CONFIGURATION_REG_REGISTER_ID:
      regis = &(regs->mac.serdes_configuration_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_SERDES_CONFIGURATION_REG_DRV_CRRNT_LVL_ID:
      field = &(regs->mac.serdes_configuration_reg[offset].drv_crrnt_lvl);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_SERDES_CONFIGURATION_REG_DRV_EQ_ID:
      field = &(regs->mac.serdes_configuration_reg[offset].drv_eq);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_SERDES_CONFIGURATION_REG_HI_DRV_ID:
      field = &(regs->mac.serdes_configuration_reg[offset].hi_drv);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_SERDES_CONFIGURATION_REG_LO_DRV_ID:
      field = &(regs->mac.serdes_configuration_reg[offset].lo_drv);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_SERDES_CONFIGURATION_REG_LPBK_ID:
      field = &(regs->mac.serdes_configuration_reg[offset].lpbk);
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
  ui_fap21v_acc_mac_leaky_bucket_control_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mac");
  soc_sand_proc_name = "ui_fap21v_acc_mac_leaky_bucket_control_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_LEAKY_BUCKET_CONTROL_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_LEAKY_BUCKET_CONTROL_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_LEAKY_BUCKET_CONTROL_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_LEAKY_BUCKET_CONTROL_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MAC_LEAKY_BUCKET_CONTROL_REG_REGISTER_ID:
      regis = &(regs->mac.leaky_bucket_control_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_LEAKY_BUCKET_CONTROL_REG_BKT_FILL_RATE_ID:
      field = &(regs->mac.leaky_bucket_control_reg[offset].bkt_fill_rate);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_LEAKY_BUCKET_CONTROL_REG_BKT_LINK_UP_TH_ID:
      field = &(regs->mac.leaky_bucket_control_reg[offset].bkt_link_up_th);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_LEAKY_BUCKET_CONTROL_REG_BKT_LINK_DN_TH_ID:
      field = &(regs->mac.leaky_bucket_control_reg[offset].bkt_link_dn_th);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_LEAKY_BUCKET_CONTROL_REG_SIG_DET_BKT_RST_ENA_ID:
      field = &(regs->mac.leaky_bucket_control_reg[offset].sig_det_bkt_rst_ena);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_LEAKY_BUCKET_CONTROL_REG_ALIGN_LCK_BKT_RST_ENA_ID:
      field = &(regs->mac.leaky_bucket_control_reg[offset].align_lck_bkt_rst_ena);
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
  soc_sand_os_printf( "mac.leaky_bucket_control_reg***: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_mac_leaky_bucket_control_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mac");
  soc_sand_proc_name = "ui_fap21v_acc_mac_leaky_bucket_control_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_LEAKY_BUCKET_CONTROL_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_LEAKY_BUCKET_CONTROL_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_LEAKY_BUCKET_CONTROL_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_LEAKY_BUCKET_CONTROL_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MAC_LEAKY_BUCKET_CONTROL_REG_REGISTER_ID:
      regis = &(regs->mac.leaky_bucket_control_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_LEAKY_BUCKET_CONTROL_REG_BKT_FILL_RATE_ID:
      field = &(regs->mac.leaky_bucket_control_reg[offset].bkt_fill_rate);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_LEAKY_BUCKET_CONTROL_REG_BKT_LINK_UP_TH_ID:
      field = &(regs->mac.leaky_bucket_control_reg[offset].bkt_link_up_th);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_LEAKY_BUCKET_CONTROL_REG_BKT_LINK_DN_TH_ID:
      field = &(regs->mac.leaky_bucket_control_reg[offset].bkt_link_dn_th);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_LEAKY_BUCKET_CONTROL_REG_SIG_DET_BKT_RST_ENA_ID:
      field = &(regs->mac.leaky_bucket_control_reg[offset].sig_det_bkt_rst_ena);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_LEAKY_BUCKET_CONTROL_REG_ALIGN_LCK_BKT_RST_ENA_ID:
      field = &(regs->mac.leaky_bucket_control_reg[offset].align_lck_bkt_rst_ena);
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
  ui_fap21v_acc_mac_mac_transmit_cell_counters_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mac");
  soc_sand_proc_name = "ui_fap21v_acc_mac_mac_transmit_cell_counters_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_MAC_TRANSMIT_CELL_COUNTERS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_MAC_TRANSMIT_CELL_COUNTERS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_MAC_TRANSMIT_CELL_COUNTERS_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_MAC_TRANSMIT_CELL_COUNTERS_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MAC_MAC_TRANSMIT_CELL_COUNTERS_REG_REGISTER_ID:
      regis = &(regs->mac.mac_transmit_cell_counters_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_MAC_TRANSMIT_CELL_COUNTERS_REG_TX_CELL_CNT_N_ID:
      field = &(regs->mac.mac_transmit_cell_counters_reg[offset].tx_cell_cnt_n);
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
  soc_sand_os_printf( "mac.mac_transmit_cell_counters_reg***: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_mac_mac_transmit_cell_counters_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mac");
  soc_sand_proc_name = "ui_fap21v_acc_mac_mac_transmit_cell_counters_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_MAC_TRANSMIT_CELL_COUNTERS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_MAC_TRANSMIT_CELL_COUNTERS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_MAC_TRANSMIT_CELL_COUNTERS_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_MAC_TRANSMIT_CELL_COUNTERS_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MAC_MAC_TRANSMIT_CELL_COUNTERS_REG_REGISTER_ID:
      regis = &(regs->mac.mac_transmit_cell_counters_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_MAC_TRANSMIT_CELL_COUNTERS_REG_TX_CELL_CNT_N_ID:
      field = &(regs->mac.mac_transmit_cell_counters_reg[offset].tx_cell_cnt_n);
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
  ui_fap21v_acc_mac_single_mac_crc_error_counter_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mac");
  soc_sand_proc_name = "ui_fap21v_acc_mac_single_mac_crc_error_counter_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_SINGLE_MAC_CRC_ERROR_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_SINGLE_MAC_CRC_ERROR_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_SINGLE_MAC_CRC_ERROR_COUNTER_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_SINGLE_MAC_CRC_ERROR_COUNTER_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MAC_SINGLE_MAC_CRC_ERROR_COUNTER_REG_REGISTER_ID:
      regis = &(regs->mac.single_mac_crc_error_counter_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_SINGLE_MAC_CRC_ERROR_COUNTER_REG_CRCERR_CNT_ID:
      field = &(regs->mac.single_mac_crc_error_counter_reg[offset].crcerr_cnt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_SINGLE_MAC_CRC_ERROR_COUNTER_REG_CRCERR_CNT_OVF_ID:
      field = &(regs->mac.single_mac_crc_error_counter_reg[offset].crcerr_cnt_ovf);
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
  soc_sand_os_printf( "mac.single_mac_crc_error_counter_reg***: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_mac_single_mac_crc_error_counter_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mac");
  soc_sand_proc_name = "ui_fap21v_acc_mac_single_mac_crc_error_counter_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_SINGLE_MAC_CRC_ERROR_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_SINGLE_MAC_CRC_ERROR_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_SINGLE_MAC_CRC_ERROR_COUNTER_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_SINGLE_MAC_CRC_ERROR_COUNTER_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MAC_SINGLE_MAC_CRC_ERROR_COUNTER_REG_REGISTER_ID:
      regis = &(regs->mac.single_mac_crc_error_counter_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_SINGLE_MAC_CRC_ERROR_COUNTER_REG_CRCERR_CNT_ID:
      field = &(regs->mac.single_mac_crc_error_counter_reg[offset].crcerr_cnt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_SINGLE_MAC_CRC_ERROR_COUNTER_REG_CRCERR_CNT_OVF_ID:
      field = &(regs->mac.single_mac_crc_error_counter_reg[offset].crcerr_cnt_ovf);
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
  ui_fap21v_acc_mac_interrupt_3_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mac");
  soc_sand_proc_name = "ui_fap21v_acc_mac_interrupt_3_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_3_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_3_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_3_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_3_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_3_REG_REGISTER_ID:
      regis = &(regs->mac.interrupt_3_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_3_REG_RX_TDMHDR_ID:
      field = &(regs->mac.interrupt_3_reg[offset].rx_tdmhdr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_3_REG_RX_DISP_ERR_N_INT_ID:
      field = &(regs->mac.interrupt_3_reg[offset].rx_disp_err_n_int);
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
  soc_sand_os_printf( "mac.interrupt_3_reg***: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_mac_interrupt_3_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mac");
  soc_sand_proc_name = "ui_fap21v_acc_mac_interrupt_3_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_3_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_3_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_3_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_3_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_3_REG_REGISTER_ID:
      regis = &(regs->mac.interrupt_3_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_3_REG_RX_TDMHDR_ID:
      field = &(regs->mac.interrupt_3_reg[offset].rx_tdmhdr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_3_REG_RX_DISP_ERR_N_INT_ID:
      field = &(regs->mac.interrupt_3_reg[offset].rx_disp_err_n_int);
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
  ui_fap21v_acc_mac_general_ser_des_configurations_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mac");
  soc_sand_proc_name = "ui_fap21v_acc_mac_general_ser_des_configurations_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_GENERAL_SER_DES_CONFIGURATIONS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_GENERAL_SER_DES_CONFIGURATIONS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_GENERAL_SER_DES_CONFIGURATIONS_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_GENERAL_SER_DES_CONFIGURATIONS_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MAC_GENERAL_SER_DES_CONFIGURATIONS_REG_REGISTER_ID:
      regis = &(regs->mac.general_ser_des_configurations_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_GENERAL_SER_DES_CONFIGURATIONS_REG_SER_DES_TERM_RES_ADJ_ID:
      field = &(regs->mac.general_ser_des_configurations_reg[offset].ser_des_term_res_adj);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_GENERAL_SER_DES_CONFIGURATIONS_REG_SER_DES_INIT_WAIT_ID:
      field = &(regs->mac.general_ser_des_configurations_reg[offset].ser_des_init_wait);
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
  soc_sand_os_printf( "mac.general_ser_des_configurations_reg***: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_mac_general_ser_des_configurations_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mac");
  soc_sand_proc_name = "ui_fap21v_acc_mac_general_ser_des_configurations_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_GENERAL_SER_DES_CONFIGURATIONS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_GENERAL_SER_DES_CONFIGURATIONS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_GENERAL_SER_DES_CONFIGURATIONS_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_GENERAL_SER_DES_CONFIGURATIONS_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MAC_GENERAL_SER_DES_CONFIGURATIONS_REG_REGISTER_ID:
      regis = &(regs->mac.general_ser_des_configurations_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_GENERAL_SER_DES_CONFIGURATIONS_REG_SER_DES_TERM_RES_ADJ_ID:
      field = &(regs->mac.general_ser_des_configurations_reg[offset].ser_des_term_res_adj);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_GENERAL_SER_DES_CONFIGURATIONS_REG_SER_DES_INIT_WAIT_ID:
      field = &(regs->mac.general_ser_des_configurations_reg[offset].ser_des_init_wait);
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
  ui_fap21v_acc_mac_interrupt_4_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mac");
  soc_sand_proc_name = "ui_fap21v_acc_mac_interrupt_4_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_4_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_4_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_4_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_4_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_4_REG_REGISTER_ID:
      regis = &(regs->mac.interrupt_4_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_4_REG_LNKLVL_AGE_N_ID:
      field = &(regs->mac.interrupt_4_reg[offset].lnklvl_age_n);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_4_REG_LNKLVL_HALT_N_ID:
      field = &(regs->mac.interrupt_4_reg[offset].lnklvl_halt_n);
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
  soc_sand_os_printf( "mac.interrupt_4_reg***: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_mac_interrupt_4_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mac");
  soc_sand_proc_name = "ui_fap21v_acc_mac_interrupt_4_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_4_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_4_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_4_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_4_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_4_REG_REGISTER_ID:
      regis = &(regs->mac.interrupt_4_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_4_REG_LNKLVL_AGE_N_ID:
      field = &(regs->mac.interrupt_4_reg[offset].lnklvl_age_n);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_4_REG_LNKLVL_HALT_N_ID:
      field = &(regs->mac.interrupt_4_reg[offset].lnklvl_halt_n);
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
  ui_fap21v_acc_mac_mac_link_level_flow_control_and_comma_configuration_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mac");
  soc_sand_proc_name = "ui_fap21v_acc_mac_mac_link_level_flow_control_and_comma_configuration_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_MAC_LINK_LEVEL_FLOW_CONTROL_AND_COMMA_CONFIGURATION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_MAC_LINK_LEVEL_FLOW_CONTROL_AND_COMMA_CONFIGURATION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_MAC_LINK_LEVEL_FLOW_CONTROL_AND_COMMA_CONFIGURATION_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_MAC_LINK_LEVEL_FLOW_CONTROL_AND_COMMA_CONFIGURATION_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MAC_MAC_LINK_LEVEL_FLOW_CONTROL_AND_COMMA_CONFIGURATION_REG_REGISTER_ID:
      regis = &(regs->mac.mac_link_level_flow_control_and_comma_configuration_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_MAC_LINK_LEVEL_FLOW_CONTROL_AND_COMMA_CONFIGURATION_REG_LNK_LVL_AGE_PRD_ID:
      field = &(regs->mac.mac_link_level_flow_control_and_comma_configuration_reg[offset].lnk_lvl_age_prd);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_MAC_LINK_LEVEL_FLOW_CONTROL_AND_COMMA_CONFIGURATION_REG_LNK_LVL_FRAG_NUM_ID:
      field = &(regs->mac.mac_link_level_flow_control_and_comma_configuration_reg[offset].lnk_lvl_frag_num);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_MAC_LINK_LEVEL_FLOW_CONTROL_AND_COMMA_CONFIGURATION_REG_CM_BRST_SIZE_ID:
      field = &(regs->mac.mac_link_level_flow_control_and_comma_configuration_reg[offset].cm_brst_size);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_MAC_LINK_LEVEL_FLOW_CONTROL_AND_COMMA_CONFIGURATION_REG_CM_TX_PERIOD_ID:
      field = &(regs->mac.mac_link_level_flow_control_and_comma_configuration_reg[offset].cm_tx_period);
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
  soc_sand_os_printf( "mac.mac_link_level_flow_control_and_comma_configuration_reg***: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_mac_mac_link_level_flow_control_and_comma_configuration_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mac");
  soc_sand_proc_name = "ui_fap21v_acc_mac_mac_link_level_flow_control_and_comma_configuration_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_MAC_LINK_LEVEL_FLOW_CONTROL_AND_COMMA_CONFIGURATION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_MAC_LINK_LEVEL_FLOW_CONTROL_AND_COMMA_CONFIGURATION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_MAC_LINK_LEVEL_FLOW_CONTROL_AND_COMMA_CONFIGURATION_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_MAC_LINK_LEVEL_FLOW_CONTROL_AND_COMMA_CONFIGURATION_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MAC_MAC_LINK_LEVEL_FLOW_CONTROL_AND_COMMA_CONFIGURATION_REG_REGISTER_ID:
      regis = &(regs->mac.mac_link_level_flow_control_and_comma_configuration_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_MAC_LINK_LEVEL_FLOW_CONTROL_AND_COMMA_CONFIGURATION_REG_LNK_LVL_AGE_PRD_ID:
      field = &(regs->mac.mac_link_level_flow_control_and_comma_configuration_reg[offset].lnk_lvl_age_prd);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_MAC_LINK_LEVEL_FLOW_CONTROL_AND_COMMA_CONFIGURATION_REG_LNK_LVL_FRAG_NUM_ID:
      field = &(regs->mac.mac_link_level_flow_control_and_comma_configuration_reg[offset].lnk_lvl_frag_num);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_MAC_LINK_LEVEL_FLOW_CONTROL_AND_COMMA_CONFIGURATION_REG_CM_BRST_SIZE_ID:
      field = &(regs->mac.mac_link_level_flow_control_and_comma_configuration_reg[offset].cm_brst_size);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_MAC_LINK_LEVEL_FLOW_CONTROL_AND_COMMA_CONFIGURATION_REG_CM_TX_PERIOD_ID:
      field = &(regs->mac.mac_link_level_flow_control_and_comma_configuration_reg[offset].cm_tx_period);
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
  ui_fap21v_acc_mac_ber_counters_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mac");
  soc_sand_proc_name = "ui_fap21v_acc_mac_ber_counters_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_BER_COUNTERS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_BER_COUNTERS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_BER_COUNTERS_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_BER_COUNTERS_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MAC_BER_COUNTERS_REG_REGISTER_ID:
      regis = &(regs->mac.ber_counters_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_BER_COUNTERS_REG_BERCNT_ID:
      field = &(regs->mac.ber_counters_reg[offset].bercnt);
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
  soc_sand_os_printf( "mac.ber_counters_reg***: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_mac_ber_counters_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mac");
  soc_sand_proc_name = "ui_fap21v_acc_mac_ber_counters_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_BER_COUNTERS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_BER_COUNTERS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_BER_COUNTERS_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_BER_COUNTERS_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MAC_BER_COUNTERS_REG_REGISTER_ID:
      regis = &(regs->mac.ber_counters_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_BER_COUNTERS_REG_BERCNT_ID:
      field = &(regs->mac.ber_counters_reg[offset].bercnt);
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
  ui_fap21v_acc_mac_leaky_bucket_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mac");
  soc_sand_proc_name = "ui_fap21v_acc_mac_leaky_bucket_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_LEAKY_BUCKET_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_LEAKY_BUCKET_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_LEAKY_BUCKET_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_LEAKY_BUCKET_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MAC_LEAKY_BUCKET_REG_REGISTER_ID:
      regis = &(regs->mac.leaky_bucket_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_LEAKY_BUCKET_REG_LKY_BKT_VALUE_ID:
      field = &(regs->mac.leaky_bucket_reg[offset].lky_bkt_value);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_LEAKY_BUCKET_REG_RX_GOOD_CELL_CNT_ID:
      field = &(regs->mac.leaky_bucket_reg[offset].rx_good_cell_cnt);
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
  soc_sand_os_printf( "mac.leaky_bucket_reg***: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_mac_leaky_bucket_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mac");
  soc_sand_proc_name = "ui_fap21v_acc_mac_leaky_bucket_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_LEAKY_BUCKET_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_LEAKY_BUCKET_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_LEAKY_BUCKET_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_LEAKY_BUCKET_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MAC_LEAKY_BUCKET_REG_REGISTER_ID:
      regis = &(regs->mac.leaky_bucket_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_LEAKY_BUCKET_REG_LKY_BKT_VALUE_ID:
      field = &(regs->mac.leaky_bucket_reg[offset].lky_bkt_value);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_LEAKY_BUCKET_REG_RX_GOOD_CELL_CNT_ID:
      field = &(regs->mac.leaky_bucket_reg[offset].rx_good_cell_cnt);
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
  ui_fap21v_acc_mac_interrupt_1_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mac");
  soc_sand_proc_name = "ui_fap21v_acc_mac_interrupt_1_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_1_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_1_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_1_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_1_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_1_REG_REGISTER_ID:
      regis = &(regs->mac.interrupt_1_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_1_REG_RX_CRCERR_N_INT_ID:
      field = &(regs->mac.interrupt_1_reg[offset].rx_crcerr_n_int);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_1_REG_RX_CGERR_N_INT_ID:
      field = &(regs->mac.interrupt_1_reg[offset].rx_cgerr_n_int);
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
  soc_sand_os_printf( "mac.interrupt_1_reg***: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_mac_interrupt_1_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mac");
  soc_sand_proc_name = "ui_fap21v_acc_mac_interrupt_1_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_1_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_1_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_1_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_1_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_1_REG_REGISTER_ID:
      regis = &(regs->mac.interrupt_1_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_1_REG_RX_CRCERR_N_INT_ID:
      field = &(regs->mac.interrupt_1_reg[offset].rx_crcerr_n_int);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_1_REG_RX_CGERR_N_INT_ID:
      field = &(regs->mac.interrupt_1_reg[offset].rx_cgerr_n_int);
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
  ui_fap21v_acc_mac_mac_enablers_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mac");
  soc_sand_proc_name = "ui_fap21v_acc_mac_mac_enablers_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_MAC_ENABLERS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_MAC_ENABLERS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_MAC_ENABLERS_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_MAC_ENABLERS_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MAC_MAC_ENABLERS_REG_REGISTER_ID:
      regis = &(regs->mac.mac_enablers_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_MAC_ENABLERS_REG_ENABLE_SERIAL_LINK_ID:
      field = &(regs->mac.mac_enablers_reg[offset].enable_serial_link);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_MAC_ENABLERS_REG_DEL_CRCERR_CELL_ID:
      field = &(regs->mac.mac_enablers_reg[offset].del_crcerr_cell);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_MAC_ENABLERS_REG_DEL_TDMCRCERR_CELL_ID:
      field = &(regs->mac.mac_enablers_reg[offset].del_tdmcrcerr_cell);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_MAC_ENABLERS_REG_DEL_TDMHDR_CRCERR_CELL_ID:
      field = &(regs->mac.mac_enablers_reg[offset].del_tdmhdr_crcerr_cell);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_MAC_ENABLERS_REG_DEL_DATA_CELL_LB_ID:
      field = &(regs->mac.mac_enablers_reg[offset].del_data_cell_lb);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_MAC_ENABLERS_REG_DEL_FSCR_CELL_LB_ID:
      field = &(regs->mac.mac_enablers_reg[offset].del_fscr_cell_lb);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_MAC_ENABLERS_REG_DEL_ALLCELLS_LB_ID:
      field = &(regs->mac.mac_enablers_reg[offset].del_allcells_lb);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_MAC_ENABLERS_REG_TX_CNT_CFG_ID:
      field = &(regs->mac.mac_enablers_reg[offset].tx_cnt_cfg);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_MAC_ENABLERS_REG_RX_CNT_CFG_ID:
      field = &(regs->mac.mac_enablers_reg[offset].rx_cnt_cfg);
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
  soc_sand_os_printf( "mac.mac_enablers_reg***: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_mac_mac_enablers_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mac");
  soc_sand_proc_name = "ui_fap21v_acc_mac_mac_enablers_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_MAC_ENABLERS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_MAC_ENABLERS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_MAC_ENABLERS_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_MAC_ENABLERS_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MAC_MAC_ENABLERS_REG_REGISTER_ID:
      regis = &(regs->mac.mac_enablers_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_MAC_ENABLERS_REG_ENABLE_SERIAL_LINK_ID:
      field = &(regs->mac.mac_enablers_reg[offset].enable_serial_link);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_MAC_ENABLERS_REG_DEL_CRCERR_CELL_ID:
      field = &(regs->mac.mac_enablers_reg[offset].del_crcerr_cell);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_MAC_ENABLERS_REG_DEL_TDMCRCERR_CELL_ID:
      field = &(regs->mac.mac_enablers_reg[offset].del_tdmcrcerr_cell);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_MAC_ENABLERS_REG_DEL_TDMHDR_CRCERR_CELL_ID:
      field = &(regs->mac.mac_enablers_reg[offset].del_tdmhdr_crcerr_cell);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_MAC_ENABLERS_REG_DEL_DATA_CELL_LB_ID:
      field = &(regs->mac.mac_enablers_reg[offset].del_data_cell_lb);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_MAC_ENABLERS_REG_DEL_FSCR_CELL_LB_ID:
      field = &(regs->mac.mac_enablers_reg[offset].del_fscr_cell_lb);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_MAC_ENABLERS_REG_DEL_ALLCELLS_LB_ID:
      field = &(regs->mac.mac_enablers_reg[offset].del_allcells_lb);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_MAC_ENABLERS_REG_TX_CNT_CFG_ID:
      field = &(regs->mac.mac_enablers_reg[offset].tx_cnt_cfg);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_MAC_ENABLERS_REG_RX_CNT_CFG_ID:
      field = &(regs->mac.mac_enablers_reg[offset].rx_cnt_cfg);
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
  ui_fap21v_acc_mac_mac_receive_reset_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mac");
  soc_sand_proc_name = "ui_fap21v_acc_mac_mac_receive_reset_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_MAC_RECEIVE_RESET_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_MAC_RECEIVE_RESET_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_MAC_RECEIVE_RESET_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_MAC_RECEIVE_RESET_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MAC_MAC_RECEIVE_RESET_REG_REGISTER_ID:
      regis = &(regs->mac.mac_receive_reset_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_MAC_RECEIVE_RESET_REG_FMACRX_RST_ID:
      field = &(regs->mac.mac_receive_reset_reg[offset].fmacrx_rst);
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
  soc_sand_os_printf( "mac.mac_receive_reset_reg***: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_mac_mac_receive_reset_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mac");
  soc_sand_proc_name = "ui_fap21v_acc_mac_mac_receive_reset_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_MAC_RECEIVE_RESET_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_MAC_RECEIVE_RESET_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_MAC_RECEIVE_RESET_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_MAC_RECEIVE_RESET_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MAC_MAC_RECEIVE_RESET_REG_REGISTER_ID:
      regis = &(regs->mac.mac_receive_reset_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_MAC_RECEIVE_RESET_REG_FMACRX_RST_ID:
      field = &(regs->mac.mac_receive_reset_reg[offset].fmacrx_rst);
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
  ui_fap21v_acc_mac_single_mac_loopback_prbs_and_link_level_flow_control_enable_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mac");
  soc_sand_proc_name = "ui_fap21v_acc_mac_single_mac_loopback_prbs_and_link_level_flow_control_enable_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_SINGLE_MAC_LOOPBACK_PRBS_AND_LINK_LEVEL_FLOW_CONTROL_ENABLE_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_SINGLE_MAC_LOOPBACK_PRBS_AND_LINK_LEVEL_FLOW_CONTROL_ENABLE_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_SINGLE_MAC_LOOPBACK_PRBS_AND_LINK_LEVEL_FLOW_CONTROL_ENABLE_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_SINGLE_MAC_LOOPBACK_PRBS_AND_LINK_LEVEL_FLOW_CONTROL_ENABLE_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MAC_SINGLE_MAC_LOOPBACK_PRBS_AND_LINK_LEVEL_FLOW_CONTROL_ENABLE_REG_REGISTER_ID:
      regis = &(regs->mac.single_mac_loopback_prbs_and_link_level_flow_control_enable_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_SINGLE_MAC_LOOPBACK_PRBS_AND_LINK_LEVEL_FLOW_CONTROL_ENABLE_REG_PRBSGEN_ON_ID:
      field = &(regs->mac.single_mac_loopback_prbs_and_link_level_flow_control_enable_reg[offset].prbsgen_on);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_SINGLE_MAC_LOOPBACK_PRBS_AND_LINK_LEVEL_FLOW_CONTROL_ENABLE_REG_PRBSCHECK_ON_ID:
      field = &(regs->mac.single_mac_loopback_prbs_and_link_level_flow_control_enable_reg[offset].prbscheck_on);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_SINGLE_MAC_LOOPBACK_PRBS_AND_LINK_LEVEL_FLOW_CONTROL_ENABLE_REG_LCL_LPBK_ON_ID:
      field = &(regs->mac.single_mac_loopback_prbs_and_link_level_flow_control_enable_reg[offset].lcl_lpbk_on);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_SINGLE_MAC_LOOPBACK_PRBS_AND_LINK_LEVEL_FLOW_CONTROL_ENABLE_REG_LNK_LVL_FC_TX_EN_ID:
      field = &(regs->mac.single_mac_loopback_prbs_and_link_level_flow_control_enable_reg[offset].lnk_lvl_fc_tx_en);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_SINGLE_MAC_LOOPBACK_PRBS_AND_LINK_LEVEL_FLOW_CONTROL_ENABLE_REG_LNK_LVL_FC_RX_EN_ID:
      field = &(regs->mac.single_mac_loopback_prbs_and_link_level_flow_control_enable_reg[offset].lnk_lvl_fc_rx_en);
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
  soc_sand_os_printf( "mac.single_mac_loopback_prbs_and_link_level_flow_control_enable_reg***: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_mac_single_mac_loopback_prbs_and_link_level_flow_control_enable_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mac");
  soc_sand_proc_name = "ui_fap21v_acc_mac_single_mac_loopback_prbs_and_link_level_flow_control_enable_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_SINGLE_MAC_LOOPBACK_PRBS_AND_LINK_LEVEL_FLOW_CONTROL_ENABLE_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_SINGLE_MAC_LOOPBACK_PRBS_AND_LINK_LEVEL_FLOW_CONTROL_ENABLE_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_SINGLE_MAC_LOOPBACK_PRBS_AND_LINK_LEVEL_FLOW_CONTROL_ENABLE_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_MAC_SINGLE_MAC_LOOPBACK_PRBS_AND_LINK_LEVEL_FLOW_CONTROL_ENABLE_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MAC_SINGLE_MAC_LOOPBACK_PRBS_AND_LINK_LEVEL_FLOW_CONTROL_ENABLE_REG_REGISTER_ID:
      regis = &(regs->mac.single_mac_loopback_prbs_and_link_level_flow_control_enable_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_SINGLE_MAC_LOOPBACK_PRBS_AND_LINK_LEVEL_FLOW_CONTROL_ENABLE_REG_PRBSGEN_ON_ID:
      field = &(regs->mac.single_mac_loopback_prbs_and_link_level_flow_control_enable_reg[offset].prbsgen_on);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_SINGLE_MAC_LOOPBACK_PRBS_AND_LINK_LEVEL_FLOW_CONTROL_ENABLE_REG_PRBSCHECK_ON_ID:
      field = &(regs->mac.single_mac_loopback_prbs_and_link_level_flow_control_enable_reg[offset].prbscheck_on);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_SINGLE_MAC_LOOPBACK_PRBS_AND_LINK_LEVEL_FLOW_CONTROL_ENABLE_REG_LCL_LPBK_ON_ID:
      field = &(regs->mac.single_mac_loopback_prbs_and_link_level_flow_control_enable_reg[offset].lcl_lpbk_on);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_SINGLE_MAC_LOOPBACK_PRBS_AND_LINK_LEVEL_FLOW_CONTROL_ENABLE_REG_LNK_LVL_FC_TX_EN_ID:
      field = &(regs->mac.single_mac_loopback_prbs_and_link_level_flow_control_enable_reg[offset].lnk_lvl_fc_tx_en);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MAC_SINGLE_MAC_LOOPBACK_PRBS_AND_LINK_LEVEL_FLOW_CONTROL_ENABLE_REG_LNK_LVL_FC_RX_EN_ID:
      field = &(regs->mac.single_mac_loopback_prbs_and_link_level_flow_control_enable_reg[offset].lnk_lvl_fc_rx_en);
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
  ui_fap21v_acc_mac_get(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mac_get");

  if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_TX_PATTERN_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_mac_tx_pattern_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_SER_DES_POWER_DOWN_AND_RESET_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_mac_ser_des_power_down_and_reset_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_2_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_mac_interrupt_2_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_SER_DES_PLL_RESET_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_mac_ser_des_pll_reset_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_SERDES_CONFIGURATION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_mac_serdes_configuration_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_LEAKY_BUCKET_CONTROL_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_mac_leaky_bucket_control_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_MAC_TRANSMIT_CELL_COUNTERS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_mac_mac_transmit_cell_counters_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_SINGLE_MAC_CRC_ERROR_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_mac_single_mac_crc_error_counter_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_3_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_mac_interrupt_3_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_GENERAL_SER_DES_CONFIGURATIONS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_mac_general_ser_des_configurations_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_4_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_mac_interrupt_4_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_MAC_LINK_LEVEL_FLOW_CONTROL_AND_COMMA_CONFIGURATION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_mac_mac_link_level_flow_control_and_comma_configuration_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_BER_COUNTERS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_mac_ber_counters_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_LEAKY_BUCKET_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_mac_leaky_bucket_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_1_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_mac_interrupt_1_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_MAC_ENABLERS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_mac_mac_enablers_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_MAC_RECEIVE_RESET_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_mac_mac_receive_reset_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_SINGLE_MAC_LOOPBACK_PRBS_AND_LINK_LEVEL_FLOW_CONTROL_ENABLE_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_mac_single_mac_loopback_prbs_and_link_level_flow_control_enable_reg_get(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after mac***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_mac_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mac_set");

  if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_TX_PATTERN_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_mac_tx_pattern_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_SER_DES_POWER_DOWN_AND_RESET_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_mac_ser_des_power_down_and_reset_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_2_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_mac_interrupt_2_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_SER_DES_PLL_RESET_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_mac_ser_des_pll_reset_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_SERDES_CONFIGURATION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_mac_serdes_configuration_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_LEAKY_BUCKET_CONTROL_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_mac_leaky_bucket_control_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_MAC_TRANSMIT_CELL_COUNTERS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_mac_mac_transmit_cell_counters_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_SINGLE_MAC_CRC_ERROR_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_mac_single_mac_crc_error_counter_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_3_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_mac_interrupt_3_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_GENERAL_SER_DES_CONFIGURATIONS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_mac_general_ser_des_configurations_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_4_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_mac_interrupt_4_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_MAC_LINK_LEVEL_FLOW_CONTROL_AND_COMMA_CONFIGURATION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_mac_mac_link_level_flow_control_and_comma_configuration_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_BER_COUNTERS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_mac_ber_counters_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_LEAKY_BUCKET_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_mac_leaky_bucket_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_INTERRUPT_1_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_mac_interrupt_1_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_MAC_ENABLERS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_mac_mac_enablers_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_MAC_RECEIVE_RESET_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_mac_mac_receive_reset_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MAC_SINGLE_MAC_LOOPBACK_PRBS_AND_LINK_LEVEL_FLOW_CONTROL_ENABLE_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_mac_single_mac_loopback_prbs_and_link_level_flow_control_enable_reg_set(current_line, value);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after mac***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}
