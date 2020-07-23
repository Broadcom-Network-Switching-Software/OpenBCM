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
  ui_fap21v_acc_mmu_indirect_command_wr_data_reg_2_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mmu");
  soc_sand_proc_name = "ui_fap21v_acc_mmu_indirect_command_wr_data_reg_2_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_2_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_2_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_2_REGISTER_ID:
      regis = &(regs->mmu.indirect_command_wr_data_reg_2.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_2_INDIRECT_COMMAND_WR_DATA_ID:
      field = &(regs->mmu.indirect_command_wr_data_reg_2.indirect_command_wr_data);
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
  soc_sand_os_printf( "mmu.indirect_command_wr_data_reg_2: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_mmu_indirect_command_wr_data_reg_2_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mmu");
  soc_sand_proc_name = "ui_fap21v_acc_mmu_indirect_command_wr_data_reg_2_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_2_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_2_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_2_REGISTER_ID:
      regis = &(regs->mmu.indirect_command_wr_data_reg_2.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_2_INDIRECT_COMMAND_WR_DATA_ID:
      field = &(regs->mmu.indirect_command_wr_data_reg_2.indirect_command_wr_data);
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
  ui_fap21v_acc_mmu_indirect_command_address_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mmu");
  soc_sand_proc_name = "ui_fap21v_acc_mmu_indirect_command_address_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_ADDRESS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_ADDRESS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_ADDRESS_REG_REGISTER_ID:
      regis = &(regs->mmu.indirect_command_address_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_ADDRESS_REG_MEM_ADD_ID:
      field = &(regs->mmu.indirect_command_address_reg.mem_add);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_ADDRESS_REG_MEM_RW_ID:
      field = &(regs->mmu.indirect_command_address_reg.mem_rw);
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
  soc_sand_os_printf( "mmu.indirect_command_address_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_mmu_indirect_command_address_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mmu");
  soc_sand_proc_name = "ui_fap21v_acc_mmu_indirect_command_address_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_ADDRESS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_ADDRESS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_ADDRESS_REG_REGISTER_ID:
      regis = &(regs->mmu.indirect_command_address_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_ADDRESS_REG_MEM_ADD_ID:
      field = &(regs->mmu.indirect_command_address_reg.mem_add);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_ADDRESS_REG_MEM_RW_ID:
      field = &(regs->mmu.indirect_command_address_reg.mem_rw);
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
  ui_fap21v_acc_mmu_indirect_command_rd_data_reg_1_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mmu");
  soc_sand_proc_name = "ui_fap21v_acc_mmu_indirect_command_rd_data_reg_1_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_1_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_1_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_1_REGISTER_ID:
      regis = &(regs->mmu.indirect_command_rd_data_reg_1.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_1_INDIRECT_COMMAND_RD_DATA_ID:
      field = &(regs->mmu.indirect_command_rd_data_reg_1.indirect_command_rd_data);
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
  soc_sand_os_printf( "mmu.indirect_command_rd_data_reg_1: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_mmu_indirect_command_rd_data_reg_1_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mmu");
  soc_sand_proc_name = "ui_fap21v_acc_mmu_indirect_command_rd_data_reg_1_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_1_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_1_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_1_REGISTER_ID:
      regis = &(regs->mmu.indirect_command_rd_data_reg_1.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_1_INDIRECT_COMMAND_RD_DATA_ID:
      field = &(regs->mmu.indirect_command_rd_data_reg_1.indirect_command_rd_data);
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
  ui_fap21v_acc_mmu_mmu_petra_header_configuration_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mmu");
  soc_sand_proc_name = "ui_fap21v_acc_mmu_mmu_petra_header_configuration_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_MMU_PETRA_HEADER_CONFIGURATION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MMU_MMU_PETRA_HEADER_CONFIGURATION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_PETRA_HEADER_CONFIGURATION_REG_REGISTER_ID:
      regis = &(regs->mmu.mmu_petra_header_configuration_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_PETRA_HEADER_CONFIGURATION_REG_IQS_ENA_ID:
      field = &(regs->mmu.mmu_petra_header_configuration_reg.iqs_ena);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_PETRA_HEADER_CONFIGURATION_REG_IQS_ENA_DEF_STAMP_ID:
      field = &(regs->mmu.mmu_petra_header_configuration_reg.iqs_ena_def_stamp);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_PETRA_HEADER_CONFIGURATION_REG_BOTTOM_UQS_ID:
      field = &(regs->mmu.mmu_petra_header_configuration_reg.bottom_uqs);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_PETRA_HEADER_CONFIGURATION_REG_IQS_DEF_STAMP_VAL_ID:
      field = &(regs->mmu.mmu_petra_header_configuration_reg.iqs_def_stamp_val);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_PETRA_HEADER_CONFIGURATION_REG_SFM_ENA_ID:
      field = &(regs->mmu.mmu_petra_header_configuration_reg.sfm_ena);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_PETRA_HEADER_CONFIGURATION_REG_SFM_TC_ENA_ID:
      field = &(regs->mmu.mmu_petra_header_configuration_reg.sfm_tc_ena);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_PETRA_HEADER_CONFIGURATION_REG_SFM_DP_ENA_ID:
      field = &(regs->mmu.mmu_petra_header_configuration_reg.sfm_dp_ena);
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
  soc_sand_os_printf( "mmu.mmu_petra_header_configuration_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_mmu_mmu_petra_header_configuration_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mmu");
  soc_sand_proc_name = "ui_fap21v_acc_mmu_mmu_petra_header_configuration_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_MMU_PETRA_HEADER_CONFIGURATION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MMU_MMU_PETRA_HEADER_CONFIGURATION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_PETRA_HEADER_CONFIGURATION_REG_REGISTER_ID:
      regis = &(regs->mmu.mmu_petra_header_configuration_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_PETRA_HEADER_CONFIGURATION_REG_IQS_ENA_ID:
      field = &(regs->mmu.mmu_petra_header_configuration_reg.iqs_ena);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_PETRA_HEADER_CONFIGURATION_REG_IQS_ENA_DEF_STAMP_ID:
      field = &(regs->mmu.mmu_petra_header_configuration_reg.iqs_ena_def_stamp);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_PETRA_HEADER_CONFIGURATION_REG_BOTTOM_UQS_ID:
      field = &(regs->mmu.mmu_petra_header_configuration_reg.bottom_uqs);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_PETRA_HEADER_CONFIGURATION_REG_IQS_DEF_STAMP_VAL_ID:
      field = &(regs->mmu.mmu_petra_header_configuration_reg.iqs_def_stamp_val);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_PETRA_HEADER_CONFIGURATION_REG_SFM_ENA_ID:
      field = &(regs->mmu.mmu_petra_header_configuration_reg.sfm_ena);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_PETRA_HEADER_CONFIGURATION_REG_SFM_TC_ENA_ID:
      field = &(regs->mmu.mmu_petra_header_configuration_reg.sfm_tc_ena);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_PETRA_HEADER_CONFIGURATION_REG_SFM_DP_ENA_ID:
      field = &(regs->mmu.mmu_petra_header_configuration_reg.sfm_dp_ena);
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
  ui_fap21v_acc_mmu_indirect_command_rd_data_reg_0_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mmu");
  soc_sand_proc_name = "ui_fap21v_acc_mmu_indirect_command_rd_data_reg_0_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_0_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_0_REGISTER_ID:
      regis = &(regs->mmu.indirect_command_rd_data_reg_0.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_0_INDIRECT_COMMAND_RD_DATA_ID:
      field = &(regs->mmu.indirect_command_rd_data_reg_0.indirect_command_rd_data);
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
  soc_sand_os_printf( "mmu.indirect_command_rd_data_reg_0: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_mmu_indirect_command_rd_data_reg_0_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mmu");
  soc_sand_proc_name = "ui_fap21v_acc_mmu_indirect_command_rd_data_reg_0_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_0_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_0_REGISTER_ID:
      regis = &(regs->mmu.indirect_command_rd_data_reg_0.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_0_INDIRECT_COMMAND_RD_DATA_ID:
      field = &(regs->mmu.indirect_command_rd_data_reg_0.indirect_command_rd_data);
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
  ui_fap21v_acc_mmu_indirect_command_wr_data_reg_1_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mmu");
  soc_sand_proc_name = "ui_fap21v_acc_mmu_indirect_command_wr_data_reg_1_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_1_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_1_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_1_REGISTER_ID:
      regis = &(regs->mmu.indirect_command_wr_data_reg_1.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_1_INDIRECT_COMMAND_WR_DATA_ID:
      field = &(regs->mmu.indirect_command_wr_data_reg_1.indirect_command_wr_data);
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
  soc_sand_os_printf( "mmu.indirect_command_wr_data_reg_1: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_mmu_indirect_command_wr_data_reg_1_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mmu");
  soc_sand_proc_name = "ui_fap21v_acc_mmu_indirect_command_wr_data_reg_1_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_1_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_1_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_1_REGISTER_ID:
      regis = &(regs->mmu.indirect_command_wr_data_reg_1.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_1_INDIRECT_COMMAND_WR_DATA_ID:
      field = &(regs->mmu.indirect_command_wr_data_reg_1.indirect_command_wr_data);
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
  ui_fap21v_acc_mmu_indirect_command_rd_data_reg_6_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mmu");
  soc_sand_proc_name = "ui_fap21v_acc_mmu_indirect_command_rd_data_reg_6_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_6_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_6_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_6_REGISTER_ID:
      regis = &(regs->mmu.indirect_command_rd_data_reg_6.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_6_INDIRECT_COMMAND_RD_DATA_ID:
      field = &(regs->mmu.indirect_command_rd_data_reg_6.indirect_command_rd_data);
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
  soc_sand_os_printf( "mmu.indirect_command_rd_data_reg_6: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_mmu_indirect_command_rd_data_reg_6_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mmu");
  soc_sand_proc_name = "ui_fap21v_acc_mmu_indirect_command_rd_data_reg_6_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_6_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_6_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_6_REGISTER_ID:
      regis = &(regs->mmu.indirect_command_rd_data_reg_6.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_6_INDIRECT_COMMAND_RD_DATA_ID:
      field = &(regs->mmu.indirect_command_rd_data_reg_6.indirect_command_rd_data);
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
  ui_fap21v_acc_mmu_indirect_command_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mmu");
  soc_sand_proc_name = "ui_fap21v_acc_mmu_indirect_command_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_REG_REGISTER_ID:
      regis = &(regs->mmu.indirect_command_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_REG_INDIRECT_COMMAND_ID:
      field = &(regs->mmu.indirect_command_reg.indirect_command);
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
  soc_sand_os_printf( "mmu.indirect_command_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_mmu_indirect_command_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mmu");
  soc_sand_proc_name = "ui_fap21v_acc_mmu_indirect_command_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_REG_REGISTER_ID:
      regis = &(regs->mmu.indirect_command_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_REG_INDIRECT_COMMAND_ID:
      field = &(regs->mmu.indirect_command_reg.indirect_command);
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
  ui_fap21v_acc_mmu_mmu_interrupts_mask_regsiter_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mmu");
  soc_sand_proc_name = "ui_fap21v_acc_mmu_mmu_interrupts_mask_regsiter_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_MMU_INTERRUPTS_MASK_REGSITER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MMU_MMU_INTERRUPTS_MASK_REGSITER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_INTERRUPTS_MASK_REGSITER_REG_REGISTER_ID:
      regis = &(regs->mmu.mmu_interrupts_mask_regsiter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_INTERRUPTS_MASK_REGSITER_REG_PEC_INT_ERR_MASK_ID:
      field = &(regs->mmu.mmu_interrupts_mask_regsiter_reg.pec_int_err_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_INTERRUPTS_MASK_REGSITER_REG_CRCERR_MASK_ID:
      field = &(regs->mmu.mmu_interrupts_mask_regsiter_reg.crcerr_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_INTERRUPTS_MASK_REGSITER_REG_PDC_SYNC_ERR_MASK_ID:
      field = &(regs->mmu.mmu_interrupts_mask_regsiter_reg.pdc_sync_err_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_INTERRUPTS_MASK_REGSITER_REG_ECCERR_MASK_ID:
      field = &(regs->mmu.mmu_interrupts_mask_regsiter_reg.eccerr_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_INTERRUPTS_MASK_REGSITER_REG_PDCBAD_DESC_SIZE_MASK_ID:
      field = &(regs->mmu.mmu_interrupts_mask_regsiter_reg.pdcbad_desc_size_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_INTERRUPTS_MASK_REGSITER_REG_PDCREREAD_TIME_OUT_MASK_ID:
      field = &(regs->mmu.mmu_interrupts_mask_regsiter_reg.pdcreread_time_out_mask);
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
  soc_sand_os_printf( "mmu.mmu_interrupts_mask_regsiter_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_mmu_mmu_interrupts_mask_regsiter_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mmu");
  soc_sand_proc_name = "ui_fap21v_acc_mmu_mmu_interrupts_mask_regsiter_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_MMU_INTERRUPTS_MASK_REGSITER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MMU_MMU_INTERRUPTS_MASK_REGSITER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_INTERRUPTS_MASK_REGSITER_REG_REGISTER_ID:
      regis = &(regs->mmu.mmu_interrupts_mask_regsiter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_INTERRUPTS_MASK_REGSITER_REG_PEC_INT_ERR_MASK_ID:
      field = &(regs->mmu.mmu_interrupts_mask_regsiter_reg.pec_int_err_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_INTERRUPTS_MASK_REGSITER_REG_CRCERR_MASK_ID:
      field = &(regs->mmu.mmu_interrupts_mask_regsiter_reg.crcerr_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_INTERRUPTS_MASK_REGSITER_REG_PDC_SYNC_ERR_MASK_ID:
      field = &(regs->mmu.mmu_interrupts_mask_regsiter_reg.pdc_sync_err_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_INTERRUPTS_MASK_REGSITER_REG_ECCERR_MASK_ID:
      field = &(regs->mmu.mmu_interrupts_mask_regsiter_reg.eccerr_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_INTERRUPTS_MASK_REGSITER_REG_PDCBAD_DESC_SIZE_MASK_ID:
      field = &(regs->mmu.mmu_interrupts_mask_regsiter_reg.pdcbad_desc_size_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_INTERRUPTS_MASK_REGSITER_REG_PDCREREAD_TIME_OUT_MASK_ID:
      field = &(regs->mmu.mmu_interrupts_mask_regsiter_reg.pdcreread_time_out_mask);
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
  ui_fap21v_acc_mmu_indirect_command_wr_data_reg_4_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mmu");
  soc_sand_proc_name = "ui_fap21v_acc_mmu_indirect_command_wr_data_reg_4_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_4_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_4_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_4_REGISTER_ID:
      regis = &(regs->mmu.indirect_command_wr_data_reg_4.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_4_INDIRECT_COMMAND_WR_DATA_ID:
      field = &(regs->mmu.indirect_command_wr_data_reg_4.indirect_command_wr_data);
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
  soc_sand_os_printf( "mmu.indirect_command_wr_data_reg_4: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_mmu_indirect_command_wr_data_reg_4_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mmu");
  soc_sand_proc_name = "ui_fap21v_acc_mmu_indirect_command_wr_data_reg_4_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_4_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_4_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_4_REGISTER_ID:
      regis = &(regs->mmu.indirect_command_wr_data_reg_4.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_4_INDIRECT_COMMAND_WR_DATA_ID:
      field = &(regs->mmu.indirect_command_wr_data_reg_4.indirect_command_wr_data);
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
  ui_fap21v_acc_mmu_indirect_command_rd_data_reg_3_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mmu");
  soc_sand_proc_name = "ui_fap21v_acc_mmu_indirect_command_rd_data_reg_3_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_3_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_3_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_3_REGISTER_ID:
      regis = &(regs->mmu.indirect_command_rd_data_reg_3.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_3_INDIRECT_COMMAND_RD_DATA_ID:
      field = &(regs->mmu.indirect_command_rd_data_reg_3.indirect_command_rd_data);
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
  soc_sand_os_printf( "mmu.indirect_command_rd_data_reg_3: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_mmu_indirect_command_rd_data_reg_3_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mmu");
  soc_sand_proc_name = "ui_fap21v_acc_mmu_indirect_command_rd_data_reg_3_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_3_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_3_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_3_REGISTER_ID:
      regis = &(regs->mmu.indirect_command_rd_data_reg_3.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_3_INDIRECT_COMMAND_RD_DATA_ID:
      field = &(regs->mmu.indirect_command_rd_data_reg_3.indirect_command_rd_data);
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
  ui_fap21v_acc_mmu_indirect_command_wr_data_reg_3_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mmu");
  soc_sand_proc_name = "ui_fap21v_acc_mmu_indirect_command_wr_data_reg_3_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_3_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_3_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_3_REGISTER_ID:
      regis = &(regs->mmu.indirect_command_wr_data_reg_3.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_3_INDIRECT_COMMAND_WR_DATA_ID:
      field = &(regs->mmu.indirect_command_wr_data_reg_3.indirect_command_wr_data);
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
  soc_sand_os_printf( "mmu.indirect_command_wr_data_reg_3: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_mmu_indirect_command_wr_data_reg_3_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mmu");
  soc_sand_proc_name = "ui_fap21v_acc_mmu_indirect_command_wr_data_reg_3_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_3_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_3_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_3_REGISTER_ID:
      regis = &(regs->mmu.indirect_command_wr_data_reg_3.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_3_INDIRECT_COMMAND_WR_DATA_ID:
      field = &(regs->mmu.indirect_command_wr_data_reg_3.indirect_command_wr_data);
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
  ui_fap21v_acc_mmu_mmu_enablers_register_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mmu");
  soc_sand_proc_name = "ui_fap21v_acc_mmu_mmu_enablers_register_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_MMU_ENABLERS_REGISTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MMU_MMU_ENABLERS_REGISTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_ENABLERS_REGISTER_REG_REGISTER_ID:
      regis = &(regs->mmu.mmu_enablers_register_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_ENABLERS_REGISTER_REG_FAP10M_USE_IPM_PTR_ID:
      field = &(regs->mmu.mmu_enablers_register_reg.fap10m_use_ipm_ptr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_ENABLERS_REGISTER_REG_FAP10M_OVERRIDE_IPM_EN_ID:
      field = &(regs->mmu.mmu_enablers_register_reg.fap10m_override_ipm_en);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_ENABLERS_REGISTER_REG_FAP10M_VID_MC_IDX_EN_ID:
      field = &(regs->mmu.mmu_enablers_register_reg.fap10m_vid_mc_idx_en);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_ENABLERS_REGISTER_REG_BURST_SIZE_MODE_ID:
      field = &(regs->mmu.mmu_enablers_register_reg.burst_size_mode);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_ENABLERS_REGISTER_REG_SCR_RVRSE_MODE_ID:
      field = &(regs->mmu.mmu_enablers_register_reg.scr_rvrse_mode);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_ENABLERS_REGISTER_REG_SCR_BIT_NUM_ID:
      field = &(regs->mmu.mmu_enablers_register_reg.scr_bit_num);
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
  soc_sand_os_printf( "mmu.mmu_enablers_register_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_mmu_mmu_enablers_register_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mmu");
  soc_sand_proc_name = "ui_fap21v_acc_mmu_mmu_enablers_register_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_MMU_ENABLERS_REGISTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MMU_MMU_ENABLERS_REGISTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_ENABLERS_REGISTER_REG_REGISTER_ID:
      regis = &(regs->mmu.mmu_enablers_register_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_ENABLERS_REGISTER_REG_FAP10M_USE_IPM_PTR_ID:
      field = &(regs->mmu.mmu_enablers_register_reg.fap10m_use_ipm_ptr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_ENABLERS_REGISTER_REG_FAP10M_OVERRIDE_IPM_EN_ID:
      field = &(regs->mmu.mmu_enablers_register_reg.fap10m_override_ipm_en);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_ENABLERS_REGISTER_REG_FAP10M_VID_MC_IDX_EN_ID:
      field = &(regs->mmu.mmu_enablers_register_reg.fap10m_vid_mc_idx_en);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_ENABLERS_REGISTER_REG_BURST_SIZE_MODE_ID:
      field = &(regs->mmu.mmu_enablers_register_reg.burst_size_mode);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_ENABLERS_REGISTER_REG_SCR_RVRSE_MODE_ID:
      field = &(regs->mmu.mmu_enablers_register_reg.scr_rvrse_mode);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_ENABLERS_REGISTER_REG_SCR_BIT_NUM_ID:
      field = &(regs->mmu.mmu_enablers_register_reg.scr_bit_num);
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
  ui_fap21v_acc_mmu_indirect_command_wr_data_reg_6_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mmu");
  soc_sand_proc_name = "ui_fap21v_acc_mmu_indirect_command_wr_data_reg_6_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_6_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_6_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_6_REGISTER_ID:
      regis = &(regs->mmu.indirect_command_wr_data_reg_6.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_6_INDIRECT_COMMAND_WR_DATA_ID:
      field = &(regs->mmu.indirect_command_wr_data_reg_6.indirect_command_wr_data);
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
  soc_sand_os_printf( "mmu.indirect_command_wr_data_reg_6: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_mmu_indirect_command_wr_data_reg_6_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mmu");
  soc_sand_proc_name = "ui_fap21v_acc_mmu_indirect_command_wr_data_reg_6_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_6_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_6_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_6_REGISTER_ID:
      regis = &(regs->mmu.indirect_command_wr_data_reg_6.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_6_INDIRECT_COMMAND_WR_DATA_ID:
      field = &(regs->mmu.indirect_command_wr_data_reg_6.indirect_command_wr_data);
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
  ui_fap21v_acc_mmu_mmu_crc_error_counter_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mmu");
  soc_sand_proc_name = "ui_fap21v_acc_mmu_mmu_crc_error_counter_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_MMU_CRC_ERROR_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MMU_MMU_CRC_ERROR_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_CRC_ERROR_COUNTER_REG_REGISTER_ID:
      regis = &(regs->mmu.mmu_crc_error_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_CRC_ERROR_COUNTER_REG_CRCERR_CNT_ID:
      field = &(regs->mmu.mmu_crc_error_counter_reg.crcerr_cnt);
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
  soc_sand_os_printf( "mmu.mmu_crc_error_counter_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_mmu_mmu_crc_error_counter_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mmu");
  soc_sand_proc_name = "ui_fap21v_acc_mmu_mmu_crc_error_counter_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_MMU_CRC_ERROR_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MMU_MMU_CRC_ERROR_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_CRC_ERROR_COUNTER_REG_REGISTER_ID:
      regis = &(regs->mmu.mmu_crc_error_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_CRC_ERROR_COUNTER_REG_CRCERR_CNT_ID:
      field = &(regs->mmu.mmu_crc_error_counter_reg.crcerr_cnt);
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
  ui_fap21v_acc_mmu_mmu_address_mapping_register_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mmu");
  soc_sand_proc_name = "ui_fap21v_acc_mmu_mmu_address_mapping_register_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_MMU_ADDRESS_MAPPING_REGISTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MMU_MMU_ADDRESS_MAPPING_REGISTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_ADDRESS_MAPPING_REGISTER_REG_REGISTER_ID:
      regis = &(regs->mmu.mmu_address_mapping_register_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_ADDRESS_MAPPING_REGISTER_REG_ADDRESS_MAP_CONFIG_WR_ID:
      field = &(regs->mmu.mmu_address_mapping_register_reg.address_map_config_wr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_ADDRESS_MAPPING_REGISTER_REG_NUM_OF_COLS_ID:
      field = &(regs->mmu.mmu_address_mapping_register_reg.num_of_cols);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_ADDRESS_MAPPING_REGISTER_REG_ENABLE_8_BANK_MODE_ID:
      field = &(regs->mmu.mmu_address_mapping_register_reg.enable_8_bank_mode);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_ADDRESS_MAPPING_REGISTER_REG_ADDRESS_MAP_CONFIG_RD_ID:
      field = &(regs->mmu.mmu_address_mapping_register_reg.address_map_config_rd);
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
  soc_sand_os_printf( "mmu.mmu_address_mapping_register_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_mmu_mmu_address_mapping_register_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mmu");
  soc_sand_proc_name = "ui_fap21v_acc_mmu_mmu_address_mapping_register_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_MMU_ADDRESS_MAPPING_REGISTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MMU_MMU_ADDRESS_MAPPING_REGISTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_ADDRESS_MAPPING_REGISTER_REG_REGISTER_ID:
      regis = &(regs->mmu.mmu_address_mapping_register_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_ADDRESS_MAPPING_REGISTER_REG_ADDRESS_MAP_CONFIG_WR_ID:
      field = &(regs->mmu.mmu_address_mapping_register_reg.address_map_config_wr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_ADDRESS_MAPPING_REGISTER_REG_NUM_OF_COLS_ID:
      field = &(regs->mmu.mmu_address_mapping_register_reg.num_of_cols);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_ADDRESS_MAPPING_REGISTER_REG_ENABLE_8_BANK_MODE_ID:
      field = &(regs->mmu.mmu_address_mapping_register_reg.enable_8_bank_mode);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_ADDRESS_MAPPING_REGISTER_REG_ADDRESS_MAP_CONFIG_RD_ID:
      field = &(regs->mmu.mmu_address_mapping_register_reg.address_map_config_rd);
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
  ui_fap21v_acc_mmu_indirect_command_rd_data_reg_5_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mmu");
  soc_sand_proc_name = "ui_fap21v_acc_mmu_indirect_command_rd_data_reg_5_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_5_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_5_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_5_REGISTER_ID:
      regis = &(regs->mmu.indirect_command_rd_data_reg_5.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_5_INDIRECT_COMMAND_RD_DATA_ID:
      field = &(regs->mmu.indirect_command_rd_data_reg_5.indirect_command_rd_data);
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
  soc_sand_os_printf( "mmu.indirect_command_rd_data_reg_5: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_mmu_indirect_command_rd_data_reg_5_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mmu");
  soc_sand_proc_name = "ui_fap21v_acc_mmu_indirect_command_rd_data_reg_5_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_5_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_5_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_5_REGISTER_ID:
      regis = &(regs->mmu.indirect_command_rd_data_reg_5.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_5_INDIRECT_COMMAND_RD_DATA_ID:
      field = &(regs->mmu.indirect_command_rd_data_reg_5.indirect_command_rd_data);
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
  ui_fap21v_acc_mmu_mmu_interrupts_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mmu");
  soc_sand_proc_name = "ui_fap21v_acc_mmu_mmu_interrupts_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_MMU_INTERRUPTS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MMU_MMU_INTERRUPTS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_INTERRUPTS_REG_REGISTER_ID:
      regis = &(regs->mmu.mmu_interrupts_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_INTERRUPTS_REG_PEC_INT_ERR_ID:
      field = &(regs->mmu.mmu_interrupts_reg.pec_int_err);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_INTERRUPTS_REG_CRCERR_ID:
      field = &(regs->mmu.mmu_interrupts_reg.crcerr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_INTERRUPTS_REG_PDC_SYNC_ERR_ID:
      field = &(regs->mmu.mmu_interrupts_reg.pdc_sync_err);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_INTERRUPTS_REG_ECCERR_ID:
      field = &(regs->mmu.mmu_interrupts_reg.eccerr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_INTERRUPTS_REG_PDCBAD_DESC_SIZE_ID:
      field = &(regs->mmu.mmu_interrupts_reg.pdcbad_desc_size);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_INTERRUPTS_REG_PDCREREAD_TIME_OUT_ID:
      field = &(regs->mmu.mmu_interrupts_reg.pdcreread_time_out);
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
  soc_sand_os_printf( "mmu.mmu_interrupts_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_mmu_mmu_interrupts_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mmu");
  soc_sand_proc_name = "ui_fap21v_acc_mmu_mmu_interrupts_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_MMU_INTERRUPTS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MMU_MMU_INTERRUPTS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_INTERRUPTS_REG_REGISTER_ID:
      regis = &(regs->mmu.mmu_interrupts_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_INTERRUPTS_REG_PEC_INT_ERR_ID:
      field = &(regs->mmu.mmu_interrupts_reg.pec_int_err);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_INTERRUPTS_REG_CRCERR_ID:
      field = &(regs->mmu.mmu_interrupts_reg.crcerr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_INTERRUPTS_REG_PDC_SYNC_ERR_ID:
      field = &(regs->mmu.mmu_interrupts_reg.pdc_sync_err);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_INTERRUPTS_REG_ECCERR_ID:
      field = &(regs->mmu.mmu_interrupts_reg.eccerr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_INTERRUPTS_REG_PDCBAD_DESC_SIZE_ID:
      field = &(regs->mmu.mmu_interrupts_reg.pdcbad_desc_size);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_MMU_INTERRUPTS_REG_PDCREREAD_TIME_OUT_ID:
      field = &(regs->mmu.mmu_interrupts_reg.pdcreread_time_out);
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
  ui_fap21v_acc_mmu_indirect_command_wr_data_reg_5_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mmu");
  soc_sand_proc_name = "ui_fap21v_acc_mmu_indirect_command_wr_data_reg_5_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_5_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_5_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_5_REGISTER_ID:
      regis = &(regs->mmu.indirect_command_wr_data_reg_5.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_5_INDIRECT_COMMAND_WR_DATA_ID:
      field = &(regs->mmu.indirect_command_wr_data_reg_5.indirect_command_wr_data);
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
  soc_sand_os_printf( "mmu.indirect_command_wr_data_reg_5: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_mmu_indirect_command_wr_data_reg_5_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mmu");
  soc_sand_proc_name = "ui_fap21v_acc_mmu_indirect_command_wr_data_reg_5_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_5_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_5_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_5_REGISTER_ID:
      regis = &(regs->mmu.indirect_command_wr_data_reg_5.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_5_INDIRECT_COMMAND_WR_DATA_ID:
      field = &(regs->mmu.indirect_command_wr_data_reg_5.indirect_command_wr_data);
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
  ui_fap21v_acc_mmu_indirect_command_rd_data_reg_2_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mmu");
  soc_sand_proc_name = "ui_fap21v_acc_mmu_indirect_command_rd_data_reg_2_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_2_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_2_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_2_REGISTER_ID:
      regis = &(regs->mmu.indirect_command_rd_data_reg_2.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_2_INDIRECT_COMMAND_RD_DATA_ID:
      field = &(regs->mmu.indirect_command_rd_data_reg_2.indirect_command_rd_data);
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
  soc_sand_os_printf( "mmu.indirect_command_rd_data_reg_2: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_mmu_indirect_command_rd_data_reg_2_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mmu");
  soc_sand_proc_name = "ui_fap21v_acc_mmu_indirect_command_rd_data_reg_2_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_2_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_2_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_2_REGISTER_ID:
      regis = &(regs->mmu.indirect_command_rd_data_reg_2.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_2_INDIRECT_COMMAND_RD_DATA_ID:
      field = &(regs->mmu.indirect_command_rd_data_reg_2.indirect_command_rd_data);
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
  ui_fap21v_acc_mmu_indirect_command_wr_data_reg_0_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mmu");
  soc_sand_proc_name = "ui_fap21v_acc_mmu_indirect_command_wr_data_reg_0_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_0_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_0_REGISTER_ID:
      regis = &(regs->mmu.indirect_command_wr_data_reg_0.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_0_INDIRECT_COMMAND_WR_DATA_ID:
      field = &(regs->mmu.indirect_command_wr_data_reg_0.indirect_command_wr_data);
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
  soc_sand_os_printf( "mmu.indirect_command_wr_data_reg_0: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_mmu_indirect_command_wr_data_reg_0_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mmu");
  soc_sand_proc_name = "ui_fap21v_acc_mmu_indirect_command_wr_data_reg_0_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_0_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_0_REGISTER_ID:
      regis = &(regs->mmu.indirect_command_wr_data_reg_0.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_0_INDIRECT_COMMAND_WR_DATA_ID:
      field = &(regs->mmu.indirect_command_wr_data_reg_0.indirect_command_wr_data);
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
  ui_fap21v_acc_mmu_indirect_command_rd_data_reg_7_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mmu");
  soc_sand_proc_name = "ui_fap21v_acc_mmu_indirect_command_rd_data_reg_7_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_7_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_7_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_7_REGISTER_ID:
      regis = &(regs->mmu.indirect_command_rd_data_reg_7.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_7_INDIRECT_COMMAND_RD_DATA_ID:
      field = &(regs->mmu.indirect_command_rd_data_reg_7.indirect_command_rd_data);
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
  soc_sand_os_printf( "mmu.indirect_command_rd_data_reg_7: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_mmu_indirect_command_rd_data_reg_7_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mmu");
  soc_sand_proc_name = "ui_fap21v_acc_mmu_indirect_command_rd_data_reg_7_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_7_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_7_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_7_REGISTER_ID:
      regis = &(regs->mmu.indirect_command_rd_data_reg_7.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_7_INDIRECT_COMMAND_RD_DATA_ID:
      field = &(regs->mmu.indirect_command_rd_data_reg_7.indirect_command_rd_data);
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
  ui_fap21v_acc_mmu_indirect_command_wr_data_reg_7_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mmu");
  soc_sand_proc_name = "ui_fap21v_acc_mmu_indirect_command_wr_data_reg_7_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_7_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_7_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_7_REGISTER_ID:
      regis = &(regs->mmu.indirect_command_wr_data_reg_7.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_7_INDIRECT_COMMAND_WR_DATA_ID:
      field = &(regs->mmu.indirect_command_wr_data_reg_7.indirect_command_wr_data);
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
  soc_sand_os_printf( "mmu.indirect_command_wr_data_reg_7: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_mmu_indirect_command_wr_data_reg_7_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mmu");
  soc_sand_proc_name = "ui_fap21v_acc_mmu_indirect_command_wr_data_reg_7_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_7_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_7_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_7_REGISTER_ID:
      regis = &(regs->mmu.indirect_command_wr_data_reg_7.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_7_INDIRECT_COMMAND_WR_DATA_ID:
      field = &(regs->mmu.indirect_command_wr_data_reg_7.indirect_command_wr_data);
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
  ui_fap21v_acc_mmu_indirect_command_rd_data_reg_4_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mmu");
  soc_sand_proc_name = "ui_fap21v_acc_mmu_indirect_command_rd_data_reg_4_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_4_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_4_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_4_REGISTER_ID:
      regis = &(regs->mmu.indirect_command_rd_data_reg_4.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_4_INDIRECT_COMMAND_RD_DATA_ID:
      field = &(regs->mmu.indirect_command_rd_data_reg_4.indirect_command_rd_data);
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
  soc_sand_os_printf( "mmu.indirect_command_rd_data_reg_4: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_mmu_indirect_command_rd_data_reg_4_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mmu");
  soc_sand_proc_name = "ui_fap21v_acc_mmu_indirect_command_rd_data_reg_4_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_4_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_4_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_4_REGISTER_ID:
      regis = &(regs->mmu.indirect_command_rd_data_reg_4.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_4_INDIRECT_COMMAND_RD_DATA_ID:
      field = &(regs->mmu.indirect_command_rd_data_reg_4.indirect_command_rd_data);
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
  ui_fap21v_acc_mmu_get(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mmu_get");

  if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_2_ID,1))
  {
    ui_ret = ui_fap21v_acc_mmu_indirect_command_wr_data_reg_2_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_ADDRESS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_mmu_indirect_command_address_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_1_ID,1))
  {
    ui_ret = ui_fap21v_acc_mmu_indirect_command_rd_data_reg_1_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_MMU_PETRA_HEADER_CONFIGURATION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_mmu_mmu_petra_header_configuration_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_0_ID,1))
  {
    ui_ret = ui_fap21v_acc_mmu_indirect_command_rd_data_reg_0_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_1_ID,1))
  {
    ui_ret = ui_fap21v_acc_mmu_indirect_command_wr_data_reg_1_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_6_ID,1))
  {
    ui_ret = ui_fap21v_acc_mmu_indirect_command_rd_data_reg_6_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_mmu_indirect_command_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_MMU_INTERRUPTS_MASK_REGSITER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_mmu_mmu_interrupts_mask_regsiter_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_4_ID,1))
  {
    ui_ret = ui_fap21v_acc_mmu_indirect_command_wr_data_reg_4_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_3_ID,1))
  {
    ui_ret = ui_fap21v_acc_mmu_indirect_command_rd_data_reg_3_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_3_ID,1))
  {
    ui_ret = ui_fap21v_acc_mmu_indirect_command_wr_data_reg_3_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_MMU_ENABLERS_REGISTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_mmu_mmu_enablers_register_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_6_ID,1))
  {
    ui_ret = ui_fap21v_acc_mmu_indirect_command_wr_data_reg_6_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_MMU_CRC_ERROR_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_mmu_mmu_crc_error_counter_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_MMU_ADDRESS_MAPPING_REGISTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_mmu_mmu_address_mapping_register_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_5_ID,1))
  {
    ui_ret = ui_fap21v_acc_mmu_indirect_command_rd_data_reg_5_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_MMU_INTERRUPTS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_mmu_mmu_interrupts_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_5_ID,1))
  {
    ui_ret = ui_fap21v_acc_mmu_indirect_command_wr_data_reg_5_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_2_ID,1))
  {
    ui_ret = ui_fap21v_acc_mmu_indirect_command_rd_data_reg_2_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_0_ID,1))
  {
    ui_ret = ui_fap21v_acc_mmu_indirect_command_wr_data_reg_0_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_7_ID,1))
  {
    ui_ret = ui_fap21v_acc_mmu_indirect_command_rd_data_reg_7_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_7_ID,1))
  {
    ui_ret = ui_fap21v_acc_mmu_indirect_command_wr_data_reg_7_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_4_ID,1))
  {
    ui_ret = ui_fap21v_acc_mmu_indirect_command_rd_data_reg_4_get(current_line);
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
  ui_fap21v_acc_mmu_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_mmu_set");

  if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_2_ID,1))
  {
    ui_ret = ui_fap21v_acc_mmu_indirect_command_wr_data_reg_2_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_ADDRESS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_mmu_indirect_command_address_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_1_ID,1))
  {
    ui_ret = ui_fap21v_acc_mmu_indirect_command_rd_data_reg_1_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_MMU_PETRA_HEADER_CONFIGURATION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_mmu_mmu_petra_header_configuration_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_0_ID,1))
  {
    ui_ret = ui_fap21v_acc_mmu_indirect_command_rd_data_reg_0_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_1_ID,1))
  {
    ui_ret = ui_fap21v_acc_mmu_indirect_command_wr_data_reg_1_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_6_ID,1))
  {
    ui_ret = ui_fap21v_acc_mmu_indirect_command_rd_data_reg_6_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_mmu_indirect_command_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_MMU_INTERRUPTS_MASK_REGSITER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_mmu_mmu_interrupts_mask_regsiter_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_4_ID,1))
  {
    ui_ret = ui_fap21v_acc_mmu_indirect_command_wr_data_reg_4_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_3_ID,1))
  {
    ui_ret = ui_fap21v_acc_mmu_indirect_command_rd_data_reg_3_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_3_ID,1))
  {
    ui_ret = ui_fap21v_acc_mmu_indirect_command_wr_data_reg_3_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_MMU_ENABLERS_REGISTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_mmu_mmu_enablers_register_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_6_ID,1))
  {
    ui_ret = ui_fap21v_acc_mmu_indirect_command_wr_data_reg_6_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_MMU_CRC_ERROR_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_mmu_mmu_crc_error_counter_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_MMU_ADDRESS_MAPPING_REGISTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_mmu_mmu_address_mapping_register_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_5_ID,1))
  {
    ui_ret = ui_fap21v_acc_mmu_indirect_command_rd_data_reg_5_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_MMU_INTERRUPTS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_mmu_mmu_interrupts_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_5_ID,1))
  {
    ui_ret = ui_fap21v_acc_mmu_indirect_command_wr_data_reg_5_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_2_ID,1))
  {
    ui_ret = ui_fap21v_acc_mmu_indirect_command_rd_data_reg_2_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_0_ID,1))
  {
    ui_ret = ui_fap21v_acc_mmu_indirect_command_wr_data_reg_0_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_7_ID,1))
  {
    ui_ret = ui_fap21v_acc_mmu_indirect_command_rd_data_reg_7_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_WR_DATA_REG_7_ID,1))
  {
    ui_ret = ui_fap21v_acc_mmu_indirect_command_wr_data_reg_7_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MMU_INDIRECT_COMMAND_RD_DATA_REG_4_ID,1))
  {
    ui_ret = ui_fap21v_acc_mmu_indirect_command_rd_data_reg_4_set(current_line, value);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after mmu***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}
