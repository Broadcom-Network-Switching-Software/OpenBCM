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
  ui_fap21v_acc_sch_cpu_and_recycle_interfaces_configuration_cpu_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_cpu_and_recycle_interfaces_configuration_cpu_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_CPU_AND_RECYCLE_INTERFACES_CONFIGURATION_CPU_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_CPU_AND_RECYCLE_INTERFACES_CONFIGURATION_CPU_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_CPU_AND_RECYCLE_INTERFACES_CONFIGURATION_CPU_REG_REGISTER_ID:
      regis = &(regs->sch.cpu_and_recycle_interfaces_configuration_cpu_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_CPU_AND_RECYCLE_INTERFACES_CONFIGURATION_CPU_REG_MAX_CR_RATE_ID:
      field = &(regs->sch.cpu_and_recycle_interfaces_configuration_cpu_reg.max_cr_rate);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_CPU_AND_RECYCLE_INTERFACES_CONFIGURATION_CPU_REG_PORT_ID_ID:
      field = &(regs->sch.cpu_and_recycle_interfaces_configuration_cpu_reg.port_id);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_CPU_AND_RECYCLE_INTERFACES_CONFIGURATION_CPU_REG_WEIGHT_ID:
      field = &(regs->sch.cpu_and_recycle_interfaces_configuration_cpu_reg.weight);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "sch.cpu_and_recycle_interfaces_configuration_cpu_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_cpu_and_recycle_interfaces_configuration_cpu_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_cpu_and_recycle_interfaces_configuration_cpu_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_CPU_AND_RECYCLE_INTERFACES_CONFIGURATION_CPU_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_CPU_AND_RECYCLE_INTERFACES_CONFIGURATION_CPU_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_CPU_AND_RECYCLE_INTERFACES_CONFIGURATION_CPU_REG_REGISTER_ID:
      regis = &(regs->sch.cpu_and_recycle_interfaces_configuration_cpu_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_CPU_AND_RECYCLE_INTERFACES_CONFIGURATION_CPU_REG_MAX_CR_RATE_ID:
      field = &(regs->sch.cpu_and_recycle_interfaces_configuration_cpu_reg.max_cr_rate);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_CPU_AND_RECYCLE_INTERFACES_CONFIGURATION_CPU_REG_PORT_ID_ID:
      field = &(regs->sch.cpu_and_recycle_interfaces_configuration_cpu_reg.port_id);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_CPU_AND_RECYCLE_INTERFACES_CONFIGURATION_CPU_REG_WEIGHT_ID:
      field = &(regs->sch.cpu_and_recycle_interfaces_configuration_cpu_reg.weight);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_select_flow_to_queue_mapping_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_select_flow_to_queue_mapping_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_SELECT_FLOW_TO_QUEUE_MAPPING_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_SELECT_FLOW_TO_QUEUE_MAPPING_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_SELECT_FLOW_TO_QUEUE_MAPPING_REG_REGISTER_ID:
      regis = &(regs->sch.select_flow_to_queue_mapping_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SELECT_FLOW_TO_QUEUE_MAPPING_REG_INTER_DIG_ID:
      field = &(regs->sch.select_flow_to_queue_mapping_reg.inter_dig);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "sch.select_flow_to_queue_mapping_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_select_flow_to_queue_mapping_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_select_flow_to_queue_mapping_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_SELECT_FLOW_TO_QUEUE_MAPPING_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_SELECT_FLOW_TO_QUEUE_MAPPING_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_SELECT_FLOW_TO_QUEUE_MAPPING_REG_REGISTER_ID:
      regis = &(regs->sch.select_flow_to_queue_mapping_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SELECT_FLOW_TO_QUEUE_MAPPING_REG_INTER_DIG_ID:
      field = &(regs->sch.select_flow_to_queue_mapping_reg.inter_dig);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_credit_counter_configuration_reg_2_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_credit_counter_configuration_reg_2_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_SCH_0X3_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_SCH_0X3_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_SCH_0X3_REG_REGISTER_ID:
      regis = &(regs->sch.credit_counter_configuration_reg_2.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SCH_0X3_REG_FILTER_DEST_FAP_ID:
      field = &(regs->sch.credit_counter_configuration_reg_2.filter_dest_fap);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SCH_0X3_REG_FILTER_BY_DEST_FAP_ID:
      field = &(regs->sch.credit_counter_configuration_reg_2.filter_by_dest_fap);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SCH_0X3_REG_FILTER_BY_FLOW_ID:
      field = &(regs->sch.credit_counter_configuration_reg_2.filter_by_flow);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SCH_0X3_REG_FILTER_BY_SUB_FLOW_ID:
      field = &(regs->sch.credit_counter_configuration_reg_2.filter_by_sub_flow);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "sch.credit_counter_configuration_reg_2: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_credit_counter_configuration_reg_2_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_credit_counter_configuration_reg_2_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_SCH_0X3_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_SCH_0X3_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_SCH_0X3_REG_REGISTER_ID:
      regis = &(regs->sch.credit_counter_configuration_reg_2.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SCH_0X3_REG_FILTER_DEST_FAP_ID:
      field = &(regs->sch.credit_counter_configuration_reg_2.filter_dest_fap);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SCH_0X3_REG_FILTER_BY_DEST_FAP_ID:
      field = &(regs->sch.credit_counter_configuration_reg_2.filter_by_dest_fap);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SCH_0X3_REG_FILTER_BY_FLOW_ID:
      field = &(regs->sch.credit_counter_configuration_reg_2.filter_by_flow);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SCH_0X3_REG_FILTER_BY_SUB_FLOW_ID:
      field = &(regs->sch.credit_counter_configuration_reg_2.filter_by_sub_flow);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_spia_cal_configuration_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_spia_cal_configuration_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_SPIA_CAL_CONFIGURATION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_SPIA_CAL_CONFIGURATION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_SPIA_CAL_CONFIGURATION_REG_REGISTER_ID:
      regis = &(regs->sch.spia_cal_configuration_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SPIA_CAL_CONFIGURATION_REG_CAL_ALEN0_ID:
      field = &(regs->sch.spia_cal_configuration_reg.cal_alen0);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SPIA_CAL_CONFIGURATION_REG_CAL_ALEN1_ID:
      field = &(regs->sch.spia_cal_configuration_reg.cal_alen1);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SPIA_CAL_CONFIGURATION_REG_SPIWEIGHT_ID:
      field = &(regs->sch.spia_cal_configuration_reg.spiweight);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SPIA_CAL_CONFIGURATION_REG_DVSCALENDAR_SEL_CH_SPIX_ID:
      field = &(regs->sch.spia_cal_configuration_reg.dvscalendar_sel_ch_spix);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "sch.spia_cal_configuration_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_spia_cal_configuration_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_spia_cal_configuration_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_SPIA_CAL_CONFIGURATION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_SPIA_CAL_CONFIGURATION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_SPIA_CAL_CONFIGURATION_REG_REGISTER_ID:
      regis = &(regs->sch.spia_cal_configuration_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SPIA_CAL_CONFIGURATION_REG_CAL_ALEN0_ID:
      field = &(regs->sch.spia_cal_configuration_reg.cal_alen0);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SPIA_CAL_CONFIGURATION_REG_CAL_ALEN1_ID:
      field = &(regs->sch.spia_cal_configuration_reg.cal_alen1);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SPIA_CAL_CONFIGURATION_REG_SPIWEIGHT_ID:
      field = &(regs->sch.spia_cal_configuration_reg.spiweight);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SPIA_CAL_CONFIGURATION_REG_DVSCALENDAR_SEL_CH_SPIX_ID:
      field = &(regs->sch.spia_cal_configuration_reg.dvscalendar_sel_ch_spix);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_force_aggr_fc_3_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_force_aggr_fc_3_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_AGGR_FC_3_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_AGGR_FC_3_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_AGGR_FC_3_REG_REGISTER_ID:
      regis = &(regs->sch.force_aggr_fc_3_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_AGGR_FC_3_REG_FORCE_AGGR_FC255_224_ID:
      field = &(regs->sch.force_aggr_fc_3_reg.force_aggr_fc255_224);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "sch.force_aggr_fc_3_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_force_aggr_fc_3_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_force_aggr_fc_3_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_AGGR_FC_3_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_AGGR_FC_3_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_AGGR_FC_3_REG_REGISTER_ID:
      regis = &(regs->sch.force_aggr_fc_3_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_AGGR_FC_3_REG_FORCE_AGGR_FC255_224_ID:
      field = &(regs->sch.force_aggr_fc_3_reg.force_aggr_fc255_224);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_force_aggr_fc_1_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_force_aggr_fc_1_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_AGGR_FC_1_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_AGGR_FC_1_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_AGGR_FC_1_REG_REGISTER_ID:
      regis = &(regs->sch.force_aggr_fc_1_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_AGGR_FC_1_REG_FORCE_AGGR_FC191_160_ID:
      field = &(regs->sch.force_aggr_fc_1_reg.force_aggr_fc191_160);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "sch.force_aggr_fc_1_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_force_aggr_fc_1_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_force_aggr_fc_1_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_AGGR_FC_1_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_AGGR_FC_1_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_AGGR_FC_1_REG_REGISTER_ID:
      regis = &(regs->sch.force_aggr_fc_1_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_AGGR_FC_1_REG_FORCE_AGGR_FC191_160_ID:
      field = &(regs->sch.force_aggr_fc_1_reg.force_aggr_fc191_160);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_hr_flow_control_mask_3_2_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_hr_flow_control_mask_3_2_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_HR_FLOW_CONTROL_MASK_3_2_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_HR_FLOW_CONTROL_MASK_3_2_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_HR_FLOW_CONTROL_MASK_3_2_REG_REGISTER_ID:
      regis = &(regs->sch.hr_flow_control_mask_3_2_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_HR_FLOW_CONTROL_MASK_3_2_REG_HRFCMASK2_ID:
      field = &(regs->sch.hr_flow_control_mask_3_2_reg.hrfcmask2);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_HR_FLOW_CONTROL_MASK_3_2_REG_HRFCMASK3_ID:
      field = &(regs->sch.hr_flow_control_mask_3_2_reg.hrfcmask3);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "sch.hr_flow_control_mask_3_2_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_hr_flow_control_mask_3_2_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_hr_flow_control_mask_3_2_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_HR_FLOW_CONTROL_MASK_3_2_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_HR_FLOW_CONTROL_MASK_3_2_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_HR_FLOW_CONTROL_MASK_3_2_REG_REGISTER_ID:
      regis = &(regs->sch.hr_flow_control_mask_3_2_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_HR_FLOW_CONTROL_MASK_3_2_REG_HRFCMASK2_ID:
      field = &(regs->sch.hr_flow_control_mask_3_2_reg.hrfcmask2);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_HR_FLOW_CONTROL_MASK_3_2_REG_HRFCMASK3_ID:
      field = &(regs->sch.hr_flow_control_mask_3_2_reg.hrfcmask3);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_force_low_fc_0_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_force_low_fc_0_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_LOW_FC_0_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_LOW_FC_0_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_LOW_FC_0_REG_REGISTER_ID:
      regis = &(regs->sch.force_low_fc_0_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_LOW_FC_0_REG_FORCE_LOW_FC31_0_ID:
      field = &(regs->sch.force_low_fc_0_reg.force_low_fc31_0);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "sch.force_low_fc_0_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_force_low_fc_0_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_force_low_fc_0_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_LOW_FC_0_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_LOW_FC_0_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_LOW_FC_0_REG_REGISTER_ID:
      regis = &(regs->sch.force_low_fc_0_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_LOW_FC_0_REG_FORCE_LOW_FC31_0_ID:
      field = &(regs->sch.force_low_fc_0_reg.force_low_fc31_0);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_hr_flow_control_mask_0_1_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_hr_flow_control_mask_0_1_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_HR_FLOW_CONTROL_MASK_0_1_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_HR_FLOW_CONTROL_MASK_0_1_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_HR_FLOW_CONTROL_MASK_0_1_REG_REGISTER_ID:
      regis = &(regs->sch.hr_flow_control_mask_0_1_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_HR_FLOW_CONTROL_MASK_0_1_REG_HRFCMASK0_ID:
      field = &(regs->sch.hr_flow_control_mask_0_1_reg.hrfcmask0);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_HR_FLOW_CONTROL_MASK_0_1_REG_HRFCMASK1_ID:
      field = &(regs->sch.hr_flow_control_mask_0_1_reg.hrfcmask1);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "sch.hr_flow_control_mask_0_1_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_hr_flow_control_mask_0_1_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_hr_flow_control_mask_0_1_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_HR_FLOW_CONTROL_MASK_0_1_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_HR_FLOW_CONTROL_MASK_0_1_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_HR_FLOW_CONTROL_MASK_0_1_REG_REGISTER_ID:
      regis = &(regs->sch.hr_flow_control_mask_0_1_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_HR_FLOW_CONTROL_MASK_0_1_REG_HRFCMASK0_ID:
      field = &(regs->sch.hr_flow_control_mask_0_1_reg.hrfcmask0);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_HR_FLOW_CONTROL_MASK_0_1_REG_HRFCMASK1_ID:
      field = &(regs->sch.hr_flow_control_mask_0_1_reg.hrfcmask1);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_dvs_link_status_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_dvs_link_status_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_DVS_LINK_STATUS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_DVS_LINK_STATUS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_DVS_LINK_STATUS_REG_REGISTER_ID:
      regis = &(regs->sch.dvs_link_status_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_DVS_LINK_STATUS_REG_DVSLINK_CNT_ID:
      field = &(regs->sch.dvs_link_status_reg.dvslink_cnt);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "sch.dvs_link_status_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_dvs_link_status_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_dvs_link_status_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_DVS_LINK_STATUS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_DVS_LINK_STATUS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_DVS_LINK_STATUS_REG_REGISTER_ID:
      regis = &(regs->sch.dvs_link_status_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_DVS_LINK_STATUS_REG_DVSLINK_CNT_ID:
      field = &(regs->sch.dvs_link_status_reg.dvslink_cnt);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_indirect_command_wr_data_reg_0_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_indirect_command_wr_data_reg_0_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_WR_DATA_REG_0_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_WR_DATA_REG_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_WR_DATA_REG_0_REGISTER_ID:
      regis = &(regs->sch.indirect_command_wr_data_reg_0.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_WR_DATA_REG_0_INDIRECT_COMMAND_WR_DATA_ID:
      field = &(regs->sch.indirect_command_wr_data_reg_0.indirect_command_wr_data);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "sch.indirect_command_wr_data_reg_0: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_indirect_command_wr_data_reg_0_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_indirect_command_wr_data_reg_0_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_WR_DATA_REG_0_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_WR_DATA_REG_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_WR_DATA_REG_0_REGISTER_ID:
      regis = &(regs->sch.indirect_command_wr_data_reg_0.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_WR_DATA_REG_0_INDIRECT_COMMAND_WR_DATA_ID:
      field = &(regs->sch.indirect_command_wr_data_reg_0.indirect_command_wr_data);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_last_flow_restart_event_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_last_flow_restart_event_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_LAST_FLOW_RESTART_EVENT_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_LAST_FLOW_RESTART_EVENT_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_LAST_FLOW_RESTART_EVENT_REG_REGISTER_ID:
      regis = &(regs->sch.last_flow_restart_event_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_LAST_FLOW_RESTART_EVENT_REG_RESTART_FLOW_ID_ID:
      field = &(regs->sch.last_flow_restart_event_reg.restart_flow_id);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_LAST_FLOW_RESTART_EVENT_REG_RESTART_FLOW_EVENT_ID:
      field = &(regs->sch.last_flow_restart_event_reg.restart_flow_event);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "sch.last_flow_restart_event_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_last_flow_restart_event_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_last_flow_restart_event_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_LAST_FLOW_RESTART_EVENT_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_LAST_FLOW_RESTART_EVENT_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_LAST_FLOW_RESTART_EVENT_REG_REGISTER_ID:
      regis = &(regs->sch.last_flow_restart_event_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_LAST_FLOW_RESTART_EVENT_REG_RESTART_FLOW_ID_ID:
      field = &(regs->sch.last_flow_restart_event_reg.restart_flow_id);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_LAST_FLOW_RESTART_EVENT_REG_RESTART_FLOW_EVENT_ID:
      field = &(regs->sch.last_flow_restart_event_reg.restart_flow_event);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_force_high_fc_0_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_force_high_fc_0_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_HIGH_FC_0_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_HIGH_FC_0_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_HIGH_FC_0_REG_REGISTER_ID:
      regis = &(regs->sch.force_high_fc_0_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_HIGH_FC_0_REG_FORCE_HIGH_FC31_0_ID:
      field = &(regs->sch.force_high_fc_0_reg.force_high_fc31_0);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "sch.force_high_fc_0_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_force_high_fc_0_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_force_high_fc_0_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_HIGH_FC_0_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_HIGH_FC_0_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_HIGH_FC_0_REG_REGISTER_ID:
      regis = &(regs->sch.force_high_fc_0_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_HIGH_FC_0_REG_FORCE_HIGH_FC31_0_ID:
      field = &(regs->sch.force_high_fc_0_reg.force_high_fc31_0);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_shaper_configuration_1_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_shaper_configuration_1_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_SHAPER_CONFIGURATION_1_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_SHAPER_CONFIGURATION_1_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_SHAPER_CONFIGURATION_1_REG_REGISTER_ID:
      regis = &(regs->sch.shaper_configuration_1_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SHAPER_CONFIGURATION_1_REG_SHAPER_SLOW_RATE1_ID:
      field = &(regs->sch.shaper_configuration_1_reg.shaper_slow_rate1);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SHAPER_CONFIGURATION_1_REG_SHAPER_SLOW_RATE2_ID:
      field = &(regs->sch.shaper_configuration_1_reg.shaper_slow_rate2);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "sch.shaper_configuration_1_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_shaper_configuration_1_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_shaper_configuration_1_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_SHAPER_CONFIGURATION_1_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_SHAPER_CONFIGURATION_1_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_SHAPER_CONFIGURATION_1_REG_REGISTER_ID:
      regis = &(regs->sch.shaper_configuration_1_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SHAPER_CONFIGURATION_1_REG_SHAPER_SLOW_RATE1_ID:
      field = &(regs->sch.shaper_configuration_1_reg.shaper_slow_rate1);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SHAPER_CONFIGURATION_1_REG_SHAPER_SLOW_RATE2_ID:
      field = &(regs->sch.shaper_configuration_1_reg.shaper_slow_rate2);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_smp_message_counter_and_status_configuration_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_smp_message_counter_and_status_configuration_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_SMP_MESSAGE_COUNTER_AND_STATUS_CONFIGURATION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_SMP_MESSAGE_COUNTER_AND_STATUS_CONFIGURATION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_SMP_MESSAGE_COUNTER_AND_STATUS_CONFIGURATION_REG_REGISTER_ID:
      regis = &(regs->sch.smp_message_counter_and_status_configuration_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SMP_MESSAGE_COUNTER_AND_STATUS_CONFIGURATION_REG_SMPCNT_TYPE_ID:
      field = &(regs->sch.smp_message_counter_and_status_configuration_reg.smpcnt_type);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SMP_MESSAGE_COUNTER_AND_STATUS_CONFIGURATION_REG_SMPFILTER_BY_TYPE_ID:
      field = &(regs->sch.smp_message_counter_and_status_configuration_reg.smpfilter_by_type);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SMP_MESSAGE_COUNTER_AND_STATUS_CONFIGURATION_REG_SMPCNT_FLOW_PORT_ID:
      field = &(regs->sch.smp_message_counter_and_status_configuration_reg.smpcnt_flow_port);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SMP_MESSAGE_COUNTER_AND_STATUS_CONFIGURATION_REG_SMPFILTER_BY_FLOW_PORT_ID:
      field = &(regs->sch.smp_message_counter_and_status_configuration_reg.smpfilter_by_flow_port);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "sch.smp_message_counter_and_status_configuration_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_smp_message_counter_and_status_configuration_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_smp_message_counter_and_status_configuration_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_SMP_MESSAGE_COUNTER_AND_STATUS_CONFIGURATION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_SMP_MESSAGE_COUNTER_AND_STATUS_CONFIGURATION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_SMP_MESSAGE_COUNTER_AND_STATUS_CONFIGURATION_REG_REGISTER_ID:
      regis = &(regs->sch.smp_message_counter_and_status_configuration_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SMP_MESSAGE_COUNTER_AND_STATUS_CONFIGURATION_REG_SMPCNT_TYPE_ID:
      field = &(regs->sch.smp_message_counter_and_status_configuration_reg.smpcnt_type);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SMP_MESSAGE_COUNTER_AND_STATUS_CONFIGURATION_REG_SMPFILTER_BY_TYPE_ID:
      field = &(regs->sch.smp_message_counter_and_status_configuration_reg.smpfilter_by_type);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SMP_MESSAGE_COUNTER_AND_STATUS_CONFIGURATION_REG_SMPCNT_FLOW_PORT_ID:
      field = &(regs->sch.smp_message_counter_and_status_configuration_reg.smpcnt_flow_port);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SMP_MESSAGE_COUNTER_AND_STATUS_CONFIGURATION_REG_SMPFILTER_BY_FLOW_PORT_ID:
      field = &(regs->sch.smp_message_counter_and_status_configuration_reg.smpfilter_by_flow_port);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_spib_rates_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_spib_rates_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_SPIB_RATES_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_SPIB_RATES_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_SPIB_RATES_REG_REGISTER_ID:
      regis = &(regs->sch.spib_rates_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SPIB_RATES_REG_SPIB_SUM_OF_PORTS_ID:
      field = &(regs->sch.spib_rates_reg.spib_sum_of_ports);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SPIB_RATES_REG_SPIB_MAX_CR_RATE_ID:
      field = &(regs->sch.spib_rates_reg.spib_max_cr_rate);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "sch.spib_rates_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_spib_rates_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_spib_rates_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_SPIB_RATES_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_SPIB_RATES_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_SPIB_RATES_REG_REGISTER_ID:
      regis = &(regs->sch.spib_rates_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SPIB_RATES_REG_SPIB_SUM_OF_PORTS_ID:
      field = &(regs->sch.spib_rates_reg.spib_sum_of_ports);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SPIB_RATES_REG_SPIB_MAX_CR_RATE_ID:
      field = &(regs->sch.spib_rates_reg.spib_max_cr_rate);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_indirect_command_rd_data_reg_0_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_indirect_command_rd_data_reg_0_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_RD_DATA_REG_0_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_RD_DATA_REG_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_RD_DATA_REG_0_REGISTER_ID:
      regis = &(regs->sch.indirect_command_rd_data_reg_0.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_RD_DATA_REG_0_INDIRECT_COMMAND_RD_DATA_ID:
      field = &(regs->sch.indirect_command_rd_data_reg_0.indirect_command_rd_data);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "sch.indirect_command_rd_data_reg_0: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_indirect_command_rd_data_reg_0_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_indirect_command_rd_data_reg_0_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_RD_DATA_REG_0_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_RD_DATA_REG_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_RD_DATA_REG_0_REGISTER_ID:
      regis = &(regs->sch.indirect_command_rd_data_reg_0.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_RD_DATA_REG_0_INDIRECT_COMMAND_RD_DATA_ID:
      field = &(regs->sch.indirect_command_rd_data_reg_0.indirect_command_rd_data);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_fsf_composite_configuration_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_fsf_composite_configuration_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_FSF_COMPOSITE_CONFIGURATION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_FSF_COMPOSITE_CONFIGURATION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_FSF_COMPOSITE_CONFIGURATION_REG_REGISTER_ID:
      regis = &(regs->sch.fsf_composite_configuration_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_FSF_COMPOSITE_CONFIGURATION_REG_FSFCOMP_ODD_EVEN_ID:
      field = &(regs->sch.fsf_composite_configuration_reg.fsfcomp_odd_even);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "sch.fsf_composite_configuration_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_fsf_composite_configuration_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_fsf_composite_configuration_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_FSF_COMPOSITE_CONFIGURATION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_FSF_COMPOSITE_CONFIGURATION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_FSF_COMPOSITE_CONFIGURATION_REG_REGISTER_ID:
      regis = &(regs->sch.fsf_composite_configuration_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_FSF_COMPOSITE_CONFIGURATION_REG_FSFCOMP_ODD_EVEN_ID:
      field = &(regs->sch.fsf_composite_configuration_reg.fsfcomp_odd_even);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_interrupt_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_interrupt_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_REG_REGISTER_ID:
      regis = &(regs->sch.interrupt_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_REG_SMP_BAD_MSG_ID:
      field = &(regs->sch.interrupt_reg.smp_bad_msg);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_REG_ACT_FLOW_BAD_PARAMS_ID:
      field = &(regs->sch.interrupt_reg.act_flow_bad_params);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_REG_SHP_FLOW_BAD_PARAMS_ID:
      field = &(regs->sch.interrupt_reg.shp_flow_bad_params);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_REG_RESTART_FLOW_EVENT_ID:
      field = &(regs->sch.interrupt_reg.restart_flow_event);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_REG_SMPFULL_LEVEL1_ID:
      field = &(regs->sch.interrupt_reg.smpfull_level1);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_REG_SMPFULL_LEVEL2_ID:
      field = &(regs->sch.interrupt_reg.smpfull_level2);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_REG_FCTFIFOOVF_ID:
      field = &(regs->sch.interrupt_reg.fctfifoovf);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_REG_DHDECC_ID:
      field = &(regs->sch.interrupt_reg.dhdecc);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_REG_DCDECC_ID:
      field = &(regs->sch.interrupt_reg.dcdecc);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_REG_FLHHRECC_ID:
      field = &(regs->sch.interrupt_reg.flhhrecc);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_REG_FLTHRECC_ID:
      field = &(regs->sch.interrupt_reg.flthrecc);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_REG_FLHCLECC_ID:
      field = &(regs->sch.interrupt_reg.flhclecc);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_REG_FLTCLECC_ID:
      field = &(regs->sch.interrupt_reg.fltclecc);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_REG_FLHFQECC_ID:
      field = &(regs->sch.interrupt_reg.flhfqecc);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_REG_FLTFQECC_ID:
      field = &(regs->sch.interrupt_reg.fltfqecc);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_REG_FDMSECC_ID:
      field = &(regs->sch.interrupt_reg.fdmsecc);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_REG_FDMDECC_ID:
      field = &(regs->sch.interrupt_reg.fdmdecc);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_REG_SHDSECC_ID:
      field = &(regs->sch.interrupt_reg.shdsecc);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_REG_SHDDECC_ID:
      field = &(regs->sch.interrupt_reg.shddecc);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_REG_FQMECC_ID:
      field = &(regs->sch.interrupt_reg.fqmecc);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_REG_SFLHECC_ID:
      field = &(regs->sch.interrupt_reg.sflhecc);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_REG_SFLTECC_ID:
      field = &(regs->sch.interrupt_reg.sfltecc);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_REG_FSMECC_ID:
      field = &(regs->sch.interrupt_reg.fsmecc);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "sch.interrupt_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_interrupt_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_interrupt_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_REG_REGISTER_ID:
      regis = &(regs->sch.interrupt_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_REG_SMP_BAD_MSG_ID:
      field = &(regs->sch.interrupt_reg.smp_bad_msg);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_REG_ACT_FLOW_BAD_PARAMS_ID:
      field = &(regs->sch.interrupt_reg.act_flow_bad_params);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_REG_SHP_FLOW_BAD_PARAMS_ID:
      field = &(regs->sch.interrupt_reg.shp_flow_bad_params);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_REG_RESTART_FLOW_EVENT_ID:
      field = &(regs->sch.interrupt_reg.restart_flow_event);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_REG_SMPFULL_LEVEL1_ID:
      field = &(regs->sch.interrupt_reg.smpfull_level1);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_REG_SMPFULL_LEVEL2_ID:
      field = &(regs->sch.interrupt_reg.smpfull_level2);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_REG_FCTFIFOOVF_ID:
      field = &(regs->sch.interrupt_reg.fctfifoovf);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_REG_DHDECC_ID:
      field = &(regs->sch.interrupt_reg.dhdecc);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_REG_DCDECC_ID:
      field = &(regs->sch.interrupt_reg.dcdecc);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_REG_FLHHRECC_ID:
      field = &(regs->sch.interrupt_reg.flhhrecc);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_REG_FLTHRECC_ID:
      field = &(regs->sch.interrupt_reg.flthrecc);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_REG_FLHCLECC_ID:
      field = &(regs->sch.interrupt_reg.flhclecc);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_REG_FLTCLECC_ID:
      field = &(regs->sch.interrupt_reg.fltclecc);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_REG_FLHFQECC_ID:
      field = &(regs->sch.interrupt_reg.flhfqecc);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_REG_FLTFQECC_ID:
      field = &(regs->sch.interrupt_reg.fltfqecc);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_REG_FDMSECC_ID:
      field = &(regs->sch.interrupt_reg.fdmsecc);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_REG_FDMDECC_ID:
      field = &(regs->sch.interrupt_reg.fdmdecc);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_REG_SHDSECC_ID:
      field = &(regs->sch.interrupt_reg.shdsecc);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_REG_SHDDECC_ID:
      field = &(regs->sch.interrupt_reg.shddecc);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_REG_FQMECC_ID:
      field = &(regs->sch.interrupt_reg.fqmecc);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_REG_SFLHECC_ID:
      field = &(regs->sch.interrupt_reg.sflhecc);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_REG_SFLTECC_ID:
      field = &(regs->sch.interrupt_reg.sfltecc);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_REG_FSMECC_ID:
      field = &(regs->sch.interrupt_reg.fsmecc);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_force_aggr_fc_2_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_force_aggr_fc_2_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_AGGR_FC_2_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_AGGR_FC_2_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_AGGR_FC_2_REG_REGISTER_ID:
      regis = &(regs->sch.force_aggr_fc_2_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_AGGR_FC_2_REG_FORCE_AGGR_FC223_192_ID:
      field = &(regs->sch.force_aggr_fc_2_reg.force_aggr_fc223_192);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "sch.force_aggr_fc_2_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_force_aggr_fc_2_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_force_aggr_fc_2_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_AGGR_FC_2_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_AGGR_FC_2_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_AGGR_FC_2_REG_REGISTER_ID:
      regis = &(regs->sch.force_aggr_fc_2_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_AGGR_FC_2_REG_FORCE_AGGR_FC223_192_ID:
      field = &(regs->sch.force_aggr_fc_2_reg.force_aggr_fc223_192);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_credit_counter_configuration_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_credit_counter_configuration_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_CREDIT_COUNTER_CONFIGURATION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_CREDIT_COUNTER_CONFIGURATION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_CREDIT_COUNTER_CONFIGURATION_REG_REGISTER_ID:
      regis = &(regs->sch.credit_counter_configuration_reg_1.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_CREDIT_COUNTER_CONFIGURATION_REG_FILTER_FLOW_ID:
      field = &(regs->sch.credit_counter_configuration_reg_1.filter_flow);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_CREDIT_COUNTER_CONFIGURATION_REG_FILTER_FLOW_MASK_ID:
      field = &(regs->sch.credit_counter_configuration_reg_1.filter_flow_mask);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "sch.credit_counter_configuration_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_credit_counter_configuration_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_credit_counter_configuration_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_CREDIT_COUNTER_CONFIGURATION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_CREDIT_COUNTER_CONFIGURATION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_CREDIT_COUNTER_CONFIGURATION_REG_REGISTER_ID:
      regis = &(regs->sch.credit_counter_configuration_reg_1.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_CREDIT_COUNTER_CONFIGURATION_REG_FILTER_FLOW_ID:
      field = &(regs->sch.credit_counter_configuration_reg_1.filter_flow);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_CREDIT_COUNTER_CONFIGURATION_REG_FILTER_FLOW_MASK_ID:
      field = &(regs->sch.credit_counter_configuration_reg_1.filter_flow_mask);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_system_red_configuration_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_system_red_configuration_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_SYSTEM_RED_CONFIGURATION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_SYSTEM_RED_CONFIGURATION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_SYSTEM_RED_CONFIGURATION_REG_REGISTER_ID:
      regis = &(regs->sch.system_red_configuration_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SYSTEM_RED_CONFIGURATION_REG_AGING_TIMER_CFG_ID:
      field = &(regs->sch.system_red_configuration_reg.aging_timer_cfg);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SYSTEM_RED_CONFIGURATION_REG_RESET_XPIRED_QSZ_ID:
      field = &(regs->sch.system_red_configuration_reg.reset_xpired_qsz);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SYSTEM_RED_CONFIGURATION_REG_AGING_ONLY_DEC_PQS_ID:
      field = &(regs->sch.system_red_configuration_reg.aging_only_dec_pqs);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SYSTEM_RED_CONFIGURATION_REG_ENABLE_SYS_RED_ID:
      field = &(regs->sch.system_red_configuration_reg.enable_sys_red);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "sch.system_red_configuration_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_system_red_configuration_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_system_red_configuration_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_SYSTEM_RED_CONFIGURATION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_SYSTEM_RED_CONFIGURATION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_SYSTEM_RED_CONFIGURATION_REG_REGISTER_ID:
      regis = &(regs->sch.system_red_configuration_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SYSTEM_RED_CONFIGURATION_REG_AGING_TIMER_CFG_ID:
      field = &(regs->sch.system_red_configuration_reg.aging_timer_cfg);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SYSTEM_RED_CONFIGURATION_REG_RESET_XPIRED_QSZ_ID:
      field = &(regs->sch.system_red_configuration_reg.reset_xpired_qsz);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SYSTEM_RED_CONFIGURATION_REG_AGING_ONLY_DEC_PQS_ID:
      field = &(regs->sch.system_red_configuration_reg.aging_only_dec_pqs);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SYSTEM_RED_CONFIGURATION_REG_ENABLE_SYS_RED_ID:
      field = &(regs->sch.system_red_configuration_reg.enable_sys_red);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_force_high_fc_1_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_force_high_fc_1_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_HIGH_FC_1_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_HIGH_FC_1_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_HIGH_FC_1_REG_REGISTER_ID:
      regis = &(regs->sch.force_high_fc_1_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_HIGH_FC_1_REG_FORCE_HIGH_FC63_32_ID:
      field = &(regs->sch.force_high_fc_1_reg.force_high_fc63_32);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "sch.force_high_fc_1_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_force_high_fc_1_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_force_high_fc_1_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_HIGH_FC_1_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_HIGH_FC_1_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_HIGH_FC_1_REG_REGISTER_ID:
      regis = &(regs->sch.force_high_fc_1_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_HIGH_FC_1_REG_FORCE_HIGH_FC63_32_ID:
      field = &(regs->sch.force_high_fc_1_reg.force_high_fc63_32);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_attempt_to_activate_flow_scheduler_with_bad_parameters_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_attempt_to_activate_flow_scheduler_with_bad_parameters_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_ATTEMPT_TO_ACTIVATE_FLOW_SCHEDULER_WITH_BAD_PARAMETERS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_ATTEMPT_TO_ACTIVATE_FLOW_SCHEDULER_WITH_BAD_PARAMETERS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_ATTEMPT_TO_ACTIVATE_FLOW_SCHEDULER_WITH_BAD_PARAMETERS_REG_REGISTER_ID:
      regis = &(regs->sch.attempt_to_activate_flow_scheduler_with_bad_parameters_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_ATTEMPT_TO_ACTIVATE_FLOW_SCHEDULER_WITH_BAD_PARAMETERS_REG_ACT_FLOW_ID_ID:
      field = &(regs->sch.attempt_to_activate_flow_scheduler_with_bad_parameters_reg.act_flow_id);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_ATTEMPT_TO_ACTIVATE_FLOW_SCHEDULER_WITH_BAD_PARAMETERS_REG_ACT_FLOW_COSN_VALID_ID:
      field = &(regs->sch.attempt_to_activate_flow_scheduler_with_bad_parameters_reg.act_flow_cosn_valid);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_ATTEMPT_TO_ACTIVATE_FLOW_SCHEDULER_WITH_BAD_PARAMETERS_REG_ACT_FLOW_BAD_SCH_ID:
      field = &(regs->sch.attempt_to_activate_flow_scheduler_with_bad_parameters_reg.act_flow_bad_sch);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_ATTEMPT_TO_ACTIVATE_FLOW_SCHEDULER_WITH_BAD_PARAMETERS_REG_ACT_FLOW_BAD_PARAMS_ID:
      field = &(regs->sch.attempt_to_activate_flow_scheduler_with_bad_parameters_reg.act_flow_bad_params);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "sch.attempt_to_activate_flow_scheduler_with_bad_parameters_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_attempt_to_activate_flow_scheduler_with_bad_parameters_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_attempt_to_activate_flow_scheduler_with_bad_parameters_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_ATTEMPT_TO_ACTIVATE_FLOW_SCHEDULER_WITH_BAD_PARAMETERS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_ATTEMPT_TO_ACTIVATE_FLOW_SCHEDULER_WITH_BAD_PARAMETERS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_ATTEMPT_TO_ACTIVATE_FLOW_SCHEDULER_WITH_BAD_PARAMETERS_REG_REGISTER_ID:
      regis = &(regs->sch.attempt_to_activate_flow_scheduler_with_bad_parameters_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_ATTEMPT_TO_ACTIVATE_FLOW_SCHEDULER_WITH_BAD_PARAMETERS_REG_ACT_FLOW_ID_ID:
      field = &(regs->sch.attempt_to_activate_flow_scheduler_with_bad_parameters_reg.act_flow_id);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_ATTEMPT_TO_ACTIVATE_FLOW_SCHEDULER_WITH_BAD_PARAMETERS_REG_ACT_FLOW_COSN_VALID_ID:
      field = &(regs->sch.attempt_to_activate_flow_scheduler_with_bad_parameters_reg.act_flow_cosn_valid);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_ATTEMPT_TO_ACTIVATE_FLOW_SCHEDULER_WITH_BAD_PARAMETERS_REG_ACT_FLOW_BAD_SCH_ID:
      field = &(regs->sch.attempt_to_activate_flow_scheduler_with_bad_parameters_reg.act_flow_bad_sch);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_ATTEMPT_TO_ACTIVATE_FLOW_SCHEDULER_WITH_BAD_PARAMETERS_REG_ACT_FLOW_BAD_PARAMS_ID:
      field = &(regs->sch.attempt_to_activate_flow_scheduler_with_bad_parameters_reg.act_flow_bad_params);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_credit_counter_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_credit_counter_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_CREDIT_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_CREDIT_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_CREDIT_COUNTER_REG_REGISTER_ID:
      regis = &(regs->sch.credit_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_CREDIT_COUNTER_REG_CREDIT_CNT_ID:
      field = &(regs->sch.credit_counter_reg.credit_cnt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_CREDIT_COUNTER_REG_CREDIT_OVF_ID:
      field = &(regs->sch.credit_counter_reg.credit_ovf);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "sch.credit_counter_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_credit_counter_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_credit_counter_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_CREDIT_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_CREDIT_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_CREDIT_COUNTER_REG_REGISTER_ID:
      regis = &(regs->sch.credit_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_CREDIT_COUNTER_REG_CREDIT_CNT_ID:
      field = &(regs->sch.credit_counter_reg.credit_cnt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_CREDIT_COUNTER_REG_CREDIT_OVF_ID:
      field = &(regs->sch.credit_counter_reg.credit_ovf);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_scl_smp_messages_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_scl_smp_messages_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_SCL_SMP_MESSAGES_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_SCL_SMP_MESSAGES_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_SCL_SMP_MESSAGES_REG_REGISTER_ID:
      regis = &(regs->sch.scl_smp_messages_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SCL_SMP_MESSAGES_REG_SMPSCLMSG_ID_ID:
      field = &(regs->sch.scl_smp_messages_reg.smpsclmsg_id);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SCL_SMP_MESSAGES_REG_SMPSCLMSG_STATUS_ID:
      field = &(regs->sch.scl_smp_messages_reg.smpsclmsg_status);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SCL_SMP_MESSAGES_REG_SMPSCLMSG_TYPE_ID:
      field = &(regs->sch.scl_smp_messages_reg.smpsclmsg_type);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SCL_SMP_MESSAGES_REG_SMPSCLMSG_THROW_ID:
      field = &(regs->sch.scl_smp_messages_reg.smpsclmsg_throw);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SCL_SMP_MESSAGES_REG_SMPSCLMSG_ID:
      field = &(regs->sch.scl_smp_messages_reg.smpsclmsg);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "sch.scl_smp_messages_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_scl_smp_messages_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_scl_smp_messages_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_SCL_SMP_MESSAGES_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_SCL_SMP_MESSAGES_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_SCL_SMP_MESSAGES_REG_REGISTER_ID:
      regis = &(regs->sch.scl_smp_messages_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SCL_SMP_MESSAGES_REG_SMPSCLMSG_ID_ID:
      field = &(regs->sch.scl_smp_messages_reg.smpsclmsg_id);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SCL_SMP_MESSAGES_REG_SMPSCLMSG_STATUS_ID:
      field = &(regs->sch.scl_smp_messages_reg.smpsclmsg_status);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SCL_SMP_MESSAGES_REG_SMPSCLMSG_TYPE_ID:
      field = &(regs->sch.scl_smp_messages_reg.smpsclmsg_type);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SCL_SMP_MESSAGES_REG_SMPSCLMSG_THROW_ID:
      field = &(regs->sch.scl_smp_messages_reg.smpsclmsg_throw);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SCL_SMP_MESSAGES_REG_SMPSCLMSG_ID:
      field = &(regs->sch.scl_smp_messages_reg.smpsclmsg);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_hrport_en_1_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_hrport_en_1_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_HRPORT_EN_1_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_HRPORT_EN_1_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_HRPORT_EN_1_REG_REGISTER_ID:
      regis = &(regs->sch.hrport_en_1_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_HRPORT_EN_1_REG_HRPORT_EN31_0_ID:
      field = &(regs->sch.hrport_en_1_reg.hrport_en31_0);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "sch.hrport_en_1_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_hrport_en_1_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_hrport_en_1_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_HRPORT_EN_1_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_HRPORT_EN_1_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_HRPORT_EN_1_REG_REGISTER_ID:
      regis = &(regs->sch.hrport_en_1_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_HRPORT_EN_1_REG_HRPORT_EN31_0_ID:
      field = &(regs->sch.hrport_en_1_reg.hrport_en31_0);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_force_low_fc_1_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_force_low_fc_1_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_LOW_FC_1_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_LOW_FC_1_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_LOW_FC_1_REG_REGISTER_ID:
      regis = &(regs->sch.force_low_fc_1_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_LOW_FC_1_REG_FORCE_LOW_FC63_32_ID:
      field = &(regs->sch.force_low_fc_1_reg.force_low_fc63_32);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "sch.force_low_fc_1_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_force_low_fc_1_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_force_low_fc_1_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_LOW_FC_1_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_LOW_FC_1_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_LOW_FC_1_REG_REGISTER_ID:
      regis = &(regs->sch.force_low_fc_1_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_LOW_FC_1_REG_FORCE_LOW_FC63_32_ID:
      field = &(regs->sch.force_low_fc_1_reg.force_low_fc63_32);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_attempt_to_activate_a_shaper_with_bad_parameters_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_attempt_to_activate_a_shaper_with_bad_parameters_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_ATTEMPT_TO_ACTIVATE_A_SHAPER_WITH_BAD_PARAMETERS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_ATTEMPT_TO_ACTIVATE_A_SHAPER_WITH_BAD_PARAMETERS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_ATTEMPT_TO_ACTIVATE_A_SHAPER_WITH_BAD_PARAMETERS_REG_REGISTER_ID:
      regis = &(regs->sch.attempt_to_activate_a_shaper_with_bad_parameters_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_ATTEMPT_TO_ACTIVATE_A_SHAPER_WITH_BAD_PARAMETERS_REG_SHP_FLOW_ID_ID:
      field = &(regs->sch.attempt_to_activate_a_shaper_with_bad_parameters_reg.shp_flow_id);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_ATTEMPT_TO_ACTIVATE_A_SHAPER_WITH_BAD_PARAMETERS_REG_SHP_FLOW_BAD_PARAMS_ID:
      field = &(regs->sch.attempt_to_activate_a_shaper_with_bad_parameters_reg.shp_flow_bad_params);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "sch.attempt_to_activate_a_shaper_with_bad_parameters_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_attempt_to_activate_a_shaper_with_bad_parameters_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_attempt_to_activate_a_shaper_with_bad_parameters_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_ATTEMPT_TO_ACTIVATE_A_SHAPER_WITH_BAD_PARAMETERS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_ATTEMPT_TO_ACTIVATE_A_SHAPER_WITH_BAD_PARAMETERS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_ATTEMPT_TO_ACTIVATE_A_SHAPER_WITH_BAD_PARAMETERS_REG_REGISTER_ID:
      regis = &(regs->sch.attempt_to_activate_a_shaper_with_bad_parameters_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_ATTEMPT_TO_ACTIVATE_A_SHAPER_WITH_BAD_PARAMETERS_REG_SHP_FLOW_ID_ID:
      field = &(regs->sch.attempt_to_activate_a_shaper_with_bad_parameters_reg.shp_flow_id);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_ATTEMPT_TO_ACTIVATE_A_SHAPER_WITH_BAD_PARAMETERS_REG_SHP_FLOW_BAD_PARAMS_ID:
      field = &(regs->sch.attempt_to_activate_a_shaper_with_bad_parameters_reg.shp_flow_bad_params);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_scheduler_configuration_register_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_scheduler_configuration_register_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_SCHEDULER_CONFIGURATION_REGISTER_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_SCHEDULER_CONFIGURATION_REGISTER_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_SCHEDULER_CONFIGURATION_REGISTER_REGISTER_ID:
      regis = &(regs->sch.scheduler_configuration_register.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SCHEDULER_CONFIGURATION_REGISTER_SMP_DISABLE_FABRIC_ID:
      field = &(regs->sch.scheduler_configuration_register.smp_disable_fabric);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "sch.scheduler_configuration_register: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_scheduler_configuration_register_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_scheduler_configuration_register_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_SCHEDULER_CONFIGURATION_REGISTER_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_SCHEDULER_CONFIGURATION_REGISTER_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_SCHEDULER_CONFIGURATION_REGISTER_REGISTER_ID:
      regis = &(regs->sch.scheduler_configuration_register.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SCHEDULER_CONFIGURATION_REGISTER_SMP_DISABLE_FABRIC_ID:
      field = &(regs->sch.scheduler_configuration_register.smp_disable_fabric);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_spib_cal_configuration_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_spib_cal_configuration_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_SPIB_CAL_CONFIGURATION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_SPIB_CAL_CONFIGURATION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_SPIB_CAL_CONFIGURATION_REG_REGISTER_ID:
      regis = &(regs->sch.spib_cal_configuration_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SPIB_CAL_CONFIGURATION_REG_CAL_BLEN0_ID:
      field = &(regs->sch.spib_cal_configuration_reg.cal_blen0);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SPIB_CAL_CONFIGURATION_REG_CAL_BLEN1_ID:
      field = &(regs->sch.spib_cal_configuration_reg.cal_blen1);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SPIB_CAL_CONFIGURATION_REG_SPIWEIGHT_ID:
      field = &(regs->sch.spib_cal_configuration_reg.spiweight);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SPIB_CAL_CONFIGURATION_REG_DVSCALENDAR_SEL_CH_SPIX_ID:
      field = &(regs->sch.spib_cal_configuration_reg.dvscalendar_sel_ch_spix);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "sch.spib_cal_configuration_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_spib_cal_configuration_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_spib_cal_configuration_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_SPIB_CAL_CONFIGURATION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_SPIB_CAL_CONFIGURATION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_SPIB_CAL_CONFIGURATION_REG_REGISTER_ID:
      regis = &(regs->sch.spib_cal_configuration_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SPIB_CAL_CONFIGURATION_REG_CAL_BLEN0_ID:
      field = &(regs->sch.spib_cal_configuration_reg.cal_blen0);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SPIB_CAL_CONFIGURATION_REG_CAL_BLEN1_ID:
      field = &(regs->sch.spib_cal_configuration_reg.cal_blen1);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SPIB_CAL_CONFIGURATION_REG_SPIWEIGHT_ID:
      field = &(regs->sch.spib_cal_configuration_reg.spiweight);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SPIB_CAL_CONFIGURATION_REG_DVSCALENDAR_SEL_CH_SPIX_ID:
      field = &(regs->sch.spib_cal_configuration_reg.dvscalendar_sel_ch_spix);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_indirect_command_address_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_indirect_command_address_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_ADDRESS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_ADDRESS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_ADDRESS_REG_REGISTER_ID:
      regis = &(regs->sch.indirect_command_address_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_ADDRESS_REG_INDIRECT_COMMAND_ADDR_ID:
      field = &(regs->sch.indirect_command_address_reg.indirect_command_addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_ADDRESS_REG_INDIRECT_COMMAND_TYPE_ID:
      field = &(regs->sch.indirect_command_address_reg.indirect_command_type);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "sch.indirect_command_address_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_indirect_command_address_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_indirect_command_address_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_ADDRESS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_ADDRESS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_ADDRESS_REG_REGISTER_ID:
      regis = &(regs->sch.indirect_command_address_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_ADDRESS_REG_INDIRECT_COMMAND_ADDR_ID:
      field = &(regs->sch.indirect_command_address_reg.indirect_command_addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_ADDRESS_REG_INDIRECT_COMMAND_TYPE_ID:
      field = &(regs->sch.indirect_command_address_reg.indirect_command_type);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_device_scheduler_dvs_config0_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_device_scheduler_dvs_config0_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_DEVICE_SCHEDULER_DVS_CONFIG0_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_DEVICE_SCHEDULER_DVS_CONFIG0_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_DEVICE_SCHEDULER_DVS_CONFIG0_REG_REGISTER_ID:
      regis = &(regs->sch.device_scheduler_dvs_config0_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_DEVICE_SCHEDULER_DVS_CONFIG0_REG_SPIA_FORCE_PAUSE_ID:
      field = &(regs->sch.device_scheduler_dvs_config0_reg.spia_force_pause);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_DEVICE_SCHEDULER_DVS_CONFIG0_REG_SPIB_FORCE_PAUSE_ID:
      field = &(regs->sch.device_scheduler_dvs_config0_reg.spib_force_pause);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "sch.device_scheduler_dvs_config0_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_device_scheduler_dvs_config0_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_device_scheduler_dvs_config0_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_DEVICE_SCHEDULER_DVS_CONFIG0_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_DEVICE_SCHEDULER_DVS_CONFIG0_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_DEVICE_SCHEDULER_DVS_CONFIG0_REG_REGISTER_ID:
      regis = &(regs->sch.device_scheduler_dvs_config0_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_DEVICE_SCHEDULER_DVS_CONFIG0_REG_SPIA_FORCE_PAUSE_ID:
      field = &(regs->sch.device_scheduler_dvs_config0_reg.spia_force_pause);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_DEVICE_SCHEDULER_DVS_CONFIG0_REG_SPIB_FORCE_PAUSE_ID:
      field = &(regs->sch.device_scheduler_dvs_config0_reg.spib_force_pause);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_indirect_command_wr_data_reg_1_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_indirect_command_wr_data_reg_1_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_WR_DATA_REG_1_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_WR_DATA_REG_1_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_WR_DATA_REG_1_REGISTER_ID:
      regis = &(regs->sch.indirect_command_wr_data_reg_1.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_WR_DATA_REG_1_INDIRECT_COMMAND_WR_DATA_ID:
      field = &(regs->sch.indirect_command_wr_data_reg_1.indirect_command_wr_data);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "sch.indirect_command_wr_data_reg_1: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_indirect_command_wr_data_reg_1_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_indirect_command_wr_data_reg_1_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_WR_DATA_REG_1_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_WR_DATA_REG_1_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_WR_DATA_REG_1_REGISTER_ID:
      regis = &(regs->sch.indirect_command_wr_data_reg_1.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_WR_DATA_REG_1_INDIRECT_COMMAND_WR_DATA_ID:
      field = &(regs->sch.indirect_command_wr_data_reg_1.indirect_command_wr_data);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_cpu_and_recycle_interfaces_configuration_recycle_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_cpu_and_recycle_interfaces_configuration_recycle_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_CPU_AND_RECYCLE_INTERFACES_CONFIGURATION_RECYCLE_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_CPU_AND_RECYCLE_INTERFACES_CONFIGURATION_RECYCLE_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_CPU_AND_RECYCLE_INTERFACES_CONFIGURATION_RECYCLE_REG_REGISTER_ID:
      regis = &(regs->sch.cpu_and_recycle_interfaces_configuration_recycle_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_CPU_AND_RECYCLE_INTERFACES_CONFIGURATION_RECYCLE_REG_MAX_CR_RATE_ID:
      field = &(regs->sch.cpu_and_recycle_interfaces_configuration_recycle_reg.max_cr_rate);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_CPU_AND_RECYCLE_INTERFACES_CONFIGURATION_RECYCLE_REG_PORT_ID_ID:
      field = &(regs->sch.cpu_and_recycle_interfaces_configuration_recycle_reg.port_id);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_CPU_AND_RECYCLE_INTERFACES_CONFIGURATION_RECYCLE_REG_WEIGHT_ID:
      field = &(regs->sch.cpu_and_recycle_interfaces_configuration_recycle_reg.weight);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "sch.cpu_and_recycle_interfaces_configuration_recycle_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_cpu_and_recycle_interfaces_configuration_recycle_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_cpu_and_recycle_interfaces_configuration_recycle_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_CPU_AND_RECYCLE_INTERFACES_CONFIGURATION_RECYCLE_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_CPU_AND_RECYCLE_INTERFACES_CONFIGURATION_RECYCLE_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_CPU_AND_RECYCLE_INTERFACES_CONFIGURATION_RECYCLE_REG_REGISTER_ID:
      regis = &(regs->sch.cpu_and_recycle_interfaces_configuration_recycle_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_CPU_AND_RECYCLE_INTERFACES_CONFIGURATION_RECYCLE_REG_MAX_CR_RATE_ID:
      field = &(regs->sch.cpu_and_recycle_interfaces_configuration_recycle_reg.max_cr_rate);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_CPU_AND_RECYCLE_INTERFACES_CONFIGURATION_RECYCLE_REG_PORT_ID_ID:
      field = &(regs->sch.cpu_and_recycle_interfaces_configuration_recycle_reg.port_id);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_CPU_AND_RECYCLE_INTERFACES_CONFIGURATION_RECYCLE_REG_WEIGHT_ID:
      field = &(regs->sch.cpu_and_recycle_interfaces_configuration_recycle_reg.weight);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_indirect_command_rd_data_reg_2_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_indirect_command_rd_data_reg_2_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_RD_DATA_REG_2_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_RD_DATA_REG_2_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_RD_DATA_REG_2_REGISTER_ID:
      regis = &(regs->sch.indirect_command_rd_data_reg_2.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_RD_DATA_REG_2_INDIRECT_COMMAND_RD_DATA_ID:
      field = &(regs->sch.indirect_command_rd_data_reg_2.indirect_command_rd_data);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "sch.indirect_command_rd_data_reg_2: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_indirect_command_rd_data_reg_2_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_indirect_command_rd_data_reg_2_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_RD_DATA_REG_2_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_RD_DATA_REG_2_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_RD_DATA_REG_2_REGISTER_ID:
      regis = &(regs->sch.indirect_command_rd_data_reg_2.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_RD_DATA_REG_2_INDIRECT_COMMAND_RD_DATA_ID:
      field = &(regs->sch.indirect_command_rd_data_reg_2.indirect_command_rd_data);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_indirect_command_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_indirect_command_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_REG_REGISTER_ID:
      regis = &(regs->sch.indirect_command_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_REG_INDIRECT_COMMAND_ID:
      field = &(regs->sch.indirect_command_reg.indirect_command);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "sch.indirect_command_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_indirect_command_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_indirect_command_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_REG_REGISTER_ID:
      regis = &(regs->sch.indirect_command_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_REG_INDIRECT_COMMAND_ID:
      field = &(regs->sch.indirect_command_reg.indirect_command);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_force_fc_configuration_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_force_fc_configuration_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_FC_CONFIGURATION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_FC_CONFIGURATION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_FC_CONFIGURATION_REG_REGISTER_ID:
      regis = &(regs->sch.force_fc_configuration_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_FC_CONFIGURATION_REG_FORCE_HIGH_FCOVERRIDE_ID:
      field = &(regs->sch.force_fc_configuration_reg.force_high_fcoverride);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_FC_CONFIGURATION_REG_FORCE_LOW_FCOVERRIDE_ID:
      field = &(regs->sch.force_fc_configuration_reg.force_low_fcoverride);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_FC_CONFIGURATION_REG_FORCE_AGGR_FCEN_ID:
      field = &(regs->sch.force_fc_configuration_reg.force_aggr_fcen);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_FC_CONFIGURATION_REG_FORCE_AGGR_FCOVERRIDE_ID:
      field = &(regs->sch.force_fc_configuration_reg.force_aggr_fcoverride);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "sch.force_fc_configuration_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_force_fc_configuration_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_force_fc_configuration_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_FC_CONFIGURATION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_FC_CONFIGURATION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_FC_CONFIGURATION_REG_REGISTER_ID:
      regis = &(regs->sch.force_fc_configuration_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_FC_CONFIGURATION_REG_FORCE_HIGH_FCOVERRIDE_ID:
      field = &(regs->sch.force_fc_configuration_reg.force_high_fcoverride);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_FC_CONFIGURATION_REG_FORCE_LOW_FCOVERRIDE_ID:
      field = &(regs->sch.force_fc_configuration_reg.force_low_fcoverride);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_FC_CONFIGURATION_REG_FORCE_AGGR_FCEN_ID:
      field = &(regs->sch.force_fc_configuration_reg.force_aggr_fcen);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_FC_CONFIGURATION_REG_FORCE_AGGR_FCOVERRIDE_ID:
      field = &(regs->sch.force_fc_configuration_reg.force_aggr_fcoverride);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_force_aggr_fc_0_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_force_aggr_fc_0_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_AGGR_FC_0_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_AGGR_FC_0_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_AGGR_FC_0_REG_REGISTER_ID:
      regis = &(regs->sch.force_aggr_fc_0_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_AGGR_FC_0_REG_FORCE_AGGR_FC159_128_ID:
      field = &(regs->sch.force_aggr_fc_0_reg.force_aggr_fc159_128);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "sch.force_aggr_fc_0_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_force_aggr_fc_0_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_force_aggr_fc_0_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_AGGR_FC_0_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_AGGR_FC_0_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_AGGR_FC_0_REG_REGISTER_ID:
      regis = &(regs->sch.force_aggr_fc_0_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_AGGR_FC_0_REG_FORCE_AGGR_FC159_128_ID:
      field = &(regs->sch.force_aggr_fc_0_reg.force_aggr_fc159_128);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_device_scheduler_dvs_weight_config_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_device_scheduler_dvs_weight_config_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_DEVICE_SCHEDULER_DVS_WEIGHT_CONFIG_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_DEVICE_SCHEDULER_DVS_WEIGHT_CONFIG_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_DEVICE_SCHEDULER_DVS_WEIGHT_CONFIG_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_DEVICE_SCHEDULER_DVS_WEIGHT_CONFIG_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_DEVICE_SCHEDULER_DVS_WEIGHT_CONFIG_REG_REGISTER_ID:
      regis = &(regs->sch.device_scheduler_dvs_weight_config_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_DEVICE_SCHEDULER_DVS_WEIGHT_CONFIG_REG_WFQWEIGHT_XX_ID:
      field = &(regs->sch.device_scheduler_dvs_weight_config_reg[offset].wfqweight_xx);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_DEVICE_SCHEDULER_DVS_WEIGHT_CONFIG_REG_WFQWEIGHT_XX_PLUS_1_ID:
      field = &(regs->sch.device_scheduler_dvs_weight_config_reg[offset].wfqweight_xx_plus_1);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "sch.device_scheduler_dvs_weight_config_reg***: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_device_scheduler_dvs_weight_config_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_device_scheduler_dvs_weight_config_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_DEVICE_SCHEDULER_DVS_WEIGHT_CONFIG_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_DEVICE_SCHEDULER_DVS_WEIGHT_CONFIG_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_DEVICE_SCHEDULER_DVS_WEIGHT_CONFIG_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_DEVICE_SCHEDULER_DVS_WEIGHT_CONFIG_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_DEVICE_SCHEDULER_DVS_WEIGHT_CONFIG_REG_REGISTER_ID:
      regis = &(regs->sch.device_scheduler_dvs_weight_config_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_DEVICE_SCHEDULER_DVS_WEIGHT_CONFIG_REG_WFQWEIGHT_XX_ID:
      field = &(regs->sch.device_scheduler_dvs_weight_config_reg[offset].wfqweight_xx);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_DEVICE_SCHEDULER_DVS_WEIGHT_CONFIG_REG_WFQWEIGHT_XX_PLUS_1_ID:
      field = &(regs->sch.device_scheduler_dvs_weight_config_reg[offset].wfqweight_xx_plus_1);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_device_scheduler_dvs_config1_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_device_scheduler_dvs_config1_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_DEVICE_SCHEDULER_DVS_CONFIG1_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_DEVICE_SCHEDULER_DVS_CONFIG1_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_DEVICE_SCHEDULER_DVS_CONFIG1_REG_REGISTER_ID:
      regis = &(regs->sch.device_scheduler_dvs_config1_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_DEVICE_SCHEDULER_DVS_CONFIG1_REG_CPUFORCE_PAUSE_ID:
      field = &(regs->sch.device_scheduler_dvs_config1_reg.cpuforce_pause);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_DEVICE_SCHEDULER_DVS_CONFIG1_REG_RCYFORCE_PAUSE_ID:
      field = &(regs->sch.device_scheduler_dvs_config1_reg.rcyforce_pause);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_DEVICE_SCHEDULER_DVS_CONFIG1_REG_FORCE_PAUSE_ID:
      field = &(regs->sch.device_scheduler_dvs_config1_reg.force_pause);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "sch.device_scheduler_dvs_config1_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_device_scheduler_dvs_config1_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_device_scheduler_dvs_config1_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_DEVICE_SCHEDULER_DVS_CONFIG1_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_DEVICE_SCHEDULER_DVS_CONFIG1_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_DEVICE_SCHEDULER_DVS_CONFIG1_REG_REGISTER_ID:
      regis = &(regs->sch.device_scheduler_dvs_config1_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_DEVICE_SCHEDULER_DVS_CONFIG1_REG_CPUFORCE_PAUSE_ID:
      field = &(regs->sch.device_scheduler_dvs_config1_reg.cpuforce_pause);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_DEVICE_SCHEDULER_DVS_CONFIG1_REG_RCYFORCE_PAUSE_ID:
      field = &(regs->sch.device_scheduler_dvs_config1_reg.rcyforce_pause);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_DEVICE_SCHEDULER_DVS_CONFIG1_REG_FORCE_PAUSE_ID:
      field = &(regs->sch.device_scheduler_dvs_config1_reg.force_pause);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_smp_message_counter_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_smp_message_counter_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_SMP_MESSAGE_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_SMP_MESSAGE_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_SMP_MESSAGE_COUNTER_REG_REGISTER_ID:
      regis = &(regs->sch.smp_message_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SMP_MESSAGE_COUNTER_REG_SMPMSG_CNT_ID:
      field = &(regs->sch.smp_message_counter_reg.smpmsg_cnt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SMP_MESSAGE_COUNTER_REG_SMPMSG_CNT_OVF_ID:
      field = &(regs->sch.smp_message_counter_reg.smpmsg_cnt_ovf);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "sch.smp_message_counter_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_smp_message_counter_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_smp_message_counter_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_SMP_MESSAGE_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_SMP_MESSAGE_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_SMP_MESSAGE_COUNTER_REG_REGISTER_ID:
      regis = &(regs->sch.smp_message_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SMP_MESSAGE_COUNTER_REG_SMPMSG_CNT_ID:
      field = &(regs->sch.smp_message_counter_reg.smpmsg_cnt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SMP_MESSAGE_COUNTER_REG_SMPMSG_CNT_OVF_ID:
      field = &(regs->sch.smp_message_counter_reg.smpmsg_cnt_ovf);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_indirect_command_rd_data_reg_1_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_indirect_command_rd_data_reg_1_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_RD_DATA_REG_1_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_RD_DATA_REG_1_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_RD_DATA_REG_1_REGISTER_ID:
      regis = &(regs->sch.indirect_command_rd_data_reg_1.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_RD_DATA_REG_1_INDIRECT_COMMAND_RD_DATA_ID:
      field = &(regs->sch.indirect_command_rd_data_reg_1.indirect_command_rd_data);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "sch.indirect_command_rd_data_reg_1: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_indirect_command_rd_data_reg_1_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_indirect_command_rd_data_reg_1_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_RD_DATA_REG_1_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_RD_DATA_REG_1_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_RD_DATA_REG_1_REGISTER_ID:
      regis = &(regs->sch.indirect_command_rd_data_reg_1.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_RD_DATA_REG_1_INDIRECT_COMMAND_RD_DATA_ID:
      field = &(regs->sch.indirect_command_rd_data_reg_1.indirect_command_rd_data);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_spia_rates_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_spia_rates_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_SPIA_RATES_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_SPIA_RATES_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_SPIA_RATES_REG_REGISTER_ID:
      regis = &(regs->sch.spia_rates_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SPIA_RATES_REG_SPIA_SUM_OF_PORTS_ID:
      field = &(regs->sch.spia_rates_reg.spia_sum_of_ports);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SPIA_RATES_REG_SPIA_MAX_CR_RATE_ID:
      field = &(regs->sch.spia_rates_reg.spia_max_cr_rate);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "sch.spia_rates_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_spia_rates_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_spia_rates_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_SPIA_RATES_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_SPIA_RATES_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_SPIA_RATES_REG_REGISTER_ID:
      regis = &(regs->sch.spia_rates_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SPIA_RATES_REG_SPIA_SUM_OF_PORTS_ID:
      field = &(regs->sch.spia_rates_reg.spia_sum_of_ports);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SPIA_RATES_REG_SPIA_MAX_CR_RATE_ID:
      field = &(regs->sch.spia_rates_reg.spia_max_cr_rate);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_hrport_en_2_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_hrport_en_2_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_HRPORT_EN_2_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_HRPORT_EN_2_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_HRPORT_EN_2_REG_REGISTER_ID:
      regis = &(regs->sch.hrport_en_2_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_HRPORT_EN_2_REG_HRPORT_EN63_32_ID:
      field = &(regs->sch.hrport_en_2_reg.hrport_en63_32);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "sch.hrport_en_2_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_hrport_en_2_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_hrport_en_2_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_HRPORT_EN_2_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_HRPORT_EN_2_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_HRPORT_EN_2_REG_REGISTER_ID:
      regis = &(regs->sch.hrport_en_2_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_HRPORT_EN_2_REG_HRPORT_EN63_32_ID:
      field = &(regs->sch.hrport_en_2_reg.hrport_en63_32);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_interrupt_mask_data_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_interrupt_mask_data_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_MASK_DATA_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_MASK_DATA_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_MASK_DATA_REG_REGISTER_ID:
      regis = &(regs->sch.interrupt_mask_data_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_MASK_DATA_REG_SMP_BAD_MSG_MASK_ID:
      field = &(regs->sch.interrupt_mask_data_reg.smp_bad_msg_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_MASK_DATA_REG_ACT_FLOW_BAD_PARAMS_MASK_ID:
      field = &(regs->sch.interrupt_mask_data_reg.act_flow_bad_params_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_MASK_DATA_REG_SHP_FLOW_BAD_PARAMS_MASK_ID:
      field = &(regs->sch.interrupt_mask_data_reg.shp_flow_bad_params_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_MASK_DATA_REG_RESTART_FLOW_EVENT_MASK_ID:
      field = &(regs->sch.interrupt_mask_data_reg.restart_flow_event_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_MASK_DATA_REG_SMPFULL_LEVEL1_MASK_ID:
      field = &(regs->sch.interrupt_mask_data_reg.smpfull_level1_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_MASK_DATA_REG_SMPFULL_LEVEL2_MASK_ID:
      field = &(regs->sch.interrupt_mask_data_reg.smpfull_level2_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_MASK_DATA_REG_FCTFIFOOVF_MASK_ID:
      field = &(regs->sch.interrupt_mask_data_reg.fctfifoovf_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_MASK_DATA_REG_DHDECCMASK_ID:
      field = &(regs->sch.interrupt_mask_data_reg.dhdeccmask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_MASK_DATA_REG_DCDECCMASK_ID:
      field = &(regs->sch.interrupt_mask_data_reg.dcdeccmask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_MASK_DATA_REG_FLHHRECCMASK_ID:
      field = &(regs->sch.interrupt_mask_data_reg.flhhreccmask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_MASK_DATA_REG_FLTHRECCMASK_ID:
      field = &(regs->sch.interrupt_mask_data_reg.flthreccmask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_MASK_DATA_REG_FLHCLECCMASK_ID:
      field = &(regs->sch.interrupt_mask_data_reg.flhcleccmask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_MASK_DATA_REG_FLTCLECCMASK_ID:
      field = &(regs->sch.interrupt_mask_data_reg.fltcleccmask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_MASK_DATA_REG_FLHFQECCMASK_ID:
      field = &(regs->sch.interrupt_mask_data_reg.flhfqeccmask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_MASK_DATA_REG_FLTFQECCMASK_ID:
      field = &(regs->sch.interrupt_mask_data_reg.fltfqeccmask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_MASK_DATA_REG_FDMSECCMASK_ID:
      field = &(regs->sch.interrupt_mask_data_reg.fdmseccmask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_MASK_DATA_REG_FDMDECCMASK_ID:
      field = &(regs->sch.interrupt_mask_data_reg.fdmdeccmask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_MASK_DATA_REG_SHDSECCMASK_ID:
      field = &(regs->sch.interrupt_mask_data_reg.shdseccmask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_MASK_DATA_REG_SHDDECCMASK_ID:
      field = &(regs->sch.interrupt_mask_data_reg.shddeccmask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_MASK_DATA_REG_FQMECCMASK_ID:
      field = &(regs->sch.interrupt_mask_data_reg.fqmeccmask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_MASK_DATA_REG_SFLHECCMASK_ID:
      field = &(regs->sch.interrupt_mask_data_reg.sflheccmask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_MASK_DATA_REG_SFLTECCMASK_ID:
      field = &(regs->sch.interrupt_mask_data_reg.sflteccmask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_MASK_DATA_REG_FSMECCMASK_ID:
      field = &(regs->sch.interrupt_mask_data_reg.fsmeccmask);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "sch.interrupt_mask_data_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_interrupt_mask_data_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_interrupt_mask_data_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_MASK_DATA_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_MASK_DATA_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_MASK_DATA_REG_REGISTER_ID:
      regis = &(regs->sch.interrupt_mask_data_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_MASK_DATA_REG_SMP_BAD_MSG_MASK_ID:
      field = &(regs->sch.interrupt_mask_data_reg.smp_bad_msg_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_MASK_DATA_REG_ACT_FLOW_BAD_PARAMS_MASK_ID:
      field = &(regs->sch.interrupt_mask_data_reg.act_flow_bad_params_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_MASK_DATA_REG_SHP_FLOW_BAD_PARAMS_MASK_ID:
      field = &(regs->sch.interrupt_mask_data_reg.shp_flow_bad_params_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_MASK_DATA_REG_RESTART_FLOW_EVENT_MASK_ID:
      field = &(regs->sch.interrupt_mask_data_reg.restart_flow_event_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_MASK_DATA_REG_SMPFULL_LEVEL1_MASK_ID:
      field = &(regs->sch.interrupt_mask_data_reg.smpfull_level1_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_MASK_DATA_REG_SMPFULL_LEVEL2_MASK_ID:
      field = &(regs->sch.interrupt_mask_data_reg.smpfull_level2_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_MASK_DATA_REG_FCTFIFOOVF_MASK_ID:
      field = &(regs->sch.interrupt_mask_data_reg.fctfifoovf_mask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_MASK_DATA_REG_DHDECCMASK_ID:
      field = &(regs->sch.interrupt_mask_data_reg.dhdeccmask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_MASK_DATA_REG_DCDECCMASK_ID:
      field = &(regs->sch.interrupt_mask_data_reg.dcdeccmask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_MASK_DATA_REG_FLHHRECCMASK_ID:
      field = &(regs->sch.interrupt_mask_data_reg.flhhreccmask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_MASK_DATA_REG_FLTHRECCMASK_ID:
      field = &(regs->sch.interrupt_mask_data_reg.flthreccmask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_MASK_DATA_REG_FLHCLECCMASK_ID:
      field = &(regs->sch.interrupt_mask_data_reg.flhcleccmask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_MASK_DATA_REG_FLTCLECCMASK_ID:
      field = &(regs->sch.interrupt_mask_data_reg.fltcleccmask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_MASK_DATA_REG_FLHFQECCMASK_ID:
      field = &(regs->sch.interrupt_mask_data_reg.flhfqeccmask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_MASK_DATA_REG_FLTFQECCMASK_ID:
      field = &(regs->sch.interrupt_mask_data_reg.fltfqeccmask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_MASK_DATA_REG_FDMSECCMASK_ID:
      field = &(regs->sch.interrupt_mask_data_reg.fdmseccmask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_MASK_DATA_REG_FDMDECCMASK_ID:
      field = &(regs->sch.interrupt_mask_data_reg.fdmdeccmask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_MASK_DATA_REG_SHDSECCMASK_ID:
      field = &(regs->sch.interrupt_mask_data_reg.shdseccmask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_MASK_DATA_REG_SHDDECCMASK_ID:
      field = &(regs->sch.interrupt_mask_data_reg.shddeccmask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_MASK_DATA_REG_FQMECCMASK_ID:
      field = &(regs->sch.interrupt_mask_data_reg.fqmeccmask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_MASK_DATA_REG_SFLHECCMASK_ID:
      field = &(regs->sch.interrupt_mask_data_reg.sflheccmask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_MASK_DATA_REG_SFLTECCMASK_ID:
      field = &(regs->sch.interrupt_mask_data_reg.sflteccmask);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_MASK_DATA_REG_FSMECCMASK_ID:
      field = &(regs->sch.interrupt_mask_data_reg.fsmeccmask);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_indirect_command_wr_data_reg_2_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_indirect_command_wr_data_reg_2_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_WR_DATA_REG_2_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_WR_DATA_REG_2_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_WR_DATA_REG_2_REGISTER_ID:
      regis = &(regs->sch.indirect_command_wr_data_reg_2.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_WR_DATA_REG_2_INDIRECT_COMMAND_WR_DATA_ID:
      field = &(regs->sch.indirect_command_wr_data_reg_2.indirect_command_wr_data);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "sch.indirect_command_wr_data_reg_2: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_indirect_command_wr_data_reg_2_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_indirect_command_wr_data_reg_2_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_WR_DATA_REG_2_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_WR_DATA_REG_2_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_WR_DATA_REG_2_REGISTER_ID:
      regis = &(regs->sch.indirect_command_wr_data_reg_2.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_WR_DATA_REG_2_INDIRECT_COMMAND_WR_DATA_ID:
      field = &(regs->sch.indirect_command_wr_data_reg_2.indirect_command_wr_data);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_switch_cir_eir_in_dual_shapers_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_switch_cir_eir_in_dual_shapers_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_SWITCH_CIR_EIR_IN_DUAL_SHAPERS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_SWITCH_CIR_EIR_IN_DUAL_SHAPERS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_SWITCH_CIR_EIR_IN_DUAL_SHAPERS_REG_REGISTER_ID:
      regis = &(regs->sch.switch_cir_eir_in_dual_shapers_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SWITCH_CIR_EIR_IN_DUAL_SHAPERS_REG_SWITCH_CIREIR_ID:
      field = &(regs->sch.switch_cir_eir_in_dual_shapers_reg.switch_cireir);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "sch.switch_cir_eir_in_dual_shapers_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_switch_cir_eir_in_dual_shapers_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch");
  soc_sand_proc_name = "ui_fap21v_acc_sch_switch_cir_eir_in_dual_shapers_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_SWITCH_CIR_EIR_IN_DUAL_SHAPERS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_SCH_SWITCH_CIR_EIR_IN_DUAL_SHAPERS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_SCH_SWITCH_CIR_EIR_IN_DUAL_SHAPERS_REG_REGISTER_ID:
      regis = &(regs->sch.switch_cir_eir_in_dual_shapers_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_SCH_SWITCH_CIR_EIR_IN_DUAL_SHAPERS_REG_SWITCH_CIREIR_ID:
      field = &(regs->sch.switch_cir_eir_in_dual_shapers_reg.switch_cireir);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_get(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch_get");

  if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_CPU_AND_RECYCLE_INTERFACES_CONFIGURATION_CPU_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_cpu_and_recycle_interfaces_configuration_cpu_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_SELECT_FLOW_TO_QUEUE_MAPPING_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_select_flow_to_queue_mapping_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_SCH_0X3_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_credit_counter_configuration_reg_2_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_SPIA_CAL_CONFIGURATION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_spia_cal_configuration_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_AGGR_FC_3_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_force_aggr_fc_3_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_AGGR_FC_1_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_force_aggr_fc_1_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_HR_FLOW_CONTROL_MASK_3_2_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_hr_flow_control_mask_3_2_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_LOW_FC_0_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_force_low_fc_0_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_HR_FLOW_CONTROL_MASK_0_1_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_hr_flow_control_mask_0_1_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_DVS_LINK_STATUS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_dvs_link_status_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_WR_DATA_REG_0_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_indirect_command_wr_data_reg_0_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_LAST_FLOW_RESTART_EVENT_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_last_flow_restart_event_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_HIGH_FC_0_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_force_high_fc_0_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_SHAPER_CONFIGURATION_1_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_shaper_configuration_1_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_SMP_MESSAGE_COUNTER_AND_STATUS_CONFIGURATION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_smp_message_counter_and_status_configuration_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_SPIB_RATES_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_spib_rates_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_RD_DATA_REG_0_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_indirect_command_rd_data_reg_0_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_FSF_COMPOSITE_CONFIGURATION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_fsf_composite_configuration_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_interrupt_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_AGGR_FC_2_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_force_aggr_fc_2_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_CREDIT_COUNTER_CONFIGURATION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_credit_counter_configuration_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_SYSTEM_RED_CONFIGURATION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_system_red_configuration_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_HIGH_FC_1_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_force_high_fc_1_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_ATTEMPT_TO_ACTIVATE_FLOW_SCHEDULER_WITH_BAD_PARAMETERS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_attempt_to_activate_flow_scheduler_with_bad_parameters_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_CREDIT_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_credit_counter_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_SCL_SMP_MESSAGES_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_scl_smp_messages_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_HRPORT_EN_1_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_hrport_en_1_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_LOW_FC_1_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_force_low_fc_1_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_ATTEMPT_TO_ACTIVATE_A_SHAPER_WITH_BAD_PARAMETERS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_attempt_to_activate_a_shaper_with_bad_parameters_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_SCHEDULER_CONFIGURATION_REGISTER_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_scheduler_configuration_register_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_SPIB_CAL_CONFIGURATION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_spib_cal_configuration_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_ADDRESS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_indirect_command_address_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_DEVICE_SCHEDULER_DVS_CONFIG0_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_device_scheduler_dvs_config0_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_WR_DATA_REG_1_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_indirect_command_wr_data_reg_1_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_CPU_AND_RECYCLE_INTERFACES_CONFIGURATION_RECYCLE_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_cpu_and_recycle_interfaces_configuration_recycle_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_RD_DATA_REG_2_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_indirect_command_rd_data_reg_2_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_indirect_command_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_FC_CONFIGURATION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_force_fc_configuration_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_AGGR_FC_0_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_force_aggr_fc_0_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_DEVICE_SCHEDULER_DVS_WEIGHT_CONFIG_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_device_scheduler_dvs_weight_config_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_DEVICE_SCHEDULER_DVS_CONFIG1_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_device_scheduler_dvs_config1_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_SMP_MESSAGE_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_smp_message_counter_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_RD_DATA_REG_1_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_indirect_command_rd_data_reg_1_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_SPIA_RATES_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_spia_rates_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_HRPORT_EN_2_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_hrport_en_2_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_MASK_DATA_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_interrupt_mask_data_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_WR_DATA_REG_2_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_indirect_command_wr_data_reg_2_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_SWITCH_CIR_EIR_IN_DUAL_SHAPERS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_switch_cir_eir_in_dual_shapers_reg_get(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after sch***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_sch_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_sch_set");

  if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_CPU_AND_RECYCLE_INTERFACES_CONFIGURATION_CPU_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_cpu_and_recycle_interfaces_configuration_cpu_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_SELECT_FLOW_TO_QUEUE_MAPPING_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_select_flow_to_queue_mapping_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_SCH_0X3_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_credit_counter_configuration_reg_2_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_SPIA_CAL_CONFIGURATION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_spia_cal_configuration_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_AGGR_FC_3_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_force_aggr_fc_3_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_AGGR_FC_1_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_force_aggr_fc_1_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_HR_FLOW_CONTROL_MASK_3_2_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_hr_flow_control_mask_3_2_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_LOW_FC_0_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_force_low_fc_0_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_HR_FLOW_CONTROL_MASK_0_1_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_hr_flow_control_mask_0_1_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_DVS_LINK_STATUS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_dvs_link_status_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_WR_DATA_REG_0_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_indirect_command_wr_data_reg_0_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_LAST_FLOW_RESTART_EVENT_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_last_flow_restart_event_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_HIGH_FC_0_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_force_high_fc_0_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_SHAPER_CONFIGURATION_1_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_shaper_configuration_1_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_SMP_MESSAGE_COUNTER_AND_STATUS_CONFIGURATION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_smp_message_counter_and_status_configuration_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_SPIB_RATES_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_spib_rates_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_RD_DATA_REG_0_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_indirect_command_rd_data_reg_0_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_FSF_COMPOSITE_CONFIGURATION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_fsf_composite_configuration_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_interrupt_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_AGGR_FC_2_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_force_aggr_fc_2_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_CREDIT_COUNTER_CONFIGURATION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_credit_counter_configuration_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_SYSTEM_RED_CONFIGURATION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_system_red_configuration_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_HIGH_FC_1_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_force_high_fc_1_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_ATTEMPT_TO_ACTIVATE_FLOW_SCHEDULER_WITH_BAD_PARAMETERS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_attempt_to_activate_flow_scheduler_with_bad_parameters_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_CREDIT_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_credit_counter_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_SCL_SMP_MESSAGES_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_scl_smp_messages_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_HRPORT_EN_1_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_hrport_en_1_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_LOW_FC_1_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_force_low_fc_1_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_ATTEMPT_TO_ACTIVATE_A_SHAPER_WITH_BAD_PARAMETERS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_attempt_to_activate_a_shaper_with_bad_parameters_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_SCHEDULER_CONFIGURATION_REGISTER_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_scheduler_configuration_register_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_SPIB_CAL_CONFIGURATION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_spib_cal_configuration_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_ADDRESS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_indirect_command_address_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_DEVICE_SCHEDULER_DVS_CONFIG0_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_device_scheduler_dvs_config0_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_WR_DATA_REG_1_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_indirect_command_wr_data_reg_1_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_CPU_AND_RECYCLE_INTERFACES_CONFIGURATION_RECYCLE_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_cpu_and_recycle_interfaces_configuration_recycle_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_RD_DATA_REG_2_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_indirect_command_rd_data_reg_2_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_indirect_command_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_FC_CONFIGURATION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_force_fc_configuration_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_FORCE_AGGR_FC_0_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_force_aggr_fc_0_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_DEVICE_SCHEDULER_DVS_WEIGHT_CONFIG_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_device_scheduler_dvs_weight_config_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_DEVICE_SCHEDULER_DVS_CONFIG1_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_device_scheduler_dvs_config1_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_SMP_MESSAGE_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_smp_message_counter_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_RD_DATA_REG_1_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_indirect_command_rd_data_reg_1_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_SPIA_RATES_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_spia_rates_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_HRPORT_EN_2_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_hrport_en_2_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_INTERRUPT_MASK_DATA_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_interrupt_mask_data_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_INDIRECT_COMMAND_WR_DATA_REG_2_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_indirect_command_wr_data_reg_2_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_SCH_SWITCH_CIR_EIR_IN_DUAL_SHAPERS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_sch_switch_cir_eir_in_dual_shapers_reg_set(current_line, value);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after sch***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}
