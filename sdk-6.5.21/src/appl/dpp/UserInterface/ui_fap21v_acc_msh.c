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
  ui_fap21v_acc_msh_init_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_msh");
  soc_sand_proc_name = "ui_fap21v_acc_msh_init_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MSH_INIT_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MSH_INIT_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MSH_INIT_REG_REGISTER_ID:
      regis = &(regs->msh.init_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MSH_INIT_REG_CFG_DLY_MSG_GEN_ID:
      field = &(regs->msh.init_reg.config_1);
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
  soc_sand_os_printf( "msh.init_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_msh_init_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_msh");
  soc_sand_proc_name = "ui_fap21v_acc_msh_init_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MSH_INIT_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MSH_INIT_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MSH_INIT_REG_REGISTER_ID:
      regis = &(regs->msh.init_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MSH_INIT_REG_CFG_DLY_MSG_GEN_ID:
      field = &(regs->msh.init_reg.config_1);
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
  ui_fap21v_acc_msh_fap_detect_ctrl_cells_cnt_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_msh");
  soc_sand_proc_name = "ui_fap21v_acc_msh_fap_detect_ctrl_cells_cnt_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MSH_FAP_DETECT_CTRL_CELLS_CNT_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MSH_FAP_DETECT_CTRL_CELLS_CNT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MSH_FAP_DETECT_CTRL_CELLS_CNT_REGISTER_ID:
      regis = &(regs->msh.fap_detect_ctrl_cells_cnt.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MSH_FAP_DETECT_CTRL_CELLS_CNT_RCV_CTRL1_ID:
      field = &(regs->msh.fap_detect_ctrl_cells_cnt.rcv_ctrl1);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MSH_FAP_DETECT_CTRL_CELLS_CNT_RCV_CTRL2_ID:
      field = &(regs->msh.fap_detect_ctrl_cells_cnt.rcv_ctrl2);
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
  soc_sand_os_printf( "msh.fap_detect_ctrl_cells_cnt: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_msh_fap_detect_ctrl_cells_cnt_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_msh");
  soc_sand_proc_name = "ui_fap21v_acc_msh_fap_detect_ctrl_cells_cnt_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MSH_FAP_DETECT_CTRL_CELLS_CNT_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MSH_FAP_DETECT_CTRL_CELLS_CNT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MSH_FAP_DETECT_CTRL_CELLS_CNT_REGISTER_ID:
      regis = &(regs->msh.fap_detect_ctrl_cells_cnt.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MSH_FAP_DETECT_CTRL_CELLS_CNT_RCV_CTRL1_ID:
      field = &(regs->msh.fap_detect_ctrl_cells_cnt.rcv_ctrl1);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MSH_FAP_DETECT_CTRL_CELLS_CNT_RCV_CTRL2_ID:
      field = &(regs->msh.fap_detect_ctrl_cells_cnt.rcv_ctrl2);
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
  ui_fap21v_acc_msh_mesh_topology_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_msh");
  soc_sand_proc_name = "ui_fap21v_acc_msh_mesh_topology_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MSH_MESH_TOPOLOGY_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MSH_MESH_TOPOLOGY_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MSH_MESH_TOPOLOGY_REG_REGISTER_ID:
      regis = &(regs->msh.mesh_topology_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MSH_MESH_TOPOLOGY_REG_STAND_ALN_ID:
      field = &(regs->msh.mesh_topology_reg.stand_aln);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MSH_MESH_TOPOLOGY_REG_IN_SYSTEM_ID:
      field = &(regs->msh.mesh_topology_reg.in_system);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MSH_MESH_TOPOLOGY_REG_MULTI_FAP_ID:
      field = &(regs->msh.mesh_topology_reg.multi_fap);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MSH_MESH_TOPOLOGY_REG_CFG_CELL_REP_ID:
      field = &(regs->msh.mesh_topology_reg.reserved);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MSH_MESH_TOPOLOGY_REG_CFG_TRIG_ID:
      field = &(regs->msh.mesh_topology_reg.trig);
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
  soc_sand_os_printf( "msh.mesh_topology_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_msh_mesh_topology_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_msh");
  soc_sand_proc_name = "ui_fap21v_acc_msh_mesh_topology_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MSH_MESH_TOPOLOGY_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_MSH_MESH_TOPOLOGY_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_MSH_MESH_TOPOLOGY_REG_REGISTER_ID:
      regis = &(regs->msh.mesh_topology_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MSH_MESH_TOPOLOGY_REG_STAND_ALN_ID:
      field = &(regs->msh.mesh_topology_reg.stand_aln);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MSH_MESH_TOPOLOGY_REG_IN_SYSTEM_ID:
      field = &(regs->msh.mesh_topology_reg.in_system);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MSH_MESH_TOPOLOGY_REG_MULTI_FAP_ID:
      field = &(regs->msh.mesh_topology_reg.multi_fap);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MSH_MESH_TOPOLOGY_REG_CFG_CELL_REP_ID:
      field = &(regs->msh.mesh_topology_reg.reserved);
      break;
    case PARAM_FAP21V_ACC_DIRECT_MSH_MESH_TOPOLOGY_REG_CFG_TRIG_ID:
      field = &(regs->msh.mesh_topology_reg.trig);
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
  ui_fap21v_acc_msh_get(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_msh_get");

  if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MSH_INIT_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_msh_init_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MSH_FAP_DETECT_CTRL_CELLS_CNT_ID,1))
  {
    ui_ret = ui_fap21v_acc_msh_fap_detect_ctrl_cells_cnt_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MSH_MESH_TOPOLOGY_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_msh_mesh_topology_reg_get(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after msh***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_msh_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_msh_set");

  if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MSH_INIT_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_msh_init_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MSH_FAP_DETECT_CTRL_CELLS_CNT_ID,1))
  {
    ui_ret = ui_fap21v_acc_msh_fap_detect_ctrl_cells_cnt_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_MSH_MESH_TOPOLOGY_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_msh_mesh_topology_reg_set(current_line, value);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after msh***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}
