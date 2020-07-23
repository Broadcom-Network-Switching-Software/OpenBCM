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
  ui_fap21v_acc_fdt_interrupt_mask_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdt");
  soc_sand_proc_name = "ui_fap21v_acc_fdt_interrupt_mask_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_INTERRUPT_MASK_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDT_INTERRUPT_MASK_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDT_INTERRUPT_MASK_REG_REGISTER_ID:
      regis = &(regs->fdt.interrupt_mask_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_INTERRUPT_MASK_REG_UNRCH_DEST_EVT_MASK_ID:
      field = &(regs->fdt.interrupt_mask_reg.unrch_dest_evt_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_INTERRUPT_MASK_REG_UNIDUP_ID:
      field = &(regs->fdt.interrupt_mask_reg.unidup);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_INTERRUPT_MASK_REG_ECC_ERROR_ID:
      field = &(regs->fdt.interrupt_mask_reg.ecc_error);
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
  soc_sand_os_printf( "fdt.interrupt_mask_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fdt_interrupt_mask_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdt");
  soc_sand_proc_name = "ui_fap21v_acc_fdt_interrupt_mask_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_INTERRUPT_MASK_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDT_INTERRUPT_MASK_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDT_INTERRUPT_MASK_REG_REGISTER_ID:
      regis = &(regs->fdt.interrupt_mask_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_INTERRUPT_MASK_REG_UNRCH_DEST_EVT_MASK_ID:
      field = &(regs->fdt.interrupt_mask_reg.unrch_dest_evt_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_INTERRUPT_MASK_REG_UNIDUP_ID:
      field = &(regs->fdt.interrupt_mask_reg.unidup);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_INTERRUPT_MASK_REG_ECC_ERROR_ID:
      field = &(regs->fdt.interrupt_mask_reg.ecc_error);
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
  ui_fap21v_acc_fdt_transmit_data_cells_registers_2_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdt");
  soc_sand_proc_name = "ui_fap21v_acc_fdt_transmit_data_cells_registers_2_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_2_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_2_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_2_REG_REGISTER_ID:
      regis = &(regs->fdt.transmit_data_cells_registers_reg[2].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_2_REG_CPUDATA_CELL_ID:
      field = &(regs->fdt.transmit_data_cells_registers_reg[2].cpudata_cell);
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
  soc_sand_os_printf( "fdt.transmit_data_cells_registers_reg[2]: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fdt_transmit_data_cells_registers_2_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdt");
  soc_sand_proc_name = "ui_fap21v_acc_fdt_transmit_data_cells_registers_2_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_2_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_2_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_2_REG_REGISTER_ID:
      regis = &(regs->fdt.transmit_data_cells_registers_reg[2].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_2_REG_CPUDATA_CELL_ID:
      field = &(regs->fdt.transmit_data_cells_registers_reg[2].cpudata_cell);
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
  ui_fap21v_acc_fdt_unicast_coexist_bitmap_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdt");
  soc_sand_proc_name = "ui_fap21v_acc_fdt_unicast_coexist_bitmap_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_UNICAST_COEXIST_BITMAP_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDT_UNICAST_COEXIST_BITMAP_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_UNICAST_COEXIST_BITMAP_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_FDT_UNICAST_COEXIST_BITMAP_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDT_UNICAST_COEXIST_BITMAP_REG_REGISTER_ID:
      regis = &(regs->fdt.unicast_coexist_bitmap_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_UNICAST_COEXIST_BITMAP_REG_UNI_COEX_BMP_ID:
      field = &(regs->fdt.unicast_coexist_bitmap_reg[offset].uni_coex_bmp);
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
  soc_sand_os_printf( "fdt.unicast_coexist_bitmap_reg***: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fdt_unicast_coexist_bitmap_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdt");
  soc_sand_proc_name = "ui_fap21v_acc_fdt_unicast_coexist_bitmap_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_UNICAST_COEXIST_BITMAP_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDT_UNICAST_COEXIST_BITMAP_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_UNICAST_COEXIST_BITMAP_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_FDT_UNICAST_COEXIST_BITMAP_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDT_UNICAST_COEXIST_BITMAP_REG_REGISTER_ID:
      regis = &(regs->fdt.unicast_coexist_bitmap_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_UNICAST_COEXIST_BITMAP_REG_UNI_COEX_BMP_ID:
      field = &(regs->fdt.unicast_coexist_bitmap_reg[offset].uni_coex_bmp);
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
  ui_fap21v_acc_fdt_transmit_data_cell_trigger_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdt");
  soc_sand_proc_name = "ui_fap21v_acc_fdt_transmit_data_cell_trigger_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELL_TRIGGER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELL_TRIGGER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELL_TRIGGER_REG_REGISTER_ID:
      regis = &(regs->fdt.transmit_data_cell_trigger_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELL_TRIGGER_REG_CELL_TRG_ID:
      field = &(regs->fdt.transmit_data_cell_trigger_reg.cell_trg);
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
  soc_sand_os_printf( "fdt.transmit_data_cell_trigger_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fdt_transmit_data_cell_trigger_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdt");
  soc_sand_proc_name = "ui_fap21v_acc_fdt_transmit_data_cell_trigger_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELL_TRIGGER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELL_TRIGGER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELL_TRIGGER_REG_REGISTER_ID:
      regis = &(regs->fdt.transmit_data_cell_trigger_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELL_TRIGGER_REG_CELL_TRG_ID:
      field = &(regs->fdt.transmit_data_cell_trigger_reg.cell_trg);
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
  ui_fap21v_acc_fdt_indirect_command_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdt");
  soc_sand_proc_name = "ui_fap21v_acc_fdt_indirect_command_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_INDIRECT_COMMAND_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDT_INDIRECT_COMMAND_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDT_INDIRECT_COMMAND_REG_REGISTER_ID:
      regis = &(regs->fdt.indirect_command_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_INDIRECT_COMMAND_REG_INDIRECT_COMMAND_ID:
      field = &(regs->fdt.indirect_command_reg.indirect_command);
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
  soc_sand_os_printf( "fdt.indirect_command_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fdt_indirect_command_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdt");
  soc_sand_proc_name = "ui_fap21v_acc_fdt_indirect_command_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_INDIRECT_COMMAND_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDT_INDIRECT_COMMAND_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDT_INDIRECT_COMMAND_REG_REGISTER_ID:
      regis = &(regs->fdt.indirect_command_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_INDIRECT_COMMAND_REG_INDIRECT_COMMAND_ID:
      field = &(regs->fdt.indirect_command_reg.indirect_command);
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
  ui_fap21v_acc_fdt_context_defines_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdt");
  soc_sand_proc_name = "ui_fap21v_acc_fdt_context_defines_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_CONTEXT_DEFINES_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDT_CONTEXT_DEFINES_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_CONTEXT_DEFINES_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_FDT_CONTEXT_DEFINES_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDT_CONTEXT_DEFINES_REG_REGISTER_ID:
      regis = &(regs->fdt.context_defines_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_CONTEXT_DEFINES_REG_CONTEXT_LINK_MASK_ID:
      field = &(regs->fdt.context_defines_reg[offset].context_link_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_CONTEXT_DEFINES_REG_CONTEXT_DEST_ID_ID:
      field = &(regs->fdt.context_defines_reg[offset].context_dest_id);
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
  soc_sand_os_printf( "fdt.context_defines_reg***: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fdt_context_defines_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdt");
  soc_sand_proc_name = "ui_fap21v_acc_fdt_context_defines_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_CONTEXT_DEFINES_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDT_CONTEXT_DEFINES_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_CONTEXT_DEFINES_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_FDT_CONTEXT_DEFINES_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDT_CONTEXT_DEFINES_REG_REGISTER_ID:
      regis = &(regs->fdt.context_defines_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_CONTEXT_DEFINES_REG_CONTEXT_LINK_MASK_ID:
      field = &(regs->fdt.context_defines_reg[offset].context_link_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_CONTEXT_DEFINES_REG_CONTEXT_DEST_ID_ID:
      field = &(regs->fdt.context_defines_reg[offset].context_dest_id);
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
  ui_fap21v_acc_fdt_transmit_data_cells_registers_0_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdt");
  soc_sand_proc_name = "ui_fap21v_acc_fdt_transmit_data_cells_registers_0_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_0_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_0_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_0_REG_REGISTER_ID:
      regis = &(regs->fdt.transmit_data_cells_registers_reg[0].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_0_REG_CPUDATA_CELL_ID:
      field = &(regs->fdt.transmit_data_cells_registers_reg[0].cpudata_cell);
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
  soc_sand_os_printf( "fdt.transmit_data_cells_registers_reg[0]: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fdt_transmit_data_cells_registers_0_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdt");
  soc_sand_proc_name = "ui_fap21v_acc_fdt_transmit_data_cells_registers_0_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_0_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_0_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_0_REG_REGISTER_ID:
      regis = &(regs->fdt.transmit_data_cells_registers_reg[0].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_0_REG_CPUDATA_CELL_ID:
      field = &(regs->fdt.transmit_data_cells_registers_reg[0].cpudata_cell);
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
  ui_fap21v_acc_fdt_transmit_cell_output_link_number_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdt");
  soc_sand_proc_name = "ui_fap21v_acc_fdt_transmit_cell_output_link_number_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_CELL_OUTPUT_LINK_NUMBER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_CELL_OUTPUT_LINK_NUMBER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_CELL_OUTPUT_LINK_NUMBER_REG_REGISTER_ID:
      regis = &(regs->fdt.transmit_cell_output_link_number_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_CELL_OUTPUT_LINK_NUMBER_REG_CPU_LINK_NUM_ID:
      field = &(regs->fdt.transmit_cell_output_link_number_reg.cpu_link_num);
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
  soc_sand_os_printf( "fdt.transmit_cell_output_link_number_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fdt_transmit_cell_output_link_number_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdt");
  soc_sand_proc_name = "ui_fap21v_acc_fdt_transmit_cell_output_link_number_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_CELL_OUTPUT_LINK_NUMBER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_CELL_OUTPUT_LINK_NUMBER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_CELL_OUTPUT_LINK_NUMBER_REG_REGISTER_ID:
      regis = &(regs->fdt.transmit_cell_output_link_number_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_CELL_OUTPUT_LINK_NUMBER_REG_CPU_LINK_NUM_ID:
      field = &(regs->fdt.transmit_cell_output_link_number_reg.cpu_link_num);
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
  ui_fap21v_acc_fdt_transmit_data_cells_registers_5_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdt");
  soc_sand_proc_name = "ui_fap21v_acc_fdt_transmit_data_cells_registers_5_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_5_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_5_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_5_REG_REGISTER_ID:
      regis = &(regs->fdt.transmit_data_cells_registers_reg[5].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_5_REG_CPUDATA_CELL_ID:
      field = &(regs->fdt.transmit_data_cells_registers_reg[5].cpudata_cell);
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
  soc_sand_os_printf( "fdt.transmit_data_cells_registers_reg[5]: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fdt_transmit_data_cells_registers_5_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdt");
  soc_sand_proc_name = "ui_fap21v_acc_fdt_transmit_data_cells_registers_5_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_5_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_5_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_5_REG_REGISTER_ID:
      regis = &(regs->fdt.transmit_data_cells_registers_reg[5].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_5_REG_CPUDATA_CELL_ID:
      field = &(regs->fdt.transmit_data_cells_registers_reg[5].cpudata_cell);
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
  ui_fap21v_acc_fdt_fdt_enablers_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdt");
  soc_sand_proc_name = "ui_fap21v_acc_fdt_fdt_enablers_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_FDT_ENABLERS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDT_FDT_ENABLERS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDT_FDT_ENABLERS_REG_REGISTER_ID:
      regis = &(regs->fdt.fdt_enablers_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_FDT_ENABLERS_REG_DISC_ALL_PKTS_ID:
      field = &(regs->fdt.fdt_enablers_reg.disc_all_pkts);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_FDT_ENABLERS_REG_SEGMENT_PKT_PDC_ID:
      field = &(regs->fdt.fdt_enablers_reg.segment_pkt_pdc);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_FDT_ENABLERS_REG_DEL_CRCPKT_ID:
      field = &(regs->fdt.fdt_enablers_reg.del_crcpkt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_FDT_ENABLERS_REG_DIS_LCL_RT_ID:
      field = &(regs->fdt.fdt_enablers_reg.dis_lcl_rt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_FDT_ENABLERS_REG_FOR_ALL_PDC_LOCAL_ID:
      field = &(regs->fdt.fdt_enablers_reg.for_all_pdc_local);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_FDT_ENABLERS_REG_FOR_ALL_LBP_LOCAL_ID:
      field = &(regs->fdt.fdt_enablers_reg.for_all_lbp_local);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_FDT_ENABLERS_REG_MESH_TYPE_ID:
      field = &(regs->fdt.fdt_enablers_reg.mesh_type);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_FDT_ENABLERS_REG_TDM_DISCARD_EN_ID:
      field = &(regs->fdt.fdt_enablers_reg.tdm_discard_en);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_FDT_ENABLERS_REG_SIZE_ERR_DISCARD_ID:
      field = &(regs->fdt.fdt_enablers_reg.size_err_discard);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_FDT_ENABLERS_REG_ALL_ONES_DISCARD_ID:
      field = &(regs->fdt.fdt_enablers_reg.all_ones_discard);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_FDT_ENABLERS_REG_WRONG_PCK_TYPE_ID:
      field = &(regs->fdt.fdt_enablers_reg.wrong_pck_type);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_FDT_ENABLERS_REG_MUL_COEXIST_ENABLE_ID:
      field = &(regs->fdt.fdt_enablers_reg.mul_coexist_enable);
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
  soc_sand_os_printf( "fdt.fdt_enablers_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fdt_fdt_enablers_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdt");
  soc_sand_proc_name = "ui_fap21v_acc_fdt_fdt_enablers_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_FDT_ENABLERS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDT_FDT_ENABLERS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDT_FDT_ENABLERS_REG_REGISTER_ID:
      regis = &(regs->fdt.fdt_enablers_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_FDT_ENABLERS_REG_DISC_ALL_PKTS_ID:
      field = &(regs->fdt.fdt_enablers_reg.disc_all_pkts);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_FDT_ENABLERS_REG_SEGMENT_PKT_PDC_ID:
      field = &(regs->fdt.fdt_enablers_reg.segment_pkt_pdc);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_FDT_ENABLERS_REG_DEL_CRCPKT_ID:
      field = &(regs->fdt.fdt_enablers_reg.del_crcpkt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_FDT_ENABLERS_REG_DIS_LCL_RT_ID:
      field = &(regs->fdt.fdt_enablers_reg.dis_lcl_rt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_FDT_ENABLERS_REG_FOR_ALL_PDC_LOCAL_ID:
      field = &(regs->fdt.fdt_enablers_reg.for_all_pdc_local);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_FDT_ENABLERS_REG_FOR_ALL_LBP_LOCAL_ID:
      field = &(regs->fdt.fdt_enablers_reg.for_all_lbp_local);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_FDT_ENABLERS_REG_MESH_TYPE_ID:
      field = &(regs->fdt.fdt_enablers_reg.mesh_type);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_FDT_ENABLERS_REG_TDM_DISCARD_EN_ID:
      field = &(regs->fdt.fdt_enablers_reg.tdm_discard_en);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_FDT_ENABLERS_REG_SIZE_ERR_DISCARD_ID:
      field = &(regs->fdt.fdt_enablers_reg.size_err_discard);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_FDT_ENABLERS_REG_ALL_ONES_DISCARD_ID:
      field = &(regs->fdt.fdt_enablers_reg.all_ones_discard);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_FDT_ENABLERS_REG_WRONG_PCK_TYPE_ID:
      field = &(regs->fdt.fdt_enablers_reg.wrong_pck_type);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_FDT_ENABLERS_REG_MUL_COEXIST_ENABLE_ID:
      field = &(regs->fdt.fdt_enablers_reg.mul_coexist_enable);
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
  ui_fap21v_acc_fdt_indirect_command_rd_data_reg_0_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdt");
  soc_sand_proc_name = "ui_fap21v_acc_fdt_indirect_command_rd_data_reg_0_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_INDIRECT_COMMAND_RD_DATA_REG_0_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDT_INDIRECT_COMMAND_RD_DATA_REG_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDT_INDIRECT_COMMAND_RD_DATA_REG_0_REGISTER_ID:
      regis = &(regs->fdt.indirect_command_rd_data_reg_0.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_INDIRECT_COMMAND_RD_DATA_REG_0_INDIRECT_COMMAND_RD_DATA_ID:
      field = &(regs->fdt.indirect_command_rd_data_reg_0.indirect_command_rd_data);
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
  soc_sand_os_printf( "fdt.indirect_command_rd_data_reg_0: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fdt_indirect_command_rd_data_reg_0_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdt");
  soc_sand_proc_name = "ui_fap21v_acc_fdt_indirect_command_rd_data_reg_0_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_INDIRECT_COMMAND_RD_DATA_REG_0_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDT_INDIRECT_COMMAND_RD_DATA_REG_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDT_INDIRECT_COMMAND_RD_DATA_REG_0_REGISTER_ID:
      regis = &(regs->fdt.indirect_command_rd_data_reg_0.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_INDIRECT_COMMAND_RD_DATA_REG_0_INDIRECT_COMMAND_RD_DATA_ID:
      field = &(regs->fdt.indirect_command_rd_data_reg_0.indirect_command_rd_data);
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
  ui_fap21v_acc_fdt_transmit_data_cells_registers_9_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdt");
  soc_sand_proc_name = "ui_fap21v_acc_fdt_transmit_data_cells_registers_9_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_9_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_9_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_9_REG_REGISTER_ID:
      regis = &(regs->fdt.transmit_data_cells_registers_reg[9].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_9_REG_CPUDATA_CELL_ID:
      field = &(regs->fdt.transmit_data_cells_registers_reg[9].cpudata_cell);
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
  soc_sand_os_printf( "fdt.transmit_data_cells_registers_reg[9]: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fdt_transmit_data_cells_registers_9_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdt");
  soc_sand_proc_name = "ui_fap21v_acc_fdt_transmit_data_cells_registers_9_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_9_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_9_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_9_REG_REGISTER_ID:
      regis = &(regs->fdt.transmit_data_cells_registers_reg[9].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_9_REG_CPUDATA_CELL_ID:
      field = &(regs->fdt.transmit_data_cells_registers_reg[9].cpudata_cell);
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
  ui_fap21v_acc_fdt_indirect_command_address_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdt");
  soc_sand_proc_name = "ui_fap21v_acc_fdt_indirect_command_address_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_INDIRECT_COMMAND_ADDRESS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDT_INDIRECT_COMMAND_ADDRESS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDT_INDIRECT_COMMAND_ADDRESS_REG_REGISTER_ID:
      regis = &(regs->fdt.indirect_command_address_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_INDIRECT_COMMAND_ADDRESS_REG_INDIRECT_COMMAND_ADDRESS_ID:
      field = &(regs->fdt.indirect_command_address_reg.indirect_command_address);
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
  soc_sand_os_printf( "fdt.indirect_command_address_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fdt_indirect_command_address_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdt");
  soc_sand_proc_name = "ui_fap21v_acc_fdt_indirect_command_address_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_INDIRECT_COMMAND_ADDRESS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDT_INDIRECT_COMMAND_ADDRESS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDT_INDIRECT_COMMAND_ADDRESS_REG_REGISTER_ID:
      regis = &(regs->fdt.indirect_command_address_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_INDIRECT_COMMAND_ADDRESS_REG_INDIRECT_COMMAND_ADDRESS_ID:
      field = &(regs->fdt.indirect_command_address_reg.indirect_command_address);
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
  ui_fap21v_acc_fdt_local_route_cell_counter_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdt");
  soc_sand_proc_name = "ui_fap21v_acc_fdt_local_route_cell_counter_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_LOCAL_ROUTE_CELL_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDT_LOCAL_ROUTE_CELL_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDT_LOCAL_ROUTE_CELL_COUNTER_REG_REGISTER_ID:
      regis = &(regs->fdt.local_route_cell_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_LOCAL_ROUTE_CELL_COUNTER_REG_LOCAL_ROUTE_CELL_CNT_ID:
      field = &(regs->fdt.local_route_cell_counter_reg.local_route_cell_cnt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_LOCAL_ROUTE_CELL_COUNTER_REG_LOCAL_ROUTE_CELL_CNT_OVF_ID:
      field = &(regs->fdt.local_route_cell_counter_reg.local_route_cell_cnt_ovf);
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
  soc_sand_os_printf( "fdt.local_route_cell_counter_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fdt_local_route_cell_counter_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdt");
  soc_sand_proc_name = "ui_fap21v_acc_fdt_local_route_cell_counter_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_LOCAL_ROUTE_CELL_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDT_LOCAL_ROUTE_CELL_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDT_LOCAL_ROUTE_CELL_COUNTER_REG_REGISTER_ID:
      regis = &(regs->fdt.local_route_cell_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_LOCAL_ROUTE_CELL_COUNTER_REG_LOCAL_ROUTE_CELL_CNT_ID:
      field = &(regs->fdt.local_route_cell_counter_reg.local_route_cell_cnt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_LOCAL_ROUTE_CELL_COUNTER_REG_LOCAL_ROUTE_CELL_CNT_OVF_ID:
      field = &(regs->fdt.local_route_cell_counter_reg.local_route_cell_cnt_ovf);
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
  ui_fap21v_acc_fdt_transmit_data_cells_registers_4_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdt");
  soc_sand_proc_name = "ui_fap21v_acc_fdt_transmit_data_cells_registers_4_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_4_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_4_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_4_REG_REGISTER_ID:
      regis = &(regs->fdt.transmit_data_cells_registers_reg[4].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_4_REG_CPUDATA_CELL_ID:
      field = &(regs->fdt.transmit_data_cells_registers_reg[4].cpudata_cell);
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
  soc_sand_os_printf( "fdt.transmit_data_cells_registers_reg[4]: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fdt_transmit_data_cells_registers_4_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdt");
  soc_sand_proc_name = "ui_fap21v_acc_fdt_transmit_data_cells_registers_4_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_4_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_4_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_4_REG_REGISTER_ID:
      regis = &(regs->fdt.transmit_data_cells_registers_reg[4].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_4_REG_CPUDATA_CELL_ID:
      field = &(regs->fdt.transmit_data_cells_registers_reg[4].cpudata_cell);
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
  ui_fap21v_acc_fdt_transmit_data_cells_registers_6_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdt");
  soc_sand_proc_name = "ui_fap21v_acc_fdt_transmit_data_cells_registers_6_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_6_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_6_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_6_REG_REGISTER_ID:
      regis = &(regs->fdt.transmit_data_cells_registers_reg[6].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_6_REG_CPUDATA_CELL_ID:
      field = &(regs->fdt.transmit_data_cells_registers_reg[6].cpudata_cell);
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
  soc_sand_os_printf( "fdt.transmit_data_cells_registers_reg[6]: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fdt_transmit_data_cells_registers_6_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdt");
  soc_sand_proc_name = "ui_fap21v_acc_fdt_transmit_data_cells_registers_6_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_6_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_6_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_6_REG_REGISTER_ID:
      regis = &(regs->fdt.transmit_data_cells_registers_reg[6].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_6_REG_CPUDATA_CELL_ID:
      field = &(regs->fdt.transmit_data_cells_registers_reg[6].cpudata_cell);
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
  ui_fap21v_acc_fdt_transmit_data_cells_registers_8_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdt");
  soc_sand_proc_name = "ui_fap21v_acc_fdt_transmit_data_cells_registers_8_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_8_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_8_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_8_REG_REGISTER_ID:
      regis = &(regs->fdt.transmit_data_cells_registers_reg[8].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_8_REG_CPUDATA_CELL_ID:
      field = &(regs->fdt.transmit_data_cells_registers_reg[8].cpudata_cell);
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
  soc_sand_os_printf( "fdt.transmit_data_cells_registers_reg[8]: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fdt_transmit_data_cells_registers_8_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdt");
  soc_sand_proc_name = "ui_fap21v_acc_fdt_transmit_data_cells_registers_8_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_8_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_8_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_8_REG_REGISTER_ID:
      regis = &(regs->fdt.transmit_data_cells_registers_reg[8].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_8_REG_CPUDATA_CELL_ID:
      field = &(regs->fdt.transmit_data_cells_registers_reg[8].cpudata_cell);
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
  ui_fap21v_acc_fdt_transmitted_data_cells_counter_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdt");
  soc_sand_proc_name = "ui_fap21v_acc_fdt_transmitted_data_cells_counter_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMITTED_DATA_CELLS_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMITTED_DATA_CELLS_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMITTED_DATA_CELLS_COUNTER_REG_REGISTER_ID:
      regis = &(regs->fdt.transmitted_data_cells_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMITTED_DATA_CELLS_COUNTER_REG_DATA_CELL_CNT_ID:
      field = &(regs->fdt.transmitted_data_cells_counter_reg.data_cell_cnt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMITTED_DATA_CELLS_COUNTER_REG_DATA_CELL_CNT_O_ID:
      field = &(regs->fdt.transmitted_data_cells_counter_reg.data_cell_cnt_o);
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
  soc_sand_os_printf( "fdt.transmitted_data_cells_counter_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fdt_transmitted_data_cells_counter_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdt");
  soc_sand_proc_name = "ui_fap21v_acc_fdt_transmitted_data_cells_counter_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMITTED_DATA_CELLS_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMITTED_DATA_CELLS_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMITTED_DATA_CELLS_COUNTER_REG_REGISTER_ID:
      regis = &(regs->fdt.transmitted_data_cells_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMITTED_DATA_CELLS_COUNTER_REG_DATA_CELL_CNT_ID:
      field = &(regs->fdt.transmitted_data_cells_counter_reg.data_cell_cnt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMITTED_DATA_CELLS_COUNTER_REG_DATA_CELL_CNT_O_ID:
      field = &(regs->fdt.transmitted_data_cells_counter_reg.data_cell_cnt_o);
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
  ui_fap21v_acc_fdt_transmit_data_cells_registers_3_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdt");
  soc_sand_proc_name = "ui_fap21v_acc_fdt_transmit_data_cells_registers_3_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_3_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_3_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_3_REG_REGISTER_ID:
      regis = &(regs->fdt.transmit_data_cells_registers_reg[3].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_3_REG_CPUDATA_CELL_ID:
      field = &(regs->fdt.transmit_data_cells_registers_reg[3].cpudata_cell);
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
  soc_sand_os_printf( "fdt.transmit_data_cells_registers_reg[3]: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fdt_transmit_data_cells_registers_3_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdt");
  soc_sand_proc_name = "ui_fap21v_acc_fdt_transmit_data_cells_registers_3_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_3_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_3_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_3_REG_REGISTER_ID:
      regis = &(regs->fdt.transmit_data_cells_registers_reg[3].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_3_REG_CPUDATA_CELL_ID:
      field = &(regs->fdt.transmit_data_cells_registers_reg[3].cpudata_cell);
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
  ui_fap21v_acc_fdt_interrupt_source_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdt");
  soc_sand_proc_name = "ui_fap21v_acc_fdt_interrupt_source_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_INTERRUPT_SOURCE_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDT_INTERRUPT_SOURCE_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDT_INTERRUPT_SOURCE_REG_REGISTER_ID:
      regis = &(regs->fdt.interrupt_source_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_INTERRUPT_SOURCE_REG_UNRCH_DEST_EVT_ID:
      field = &(regs->fdt.interrupt_source_reg.unrch_dest_evt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_INTERRUPT_SOURCE_REG_UNIDUP_ID:
      field = &(regs->fdt.interrupt_source_reg.unidup);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_INTERRUPT_SOURCE_REG_ECC_ERROR_ID:
      field = &(regs->fdt.interrupt_source_reg.ecc_error);
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
  soc_sand_os_printf( "fdt.interrupt_source_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fdt_interrupt_source_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdt");
  soc_sand_proc_name = "ui_fap21v_acc_fdt_interrupt_source_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_INTERRUPT_SOURCE_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDT_INTERRUPT_SOURCE_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDT_INTERRUPT_SOURCE_REG_REGISTER_ID:
      regis = &(regs->fdt.interrupt_source_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_INTERRUPT_SOURCE_REG_UNRCH_DEST_EVT_ID:
      field = &(regs->fdt.interrupt_source_reg.unrch_dest_evt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_INTERRUPT_SOURCE_REG_UNIDUP_ID:
      field = &(regs->fdt.interrupt_source_reg.unidup);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_INTERRUPT_SOURCE_REG_ECC_ERROR_ID:
      field = &(regs->fdt.interrupt_source_reg.ecc_error);
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
  ui_fap21v_acc_fdt_indirect_command_wr_data_reg_0_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdt");
  soc_sand_proc_name = "ui_fap21v_acc_fdt_indirect_command_wr_data_reg_0_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_INDIRECT_COMMAND_WR_DATA_REG_0_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDT_INDIRECT_COMMAND_WR_DATA_REG_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDT_INDIRECT_COMMAND_WR_DATA_REG_0_REGISTER_ID:
      regis = &(regs->fdt.indirect_command_wr_data_reg_0.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_INDIRECT_COMMAND_WR_DATA_REG_0_INDIRECT_COMMAND_WR_DATA_ID:
      field = &(regs->fdt.indirect_command_wr_data_reg_0.indirect_command_wr_data);
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
  soc_sand_os_printf( "fdt.indirect_command_wr_data_reg_0: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fdt_indirect_command_wr_data_reg_0_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdt");
  soc_sand_proc_name = "ui_fap21v_acc_fdt_indirect_command_wr_data_reg_0_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_INDIRECT_COMMAND_WR_DATA_REG_0_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDT_INDIRECT_COMMAND_WR_DATA_REG_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDT_INDIRECT_COMMAND_WR_DATA_REG_0_REGISTER_ID:
      regis = &(regs->fdt.indirect_command_wr_data_reg_0.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_INDIRECT_COMMAND_WR_DATA_REG_0_INDIRECT_COMMAND_WR_DATA_ID:
      field = &(regs->fdt.indirect_command_wr_data_reg_0.indirect_command_wr_data);
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
  ui_fap21v_acc_fdt_transmit_data_cells_registers_1_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdt");
  soc_sand_proc_name = "ui_fap21v_acc_fdt_transmit_data_cells_registers_1_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_1_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_1_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_1_REG_REGISTER_ID:
      regis = &(regs->fdt.transmit_data_cells_registers_reg[1].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_1_REG_CPUDATA_CELL_ID:
      field = &(regs->fdt.transmit_data_cells_registers_reg[1].cpudata_cell);
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
  soc_sand_os_printf( "fdt.transmit_data_cells_registers_reg[1]: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fdt_transmit_data_cells_registers_1_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdt");
  soc_sand_proc_name = "ui_fap21v_acc_fdt_transmit_data_cells_registers_1_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_1_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_1_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_1_REG_REGISTER_ID:
      regis = &(regs->fdt.transmit_data_cells_registers_reg[1].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_1_REG_CPUDATA_CELL_ID:
      field = &(regs->fdt.transmit_data_cells_registers_reg[1].cpudata_cell);
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
  ui_fap21v_acc_fdt_transmit_data_cells_registers_7_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdt");
  soc_sand_proc_name = "ui_fap21v_acc_fdt_transmit_data_cells_registers_7_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_7_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_7_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_7_REG_REGISTER_ID:
      regis = &(regs->fdt.transmit_data_cells_registers_reg[7].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_7_REG_CPUDATA_CELL_ID:
      field = &(regs->fdt.transmit_data_cells_registers_reg[7].cpudata_cell);
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
  soc_sand_os_printf( "fdt.transmit_data_cells_registers_reg[7]: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fdt_transmit_data_cells_registers_7_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdt");
  soc_sand_proc_name = "ui_fap21v_acc_fdt_transmit_data_cells_registers_7_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_7_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_7_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_7_REG_REGISTER_ID:
      regis = &(regs->fdt.transmit_data_cells_registers_reg[7].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_7_REG_CPUDATA_CELL_ID:
      field = &(regs->fdt.transmit_data_cells_registers_reg[7].cpudata_cell);
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
  ui_fap21v_acc_fdt_unreachable_destination_discarded_cells_counter_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdt");
  soc_sand_proc_name = "ui_fap21v_acc_fdt_unreachable_destination_discarded_cells_counter_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_UNREACHABLE_DESTINATION_DISCARDED_CELLS_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDT_UNREACHABLE_DESTINATION_DISCARDED_CELLS_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDT_UNREACHABLE_DESTINATION_DISCARDED_CELLS_COUNTER_REG_REGISTER_ID:
      regis = &(regs->fdt.unreachable_destination_discarded_cells_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_UNREACHABLE_DESTINATION_DISCARDED_CELLS_COUNTER_REG_UNREACH_DEST_ID:
      field = &(regs->fdt.unreachable_destination_discarded_cells_counter_reg.unreach_dest);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_UNREACHABLE_DESTINATION_DISCARDED_CELLS_COUNTER_REG_UNRCH_DEST_CNT_ID:
      field = &(regs->fdt.unreachable_destination_discarded_cells_counter_reg.unrch_dest_cnt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_UNREACHABLE_DESTINATION_DISCARDED_CELLS_COUNTER_REG_UNRCH_DEST_CNT_OVF_ID:
      field = &(regs->fdt.unreachable_destination_discarded_cells_counter_reg.unrch_dest_cnt_ovf);
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
  soc_sand_os_printf( "fdt.unreachable_destination_discarded_cells_counter_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fdt_unreachable_destination_discarded_cells_counter_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdt");
  soc_sand_proc_name = "ui_fap21v_acc_fdt_unreachable_destination_discarded_cells_counter_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_UNREACHABLE_DESTINATION_DISCARDED_CELLS_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDT_UNREACHABLE_DESTINATION_DISCARDED_CELLS_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDT_UNREACHABLE_DESTINATION_DISCARDED_CELLS_COUNTER_REG_REGISTER_ID:
      regis = &(regs->fdt.unreachable_destination_discarded_cells_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_UNREACHABLE_DESTINATION_DISCARDED_CELLS_COUNTER_REG_UNREACH_DEST_ID:
      field = &(regs->fdt.unreachable_destination_discarded_cells_counter_reg.unreach_dest);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_UNREACHABLE_DESTINATION_DISCARDED_CELLS_COUNTER_REG_UNRCH_DEST_CNT_ID:
      field = &(regs->fdt.unreachable_destination_discarded_cells_counter_reg.unrch_dest_cnt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDT_UNREACHABLE_DESTINATION_DISCARDED_CELLS_COUNTER_REG_UNRCH_DEST_CNT_OVF_ID:
      field = &(regs->fdt.unreachable_destination_discarded_cells_counter_reg.unrch_dest_cnt_ovf);
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
  ui_fap21v_acc_fdt_get(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdt_get");

  if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_INTERRUPT_MASK_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdt_interrupt_mask_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_2_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdt_transmit_data_cells_registers_2_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_UNICAST_COEXIST_BITMAP_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdt_unicast_coexist_bitmap_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELL_TRIGGER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdt_transmit_data_cell_trigger_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_INDIRECT_COMMAND_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdt_indirect_command_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_CONTEXT_DEFINES_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdt_context_defines_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_0_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdt_transmit_data_cells_registers_0_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_CELL_OUTPUT_LINK_NUMBER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdt_transmit_cell_output_link_number_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_5_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdt_transmit_data_cells_registers_5_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_FDT_ENABLERS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdt_fdt_enablers_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_INDIRECT_COMMAND_RD_DATA_REG_0_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdt_indirect_command_rd_data_reg_0_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_9_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdt_transmit_data_cells_registers_9_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_INDIRECT_COMMAND_ADDRESS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdt_indirect_command_address_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_LOCAL_ROUTE_CELL_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdt_local_route_cell_counter_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_4_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdt_transmit_data_cells_registers_4_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_6_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdt_transmit_data_cells_registers_6_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_8_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdt_transmit_data_cells_registers_8_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMITTED_DATA_CELLS_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdt_transmitted_data_cells_counter_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_3_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdt_transmit_data_cells_registers_3_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_INTERRUPT_SOURCE_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdt_interrupt_source_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_INDIRECT_COMMAND_WR_DATA_REG_0_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdt_indirect_command_wr_data_reg_0_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_1_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdt_transmit_data_cells_registers_1_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_7_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdt_transmit_data_cells_registers_7_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_UNREACHABLE_DESTINATION_DISCARDED_CELLS_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdt_unreachable_destination_discarded_cells_counter_reg_get(current_line);
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
  ui_fap21v_acc_fdt_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdt_set");

  if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_INTERRUPT_MASK_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdt_interrupt_mask_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_2_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdt_transmit_data_cells_registers_2_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_UNICAST_COEXIST_BITMAP_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdt_unicast_coexist_bitmap_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELL_TRIGGER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdt_transmit_data_cell_trigger_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_INDIRECT_COMMAND_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdt_indirect_command_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_CONTEXT_DEFINES_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdt_context_defines_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_0_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdt_transmit_data_cells_registers_0_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_CELL_OUTPUT_LINK_NUMBER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdt_transmit_cell_output_link_number_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_5_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdt_transmit_data_cells_registers_5_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_FDT_ENABLERS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdt_fdt_enablers_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_INDIRECT_COMMAND_RD_DATA_REG_0_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdt_indirect_command_rd_data_reg_0_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_9_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdt_transmit_data_cells_registers_9_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_INDIRECT_COMMAND_ADDRESS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdt_indirect_command_address_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_LOCAL_ROUTE_CELL_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdt_local_route_cell_counter_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_4_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdt_transmit_data_cells_registers_4_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_6_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdt_transmit_data_cells_registers_6_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_8_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdt_transmit_data_cells_registers_8_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMITTED_DATA_CELLS_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdt_transmitted_data_cells_counter_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_3_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdt_transmit_data_cells_registers_3_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_INTERRUPT_SOURCE_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdt_interrupt_source_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_INDIRECT_COMMAND_WR_DATA_REG_0_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdt_indirect_command_wr_data_reg_0_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_1_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdt_transmit_data_cells_registers_1_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_TRANSMIT_DATA_CELLS_REGISTERS_7_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdt_transmit_data_cells_registers_7_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDT_UNREACHABLE_DESTINATION_DISCARDED_CELLS_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdt_unreachable_destination_discarded_cells_counter_reg_set(current_line, value);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after fdt***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}
