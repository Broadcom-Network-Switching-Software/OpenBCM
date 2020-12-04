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
  ui_fap21v_acc_ips_internal_low_priority_dqcq_fc_status_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_internal_low_priority_dqcq_fc_status_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_INTERNAL_LOW_PRIORITY_DQCQ_FC_STATUS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_INTERNAL_LOW_PRIORITY_DQCQ_FC_STATUS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERNAL_LOW_PRIORITY_DQCQ_FC_STATUS_REG_REGISTER_ID:
      regis = &(regs->ips.internal_low_priority_dqcq_fc_status_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERNAL_LOW_PRIORITY_DQCQ_FC_STATUS_REG_LP_DQCQ_QUEUE_ID_ID:
      field = &(regs->ips.internal_low_priority_dqcq_fc_status_reg.lp_dqcq_queue_id);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERNAL_LOW_PRIORITY_DQCQ_FC_STATUS_REG_LP_DQCQ_QUEUE_SIZE_ID:
      field = &(regs->ips.internal_low_priority_dqcq_fc_status_reg.lp_dqcq_queue_size);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERNAL_LOW_PRIORITY_DQCQ_FC_STATUS_REG_LP_DQCQ_FLOW_CONTROL_ID:
      field = &(regs->ips.internal_low_priority_dqcq_fc_status_reg.lp_dqcq_flow_control);
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
  soc_sand_os_printf( "ips.internal_low_priority_dqcq_fc_status_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_internal_low_priority_dqcq_fc_status_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_internal_low_priority_dqcq_fc_status_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_INTERNAL_LOW_PRIORITY_DQCQ_FC_STATUS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_INTERNAL_LOW_PRIORITY_DQCQ_FC_STATUS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERNAL_LOW_PRIORITY_DQCQ_FC_STATUS_REG_REGISTER_ID:
      regis = &(regs->ips.internal_low_priority_dqcq_fc_status_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERNAL_LOW_PRIORITY_DQCQ_FC_STATUS_REG_LP_DQCQ_QUEUE_ID_ID:
      field = &(regs->ips.internal_low_priority_dqcq_fc_status_reg.lp_dqcq_queue_id);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERNAL_LOW_PRIORITY_DQCQ_FC_STATUS_REG_LP_DQCQ_QUEUE_SIZE_ID:
      field = &(regs->ips.internal_low_priority_dqcq_fc_status_reg.lp_dqcq_queue_size);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERNAL_LOW_PRIORITY_DQCQ_FC_STATUS_REG_LP_DQCQ_FLOW_CONTROL_ID:
      field = &(regs->ips.internal_low_priority_dqcq_fc_status_reg.lp_dqcq_flow_control);
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
  ui_fap21v_acc_ips_low_priority_dqcq_depth_config3_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_low_priority_dqcq_depth_config3_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG3_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG3_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG3_REG_REGISTER_ID:
      regis = &(regs->ips.low_priority_dqcq_depth_config3_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG3_REG_DEST4_DEPTH_ID:
      field = &(regs->ips.low_priority_dqcq_depth_config3_reg.dest4_depth);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG3_REG_DEST5_DEPTH_ID:
      field = &(regs->ips.low_priority_dqcq_depth_config3_reg.dest5_depth);
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
  soc_sand_os_printf( "ips.low_priority_dqcq_depth_config3_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_low_priority_dqcq_depth_config3_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_low_priority_dqcq_depth_config3_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG3_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG3_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG3_REG_REGISTER_ID:
      regis = &(regs->ips.low_priority_dqcq_depth_config3_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG3_REG_DEST4_DEPTH_ID:
      field = &(regs->ips.low_priority_dqcq_depth_config3_reg.dest4_depth);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG3_REG_DEST5_DEPTH_ID:
      field = &(regs->ips.low_priority_dqcq_depth_config3_reg.dest5_depth);
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
  ui_fap21v_acc_ips_system_red_aging_configuration_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_system_red_aging_configuration_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_SYSTEM_RED_AGING_CONFIGURATION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_SYSTEM_RED_AGING_CONFIGURATION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_SYSTEM_RED_AGING_CONFIGURATION_REG_REGISTER_ID:
      regis = &(regs->ips.system_red_aging_configuration_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_SYSTEM_RED_AGING_CONFIGURATION_REG_SYSTEM_RED_AGE_PERIOD_ID:
      field = &(regs->ips.system_red_aging_configuration_reg.system_red_age_period);
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
  soc_sand_os_printf( "ips.system_red_aging_configuration_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_system_red_aging_configuration_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_system_red_aging_configuration_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_SYSTEM_RED_AGING_CONFIGURATION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_SYSTEM_RED_AGING_CONFIGURATION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_SYSTEM_RED_AGING_CONFIGURATION_REG_REGISTER_ID:
      regis = &(regs->ips.system_red_aging_configuration_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_SYSTEM_RED_AGING_CONFIGURATION_REG_SYSTEM_RED_AGE_PERIOD_ID:
      field = &(regs->ips.system_red_aging_configuration_reg.system_red_age_period);
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
  ui_fap21v_acc_ips_ips_credit_config_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_ips_credit_config_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_IPS_CREDIT_CONFIG_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_IPS_CREDIT_CONFIG_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_IPS_CREDIT_CONFIG_REG_REGISTER_ID:
      regis = &(regs->ips.ips_credit_config_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_IPS_CREDIT_CONFIG_REG_CREDIT_VALUE_ID:
      field = &(regs->ips.ips_credit_config_reg.credit_value);
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
  soc_sand_os_printf( "ips.ips_credit_config_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_ips_credit_config_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_ips_credit_config_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_IPS_CREDIT_CONFIG_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_IPS_CREDIT_CONFIG_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_IPS_CREDIT_CONFIG_REG_REGISTER_ID:
      regis = &(regs->ips.ips_credit_config_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_IPS_CREDIT_CONFIG_REG_CREDIT_VALUE_ID:
      field = &(regs->ips.ips_credit_config_reg.credit_value);
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
  ui_fap21v_acc_ips_low_priority_dqcq_depth_config2_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_low_priority_dqcq_depth_config2_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG2_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG2_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG2_REG_REGISTER_ID:
      regis = &(regs->ips.low_priority_dqcq_depth_config2_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG2_REG_DEST2_DEPTH_ID:
      field = &(regs->ips.low_priority_dqcq_depth_config2_reg.dest2_depth);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG2_REG_DEST3_DEPTH_ID:
      field = &(regs->ips.low_priority_dqcq_depth_config2_reg.dest3_depth);
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
  soc_sand_os_printf( "ips.low_priority_dqcq_depth_config2_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_low_priority_dqcq_depth_config2_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_low_priority_dqcq_depth_config2_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG2_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG2_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG2_REG_REGISTER_ID:
      regis = &(regs->ips.low_priority_dqcq_depth_config2_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG2_REG_DEST2_DEPTH_ID:
      field = &(regs->ips.low_priority_dqcq_depth_config2_reg.dest2_depth);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG2_REG_DEST3_DEPTH_ID:
      field = &(regs->ips.low_priority_dqcq_depth_config2_reg.dest3_depth);
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
  ui_fap21v_acc_ips_low_priority_dqcq_depth_config1_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_low_priority_dqcq_depth_config1_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG1_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG1_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG1_REG_REGISTER_ID:
      regis = &(regs->ips.low_priority_dqcq_depth_config1_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG1_REG_DEST0_DEPTH_ID:
      field = &(regs->ips.low_priority_dqcq_depth_config1_reg.dest0_depth);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG1_REG_DEST1_DEPTH_ID:
      field = &(regs->ips.low_priority_dqcq_depth_config1_reg.dest1_depth);
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
  soc_sand_os_printf( "ips.low_priority_dqcq_depth_config1_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_low_priority_dqcq_depth_config1_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_low_priority_dqcq_depth_config1_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG1_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG1_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG1_REG_REGISTER_ID:
      regis = &(regs->ips.low_priority_dqcq_depth_config1_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG1_REG_DEST0_DEPTH_ID:
      field = &(regs->ips.low_priority_dqcq_depth_config1_reg.dest0_depth);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG1_REG_DEST1_DEPTH_ID:
      field = &(regs->ips.low_priority_dqcq_depth_config1_reg.dest1_depth);
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
  ui_fap21v_acc_ips_max_port_queue_size_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_max_port_queue_size_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_MAX_PORT_QUEUE_SIZE_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_MAX_PORT_QUEUE_SIZE_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_MAX_PORT_QUEUE_SIZE_REG_REGISTER_ID:
      regis = &(regs->ips.max_port_queue_size_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_MAX_PORT_QUEUE_SIZE_REG_MAX_PORT_QUEUE_SIZE_ID:
      field = &(regs->ips.max_port_queue_size_reg.max_port_queue_size);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_MAX_PORT_QUEUE_SIZE_REG_INS_PORT_QUEUE_SIZE_ID:
      field = &(regs->ips.max_port_queue_size_reg.ins_port_queue_size);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_MAX_PORT_QUEUE_SIZE_REG_MAX_PORT_QUEUE_SIZE_PORT_ID_ID:
      field = &(regs->ips.max_port_queue_size_reg.max_port_queue_size_port_id);
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
  soc_sand_os_printf( "ips.max_port_queue_size_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_max_port_queue_size_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_max_port_queue_size_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_MAX_PORT_QUEUE_SIZE_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_MAX_PORT_QUEUE_SIZE_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_MAX_PORT_QUEUE_SIZE_REG_REGISTER_ID:
      regis = &(regs->ips.max_port_queue_size_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_MAX_PORT_QUEUE_SIZE_REG_MAX_PORT_QUEUE_SIZE_ID:
      field = &(regs->ips.max_port_queue_size_reg.max_port_queue_size);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_MAX_PORT_QUEUE_SIZE_REG_INS_PORT_QUEUE_SIZE_ID:
      field = &(regs->ips.max_port_queue_size_reg.ins_port_queue_size);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_MAX_PORT_QUEUE_SIZE_REG_MAX_PORT_QUEUE_SIZE_PORT_ID_ID:
      field = &(regs->ips.max_port_queue_size_reg.max_port_queue_size_port_id);
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
  ui_fap21v_acc_ips_global_credit_counter_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_global_credit_counter_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_GLOBAL_CREDIT_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_GLOBAL_CREDIT_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_GLOBAL_CREDIT_COUNTER_REG_REGISTER_ID:
      regis = &(regs->ips.global_credit_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_GLOBAL_CREDIT_COUNTER_REG_GLOBAL_CREDIT_COUNTER_ID:
      field = &(regs->ips.global_credit_counter_reg.global_credit_counter);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_GLOBAL_CREDIT_COUNTER_REG_GLOBAL_CREDIT_COUNTER_OVERFLOW_ID:
      field = &(regs->ips.global_credit_counter_reg.global_credit_counter_overflow);
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
  soc_sand_os_printf( "ips.global_credit_counter_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_global_credit_counter_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_global_credit_counter_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_GLOBAL_CREDIT_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_GLOBAL_CREDIT_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_GLOBAL_CREDIT_COUNTER_REG_REGISTER_ID:
      regis = &(regs->ips.global_credit_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_GLOBAL_CREDIT_COUNTER_REG_GLOBAL_CREDIT_COUNTER_ID:
      field = &(regs->ips.global_credit_counter_reg.global_credit_counter);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_GLOBAL_CREDIT_COUNTER_REG_GLOBAL_CREDIT_COUNTER_OVERFLOW_ID:
      field = &(regs->ips.global_credit_counter_reg.global_credit_counter_overflow);
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
  ui_fap21v_acc_ips_masked_qdp_event_counter_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_masked_qdp_event_counter_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_MASKED_QDP_EVENT_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_MASKED_QDP_EVENT_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_MASKED_QDP_EVENT_COUNTER_REG_REGISTER_ID:
      regis = &(regs->ips.masked_qdp_event_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_MASKED_QDP_EVENT_COUNTER_REG_MSKD_QDP_EVNTS_COUNTER_ID:
      field = &(regs->ips.masked_qdp_event_counter_reg.mskd_qdp_evnts_counter);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_MASKED_QDP_EVENT_COUNTER_REG_MSKD_QDP_EVNTS_COUNTER_OVERFLOW_ID:
      field = &(regs->ips.masked_qdp_event_counter_reg.mskd_qdp_evnts_counter_overflow);
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
  soc_sand_os_printf( "ips.masked_qdp_event_counter_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_masked_qdp_event_counter_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_masked_qdp_event_counter_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_MASKED_QDP_EVENT_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_MASKED_QDP_EVENT_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_MASKED_QDP_EVENT_COUNTER_REG_REGISTER_ID:
      regis = &(regs->ips.masked_qdp_event_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_MASKED_QDP_EVENT_COUNTER_REG_MSKD_QDP_EVNTS_COUNTER_ID:
      field = &(regs->ips.masked_qdp_event_counter_reg.mskd_qdp_evnts_counter);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_MASKED_QDP_EVENT_COUNTER_REG_MSKD_QDP_EVNTS_COUNTER_OVERFLOW_ID:
      field = &(regs->ips.masked_qdp_event_counter_reg.mskd_qdp_evnts_counter_overflow);
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
  ui_fap21v_acc_ips_illegal_mesh_destination_queue_number_error_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_illegal_mesh_destination_queue_number_error_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_ILLEGAL_MESH_DESTINATION_QUEUE_NUMBER_ERROR_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_ILLEGAL_MESH_DESTINATION_QUEUE_NUMBER_ERROR_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_ILLEGAL_MESH_DESTINATION_QUEUE_NUMBER_ERROR_REG_REGISTER_ID:
      regis = &(regs->ips.illegal_mesh_destination_queue_number_error_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_ILLEGAL_MESH_DESTINATION_QUEUE_NUMBER_ERROR_REG_ILLEGEL_MESH_DEST_QUEUE_ID:
      field = &(regs->ips.illegal_mesh_destination_queue_number_error_reg.illegel_mesh_dest_queue);
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
  soc_sand_os_printf( "ips.illegal_mesh_destination_queue_number_error_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_illegal_mesh_destination_queue_number_error_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_illegal_mesh_destination_queue_number_error_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_ILLEGAL_MESH_DESTINATION_QUEUE_NUMBER_ERROR_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_ILLEGAL_MESH_DESTINATION_QUEUE_NUMBER_ERROR_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_ILLEGAL_MESH_DESTINATION_QUEUE_NUMBER_ERROR_REG_REGISTER_ID:
      regis = &(regs->ips.illegal_mesh_destination_queue_number_error_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_ILLEGAL_MESH_DESTINATION_QUEUE_NUMBER_ERROR_REG_ILLEGEL_MESH_DEST_QUEUE_ID:
      field = &(regs->ips.illegal_mesh_destination_queue_number_error_reg.illegel_mesh_dest_queue);
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
  ui_fap21v_acc_ips_dqcq_id_filter_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_dqcq_id_filter_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_DQCQ_ID_FILTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_DQCQ_ID_FILTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_DQCQ_ID_FILTER_REG_REGISTER_ID:
      regis = &(regs->ips.dqcq_id_filter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_DQCQ_ID_FILTER_REG_DQCQ_ID_FILTER_ID:
      field = &(regs->ips.dqcq_id_filter_reg.dqcq_id_filter);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_DQCQ_ID_FILTER_REG_DQCQ_DEST_PORT_FILTER_ID:
      field = &(regs->ips.dqcq_id_filter_reg.dqcq_dest_port_filter);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_DQCQ_ID_FILTER_REG_DQCQ_DEST_DEV_FILTER_ID:
      field = &(regs->ips.dqcq_id_filter_reg.dqcq_dest_dev_filter);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_DQCQ_ID_FILTER_REG_DQCQ_ID_FILTER_EN_ID:
      field = &(regs->ips.dqcq_id_filter_reg.dqcq_id_filter_en);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_DQCQ_ID_FILTER_REG_DQCQ_DEST_PORT_FILTER_EN_ID:
      field = &(regs->ips.dqcq_id_filter_reg.dqcq_dest_port_filter_en);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_DQCQ_ID_FILTER_REG_DQCQ_DEST_DEV_FILTER_EN_ID:
      field = &(regs->ips.dqcq_id_filter_reg.dqcq_dest_dev_filter_en);
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
  soc_sand_os_printf( "ips.dqcq_id_filter_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_dqcq_id_filter_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_dqcq_id_filter_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_DQCQ_ID_FILTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_DQCQ_ID_FILTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_DQCQ_ID_FILTER_REG_REGISTER_ID:
      regis = &(regs->ips.dqcq_id_filter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_DQCQ_ID_FILTER_REG_DQCQ_ID_FILTER_ID:
      field = &(regs->ips.dqcq_id_filter_reg.dqcq_id_filter);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_DQCQ_ID_FILTER_REG_DQCQ_DEST_PORT_FILTER_ID:
      field = &(regs->ips.dqcq_id_filter_reg.dqcq_dest_port_filter);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_DQCQ_ID_FILTER_REG_DQCQ_DEST_DEV_FILTER_ID:
      field = &(regs->ips.dqcq_id_filter_reg.dqcq_dest_dev_filter);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_DQCQ_ID_FILTER_REG_DQCQ_ID_FILTER_EN_ID:
      field = &(regs->ips.dqcq_id_filter_reg.dqcq_id_filter_en);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_DQCQ_ID_FILTER_REG_DQCQ_DEST_PORT_FILTER_EN_ID:
      field = &(regs->ips.dqcq_id_filter_reg.dqcq_dest_port_filter_en);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_DQCQ_ID_FILTER_REG_DQCQ_DEST_DEV_FILTER_EN_ID:
      field = &(regs->ips.dqcq_id_filter_reg.dqcq_dest_dev_filter_en);
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
  ui_fap21v_acc_ips_time_in_slow_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_time_in_slow_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_TIME_IN_SLOW_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_TIME_IN_SLOW_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_TIME_IN_SLOW_REG_REGISTER_ID:
      regis = &(regs->ips.time_in_slow_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_TIME_IN_SLOW_REG_TIME_IN_SLOW_ID:
      field = &(regs->ips.time_in_slow_reg.time_in_slow);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_TIME_IN_SLOW_REG_TIME_IN_SLOW_VALID_ID:
      field = &(regs->ips.time_in_slow_reg.time_in_slow_valid);
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
  soc_sand_os_printf( "ips.time_in_slow_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_time_in_slow_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_time_in_slow_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_TIME_IN_SLOW_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_TIME_IN_SLOW_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_TIME_IN_SLOW_REG_REGISTER_ID:
      regis = &(regs->ips.time_in_slow_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_TIME_IN_SLOW_REG_TIME_IN_SLOW_ID:
      field = &(regs->ips.time_in_slow_reg.time_in_slow);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_TIME_IN_SLOW_REG_TIME_IN_SLOW_VALID_ID:
      field = &(regs->ips.time_in_slow_reg.time_in_slow_valid);
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
  ui_fap21v_acc_ips_fsm_on_message_shaper_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_fsm_on_message_shaper_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FSM_ON_MESSAGE_SHAPER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_FSM_ON_MESSAGE_SHAPER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_FSM_ON_MESSAGE_SHAPER_REG_REGISTER_ID:
      regis = &(regs->ips.fsm_on_message_shaper_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FSM_ON_MESSAGE_SHAPER_REG_FSM_MAX_RATE_ID:
      field = &(regs->ips.fsm_on_message_shaper_reg.fsm_max_rate);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FSM_ON_MESSAGE_SHAPER_REG_FSM_MAX_BURST_ID:
      field = &(regs->ips.fsm_on_message_shaper_reg.fsm_max_burst);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FSM_ON_MESSAGE_SHAPER_REG_SHAPER_LOW_PRI_ID:
      field = &(regs->ips.fsm_on_message_shaper_reg.shaper_low_pri);
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
  soc_sand_os_printf( "ips.fsm_on_message_shaper_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_fsm_on_message_shaper_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_fsm_on_message_shaper_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FSM_ON_MESSAGE_SHAPER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_FSM_ON_MESSAGE_SHAPER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_FSM_ON_MESSAGE_SHAPER_REG_REGISTER_ID:
      regis = &(regs->ips.fsm_on_message_shaper_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FSM_ON_MESSAGE_SHAPER_REG_FSM_MAX_RATE_ID:
      field = &(regs->ips.fsm_on_message_shaper_reg.fsm_max_rate);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FSM_ON_MESSAGE_SHAPER_REG_FSM_MAX_BURST_ID:
      field = &(regs->ips.fsm_on_message_shaper_reg.fsm_max_burst);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FSM_ON_MESSAGE_SHAPER_REG_SHAPER_LOW_PRI_ID:
      field = &(regs->ips.fsm_on_message_shaper_reg.shaper_low_pri);
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
  ui_fap21v_acc_ips_activate_timer_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_activate_timer_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_ACTIVATE_TIMER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_ACTIVATE_TIMER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_ACTIVATE_TIMER_REG_REGISTER_ID:
      regis = &(regs->ips.activate_timer_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_ACTIVATE_TIMER_REG_ACTIVATE_TIMER_ID:
      field = &(regs->ips.activate_timer_reg.activate_timer);
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
  soc_sand_os_printf( "ips.activate_timer_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_activate_timer_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_activate_timer_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_ACTIVATE_TIMER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_ACTIVATE_TIMER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_ACTIVATE_TIMER_REG_REGISTER_ID:
      regis = &(regs->ips.activate_timer_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_ACTIVATE_TIMER_REG_ACTIVATE_TIMER_ID:
      field = &(regs->ips.activate_timer_reg.activate_timer);
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
  ui_fap21v_acc_ips_flow_status_filter_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_flow_status_filter_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FLOW_STATUS_FILTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_FLOW_STATUS_FILTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_FLOW_STATUS_FILTER_REG_REGISTER_ID:
      regis = &(regs->ips.flow_status_filter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FLOW_STATUS_FILTER_REG_FS_DEST_PORT_ID_ID:
      field = &(regs->ips.flow_status_filter_reg.fs_dest_port_id);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FLOW_STATUS_FILTER_REG_FS_DEST_DEV_ID_ID:
      field = &(regs->ips.flow_status_filter_reg.fs_dest_dev_id);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FLOW_STATUS_FILTER_REG_FLOW_STATUS_FILTER_ID:
      field = &(regs->ips.flow_status_filter_reg.flow_status_filter);
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
  soc_sand_os_printf( "ips.flow_status_filter_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_flow_status_filter_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_flow_status_filter_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FLOW_STATUS_FILTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_FLOW_STATUS_FILTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_FLOW_STATUS_FILTER_REG_REGISTER_ID:
      regis = &(regs->ips.flow_status_filter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FLOW_STATUS_FILTER_REG_FS_DEST_PORT_ID_ID:
      field = &(regs->ips.flow_status_filter_reg.fs_dest_port_id);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FLOW_STATUS_FILTER_REG_FS_DEST_DEV_ID_ID:
      field = &(regs->ips.flow_status_filter_reg.fs_dest_dev_id);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FLOW_STATUS_FILTER_REG_FLOW_STATUS_FILTER_ID:
      field = &(regs->ips.flow_status_filter_reg.flow_status_filter);
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
  ui_fap21v_acc_ips_global_flow_status_counter_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_global_flow_status_counter_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_GLOBAL_FLOW_STATUS_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_GLOBAL_FLOW_STATUS_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_GLOBAL_FLOW_STATUS_COUNTER_REG_REGISTER_ID:
      regis = &(regs->ips.global_flow_status_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_GLOBAL_FLOW_STATUS_COUNTER_REG_GLOBAL_FLOW_STATUS_COUNTER_ID:
      field = &(regs->ips.global_flow_status_counter_reg.global_flow_status_counter);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_GLOBAL_FLOW_STATUS_COUNTER_REG_GLOBAL_FLOW_STATUS_COUNTER_OVERFLOW_ID:
      field = &(regs->ips.global_flow_status_counter_reg.global_flow_status_counter_overflow);
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
  soc_sand_os_printf( "ips.global_flow_status_counter_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_global_flow_status_counter_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_global_flow_status_counter_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_GLOBAL_FLOW_STATUS_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_GLOBAL_FLOW_STATUS_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_GLOBAL_FLOW_STATUS_COUNTER_REG_REGISTER_ID:
      regis = &(regs->ips.global_flow_status_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_GLOBAL_FLOW_STATUS_COUNTER_REG_GLOBAL_FLOW_STATUS_COUNTER_ID:
      field = &(regs->ips.global_flow_status_counter_reg.global_flow_status_counter);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_GLOBAL_FLOW_STATUS_COUNTER_REG_GLOBAL_FLOW_STATUS_COUNTER_OVERFLOW_ID:
      field = &(regs->ips.global_flow_status_counter_reg.global_flow_status_counter_overflow);
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
  ui_fap21v_acc_ips_low_priority_dqcq_depth_config4_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_low_priority_dqcq_depth_config4_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG4_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG4_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG4_REG_REGISTER_ID:
      regis = &(regs->ips.low_priority_dqcq_depth_config4_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG4_REG_BFMC_DEPTH_ID:
      field = &(regs->ips.low_priority_dqcq_depth_config4_reg.bfmc_depth);
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
  soc_sand_os_printf( "ips.low_priority_dqcq_depth_config4_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_low_priority_dqcq_depth_config4_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_low_priority_dqcq_depth_config4_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG4_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG4_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG4_REG_REGISTER_ID:
      regis = &(regs->ips.low_priority_dqcq_depth_config4_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG4_REG_BFMC_DEPTH_ID:
      field = &(regs->ips.low_priority_dqcq_depth_config4_reg.bfmc_depth);
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
  ui_fap21v_acc_ips_flow_control_count_select_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_flow_control_count_select_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FLOW_CONTROL_COUNT_SELECT_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_FLOW_CONTROL_COUNT_SELECT_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_FLOW_CONTROL_COUNT_SELECT_REG_REGISTER_ID:
      regis = &(regs->ips.flow_control_count_select_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FLOW_CONTROL_COUNT_SELECT_REG_FC_COUNT_SEL_ID:
      field = &(regs->ips.flow_control_count_select_reg.fc_count_sel);
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
  soc_sand_os_printf( "ips.flow_control_count_select_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_flow_control_count_select_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_flow_control_count_select_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FLOW_CONTROL_COUNT_SELECT_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_FLOW_CONTROL_COUNT_SELECT_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_FLOW_CONTROL_COUNT_SELECT_REG_REGISTER_ID:
      regis = &(regs->ips.flow_control_count_select_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FLOW_CONTROL_COUNT_SELECT_REG_FC_COUNT_SEL_ID:
      field = &(regs->ips.flow_control_count_select_reg.fc_count_sel);
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
  ui_fap21v_acc_ips_indirect_command_wr_data_reg_0_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_indirect_command_wr_data_reg_0_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_WR_DATA_REG_0_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_WR_DATA_REG_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_WR_DATA_REG_0_REGISTER_ID:
      regis = &(regs->ips.indirect_command_wr_data_reg_0.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_WR_DATA_REG_0_INDIRECT_COMMAND_WR_DATA_ID:
      field = &(regs->ips.indirect_command_wr_data_reg_0.indirect_command_wr_data);
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
  soc_sand_os_printf( "ips.indirect_command_wr_data_reg_0: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_indirect_command_wr_data_reg_0_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_indirect_command_wr_data_reg_0_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_WR_DATA_REG_0_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_WR_DATA_REG_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_WR_DATA_REG_0_REGISTER_ID:
      regis = &(regs->ips.indirect_command_wr_data_reg_0.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_WR_DATA_REG_0_INDIRECT_COMMAND_WR_DATA_ID:
      field = &(regs->ips.indirect_command_wr_data_reg_0.indirect_command_wr_data);
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
  ui_fap21v_acc_ips_high_priority_dqcq_depth_config1_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_high_priority_dqcq_depth_config1_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_HIGH_PRIORITY_DQCQ_DEPTH_CONFIG1_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_HIGH_PRIORITY_DQCQ_DEPTH_CONFIG1_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_HIGH_PRIORITY_DQCQ_DEPTH_CONFIG1_REG_REGISTER_ID:
      regis = &(regs->ips.high_priority_dqcq_depth_config1_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_HIGH_PRIORITY_DQCQ_DEPTH_CONFIG1_REG_GFMC_DEPTH_ID:
      field = &(regs->ips.high_priority_dqcq_depth_config1_reg.gfmc_depth);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_HIGH_PRIORITY_DQCQ_DEPTH_CONFIG1_REG_IS_DEPTH_ID:
      field = &(regs->ips.high_priority_dqcq_depth_config1_reg.is_depth);
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
  soc_sand_os_printf( "ips.high_priority_dqcq_depth_config1_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_high_priority_dqcq_depth_config1_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_high_priority_dqcq_depth_config1_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_HIGH_PRIORITY_DQCQ_DEPTH_CONFIG1_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_HIGH_PRIORITY_DQCQ_DEPTH_CONFIG1_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_HIGH_PRIORITY_DQCQ_DEPTH_CONFIG1_REG_REGISTER_ID:
      regis = &(regs->ips.high_priority_dqcq_depth_config1_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_HIGH_PRIORITY_DQCQ_DEPTH_CONFIG1_REG_GFMC_DEPTH_ID:
      field = &(regs->ips.high_priority_dqcq_depth_config1_reg.gfmc_depth);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_HIGH_PRIORITY_DQCQ_DEPTH_CONFIG1_REG_IS_DEPTH_ID:
      field = &(regs->ips.high_priority_dqcq_depth_config1_reg.is_depth);
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
  ui_fap21v_acc_ips_qdp_report_filter_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_qdp_report_filter_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_QDP_REPORT_FILTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_QDP_REPORT_FILTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_QDP_REPORT_FILTER_REG_REGISTER_ID:
      regis = &(regs->ips.qdp_report_filter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_QDP_REPORT_FILTER_REG_DEQ_NOT_ENQ_FLTR_ID:
      field = &(regs->ips.qdp_report_filter_reg.deq_not_enq_fltr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_QDP_REPORT_FILTER_REG_DISCARD_FLTR_ID:
      field = &(regs->ips.qdp_report_filter_reg.discard_fltr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_QDP_REPORT_FILTER_REG_WQUP_FLTR_ID:
      field = &(regs->ips.qdp_report_filter_reg.wqup_fltr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_QDP_REPORT_FILTER_REG_DEQ_NOT_ENQ_FLTR_MASK_ID:
      field = &(regs->ips.qdp_report_filter_reg.deq_not_enq_fltr_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_QDP_REPORT_FILTER_REG_DISCARD_FLTR_MASK_ID:
      field = &(regs->ips.qdp_report_filter_reg.discard_fltr_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_QDP_REPORT_FILTER_REG_WQUP_FLTR_MASK_ID:
      field = &(regs->ips.qdp_report_filter_reg.wqup_fltr_mask);
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
  soc_sand_os_printf( "ips.qdp_report_filter_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_qdp_report_filter_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_qdp_report_filter_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_QDP_REPORT_FILTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_QDP_REPORT_FILTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_QDP_REPORT_FILTER_REG_REGISTER_ID:
      regis = &(regs->ips.qdp_report_filter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_QDP_REPORT_FILTER_REG_DEQ_NOT_ENQ_FLTR_ID:
      field = &(regs->ips.qdp_report_filter_reg.deq_not_enq_fltr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_QDP_REPORT_FILTER_REG_DISCARD_FLTR_ID:
      field = &(regs->ips.qdp_report_filter_reg.discard_fltr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_QDP_REPORT_FILTER_REG_WQUP_FLTR_ID:
      field = &(regs->ips.qdp_report_filter_reg.wqup_fltr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_QDP_REPORT_FILTER_REG_DEQ_NOT_ENQ_FLTR_MASK_ID:
      field = &(regs->ips.qdp_report_filter_reg.deq_not_enq_fltr_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_QDP_REPORT_FILTER_REG_DISCARD_FLTR_MASK_ID:
      field = &(regs->ips.qdp_report_filter_reg.discard_fltr_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_QDP_REPORT_FILTER_REG_WQUP_FLTR_MASK_ID:
      field = &(regs->ips.qdp_report_filter_reg.wqup_fltr_mask);
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
  ui_fap21v_acc_ips_credit_fabric_latency_counter_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_credit_fabric_latency_counter_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_CREDIT_FABRIC_LATENCY_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_CREDIT_FABRIC_LATENCY_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_CREDIT_FABRIC_LATENCY_COUNTER_REG_REGISTER_ID:
      regis = &(regs->ips.credit_fabric_latency_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_CREDIT_FABRIC_LATENCY_COUNTER_REG_CR_LTNCY_COUNTER_ID:
      field = &(regs->ips.credit_fabric_latency_counter_reg.cr_ltncy_counter);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_CREDIT_FABRIC_LATENCY_COUNTER_REG_CR_LTNCY_QUE_ID:
      field = &(regs->ips.credit_fabric_latency_counter_reg.cr_ltncy_que);
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
  soc_sand_os_printf( "ips.credit_fabric_latency_counter_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_credit_fabric_latency_counter_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_credit_fabric_latency_counter_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_CREDIT_FABRIC_LATENCY_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_CREDIT_FABRIC_LATENCY_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_CREDIT_FABRIC_LATENCY_COUNTER_REG_REGISTER_ID:
      regis = &(regs->ips.credit_fabric_latency_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_CREDIT_FABRIC_LATENCY_COUNTER_REG_CR_LTNCY_COUNTER_ID:
      field = &(regs->ips.credit_fabric_latency_counter_reg.cr_ltncy_counter);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_CREDIT_FABRIC_LATENCY_COUNTER_REG_CR_LTNCY_QUE_ID:
      field = &(regs->ips.credit_fabric_latency_counter_reg.cr_ltncy_que);
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
  ui_fap21v_acc_ips_time_in_norm_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_time_in_norm_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_TIME_IN_NORM_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_TIME_IN_NORM_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_TIME_IN_NORM_REG_REGISTER_ID:
      regis = &(regs->ips.time_in_norm_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_TIME_IN_NORM_REG_TIME_IN_NORM_ID:
      field = &(regs->ips.time_in_norm_reg.time_in_norm);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_TIME_IN_NORM_REG_TIME_IN_NORM_VALID_ID:
      field = &(regs->ips.time_in_norm_reg.time_in_norm_valid);
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
  soc_sand_os_printf( "ips.time_in_norm_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_time_in_norm_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_time_in_norm_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_TIME_IN_NORM_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_TIME_IN_NORM_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_TIME_IN_NORM_REG_REGISTER_ID:
      regis = &(regs->ips.time_in_norm_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_TIME_IN_NORM_REG_TIME_IN_NORM_ID:
      field = &(regs->ips.time_in_norm_reg.time_in_norm);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_TIME_IN_NORM_REG_TIME_IN_NORM_VALID_ID:
      field = &(regs->ips.time_in_norm_reg.time_in_norm_valid);
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
  ui_fap21v_acc_ips_timer_configuration_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_timer_configuration_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_TIMER_CONFIGURATION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_TIMER_CONFIGURATION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_TIMER_CONFIGURATION_REG_REGISTER_ID:
      regis = &(regs->ips.timer_configuration_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_TIMER_CONFIGURATION_REG_TIME_COUNT_CONFIG_ID:
      field = &(regs->ips.timer_configuration_reg.time_count_config);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_TIMER_CONFIGURATION_REG_COUNTER_RESET_ID:
      field = &(regs->ips.timer_configuration_reg.counter_reset);
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
  soc_sand_os_printf( "ips.timer_configuration_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_timer_configuration_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_timer_configuration_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_TIMER_CONFIGURATION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_TIMER_CONFIGURATION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_TIMER_CONFIGURATION_REG_REGISTER_ID:
      regis = &(regs->ips.timer_configuration_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_TIMER_CONFIGURATION_REG_TIME_COUNT_CONFIG_ID:
      field = &(regs->ips.timer_configuration_reg.time_count_config);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_TIMER_CONFIGURATION_REG_COUNTER_RESET_ID:
      field = &(regs->ips.timer_configuration_reg.counter_reset);
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
  ui_fap21v_acc_ips_fsmrq_max_occupancy_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_fsmrq_max_occupancy_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FSMRQ_MAX_OCCUPANCY_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_FSMRQ_MAX_OCCUPANCY_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_FSMRQ_MAX_OCCUPANCY_REG_REGISTER_ID:
      regis = &(regs->ips.fsmrq_max_occupancy_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FSMRQ_MAX_OCCUPANCY_REG_LP_FSMRQ_MAX_OCC_ID:
      field = &(regs->ips.fsmrq_max_occupancy_reg.lp_fsmrq_max_occ);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FSMRQ_MAX_OCCUPANCY_REG_HP_FSMRQ_MAX_OCC_ID:
      field = &(regs->ips.fsmrq_max_occupancy_reg.hp_fsmrq_max_occ);
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
  soc_sand_os_printf( "ips.fsmrq_max_occupancy_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_fsmrq_max_occupancy_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_fsmrq_max_occupancy_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FSMRQ_MAX_OCCUPANCY_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_FSMRQ_MAX_OCCUPANCY_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_FSMRQ_MAX_OCCUPANCY_REG_REGISTER_ID:
      regis = &(regs->ips.fsmrq_max_occupancy_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FSMRQ_MAX_OCCUPANCY_REG_LP_FSMRQ_MAX_OCC_ID:
      field = &(regs->ips.fsmrq_max_occupancy_reg.lp_fsmrq_max_occ);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FSMRQ_MAX_OCCUPANCY_REG_HP_FSMRQ_MAX_OCC_ID:
      field = &(regs->ips.fsmrq_max_occupancy_reg.hp_fsmrq_max_occ);
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
  ui_fap21v_acc_ips_global_qdp_report_counter_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_global_qdp_report_counter_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_GLOBAL_QDP_REPORT_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_GLOBAL_QDP_REPORT_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_GLOBAL_QDP_REPORT_COUNTER_REG_REGISTER_ID:
      regis = &(regs->ips.global_qdp_report_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_GLOBAL_QDP_REPORT_COUNTER_REG_GLBL_QDP_RPRT_COUNTER_ID:
      field = &(regs->ips.global_qdp_report_counter_reg.glbl_qdp_rprt_counter);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_GLOBAL_QDP_REPORT_COUNTER_REG_GLBL_QDP_RPRT_COUNTER_OVERFLOW_ID:
      field = &(regs->ips.global_qdp_report_counter_reg.glbl_qdp_rprt_counter_overflow);
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
  soc_sand_os_printf( "ips.global_qdp_report_counter_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_global_qdp_report_counter_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_global_qdp_report_counter_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_GLOBAL_QDP_REPORT_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_GLOBAL_QDP_REPORT_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_GLOBAL_QDP_REPORT_COUNTER_REG_REGISTER_ID:
      regis = &(regs->ips.global_qdp_report_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_GLOBAL_QDP_REPORT_COUNTER_REG_GLBL_QDP_RPRT_COUNTER_ID:
      field = &(regs->ips.global_qdp_report_counter_reg.glbl_qdp_rprt_counter);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_GLOBAL_QDP_REPORT_COUNTER_REG_GLBL_QDP_RPRT_COUNTER_OVERFLOW_ID:
      field = &(regs->ips.global_qdp_report_counter_reg.glbl_qdp_rprt_counter_overflow);
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
  ui_fap21v_acc_ips_indirect_command_rd_data_reg_0_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_indirect_command_rd_data_reg_0_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_RD_DATA_REG_0_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_RD_DATA_REG_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_RD_DATA_REG_0_REGISTER_ID:
      regis = &(regs->ips.indirect_command_rd_data_reg_0.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_RD_DATA_REG_0_INDIRECT_COMMAND_RD_DATA_ID:
      field = &(regs->ips.indirect_command_rd_data_reg_0.indirect_command_rd_data);
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
  soc_sand_os_printf( "ips.indirect_command_rd_data_reg_0: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_indirect_command_rd_data_reg_0_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_indirect_command_rd_data_reg_0_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_RD_DATA_REG_0_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_RD_DATA_REG_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_RD_DATA_REG_0_REGISTER_ID:
      regis = &(regs->ips.indirect_command_rd_data_reg_0.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_RD_DATA_REG_0_INDIRECT_COMMAND_RD_DATA_ID:
      field = &(regs->ips.indirect_command_rd_data_reg_0.indirect_command_rd_data);
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
  ui_fap21v_acc_ips_fms_max_occupancy_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_fms_max_occupancy_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FMS_MAX_OCCUPANCY_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_FMS_MAX_OCCUPANCY_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_FMS_MAX_OCCUPANCY_REG_REGISTER_ID:
      regis = &(regs->ips.fms_max_occupancy_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FMS_MAX_OCCUPANCY_REG_FMS_MAX_OCCUPANCY_ID:
      field = &(regs->ips.fms_max_occupancy_reg.fms_max_occupancy);
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
  soc_sand_os_printf( "ips.fms_max_occupancy_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_fms_max_occupancy_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_fms_max_occupancy_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FMS_MAX_OCCUPANCY_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_FMS_MAX_OCCUPANCY_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_FMS_MAX_OCCUPANCY_REG_REGISTER_ID:
      regis = &(regs->ips.fms_max_occupancy_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FMS_MAX_OCCUPANCY_REG_FMS_MAX_OCCUPANCY_ID:
      field = &(regs->ips.fms_max_occupancy_reg.fms_max_occupancy);
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
  ui_fap21v_acc_ips_capture_queue_descriptor_config_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_capture_queue_descriptor_config_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_CAPTURE_QUEUE_DESCRIPTOR_CONFIG_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_CAPTURE_QUEUE_DESCRIPTOR_CONFIG_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_CAPTURE_QUEUE_DESCRIPTOR_CONFIG_REG_REGISTER_ID:
      regis = &(regs->ips.capture_queue_descriptor_config_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_CAPTURE_QUEUE_DESCRIPTOR_CONFIG_REG_CAPTURE_CRS_ID:
      field = &(regs->ips.capture_queue_descriptor_config_reg.capture_crs);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_CAPTURE_QUEUE_DESCRIPTOR_CONFIG_REG_CAPTURE_CRS_EN_ID:
      field = &(regs->ips.capture_queue_descriptor_config_reg.capture_crs_en);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_CAPTURE_QUEUE_DESCRIPTOR_CONFIG_REG_CAPTURE_FSMRQ_CTRL_ID:
      field = &(regs->ips.capture_queue_descriptor_config_reg.capture_fsmrq_ctrl);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_CAPTURE_QUEUE_DESCRIPTOR_CONFIG_REG_CAPTURE_FSMRQ_CTRL_EN_ID:
      field = &(regs->ips.capture_queue_descriptor_config_reg.capture_fsmrq_ctrl_en);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_CAPTURE_QUEUE_DESCRIPTOR_CONFIG_REG_CAPTURE_IN_DQCQ_ID:
      field = &(regs->ips.capture_queue_descriptor_config_reg.capture_in_dqcq);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_CAPTURE_QUEUE_DESCRIPTOR_CONFIG_REG_CAPTURE_IN_DQCQ_EN_ID:
      field = &(regs->ips.capture_queue_descriptor_config_reg.capture_in_dqcq_en);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_CAPTURE_QUEUE_DESCRIPTOR_CONFIG_REG_CAPTURE_ONE_PKT_DEQ_ID:
      field = &(regs->ips.capture_queue_descriptor_config_reg.capture_one_pkt_deq);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_CAPTURE_QUEUE_DESCRIPTOR_CONFIG_REG_CAPTURE_ONE_PKT_DEQ_EN_ID:
      field = &(regs->ips.capture_queue_descriptor_config_reg.capture_one_pkt_deq_en);
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
  soc_sand_os_printf( "ips.capture_queue_descriptor_config_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_capture_queue_descriptor_config_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_capture_queue_descriptor_config_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_CAPTURE_QUEUE_DESCRIPTOR_CONFIG_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_CAPTURE_QUEUE_DESCRIPTOR_CONFIG_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_CAPTURE_QUEUE_DESCRIPTOR_CONFIG_REG_REGISTER_ID:
      regis = &(regs->ips.capture_queue_descriptor_config_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_CAPTURE_QUEUE_DESCRIPTOR_CONFIG_REG_CAPTURE_CRS_ID:
      field = &(regs->ips.capture_queue_descriptor_config_reg.capture_crs);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_CAPTURE_QUEUE_DESCRIPTOR_CONFIG_REG_CAPTURE_CRS_EN_ID:
      field = &(regs->ips.capture_queue_descriptor_config_reg.capture_crs_en);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_CAPTURE_QUEUE_DESCRIPTOR_CONFIG_REG_CAPTURE_FSMRQ_CTRL_ID:
      field = &(regs->ips.capture_queue_descriptor_config_reg.capture_fsmrq_ctrl);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_CAPTURE_QUEUE_DESCRIPTOR_CONFIG_REG_CAPTURE_FSMRQ_CTRL_EN_ID:
      field = &(regs->ips.capture_queue_descriptor_config_reg.capture_fsmrq_ctrl_en);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_CAPTURE_QUEUE_DESCRIPTOR_CONFIG_REG_CAPTURE_IN_DQCQ_ID:
      field = &(regs->ips.capture_queue_descriptor_config_reg.capture_in_dqcq);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_CAPTURE_QUEUE_DESCRIPTOR_CONFIG_REG_CAPTURE_IN_DQCQ_EN_ID:
      field = &(regs->ips.capture_queue_descriptor_config_reg.capture_in_dqcq_en);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_CAPTURE_QUEUE_DESCRIPTOR_CONFIG_REG_CAPTURE_ONE_PKT_DEQ_ID:
      field = &(regs->ips.capture_queue_descriptor_config_reg.capture_one_pkt_deq);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_CAPTURE_QUEUE_DESCRIPTOR_CONFIG_REG_CAPTURE_ONE_PKT_DEQ_EN_ID:
      field = &(regs->ips.capture_queue_descriptor_config_reg.capture_one_pkt_deq_en);
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
  ui_fap21v_acc_ips_interrupt_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_interrupt_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_REG_REGISTER_ID:
      regis = &(regs->ips.interrupt_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_REG_DQCQ_DEPTH_OVF_ID:
      field = &(regs->ips.interrupt_reg.dqcq_depth_ovf);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_REG_EMPTY_DQCQ_WRITE_ID:
      field = &(regs->ips.interrupt_reg.empty_dqcq_write);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_REG_ILLEGEL_MESH_DEST_ID:
      field = &(regs->ips.interrupt_reg.illegel_mesh_dest);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_REG_CREDIT_LOST_ID:
      field = &(regs->ips.interrupt_reg.credit_lost);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_REG_CREDIT_OVERFLOW_ID:
      field = &(regs->ips.interrupt_reg.credit_overflow);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_REG_SYSTEM_PORT_QUEUE_SIZE_EXPIRED_ID:
      field = &(regs->ips.interrupt_reg.system_port_queue_size_expired);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_REG_FLWID_ECC_ERR_ID:
      field = &(regs->ips.interrupt_reg.flwid_ecc_err);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_REG_QTYPE_ECC_ERR_ID:
      field = &(regs->ips.interrupt_reg.qtype_ecc_err);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_REG_QDESC_ECC_ERR_ID:
      field = &(regs->ips.interrupt_reg.qdesc_ecc_err);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_REG_QSZ_ECC_ERR_ID:
      field = &(regs->ips.interrupt_reg.qsz_ecc_err);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_REG_FSMRQCTRL_ECC_ERR_ID:
      field = &(regs->ips.interrupt_reg.fsmrqctrl_ecc_err);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_REG_DQCQMEM_ECC_ERR_ID:
      field = &(regs->ips.interrupt_reg.dqcqmem_ecc_err);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_REG_FSMRQMEM_ECC_ERR_ID:
      field = &(regs->ips.interrupt_reg.fsmrqmem_ecc_err);
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
  soc_sand_os_printf( "ips.interrupt_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_interrupt_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_interrupt_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_REG_REGISTER_ID:
      regis = &(regs->ips.interrupt_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_REG_DQCQ_DEPTH_OVF_ID:
      field = &(regs->ips.interrupt_reg.dqcq_depth_ovf);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_REG_EMPTY_DQCQ_WRITE_ID:
      field = &(regs->ips.interrupt_reg.empty_dqcq_write);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_REG_ILLEGEL_MESH_DEST_ID:
      field = &(regs->ips.interrupt_reg.illegel_mesh_dest);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_REG_CREDIT_LOST_ID:
      field = &(regs->ips.interrupt_reg.credit_lost);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_REG_CREDIT_OVERFLOW_ID:
      field = &(regs->ips.interrupt_reg.credit_overflow);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_REG_SYSTEM_PORT_QUEUE_SIZE_EXPIRED_ID:
      field = &(regs->ips.interrupt_reg.system_port_queue_size_expired);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_REG_FLWID_ECC_ERR_ID:
      field = &(regs->ips.interrupt_reg.flwid_ecc_err);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_REG_QTYPE_ECC_ERR_ID:
      field = &(regs->ips.interrupt_reg.qtype_ecc_err);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_REG_QDESC_ECC_ERR_ID:
      field = &(regs->ips.interrupt_reg.qdesc_ecc_err);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_REG_QSZ_ECC_ERR_ID:
      field = &(regs->ips.interrupt_reg.qsz_ecc_err);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_REG_FSMRQCTRL_ECC_ERR_ID:
      field = &(regs->ips.interrupt_reg.fsmrqctrl_ecc_err);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_REG_DQCQMEM_ECC_ERR_ID:
      field = &(regs->ips.interrupt_reg.dqcqmem_ecc_err);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_REG_FSMRQMEM_ECC_ERR_ID:
      field = &(regs->ips.interrupt_reg.fsmrqmem_ecc_err);
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
  ui_fap21v_acc_ips_bfmc_class_configs_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_bfmc_class_configs_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_BFMC_CLASS_CONFIGS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_BFMC_CLASS_CONFIGS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_BFMC_CLASS_CONFIGS_REG_REGISTER_ID:
      regis = &(regs->ips.bfmc_class_configs_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_BFMC_CLASS_CONFIGS_REG_BFMC_WFQ_EN_ID:
      field = &(regs->ips.bfmc_class_configs_reg.bfmc_wfq_en);
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
  soc_sand_os_printf( "ips.bfmc_class_configs_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_bfmc_class_configs_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_bfmc_class_configs_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_BFMC_CLASS_CONFIGS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_BFMC_CLASS_CONFIGS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_BFMC_CLASS_CONFIGS_REG_REGISTER_ID:
      regis = &(regs->ips.bfmc_class_configs_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_BFMC_CLASS_CONFIGS_REG_BFMC_WFQ_EN_ID:
      field = &(regs->ips.bfmc_class_configs_reg.bfmc_wfq_en);
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
  ui_fap21v_acc_ips_ips_general_configurations_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_ips_general_configurations_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_IPS_GENERAL_CONFIGURATIONS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_IPS_GENERAL_CONFIGURATIONS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_IPS_GENERAL_CONFIGURATIONS_REG_REGISTER_ID:
      regis = &(regs->ips.ips_general_configurations_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_IPS_GENERAL_CONFIGURATIONS_REG_DISCARD_ALL_QDPMSG_ID:
      field = &(regs->ips.ips_general_configurations_reg.discard_all_qdpmsg);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_IPS_GENERAL_CONFIGURATIONS_REG_DISCARD_ALL_CRDT_ID:
      field = &(regs->ips.ips_general_configurations_reg.discard_all_crdt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_IPS_GENERAL_CONFIGURATIONS_REG_DISABLE_STATUS_MSG_GEN_ID:
      field = &(regs->ips.ips_general_configurations_reg.disable_status_msg_gen);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_IPS_GENERAL_CONFIGURATIONS_REG_DISABLE_CREDIT_SURPLUS_ID:
      field = &(regs->ips.ips_general_configurations_reg.disable_credit_surplus);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_IPS_GENERAL_CONFIGURATIONS_REG_FMC_CREDITS_FROM_SCH_ID:
      field = &(regs->ips.ips_general_configurations_reg.fmc_credits_from_sch);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_IPS_GENERAL_CONFIGURATIONS_REG_DIS_OFF_SEND_AFTER_DEQ_ID:
      field = &(regs->ips.ips_general_configurations_reg.dis_off_send_after_deq);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_IPS_GENERAL_CONFIGURATIONS_REG_SEND_FSM_ON_CREDIT_ID:
      field = &(regs->ips.ips_general_configurations_reg.send_fsm_on_credit);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_IPS_GENERAL_CONFIGURATIONS_REG_SEND_NO_CHG_ON_CREDIT_ID:
      field = &(regs->ips.ips_general_configurations_reg.send_no_chg_on_credit);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_IPS_GENERAL_CONFIGURATIONS_REG_SEND_FSM_FOR_BIGGER_MAX_QUEUE_SIZE_ID:
      field = &(regs->ips.ips_general_configurations_reg.send_fsm_for_bigger_max_queue_size);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_IPS_GENERAL_CONFIGURATIONS_REG_SEND_FSM_FOR_MUL_CROSS_DOWN_ID:
      field = &(regs->ips.ips_general_configurations_reg.send_fsm_for_mul_cross_down);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_IPS_GENERAL_CONFIGURATIONS_REG_UPDATE_MAX_QSZ_FROM_LOCAL_ID:
      field = &(regs->ips.ips_general_configurations_reg.update_max_qsz_from_local);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_IPS_GENERAL_CONFIGURATIONS_REG_DIS_AUTO_CREDIT_SEND_MSG_ID:
      field = &(regs->ips.ips_general_configurations_reg.dis_auto_credit_send_msg);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_IPS_GENERAL_CONFIGURATIONS_REG_EN_INT_LP_DQCQ_FC_ID:
      field = &(regs->ips.ips_general_configurations_reg.en_int_lp_dqcq_fc);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_IPS_GENERAL_CONFIGURATIONS_REG_DIS_DEQ_CMDS_ID:
      field = &(regs->ips.ips_general_configurations_reg.dis_deq_cmds);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_IPS_GENERAL_CONFIGURATIONS_REG_IPS_COUNT_BY_TIMER_ID:
      field = &(regs->ips.ips_general_configurations_reg.ips_count_by_timer);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_IPS_GENERAL_CONFIGURATIONS_REG_IPS_INIT_TRIGGER_ID:
      field = &(regs->ips.ips_general_configurations_reg.ips_init_trigger);
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
  soc_sand_os_printf( "ips.ips_general_configurations_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_ips_general_configurations_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_ips_general_configurations_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_IPS_GENERAL_CONFIGURATIONS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_IPS_GENERAL_CONFIGURATIONS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_IPS_GENERAL_CONFIGURATIONS_REG_REGISTER_ID:
      regis = &(regs->ips.ips_general_configurations_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_IPS_GENERAL_CONFIGURATIONS_REG_DISCARD_ALL_QDPMSG_ID:
      field = &(regs->ips.ips_general_configurations_reg.discard_all_qdpmsg);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_IPS_GENERAL_CONFIGURATIONS_REG_DISCARD_ALL_CRDT_ID:
      field = &(regs->ips.ips_general_configurations_reg.discard_all_crdt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_IPS_GENERAL_CONFIGURATIONS_REG_DISABLE_STATUS_MSG_GEN_ID:
      field = &(regs->ips.ips_general_configurations_reg.disable_status_msg_gen);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_IPS_GENERAL_CONFIGURATIONS_REG_DISABLE_CREDIT_SURPLUS_ID:
      field = &(regs->ips.ips_general_configurations_reg.disable_credit_surplus);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_IPS_GENERAL_CONFIGURATIONS_REG_FMC_CREDITS_FROM_SCH_ID:
      field = &(regs->ips.ips_general_configurations_reg.fmc_credits_from_sch);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_IPS_GENERAL_CONFIGURATIONS_REG_DIS_OFF_SEND_AFTER_DEQ_ID:
      field = &(regs->ips.ips_general_configurations_reg.dis_off_send_after_deq);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_IPS_GENERAL_CONFIGURATIONS_REG_SEND_FSM_ON_CREDIT_ID:
      field = &(regs->ips.ips_general_configurations_reg.send_fsm_on_credit);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_IPS_GENERAL_CONFIGURATIONS_REG_SEND_NO_CHG_ON_CREDIT_ID:
      field = &(regs->ips.ips_general_configurations_reg.send_no_chg_on_credit);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_IPS_GENERAL_CONFIGURATIONS_REG_SEND_FSM_FOR_BIGGER_MAX_QUEUE_SIZE_ID:
      field = &(regs->ips.ips_general_configurations_reg.send_fsm_for_bigger_max_queue_size);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_IPS_GENERAL_CONFIGURATIONS_REG_SEND_FSM_FOR_MUL_CROSS_DOWN_ID:
      field = &(regs->ips.ips_general_configurations_reg.send_fsm_for_mul_cross_down);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_IPS_GENERAL_CONFIGURATIONS_REG_UPDATE_MAX_QSZ_FROM_LOCAL_ID:
      field = &(regs->ips.ips_general_configurations_reg.update_max_qsz_from_local);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_IPS_GENERAL_CONFIGURATIONS_REG_DIS_AUTO_CREDIT_SEND_MSG_ID:
      field = &(regs->ips.ips_general_configurations_reg.dis_auto_credit_send_msg);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_IPS_GENERAL_CONFIGURATIONS_REG_EN_INT_LP_DQCQ_FC_ID:
      field = &(regs->ips.ips_general_configurations_reg.en_int_lp_dqcq_fc);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_IPS_GENERAL_CONFIGURATIONS_REG_DIS_DEQ_CMDS_ID:
      field = &(regs->ips.ips_general_configurations_reg.dis_deq_cmds);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_IPS_GENERAL_CONFIGURATIONS_REG_IPS_COUNT_BY_TIMER_ID:
      field = &(regs->ips.ips_general_configurations_reg.ips_count_by_timer);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_IPS_GENERAL_CONFIGURATIONS_REG_IPS_INIT_TRIGGER_ID:
      field = &(regs->ips.ips_general_configurations_reg.ips_init_trigger);
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
  ui_fap21v_acc_ips_interdigitated_mode_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_interdigitated_mode_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_INTERDIGITATED_MODE_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_INTERDIGITATED_MODE_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERDIGITATED_MODE_REG_REGISTER_ID:
      regis = &(regs->ips.interdigitated_mode_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERDIGITATED_MODE_REG_INTERDIGITATED_MODE_ID:
      field = &(regs->ips.interdigitated_mode_reg.interdigitated_mode);
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
  soc_sand_os_printf( "ips.interdigitated_mode_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_interdigitated_mode_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_interdigitated_mode_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_INTERDIGITATED_MODE_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_INTERDIGITATED_MODE_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERDIGITATED_MODE_REG_REGISTER_ID:
      regis = &(regs->ips.interdigitated_mode_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERDIGITATED_MODE_REG_INTERDIGITATED_MODE_ID:
      field = &(regs->ips.interdigitated_mode_reg.interdigitated_mode);
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
  ui_fap21v_acc_ips_fmc_scheduler_configs_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_fmc_scheduler_configs_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FMC_SCHEDULER_CONFIGS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_FMC_SCHEDULER_CONFIGS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_FMC_SCHEDULER_CONFIGS_REG_REGISTER_ID:
      regis = &(regs->ips.fmc_scheduler_configs_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FMC_SCHEDULER_CONFIGS_REG_FMC_MAX_CR_RATE_ID:
      field = &(regs->ips.fmc_scheduler_configs_reg.fmc_max_cr_rate);
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
  soc_sand_os_printf( "ips.fmc_scheduler_configs_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_fmc_scheduler_configs_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_fmc_scheduler_configs_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FMC_SCHEDULER_CONFIGS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_FMC_SCHEDULER_CONFIGS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_FMC_SCHEDULER_CONFIGS_REG_REGISTER_ID:
      regis = &(regs->ips.fmc_scheduler_configs_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FMC_SCHEDULER_CONFIGS_REG_FMC_MAX_CR_RATE_ID:
      field = &(regs->ips.fmc_scheduler_configs_reg.fmc_max_cr_rate);
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
  ui_fap21v_acc_ips_max_cr_bal_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_max_cr_bal_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_MAX_CR_BAL_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_MAX_CR_BAL_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_MAX_CR_BAL_REG_REGISTER_ID:
      regis = &(regs->ips.max_cr_bal_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_MAX_CR_BAL_REG_MAX_CR_BAL_ID:
      field = &(regs->ips.max_cr_bal_reg.max_cr_bal);
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
  soc_sand_os_printf( "ips.max_cr_bal_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_max_cr_bal_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_max_cr_bal_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_MAX_CR_BAL_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_MAX_CR_BAL_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_MAX_CR_BAL_REG_REGISTER_ID:
      regis = &(regs->ips.max_cr_bal_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_MAX_CR_BAL_REG_MAX_CR_BAL_ID:
      field = &(regs->ips.max_cr_bal_reg.max_cr_bal);
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
  ui_fap21v_acc_ips_credit_flow_control_threshold_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_credit_flow_control_threshold_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_CREDIT_FLOW_CONTROL_THRESHOLD_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_CREDIT_FLOW_CONTROL_THRESHOLD_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_CREDIT_FLOW_CONTROL_THRESHOLD_REG_REGISTER_ID:
      regis = &(regs->ips.credit_flow_control_threshold_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_CREDIT_FLOW_CONTROL_THRESHOLD_REG_CRDT_FC_SET_ID:
      field = &(regs->ips.credit_flow_control_threshold_reg.crdt_fc_set);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_CREDIT_FLOW_CONTROL_THRESHOLD_REG_CRDT_FC_RESET_ID:
      field = &(regs->ips.credit_flow_control_threshold_reg.crdt_fc_reset);
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
  soc_sand_os_printf( "ips.credit_flow_control_threshold_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_credit_flow_control_threshold_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_credit_flow_control_threshold_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_CREDIT_FLOW_CONTROL_THRESHOLD_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_CREDIT_FLOW_CONTROL_THRESHOLD_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_CREDIT_FLOW_CONTROL_THRESHOLD_REG_REGISTER_ID:
      regis = &(regs->ips.credit_flow_control_threshold_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_CREDIT_FLOW_CONTROL_THRESHOLD_REG_CRDT_FC_SET_ID:
      field = &(regs->ips.credit_flow_control_threshold_reg.crdt_fc_set);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_CREDIT_FLOW_CONTROL_THRESHOLD_REG_CRDT_FC_RESET_ID:
      field = &(regs->ips.credit_flow_control_threshold_reg.crdt_fc_reset);
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
  ui_fap21v_acc_ips_interrupt_mask_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_interrupt_mask_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_MASK_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_MASK_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_MASK_REG_REGISTER_ID:
      regis = &(regs->ips.interrupt_mask_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_MASK_REG_DQCQ_DEPTH_OVF_MASK_ID:
      field = &(regs->ips.interrupt_mask_reg.dqcq_depth_ovf_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_MASK_REG_EMPTY_DQCQ_WRITE_MASK_ID:
      field = &(regs->ips.interrupt_mask_reg.empty_dqcq_write_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_MASK_REG_ILLEGEL_MESH_DEST_MASK_ID:
      field = &(regs->ips.interrupt_mask_reg.illegel_mesh_dest_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_MASK_REG_CREDIT_LOST_MASK_ID:
      field = &(regs->ips.interrupt_mask_reg.credit_lost_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_MASK_REG_CREDIT_OVERFLOW_MASK_ID:
      field = &(regs->ips.interrupt_mask_reg.credit_overflow_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_MASK_REG_SYSTEM_PORT_QUEUE_SIZE_EXPIRED_MASK_ID:
      field = &(regs->ips.interrupt_mask_reg.system_port_queue_size_expired_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_MASK_REG_FLWID_ECC_ERR_MASK_ID:
      field = &(regs->ips.interrupt_mask_reg.flwid_ecc_err_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_MASK_REG_QTYPE_ECC_ERR_MASK_ID:
      field = &(regs->ips.interrupt_mask_reg.qtype_ecc_err_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_MASK_REG_QDESC_ECC_ERR_MASK_ID:
      field = &(regs->ips.interrupt_mask_reg.qdesc_ecc_err_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_MASK_REG_QSZ_ECC_ERR_MASK_ID:
      field = &(regs->ips.interrupt_mask_reg.qsz_ecc_err_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_MASK_REG_FSMRQCTRL_ECC_ERR_MASK_ID:
      field = &(regs->ips.interrupt_mask_reg.fsmrqctrl_ecc_err_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_MASK_REG_DQCQMEM_ECC_ERR_MASK_ID:
      field = &(regs->ips.interrupt_mask_reg.dqcqmem_ecc_err_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_MASK_REG_FSMRQMEM_ECC_ERR_MASK_ID:
      field = &(regs->ips.interrupt_mask_reg.fsmrqmem_ecc_err_mask);
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
  soc_sand_os_printf( "ips.interrupt_mask_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_interrupt_mask_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_interrupt_mask_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_MASK_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_MASK_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_MASK_REG_REGISTER_ID:
      regis = &(regs->ips.interrupt_mask_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_MASK_REG_DQCQ_DEPTH_OVF_MASK_ID:
      field = &(regs->ips.interrupt_mask_reg.dqcq_depth_ovf_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_MASK_REG_EMPTY_DQCQ_WRITE_MASK_ID:
      field = &(regs->ips.interrupt_mask_reg.empty_dqcq_write_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_MASK_REG_ILLEGEL_MESH_DEST_MASK_ID:
      field = &(regs->ips.interrupt_mask_reg.illegel_mesh_dest_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_MASK_REG_CREDIT_LOST_MASK_ID:
      field = &(regs->ips.interrupt_mask_reg.credit_lost_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_MASK_REG_CREDIT_OVERFLOW_MASK_ID:
      field = &(regs->ips.interrupt_mask_reg.credit_overflow_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_MASK_REG_SYSTEM_PORT_QUEUE_SIZE_EXPIRED_MASK_ID:
      field = &(regs->ips.interrupt_mask_reg.system_port_queue_size_expired_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_MASK_REG_FLWID_ECC_ERR_MASK_ID:
      field = &(regs->ips.interrupt_mask_reg.flwid_ecc_err_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_MASK_REG_QTYPE_ECC_ERR_MASK_ID:
      field = &(regs->ips.interrupt_mask_reg.qtype_ecc_err_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_MASK_REG_QDESC_ECC_ERR_MASK_ID:
      field = &(regs->ips.interrupt_mask_reg.qdesc_ecc_err_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_MASK_REG_QSZ_ECC_ERR_MASK_ID:
      field = &(regs->ips.interrupt_mask_reg.qsz_ecc_err_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_MASK_REG_FSMRQCTRL_ECC_ERR_MASK_ID:
      field = &(regs->ips.interrupt_mask_reg.fsmrqctrl_ecc_err_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_MASK_REG_DQCQMEM_ECC_ERR_MASK_ID:
      field = &(regs->ips.interrupt_mask_reg.dqcqmem_ecc_err_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_MASK_REG_FSMRQMEM_ECC_ERR_MASK_ID:
      field = &(regs->ips.interrupt_mask_reg.fsmrqmem_ecc_err_mask);
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
  ui_fap21v_acc_ips_fmc3_credit_counter_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_fmc3_credit_counter_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FMC3_CREDIT_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_FMC3_CREDIT_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_FMC3_CREDIT_COUNTER_REG_REGISTER_ID:
      regis = &(regs->ips.fmc3_credit_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FMC3_CREDIT_COUNTER_REG_FMC3_CREDIT_COUNTER_ID:
      field = &(regs->ips.fmc3_credit_counter_reg.fmc3_credit_counter);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FMC3_CREDIT_COUNTER_REG_FMC3_CREDIT_COUNTER_OVERFLOW_ID:
      field = &(regs->ips.fmc3_credit_counter_reg.fmc3_credit_counter_overflow);
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
  soc_sand_os_printf( "ips.fmc3_credit_counter_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_fmc3_credit_counter_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_fmc3_credit_counter_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FMC3_CREDIT_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_FMC3_CREDIT_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_FMC3_CREDIT_COUNTER_REG_REGISTER_ID:
      regis = &(regs->ips.fmc3_credit_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FMC3_CREDIT_COUNTER_REG_FMC3_CREDIT_COUNTER_ID:
      field = &(regs->ips.fmc3_credit_counter_reg.fmc3_credit_counter);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FMC3_CREDIT_COUNTER_REG_FMC3_CREDIT_COUNTER_OVERFLOW_ID:
      field = &(regs->ips.fmc3_credit_counter_reg.fmc3_credit_counter_overflow);
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
  ui_fap21v_acc_ips_flow_status_filter_mask_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_flow_status_filter_mask_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FLOW_STATUS_FILTER_MASK_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_FLOW_STATUS_FILTER_MASK_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_FLOW_STATUS_FILTER_MASK_REG_REGISTER_ID:
      regis = &(regs->ips.flow_status_filter_mask_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FLOW_STATUS_FILTER_MASK_REG_FS_DEST_PORT_ID_MASK_ID:
      field = &(regs->ips.flow_status_filter_mask_reg.fs_dest_port_id_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FLOW_STATUS_FILTER_MASK_REG_FS_DEST_DEV_ID_MASK_ID:
      field = &(regs->ips.flow_status_filter_mask_reg.fs_dest_dev_id_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FLOW_STATUS_FILTER_MASK_REG_FLOW_STATUS_FILTER_MASK_ID:
      field = &(regs->ips.flow_status_filter_mask_reg.flow_status_filter_mask);
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
  soc_sand_os_printf( "ips.flow_status_filter_mask_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_flow_status_filter_mask_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_flow_status_filter_mask_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FLOW_STATUS_FILTER_MASK_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_FLOW_STATUS_FILTER_MASK_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_FLOW_STATUS_FILTER_MASK_REG_REGISTER_ID:
      regis = &(regs->ips.flow_status_filter_mask_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FLOW_STATUS_FILTER_MASK_REG_FS_DEST_PORT_ID_MASK_ID:
      field = &(regs->ips.flow_status_filter_mask_reg.fs_dest_port_id_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FLOW_STATUS_FILTER_MASK_REG_FS_DEST_DEV_ID_MASK_ID:
      field = &(regs->ips.flow_status_filter_mask_reg.fs_dest_dev_id_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FLOW_STATUS_FILTER_MASK_REG_FLOW_STATUS_FILTER_MASK_ID:
      field = &(regs->ips.flow_status_filter_mask_reg.flow_status_filter_mask);
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
  ui_fap21v_acc_ips_ingress_shape_scheduler_config_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_ingress_shape_scheduler_config_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_INGRESS_SHAPE_SCHEDULER_CONFIG_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_INGRESS_SHAPE_SCHEDULER_CONFIG_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_INGRESS_SHAPE_SCHEDULER_CONFIG_REG_REGISTER_ID:
      regis = &(regs->ips.ingress_shape_scheduler_config_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INGRESS_SHAPE_SCHEDULER_CONFIG_REG_ISS_MAX_CR_RATE_ID:
      field = &(regs->ips.ingress_shape_scheduler_config_reg.iss_max_cr_rate);
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
  soc_sand_os_printf( "ips.ingress_shape_scheduler_config_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_ingress_shape_scheduler_config_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_ingress_shape_scheduler_config_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_INGRESS_SHAPE_SCHEDULER_CONFIG_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_INGRESS_SHAPE_SCHEDULER_CONFIG_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_INGRESS_SHAPE_SCHEDULER_CONFIG_REG_REGISTER_ID:
      regis = &(regs->ips.ingress_shape_scheduler_config_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INGRESS_SHAPE_SCHEDULER_CONFIG_REG_ISS_MAX_CR_RATE_ID:
      field = &(regs->ips.ingress_shape_scheduler_config_reg.iss_max_cr_rate);
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
  ui_fap21v_acc_ips_indirect_command_rd_data_reg_1_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_indirect_command_rd_data_reg_1_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_RD_DATA_REG_1_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_RD_DATA_REG_1_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_RD_DATA_REG_1_REGISTER_ID:
      regis = &(regs->ips.indirect_command_rd_data_reg_1.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_RD_DATA_REG_1_INDIRECT_COMMAND_RD_DATA_ID:
      field = &(regs->ips.indirect_command_rd_data_reg_1.indirect_command_rd_data);
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
  soc_sand_os_printf( "ips.indirect_command_rd_data_reg_1: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_indirect_command_rd_data_reg_1_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_indirect_command_rd_data_reg_1_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_RD_DATA_REG_1_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_RD_DATA_REG_1_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_RD_DATA_REG_1_REGISTER_ID:
      regis = &(regs->ips.indirect_command_rd_data_reg_1.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_RD_DATA_REG_1_INDIRECT_COMMAND_RD_DATA_ID:
      field = &(regs->ips.indirect_command_rd_data_reg_1.indirect_command_rd_data);
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
  ui_fap21v_acc_ips_deq_cmd_byte_counter_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_deq_cmd_byte_counter_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_DEQ_CMD_BYTE_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_DEQ_CMD_BYTE_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_DEQ_CMD_BYTE_COUNTER_REG_REGISTER_ID:
      regis = &(regs->ips.deq_cmd_byte_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_DEQ_CMD_BYTE_COUNTER_REG_DEQ_CMD_BYTE_COUNTER_ID:
      field = &(regs->ips.deq_cmd_byte_counter_reg.deq_cmd_byte_counter);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_DEQ_CMD_BYTE_COUNTER_REG_DEQ_CMD_BYTE_COUNTER_OVERFLOW_ID:
      field = &(regs->ips.deq_cmd_byte_counter_reg.deq_cmd_byte_counter_overflow);
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
  soc_sand_os_printf( "ips.deq_cmd_byte_counter_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_deq_cmd_byte_counter_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_deq_cmd_byte_counter_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_DEQ_CMD_BYTE_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_DEQ_CMD_BYTE_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_DEQ_CMD_BYTE_COUNTER_REG_REGISTER_ID:
      regis = &(regs->ips.deq_cmd_byte_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_DEQ_CMD_BYTE_COUNTER_REG_DEQ_CMD_BYTE_COUNTER_ID:
      field = &(regs->ips.deq_cmd_byte_counter_reg.deq_cmd_byte_counter);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_DEQ_CMD_BYTE_COUNTER_REG_DEQ_CMD_BYTE_COUNTER_OVERFLOW_ID:
      field = &(regs->ips.deq_cmd_byte_counter_reg.deq_cmd_byte_counter_overflow);
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
  ui_fap21v_acc_ips_manual_queue_operation_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_manual_queue_operation_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_MANUAL_QUEUE_OPERATION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_MANUAL_QUEUE_OPERATION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_MANUAL_QUEUE_OPERATION_REG_REGISTER_ID:
      regis = &(regs->ips.manual_queue_operation_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_MANUAL_QUEUE_OPERATION_REG_MAN_QUEUE_ID_ID:
      field = &(regs->ips.manual_queue_operation_reg.man_queue_id);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_MANUAL_QUEUE_OPERATION_REG_DEQ_CMD_CR2SEND_ID:
      field = &(regs->ips.manual_queue_operation_reg.deq_cmd_cr2send);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_MANUAL_QUEUE_OPERATION_REG_FS_VALUE_ID:
      field = &(regs->ips.manual_queue_operation_reg.fs_value);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_MANUAL_QUEUE_OPERATION_REG_FS_OVERRIDE_ID:
      field = &(regs->ips.manual_queue_operation_reg.fs_override);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_MANUAL_QUEUE_OPERATION_REG_DEL_DQCQ_NOT_VALID_FOR_FAP21_ID:
      field = &(regs->ips.manual_queue_operation_reg.del_dqcq_not_valid_for_fap21);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_MANUAL_QUEUE_OPERATION_REG_FS_TRIGGER_ID:
      field = &(regs->ips.manual_queue_operation_reg.fs_trigger);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_MANUAL_QUEUE_OPERATION_REG_GRANT_CREDIT_ID:
      field = &(regs->ips.manual_queue_operation_reg.grant_credit);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_MANUAL_QUEUE_OPERATION_REG_FLUSH_TRIGGER_NOT_VALID_FOR_FAP21_ID:
      field = &(regs->ips.manual_queue_operation_reg.flush_trigger_not_valid_for_fap21);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_MANUAL_QUEUE_OPERATION_REG_SEND_PKT_TRIGGER_ID:
      field = &(regs->ips.manual_queue_operation_reg.send_pkt_trigger);
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
  soc_sand_os_printf( "ips.manual_queue_operation_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_manual_queue_operation_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_manual_queue_operation_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_MANUAL_QUEUE_OPERATION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_MANUAL_QUEUE_OPERATION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_MANUAL_QUEUE_OPERATION_REG_REGISTER_ID:
      regis = &(regs->ips.manual_queue_operation_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_MANUAL_QUEUE_OPERATION_REG_MAN_QUEUE_ID_ID:
      field = &(regs->ips.manual_queue_operation_reg.man_queue_id);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_MANUAL_QUEUE_OPERATION_REG_DEQ_CMD_CR2SEND_ID:
      field = &(regs->ips.manual_queue_operation_reg.deq_cmd_cr2send);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_MANUAL_QUEUE_OPERATION_REG_FS_VALUE_ID:
      field = &(regs->ips.manual_queue_operation_reg.fs_value);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_MANUAL_QUEUE_OPERATION_REG_FS_OVERRIDE_ID:
      field = &(regs->ips.manual_queue_operation_reg.fs_override);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_MANUAL_QUEUE_OPERATION_REG_DEL_DQCQ_NOT_VALID_FOR_FAP21_ID:
      field = &(regs->ips.manual_queue_operation_reg.del_dqcq_not_valid_for_fap21);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_MANUAL_QUEUE_OPERATION_REG_FS_TRIGGER_ID:
      field = &(regs->ips.manual_queue_operation_reg.fs_trigger);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_MANUAL_QUEUE_OPERATION_REG_GRANT_CREDIT_ID:
      field = &(regs->ips.manual_queue_operation_reg.grant_credit);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_MANUAL_QUEUE_OPERATION_REG_FLUSH_TRIGGER_NOT_VALID_FOR_FAP21_ID:
      field = &(regs->ips.manual_queue_operation_reg.flush_trigger_not_valid_for_fap21);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_MANUAL_QUEUE_OPERATION_REG_SEND_PKT_TRIGGER_ID:
      field = &(regs->ips.manual_queue_operation_reg.send_pkt_trigger);
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
  ui_fap21v_acc_ips_auto_credit_mechanism_queue_boundaries_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_auto_credit_mechanism_queue_boundaries_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_AUTO_CREDIT_MECHANISM_QUEUE_BOUNDARIES_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_AUTO_CREDIT_MECHANISM_QUEUE_BOUNDARIES_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_AUTO_CREDIT_MECHANISM_QUEUE_BOUNDARIES_REG_REGISTER_ID:
      regis = &(regs->ips.auto_credit_mechanism_queue_boundaries_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_AUTO_CREDIT_MECHANISM_QUEUE_BOUNDARIES_REG_AUTO_CR_FRST_QUE_ID:
      field = &(regs->ips.auto_credit_mechanism_queue_boundaries_reg.auto_cr_frst_que);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_AUTO_CREDIT_MECHANISM_QUEUE_BOUNDARIES_REG_AUTO_CR_LAST_QUE_ID:
      field = &(regs->ips.auto_credit_mechanism_queue_boundaries_reg.auto_cr_last_que);
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
  soc_sand_os_printf( "ips.auto_credit_mechanism_queue_boundaries_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_auto_credit_mechanism_queue_boundaries_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_auto_credit_mechanism_queue_boundaries_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_AUTO_CREDIT_MECHANISM_QUEUE_BOUNDARIES_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_AUTO_CREDIT_MECHANISM_QUEUE_BOUNDARIES_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_AUTO_CREDIT_MECHANISM_QUEUE_BOUNDARIES_REG_REGISTER_ID:
      regis = &(regs->ips.auto_credit_mechanism_queue_boundaries_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_AUTO_CREDIT_MECHANISM_QUEUE_BOUNDARIES_REG_AUTO_CR_FRST_QUE_ID:
      field = &(regs->ips.auto_credit_mechanism_queue_boundaries_reg.auto_cr_frst_que);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_AUTO_CREDIT_MECHANISM_QUEUE_BOUNDARIES_REG_AUTO_CR_LAST_QUE_ID:
      field = &(regs->ips.auto_credit_mechanism_queue_boundaries_reg.auto_cr_last_que);
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
  ui_fap21v_acc_ips_fmc1_credit_counter_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_fmc1_credit_counter_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FMC1_CREDIT_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_FMC1_CREDIT_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_FMC1_CREDIT_COUNTER_REG_REGISTER_ID:
      regis = &(regs->ips.fmc1_credit_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FMC1_CREDIT_COUNTER_REG_FMC1_CREDIT_COUNTER_ID:
      field = &(regs->ips.fmc1_credit_counter_reg.fmc1_credit_counter);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FMC1_CREDIT_COUNTER_REG_FMC1_CREDIT_COUNTER_OVERFLOW_ID:
      field = &(regs->ips.fmc1_credit_counter_reg.fmc1_credit_counter_overflow);
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
  soc_sand_os_printf( "ips.fmc1_credit_counter_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_fmc1_credit_counter_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_fmc1_credit_counter_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FMC1_CREDIT_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_FMC1_CREDIT_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_FMC1_CREDIT_COUNTER_REG_REGISTER_ID:
      regis = &(regs->ips.fmc1_credit_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FMC1_CREDIT_COUNTER_REG_FMC1_CREDIT_COUNTER_ID:
      field = &(regs->ips.fmc1_credit_counter_reg.fmc1_credit_counter);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FMC1_CREDIT_COUNTER_REG_FMC1_CREDIT_COUNTER_OVERFLOW_ID:
      field = &(regs->ips.fmc1_credit_counter_reg.fmc1_credit_counter_overflow);
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
  ui_fap21v_acc_ips_fsm_scrubber_event_counter_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_fsm_scrubber_event_counter_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FSM_SCRUBBER_EVENT_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_FSM_SCRUBBER_EVENT_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_FSM_SCRUBBER_EVENT_COUNTER_REG_REGISTER_ID:
      regis = &(regs->ips.fsm_scrubber_event_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FSM_SCRUBBER_EVENT_COUNTER_REG_SCRUBBER_COUNTER_ID:
      field = &(regs->ips.fsm_scrubber_event_counter_reg.scrubber_counter);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FSM_SCRUBBER_EVENT_COUNTER_REG_SCRUBBER_COUNTER_OVERFLOW_ID:
      field = &(regs->ips.fsm_scrubber_event_counter_reg.scrubber_counter_overflow);
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
  soc_sand_os_printf( "ips.fsm_scrubber_event_counter_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_fsm_scrubber_event_counter_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_fsm_scrubber_event_counter_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FSM_SCRUBBER_EVENT_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_FSM_SCRUBBER_EVENT_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_FSM_SCRUBBER_EVENT_COUNTER_REG_REGISTER_ID:
      regis = &(regs->ips.fsm_scrubber_event_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FSM_SCRUBBER_EVENT_COUNTER_REG_SCRUBBER_COUNTER_ID:
      field = &(regs->ips.fsm_scrubber_event_counter_reg.scrubber_counter);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FSM_SCRUBBER_EVENT_COUNTER_REG_SCRUBBER_COUNTER_OVERFLOW_ID:
      field = &(regs->ips.fsm_scrubber_event_counter_reg.scrubber_counter_overflow);
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
  ui_fap21v_acc_ips_fmc2_credit_counter_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_fmc2_credit_counter_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FMC2_CREDIT_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_FMC2_CREDIT_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_FMC2_CREDIT_COUNTER_REG_REGISTER_ID:
      regis = &(regs->ips.fmc2_credit_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FMC2_CREDIT_COUNTER_REG_FMC2_CREDIT_COUNTER_ID:
      field = &(regs->ips.fmc2_credit_counter_reg.fmc2_credit_counter);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FMC2_CREDIT_COUNTER_REG_FMC2_CREDIT_COUNTER_OVERFLOW_ID:
      field = &(regs->ips.fmc2_credit_counter_reg.fmc2_credit_counter_overflow);
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
  soc_sand_os_printf( "ips.fmc2_credit_counter_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_fmc2_credit_counter_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_fmc2_credit_counter_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FMC2_CREDIT_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_FMC2_CREDIT_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_FMC2_CREDIT_COUNTER_REG_REGISTER_ID:
      regis = &(regs->ips.fmc2_credit_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FMC2_CREDIT_COUNTER_REG_FMC2_CREDIT_COUNTER_ID:
      field = &(regs->ips.fmc2_credit_counter_reg.fmc2_credit_counter);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FMC2_CREDIT_COUNTER_REG_FMC2_CREDIT_COUNTER_OVERFLOW_ID:
      field = &(regs->ips.fmc2_credit_counter_reg.fmc2_credit_counter_overflow);
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
  ui_fap21v_acc_ips_fms_flow_status_counter_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_fms_flow_status_counter_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FMS_FLOW_STATUS_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_FMS_FLOW_STATUS_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_FMS_FLOW_STATUS_COUNTER_REG_REGISTER_ID:
      regis = &(regs->ips.fms_flow_status_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FMS_FLOW_STATUS_COUNTER_REG_FMS_FLOW_STATUS_COUNTER_ID:
      field = &(regs->ips.fms_flow_status_counter_reg.fms_flow_status_counter);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FMS_FLOW_STATUS_COUNTER_REG_FMS_FLOW_STATUS_COUNTER_OVERFLOW_ID:
      field = &(regs->ips.fms_flow_status_counter_reg.fms_flow_status_counter_overflow);
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
  soc_sand_os_printf( "ips.fms_flow_status_counter_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_fms_flow_status_counter_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_fms_flow_status_counter_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FMS_FLOW_STATUS_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_FMS_FLOW_STATUS_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_FMS_FLOW_STATUS_COUNTER_REG_REGISTER_ID:
      regis = &(regs->ips.fms_flow_status_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FMS_FLOW_STATUS_COUNTER_REG_FMS_FLOW_STATUS_COUNTER_ID:
      field = &(regs->ips.fms_flow_status_counter_reg.fms_flow_status_counter);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FMS_FLOW_STATUS_COUNTER_REG_FMS_FLOW_STATUS_COUNTER_OVERFLOW_ID:
      field = &(regs->ips.fms_flow_status_counter_reg.fms_flow_status_counter_overflow);
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
  ui_fap21v_acc_ips_fsm_scrubber_configuration_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_fsm_scrubber_configuration_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FSM_SCRUBBER_CONFIGURATION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_FSM_SCRUBBER_CONFIGURATION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_FSM_SCRUBBER_CONFIGURATION_REG_REGISTER_ID:
      regis = &(regs->ips.fsm_scrubber_configuration_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FSM_SCRUBBER_CONFIGURATION_REG_SCRUBBER_MAX_FLOW_MSG_GEN_RATE_ID:
      field = &(regs->ips.fsm_scrubber_configuration_reg.scrubber_max_flow_msg_gen_rate);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FSM_SCRUBBER_CONFIGURATION_REG_SCRUBBER_MAX_FLOW_MSG_GEN_BURST_SIZE_ID:
      field = &(regs->ips.fsm_scrubber_configuration_reg.scrubber_max_flow_msg_gen_burst_size);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FSM_SCRUBBER_CONFIGURATION_REG_SCRUBBER_MIN_SCAN_CYCLE_PERIOD_ID:
      field = &(regs->ips.fsm_scrubber_configuration_reg.scrubber_min_scan_cycle_period);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FSM_SCRUBBER_CONFIGURATION_REG_SCRUBBER_GEN_OFF_MSGS_ID:
      field = &(regs->ips.fsm_scrubber_configuration_reg.scrubber_gen_off_msgs);
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
  soc_sand_os_printf( "ips.fsm_scrubber_configuration_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_fsm_scrubber_configuration_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_fsm_scrubber_configuration_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FSM_SCRUBBER_CONFIGURATION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_FSM_SCRUBBER_CONFIGURATION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_FSM_SCRUBBER_CONFIGURATION_REG_REGISTER_ID:
      regis = &(regs->ips.fsm_scrubber_configuration_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FSM_SCRUBBER_CONFIGURATION_REG_SCRUBBER_MAX_FLOW_MSG_GEN_RATE_ID:
      field = &(regs->ips.fsm_scrubber_configuration_reg.scrubber_max_flow_msg_gen_rate);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FSM_SCRUBBER_CONFIGURATION_REG_SCRUBBER_MAX_FLOW_MSG_GEN_BURST_SIZE_ID:
      field = &(regs->ips.fsm_scrubber_configuration_reg.scrubber_max_flow_msg_gen_burst_size);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FSM_SCRUBBER_CONFIGURATION_REG_SCRUBBER_MIN_SCAN_CYCLE_PERIOD_ID:
      field = &(regs->ips.fsm_scrubber_configuration_reg.scrubber_min_scan_cycle_period);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FSM_SCRUBBER_CONFIGURATION_REG_SCRUBBER_GEN_OFF_MSGS_ID:
      field = &(regs->ips.fsm_scrubber_configuration_reg.scrubber_gen_off_msgs);
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
  ui_fap21v_acc_ips_capture_queue_descriptor_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_capture_queue_descriptor_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_CAPTURE_QUEUE_DESCRIPTOR_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_CAPTURE_QUEUE_DESCRIPTOR_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_CAPTURE_QUEUE_DESCRIPTOR_REG_REGISTER_ID:
      regis = &(regs->ips.capture_queue_descriptor_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_CAPTURE_QUEUE_DESCRIPTOR_REG_CPTRD_CR_BAL_ID:
      field = &(regs->ips.capture_queue_descriptor_reg.cptrd_cr_bal);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_CAPTURE_QUEUE_DESCRIPTOR_REG_CPTRD_QSZ_ID:
      field = &(regs->ips.capture_queue_descriptor_reg.cptrd_qsz);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_CAPTURE_QUEUE_DESCRIPTOR_REG_CPTRD_CRS_ID:
      field = &(regs->ips.capture_queue_descriptor_reg.cptrd_crs);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_CAPTURE_QUEUE_DESCRIPTOR_REG_CPTRD_FSMRQ_CTRL_ID:
      field = &(regs->ips.capture_queue_descriptor_reg.cptrd_fsmrq_ctrl);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_CAPTURE_QUEUE_DESCRIPTOR_REG_CPTRD_IN_DQCQ_ID:
      field = &(regs->ips.capture_queue_descriptor_reg.cptrd_in_dqcq);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_CAPTURE_QUEUE_DESCRIPTOR_REG_CPTRD_ONE_PKT_DEQ_ID:
      field = &(regs->ips.capture_queue_descriptor_reg.cptrd_one_pkt_deq);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_CAPTURE_QUEUE_DESCRIPTOR_REG_CPTRD_VALID_ID:
      field = &(regs->ips.capture_queue_descriptor_reg.cptrd_valid);
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
  soc_sand_os_printf( "ips.capture_queue_descriptor_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_capture_queue_descriptor_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_capture_queue_descriptor_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_CAPTURE_QUEUE_DESCRIPTOR_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_CAPTURE_QUEUE_DESCRIPTOR_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_CAPTURE_QUEUE_DESCRIPTOR_REG_REGISTER_ID:
      regis = &(regs->ips.capture_queue_descriptor_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_CAPTURE_QUEUE_DESCRIPTOR_REG_CPTRD_CR_BAL_ID:
      field = &(regs->ips.capture_queue_descriptor_reg.cptrd_cr_bal);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_CAPTURE_QUEUE_DESCRIPTOR_REG_CPTRD_QSZ_ID:
      field = &(regs->ips.capture_queue_descriptor_reg.cptrd_qsz);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_CAPTURE_QUEUE_DESCRIPTOR_REG_CPTRD_CRS_ID:
      field = &(regs->ips.capture_queue_descriptor_reg.cptrd_crs);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_CAPTURE_QUEUE_DESCRIPTOR_REG_CPTRD_FSMRQ_CTRL_ID:
      field = &(regs->ips.capture_queue_descriptor_reg.cptrd_fsmrq_ctrl);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_CAPTURE_QUEUE_DESCRIPTOR_REG_CPTRD_IN_DQCQ_ID:
      field = &(regs->ips.capture_queue_descriptor_reg.cptrd_in_dqcq);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_CAPTURE_QUEUE_DESCRIPTOR_REG_CPTRD_ONE_PKT_DEQ_ID:
      field = &(regs->ips.capture_queue_descriptor_reg.cptrd_one_pkt_deq);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_CAPTURE_QUEUE_DESCRIPTOR_REG_CPTRD_VALID_ID:
      field = &(regs->ips.capture_queue_descriptor_reg.cptrd_valid);
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
  ui_fap21v_acc_ips_flow_control_counter_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_flow_control_counter_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FLOW_CONTROL_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_FLOW_CONTROL_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_FLOW_CONTROL_COUNTER_REG_REGISTER_ID:
      regis = &(regs->ips.flow_control_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FLOW_CONTROL_COUNTER_REG_FC_COUNT_ID:
      field = &(regs->ips.flow_control_counter_reg.fc_count);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FLOW_CONTROL_COUNTER_REG_FC_COUNT_OVERFLOW_ID:
      field = &(regs->ips.flow_control_counter_reg.fc_count_overflow);
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
  soc_sand_os_printf( "ips.flow_control_counter_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_flow_control_counter_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_flow_control_counter_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FLOW_CONTROL_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_FLOW_CONTROL_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_FLOW_CONTROL_COUNTER_REG_REGISTER_ID:
      regis = &(regs->ips.flow_control_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FLOW_CONTROL_COUNTER_REG_FC_COUNT_ID:
      field = &(regs->ips.flow_control_counter_reg.fc_count);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FLOW_CONTROL_COUNTER_REG_FC_COUNT_OVERFLOW_ID:
      field = &(regs->ips.flow_control_counter_reg.fc_count_overflow);
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
  ui_fap21v_acc_ips_indirect_command_address_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_indirect_command_address_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_ADDRESS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_ADDRESS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_ADDRESS_REG_REGISTER_ID:
      regis = &(regs->ips.indirect_command_address_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_ADDRESS_REG_INDIRECT_COMMAND_ADDR_ID:
      field = &(regs->ips.indirect_command_address_reg.indirect_command_addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_ADDRESS_REG_INDIRECT_COMMAND_TYPE_ID:
      field = &(regs->ips.indirect_command_address_reg.indirect_command_type);
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
  soc_sand_os_printf( "ips.indirect_command_address_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_indirect_command_address_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_indirect_command_address_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_ADDRESS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_ADDRESS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_ADDRESS_REG_REGISTER_ID:
      regis = &(regs->ips.indirect_command_address_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_ADDRESS_REG_INDIRECT_COMMAND_ADDR_ID:
      field = &(regs->ips.indirect_command_address_reg.indirect_command_addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_ADDRESS_REG_INDIRECT_COMMAND_TYPE_ID:
      field = &(regs->ips.indirect_command_address_reg.indirect_command_type);
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
  ui_fap21v_acc_ips_active_queue_count_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_active_queue_count_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_ACTIVE_QUEUE_COUNT_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_ACTIVE_QUEUE_COUNT_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_ACTIVE_QUEUE_COUNT_REG_REGISTER_ID:
      regis = &(regs->ips.active_queue_count_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_ACTIVE_QUEUE_COUNT_REG_MAX_ACTIVE_QUEUE_COUNT_ID:
      field = &(regs->ips.active_queue_count_reg.max_active_queue_count);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_ACTIVE_QUEUE_COUNT_REG_ACTIVE_QUEUE_COUNT_ID:
      field = &(regs->ips.active_queue_count_reg.active_queue_count);
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
  soc_sand_os_printf( "ips.active_queue_count_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_active_queue_count_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_active_queue_count_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_ACTIVE_QUEUE_COUNT_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_ACTIVE_QUEUE_COUNT_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_ACTIVE_QUEUE_COUNT_REG_REGISTER_ID:
      regis = &(regs->ips.active_queue_count_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_ACTIVE_QUEUE_COUNT_REG_MAX_ACTIVE_QUEUE_COUNT_ID:
      field = &(regs->ips.active_queue_count_reg.max_active_queue_count);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_ACTIVE_QUEUE_COUNT_REG_ACTIVE_QUEUE_COUNT_ID:
      field = &(regs->ips.active_queue_count_reg.active_queue_count);
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
  ui_fap21v_acc_ips_internal_low_priority_dqcq_flow_control_configuration_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_internal_low_priority_dqcq_flow_control_configuration_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_INTERNAL_LOW_PRIORITY_DQCQ_FLOW_CONTROL_CONFIGURATION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_INTERNAL_LOW_PRIORITY_DQCQ_FLOW_CONTROL_CONFIGURATION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERNAL_LOW_PRIORITY_DQCQ_FLOW_CONTROL_CONFIGURATION_REG_REGISTER_ID:
      regis = &(regs->ips.internal_low_priority_dqcq_flow_control_configuration_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERNAL_LOW_PRIORITY_DQCQ_FLOW_CONTROL_CONFIGURATION_REG_INT_LP_DQCQ_FC_SET_TH_ID:
      field = &(regs->ips.internal_low_priority_dqcq_flow_control_configuration_reg.int_lp_dqcq_fc_set_th);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERNAL_LOW_PRIORITY_DQCQ_FLOW_CONTROL_CONFIGURATION_REG_INT_LP_DQCQ_FC_RESET_TH_ID:
      field = &(regs->ips.internal_low_priority_dqcq_flow_control_configuration_reg.int_lp_dqcq_fc_reset_th);
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
  soc_sand_os_printf( "ips.internal_low_priority_dqcq_flow_control_configuration_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_internal_low_priority_dqcq_flow_control_configuration_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_internal_low_priority_dqcq_flow_control_configuration_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_INTERNAL_LOW_PRIORITY_DQCQ_FLOW_CONTROL_CONFIGURATION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_INTERNAL_LOW_PRIORITY_DQCQ_FLOW_CONTROL_CONFIGURATION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERNAL_LOW_PRIORITY_DQCQ_FLOW_CONTROL_CONFIGURATION_REG_REGISTER_ID:
      regis = &(regs->ips.internal_low_priority_dqcq_flow_control_configuration_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERNAL_LOW_PRIORITY_DQCQ_FLOW_CONTROL_CONFIGURATION_REG_INT_LP_DQCQ_FC_SET_TH_ID:
      field = &(regs->ips.internal_low_priority_dqcq_flow_control_configuration_reg.int_lp_dqcq_fc_set_th);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INTERNAL_LOW_PRIORITY_DQCQ_FLOW_CONTROL_CONFIGURATION_REG_INT_LP_DQCQ_FC_RESET_TH_ID:
      field = &(regs->ips.internal_low_priority_dqcq_flow_control_configuration_reg.int_lp_dqcq_fc_reset_th);
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
  ui_fap21v_acc_ips_indirect_command_wr_data_reg_1_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_indirect_command_wr_data_reg_1_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_WR_DATA_REG_1_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_WR_DATA_REG_1_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_WR_DATA_REG_1_REGISTER_ID:
      regis = &(regs->ips.indirect_command_wr_data_reg_1.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_WR_DATA_REG_1_INDIRECT_COMMAND_WR_DATA_ID:
      field = &(regs->ips.indirect_command_wr_data_reg_1.indirect_command_wr_data);
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
  soc_sand_os_printf( "ips.indirect_command_wr_data_reg_1: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_indirect_command_wr_data_reg_1_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_indirect_command_wr_data_reg_1_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_WR_DATA_REG_1_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_WR_DATA_REG_1_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_WR_DATA_REG_1_REGISTER_ID:
      regis = &(regs->ips.indirect_command_wr_data_reg_1.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_WR_DATA_REG_1_INDIRECT_COMMAND_WR_DATA_ID:
      field = &(regs->ips.indirect_command_wr_data_reg_1.indirect_command_wr_data);
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
  ui_fap21v_acc_ips_fms_delay_counter_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_fms_delay_counter_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FMS_DELAY_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_FMS_DELAY_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_FMS_DELAY_COUNTER_REG_REGISTER_ID:
      regis = &(regs->ips.fms_delay_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FMS_DELAY_COUNTER_REG_FMS_DELAY_COUNTER_ID:
      field = &(regs->ips.fms_delay_counter_reg.fms_delay_counter);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FMS_DELAY_COUNTER_REG_FMS_DELAY_QUEUE_ID:
      field = &(regs->ips.fms_delay_counter_reg.fms_delay_queue);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FMS_DELAY_COUNTER_REG_FMS_DELAY_BY_QUEUE_ID:
      field = &(regs->ips.fms_delay_counter_reg.fms_delay_by_queue);
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
  soc_sand_os_printf( "ips.fms_delay_counter_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_fms_delay_counter_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_fms_delay_counter_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FMS_DELAY_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_FMS_DELAY_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_FMS_DELAY_COUNTER_REG_REGISTER_ID:
      regis = &(regs->ips.fms_delay_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FMS_DELAY_COUNTER_REG_FMS_DELAY_COUNTER_ID:
      field = &(regs->ips.fms_delay_counter_reg.fms_delay_counter);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FMS_DELAY_COUNTER_REG_FMS_DELAY_QUEUE_ID:
      field = &(regs->ips.fms_delay_counter_reg.fms_delay_queue);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FMS_DELAY_COUNTER_REG_FMS_DELAY_BY_QUEUE_ID:
      field = &(regs->ips.fms_delay_counter_reg.fms_delay_by_queue);
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
  ui_fap21v_acc_ips_fsm_scrubber_queue_boundaries_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_fsm_scrubber_queue_boundaries_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FSM_SCRUBBER_QUEUE_BOUNDARIES_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_FSM_SCRUBBER_QUEUE_BOUNDARIES_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_FSM_SCRUBBER_QUEUE_BOUNDARIES_REG_REGISTER_ID:
      regis = &(regs->ips.fsm_scrubber_queue_boundaries_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FSM_SCRUBBER_QUEUE_BOUNDARIES_REG_SCRUBBER_BOTTOM_Q_ID:
      field = &(regs->ips.fsm_scrubber_queue_boundaries_reg.scrubber_bottom_q);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FSM_SCRUBBER_QUEUE_BOUNDARIES_REG_SCRUBBER_TOP_Q_ID:
      field = &(regs->ips.fsm_scrubber_queue_boundaries_reg.scrubber_top_q);
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
  soc_sand_os_printf( "ips.fsm_scrubber_queue_boundaries_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_fsm_scrubber_queue_boundaries_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_fsm_scrubber_queue_boundaries_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FSM_SCRUBBER_QUEUE_BOUNDARIES_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_FSM_SCRUBBER_QUEUE_BOUNDARIES_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_FSM_SCRUBBER_QUEUE_BOUNDARIES_REG_REGISTER_ID:
      regis = &(regs->ips.fsm_scrubber_queue_boundaries_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FSM_SCRUBBER_QUEUE_BOUNDARIES_REG_SCRUBBER_BOTTOM_Q_ID:
      field = &(regs->ips.fsm_scrubber_queue_boundaries_reg.scrubber_bottom_q);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FSM_SCRUBBER_QUEUE_BOUNDARIES_REG_SCRUBBER_TOP_Q_ID:
      field = &(regs->ips.fsm_scrubber_queue_boundaries_reg.scrubber_top_q);
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
  ui_fap21v_acc_ips_programmable_qdp_report_counter_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_programmable_qdp_report_counter_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_PROGRAMMABLE_QDP_REPORT_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_PROGRAMMABLE_QDP_REPORT_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_PROGRAMMABLE_QDP_REPORT_COUNTER_REG_REGISTER_ID:
      regis = &(regs->ips.programmable_qdp_report_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_PROGRAMMABLE_QDP_REPORT_COUNTER_REG_PRG_QDP_RPRT_COUNTER_ID:
      field = &(regs->ips.programmable_qdp_report_counter_reg.prg_qdp_rprt_counter);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_PROGRAMMABLE_QDP_REPORT_COUNTER_REG_PRG_QDP_RPRT_COUNTER_OVERFLOW_ID:
      field = &(regs->ips.programmable_qdp_report_counter_reg.prg_qdp_rprt_counter_overflow);
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
  soc_sand_os_printf( "ips.programmable_qdp_report_counter_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_programmable_qdp_report_counter_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_programmable_qdp_report_counter_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_PROGRAMMABLE_QDP_REPORT_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_PROGRAMMABLE_QDP_REPORT_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_PROGRAMMABLE_QDP_REPORT_COUNTER_REG_REGISTER_ID:
      regis = &(regs->ips.programmable_qdp_report_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_PROGRAMMABLE_QDP_REPORT_COUNTER_REG_PRG_QDP_RPRT_COUNTER_ID:
      field = &(regs->ips.programmable_qdp_report_counter_reg.prg_qdp_rprt_counter);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_PROGRAMMABLE_QDP_REPORT_COUNTER_REG_PRG_QDP_RPRT_COUNTER_OVERFLOW_ID:
      field = &(regs->ips.programmable_qdp_report_counter_reg.prg_qdp_rprt_counter_overflow);
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
  ui_fap21v_acc_ips_indirect_command_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_indirect_command_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_REG_REGISTER_ID:
      regis = &(regs->ips.indirect_command_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_REG_INDIRECT_COMMAND_TRIGGER_ID:
      field = &(regs->ips.indirect_command_reg.indirect_command_trigger);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_REG_INDIRECT_COMMAND_TRIGGER_ON_DATA_ID:
      field = &(regs->ips.indirect_command_reg.indirect_command_trigger_on_data);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_REG_INDIRECT_COMMAND_COUNT_ID:
      field = &(regs->ips.indirect_command_reg.indirect_command_count);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_REG_INDIRECT_COMMAND_TIMEOUT_ID:
      field = &(regs->ips.indirect_command_reg.indirect_command_timeout);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_REG_INDIRECT_COMMAND_STATUS_ID:
      field = &(regs->ips.indirect_command_reg.indirect_command_status);
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
  soc_sand_os_printf( "ips.indirect_command_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_indirect_command_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_indirect_command_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_REG_REGISTER_ID:
      regis = &(regs->ips.indirect_command_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_REG_INDIRECT_COMMAND_TRIGGER_ID:
      field = &(regs->ips.indirect_command_reg.indirect_command_trigger);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_REG_INDIRECT_COMMAND_TRIGGER_ON_DATA_ID:
      field = &(regs->ips.indirect_command_reg.indirect_command_trigger_on_data);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_REG_INDIRECT_COMMAND_COUNT_ID:
      field = &(regs->ips.indirect_command_reg.indirect_command_count);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_REG_INDIRECT_COMMAND_TIMEOUT_ID:
      field = &(regs->ips.indirect_command_reg.indirect_command_timeout);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_REG_INDIRECT_COMMAND_STATUS_ID:
      field = &(regs->ips.indirect_command_reg.indirect_command_status);
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
  ui_fap21v_acc_ips_dqcq_max_occupancy_hp_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_dqcq_max_occupancy_hp_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_DQCQ_MAX_OCCUPANCY_HP_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_DQCQ_MAX_OCCUPANCY_HP_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_DQCQ_MAX_OCCUPANCY_HP_REG_REGISTER_ID:
      regis = &(regs->ips.dqcq_max_occupancy_hp_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_DQCQ_MAX_OCCUPANCY_HP_REG_HP_DQCQ_MAX_OCC_ID:
      field = &(regs->ips.dqcq_max_occupancy_hp_reg.hp_dqcq_max_occ);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_DQCQ_MAX_OCCUPANCY_HP_REG_HP_DQCQ_MAX_OCC_ID_ID:
      field = &(regs->ips.dqcq_max_occupancy_hp_reg.hp_dqcq_max_occ_id);
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
  soc_sand_os_printf( "ips.dqcq_max_occupancy_hp_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_dqcq_max_occupancy_hp_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_dqcq_max_occupancy_hp_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_DQCQ_MAX_OCCUPANCY_HP_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_DQCQ_MAX_OCCUPANCY_HP_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_DQCQ_MAX_OCCUPANCY_HP_REG_REGISTER_ID:
      regis = &(regs->ips.dqcq_max_occupancy_hp_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_DQCQ_MAX_OCCUPANCY_HP_REG_HP_DQCQ_MAX_OCC_ID:
      field = &(regs->ips.dqcq_max_occupancy_hp_reg.hp_dqcq_max_occ);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_DQCQ_MAX_OCCUPANCY_HP_REG_HP_DQCQ_MAX_OCC_ID_ID:
      field = &(regs->ips.dqcq_max_occupancy_hp_reg.hp_dqcq_max_occ_id);
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
  ui_fap21v_acc_ips_fcr_credit_counter_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_fcr_credit_counter_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FCR_CREDIT_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_FCR_CREDIT_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_FCR_CREDIT_COUNTER_REG_REGISTER_ID:
      regis = &(regs->ips.fcr_credit_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FCR_CREDIT_COUNTER_REG_FCR_CREDIT_COUNTER_ID:
      field = &(regs->ips.fcr_credit_counter_reg.fcr_credit_counter);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FCR_CREDIT_COUNTER_REG_FCR_CREDIT_COUNTER_OVERFLOW_ID:
      field = &(regs->ips.fcr_credit_counter_reg.fcr_credit_counter_overflow);
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
  soc_sand_os_printf( "ips.fcr_credit_counter_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_fcr_credit_counter_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_fcr_credit_counter_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FCR_CREDIT_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_FCR_CREDIT_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_FCR_CREDIT_COUNTER_REG_REGISTER_ID:
      regis = &(regs->ips.fcr_credit_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FCR_CREDIT_COUNTER_REG_FCR_CREDIT_COUNTER_ID:
      field = &(regs->ips.fcr_credit_counter_reg.fcr_credit_counter);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FCR_CREDIT_COUNTER_REG_FCR_CREDIT_COUNTER_OVERFLOW_ID:
      field = &(regs->ips.fcr_credit_counter_reg.fcr_credit_counter_overflow);
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
  ui_fap21v_acc_ips_lost_credit_queue_number_error_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_lost_credit_queue_number_error_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_LOST_CREDIT_QUEUE_NUMBER_ERROR_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_LOST_CREDIT_QUEUE_NUMBER_ERROR_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_LOST_CREDIT_QUEUE_NUMBER_ERROR_REG_REGISTER_ID:
      regis = &(regs->ips.lost_credit_queue_number_error_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_LOST_CREDIT_QUEUE_NUMBER_ERROR_REG_LOST_CR_QUEUE_NUM_ID:
      field = &(regs->ips.lost_credit_queue_number_error_reg.lost_cr_queue_num);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_LOST_CREDIT_QUEUE_NUMBER_ERROR_REG_DQCQ_ID_ID:
      field = &(regs->ips.lost_credit_queue_number_error_reg.dqcq_id);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_LOST_CREDIT_QUEUE_NUMBER_ERROR_REG_DQCQ_NRDY_ID:
      field = &(regs->ips.lost_credit_queue_number_error_reg.dqcq_nrdy);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_LOST_CREDIT_QUEUE_NUMBER_ERROR_REG_FMS_NRDY_ID:
      field = &(regs->ips.lost_credit_queue_number_error_reg.fms_nrdy);
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
  soc_sand_os_printf( "ips.lost_credit_queue_number_error_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_lost_credit_queue_number_error_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_lost_credit_queue_number_error_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_LOST_CREDIT_QUEUE_NUMBER_ERROR_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_LOST_CREDIT_QUEUE_NUMBER_ERROR_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_LOST_CREDIT_QUEUE_NUMBER_ERROR_REG_REGISTER_ID:
      regis = &(regs->ips.lost_credit_queue_number_error_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_LOST_CREDIT_QUEUE_NUMBER_ERROR_REG_LOST_CR_QUEUE_NUM_ID:
      field = &(regs->ips.lost_credit_queue_number_error_reg.lost_cr_queue_num);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_LOST_CREDIT_QUEUE_NUMBER_ERROR_REG_DQCQ_ID_ID:
      field = &(regs->ips.lost_credit_queue_number_error_reg.dqcq_id);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_LOST_CREDIT_QUEUE_NUMBER_ERROR_REG_DQCQ_NRDY_ID:
      field = &(regs->ips.lost_credit_queue_number_error_reg.dqcq_nrdy);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_LOST_CREDIT_QUEUE_NUMBER_ERROR_REG_FMS_NRDY_ID:
      field = &(regs->ips.lost_credit_queue_number_error_reg.fms_nrdy);
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
  ui_fap21v_acc_ips_fmc0_credit_counter_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_fmc0_credit_counter_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FMC0_CREDIT_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_FMC0_CREDIT_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_FMC0_CREDIT_COUNTER_REG_REGISTER_ID:
      regis = &(regs->ips.fmc0_credit_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FMC0_CREDIT_COUNTER_REG_FMC0_CREDIT_COUNTER_ID:
      field = &(regs->ips.fmc0_credit_counter_reg.fmc0_credit_counter);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FMC0_CREDIT_COUNTER_REG_FMC0_CREDIT_COUNTER_OVERFLOW_ID:
      field = &(regs->ips.fmc0_credit_counter_reg.fmc0_credit_counter_overflow);
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
  soc_sand_os_printf( "ips.fmc0_credit_counter_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_fmc0_credit_counter_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_fmc0_credit_counter_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FMC0_CREDIT_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_FMC0_CREDIT_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_FMC0_CREDIT_COUNTER_REG_REGISTER_ID:
      regis = &(regs->ips.fmc0_credit_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FMC0_CREDIT_COUNTER_REG_FMC0_CREDIT_COUNTER_ID:
      field = &(regs->ips.fmc0_credit_counter_reg.fmc0_credit_counter);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FMC0_CREDIT_COUNTER_REG_FMC0_CREDIT_COUNTER_OVERFLOW_ID:
      field = &(regs->ips.fmc0_credit_counter_reg.fmc0_credit_counter_overflow);
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
  ui_fap21v_acc_ips_expired_system_port_id_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_expired_system_port_id_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_EXPIRED_SYSTEM_PORT_ID_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_EXPIRED_SYSTEM_PORT_ID_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_EXPIRED_SYSTEM_PORT_ID_REG_REGISTER_ID:
      regis = &(regs->ips.expired_system_port_id_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_EXPIRED_SYSTEM_PORT_ID_REG_EXPIRED_SYSTEM_PORT_ID_ID:
      field = &(regs->ips.expired_system_port_id_reg.expired_system_port_id);
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
  soc_sand_os_printf( "ips.expired_system_port_id_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_expired_system_port_id_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_expired_system_port_id_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_EXPIRED_SYSTEM_PORT_ID_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_EXPIRED_SYSTEM_PORT_ID_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_EXPIRED_SYSTEM_PORT_ID_REG_REGISTER_ID:
      regis = &(regs->ips.expired_system_port_id_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_EXPIRED_SYSTEM_PORT_ID_REG_EXPIRED_SYSTEM_PORT_ID_ID:
      field = &(regs->ips.expired_system_port_id_reg.expired_system_port_id);
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
  ui_fap21v_acc_ips_queue_num_filter_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_queue_num_filter_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_QUEUE_NUM_FILTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_QUEUE_NUM_FILTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_QUEUE_NUM_FILTER_REG_REGISTER_ID:
      regis = &(regs->ips.queue_num_filter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_QUEUE_NUM_FILTER_REG_QUEUE_NUM_FILTER_ID:
      field = &(regs->ips.queue_num_filter_reg.queue_num_filter);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_QUEUE_NUM_FILTER_REG_QUEUE_NUM_FILTER_MASK_ID:
      field = &(regs->ips.queue_num_filter_reg.queue_num_filter_mask);
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
  soc_sand_os_printf( "ips.queue_num_filter_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_queue_num_filter_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_queue_num_filter_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_QUEUE_NUM_FILTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_QUEUE_NUM_FILTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_QUEUE_NUM_FILTER_REG_REGISTER_ID:
      regis = &(regs->ips.queue_num_filter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_QUEUE_NUM_FILTER_REG_QUEUE_NUM_FILTER_ID:
      field = &(regs->ips.queue_num_filter_reg.queue_num_filter);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_QUEUE_NUM_FILTER_REG_QUEUE_NUM_FILTER_MASK_ID:
      field = &(regs->ips.queue_num_filter_reg.queue_num_filter_mask);
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
  ui_fap21v_acc_ips_auto_credit_mechanism_rate_configuration_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_auto_credit_mechanism_rate_configuration_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_AUTO_CREDIT_MECHANISM_RATE_CONFIGURATION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_AUTO_CREDIT_MECHANISM_RATE_CONFIGURATION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_AUTO_CREDIT_MECHANISM_RATE_CONFIGURATION_REG_REGISTER_ID:
      regis = &(regs->ips.auto_credit_mechanism_rate_configuration_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_AUTO_CREDIT_MECHANISM_RATE_CONFIGURATION_REG_AUTO_CR_RATE_ID:
      field = &(regs->ips.auto_credit_mechanism_rate_configuration_reg.auto_cr_rate);
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
  soc_sand_os_printf( "ips.auto_credit_mechanism_rate_configuration_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_auto_credit_mechanism_rate_configuration_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_auto_credit_mechanism_rate_configuration_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_AUTO_CREDIT_MECHANISM_RATE_CONFIGURATION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_AUTO_CREDIT_MECHANISM_RATE_CONFIGURATION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_AUTO_CREDIT_MECHANISM_RATE_CONFIGURATION_REG_REGISTER_ID:
      regis = &(regs->ips.auto_credit_mechanism_rate_configuration_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_AUTO_CREDIT_MECHANISM_RATE_CONFIGURATION_REG_AUTO_CR_RATE_ID:
      field = &(regs->ips.auto_credit_mechanism_rate_configuration_reg.auto_cr_rate);
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
  ui_fap21v_acc_ips_gfmc_shaper_configs_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_gfmc_shaper_configs_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_GFMC_SHAPER_CONFIGS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_GFMC_SHAPER_CONFIGS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_GFMC_SHAPER_CONFIGS_REG_REGISTER_ID:
      regis = &(regs->ips.gfmc_shaper_configs_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_GFMC_SHAPER_CONFIGS_REG_FMC_MAX_CR_RATE_ID:
      field = &(regs->ips.gfmc_shaper_configs_reg.fmc_max_cr_rate);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_GFMC_SHAPER_CONFIGS_REG_FMC_MAX_BURST_ID:
      field = &(regs->ips.gfmc_shaper_configs_reg.fmc_max_burst);
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
  soc_sand_os_printf( "ips.gfmc_shaper_configs_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_gfmc_shaper_configs_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_gfmc_shaper_configs_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_GFMC_SHAPER_CONFIGS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_GFMC_SHAPER_CONFIGS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_GFMC_SHAPER_CONFIGS_REG_REGISTER_ID:
      regis = &(regs->ips.gfmc_shaper_configs_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_GFMC_SHAPER_CONFIGS_REG_FMC_MAX_CR_RATE_ID:
      field = &(regs->ips.gfmc_shaper_configs_reg.fmc_max_cr_rate);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_GFMC_SHAPER_CONFIGS_REG_FMC_MAX_BURST_ID:
      field = &(regs->ips.gfmc_shaper_configs_reg.fmc_max_burst);
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
  ui_fap21v_acc_ips_bfmc_shaper_configs_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_bfmc_shaper_configs_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_BFMC_SHAPER_CONFIGS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_BFMC_SHAPER_CONFIGS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_BFMC_SHAPER_CONFIGS_REG_REGISTER_ID:
      regis = &(regs->ips.bfmc_shaper_configs_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_BFMC_SHAPER_CONFIGS_REG_FMC_MAX_CR_RATE_ID:
      field = &(regs->ips.bfmc_shaper_configs_reg.fmc_max_cr_rate);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_BFMC_SHAPER_CONFIGS_REG_FMC_MAX_BURST_ID:
      field = &(regs->ips.bfmc_shaper_configs_reg.fmc_max_burst);
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
  soc_sand_os_printf( "ips.bfmc_shaper_configs_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_bfmc_shaper_configs_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_bfmc_shaper_configs_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_BFMC_SHAPER_CONFIGS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_BFMC_SHAPER_CONFIGS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_BFMC_SHAPER_CONFIGS_REG_REGISTER_ID:
      regis = &(regs->ips.bfmc_shaper_configs_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_BFMC_SHAPER_CONFIGS_REG_FMC_MAX_CR_RATE_ID:
      field = &(regs->ips.bfmc_shaper_configs_reg.fmc_max_cr_rate);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_BFMC_SHAPER_CONFIGS_REG_FMC_MAX_BURST_ID:
      field = &(regs->ips.bfmc_shaper_configs_reg.fmc_max_burst);
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
  ui_fap21v_acc_ips_fsmrq_flow_status_counter_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_fsmrq_flow_status_counter_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FSMRQ_FLOW_STATUS_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_FSMRQ_FLOW_STATUS_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_FSMRQ_FLOW_STATUS_COUNTER_REG_REGISTER_ID:
      regis = &(regs->ips.fsmrq_flow_status_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FSMRQ_FLOW_STATUS_COUNTER_REG_FSMRQ_FLOW_STATUS_COUNTER_ID:
      field = &(regs->ips.fsmrq_flow_status_counter_reg.fsmrq_flow_status_counter);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FSMRQ_FLOW_STATUS_COUNTER_REG_FSMRQ_FLOW_STATUS_COUNTER_OVERFLOW_ID:
      field = &(regs->ips.fsmrq_flow_status_counter_reg.fsmrq_flow_status_counter_overflow);
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
  soc_sand_os_printf( "ips.fsmrq_flow_status_counter_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_fsmrq_flow_status_counter_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_fsmrq_flow_status_counter_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FSMRQ_FLOW_STATUS_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_FSMRQ_FLOW_STATUS_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_FSMRQ_FLOW_STATUS_COUNTER_REG_REGISTER_ID:
      regis = &(regs->ips.fsmrq_flow_status_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FSMRQ_FLOW_STATUS_COUNTER_REG_FSMRQ_FLOW_STATUS_COUNTER_ID:
      field = &(regs->ips.fsmrq_flow_status_counter_reg.fsmrq_flow_status_counter);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FSMRQ_FLOW_STATUS_COUNTER_REG_FSMRQ_FLOW_STATUS_COUNTER_OVERFLOW_ID:
      field = &(regs->ips.fsmrq_flow_status_counter_reg.fsmrq_flow_status_counter_overflow);
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
  ui_fap21v_acc_ips_fsmrq_delay_counter_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_fsmrq_delay_counter_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FSMRQ_DELAY_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_FSMRQ_DELAY_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_FSMRQ_DELAY_COUNTER_REG_REGISTER_ID:
      regis = &(regs->ips.fsmrq_delay_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FSMRQ_DELAY_COUNTER_REG_FSMRQ_DELAY_COUNTER_ID:
      field = &(regs->ips.fsmrq_delay_counter_reg.fsmrq_delay_counter);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FSMRQ_DELAY_COUNTER_REG_FSMRQ_DELAY_QUEUE_ID:
      field = &(regs->ips.fsmrq_delay_counter_reg.fsmrq_delay_queue);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FSMRQ_DELAY_COUNTER_REG_FSMRQ_DELAY_BY_QUEUE_ID:
      field = &(regs->ips.fsmrq_delay_counter_reg.fsmrq_delay_by_queue);
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
  soc_sand_os_printf( "ips.fsmrq_delay_counter_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_fsmrq_delay_counter_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_fsmrq_delay_counter_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FSMRQ_DELAY_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_FSMRQ_DELAY_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_FSMRQ_DELAY_COUNTER_REG_REGISTER_ID:
      regis = &(regs->ips.fsmrq_delay_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FSMRQ_DELAY_COUNTER_REG_FSMRQ_DELAY_COUNTER_ID:
      field = &(regs->ips.fsmrq_delay_counter_reg.fsmrq_delay_counter);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FSMRQ_DELAY_COUNTER_REG_FSMRQ_DELAY_QUEUE_ID:
      field = &(regs->ips.fsmrq_delay_counter_reg.fsmrq_delay_queue);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FSMRQ_DELAY_COUNTER_REG_FSMRQ_DELAY_BY_QUEUE_ID:
      field = &(regs->ips.fsmrq_delay_counter_reg.fsmrq_delay_by_queue);
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
  ui_fap21v_acc_ips_high_priority_dqcq_depth_config2_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_high_priority_dqcq_depth_config2_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_HIGH_PRIORITY_DQCQ_DEPTH_CONFIG2_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_HIGH_PRIORITY_DQCQ_DEPTH_CONFIG2_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_HIGH_PRIORITY_DQCQ_DEPTH_CONFIG2_REG_REGISTER_ID:
      regis = &(regs->ips.high_priority_dqcq_depth_config2_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_HIGH_PRIORITY_DQCQ_DEPTH_CONFIG2_REG_LOW_DELAY_DEPTH_ID:
      field = &(regs->ips.high_priority_dqcq_depth_config2_reg.low_delay_depth);
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
  soc_sand_os_printf( "ips.high_priority_dqcq_depth_config2_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_high_priority_dqcq_depth_config2_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_high_priority_dqcq_depth_config2_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_HIGH_PRIORITY_DQCQ_DEPTH_CONFIG2_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_HIGH_PRIORITY_DQCQ_DEPTH_CONFIG2_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_HIGH_PRIORITY_DQCQ_DEPTH_CONFIG2_REG_REGISTER_ID:
      regis = &(regs->ips.high_priority_dqcq_depth_config2_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_HIGH_PRIORITY_DQCQ_DEPTH_CONFIG2_REG_LOW_DELAY_DEPTH_ID:
      field = &(regs->ips.high_priority_dqcq_depth_config2_reg.low_delay_depth);
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
  ui_fap21v_acc_ips_dqcq_max_occupancy_lp_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_dqcq_max_occupancy_lp_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_DQCQ_MAX_OCCUPANCY_LP_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_DQCQ_MAX_OCCUPANCY_LP_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_DQCQ_MAX_OCCUPANCY_LP_REG_REGISTER_ID:
      regis = &(regs->ips.dqcq_max_occupancy_lp_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_DQCQ_MAX_OCCUPANCY_LP_REG_LP_DQCQ_MAX_OCC_ID:
      field = &(regs->ips.dqcq_max_occupancy_lp_reg.lp_dqcq_max_occ);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_DQCQ_MAX_OCCUPANCY_LP_REG_LP_DQCQ_MAX_OCC_ID_ID:
      field = &(regs->ips.dqcq_max_occupancy_lp_reg.lp_dqcq_max_occ_id);
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
  soc_sand_os_printf( "ips.dqcq_max_occupancy_lp_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_dqcq_max_occupancy_lp_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_dqcq_max_occupancy_lp_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_DQCQ_MAX_OCCUPANCY_LP_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_DQCQ_MAX_OCCUPANCY_LP_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_DQCQ_MAX_OCCUPANCY_LP_REG_REGISTER_ID:
      regis = &(regs->ips.dqcq_max_occupancy_lp_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_DQCQ_MAX_OCCUPANCY_LP_REG_LP_DQCQ_MAX_OCC_ID:
      field = &(regs->ips.dqcq_max_occupancy_lp_reg.lp_dqcq_max_occ);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_DQCQ_MAX_OCCUPANCY_LP_REG_LP_DQCQ_MAX_OCC_ID_ID:
      field = &(regs->ips.dqcq_max_occupancy_lp_reg.lp_dqcq_max_occ_id);
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
  ui_fap21v_acc_ips_fms_parameters_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_fms_parameters_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FMS_PARAMETERS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_FMS_PARAMETERS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_FMS_PARAMETERS_REG_REGISTER_ID:
      regis = &(regs->ips.fms_parameters_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FMS_PARAMETERS_REG_BYPASS_SHAPER_ID:
      field = &(regs->ips.fms_parameters_reg.bypass_shaper);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FMS_PARAMETERS_REG_DISABLE_SLOW_DELAY_ID:
      field = &(regs->ips.fms_parameters_reg.disable_slow_delay);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FMS_PARAMETERS_REG_LOCAL_ROUTE_BYPASS_ID:
      field = &(regs->ips.fms_parameters_reg.local_route_bypass);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FMS_PARAMETERS_REG_MIN_DLY_ID:
      field = &(regs->ips.fms_parameters_reg.min_dly);
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
  soc_sand_os_printf( "ips.fms_parameters_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_fms_parameters_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_fms_parameters_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FMS_PARAMETERS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_FMS_PARAMETERS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_FMS_PARAMETERS_REG_REGISTER_ID:
      regis = &(regs->ips.fms_parameters_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FMS_PARAMETERS_REG_BYPASS_SHAPER_ID:
      field = &(regs->ips.fms_parameters_reg.bypass_shaper);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FMS_PARAMETERS_REG_DISABLE_SLOW_DELAY_ID:
      field = &(regs->ips.fms_parameters_reg.disable_slow_delay);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FMS_PARAMETERS_REG_LOCAL_ROUTE_BYPASS_ID:
      field = &(regs->ips.fms_parameters_reg.local_route_bypass);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_FMS_PARAMETERS_REG_MIN_DLY_ID:
      field = &(regs->ips.fms_parameters_reg.min_dly);
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
  ui_fap21v_acc_ips_empty_dqcq_id_error_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_empty_dqcq_id_error_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_EMPTY_DQCQ_ID_ERROR_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_EMPTY_DQCQ_ID_ERROR_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_EMPTY_DQCQ_ID_ERROR_REG_REGISTER_ID:
      regis = &(regs->ips.empty_dqcq_id_error_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_EMPTY_DQCQ_ID_ERROR_REG_EMPTY_DQCQ_ID_ID:
      field = &(regs->ips.empty_dqcq_id_error_reg.empty_dqcq_id);
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
  soc_sand_os_printf( "ips.empty_dqcq_id_error_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_empty_dqcq_id_error_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_empty_dqcq_id_error_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_EMPTY_DQCQ_ID_ERROR_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_EMPTY_DQCQ_ID_ERROR_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_EMPTY_DQCQ_ID_ERROR_REG_REGISTER_ID:
      regis = &(regs->ips.empty_dqcq_id_error_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_EMPTY_DQCQ_ID_ERROR_REG_EMPTY_DQCQ_ID_ID:
      field = &(regs->ips.empty_dqcq_id_error_reg.empty_dqcq_id);
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
  ui_fap21v_acc_ips_credit_overflow_queue_num_error_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_credit_overflow_queue_num_error_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_CREDIT_OVERFLOW_QUEUE_NUM_ERROR_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_CREDIT_OVERFLOW_QUEUE_NUM_ERROR_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_CREDIT_OVERFLOW_QUEUE_NUM_ERROR_REG_REGISTER_ID:
      regis = &(regs->ips.credit_overflow_queue_num_error_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_CREDIT_OVERFLOW_QUEUE_NUM_ERROR_REG_CREDIT_OVERFLOW_QUEUE_NUM_ID:
      field = &(regs->ips.credit_overflow_queue_num_error_reg.credit_overflow_queue_num);
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
  soc_sand_os_printf( "ips.credit_overflow_queue_num_error_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_credit_overflow_queue_num_error_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_credit_overflow_queue_num_error_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_CREDIT_OVERFLOW_QUEUE_NUM_ERROR_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_CREDIT_OVERFLOW_QUEUE_NUM_ERROR_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_CREDIT_OVERFLOW_QUEUE_NUM_ERROR_REG_REGISTER_ID:
      regis = &(regs->ips.credit_overflow_queue_num_error_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_CREDIT_OVERFLOW_QUEUE_NUM_ERROR_REG_CREDIT_OVERFLOW_QUEUE_NUM_ID:
      field = &(regs->ips.credit_overflow_queue_num_error_reg.credit_overflow_queue_num);
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
  ui_fap21v_acc_ips_max_fsmrq_req_queues_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_max_fsmrq_req_queues_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_MAX_FSMRQ_REQ_QUEUES_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_MAX_FSMRQ_REQ_QUEUES_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_MAX_FSMRQ_REQ_QUEUES_REG_REGISTER_ID:
      regis = &(regs->ips.max_fsmrq_req_queues_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_MAX_FSMRQ_REQ_QUEUES_REG_MAX_FSMRQ_REQ_QUEUES_ID:
      field = &(regs->ips.max_fsmrq_req_queues_reg.max_fsmrq_req_queues);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_MAX_FSMRQ_REQ_QUEUES_REG_FSMRQ_REQ_QUEUES_ID:
      field = &(regs->ips.max_fsmrq_req_queues_reg.fsmrq_req_queues);
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
  soc_sand_os_printf( "ips.max_fsmrq_req_queues_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_max_fsmrq_req_queues_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips");
  soc_sand_proc_name = "ui_fap21v_acc_ips_max_fsmrq_req_queues_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_MAX_FSMRQ_REQ_QUEUES_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_IPS_MAX_FSMRQ_REQ_QUEUES_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_IPS_MAX_FSMRQ_REQ_QUEUES_REG_REGISTER_ID:
      regis = &(regs->ips.max_fsmrq_req_queues_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_MAX_FSMRQ_REQ_QUEUES_REG_MAX_FSMRQ_REQ_QUEUES_ID:
      field = &(regs->ips.max_fsmrq_req_queues_reg.max_fsmrq_req_queues);
      break;
    case PARAM_FAP21V_ACC_DIRECT_IPS_MAX_FSMRQ_REQ_QUEUES_REG_FSMRQ_REQ_QUEUES_ID:
      field = &(regs->ips.max_fsmrq_req_queues_reg.fsmrq_req_queues);
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
  ui_fap21v_acc_ips_get(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips_get");

  if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_INTERNAL_LOW_PRIORITY_DQCQ_FC_STATUS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_internal_low_priority_dqcq_fc_status_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG3_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_low_priority_dqcq_depth_config3_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_SYSTEM_RED_AGING_CONFIGURATION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_system_red_aging_configuration_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_IPS_CREDIT_CONFIG_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_ips_credit_config_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG2_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_low_priority_dqcq_depth_config2_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG1_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_low_priority_dqcq_depth_config1_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_MAX_PORT_QUEUE_SIZE_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_max_port_queue_size_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_GLOBAL_CREDIT_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_global_credit_counter_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_MASKED_QDP_EVENT_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_masked_qdp_event_counter_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_ILLEGAL_MESH_DESTINATION_QUEUE_NUMBER_ERROR_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_illegal_mesh_destination_queue_number_error_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_DQCQ_ID_FILTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_dqcq_id_filter_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_TIME_IN_SLOW_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_time_in_slow_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FSM_ON_MESSAGE_SHAPER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_fsm_on_message_shaper_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_ACTIVATE_TIMER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_activate_timer_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FLOW_STATUS_FILTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_flow_status_filter_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_GLOBAL_FLOW_STATUS_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_global_flow_status_counter_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG4_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_low_priority_dqcq_depth_config4_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FLOW_CONTROL_COUNT_SELECT_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_flow_control_count_select_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_WR_DATA_REG_0_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_indirect_command_wr_data_reg_0_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_HIGH_PRIORITY_DQCQ_DEPTH_CONFIG1_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_high_priority_dqcq_depth_config1_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_QDP_REPORT_FILTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_qdp_report_filter_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_CREDIT_FABRIC_LATENCY_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_credit_fabric_latency_counter_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_TIME_IN_NORM_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_time_in_norm_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_TIMER_CONFIGURATION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_timer_configuration_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FSMRQ_MAX_OCCUPANCY_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_fsmrq_max_occupancy_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_GLOBAL_QDP_REPORT_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_global_qdp_report_counter_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_RD_DATA_REG_0_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_indirect_command_rd_data_reg_0_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FMS_MAX_OCCUPANCY_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_fms_max_occupancy_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_CAPTURE_QUEUE_DESCRIPTOR_CONFIG_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_capture_queue_descriptor_config_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_interrupt_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_BFMC_CLASS_CONFIGS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_bfmc_class_configs_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_IPS_GENERAL_CONFIGURATIONS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_ips_general_configurations_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_INTERDIGITATED_MODE_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_interdigitated_mode_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FMC_SCHEDULER_CONFIGS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_fmc_scheduler_configs_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_MAX_CR_BAL_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_max_cr_bal_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_CREDIT_FLOW_CONTROL_THRESHOLD_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_credit_flow_control_threshold_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_MASK_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_interrupt_mask_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FMC3_CREDIT_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_fmc3_credit_counter_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FLOW_STATUS_FILTER_MASK_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_flow_status_filter_mask_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_INGRESS_SHAPE_SCHEDULER_CONFIG_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_ingress_shape_scheduler_config_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_RD_DATA_REG_1_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_indirect_command_rd_data_reg_1_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_DEQ_CMD_BYTE_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_deq_cmd_byte_counter_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_MANUAL_QUEUE_OPERATION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_manual_queue_operation_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_AUTO_CREDIT_MECHANISM_QUEUE_BOUNDARIES_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_auto_credit_mechanism_queue_boundaries_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FMC1_CREDIT_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_fmc1_credit_counter_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FSM_SCRUBBER_EVENT_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_fsm_scrubber_event_counter_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FMC2_CREDIT_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_fmc2_credit_counter_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FMS_FLOW_STATUS_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_fms_flow_status_counter_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FSM_SCRUBBER_CONFIGURATION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_fsm_scrubber_configuration_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_CAPTURE_QUEUE_DESCRIPTOR_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_capture_queue_descriptor_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FLOW_CONTROL_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_flow_control_counter_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_ADDRESS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_indirect_command_address_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_ACTIVE_QUEUE_COUNT_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_active_queue_count_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_INTERNAL_LOW_PRIORITY_DQCQ_FLOW_CONTROL_CONFIGURATION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_internal_low_priority_dqcq_flow_control_configuration_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_WR_DATA_REG_1_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_indirect_command_wr_data_reg_1_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FMS_DELAY_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_fms_delay_counter_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FSM_SCRUBBER_QUEUE_BOUNDARIES_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_fsm_scrubber_queue_boundaries_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_PROGRAMMABLE_QDP_REPORT_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_programmable_qdp_report_counter_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_indirect_command_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_DQCQ_MAX_OCCUPANCY_HP_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_dqcq_max_occupancy_hp_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FCR_CREDIT_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_fcr_credit_counter_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_LOST_CREDIT_QUEUE_NUMBER_ERROR_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_lost_credit_queue_number_error_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FMC0_CREDIT_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_fmc0_credit_counter_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_EXPIRED_SYSTEM_PORT_ID_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_expired_system_port_id_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_QUEUE_NUM_FILTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_queue_num_filter_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_AUTO_CREDIT_MECHANISM_RATE_CONFIGURATION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_auto_credit_mechanism_rate_configuration_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_GFMC_SHAPER_CONFIGS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_gfmc_shaper_configs_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_BFMC_SHAPER_CONFIGS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_bfmc_shaper_configs_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FSMRQ_FLOW_STATUS_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_fsmrq_flow_status_counter_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FSMRQ_DELAY_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_fsmrq_delay_counter_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_HIGH_PRIORITY_DQCQ_DEPTH_CONFIG2_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_high_priority_dqcq_depth_config2_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_DQCQ_MAX_OCCUPANCY_LP_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_dqcq_max_occupancy_lp_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FMS_PARAMETERS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_fms_parameters_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_EMPTY_DQCQ_ID_ERROR_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_empty_dqcq_id_error_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_CREDIT_OVERFLOW_QUEUE_NUM_ERROR_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_credit_overflow_queue_num_error_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_MAX_FSMRQ_REQ_QUEUES_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_max_fsmrq_req_queues_reg_get(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after ips***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_ips_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_ips_set");

  if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_INTERNAL_LOW_PRIORITY_DQCQ_FC_STATUS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_internal_low_priority_dqcq_fc_status_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG3_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_low_priority_dqcq_depth_config3_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_SYSTEM_RED_AGING_CONFIGURATION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_system_red_aging_configuration_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_IPS_CREDIT_CONFIG_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_ips_credit_config_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG2_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_low_priority_dqcq_depth_config2_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG1_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_low_priority_dqcq_depth_config1_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_MAX_PORT_QUEUE_SIZE_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_max_port_queue_size_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_GLOBAL_CREDIT_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_global_credit_counter_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_MASKED_QDP_EVENT_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_masked_qdp_event_counter_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_ILLEGAL_MESH_DESTINATION_QUEUE_NUMBER_ERROR_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_illegal_mesh_destination_queue_number_error_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_DQCQ_ID_FILTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_dqcq_id_filter_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_TIME_IN_SLOW_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_time_in_slow_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FSM_ON_MESSAGE_SHAPER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_fsm_on_message_shaper_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_ACTIVATE_TIMER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_activate_timer_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FLOW_STATUS_FILTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_flow_status_filter_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_GLOBAL_FLOW_STATUS_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_global_flow_status_counter_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG4_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_low_priority_dqcq_depth_config4_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FLOW_CONTROL_COUNT_SELECT_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_flow_control_count_select_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_WR_DATA_REG_0_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_indirect_command_wr_data_reg_0_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_HIGH_PRIORITY_DQCQ_DEPTH_CONFIG1_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_high_priority_dqcq_depth_config1_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_QDP_REPORT_FILTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_qdp_report_filter_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_CREDIT_FABRIC_LATENCY_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_credit_fabric_latency_counter_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_TIME_IN_NORM_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_time_in_norm_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_TIMER_CONFIGURATION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_timer_configuration_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FSMRQ_MAX_OCCUPANCY_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_fsmrq_max_occupancy_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_GLOBAL_QDP_REPORT_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_global_qdp_report_counter_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_RD_DATA_REG_0_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_indirect_command_rd_data_reg_0_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FMS_MAX_OCCUPANCY_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_fms_max_occupancy_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_CAPTURE_QUEUE_DESCRIPTOR_CONFIG_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_capture_queue_descriptor_config_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_interrupt_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_BFMC_CLASS_CONFIGS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_bfmc_class_configs_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_IPS_GENERAL_CONFIGURATIONS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_ips_general_configurations_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_INTERDIGITATED_MODE_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_interdigitated_mode_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FMC_SCHEDULER_CONFIGS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_fmc_scheduler_configs_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_MAX_CR_BAL_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_max_cr_bal_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_CREDIT_FLOW_CONTROL_THRESHOLD_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_credit_flow_control_threshold_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_INTERRUPT_MASK_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_interrupt_mask_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FMC3_CREDIT_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_fmc3_credit_counter_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FLOW_STATUS_FILTER_MASK_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_flow_status_filter_mask_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_INGRESS_SHAPE_SCHEDULER_CONFIG_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_ingress_shape_scheduler_config_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_RD_DATA_REG_1_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_indirect_command_rd_data_reg_1_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_DEQ_CMD_BYTE_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_deq_cmd_byte_counter_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_MANUAL_QUEUE_OPERATION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_manual_queue_operation_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_AUTO_CREDIT_MECHANISM_QUEUE_BOUNDARIES_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_auto_credit_mechanism_queue_boundaries_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FMC1_CREDIT_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_fmc1_credit_counter_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FSM_SCRUBBER_EVENT_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_fsm_scrubber_event_counter_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FMC2_CREDIT_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_fmc2_credit_counter_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FMS_FLOW_STATUS_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_fms_flow_status_counter_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FSM_SCRUBBER_CONFIGURATION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_fsm_scrubber_configuration_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_CAPTURE_QUEUE_DESCRIPTOR_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_capture_queue_descriptor_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FLOW_CONTROL_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_flow_control_counter_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_ADDRESS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_indirect_command_address_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_ACTIVE_QUEUE_COUNT_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_active_queue_count_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_INTERNAL_LOW_PRIORITY_DQCQ_FLOW_CONTROL_CONFIGURATION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_internal_low_priority_dqcq_flow_control_configuration_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_WR_DATA_REG_1_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_indirect_command_wr_data_reg_1_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FMS_DELAY_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_fms_delay_counter_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FSM_SCRUBBER_QUEUE_BOUNDARIES_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_fsm_scrubber_queue_boundaries_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_PROGRAMMABLE_QDP_REPORT_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_programmable_qdp_report_counter_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_INDIRECT_COMMAND_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_indirect_command_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_DQCQ_MAX_OCCUPANCY_HP_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_dqcq_max_occupancy_hp_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FCR_CREDIT_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_fcr_credit_counter_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_LOST_CREDIT_QUEUE_NUMBER_ERROR_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_lost_credit_queue_number_error_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FMC0_CREDIT_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_fmc0_credit_counter_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_EXPIRED_SYSTEM_PORT_ID_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_expired_system_port_id_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_QUEUE_NUM_FILTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_queue_num_filter_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_AUTO_CREDIT_MECHANISM_RATE_CONFIGURATION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_auto_credit_mechanism_rate_configuration_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_GFMC_SHAPER_CONFIGS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_gfmc_shaper_configs_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_BFMC_SHAPER_CONFIGS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_bfmc_shaper_configs_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FSMRQ_FLOW_STATUS_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_fsmrq_flow_status_counter_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FSMRQ_DELAY_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_fsmrq_delay_counter_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_HIGH_PRIORITY_DQCQ_DEPTH_CONFIG2_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_high_priority_dqcq_depth_config2_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_DQCQ_MAX_OCCUPANCY_LP_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_dqcq_max_occupancy_lp_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_FMS_PARAMETERS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_fms_parameters_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_EMPTY_DQCQ_ID_ERROR_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_empty_dqcq_id_error_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_CREDIT_OVERFLOW_QUEUE_NUM_ERROR_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_credit_overflow_queue_num_error_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_IPS_MAX_FSMRQ_REQ_QUEUES_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_ips_max_fsmrq_req_queues_reg_set(current_line, value);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after ips***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}
