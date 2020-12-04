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
  ui_fap21v_acc_rtp_multicast_link_up_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_rtp");
  soc_sand_proc_name = "ui_fap21v_acc_rtp_multicast_link_up_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_MULTICAST_LINK_UP_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_RTP_MULTICAST_LINK_UP_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_RTP_MULTICAST_LINK_UP_REG_REGISTER_ID:
      regis = &(regs->rtp.multicast_link_up_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_RTP_MULTICAST_LINK_UP_REG_MUL_LINK_UP_ID:
      field = &(regs->rtp.multicast_link_up_reg.mul_link_up);
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
  soc_sand_os_printf( "rtp.multicast_link_up_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_rtp_multicast_link_up_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_rtp");
  soc_sand_proc_name = "ui_fap21v_acc_rtp_multicast_link_up_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_MULTICAST_LINK_UP_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_RTP_MULTICAST_LINK_UP_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_RTP_MULTICAST_LINK_UP_REG_REGISTER_ID:
      regis = &(regs->rtp.multicast_link_up_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_RTP_MULTICAST_LINK_UP_REG_MUL_LINK_UP_ID:
      field = &(regs->rtp.multicast_link_up_reg.mul_link_up);
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
  ui_fap21v_acc_rtp_acl_received_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_rtp");
  soc_sand_proc_name = "ui_fap21v_acc_rtp_acl_received_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_ACL_RECEIVED_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_RTP_ACL_RECEIVED_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_RTP_ACL_RECEIVED_REG_REGISTER_ID:
      regis = &(regs->rtp.acl_received_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_RTP_ACL_RECEIVED_REG_ACLRCV_N_ID:
      field = &(regs->rtp.acl_received_reg.aclrcv_n);
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
  soc_sand_os_printf( "rtp.acl_received_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_rtp_acl_received_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_rtp");
  soc_sand_proc_name = "ui_fap21v_acc_rtp_acl_received_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_ACL_RECEIVED_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_RTP_ACL_RECEIVED_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_RTP_ACL_RECEIVED_REG_REGISTER_ID:
      regis = &(regs->rtp.acl_received_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_RTP_ACL_RECEIVED_REG_ACLRCV_N_ID:
      field = &(regs->rtp.acl_received_reg.aclrcv_n);
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
  ui_fap21v_acc_rtp_acl_generated_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_rtp");
  soc_sand_proc_name = "ui_fap21v_acc_rtp_acl_generated_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_ACL_GENERATED_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_RTP_ACL_GENERATED_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_RTP_ACL_GENERATED_REG_REGISTER_ID:
      regis = &(regs->rtp.acl_generated_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_RTP_ACL_GENERATED_REG_MLINK_MSK_CHANGED_ID:
      field = &(regs->rtp.acl_generated_reg.mlink_msk_changed);
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
  soc_sand_os_printf( "rtp.acl_generated_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_rtp_acl_generated_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_rtp");
  soc_sand_proc_name = "ui_fap21v_acc_rtp_acl_generated_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_ACL_GENERATED_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_RTP_ACL_GENERATED_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_RTP_ACL_GENERATED_REG_REGISTER_ID:
      regis = &(regs->rtp.acl_generated_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_RTP_ACL_GENERATED_REG_MLINK_MSK_CHANGED_ID:
      field = &(regs->rtp.acl_generated_reg.mlink_msk_changed);
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
  ui_fap21v_acc_rtp_indirect_command_wr_data_reg_1_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_rtp");
  soc_sand_proc_name = "ui_fap21v_acc_rtp_indirect_command_wr_data_reg_1_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_WR_DATA_REG_1_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_WR_DATA_REG_1_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_WR_DATA_REG_1_REGISTER_ID:
      regis = &(regs->rtp.indirect_command_wr_data_reg_1.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_WR_DATA_REG_1_INDIRECT_COMMAND_WR_DATA_ID:
      field = &(regs->rtp.indirect_command_wr_data_reg_1.indirect_command_wr_data);
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
  soc_sand_os_printf( "rtp.indirect_command_wr_data_reg_1: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_rtp_indirect_command_wr_data_reg_1_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_rtp");
  soc_sand_proc_name = "ui_fap21v_acc_rtp_indirect_command_wr_data_reg_1_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_WR_DATA_REG_1_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_WR_DATA_REG_1_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_WR_DATA_REG_1_REGISTER_ID:
      regis = &(regs->rtp.indirect_command_wr_data_reg_1.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_WR_DATA_REG_1_INDIRECT_COMMAND_WR_DATA_ID:
      field = &(regs->rtp.indirect_command_wr_data_reg_1.indirect_command_wr_data);
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
  ui_fap21v_acc_rtp_indirect_command_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_rtp");
  soc_sand_proc_name = "ui_fap21v_acc_rtp_indirect_command_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_REG_REGISTER_ID:
      regis = &(regs->rtp.indirect_command_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_REG_INDIRECT_COMMAND_ID:
      field = &(regs->rtp.indirect_command_reg.indirect_command);
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
  soc_sand_os_printf( "rtp.indirect_command_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_rtp_indirect_command_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_rtp");
  soc_sand_proc_name = "ui_fap21v_acc_rtp_indirect_command_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_REG_REGISTER_ID:
      regis = &(regs->rtp.indirect_command_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_REG_INDIRECT_COMMAND_ID:
      field = &(regs->rtp.indirect_command_reg.indirect_command);
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
  ui_fap21v_acc_rtp_multicast_distribution_map_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_rtp");
  soc_sand_proc_name = "ui_fap21v_acc_rtp_multicast_distribution_map_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_MULTICAST_DISTRIBUTION_MAP_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_RTP_MULTICAST_DISTRIBUTION_MAP_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_RTP_MULTICAST_DISTRIBUTION_MAP_REG_REGISTER_ID:
      regis = &(regs->rtp.multicast_distribution_map_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_RTP_MULTICAST_DISTRIBUTION_MAP_REG_MCDIST_ID:
      field = &(regs->rtp.multicast_distribution_map_reg.mcdist);
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
  soc_sand_os_printf( "rtp.multicast_distribution_map_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_rtp_multicast_distribution_map_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_rtp");
  soc_sand_proc_name = "ui_fap21v_acc_rtp_multicast_distribution_map_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_MULTICAST_DISTRIBUTION_MAP_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_RTP_MULTICAST_DISTRIBUTION_MAP_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_RTP_MULTICAST_DISTRIBUTION_MAP_REG_REGISTER_ID:
      regis = &(regs->rtp.multicast_distribution_map_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_RTP_MULTICAST_DISTRIBUTION_MAP_REG_MCDIST_ID:
      field = &(regs->rtp.multicast_distribution_map_reg.mcdist);
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
  ui_fap21v_acc_rtp_multicast_distribution_config_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_rtp");
  soc_sand_proc_name = "ui_fap21v_acc_rtp_multicast_distribution_config_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_MULTICAST_DISTRIBUTION_CONFIG_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_RTP_MULTICAST_DISTRIBUTION_CONFIG_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_RTP_MULTICAST_DISTRIBUTION_CONFIG_REG_REGISTER_ID:
      regis = &(regs->rtp.multicast_distribution_config_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_RTP_MULTICAST_DISTRIBUTION_CONFIG_REG_MUL_NUM_TRAV_ID:
      field = &(regs->rtp.multicast_distribution_config_reg.mul_num_trav);
      break;
    case PARAM_FAP21V_ACC_DIRECT_RTP_MULTICAST_DISTRIBUTION_CONFIG_REG_BYPASS_UPDATE_ID:
      field = &(regs->rtp.multicast_distribution_config_reg.bypass_update);
      break;
    case PARAM_FAP21V_ACC_DIRECT_RTP_MULTICAST_DISTRIBUTION_CONFIG_REG_ENABLE_MCLUPDATES_ID:
      field = &(regs->rtp.multicast_distribution_config_reg.enable_mclupdates);
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
  soc_sand_os_printf( "rtp.multicast_distribution_config_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_rtp_multicast_distribution_config_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_rtp");
  soc_sand_proc_name = "ui_fap21v_acc_rtp_multicast_distribution_config_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_MULTICAST_DISTRIBUTION_CONFIG_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_RTP_MULTICAST_DISTRIBUTION_CONFIG_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_RTP_MULTICAST_DISTRIBUTION_CONFIG_REG_REGISTER_ID:
      regis = &(regs->rtp.multicast_distribution_config_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_RTP_MULTICAST_DISTRIBUTION_CONFIG_REG_MUL_NUM_TRAV_ID:
      field = &(regs->rtp.multicast_distribution_config_reg.mul_num_trav);
      break;
    case PARAM_FAP21V_ACC_DIRECT_RTP_MULTICAST_DISTRIBUTION_CONFIG_REG_BYPASS_UPDATE_ID:
      field = &(regs->rtp.multicast_distribution_config_reg.bypass_update);
      break;
    case PARAM_FAP21V_ACC_DIRECT_RTP_MULTICAST_DISTRIBUTION_CONFIG_REG_ENABLE_MCLUPDATES_ID:
      field = &(regs->rtp.multicast_distribution_config_reg.enable_mclupdates);
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
  ui_fap21v_acc_rtp_indirect_command_address_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_rtp");
  soc_sand_proc_name = "ui_fap21v_acc_rtp_indirect_command_address_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_ADDRESS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_ADDRESS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_ADDRESS_REG_REGISTER_ID:
      regis = &(regs->rtp.indirect_command_address_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_ADDRESS_REG_INDIRECT_COMMAND_ADDRESS_ID:
      field = &(regs->rtp.indirect_command_address_reg.indirect_command_address);
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
  soc_sand_os_printf( "rtp.indirect_command_address_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_rtp_indirect_command_address_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_rtp");
  soc_sand_proc_name = "ui_fap21v_acc_rtp_indirect_command_address_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_ADDRESS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_ADDRESS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_ADDRESS_REG_REGISTER_ID:
      regis = &(regs->rtp.indirect_command_address_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_ADDRESS_REG_INDIRECT_COMMAND_ADDRESS_ID:
      field = &(regs->rtp.indirect_command_address_reg.indirect_command_address);
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
  ui_fap21v_acc_rtp_active_link_map_register_low_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_rtp");
  soc_sand_proc_name = "ui_fap21v_acc_rtp_active_link_map_register_low_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_ACTIVE_LINK_MAP_REGISTER_LOW_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_RTP_ACTIVE_LINK_MAP_REGISTER_LOW_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_RTP_ACTIVE_LINK_MAP_REGISTER_LOW_REG_REGISTER_ID:
      regis = &(regs->rtp.active_link_map_register_low_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_RTP_ACTIVE_LINK_MAP_REGISTER_LOW_REG_LNK_ACTV_MSK_ID:
      field = &(regs->rtp.active_link_map_register_low_reg.lnk_actv_msk);
      break;
    case PARAM_FAP21V_ACC_DIRECT_RTP_ACTIVE_LINK_MAP_REGISTER_LOW_REG_LNK_ACTV_MSK_CH_ID:
      field = &(regs->rtp.active_link_map_register_low_reg.lnk_actv_msk_ch);
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
  soc_sand_os_printf( "rtp.active_link_map_register_low_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_rtp_active_link_map_register_low_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_rtp");
  soc_sand_proc_name = "ui_fap21v_acc_rtp_active_link_map_register_low_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_ACTIVE_LINK_MAP_REGISTER_LOW_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_RTP_ACTIVE_LINK_MAP_REGISTER_LOW_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_RTP_ACTIVE_LINK_MAP_REGISTER_LOW_REG_REGISTER_ID:
      regis = &(regs->rtp.active_link_map_register_low_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_RTP_ACTIVE_LINK_MAP_REGISTER_LOW_REG_LNK_ACTV_MSK_ID:
      field = &(regs->rtp.active_link_map_register_low_reg.lnk_actv_msk);
      break;
    case PARAM_FAP21V_ACC_DIRECT_RTP_ACTIVE_LINK_MAP_REGISTER_LOW_REG_LNK_ACTV_MSK_CH_ID:
      field = &(regs->rtp.active_link_map_register_low_reg.lnk_actv_msk_ch);
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
  ui_fap21v_acc_rtp_indirect_command_rd_data_reg_0_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_rtp");
  soc_sand_proc_name = "ui_fap21v_acc_rtp_indirect_command_rd_data_reg_0_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_RD_DATA_REG_0_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_RD_DATA_REG_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_RD_DATA_REG_0_REGISTER_ID:
      regis = &(regs->rtp.indirect_command_rd_data_reg_0.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_RD_DATA_REG_0_INDIRECT_COMMAND_RD_DATA_ID:
      field = &(regs->rtp.indirect_command_rd_data_reg_0.indirect_command_rd_data);
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
  soc_sand_os_printf( "rtp.indirect_command_rd_data_reg_0: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_rtp_indirect_command_rd_data_reg_0_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_rtp");
  soc_sand_proc_name = "ui_fap21v_acc_rtp_indirect_command_rd_data_reg_0_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_RD_DATA_REG_0_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_RD_DATA_REG_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_RD_DATA_REG_0_REGISTER_ID:
      regis = &(regs->rtp.indirect_command_rd_data_reg_0.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_RD_DATA_REG_0_INDIRECT_COMMAND_RD_DATA_ID:
      field = &(regs->rtp.indirect_command_rd_data_reg_0.indirect_command_rd_data);
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
  ui_fap21v_acc_rtp_indirect_command_rd_data_reg_1_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_rtp");
  soc_sand_proc_name = "ui_fap21v_acc_rtp_indirect_command_rd_data_reg_1_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_RD_DATA_REG_1_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_RD_DATA_REG_1_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_RD_DATA_REG_1_REGISTER_ID:
      regis = &(regs->rtp.indirect_command_rd_data_reg_1.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_RD_DATA_REG_1_INDIRECT_COMMAND_RD_DATA_ID:
      field = &(regs->rtp.indirect_command_rd_data_reg_1.indirect_command_rd_data);
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
  soc_sand_os_printf( "rtp.indirect_command_rd_data_reg_1: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_rtp_indirect_command_rd_data_reg_1_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_rtp");
  soc_sand_proc_name = "ui_fap21v_acc_rtp_indirect_command_rd_data_reg_1_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_RD_DATA_REG_1_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_RD_DATA_REG_1_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_RD_DATA_REG_1_REGISTER_ID:
      regis = &(regs->rtp.indirect_command_rd_data_reg_1.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_RD_DATA_REG_1_INDIRECT_COMMAND_RD_DATA_ID:
      field = &(regs->rtp.indirect_command_rd_data_reg_1.indirect_command_rd_data);
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
  ui_fap21v_acc_rtp_fap21_coexist_connected_to_fdt_and_fct_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_rtp");
  soc_sand_proc_name = "ui_fap21v_acc_rtp_fap21_coexist_connected_to_fdt_and_fct_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_FAP21_COEXIST_CONNECTED_TO_FDT_AND_FCT_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_RTP_FAP21_COEXIST_CONNECTED_TO_FDT_AND_FCT_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_RTP_FAP21_COEXIST_CONNECTED_TO_FDT_AND_FCT_REG_REGISTER_ID:
      regis = &(regs->rtp.fap21_coexist_connected_to_fdt_and_fct_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_RTP_FAP21_COEXIST_CONNECTED_TO_FDT_AND_FCT_REG_DEST_CFG_ID:
      field = &(regs->rtp.fap21_coexist_connected_to_fdt_and_fct_reg.dest_cfg);
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
  soc_sand_os_printf( "rtp.fap21_coexist_connected_to_fdt_and_fct_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_rtp_fap21_coexist_connected_to_fdt_and_fct_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_rtp");
  soc_sand_proc_name = "ui_fap21v_acc_rtp_fap21_coexist_connected_to_fdt_and_fct_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_FAP21_COEXIST_CONNECTED_TO_FDT_AND_FCT_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_RTP_FAP21_COEXIST_CONNECTED_TO_FDT_AND_FCT_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_RTP_FAP21_COEXIST_CONNECTED_TO_FDT_AND_FCT_REG_REGISTER_ID:
      regis = &(regs->rtp.fap21_coexist_connected_to_fdt_and_fct_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_RTP_FAP21_COEXIST_CONNECTED_TO_FDT_AND_FCT_REG_DEST_CFG_ID:
      field = &(regs->rtp.fap21_coexist_connected_to_fdt_and_fct_reg.dest_cfg);
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
  ui_fap21v_acc_rtp_routing_processor_enables_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_rtp");
  soc_sand_proc_name = "ui_fap21v_acc_rtp_routing_processor_enables_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_ROUTING_PROCESSOR_ENABLES_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_RTP_ROUTING_PROCESSOR_ENABLES_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_RTP_ROUTING_PROCESSOR_ENABLES_REG_REGISTER_ID:
      regis = &(regs->rtp.routing_processor_enables_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_RTP_ROUTING_PROCESSOR_ENABLES_REG_RTP_WP_ID:
      field = &(regs->rtp.routing_processor_enables_reg.rtp_wp);
      break;
    case PARAM_FAP21V_ACC_DIRECT_RTP_ROUTING_PROCESSOR_ENABLES_REG_ACLM_ID:
      field = &(regs->rtp.routing_processor_enables_reg.aclm);
      break;
    case PARAM_FAP21V_ACC_DIRECT_RTP_ROUTING_PROCESSOR_ENABLES_REG_RTP_UP_EN_ID:
      field = &(regs->rtp.routing_processor_enables_reg.rtp_up_en);
      break;
    case PARAM_FAP21V_ACC_DIRECT_RTP_ROUTING_PROCESSOR_ENABLES_REG_RTP_EN_MSK_ID:
      field = &(regs->rtp.routing_processor_enables_reg.rtp_en_msk);
      break;
    case PARAM_FAP21V_ACC_DIRECT_RTP_ROUTING_PROCESSOR_ENABLES_REG_RMGR_ID:
      field = &(regs->rtp.routing_processor_enables_reg.rmgr);
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
  soc_sand_os_printf( "rtp.routing_processor_enables_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_rtp_routing_processor_enables_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_rtp");
  soc_sand_proc_name = "ui_fap21v_acc_rtp_routing_processor_enables_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_ROUTING_PROCESSOR_ENABLES_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_RTP_ROUTING_PROCESSOR_ENABLES_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_RTP_ROUTING_PROCESSOR_ENABLES_REG_REGISTER_ID:
      regis = &(regs->rtp.routing_processor_enables_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_RTP_ROUTING_PROCESSOR_ENABLES_REG_RTP_WP_ID:
      field = &(regs->rtp.routing_processor_enables_reg.rtp_wp);
      break;
    case PARAM_FAP21V_ACC_DIRECT_RTP_ROUTING_PROCESSOR_ENABLES_REG_ACLM_ID:
      field = &(regs->rtp.routing_processor_enables_reg.aclm);
      break;
    case PARAM_FAP21V_ACC_DIRECT_RTP_ROUTING_PROCESSOR_ENABLES_REG_RTP_UP_EN_ID:
      field = &(regs->rtp.routing_processor_enables_reg.rtp_up_en);
      break;
    case PARAM_FAP21V_ACC_DIRECT_RTP_ROUTING_PROCESSOR_ENABLES_REG_RTP_EN_MSK_ID:
      field = &(regs->rtp.routing_processor_enables_reg.rtp_en_msk);
      break;
    case PARAM_FAP21V_ACC_DIRECT_RTP_ROUTING_PROCESSOR_ENABLES_REG_RMGR_ID:
      field = &(regs->rtp.routing_processor_enables_reg.rmgr);
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
  ui_fap21v_acc_rtp_interrupts_mask_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_rtp");
  soc_sand_proc_name = "ui_fap21v_acc_rtp_interrupts_mask_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_INTERRUPTS_MASK_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_RTP_INTERRUPTS_MASK_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_RTP_INTERRUPTS_MASK_REG_REGISTER_ID:
      regis = &(regs->rtp.interrupts_mask_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_RTP_INTERRUPTS_MASK_REG_LINK_MSK_CHANGED_ID:
      field = &(regs->rtp.interrupts_mask_reg.link_msk_changed);
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
  soc_sand_os_printf( "rtp.interrupts_mask_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_rtp_interrupts_mask_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_rtp");
  soc_sand_proc_name = "ui_fap21v_acc_rtp_interrupts_mask_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_INTERRUPTS_MASK_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_RTP_INTERRUPTS_MASK_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_RTP_INTERRUPTS_MASK_REG_REGISTER_ID:
      regis = &(regs->rtp.interrupts_mask_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_RTP_INTERRUPTS_MASK_REG_LINK_MSK_CHANGED_ID:
      field = &(regs->rtp.interrupts_mask_reg.link_msk_changed);
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
  ui_fap21v_acc_rtp_indirect_command_wr_data_reg_0_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_rtp");
  soc_sand_proc_name = "ui_fap21v_acc_rtp_indirect_command_wr_data_reg_0_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_WR_DATA_REG_0_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_WR_DATA_REG_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_WR_DATA_REG_0_REGISTER_ID:
      regis = &(regs->rtp.indirect_command_wr_data_reg_0.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_WR_DATA_REG_0_INDIRECT_COMMAND_WR_DATA_ID:
      field = &(regs->rtp.indirect_command_wr_data_reg_0.indirect_command_wr_data);
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
  soc_sand_os_printf( "rtp.indirect_command_wr_data_reg_0: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_rtp_indirect_command_wr_data_reg_0_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_rtp");
  soc_sand_proc_name = "ui_fap21v_acc_rtp_indirect_command_wr_data_reg_0_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_WR_DATA_REG_0_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_WR_DATA_REG_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_WR_DATA_REG_0_REGISTER_ID:
      regis = &(regs->rtp.indirect_command_wr_data_reg_0.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_WR_DATA_REG_0_INDIRECT_COMMAND_WR_DATA_ID:
      field = &(regs->rtp.indirect_command_wr_data_reg_0.indirect_command_wr_data);
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
  ui_fap21v_acc_rtp_bypassing_the_tables_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_rtp");
  soc_sand_proc_name = "ui_fap21v_acc_rtp_bypassing_the_tables_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_BYPASSING_THE_TABLES_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_RTP_BYPASSING_THE_TABLES_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_RTP_BYPASSING_THE_TABLES_REG_REGISTER_ID:
      regis = &(regs->rtp.bypassing_the_tables_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_RTP_BYPASSING_THE_TABLES_REG_FRC_LNKS_HIGH_ID:
      field = &(regs->rtp.bypassing_the_tables_reg.frc_lnks_high);
      break;
    case PARAM_FAP21V_ACC_DIRECT_RTP_BYPASSING_THE_TABLES_REG_FRC_LNK_NUM_HIGH_ID:
      field = &(regs->rtp.bypassing_the_tables_reg.frc_lnk_num_high);
      break;
    case PARAM_FAP21V_ACC_DIRECT_RTP_BYPASSING_THE_TABLES_REG_FRC_LNK_NUM_ID:
      field = &(regs->rtp.bypassing_the_tables_reg.frc_lnk_num);
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
  soc_sand_os_printf( "rtp.bypassing_the_tables_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_rtp_bypassing_the_tables_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_rtp");
  soc_sand_proc_name = "ui_fap21v_acc_rtp_bypassing_the_tables_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_BYPASSING_THE_TABLES_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_RTP_BYPASSING_THE_TABLES_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_RTP_BYPASSING_THE_TABLES_REG_REGISTER_ID:
      regis = &(regs->rtp.bypassing_the_tables_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_RTP_BYPASSING_THE_TABLES_REG_FRC_LNKS_HIGH_ID:
      field = &(regs->rtp.bypassing_the_tables_reg.frc_lnks_high);
      break;
    case PARAM_FAP21V_ACC_DIRECT_RTP_BYPASSING_THE_TABLES_REG_FRC_LNK_NUM_HIGH_ID:
      field = &(regs->rtp.bypassing_the_tables_reg.frc_lnk_num_high);
      break;
    case PARAM_FAP21V_ACC_DIRECT_RTP_BYPASSING_THE_TABLES_REG_FRC_LNK_NUM_ID:
      field = &(regs->rtp.bypassing_the_tables_reg.frc_lnk_num);
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
  ui_fap21v_acc_rtp_interrupts_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_rtp");
  soc_sand_proc_name = "ui_fap21v_acc_rtp_interrupts_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_INTERRUPTS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_RTP_INTERRUPTS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_RTP_INTERRUPTS_REG_REGISTER_ID:
      regis = &(regs->rtp.interrupts_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_RTP_INTERRUPTS_REG_LINK_MSK_CHANGED_ID:
      field = &(regs->rtp.interrupts_reg.link_msk_changed);
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
  soc_sand_os_printf( "rtp.interrupts_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_rtp_interrupts_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_rtp");
  soc_sand_proc_name = "ui_fap21v_acc_rtp_interrupts_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_INTERRUPTS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_RTP_INTERRUPTS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_RTP_INTERRUPTS_REG_REGISTER_ID:
      regis = &(regs->rtp.interrupts_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_RTP_INTERRUPTS_REG_LINK_MSK_CHANGED_ID:
      field = &(regs->rtp.interrupts_reg.link_msk_changed);
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
  ui_fap21v_acc_rtp_max_base_index_for_reachabilty_message_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_rtp");
  soc_sand_proc_name = "ui_fap21v_acc_rtp_max_base_index_for_reachabilty_message_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_MAX_BASE_INDEX_FOR_REACHABILTY_MESSAGE_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_RTP_MAX_BASE_INDEX_FOR_REACHABILTY_MESSAGE_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_RTP_MAX_BASE_INDEX_FOR_REACHABILTY_MESSAGE_REG_REGISTER_ID:
      regis = &(regs->rtp.max_base_index_for_reachabilty_message_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_RTP_MAX_BASE_INDEX_FOR_REACHABILTY_MESSAGE_REG_MAX_BI_ID:
      field = &(regs->rtp.max_base_index_for_reachabilty_message_reg.max_bi);
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
  soc_sand_os_printf( "rtp.max_base_index_for_reachabilty_message_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_rtp_max_base_index_for_reachabilty_message_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_rtp");
  soc_sand_proc_name = "ui_fap21v_acc_rtp_max_base_index_for_reachabilty_message_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_MAX_BASE_INDEX_FOR_REACHABILTY_MESSAGE_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_RTP_MAX_BASE_INDEX_FOR_REACHABILTY_MESSAGE_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_RTP_MAX_BASE_INDEX_FOR_REACHABILTY_MESSAGE_REG_REGISTER_ID:
      regis = &(regs->rtp.max_base_index_for_reachabilty_message_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_RTP_MAX_BASE_INDEX_FOR_REACHABILTY_MESSAGE_REG_MAX_BI_ID:
      field = &(regs->rtp.max_base_index_for_reachabilty_message_reg.max_bi);
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
  ui_fap21v_acc_rtp_get(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_rtp_get");

  if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_MULTICAST_LINK_UP_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_rtp_multicast_link_up_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_ACL_RECEIVED_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_rtp_acl_received_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_ACL_GENERATED_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_rtp_acl_generated_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_WR_DATA_REG_1_ID,1))
  {
    ui_ret = ui_fap21v_acc_rtp_indirect_command_wr_data_reg_1_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_rtp_indirect_command_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_MULTICAST_DISTRIBUTION_MAP_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_rtp_multicast_distribution_map_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_MULTICAST_DISTRIBUTION_CONFIG_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_rtp_multicast_distribution_config_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_ADDRESS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_rtp_indirect_command_address_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_ACTIVE_LINK_MAP_REGISTER_LOW_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_rtp_active_link_map_register_low_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_RD_DATA_REG_0_ID,1))
  {
    ui_ret = ui_fap21v_acc_rtp_indirect_command_rd_data_reg_0_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_RD_DATA_REG_1_ID,1))
  {
    ui_ret = ui_fap21v_acc_rtp_indirect_command_rd_data_reg_1_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_FAP21_COEXIST_CONNECTED_TO_FDT_AND_FCT_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_rtp_fap21_coexist_connected_to_fdt_and_fct_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_ROUTING_PROCESSOR_ENABLES_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_rtp_routing_processor_enables_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_INTERRUPTS_MASK_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_rtp_interrupts_mask_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_WR_DATA_REG_0_ID,1))
  {
    ui_ret = ui_fap21v_acc_rtp_indirect_command_wr_data_reg_0_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_BYPASSING_THE_TABLES_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_rtp_bypassing_the_tables_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_INTERRUPTS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_rtp_interrupts_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_MAX_BASE_INDEX_FOR_REACHABILTY_MESSAGE_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_rtp_max_base_index_for_reachabilty_message_reg_get(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after rtp***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_rtp_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_rtp_set");

  if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_MULTICAST_LINK_UP_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_rtp_multicast_link_up_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_ACL_RECEIVED_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_rtp_acl_received_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_ACL_GENERATED_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_rtp_acl_generated_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_WR_DATA_REG_1_ID,1))
  {
    ui_ret = ui_fap21v_acc_rtp_indirect_command_wr_data_reg_1_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_rtp_indirect_command_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_MULTICAST_DISTRIBUTION_MAP_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_rtp_multicast_distribution_map_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_MULTICAST_DISTRIBUTION_CONFIG_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_rtp_multicast_distribution_config_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_ADDRESS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_rtp_indirect_command_address_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_ACTIVE_LINK_MAP_REGISTER_LOW_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_rtp_active_link_map_register_low_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_RD_DATA_REG_0_ID,1))
  {
    ui_ret = ui_fap21v_acc_rtp_indirect_command_rd_data_reg_0_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_RD_DATA_REG_1_ID,1))
  {
    ui_ret = ui_fap21v_acc_rtp_indirect_command_rd_data_reg_1_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_FAP21_COEXIST_CONNECTED_TO_FDT_AND_FCT_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_rtp_fap21_coexist_connected_to_fdt_and_fct_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_ROUTING_PROCESSOR_ENABLES_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_rtp_routing_processor_enables_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_INTERRUPTS_MASK_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_rtp_interrupts_mask_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_INDIRECT_COMMAND_WR_DATA_REG_0_ID,1))
  {
    ui_ret = ui_fap21v_acc_rtp_indirect_command_wr_data_reg_0_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_BYPASSING_THE_TABLES_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_rtp_bypassing_the_tables_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_INTERRUPTS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_rtp_interrupts_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_RTP_MAX_BASE_INDEX_FOR_REACHABILTY_MESSAGE_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_rtp_max_base_index_for_reachabilty_message_reg_set(current_line, value);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after rtp***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}
