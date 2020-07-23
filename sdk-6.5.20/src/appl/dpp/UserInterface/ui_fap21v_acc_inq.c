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
  ui_fap21v_acc_inq_indirect_command_wr_data_reg_2_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_indirect_command_wr_data_reg_2_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_2_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_2_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_2_REGISTER_ID:
      regis = &(regs->inq.indirect_command_wr_data_reg_2.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_2_INDIRECT_COMMAND_WR_DATA_ID:
      field = &(regs->inq.indirect_command_wr_data_reg_2.indirect_command_wr_data);
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
  soc_sand_os_printf( "inq.indirect_command_wr_data_reg_2: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_inq_indirect_command_wr_data_reg_2_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_indirect_command_wr_data_reg_2_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_2_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_2_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_2_REGISTER_ID:
      regis = &(regs->inq.indirect_command_wr_data_reg_2.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_2_INDIRECT_COMMAND_WR_DATA_ID:
      field = &(regs->inq.indirect_command_wr_data_reg_2.indirect_command_wr_data);
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
  ui_fap21v_acc_inq_ipb_overflow_header_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_ipb_overflow_header_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_IPB_OVERFLOW_HEADER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_IPB_OVERFLOW_HEADER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_IPB_OVERFLOW_HEADER_REG_REGISTER_ID:
      regis = &(regs->inq.ipb_overflow_header_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_IPB_OVERFLOW_HEADER_REG_IPB_OVF_HEADER_ID:
      field = &(regs->inq.ipb_overflow_header_reg.ipb_ovf_header);
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
  soc_sand_os_printf( "inq.ipb_overflow_header_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_inq_ipb_overflow_header_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_ipb_overflow_header_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_IPB_OVERFLOW_HEADER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_IPB_OVERFLOW_HEADER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_IPB_OVERFLOW_HEADER_REG_REGISTER_ID:
      regis = &(regs->inq.ipb_overflow_header_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_IPB_OVERFLOW_HEADER_REG_IPB_OVF_HEADER_ID:
      field = &(regs->inq.ipb_overflow_header_reg.ipb_ovf_header);
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
  ui_fap21v_acc_inq_indirect_command_rd_data_reg_1_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_indirect_command_rd_data_reg_1_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_1_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_1_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_1_REGISTER_ID:
      regis = &(regs->inq.indirect_command_rd_data_reg_1.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_1_INDIRECT_COMMAND_RD_DATA_ID:
      field = &(regs->inq.indirect_command_rd_data_reg_1.indirect_command_rd_data);
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
  soc_sand_os_printf( "inq.indirect_command_rd_data_reg_1: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_inq_indirect_command_rd_data_reg_1_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_indirect_command_rd_data_reg_1_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_1_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_1_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_1_REGISTER_ID:
      regis = &(regs->inq.indirect_command_rd_data_reg_1.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_1_INDIRECT_COMMAND_RD_DATA_ID:
      field = &(regs->inq.indirect_command_rd_data_reg_1.indirect_command_rd_data);
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
  ui_fap21v_acc_inq_indirect_command_wr_data_reg_4_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_indirect_command_wr_data_reg_4_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_4_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_4_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_4_REGISTER_ID:
      regis = &(regs->inq.indirect_command_wr_data_reg_4.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_4_INDIRECT_COMMAND_WR_DATA_ID:
      field = &(regs->inq.indirect_command_wr_data_reg_4.indirect_command_wr_data);
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
  soc_sand_os_printf( "inq.indirect_command_wr_data_reg_4: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_inq_indirect_command_wr_data_reg_4_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_indirect_command_wr_data_reg_4_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_4_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_4_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_4_REGISTER_ID:
      regis = &(regs->inq.indirect_command_wr_data_reg_4.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_4_INDIRECT_COMMAND_WR_DATA_ID:
      field = &(regs->inq.indirect_command_wr_data_reg_4.indirect_command_wr_data);
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
  ui_fap21v_acc_inq_lbpth_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_lbpth_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_LBPTH_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_LBPTH_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_LBPTH_REG_REGISTER_ID:
      regis = &(regs->inq.lbpth_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_LBPTH_REG_LBP_TH_A_ID:
      field = &(regs->inq.lbpth_reg.lbp_th_a);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_LBPTH_REG_LBP_TH_B_ID:
      field = &(regs->inq.lbpth_reg.lbp_th_b);
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
  soc_sand_os_printf( "inq.lbpth_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_inq_lbpth_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_lbpth_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_LBPTH_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_LBPTH_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_LBPTH_REG_REGISTER_ID:
      regis = &(regs->inq.lbpth_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_LBPTH_REG_LBP_TH_A_ID:
      field = &(regs->inq.lbpth_reg.lbp_th_a);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_LBPTH_REG_LBP_TH_B_ID:
      field = &(regs->inq.lbpth_reg.lbp_th_b);
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
  ui_fap21v_acc_inq_indirect_command_wr_data_reg_1_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_indirect_command_wr_data_reg_1_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_1_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_1_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_1_REGISTER_ID:
      regis = &(regs->inq.indirect_command_wr_data_reg_1.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_1_INDIRECT_COMMAND_WR_DATA_ID:
      field = &(regs->inq.indirect_command_wr_data_reg_1.indirect_command_wr_data);
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
  soc_sand_os_printf( "inq.indirect_command_wr_data_reg_1: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_inq_indirect_command_wr_data_reg_1_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_indirect_command_wr_data_reg_1_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_1_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_1_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_1_REGISTER_ID:
      regis = &(regs->inq.indirect_command_wr_data_reg_1.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_1_INDIRECT_COMMAND_WR_DATA_ID:
      field = &(regs->inq.indirect_command_wr_data_reg_1.indirect_command_wr_data);
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
  ui_fap21v_acc_inq_inq_fifo_status_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_inq_fifo_status_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INQ_FIFO_STATUS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_INQ_FIFO_STATUS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_INQ_FIFO_STATUS_REG_REGISTER_ID:
      regis = &(regs->inq.inq_fifo_status_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INQ_FIFO_STATUS_REG_SPI4_ASTAT_ID:
      field = &(regs->inq.inq_fifo_status_reg.spi4_astat);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INQ_FIFO_STATUS_REG_SPI4_BSTAT_ID:
      field = &(regs->inq.inq_fifo_status_reg.spi4_bstat);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INQ_FIFO_STATUS_REG_LBPSTAT_ID:
      field = &(regs->inq.inq_fifo_status_reg.lbpstat);
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
  soc_sand_os_printf( "inq.inq_fifo_status_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_inq_inq_fifo_status_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_inq_fifo_status_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INQ_FIFO_STATUS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_INQ_FIFO_STATUS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_INQ_FIFO_STATUS_REG_REGISTER_ID:
      regis = &(regs->inq.inq_fifo_status_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INQ_FIFO_STATUS_REG_SPI4_ASTAT_ID:
      field = &(regs->inq.inq_fifo_status_reg.spi4_astat);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INQ_FIFO_STATUS_REG_SPI4_BSTAT_ID:
      field = &(regs->inq.inq_fifo_status_reg.spi4_bstat);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INQ_FIFO_STATUS_REG_LBPSTAT_ID:
      field = &(regs->inq.inq_fifo_status_reg.lbpstat);
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
  ui_fap21v_acc_inq_ipa_overflow_header_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_ipa_overflow_header_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_IPA_OVERFLOW_HEADER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_IPA_OVERFLOW_HEADER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_IPA_OVERFLOW_HEADER_REG_REGISTER_ID:
      regis = &(regs->inq.ipa_overflow_header_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_IPA_OVERFLOW_HEADER_REG_IPA_OVF_HEADER_ID:
      field = &(regs->inq.ipa_overflow_header_reg.ipa_ovf_header);
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
  soc_sand_os_printf( "inq.ipa_overflow_header_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_inq_ipa_overflow_header_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_ipa_overflow_header_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_IPA_OVERFLOW_HEADER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_IPA_OVERFLOW_HEADER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_IPA_OVERFLOW_HEADER_REG_REGISTER_ID:
      regis = &(regs->inq.ipa_overflow_header_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_IPA_OVERFLOW_HEADER_REG_IPA_OVF_HEADER_ID:
      field = &(regs->inq.ipa_overflow_header_reg.ipa_ovf_header);
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
  ui_fap21v_acc_inq_spi4_frst_th_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_spi4_frst_th_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_SPI4_FRST_TH_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_SPI4_FRST_TH_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_SPI4_FRST_TH_REG_REGISTER_ID:
      regis = &(regs->inq.spi4_frst_th_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_SPI4_FRST_TH_REG_FRST_THRSH_SPI4_A_ID:
      field = &(regs->inq.spi4_frst_th_reg.frst_thrsh_spi4_a);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_SPI4_FRST_TH_REG_FRST_THRSH_SPI4_B_ID:
      field = &(regs->inq.spi4_frst_th_reg.frst_thrsh_spi4_b);
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
  soc_sand_os_printf( "inq.spi4_frst_th_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_inq_spi4_frst_th_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_spi4_frst_th_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_SPI4_FRST_TH_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_SPI4_FRST_TH_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_SPI4_FRST_TH_REG_REGISTER_ID:
      regis = &(regs->inq.spi4_frst_th_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_SPI4_FRST_TH_REG_FRST_THRSH_SPI4_A_ID:
      field = &(regs->inq.spi4_frst_th_reg.frst_thrsh_spi4_a);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_SPI4_FRST_TH_REG_FRST_THRSH_SPI4_B_ID:
      field = &(regs->inq.spi4_frst_th_reg.frst_thrsh_spi4_b);
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
  ui_fap21v_acc_inq_tdmb_packet_counters_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_tdmb_packet_counters_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_TDMB_PACKET_COUNTERS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_TDMB_PACKET_COUNTERS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_TDMB_PACKET_COUNTERS_REG_REGISTER_ID:
      regis = &(regs->inq.tdmb_packet_counters_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_TDMB_PACKET_COUNTERS_REG_TDMBCNT_ID:
      field = &(regs->inq.tdmb_packet_counters_reg.tdmbcnt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_TDMB_PACKET_COUNTERS_REG_TDMBCNT_OVF_ID:
      field = &(regs->inq.tdmb_packet_counters_reg.tdmbcnt_ovf);
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
  soc_sand_os_printf( "inq.tdmb_packet_counters_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_inq_tdmb_packet_counters_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_tdmb_packet_counters_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_TDMB_PACKET_COUNTERS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_TDMB_PACKET_COUNTERS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_TDMB_PACKET_COUNTERS_REG_REGISTER_ID:
      regis = &(regs->inq.tdmb_packet_counters_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_TDMB_PACKET_COUNTERS_REG_TDMBCNT_ID:
      field = &(regs->inq.tdmb_packet_counters_reg.tdmbcnt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_TDMB_PACKET_COUNTERS_REG_TDMBCNT_OVF_ID:
      field = &(regs->inq.tdmb_packet_counters_reg.tdmbcnt_ovf);
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
  ui_fap21v_acc_inq_spi4_scnd_th_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_spi4_scnd_th_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_SPI4_SCND_TH_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_SPI4_SCND_TH_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_SPI4_SCND_TH_REG_REGISTER_ID:
      regis = &(regs->inq.spi4_scnd_th_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_SPI4_SCND_TH_REG_SCND_THRSH_SPI4_A_ID:
      field = &(regs->inq.spi4_scnd_th_reg.scnd_thrsh_spi4_a);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_SPI4_SCND_TH_REG_SCND_THRSH_SPI4_B_ID:
      field = &(regs->inq.spi4_scnd_th_reg.scnd_thrsh_spi4_b);
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
  soc_sand_os_printf( "inq.spi4_scnd_th_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_inq_spi4_scnd_th_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_spi4_scnd_th_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_SPI4_SCND_TH_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_SPI4_SCND_TH_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_SPI4_SCND_TH_REG_REGISTER_ID:
      regis = &(regs->inq.spi4_scnd_th_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_SPI4_SCND_TH_REG_SCND_THRSH_SPI4_A_ID:
      field = &(regs->inq.spi4_scnd_th_reg.scnd_thrsh_spi4_a);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_SPI4_SCND_TH_REG_SCND_THRSH_SPI4_B_ID:
      field = &(regs->inq.spi4_scnd_th_reg.scnd_thrsh_spi4_b);
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
  ui_fap21v_acc_inq_indirect_command_rd_data_reg_3_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_indirect_command_rd_data_reg_3_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_3_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_3_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_3_REGISTER_ID:
      regis = &(regs->inq.indirect_command_rd_data_reg_3.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_3_INDIRECT_COMMAND_RD_DATA_ID:
      field = &(regs->inq.indirect_command_rd_data_reg_3.indirect_command_rd_data);
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
  soc_sand_os_printf( "inq.indirect_command_rd_data_reg_3: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_inq_indirect_command_rd_data_reg_3_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_indirect_command_rd_data_reg_3_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_3_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_3_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_3_REGISTER_ID:
      regis = &(regs->inq.indirect_command_rd_data_reg_3.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_3_INDIRECT_COMMAND_RD_DATA_ID:
      field = &(regs->inq.indirect_command_rd_data_reg_3.indirect_command_rd_data);
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
  ui_fap21v_acc_inq_masks_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_masks_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_MASKS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_MASKS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_MASKS_REG_REGISTER_ID:
      regis = &(regs->inq.masks_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_MASKS_REG_IPA_OVF_MASK_ID:
      field = &(regs->inq.masks_reg.ipa_ovf_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_MASKS_REG_IPB_OVF_MASK_ID:
      field = &(regs->inq.masks_reg.ipb_ovf_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_MASKS_REG_TDMA_OVF_MASK_ID:
      field = &(regs->inq.masks_reg.tdma_ovf_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_MASKS_REG_TDMB_OVF_MASK_ID:
      field = &(regs->inq.masks_reg.tdmb_ovf_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_MASKS_REG_IPA_MSB_ECC_ERR_MASK_ID:
      field = &(regs->inq.masks_reg.ipa_msb_ecc_err_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_MASKS_REG_IPA_LSB_ECC_ERR_MASK_ID:
      field = &(regs->inq.masks_reg.ipa_lsb_ecc_err_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_MASKS_REG_IPB_MSB_ECC_ERR_MASK_ID:
      field = &(regs->inq.masks_reg.ipb_msb_ecc_err_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_MASKS_REG_IPB_LSB_ECC_ERR_MASK_ID:
      field = &(regs->inq.masks_reg.ipb_lsb_ecc_err_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_MASKS_REG_TDMACOUNT_OVF_MASK_ID:
      field = &(regs->inq.masks_reg.tdmacount_ovf_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_MASKS_REG_TDMBCOUNT_OVF_MASK_ID:
      field = &(regs->inq.masks_reg.tdmbcount_ovf_mask);
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
  soc_sand_os_printf( "inq.masks_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_inq_masks_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_masks_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_MASKS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_MASKS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_MASKS_REG_REGISTER_ID:
      regis = &(regs->inq.masks_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_MASKS_REG_IPA_OVF_MASK_ID:
      field = &(regs->inq.masks_reg.ipa_ovf_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_MASKS_REG_IPB_OVF_MASK_ID:
      field = &(regs->inq.masks_reg.ipb_ovf_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_MASKS_REG_TDMA_OVF_MASK_ID:
      field = &(regs->inq.masks_reg.tdma_ovf_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_MASKS_REG_TDMB_OVF_MASK_ID:
      field = &(regs->inq.masks_reg.tdmb_ovf_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_MASKS_REG_IPA_MSB_ECC_ERR_MASK_ID:
      field = &(regs->inq.masks_reg.ipa_msb_ecc_err_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_MASKS_REG_IPA_LSB_ECC_ERR_MASK_ID:
      field = &(regs->inq.masks_reg.ipa_lsb_ecc_err_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_MASKS_REG_IPB_MSB_ECC_ERR_MASK_ID:
      field = &(regs->inq.masks_reg.ipb_msb_ecc_err_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_MASKS_REG_IPB_LSB_ECC_ERR_MASK_ID:
      field = &(regs->inq.masks_reg.ipb_lsb_ecc_err_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_MASKS_REG_TDMACOUNT_OVF_MASK_ID:
      field = &(regs->inq.masks_reg.tdmacount_ovf_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_MASKS_REG_TDMBCOUNT_OVF_MASK_ID:
      field = &(regs->inq.masks_reg.tdmbcount_ovf_mask);
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
  ui_fap21v_acc_inq_indirect_command_wr_data_reg_3_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_indirect_command_wr_data_reg_3_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_3_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_3_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_3_REGISTER_ID:
      regis = &(regs->inq.indirect_command_wr_data_reg_3.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_3_INDIRECT_COMMAND_WR_DATA_ID:
      field = &(regs->inq.indirect_command_wr_data_reg_3.indirect_command_wr_data);
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
  soc_sand_os_printf( "inq.indirect_command_wr_data_reg_3: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_inq_indirect_command_wr_data_reg_3_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_indirect_command_wr_data_reg_3_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_3_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_3_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_3_REGISTER_ID:
      regis = &(regs->inq.indirect_command_wr_data_reg_3.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_3_INDIRECT_COMMAND_WR_DATA_ID:
      field = &(regs->inq.indirect_command_wr_data_reg_3.indirect_command_wr_data);
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
  ui_fap21v_acc_inq_tdm_fifos_memory_locks_for_data_flow_read_and_or_write_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_tdm_fifos_memory_locks_for_data_flow_read_and_or_write_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_TDM_FIFOS_MEMORY_LOCKS_FOR_DATA_FLOW_READ_AND_OR_WRITE_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_TDM_FIFOS_MEMORY_LOCKS_FOR_DATA_FLOW_READ_AND_OR_WRITE_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_TDM_FIFOS_MEMORY_LOCKS_FOR_DATA_FLOW_READ_AND_OR_WRITE_REG_REGISTER_ID:
      regis = &(regs->inq.tdm_fifos_memory_locks_for_data_flow_read_and_or_write_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_TDM_FIFOS_MEMORY_LOCKS_FOR_DATA_FLOW_READ_AND_OR_WRITE_REG_LOCK_TDMARD_ID:
      field = &(regs->inq.tdm_fifos_memory_locks_for_data_flow_read_and_or_write_reg.lock_tdmard);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_TDM_FIFOS_MEMORY_LOCKS_FOR_DATA_FLOW_READ_AND_OR_WRITE_REG_LOCK_TDMBRD_ID:
      field = &(regs->inq.tdm_fifos_memory_locks_for_data_flow_read_and_or_write_reg.lock_tdmbrd);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_TDM_FIFOS_MEMORY_LOCKS_FOR_DATA_FLOW_READ_AND_OR_WRITE_REG_LOCK_TDMAWR_ID:
      field = &(regs->inq.tdm_fifos_memory_locks_for_data_flow_read_and_or_write_reg.lock_tdmawr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_TDM_FIFOS_MEMORY_LOCKS_FOR_DATA_FLOW_READ_AND_OR_WRITE_REG_LOCK_TDMBWR_ID:
      field = &(regs->inq.tdm_fifos_memory_locks_for_data_flow_read_and_or_write_reg.lock_tdmbwr);
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
  soc_sand_os_printf( "inq.tdm_fifos_memory_locks_for_data_flow_read_and_or_write_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_inq_tdm_fifos_memory_locks_for_data_flow_read_and_or_write_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_tdm_fifos_memory_locks_for_data_flow_read_and_or_write_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_TDM_FIFOS_MEMORY_LOCKS_FOR_DATA_FLOW_READ_AND_OR_WRITE_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_TDM_FIFOS_MEMORY_LOCKS_FOR_DATA_FLOW_READ_AND_OR_WRITE_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_TDM_FIFOS_MEMORY_LOCKS_FOR_DATA_FLOW_READ_AND_OR_WRITE_REG_REGISTER_ID:
      regis = &(regs->inq.tdm_fifos_memory_locks_for_data_flow_read_and_or_write_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_TDM_FIFOS_MEMORY_LOCKS_FOR_DATA_FLOW_READ_AND_OR_WRITE_REG_LOCK_TDMARD_ID:
      field = &(regs->inq.tdm_fifos_memory_locks_for_data_flow_read_and_or_write_reg.lock_tdmard);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_TDM_FIFOS_MEMORY_LOCKS_FOR_DATA_FLOW_READ_AND_OR_WRITE_REG_LOCK_TDMBRD_ID:
      field = &(regs->inq.tdm_fifos_memory_locks_for_data_flow_read_and_or_write_reg.lock_tdmbrd);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_TDM_FIFOS_MEMORY_LOCKS_FOR_DATA_FLOW_READ_AND_OR_WRITE_REG_LOCK_TDMAWR_ID:
      field = &(regs->inq.tdm_fifos_memory_locks_for_data_flow_read_and_or_write_reg.lock_tdmawr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_TDM_FIFOS_MEMORY_LOCKS_FOR_DATA_FLOW_READ_AND_OR_WRITE_REG_LOCK_TDMBWR_ID:
      field = &(regs->inq.tdm_fifos_memory_locks_for_data_flow_read_and_or_write_reg.lock_tdmbwr);
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
  ui_fap21v_acc_inq_indirect_command_address_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_indirect_command_address_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_ADDRESS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_ADDRESS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_ADDRESS_REG_REGISTER_ID:
      regis = &(regs->inq.indirect_command_address_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_ADDRESS_REG_INDIRECT_COMMAND_ADDRESS_ID:
      field = &(regs->inq.indirect_command_address_reg.indirect_command_address);
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
  soc_sand_os_printf( "inq.indirect_command_address_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_inq_indirect_command_address_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_indirect_command_address_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_ADDRESS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_ADDRESS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_ADDRESS_REG_REGISTER_ID:
      regis = &(regs->inq.indirect_command_address_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_ADDRESS_REG_INDIRECT_COMMAND_ADDRESS_ID:
      field = &(regs->inq.indirect_command_address_reg.indirect_command_address);
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
  ui_fap21v_acc_inq_indirect_command_rd_data_reg_0_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_indirect_command_rd_data_reg_0_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_0_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_0_REGISTER_ID:
      regis = &(regs->inq.indirect_command_rd_data_reg_0.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_0_INDIRECT_COMMAND_RD_DATA_ID:
      field = &(regs->inq.indirect_command_rd_data_reg_0.indirect_command_rd_data);
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
  soc_sand_os_printf( "inq.indirect_command_rd_data_reg_0: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_inq_indirect_command_rd_data_reg_0_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_indirect_command_rd_data_reg_0_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_0_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_0_REGISTER_ID:
      regis = &(regs->inq.indirect_command_rd_data_reg_0.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_0_INDIRECT_COMMAND_RD_DATA_ID:
      field = &(regs->inq.indirect_command_rd_data_reg_0.indirect_command_rd_data);
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
  ui_fap21v_acc_inq_indirect_command_rd_data_reg_5_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_indirect_command_rd_data_reg_5_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_5_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_5_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_5_REGISTER_ID:
      regis = &(regs->inq.indirect_command_rd_data_reg_5.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_5_INDIRECT_COMMAND_RD_DATA_ID:
      field = &(regs->inq.indirect_command_rd_data_reg_5.indirect_command_rd_data);
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
  soc_sand_os_printf( "inq.indirect_command_rd_data_reg_5: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_inq_indirect_command_rd_data_reg_5_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_indirect_command_rd_data_reg_5_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_5_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_5_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_5_REGISTER_ID:
      regis = &(regs->inq.indirect_command_rd_data_reg_5.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_5_INDIRECT_COMMAND_RD_DATA_ID:
      field = &(regs->inq.indirect_command_rd_data_reg_5.indirect_command_rd_data);
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
  ui_fap21v_acc_inq_indirect_command_wr_data_reg_6_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_indirect_command_wr_data_reg_6_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_6_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_6_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_6_REGISTER_ID:
      regis = &(regs->inq.indirect_command_wr_data_reg_6.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_6_INDIRECT_COMMAND_WR_DATA_ID:
      field = &(regs->inq.indirect_command_wr_data_reg_6.indirect_command_wr_data);
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
  soc_sand_os_printf( "inq.indirect_command_wr_data_reg_6: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_inq_indirect_command_wr_data_reg_6_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_indirect_command_wr_data_reg_6_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_6_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_6_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_6_REGISTER_ID:
      regis = &(regs->inq.indirect_command_wr_data_reg_6.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_6_INDIRECT_COMMAND_WR_DATA_ID:
      field = &(regs->inq.indirect_command_wr_data_reg_6.indirect_command_wr_data);
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
  ui_fap21v_acc_inq_tdma_packet_counters_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_tdma_packet_counters_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_TDMA_PACKET_COUNTERS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_TDMA_PACKET_COUNTERS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_TDMA_PACKET_COUNTERS_REG_REGISTER_ID:
      regis = &(regs->inq.tdma_packet_counters_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_TDMA_PACKET_COUNTERS_REG_TDMACNT_ID:
      field = &(regs->inq.tdma_packet_counters_reg.tdmacnt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_TDMA_PACKET_COUNTERS_REG_TDMACNT_OVF_ID:
      field = &(regs->inq.tdma_packet_counters_reg.tdmacnt_ovf);
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
  soc_sand_os_printf( "inq.tdma_packet_counters_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_inq_tdma_packet_counters_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_tdma_packet_counters_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_TDMA_PACKET_COUNTERS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_TDMA_PACKET_COUNTERS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_TDMA_PACKET_COUNTERS_REG_REGISTER_ID:
      regis = &(regs->inq.tdma_packet_counters_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_TDMA_PACKET_COUNTERS_REG_TDMACNT_ID:
      field = &(regs->inq.tdma_packet_counters_reg.tdmacnt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_TDMA_PACKET_COUNTERS_REG_TDMACNT_OVF_ID:
      field = &(regs->inq.tdma_packet_counters_reg.tdmacnt_ovf);
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
  ui_fap21v_acc_inq_indirect_command_rd_data_reg_4_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_indirect_command_rd_data_reg_4_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_4_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_4_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_4_REGISTER_ID:
      regis = &(regs->inq.indirect_command_rd_data_reg_4.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_4_INDIRECT_COMMAND_RD_DATA_ID:
      field = &(regs->inq.indirect_command_rd_data_reg_4.indirect_command_rd_data);
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
  soc_sand_os_printf( "inq.indirect_command_rd_data_reg_4: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_inq_indirect_command_rd_data_reg_4_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_indirect_command_rd_data_reg_4_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_4_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_4_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_4_REGISTER_ID:
      regis = &(regs->inq.indirect_command_rd_data_reg_4.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_4_INDIRECT_COMMAND_RD_DATA_ID:
      field = &(regs->inq.indirect_command_rd_data_reg_4.indirect_command_rd_data);
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
  ui_fap21v_acc_inq_indirect_command_wr_data_reg_0_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_indirect_command_wr_data_reg_0_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_0_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_0_REGISTER_ID:
      regis = &(regs->inq.indirect_command_wr_data_reg_0.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_0_INDIRECT_COMMAND_WR_DATA_ID:
      field = &(regs->inq.indirect_command_wr_data_reg_0.indirect_command_wr_data);
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
  soc_sand_os_printf( "inq.indirect_command_wr_data_reg_0: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_inq_indirect_command_wr_data_reg_0_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_indirect_command_wr_data_reg_0_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_0_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_0_REGISTER_ID:
      regis = &(regs->inq.indirect_command_wr_data_reg_0.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_0_INDIRECT_COMMAND_WR_DATA_ID:
      field = &(regs->inq.indirect_command_wr_data_reg_0.indirect_command_wr_data);
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
  ui_fap21v_acc_inq_indirect_command_wr_data_reg_5_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_indirect_command_wr_data_reg_5_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_5_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_5_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_5_REGISTER_ID:
      regis = &(regs->inq.indirect_command_wr_data_reg_5.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_5_INDIRECT_COMMAND_WR_DATA_ID:
      field = &(regs->inq.indirect_command_wr_data_reg_5.indirect_command_wr_data);
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
  soc_sand_os_printf( "inq.indirect_command_wr_data_reg_5: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_inq_indirect_command_wr_data_reg_5_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_indirect_command_wr_data_reg_5_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_5_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_5_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_5_REGISTER_ID:
      regis = &(regs->inq.indirect_command_wr_data_reg_5.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_5_INDIRECT_COMMAND_WR_DATA_ID:
      field = &(regs->inq.indirect_command_wr_data_reg_5.indirect_command_wr_data);
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
  ui_fap21v_acc_inq_indirect_command_rd_data_reg_2_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_indirect_command_rd_data_reg_2_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_2_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_2_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_2_REGISTER_ID:
      regis = &(regs->inq.indirect_command_rd_data_reg_2.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_2_INDIRECT_COMMAND_RD_DATA_ID:
      field = &(regs->inq.indirect_command_rd_data_reg_2.indirect_command_rd_data);
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
  soc_sand_os_printf( "inq.indirect_command_rd_data_reg_2: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_inq_indirect_command_rd_data_reg_2_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_indirect_command_rd_data_reg_2_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_2_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_2_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_2_REGISTER_ID:
      regis = &(regs->inq.indirect_command_rd_data_reg_2.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_2_INDIRECT_COMMAND_RD_DATA_ID:
      field = &(regs->inq.indirect_command_rd_data_reg_2.indirect_command_rd_data);
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
  ui_fap21v_acc_inq_ip_fifos_memory_locks_for_data_flow_read_and_or_write_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_ip_fifos_memory_locks_for_data_flow_read_and_or_write_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_IP_FIFOS_MEMORY_LOCKS_FOR_DATA_FLOW_READ_AND_OR_WRITE_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_IP_FIFOS_MEMORY_LOCKS_FOR_DATA_FLOW_READ_AND_OR_WRITE_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_IP_FIFOS_MEMORY_LOCKS_FOR_DATA_FLOW_READ_AND_OR_WRITE_REG_REGISTER_ID:
      regis = &(regs->inq.ip_fifos_memory_locks_for_data_flow_read_and_or_write_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_IP_FIFOS_MEMORY_LOCKS_FOR_DATA_FLOW_READ_AND_OR_WRITE_REG_LOCK_IPARD_ID:
      field = &(regs->inq.ip_fifos_memory_locks_for_data_flow_read_and_or_write_reg.lock_ipard);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_IP_FIFOS_MEMORY_LOCKS_FOR_DATA_FLOW_READ_AND_OR_WRITE_REG_LOCK_IPBRD_ID:
      field = &(regs->inq.ip_fifos_memory_locks_for_data_flow_read_and_or_write_reg.lock_ipbrd);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_IP_FIFOS_MEMORY_LOCKS_FOR_DATA_FLOW_READ_AND_OR_WRITE_REG_LOCK_IPAWR_ID:
      field = &(regs->inq.ip_fifos_memory_locks_for_data_flow_read_and_or_write_reg.lock_ipawr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_IP_FIFOS_MEMORY_LOCKS_FOR_DATA_FLOW_READ_AND_OR_WRITE_REG_LOCK_IPBWR_ID:
      field = &(regs->inq.ip_fifos_memory_locks_for_data_flow_read_and_or_write_reg.lock_ipbwr);
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
  soc_sand_os_printf( "inq.ip_fifos_memory_locks_for_data_flow_read_and_or_write_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_inq_ip_fifos_memory_locks_for_data_flow_read_and_or_write_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_ip_fifos_memory_locks_for_data_flow_read_and_or_write_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_IP_FIFOS_MEMORY_LOCKS_FOR_DATA_FLOW_READ_AND_OR_WRITE_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_IP_FIFOS_MEMORY_LOCKS_FOR_DATA_FLOW_READ_AND_OR_WRITE_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_IP_FIFOS_MEMORY_LOCKS_FOR_DATA_FLOW_READ_AND_OR_WRITE_REG_REGISTER_ID:
      regis = &(regs->inq.ip_fifos_memory_locks_for_data_flow_read_and_or_write_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_IP_FIFOS_MEMORY_LOCKS_FOR_DATA_FLOW_READ_AND_OR_WRITE_REG_LOCK_IPARD_ID:
      field = &(regs->inq.ip_fifos_memory_locks_for_data_flow_read_and_or_write_reg.lock_ipard);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_IP_FIFOS_MEMORY_LOCKS_FOR_DATA_FLOW_READ_AND_OR_WRITE_REG_LOCK_IPBRD_ID:
      field = &(regs->inq.ip_fifos_memory_locks_for_data_flow_read_and_or_write_reg.lock_ipbrd);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_IP_FIFOS_MEMORY_LOCKS_FOR_DATA_FLOW_READ_AND_OR_WRITE_REG_LOCK_IPAWR_ID:
      field = &(regs->inq.ip_fifos_memory_locks_for_data_flow_read_and_or_write_reg.lock_ipawr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_IP_FIFOS_MEMORY_LOCKS_FOR_DATA_FLOW_READ_AND_OR_WRITE_REG_LOCK_IPBWR_ID:
      field = &(regs->inq.ip_fifos_memory_locks_for_data_flow_read_and_or_write_reg.lock_ipbwr);
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
  ui_fap21v_acc_inq_indirect_command_rd_data_reg_7_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_indirect_command_rd_data_reg_7_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_7_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_7_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_7_REGISTER_ID:
      regis = &(regs->inq.indirect_command_rd_data_reg_7.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_7_INDIRECT_COMMAND_RD_DATA_ID:
      field = &(regs->inq.indirect_command_rd_data_reg_7.indirect_command_rd_data);
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
  soc_sand_os_printf( "inq.indirect_command_rd_data_reg_7: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_inq_indirect_command_rd_data_reg_7_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_indirect_command_rd_data_reg_7_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_7_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_7_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_7_REGISTER_ID:
      regis = &(regs->inq.indirect_command_rd_data_reg_7.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_7_INDIRECT_COMMAND_RD_DATA_ID:
      field = &(regs->inq.indirect_command_rd_data_reg_7.indirect_command_rd_data);
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
  ui_fap21v_acc_inq_indirect_command_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_indirect_command_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_REG_REGISTER_ID:
      regis = &(regs->inq.indirect_command_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_REG_INDIRECT_COMMAND_ID:
      field = &(regs->inq.indirect_command_reg.indirect_command);
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
  soc_sand_os_printf( "inq.indirect_command_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_inq_indirect_command_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_indirect_command_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_REG_REGISTER_ID:
      regis = &(regs->inq.indirect_command_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_REG_INDIRECT_COMMAND_ID:
      field = &(regs->inq.indirect_command_reg.indirect_command);
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
  ui_fap21v_acc_inq_interrupts_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_interrupts_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INTERRUPTS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_INTERRUPTS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_INTERRUPTS_REG_REGISTER_ID:
      regis = &(regs->inq.interrupts_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INTERRUPTS_REG_IPA_OVF_ID:
      field = &(regs->inq.interrupts_reg.ipa_ovf);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INTERRUPTS_REG_IPB_OVF_ID:
      field = &(regs->inq.interrupts_reg.ipb_ovf);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INTERRUPTS_REG_TDMA_OVF_ID:
      field = &(regs->inq.interrupts_reg.tdma_ovf);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INTERRUPTS_REG_TDMB_OVF_ID:
      field = &(regs->inq.interrupts_reg.tdmb_ovf);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INTERRUPTS_REG_IPA_MSB_ECC_ERR_ID:
      field = &(regs->inq.interrupts_reg.ipa_msb_ecc_err);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INTERRUPTS_REG_IPA_LSB_ECC_ERR_ID:
      field = &(regs->inq.interrupts_reg.ipa_lsb_ecc_err);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INTERRUPTS_REG_IPB_MSB_ECC_ERR_ID:
      field = &(regs->inq.interrupts_reg.ipb_msb_ecc_err);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INTERRUPTS_REG_IPB_LSB_ECC_ERR_ID:
      field = &(regs->inq.interrupts_reg.ipb_lsb_ecc_err);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INTERRUPTS_REG_TDMACOUNT_OVF_ID:
      field = &(regs->inq.interrupts_reg.tdmacount_ovf);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INTERRUPTS_REG_TDMBCOUNT_OVF_ID:
      field = &(regs->inq.interrupts_reg.tdmbcount_ovf);
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
  soc_sand_os_printf( "inq.interrupts_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_inq_interrupts_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_interrupts_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INTERRUPTS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_INTERRUPTS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_INTERRUPTS_REG_REGISTER_ID:
      regis = &(regs->inq.interrupts_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INTERRUPTS_REG_IPA_OVF_ID:
      field = &(regs->inq.interrupts_reg.ipa_ovf);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INTERRUPTS_REG_IPB_OVF_ID:
      field = &(regs->inq.interrupts_reg.ipb_ovf);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INTERRUPTS_REG_TDMA_OVF_ID:
      field = &(regs->inq.interrupts_reg.tdma_ovf);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INTERRUPTS_REG_TDMB_OVF_ID:
      field = &(regs->inq.interrupts_reg.tdmb_ovf);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INTERRUPTS_REG_IPA_MSB_ECC_ERR_ID:
      field = &(regs->inq.interrupts_reg.ipa_msb_ecc_err);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INTERRUPTS_REG_IPA_LSB_ECC_ERR_ID:
      field = &(regs->inq.interrupts_reg.ipa_lsb_ecc_err);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INTERRUPTS_REG_IPB_MSB_ECC_ERR_ID:
      field = &(regs->inq.interrupts_reg.ipb_msb_ecc_err);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INTERRUPTS_REG_IPB_LSB_ECC_ERR_ID:
      field = &(regs->inq.interrupts_reg.ipb_lsb_ecc_err);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INTERRUPTS_REG_TDMACOUNT_OVF_ID:
      field = &(regs->inq.interrupts_reg.tdmacount_ovf);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INTERRUPTS_REG_TDMBCOUNT_OVF_ID:
      field = &(regs->inq.interrupts_reg.tdmbcount_ovf);
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
  ui_fap21v_acc_inq_indirect_command_wr_data_reg_7_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_indirect_command_wr_data_reg_7_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_7_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_7_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_7_REGISTER_ID:
      regis = &(regs->inq.indirect_command_wr_data_reg_7.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_7_INDIRECT_COMMAND_WR_DATA_ID:
      field = &(regs->inq.indirect_command_wr_data_reg_7.indirect_command_wr_data);
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
  soc_sand_os_printf( "inq.indirect_command_wr_data_reg_7: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_inq_indirect_command_wr_data_reg_7_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_indirect_command_wr_data_reg_7_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_7_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_7_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_7_REGISTER_ID:
      regis = &(regs->inq.indirect_command_wr_data_reg_7.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_7_INDIRECT_COMMAND_WR_DATA_ID:
      field = &(regs->inq.indirect_command_wr_data_reg_7.indirect_command_wr_data);
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
  ui_fap21v_acc_inq_tdm_overflow_header_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_tdm_overflow_header_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_TDM_OVERFLOW_HEADER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_TDM_OVERFLOW_HEADER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_TDM_OVERFLOW_HEADER_REG_REGISTER_ID:
      regis = &(regs->inq.tdm_overflow_header_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_TDM_OVERFLOW_HEADER_REG_TDMA_OVF_HEADER_ID:
      field = &(regs->inq.tdm_overflow_header_reg.tdma_ovf_header);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_TDM_OVERFLOW_HEADER_REG_TDMB_OVF_HEADER_ID:
      field = &(regs->inq.tdm_overflow_header_reg.tdmb_ovf_header);
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
  soc_sand_os_printf( "inq.tdm_overflow_header_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_inq_tdm_overflow_header_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_tdm_overflow_header_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_TDM_OVERFLOW_HEADER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_TDM_OVERFLOW_HEADER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_TDM_OVERFLOW_HEADER_REG_REGISTER_ID:
      regis = &(regs->inq.tdm_overflow_header_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_TDM_OVERFLOW_HEADER_REG_TDMA_OVF_HEADER_ID:
      field = &(regs->inq.tdm_overflow_header_reg.tdma_ovf_header);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_TDM_OVERFLOW_HEADER_REG_TDMB_OVF_HEADER_ID:
      field = &(regs->inq.tdm_overflow_header_reg.tdmb_ovf_header);
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
  ui_fap21v_acc_inq_indirect_command_rd_data_reg_6_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_indirect_command_rd_data_reg_6_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_6_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_6_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_6_REGISTER_ID:
      regis = &(regs->inq.indirect_command_rd_data_reg_6.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_6_INDIRECT_COMMAND_RD_DATA_ID:
      field = &(regs->inq.indirect_command_rd_data_reg_6.indirect_command_rd_data);
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
  soc_sand_os_printf( "inq.indirect_command_rd_data_reg_6: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_inq_indirect_command_rd_data_reg_6_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq");
  soc_sand_proc_name = "ui_fap21v_acc_inq_indirect_command_rd_data_reg_6_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_6_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_6_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_6_REGISTER_ID:
      regis = &(regs->inq.indirect_command_rd_data_reg_6.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_6_INDIRECT_COMMAND_RD_DATA_ID:
      field = &(regs->inq.indirect_command_rd_data_reg_6.indirect_command_rd_data);
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
  ui_fap21v_acc_inq_get(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq_get");

  if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_2_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_indirect_command_wr_data_reg_2_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_IPB_OVERFLOW_HEADER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_ipb_overflow_header_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_1_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_indirect_command_rd_data_reg_1_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_4_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_indirect_command_wr_data_reg_4_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_LBPTH_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_lbpth_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_1_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_indirect_command_wr_data_reg_1_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INQ_FIFO_STATUS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_inq_fifo_status_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_IPA_OVERFLOW_HEADER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_ipa_overflow_header_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_SPI4_FRST_TH_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_spi4_frst_th_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_TDMB_PACKET_COUNTERS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_tdmb_packet_counters_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_SPI4_SCND_TH_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_spi4_scnd_th_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_3_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_indirect_command_rd_data_reg_3_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_MASKS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_masks_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_3_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_indirect_command_wr_data_reg_3_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_TDM_FIFOS_MEMORY_LOCKS_FOR_DATA_FLOW_READ_AND_OR_WRITE_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_tdm_fifos_memory_locks_for_data_flow_read_and_or_write_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_ADDRESS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_indirect_command_address_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_0_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_indirect_command_rd_data_reg_0_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_5_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_indirect_command_rd_data_reg_5_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_6_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_indirect_command_wr_data_reg_6_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_TDMA_PACKET_COUNTERS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_tdma_packet_counters_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_4_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_indirect_command_rd_data_reg_4_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_0_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_indirect_command_wr_data_reg_0_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_5_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_indirect_command_wr_data_reg_5_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_2_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_indirect_command_rd_data_reg_2_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_IP_FIFOS_MEMORY_LOCKS_FOR_DATA_FLOW_READ_AND_OR_WRITE_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_ip_fifos_memory_locks_for_data_flow_read_and_or_write_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_7_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_indirect_command_rd_data_reg_7_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_indirect_command_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INTERRUPTS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_interrupts_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_7_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_indirect_command_wr_data_reg_7_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_TDM_OVERFLOW_HEADER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_tdm_overflow_header_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_6_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_indirect_command_rd_data_reg_6_get(current_line);
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
  ui_fap21v_acc_inq_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_inq_set");

  if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_2_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_indirect_command_wr_data_reg_2_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_IPB_OVERFLOW_HEADER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_ipb_overflow_header_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_1_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_indirect_command_rd_data_reg_1_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_4_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_indirect_command_wr_data_reg_4_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_LBPTH_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_lbpth_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_1_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_indirect_command_wr_data_reg_1_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INQ_FIFO_STATUS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_inq_fifo_status_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_IPA_OVERFLOW_HEADER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_ipa_overflow_header_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_SPI4_FRST_TH_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_spi4_frst_th_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_TDMB_PACKET_COUNTERS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_tdmb_packet_counters_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_SPI4_SCND_TH_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_spi4_scnd_th_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_3_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_indirect_command_rd_data_reg_3_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_MASKS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_masks_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_3_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_indirect_command_wr_data_reg_3_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_TDM_FIFOS_MEMORY_LOCKS_FOR_DATA_FLOW_READ_AND_OR_WRITE_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_tdm_fifos_memory_locks_for_data_flow_read_and_or_write_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_ADDRESS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_indirect_command_address_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_0_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_indirect_command_rd_data_reg_0_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_5_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_indirect_command_rd_data_reg_5_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_6_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_indirect_command_wr_data_reg_6_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_TDMA_PACKET_COUNTERS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_tdma_packet_counters_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_4_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_indirect_command_rd_data_reg_4_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_0_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_indirect_command_wr_data_reg_0_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_5_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_indirect_command_wr_data_reg_5_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_2_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_indirect_command_rd_data_reg_2_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_IP_FIFOS_MEMORY_LOCKS_FOR_DATA_FLOW_READ_AND_OR_WRITE_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_ip_fifos_memory_locks_for_data_flow_read_and_or_write_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_7_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_indirect_command_rd_data_reg_7_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_indirect_command_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INTERRUPTS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_interrupts_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_WR_DATA_REG_7_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_indirect_command_wr_data_reg_7_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_TDM_OVERFLOW_HEADER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_tdm_overflow_header_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_INQ_INDIRECT_COMMAND_RD_DATA_REG_6_ID,1))
  {
    ui_ret = ui_fap21v_acc_inq_indirect_command_rd_data_reg_6_set(current_line, value);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after inq***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}
