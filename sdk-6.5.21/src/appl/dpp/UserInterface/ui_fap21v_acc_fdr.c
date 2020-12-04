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
  ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_2_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdr");
  soc_sand_proc_name = "ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_2_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_2_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_2_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_2_REG_REGISTER_ID:
      regis = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_a_2_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_2_REG_CPUDATA_CELL_WORD_A0_A9_CPUDATA_CELL_WORD_B0_B9_ID:
      field = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_a_2_reg.cpudata_cell_word_a0_a9_cpudata_cell_word_b0_b9);
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
  soc_sand_os_printf( "fdr.receive_source_routed_data_cell_filtered_cells_side_a_2_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_2_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdr");
  soc_sand_proc_name = "ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_2_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_2_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_2_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_2_REG_REGISTER_ID:
      regis = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_a_2_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_2_REG_CPUDATA_CELL_WORD_A0_A9_CPUDATA_CELL_WORD_B0_B9_ID:
      field = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_a_2_reg.cpudata_cell_word_a0_a9_cpudata_cell_word_b0_b9);
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
  ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_4_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdr");
  soc_sand_proc_name = "ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_4_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_4_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_4_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_4_REG_REGISTER_ID:
      regis = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_a_4_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_4_REG_CPUDATA_CELL_WORD_A0_A9_CPUDATA_CELL_WORD_B0_B9_ID:
      field = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_a_4_reg.cpudata_cell_word_a0_a9_cpudata_cell_word_b0_b9);
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
  soc_sand_os_printf( "fdr.receive_source_routed_data_cell_filtered_cells_side_a_4_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_4_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdr");
  soc_sand_proc_name = "ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_4_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_4_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_4_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_4_REG_REGISTER_ID:
      regis = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_a_4_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_4_REG_CPUDATA_CELL_WORD_A0_A9_CPUDATA_CELL_WORD_B0_B9_ID:
      field = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_a_4_reg.cpudata_cell_word_a0_a9_cpudata_cell_word_b0_b9);
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
  ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_1_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdr");
  soc_sand_proc_name = "ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_1_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_1_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_1_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_1_REG_REGISTER_ID:
      regis = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_a_1_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_1_REG_CPUDATA_CELL_WORD_A0_A9_CPUDATA_CELL_WORD_B0_B9_ID:
      field = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_a_1_reg.cpudata_cell_word_a0_a9_cpudata_cell_word_b0_b9);
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
  soc_sand_os_printf( "fdr.receive_source_routed_data_cell_filtered_cells_side_a_1_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_1_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdr");
  soc_sand_proc_name = "ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_1_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_1_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_1_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_1_REG_REGISTER_ID:
      regis = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_a_1_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_1_REG_CPUDATA_CELL_WORD_A0_A9_CPUDATA_CELL_WORD_B0_B9_ID:
      field = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_a_1_reg.cpudata_cell_word_a0_a9_cpudata_cell_word_b0_b9);
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
  ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_0_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdr");
  soc_sand_proc_name = "ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_0_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_0_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_0_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_0_REG_REGISTER_ID:
      regis = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_b_0_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_0_REG_CPUDATA_CELL_WORD_A0_A9_CPUDATA_CELL_WORD_B0_B9_ID:
      field = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_b_0_reg.cpudata_cell_word_a0_a9_cpudata_cell_word_b0_b9);
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
  soc_sand_os_printf( "fdr.receive_source_routed_data_cell_filtered_cells_side_b_0_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_0_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdr");
  soc_sand_proc_name = "ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_0_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_0_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_0_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_0_REG_REGISTER_ID:
      regis = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_b_0_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_0_REG_CPUDATA_CELL_WORD_A0_A9_CPUDATA_CELL_WORD_B0_B9_ID:
      field = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_b_0_reg.cpudata_cell_word_a0_a9_cpudata_cell_word_b0_b9);
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
  ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_0_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdr");
  soc_sand_proc_name = "ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_0_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_0_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_0_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_0_REG_REGISTER_ID:
      regis = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_a_0_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_0_REG_CPUDATA_CELL_WORD_A0_A9_CPUDATA_CELL_WORD_B0_B9_ID:
      field = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_a_0_reg.cpudata_cell_word_a0_a9_cpudata_cell_word_b0_b9);
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
  soc_sand_os_printf( "fdr.receive_source_routed_data_cell_filtered_cells_side_a_0_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_0_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdr");
  soc_sand_proc_name = "ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_0_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_0_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_0_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_0_REG_REGISTER_ID:
      regis = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_a_0_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_0_REG_CPUDATA_CELL_WORD_A0_A9_CPUDATA_CELL_WORD_B0_B9_ID:
      field = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_a_0_reg.cpudata_cell_word_a0_a9_cpudata_cell_word_b0_b9);
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
  ui_fap21v_acc_fdr_interrupt_mask_reg_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    buffer,
    field_val = 0;
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdr");
  soc_sand_proc_name = "ui_fap21v_acc_fdr_interrupt_mask_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_INTERRUPT_MASK_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDR_INTERRUPT_MASK_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
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
  soc_sand_os_printf( "fdr.interrupt_mask_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fdr_interrupt_mask_reg_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  uint32
    ret;
  uint32
    field_val = 0;
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdr");
  soc_sand_proc_name = "ui_fap21v_acc_fdr_interrupt_mask_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_INTERRUPT_MASK_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDR_INTERRUPT_MASK_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
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
  ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_8_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdr");
  soc_sand_proc_name = "ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_8_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_8_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_8_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_8_REG_REGISTER_ID:
      regis = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_b_8_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_8_REG_CPUDATA_CELL_WORD_A0_A9_CPUDATA_CELL_WORD_B0_B9_ID:
      field = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_b_8_reg.cpudata_cell_word_a0_a9_cpudata_cell_word_b0_b9);
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
  soc_sand_os_printf( "fdr.receive_source_routed_data_cell_filtered_cells_side_b_8_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_8_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdr");
  soc_sand_proc_name = "ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_8_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_8_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_8_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_8_REG_REGISTER_ID:
      regis = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_b_8_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_8_REG_CPUDATA_CELL_WORD_A0_A9_CPUDATA_CELL_WORD_B0_B9_ID:
      field = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_b_8_reg.cpudata_cell_word_a0_a9_cpudata_cell_word_b0_b9);
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
  ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_8_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdr");
  soc_sand_proc_name = "ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_8_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_8_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_8_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_8_REG_REGISTER_ID:
      regis = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_a_8_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_8_REG_CPUDATA_CELL_WORD_A0_A9_CPUDATA_CELL_WORD_B0_B9_ID:
      field = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_a_8_reg.cpudata_cell_word_a0_a9_cpudata_cell_word_b0_b9);
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
  soc_sand_os_printf( "fdr.receive_source_routed_data_cell_filtered_cells_side_a_8_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_8_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdr");
  soc_sand_proc_name = "ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_8_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_8_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_8_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_8_REG_REGISTER_ID:
      regis = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_a_8_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_8_REG_CPUDATA_CELL_WORD_A0_A9_CPUDATA_CELL_WORD_B0_B9_ID:
      field = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_a_8_reg.cpudata_cell_word_a0_a9_cpudata_cell_word_b0_b9);
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
  ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_7_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdr");
  soc_sand_proc_name = "ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_7_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_7_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_7_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_7_REG_REGISTER_ID:
      regis = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_b_7_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_7_REG_CPUDATA_CELL_WORD_A0_A9_CPUDATA_CELL_WORD_B0_B9_ID:
      field = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_b_7_reg.cpudata_cell_word_a0_a9_cpudata_cell_word_b0_b9);
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
  soc_sand_os_printf( "fdr.receive_source_routed_data_cell_filtered_cells_side_b_7_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_7_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdr");
  soc_sand_proc_name = "ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_7_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_7_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_7_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_7_REG_REGISTER_ID:
      regis = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_b_7_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_7_REG_CPUDATA_CELL_WORD_A0_A9_CPUDATA_CELL_WORD_B0_B9_ID:
      field = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_b_7_reg.cpudata_cell_word_a0_a9_cpudata_cell_word_b0_b9);
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
  ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_6_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdr");
  soc_sand_proc_name = "ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_6_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_6_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_6_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_6_REG_REGISTER_ID:
      regis = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_b_6_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_6_REG_CPUDATA_CELL_WORD_A0_A9_CPUDATA_CELL_WORD_B0_B9_ID:
      field = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_b_6_reg.cpudata_cell_word_a0_a9_cpudata_cell_word_b0_b9);
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
  soc_sand_os_printf( "fdr.receive_source_routed_data_cell_filtered_cells_side_b_6_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_6_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdr");
  soc_sand_proc_name = "ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_6_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_6_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_6_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_6_REG_REGISTER_ID:
      regis = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_b_6_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_6_REG_CPUDATA_CELL_WORD_A0_A9_CPUDATA_CELL_WORD_B0_B9_ID:
      field = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_b_6_reg.cpudata_cell_word_a0_a9_cpudata_cell_word_b0_b9);
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
  ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_9_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdr");
  soc_sand_proc_name = "ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_9_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_9_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_9_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_9_REG_REGISTER_ID:
      regis = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_b_9_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_9_REG_CPUDATA_CELL_WORD_A0_A9_CPUDATA_CELL_WORD_B0_B9_ID:
      field = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_b_9_reg.cpudata_cell_word_a0_a9_cpudata_cell_word_b0_b9);
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
  soc_sand_os_printf( "fdr.receive_source_routed_data_cell_filtered_cells_side_b_9_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_9_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdr");
  soc_sand_proc_name = "ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_9_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_9_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_9_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_9_REG_REGISTER_ID:
      regis = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_b_9_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_9_REG_CPUDATA_CELL_WORD_A0_A9_CPUDATA_CELL_WORD_B0_B9_ID:
      field = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_b_9_reg.cpudata_cell_word_a0_a9_cpudata_cell_word_b0_b9);
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
  ui_fap21v_acc_fdr_programmable_cells_counter_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdr");
  soc_sand_proc_name = "ui_fap21v_acc_fdr_programmable_cells_counter_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_PROGRAMMABLE_CELLS_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDR_PROGRAMMABLE_CELLS_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDR_PROGRAMMABLE_CELLS_COUNTER_REG_REGISTER_ID:
      regis = &(regs->fdr.programmable_cells_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_PROGRAMMABLE_CELLS_COUNTER_REG_PRG_CELL_CNT_ID:
      field = &(regs->fdr.programmable_cells_counter_reg.prg_cell_cnt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_PROGRAMMABLE_CELLS_COUNTER_REG_PRG_CELL_CNT_O_ID:
      field = &(regs->fdr.programmable_cells_counter_reg.prg_cell_cnt_o);
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
  soc_sand_os_printf( "fdr.programmable_cells_counter_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fdr_programmable_cells_counter_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdr");
  soc_sand_proc_name = "ui_fap21v_acc_fdr_programmable_cells_counter_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_PROGRAMMABLE_CELLS_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDR_PROGRAMMABLE_CELLS_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDR_PROGRAMMABLE_CELLS_COUNTER_REG_REGISTER_ID:
      regis = &(regs->fdr.programmable_cells_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_PROGRAMMABLE_CELLS_COUNTER_REG_PRG_CELL_CNT_ID:
      field = &(regs->fdr.programmable_cells_counter_reg.prg_cell_cnt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_PROGRAMMABLE_CELLS_COUNTER_REG_PRG_CELL_CNT_O_ID:
      field = &(regs->fdr.programmable_cells_counter_reg.prg_cell_cnt_o);
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
  ui_fap21v_acc_fdr_filter_match_input_link_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdr");
  soc_sand_proc_name = "ui_fap21v_acc_fdr_filter_match_input_link_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_FILTER_MATCH_INPUT_LINK_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDR_FILTER_MATCH_INPUT_LINK_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDR_FILTER_MATCH_INPUT_LINK_REG_REGISTER_ID:
      regis = &(regs->fdr.filter_match_input_link_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_FILTER_MATCH_INPUT_LINK_REG_PC_MTCH_LINK_ID:
      field = &(regs->fdr.filter_match_input_link_reg.pc_mtch_link);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_FILTER_MATCH_INPUT_LINK_REG_PC_MTCH_LINK_ON_ID:
      field = &(regs->fdr.filter_match_input_link_reg.pc_mtch_link_on);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_FILTER_MATCH_INPUT_LINK_REG_CRC_MTCH_ON_ID:
      field = &(regs->fdr.filter_match_input_link_reg.crc_mtch_on);
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
  soc_sand_os_printf( "fdr.filter_match_input_link_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fdr_filter_match_input_link_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdr");
  soc_sand_proc_name = "ui_fap21v_acc_fdr_filter_match_input_link_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_FILTER_MATCH_INPUT_LINK_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDR_FILTER_MATCH_INPUT_LINK_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDR_FILTER_MATCH_INPUT_LINK_REG_REGISTER_ID:
      regis = &(regs->fdr.filter_match_input_link_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_FILTER_MATCH_INPUT_LINK_REG_PC_MTCH_LINK_ID:
      field = &(regs->fdr.filter_match_input_link_reg.pc_mtch_link);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_FILTER_MATCH_INPUT_LINK_REG_PC_MTCH_LINK_ON_ID:
      field = &(regs->fdr.filter_match_input_link_reg.pc_mtch_link_on);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_FILTER_MATCH_INPUT_LINK_REG_CRC_MTCH_ON_ID:
      field = &(regs->fdr.filter_match_input_link_reg.crc_mtch_on);
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
  ui_fap21v_acc_fdr_programmable_data_cell_counter_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdr");
  soc_sand_proc_name = "ui_fap21v_acc_fdr_programmable_data_cell_counter_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_PROGRAMMABLE_DATA_CELL_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDR_PROGRAMMABLE_DATA_CELL_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDR_PROGRAMMABLE_DATA_CELL_COUNTER_REG_REGISTER_ID:
      regis = &(regs->fdr.programmable_data_cell_counter_reg[0].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_PROGRAMMABLE_DATA_CELL_COUNTER_REG_PCMTCH_MSK1_ID:
      field = &(regs->fdr.programmable_data_cell_counter_reg[0].pcmtch_msk1);
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
  soc_sand_os_printf( "fdr.programmable_data_cell_counter_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fdr_programmable_data_cell_counter_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdr");
  soc_sand_proc_name = "ui_fap21v_acc_fdr_programmable_data_cell_counter_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_PROGRAMMABLE_DATA_CELL_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDR_PROGRAMMABLE_DATA_CELL_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDR_PROGRAMMABLE_DATA_CELL_COUNTER_REG_REGISTER_ID:
      regis = &(regs->fdr.programmable_data_cell_counter_reg[0].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_PROGRAMMABLE_DATA_CELL_COUNTER_REG_PCMTCH_MSK1_ID:
      field = &(regs->fdr.programmable_data_cell_counter_reg[0].pcmtch_msk1);
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
  ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_3_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdr");
  soc_sand_proc_name = "ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_3_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_3_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_3_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_3_REG_REGISTER_ID:
      regis = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_a_3_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_3_REG_CPUDATA_CELL_WORD_A0_A9_CPUDATA_CELL_WORD_B0_B9_ID:
      field = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_a_3_reg.cpudata_cell_word_a0_a9_cpudata_cell_word_b0_b9);
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
  soc_sand_os_printf( "fdr.receive_source_routed_data_cell_filtered_cells_side_a_3_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_3_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdr");
  soc_sand_proc_name = "ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_3_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_3_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_3_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_3_REG_REGISTER_ID:
      regis = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_a_3_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_3_REG_CPUDATA_CELL_WORD_A0_A9_CPUDATA_CELL_WORD_B0_B9_ID:
      field = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_a_3_reg.cpudata_cell_word_a0_a9_cpudata_cell_word_b0_b9);
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
  ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_2_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdr");
  soc_sand_proc_name = "ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_2_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_2_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_2_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_2_REG_REGISTER_ID:
      regis = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_b_2_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_2_REG_CPUDATA_CELL_WORD_A0_A9_CPUDATA_CELL_WORD_B0_B9_ID:
      field = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_b_2_reg.cpudata_cell_word_a0_a9_cpudata_cell_word_b0_b9);
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
  soc_sand_os_printf( "fdr.receive_source_routed_data_cell_filtered_cells_side_b_2_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_2_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdr");
  soc_sand_proc_name = "ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_2_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_2_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_2_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_2_REG_REGISTER_ID:
      regis = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_b_2_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_2_REG_CPUDATA_CELL_WORD_A0_A9_CPUDATA_CELL_WORD_B0_B9_ID:
      field = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_b_2_reg.cpudata_cell_word_a0_a9_cpudata_cell_word_b0_b9);
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
  ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_5_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdr");
  soc_sand_proc_name = "ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_5_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_5_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_5_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_5_REG_REGISTER_ID:
      regis = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_a_5_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_5_REG_CPUDATA_CELL_WORD_A0_A9_CPUDATA_CELL_WORD_B0_B9_ID:
      field = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_a_5_reg.cpudata_cell_word_a0_a9_cpudata_cell_word_b0_b9);
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
  soc_sand_os_printf( "fdr.receive_source_routed_data_cell_filtered_cells_side_a_5_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_5_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdr");
  soc_sand_proc_name = "ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_5_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_5_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_5_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_5_REG_REGISTER_ID:
      regis = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_a_5_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_5_REG_CPUDATA_CELL_WORD_A0_A9_CPUDATA_CELL_WORD_B0_B9_ID:
      field = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_a_5_reg.cpudata_cell_word_a0_a9_cpudata_cell_word_b0_b9);
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
  ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_4_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdr");
  soc_sand_proc_name = "ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_4_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_4_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_4_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_4_REG_REGISTER_ID:
      regis = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_b_4_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_4_REG_CPUDATA_CELL_WORD_A0_A9_CPUDATA_CELL_WORD_B0_B9_ID:
      field = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_b_4_reg.cpudata_cell_word_a0_a9_cpudata_cell_word_b0_b9);
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
  soc_sand_os_printf( "fdr.receive_source_routed_data_cell_filtered_cells_side_b_4_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_4_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdr");
  soc_sand_proc_name = "ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_4_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_4_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_4_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_4_REG_REGISTER_ID:
      regis = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_b_4_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_4_REG_CPUDATA_CELL_WORD_A0_A9_CPUDATA_CELL_WORD_B0_B9_ID:
      field = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_b_4_reg.cpudata_cell_word_a0_a9_cpudata_cell_word_b0_b9);
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
  ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_1_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdr");
  soc_sand_proc_name = "ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_1_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_1_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_1_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_1_REG_REGISTER_ID:
      regis = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_b_1_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_1_REG_CPUDATA_CELL_WORD_A0_A9_CPUDATA_CELL_WORD_B0_B9_ID:
      field = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_b_1_reg.cpudata_cell_word_a0_a9_cpudata_cell_word_b0_b9);
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
  soc_sand_os_printf( "fdr.receive_source_routed_data_cell_filtered_cells_side_b_1_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_1_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdr");
  soc_sand_proc_name = "ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_1_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_1_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_1_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_1_REG_REGISTER_ID:
      regis = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_b_1_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_1_REG_CPUDATA_CELL_WORD_A0_A9_CPUDATA_CELL_WORD_B0_B9_ID:
      field = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_b_1_reg.cpudata_cell_word_a0_a9_cpudata_cell_word_b0_b9);
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
  ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_3_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdr");
  soc_sand_proc_name = "ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_3_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_3_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_3_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_3_REG_REGISTER_ID:
      regis = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_b_3_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_3_REG_CPUDATA_CELL_WORD_A0_A9_CPUDATA_CELL_WORD_B0_B9_ID:
      field = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_b_3_reg.cpudata_cell_word_a0_a9_cpudata_cell_word_b0_b9);
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
  soc_sand_os_printf( "fdr.receive_source_routed_data_cell_filtered_cells_side_b_3_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_3_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdr");
  soc_sand_proc_name = "ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_3_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_3_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_3_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_3_REG_REGISTER_ID:
      regis = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_b_3_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_3_REG_CPUDATA_CELL_WORD_A0_A9_CPUDATA_CELL_WORD_B0_B9_ID:
      field = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_b_3_reg.cpudata_cell_word_a0_a9_cpudata_cell_word_b0_b9);
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
  ui_fap21v_acc_fdr_fdr_overflows_and_fifos_statuses_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdr");
  soc_sand_proc_name = "ui_fap21v_acc_fdr_fdr_overflows_and_fifos_statuses_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_FDR_OVERFLOWS_AND_FIFOS_STATUSES_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDR_FDR_OVERFLOWS_AND_FIFOS_STATUSES_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDR_FDR_OVERFLOWS_AND_FIFOS_STATUSES_REG_REGISTER_ID:
      regis = &(regs->fdr.fdr_overflows_and_fifos_statuses_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_FDR_OVERFLOWS_AND_FIFOS_STATUSES_REG_CPUDATA_CELL_FO_A_ID:
      field = &(regs->fdr.fdr_overflows_and_fifos_statuses_reg.cpudata_cell_fo_a);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_FDR_OVERFLOWS_AND_FIFOS_STATUSES_REG_CPUDATA_CELL_FO_B_ID:
      field = &(regs->fdr.fdr_overflows_and_fifos_statuses_reg.cpudata_cell_fo_b);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_FDR_OVERFLOWS_AND_FIFOS_STATUSES_REG_MAX_IFMAF_ID:
      field = &(regs->fdr.fdr_overflows_and_fifos_statuses_reg.max_ifmaf);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_FDR_OVERFLOWS_AND_FIFOS_STATUSES_REG_MAX_IFMBF_ID:
      field = &(regs->fdr.fdr_overflows_and_fifos_statuses_reg.max_ifmbf);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_FDR_OVERFLOWS_AND_FIFOS_STATUSES_REG_MAX_IFMF_NUM_ID:
      field = &(regs->fdr.fdr_overflows_and_fifos_statuses_reg.max_ifmf_num);
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
  soc_sand_os_printf( "fdr.fdr_overflows_and_fifos_statuses_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fdr_fdr_overflows_and_fifos_statuses_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdr");
  soc_sand_proc_name = "ui_fap21v_acc_fdr_fdr_overflows_and_fifos_statuses_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_FDR_OVERFLOWS_AND_FIFOS_STATUSES_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDR_FDR_OVERFLOWS_AND_FIFOS_STATUSES_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDR_FDR_OVERFLOWS_AND_FIFOS_STATUSES_REG_REGISTER_ID:
      regis = &(regs->fdr.fdr_overflows_and_fifos_statuses_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_FDR_OVERFLOWS_AND_FIFOS_STATUSES_REG_CPUDATA_CELL_FO_A_ID:
      field = &(regs->fdr.fdr_overflows_and_fifos_statuses_reg.cpudata_cell_fo_a);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_FDR_OVERFLOWS_AND_FIFOS_STATUSES_REG_CPUDATA_CELL_FO_B_ID:
      field = &(regs->fdr.fdr_overflows_and_fifos_statuses_reg.cpudata_cell_fo_b);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_FDR_OVERFLOWS_AND_FIFOS_STATUSES_REG_MAX_IFMAF_ID:
      field = &(regs->fdr.fdr_overflows_and_fifos_statuses_reg.max_ifmaf);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_FDR_OVERFLOWS_AND_FIFOS_STATUSES_REG_MAX_IFMBF_ID:
      field = &(regs->fdr.fdr_overflows_and_fifos_statuses_reg.max_ifmbf);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_FDR_OVERFLOWS_AND_FIFOS_STATUSES_REG_MAX_IFMF_NUM_ID:
      field = &(regs->fdr.fdr_overflows_and_fifos_statuses_reg.max_ifmf_num);
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
  ui_fap21v_acc_fdr_interrupt_sources_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdr");
  soc_sand_proc_name = "ui_fap21v_acc_fdr_interrupt_sources_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_INTERRUPT_SOURCES_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDR_INTERRUPT_SOURCES_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDR_INTERRUPT_SOURCES_REG_REGISTER_ID:
      regis = &(regs->fdr.interrupt_sources_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_INTERRUPT_SOURCES_REG_IFMAFO_ID:
      field = &(regs->fdr.interrupt_sources_reg.ifmafo);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_INTERRUPT_SOURCES_REG_IFMBFO_ID:
      field = &(regs->fdr.interrupt_sources_reg.ifmbfo);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_INTERRUPT_SOURCES_REG_CPUDATA_CELL_FNE_A_ID:
      field = &(regs->fdr.interrupt_sources_reg.cpudata_cell_fne_a);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_INTERRUPT_SOURCES_REG_CPUDATA_CELL_FNE_B_ID:
      field = &(regs->fdr.interrupt_sources_reg.cpudata_cell_fne_b);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_INTERRUPT_SOURCES_REG_IFMA_LOW_ECC_ERROR_ID:
      field = &(regs->fdr.interrupt_sources_reg.ifma_low_ecc_error);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_INTERRUPT_SOURCES_REG_IFMA_HIGH_ECC_ERROR_ID:
      field = &(regs->fdr.interrupt_sources_reg.ifma_high_ecc_error);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_INTERRUPT_SOURCES_REG_IFMB_LOW_ECC_ERROR_ID:
      field = &(regs->fdr.interrupt_sources_reg.ifmb_low_ecc_error);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_INTERRUPT_SOURCES_REG_IFMB_HIGH_ECC_ERROR_ID:
      field = &(regs->fdr.interrupt_sources_reg.ifmb_high_ecc_error);
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
  soc_sand_os_printf( "fdr.interrupt_sources_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fdr_interrupt_sources_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdr");
  soc_sand_proc_name = "ui_fap21v_acc_fdr_interrupt_sources_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_INTERRUPT_SOURCES_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDR_INTERRUPT_SOURCES_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDR_INTERRUPT_SOURCES_REG_REGISTER_ID:
      regis = &(regs->fdr.interrupt_sources_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_INTERRUPT_SOURCES_REG_IFMAFO_ID:
      field = &(regs->fdr.interrupt_sources_reg.ifmafo);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_INTERRUPT_SOURCES_REG_IFMBFO_ID:
      field = &(regs->fdr.interrupt_sources_reg.ifmbfo);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_INTERRUPT_SOURCES_REG_CPUDATA_CELL_FNE_A_ID:
      field = &(regs->fdr.interrupt_sources_reg.cpudata_cell_fne_a);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_INTERRUPT_SOURCES_REG_CPUDATA_CELL_FNE_B_ID:
      field = &(regs->fdr.interrupt_sources_reg.cpudata_cell_fne_b);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_INTERRUPT_SOURCES_REG_IFMA_LOW_ECC_ERROR_ID:
      field = &(regs->fdr.interrupt_sources_reg.ifma_low_ecc_error);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_INTERRUPT_SOURCES_REG_IFMA_HIGH_ECC_ERROR_ID:
      field = &(regs->fdr.interrupt_sources_reg.ifma_high_ecc_error);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_INTERRUPT_SOURCES_REG_IFMB_LOW_ECC_ERROR_ID:
      field = &(regs->fdr.interrupt_sources_reg.ifmb_low_ecc_error);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_INTERRUPT_SOURCES_REG_IFMB_HIGH_ECC_ERROR_ID:
      field = &(regs->fdr.interrupt_sources_reg.ifmb_high_ecc_error);
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
  ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_7_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdr");
  soc_sand_proc_name = "ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_7_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_7_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_7_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_7_REG_REGISTER_ID:
      regis = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_a_7_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_7_REG_CPUDATA_CELL_WORD_A0_A9_CPUDATA_CELL_WORD_B0_B9_ID:
      field = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_a_7_reg.cpudata_cell_word_a0_a9_cpudata_cell_word_b0_b9);
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
  soc_sand_os_printf( "fdr.receive_source_routed_data_cell_filtered_cells_side_a_7_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_7_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdr");
  soc_sand_proc_name = "ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_7_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_7_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_7_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_7_REG_REGISTER_ID:
      regis = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_a_7_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_7_REG_CPUDATA_CELL_WORD_A0_A9_CPUDATA_CELL_WORD_B0_B9_ID:
      field = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_a_7_reg.cpudata_cell_word_a0_a9_cpudata_cell_word_b0_b9);
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
  ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_6_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdr");
  soc_sand_proc_name = "ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_6_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_6_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_6_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_6_REG_REGISTER_ID:
      regis = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_a_6_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_6_REG_CPUDATA_CELL_WORD_A0_A9_CPUDATA_CELL_WORD_B0_B9_ID:
      field = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_a_6_reg.cpudata_cell_word_a0_a9_cpudata_cell_word_b0_b9);
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
  soc_sand_os_printf( "fdr.receive_source_routed_data_cell_filtered_cells_side_a_6_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_6_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdr");
  soc_sand_proc_name = "ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_6_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_6_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_6_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_6_REG_REGISTER_ID:
      regis = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_a_6_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_6_REG_CPUDATA_CELL_WORD_A0_A9_CPUDATA_CELL_WORD_B0_B9_ID:
      field = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_a_6_reg.cpudata_cell_word_a0_a9_cpudata_cell_word_b0_b9);
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
  ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_9_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdr");
  soc_sand_proc_name = "ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_9_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_9_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_9_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_9_REG_REGISTER_ID:
      regis = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_a_9_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_9_REG_CPUDATA_CELL_WORD_A0_A9_CPUDATA_CELL_WORD_B0_B9_ID:
      field = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_a_9_reg.cpudata_cell_word_a0_a9_cpudata_cell_word_b0_b9);
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
  soc_sand_os_printf( "fdr.receive_source_routed_data_cell_filtered_cells_side_a_9_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_9_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdr");
  soc_sand_proc_name = "ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_9_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_9_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_9_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_9_REG_REGISTER_ID:
      regis = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_a_9_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_9_REG_CPUDATA_CELL_WORD_A0_A9_CPUDATA_CELL_WORD_B0_B9_ID:
      field = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_a_9_reg.cpudata_cell_word_a0_a9_cpudata_cell_word_b0_b9);
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
  ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_5_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdr");
  soc_sand_proc_name = "ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_5_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_5_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_5_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_5_REG_REGISTER_ID:
      regis = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_b_5_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_5_REG_CPUDATA_CELL_WORD_A0_A9_CPUDATA_CELL_WORD_B0_B9_ID:
      field = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_b_5_reg.cpudata_cell_word_a0_a9_cpudata_cell_word_b0_b9);
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
  soc_sand_os_printf( "fdr.receive_source_routed_data_cell_filtered_cells_side_b_5_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_5_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdr");
  soc_sand_proc_name = "ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_5_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_5_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_5_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_5_REG_REGISTER_ID:
      regis = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_b_5_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_5_REG_CPUDATA_CELL_WORD_A0_A9_CPUDATA_CELL_WORD_B0_B9_ID:
      field = &(regs->fdr.receive_source_routed_data_cell_filtered_cells_side_b_5_reg.cpudata_cell_word_a0_a9_cpudata_cell_word_b0_b9);
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
  ui_fap21v_acc_fdr_get(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdr_get");

  if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_2_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_2_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_4_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_4_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_1_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_1_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_0_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_0_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_0_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_0_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_INTERRUPT_MASK_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdr_interrupt_mask_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_8_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_8_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_8_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_8_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_7_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_7_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_6_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_6_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_9_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_9_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_PROGRAMMABLE_CELLS_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdr_programmable_cells_counter_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_FILTER_MATCH_INPUT_LINK_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdr_filter_match_input_link_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_PROGRAMMABLE_DATA_CELL_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdr_programmable_data_cell_counter_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_3_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_3_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_2_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_2_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_5_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_5_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_4_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_4_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_1_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_1_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_3_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_3_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_FDR_OVERFLOWS_AND_FIFOS_STATUSES_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdr_fdr_overflows_and_fifos_statuses_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_INTERRUPT_SOURCES_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdr_interrupt_sources_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_7_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_7_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_6_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_6_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_9_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_9_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_5_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_5_reg_get(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after fdr***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fdr_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fdr_set");

  if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_2_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_2_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_4_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_4_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_1_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_1_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_0_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_0_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_0_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_0_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_INTERRUPT_MASK_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdr_interrupt_mask_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_8_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_8_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_8_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_8_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_7_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_7_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_6_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_6_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_9_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_9_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_PROGRAMMABLE_CELLS_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdr_programmable_cells_counter_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_FILTER_MATCH_INPUT_LINK_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdr_filter_match_input_link_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_PROGRAMMABLE_DATA_CELL_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdr_programmable_data_cell_counter_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_3_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_3_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_2_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_2_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_5_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_5_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_4_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_4_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_1_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_1_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_3_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_3_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_FDR_OVERFLOWS_AND_FIFOS_STATUSES_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdr_fdr_overflows_and_fifos_statuses_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_INTERRUPT_SOURCES_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdr_interrupt_sources_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_7_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_7_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_6_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_6_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_A_9_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_a_9_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FDR_RECEIVE_SOURCE_ROUTED_DATA_CELL_FILTERED_CELLS_SIDE_B_5_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fdr_receive_source_routed_data_cell_filtered_cells_side_b_5_reg_set(current_line, value);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after fdr***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}
