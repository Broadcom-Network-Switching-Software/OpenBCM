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
  ui_fap21v_acc_fct_transmitted_control_cells_counter_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fct");
  soc_sand_proc_name = "ui_fap21v_acc_fct_transmitted_control_cells_counter_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCT_TRANSMITTED_CONTROL_CELLS_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FCT_TRANSMITTED_CONTROL_CELLS_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FCT_TRANSMITTED_CONTROL_CELLS_COUNTER_REG_REGISTER_ID:
      regis = &(regs->fct.transmitted_control_cells_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCT_TRANSMITTED_CONTROL_CELLS_COUNTER_REG_CTRL_CELL_CNT_ID:
      field = &(regs->fct.transmitted_control_cells_counter_reg.ctrl_cell_cnt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCT_TRANSMITTED_CONTROL_CELLS_COUNTER_REG_CTRL_CELL_CNT_O_ID:
      field = &(regs->fct.transmitted_control_cells_counter_reg.ctrl_cell_cnt_o);
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
  soc_sand_os_printf( "fct.transmitted_control_cells_counter_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fct_transmitted_control_cells_counter_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fct");
  soc_sand_proc_name = "ui_fap21v_acc_fct_transmitted_control_cells_counter_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCT_TRANSMITTED_CONTROL_CELLS_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FCT_TRANSMITTED_CONTROL_CELLS_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FCT_TRANSMITTED_CONTROL_CELLS_COUNTER_REG_REGISTER_ID:
      regis = &(regs->fct.transmitted_control_cells_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCT_TRANSMITTED_CONTROL_CELLS_COUNTER_REG_CTRL_CELL_CNT_ID:
      field = &(regs->fct.transmitted_control_cells_counter_reg.ctrl_cell_cnt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCT_TRANSMITTED_CONTROL_CELLS_COUNTER_REG_CTRL_CELL_CNT_O_ID:
      field = &(regs->fct.transmitted_control_cells_counter_reg.ctrl_cell_cnt_o);
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
  ui_fap21v_acc_fct_fct_enablers_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fct");
  soc_sand_proc_name = "ui_fap21v_acc_fct_fct_enablers_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCT_FCT_ENABLERS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FCT_FCT_ENABLERS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FCT_FCT_ENABLERS_REG_REGISTER_ID:
      regis = &(regs->fct.fct_enablers_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCT_FCT_ENABLERS_REG_DIS_STS_ID:
      field = &(regs->fct.fct_enablers_reg.dis_sts);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCT_FCT_ENABLERS_REG_DIS_CRD_ID:
      field = &(regs->fct.fct_enablers_reg.dis_crd);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCT_FCT_ENABLERS_REG_DIS_RCH_ID:
      field = &(regs->fct.fct_enablers_reg.dis_rch);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCT_FCT_ENABLERS_REG_DIS_LCL_RT_ID:
      field = &(regs->fct.fct_enablers_reg.dis_lcl_rt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCT_FCT_ENABLERS_REG_FORCE_FSLCL_ID:
      field = &(regs->fct.fct_enablers_reg.force_fslcl);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCT_FCT_ENABLERS_REG_FORCE_CRLCL_ID:
      field = &(regs->fct.fct_enablers_reg.force_crlcl);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCT_FCT_ENABLERS_REG_EXT_PORT_MODE_ID:
      field = &(regs->fct.fct_enablers_reg.ext_port_mode);
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
  soc_sand_os_printf( "fct.fct_enablers_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fct_fct_enablers_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fct");
  soc_sand_proc_name = "ui_fap21v_acc_fct_fct_enablers_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCT_FCT_ENABLERS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FCT_FCT_ENABLERS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FCT_FCT_ENABLERS_REG_REGISTER_ID:
      regis = &(regs->fct.fct_enablers_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCT_FCT_ENABLERS_REG_DIS_STS_ID:
      field = &(regs->fct.fct_enablers_reg.dis_sts);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCT_FCT_ENABLERS_REG_DIS_CRD_ID:
      field = &(regs->fct.fct_enablers_reg.dis_crd);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCT_FCT_ENABLERS_REG_DIS_RCH_ID:
      field = &(regs->fct.fct_enablers_reg.dis_rch);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCT_FCT_ENABLERS_REG_DIS_LCL_RT_ID:
      field = &(regs->fct.fct_enablers_reg.dis_lcl_rt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCT_FCT_ENABLERS_REG_FORCE_FSLCL_ID:
      field = &(regs->fct.fct_enablers_reg.force_fslcl);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCT_FCT_ENABLERS_REG_FORCE_CRLCL_ID:
      field = &(regs->fct.fct_enablers_reg.force_crlcl);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCT_FCT_ENABLERS_REG_EXT_PORT_MODE_ID:
      field = &(regs->fct.fct_enablers_reg.ext_port_mode);
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
  ui_fap21v_acc_fct_cpu_transmit_cells_1_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fct");
  soc_sand_proc_name = "ui_fap21v_acc_fct_cpu_transmit_cells_1_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCT_CPU_TRANSMIT_CELLS_1_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FCT_CPU_TRANSMIT_CELLS_1_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FCT_CPU_TRANSMIT_CELLS_1_REG_REGISTER_ID:
      regis = &(regs->fct.cpu_transmit_cells_1_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCT_CPU_TRANSMIT_CELLS_1_REG_TX_CTR_CELL_BUFF1_ID:
      field = &(regs->fct.cpu_transmit_cells_1_reg.tx_ctr_cell_buff1);
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
  soc_sand_os_printf( "fct.cpu_transmit_cells_1_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fct_cpu_transmit_cells_1_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fct");
  soc_sand_proc_name = "ui_fap21v_acc_fct_cpu_transmit_cells_1_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCT_CPU_TRANSMIT_CELLS_1_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FCT_CPU_TRANSMIT_CELLS_1_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FCT_CPU_TRANSMIT_CELLS_1_REG_REGISTER_ID:
      regis = &(regs->fct.cpu_transmit_cells_1_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCT_CPU_TRANSMIT_CELLS_1_REG_TX_CTR_CELL_BUFF1_ID:
      field = &(regs->fct.cpu_transmit_cells_1_reg.tx_ctr_cell_buff1);
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
  ui_fap21v_acc_fct_cpu_transmit_cells_link_number_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fct");
  soc_sand_proc_name = "ui_fap21v_acc_fct_cpu_transmit_cells_link_number_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCT_CPU_TRANSMIT_CELLS_LINK_NUMBER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FCT_CPU_TRANSMIT_CELLS_LINK_NUMBER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FCT_CPU_TRANSMIT_CELLS_LINK_NUMBER_REG_REGISTER_ID:
      regis = &(regs->fct.cpu_transmit_cells_link_number_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCT_CPU_TRANSMIT_CELLS_LINK_NUMBER_REG_TX_CELL_OUT_LINK_ID:
      field = &(regs->fct.cpu_transmit_cells_link_number_reg.tx_cell_out_link);
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
  soc_sand_os_printf( "fct.cpu_transmit_cells_link_number_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fct_cpu_transmit_cells_link_number_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fct");
  soc_sand_proc_name = "ui_fap21v_acc_fct_cpu_transmit_cells_link_number_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCT_CPU_TRANSMIT_CELLS_LINK_NUMBER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FCT_CPU_TRANSMIT_CELLS_LINK_NUMBER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FCT_CPU_TRANSMIT_CELLS_LINK_NUMBER_REG_REGISTER_ID:
      regis = &(regs->fct.cpu_transmit_cells_link_number_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCT_CPU_TRANSMIT_CELLS_LINK_NUMBER_REG_TX_CELL_OUT_LINK_ID:
      field = &(regs->fct.cpu_transmit_cells_link_number_reg.tx_cell_out_link);
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
  ui_fap21v_acc_fct_unreachable_destination_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fct");
  soc_sand_proc_name = "ui_fap21v_acc_fct_unreachable_destination_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCT_UNREACHABLE_DESTINATION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FCT_UNREACHABLE_DESTINATION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FCT_UNREACHABLE_DESTINATION_REG_REGISTER_ID:
      regis = &(regs->fct.unreachable_destination_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCT_UNREACHABLE_DESTINATION_REG_UNRCH_DEST_ID:
      field = &(regs->fct.unreachable_destination_reg.unrch_dest);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCT_UNREACHABLE_DESTINATION_REG_UNRCH_DEST_EVT_ID:
      field = &(regs->fct.unreachable_destination_reg.unrch_dest_evt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCT_UNREACHABLE_DESTINATION_REG_UNRCH_CREDIT_CNT_ID:
      field = &(regs->fct.unreachable_destination_reg.unrch_credit_cnt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCT_UNREACHABLE_DESTINATION_REG_UNRCH_CREDIT_CNT_OVF_ID:
      field = &(regs->fct.unreachable_destination_reg.unrch_credit_cnt_ovf);
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
  soc_sand_os_printf( "fct.unreachable_destination_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fct_unreachable_destination_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fct");
  soc_sand_proc_name = "ui_fap21v_acc_fct_unreachable_destination_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCT_UNREACHABLE_DESTINATION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FCT_UNREACHABLE_DESTINATION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FCT_UNREACHABLE_DESTINATION_REG_REGISTER_ID:
      regis = &(regs->fct.unreachable_destination_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCT_UNREACHABLE_DESTINATION_REG_UNRCH_DEST_ID:
      field = &(regs->fct.unreachable_destination_reg.unrch_dest);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCT_UNREACHABLE_DESTINATION_REG_UNRCH_DEST_EVT_ID:
      field = &(regs->fct.unreachable_destination_reg.unrch_dest_evt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCT_UNREACHABLE_DESTINATION_REG_UNRCH_CREDIT_CNT_ID:
      field = &(regs->fct.unreachable_destination_reg.unrch_credit_cnt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCT_UNREACHABLE_DESTINATION_REG_UNRCH_CREDIT_CNT_OVF_ID:
      field = &(regs->fct.unreachable_destination_reg.unrch_credit_cnt_ovf);
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
  ui_fap21v_acc_fct_cpu_transmit_cells_0_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fct");
  soc_sand_proc_name = "ui_fap21v_acc_fct_cpu_transmit_cells_0_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCT_CPU_TRANSMIT_CELLS_0_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FCT_CPU_TRANSMIT_CELLS_0_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FCT_CPU_TRANSMIT_CELLS_0_REG_REGISTER_ID:
      regis = &(regs->fct.cpu_transmit_cells_0_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCT_CPU_TRANSMIT_CELLS_0_REG_TX_CTR_CELL_BUFF0_ID:
      field = &(regs->fct.cpu_transmit_cells_0_reg.tx_ctr_cell_buff0);
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
  soc_sand_os_printf( "fct.cpu_transmit_cells_0_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fct_cpu_transmit_cells_0_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fct");
  soc_sand_proc_name = "ui_fap21v_acc_fct_cpu_transmit_cells_0_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCT_CPU_TRANSMIT_CELLS_0_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FCT_CPU_TRANSMIT_CELLS_0_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FCT_CPU_TRANSMIT_CELLS_0_REG_REGISTER_ID:
      regis = &(regs->fct.cpu_transmit_cells_0_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCT_CPU_TRANSMIT_CELLS_0_REG_TX_CTR_CELL_BUFF0_ID:
      field = &(regs->fct.cpu_transmit_cells_0_reg.tx_ctr_cell_buff0);
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
  ui_fap21v_acc_fct_cpu_transmit_cells_2_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fct");
  soc_sand_proc_name = "ui_fap21v_acc_fct_cpu_transmit_cells_2_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCT_CPU_TRANSMIT_CELLS_2_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FCT_CPU_TRANSMIT_CELLS_2_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FCT_CPU_TRANSMIT_CELLS_2_REG_REGISTER_ID:
      regis = &(regs->fct.cpu_transmit_cells_2_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCT_CPU_TRANSMIT_CELLS_2_REG_TX_CTR_CELL_BUFF2_ID:
      field = &(regs->fct.cpu_transmit_cells_2_reg.tx_ctr_cell_buff2);
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
  soc_sand_os_printf( "fct.cpu_transmit_cells_2_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fct_cpu_transmit_cells_2_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fct");
  soc_sand_proc_name = "ui_fap21v_acc_fct_cpu_transmit_cells_2_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCT_CPU_TRANSMIT_CELLS_2_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FCT_CPU_TRANSMIT_CELLS_2_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FCT_CPU_TRANSMIT_CELLS_2_REG_REGISTER_ID:
      regis = &(regs->fct.cpu_transmit_cells_2_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCT_CPU_TRANSMIT_CELLS_2_REG_TX_CTR_CELL_BUFF2_ID:
      field = &(regs->fct.cpu_transmit_cells_2_reg.tx_ctr_cell_buff2);
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
  ui_fap21v_acc_fct_cpu_transmit_cells_trigger_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fct");
  soc_sand_proc_name = "ui_fap21v_acc_fct_cpu_transmit_cells_trigger_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCT_CPU_TRANSMIT_CELLS_TRIGGER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FCT_CPU_TRANSMIT_CELLS_TRIGGER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FCT_CPU_TRANSMIT_CELLS_TRIGGER_REG_REGISTER_ID:
      regis = &(regs->fct.cpu_transmit_cells_trigger_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCT_CPU_TRANSMIT_CELLS_TRIGGER_REG_TX_CNT_CELL_TRG_ID:
      field = &(regs->fct.cpu_transmit_cells_trigger_reg.tx_cnt_cell_trg);
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
  soc_sand_os_printf( "fct.cpu_transmit_cells_trigger_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fct_cpu_transmit_cells_trigger_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fct");
  soc_sand_proc_name = "ui_fap21v_acc_fct_cpu_transmit_cells_trigger_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCT_CPU_TRANSMIT_CELLS_TRIGGER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FCT_CPU_TRANSMIT_CELLS_TRIGGER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FCT_CPU_TRANSMIT_CELLS_TRIGGER_REG_REGISTER_ID:
      regis = &(regs->fct.cpu_transmit_cells_trigger_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCT_CPU_TRANSMIT_CELLS_TRIGGER_REG_TX_CNT_CELL_TRG_ID:
      field = &(regs->fct.cpu_transmit_cells_trigger_reg.tx_cnt_cell_trg);
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
  ui_fap21v_acc_fct_local_route_cell_counter_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fct");
  soc_sand_proc_name = "ui_fap21v_acc_fct_local_route_cell_counter_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCT_LOCAL_ROUTE_CELL_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FCT_LOCAL_ROUTE_CELL_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FCT_LOCAL_ROUTE_CELL_COUNTER_REG_REGISTER_ID:
      regis = &(regs->fct.local_route_cell_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCT_LOCAL_ROUTE_CELL_COUNTER_REG_LOCAL_ROUTE_CELL_CNT_ID:
      field = &(regs->fct.local_route_cell_counter_reg.local_route_cell_cnt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCT_LOCAL_ROUTE_CELL_COUNTER_REG_LOCAL_ROUTE_CELL_CNT_OVF_ID:
      field = &(regs->fct.local_route_cell_counter_reg.local_route_cell_cnt_ovf);
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
  soc_sand_os_printf( "fct.local_route_cell_counter_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fct_local_route_cell_counter_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fct");
  soc_sand_proc_name = "ui_fap21v_acc_fct_local_route_cell_counter_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCT_LOCAL_ROUTE_CELL_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_FCT_LOCAL_ROUTE_CELL_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_FCT_LOCAL_ROUTE_CELL_COUNTER_REG_REGISTER_ID:
      regis = &(regs->fct.local_route_cell_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCT_LOCAL_ROUTE_CELL_COUNTER_REG_LOCAL_ROUTE_CELL_CNT_ID:
      field = &(regs->fct.local_route_cell_counter_reg.local_route_cell_cnt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_FCT_LOCAL_ROUTE_CELL_COUNTER_REG_LOCAL_ROUTE_CELL_CNT_OVF_ID:
      field = &(regs->fct.local_route_cell_counter_reg.local_route_cell_cnt_ovf);
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
  ui_fap21v_acc_fct_get(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fct_get");

  if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCT_TRANSMITTED_CONTROL_CELLS_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fct_transmitted_control_cells_counter_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCT_FCT_ENABLERS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fct_fct_enablers_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCT_CPU_TRANSMIT_CELLS_1_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fct_cpu_transmit_cells_1_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCT_CPU_TRANSMIT_CELLS_LINK_NUMBER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fct_cpu_transmit_cells_link_number_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCT_UNREACHABLE_DESTINATION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fct_unreachable_destination_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCT_CPU_TRANSMIT_CELLS_0_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fct_cpu_transmit_cells_0_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCT_CPU_TRANSMIT_CELLS_2_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fct_cpu_transmit_cells_2_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCT_CPU_TRANSMIT_CELLS_TRIGGER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fct_cpu_transmit_cells_trigger_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCT_LOCAL_ROUTE_CELL_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fct_local_route_cell_counter_reg_get(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after fct***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_fct_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_fct_set");

  if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCT_TRANSMITTED_CONTROL_CELLS_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fct_transmitted_control_cells_counter_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCT_FCT_ENABLERS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fct_fct_enablers_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCT_CPU_TRANSMIT_CELLS_1_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fct_cpu_transmit_cells_1_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCT_CPU_TRANSMIT_CELLS_LINK_NUMBER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fct_cpu_transmit_cells_link_number_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCT_UNREACHABLE_DESTINATION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fct_unreachable_destination_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCT_CPU_TRANSMIT_CELLS_0_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fct_cpu_transmit_cells_0_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCT_CPU_TRANSMIT_CELLS_2_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fct_cpu_transmit_cells_2_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCT_CPU_TRANSMIT_CELLS_TRIGGER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fct_cpu_transmit_cells_trigger_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_FCT_LOCAL_ROUTE_CELL_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_fct_local_route_cell_counter_reg_set(current_line, value);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after fct***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}
