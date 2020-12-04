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
  ui_fap21v_acc_qdp_zbt_physical_interface_configuration_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_zbt_physical_interface_configuration_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_ZBT_PHYSICAL_INTERFACE_CONFIGURATION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_ZBT_PHYSICAL_INTERFACE_CONFIGURATION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_ZBT_PHYSICAL_INTERFACE_CONFIGURATION_REG_REGISTER_ID:
      regis = &(regs->qdp.zbt_physical_interface_configuration_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_ZBT_PHYSICAL_INTERFACE_CONFIGURATION_REG_ZBTCFG_ID:
      field = &(regs->qdp.zbt_physical_interface_configuration_reg.zbtcfg);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "qdp.zbt_physical_interface_configuration_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_zbt_physical_interface_configuration_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_zbt_physical_interface_configuration_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_ZBT_PHYSICAL_INTERFACE_CONFIGURATION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_ZBT_PHYSICAL_INTERFACE_CONFIGURATION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_ZBT_PHYSICAL_INTERFACE_CONFIGURATION_REG_REGISTER_ID:
      regis = &(regs->qdp.zbt_physical_interface_configuration_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_ZBT_PHYSICAL_INTERFACE_CONFIGURATION_REG_ZBTCFG_ID:
      field = &(regs->qdp.zbt_physical_interface_configuration_reg.zbtcfg);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_enqueue_queued_total_packets_counter_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_enqueue_queued_total_packets_counter_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_ENQUEUE_QUEUED_TOTAL_PACKETS_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_ENQUEUE_QUEUED_TOTAL_PACKETS_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_ENQUEUE_QUEUED_TOTAL_PACKETS_COUNTER_REG_REGISTER_ID:
      regis = &(regs->qdp.enqueue_queued_total_packets_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_ENQUEUE_QUEUED_TOTAL_PACKETS_COUNTER_REG_EN_QPKT_CNT_ID:
      field = &(regs->qdp.enqueue_queued_total_packets_counter_reg.en_qpkt_cnt);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "qdp.enqueue_queued_total_packets_counter_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_enqueue_queued_total_packets_counter_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_enqueue_queued_total_packets_counter_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_ENQUEUE_QUEUED_TOTAL_PACKETS_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_ENQUEUE_QUEUED_TOTAL_PACKETS_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_ENQUEUE_QUEUED_TOTAL_PACKETS_COUNTER_REG_REGISTER_ID:
      regis = &(regs->qdp.enqueue_queued_total_packets_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_ENQUEUE_QUEUED_TOTAL_PACKETS_COUNTER_REG_EN_QPKT_CNT_ID:
      field = &(regs->qdp.enqueue_queued_total_packets_counter_reg.en_qpkt_cnt);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_programmable_counter_queue_selection_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_programmable_counter_queue_selection_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_COUNTER_QUEUE_SELECTION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_COUNTER_QUEUE_SELECTION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_COUNTER_QUEUE_SELECTION_REG_REGISTER_ID:
      regis = &(regs->qdp.programmable_counter_queue_selection_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_COUNTER_QUEUE_SELECTION_REG_PRG_CNT_TARGET_Q_ID:
      field = &(regs->qdp.programmable_counter_queue_selection_reg.prg_cnt_target_q);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_COUNTER_QUEUE_SELECTION_REG_PRG_CNT_TARGET_Q_MASK_ID:
      field = &(regs->qdp.programmable_counter_queue_selection_reg.prg_cnt_target_q_mask);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "qdp.programmable_counter_queue_selection_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_programmable_counter_queue_selection_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_programmable_counter_queue_selection_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_COUNTER_QUEUE_SELECTION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_COUNTER_QUEUE_SELECTION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_COUNTER_QUEUE_SELECTION_REG_REGISTER_ID:
      regis = &(regs->qdp.programmable_counter_queue_selection_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_COUNTER_QUEUE_SELECTION_REG_PRG_CNT_TARGET_Q_ID:
      field = &(regs->qdp.programmable_counter_queue_selection_reg.prg_cnt_target_q);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_COUNTER_QUEUE_SELECTION_REG_PRG_CNT_TARGET_Q_MASK_ID:
      field = &(regs->qdp.programmable_counter_queue_selection_reg.prg_cnt_target_q_mask);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_interrupts_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_interrupts_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_REGISTER_ID:
      regis = &(regs->qdp.interrupts_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_DPTF0_ID:
      field = &(regs->qdp.interrupts_reg.dptf0);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_DPTF1_ID:
      field = &(regs->qdp.interrupts_reg.dptf1);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_DPTF2_ID:
      field = &(regs->qdp.interrupts_reg.dptf2);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_DPTF3_ID:
      field = &(regs->qdp.interrupts_reg.dptf3);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_NVALID_HDR_ID:
      field = &(regs->qdp.interrupts_reg.nvalid_hdr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_SFT_ERR_RD_DESC_ID:
      field = &(regs->qdp.interrupts_reg.sft_err_rd_desc);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_SFT_ERR_WR_DESC_ID:
      field = &(regs->qdp.interrupts_reg.sft_err_wr_desc);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_SFT_ERR_INS_QSIZE_ID:
      field = &(regs->qdp.interrupts_reg.sft_err_ins_qsize);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_SFT_ERR_QEMPTY_ID:
      field = &(regs->qdp.interrupts_reg.sft_err_qempty);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_QROLL_OVER_ID:
      field = &(regs->qdp.interrupts_reg.qroll_over);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_SFT_ERR_ZBT_ID:
      field = &(regs->qdp.interrupts_reg.sft_err_zbt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_SFT_ERR_BUFF_QID_ID:
      field = &(regs->qdp.interrupts_reg.sft_err_buff_qid);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_OVF_BUFF_QID_ID:
      field = &(regs->qdp.interrupts_reg.ovf_buff_qid);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_UDF_BUFF_QID_ID:
      field = &(regs->qdp.interrupts_reg.udf_buff_qid);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_EN_QPKT_CNT_O_ID:
      field = &(regs->qdp.interrupts_reg.en_qpkt_cnt_o);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_EN_QWORD_CNT_O_ID:
      field = &(regs->qdp.interrupts_reg.en_qword_cnt_o);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_HEAD_DEL_PKT_CNT_O_ID:
      field = &(regs->qdp.interrupts_reg.head_del_pkt_cnt_o);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_HEAD_DEL_WORD_CNT_O_ID:
      field = &(regs->qdp.interrupts_reg.head_del_word_cnt_o);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_DE_QPKT_CNT_O_ID:
      field = &(regs->qdp.interrupts_reg.de_qpkt_cnt_o);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_DE_QWORD_CNT_O_ID:
      field = &(regs->qdp.interrupts_reg.de_qword_cnt_o);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_TAIL_DEL_PKT_CNT_O_ID:
      field = &(regs->qdp.interrupts_reg.tail_del_pkt_cnt_o);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_TAIL_DEL_WORD_CNT_O_ID:
      field = &(regs->qdp.interrupts_reg.tail_del_word_cnt_o);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_PRG_EN_QPKT_CNT_O_ID:
      field = &(regs->qdp.interrupts_reg.prg_en_qpkt_cnt_o);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_PRG_EN_QWORD_CNT_O_ID:
      field = &(regs->qdp.interrupts_reg.prg_en_qword_cnt_o);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_PRG_HEAD_DEL_PKT_CNT_O_ID:
      field = &(regs->qdp.interrupts_reg.prg_head_del_pkt_cnt_o);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_PRG_HEAD_DEL_WORD_CNT_O_ID:
      field = &(regs->qdp.interrupts_reg.prg_head_del_word_cnt_o);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_PRG_DE_QPKT_CNT_O_ID:
      field = &(regs->qdp.interrupts_reg.prg_de_qpkt_cnt_o);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_PRG_DE_QWORD_CNT_O_ID:
      field = &(regs->qdp.interrupts_reg.prg_de_qword_cnt_o);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_PRG_TAIL_DEL_PKT_CNT_O_ID:
      field = &(regs->qdp.interrupts_reg.prg_tail_del_pkt_cnt_o);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_PRG_TAIL_DEL_WORD_CNT_O_ID:
      field = &(regs->qdp.interrupts_reg.prg_tail_del_word_cnt_o);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_NV_PKT_CNT_O_ID:
      field = &(regs->qdp.interrupts_reg.nv_pkt_cnt_o);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "qdp.interrupts_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_interrupts_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_interrupts_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_REGISTER_ID:
      regis = &(regs->qdp.interrupts_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_DPTF0_ID:
      field = &(regs->qdp.interrupts_reg.dptf0);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_DPTF1_ID:
      field = &(regs->qdp.interrupts_reg.dptf1);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_DPTF2_ID:
      field = &(regs->qdp.interrupts_reg.dptf2);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_DPTF3_ID:
      field = &(regs->qdp.interrupts_reg.dptf3);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_NVALID_HDR_ID:
      field = &(regs->qdp.interrupts_reg.nvalid_hdr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_SFT_ERR_RD_DESC_ID:
      field = &(regs->qdp.interrupts_reg.sft_err_rd_desc);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_SFT_ERR_WR_DESC_ID:
      field = &(regs->qdp.interrupts_reg.sft_err_wr_desc);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_SFT_ERR_INS_QSIZE_ID:
      field = &(regs->qdp.interrupts_reg.sft_err_ins_qsize);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_SFT_ERR_QEMPTY_ID:
      field = &(regs->qdp.interrupts_reg.sft_err_qempty);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_QROLL_OVER_ID:
      field = &(regs->qdp.interrupts_reg.qroll_over);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_SFT_ERR_ZBT_ID:
      field = &(regs->qdp.interrupts_reg.sft_err_zbt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_SFT_ERR_BUFF_QID_ID:
      field = &(regs->qdp.interrupts_reg.sft_err_buff_qid);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_OVF_BUFF_QID_ID:
      field = &(regs->qdp.interrupts_reg.ovf_buff_qid);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_UDF_BUFF_QID_ID:
      field = &(regs->qdp.interrupts_reg.udf_buff_qid);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_EN_QPKT_CNT_O_ID:
      field = &(regs->qdp.interrupts_reg.en_qpkt_cnt_o);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_EN_QWORD_CNT_O_ID:
      field = &(regs->qdp.interrupts_reg.en_qword_cnt_o);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_HEAD_DEL_PKT_CNT_O_ID:
      field = &(regs->qdp.interrupts_reg.head_del_pkt_cnt_o);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_HEAD_DEL_WORD_CNT_O_ID:
      field = &(regs->qdp.interrupts_reg.head_del_word_cnt_o);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_DE_QPKT_CNT_O_ID:
      field = &(regs->qdp.interrupts_reg.de_qpkt_cnt_o);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_DE_QWORD_CNT_O_ID:
      field = &(regs->qdp.interrupts_reg.de_qword_cnt_o);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_TAIL_DEL_PKT_CNT_O_ID:
      field = &(regs->qdp.interrupts_reg.tail_del_pkt_cnt_o);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_TAIL_DEL_WORD_CNT_O_ID:
      field = &(regs->qdp.interrupts_reg.tail_del_word_cnt_o);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_PRG_EN_QPKT_CNT_O_ID:
      field = &(regs->qdp.interrupts_reg.prg_en_qpkt_cnt_o);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_PRG_EN_QWORD_CNT_O_ID:
      field = &(regs->qdp.interrupts_reg.prg_en_qword_cnt_o);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_PRG_HEAD_DEL_PKT_CNT_O_ID:
      field = &(regs->qdp.interrupts_reg.prg_head_del_pkt_cnt_o);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_PRG_HEAD_DEL_WORD_CNT_O_ID:
      field = &(regs->qdp.interrupts_reg.prg_head_del_word_cnt_o);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_PRG_DE_QPKT_CNT_O_ID:
      field = &(regs->qdp.interrupts_reg.prg_de_qpkt_cnt_o);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_PRG_DE_QWORD_CNT_O_ID:
      field = &(regs->qdp.interrupts_reg.prg_de_qword_cnt_o);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_PRG_TAIL_DEL_PKT_CNT_O_ID:
      field = &(regs->qdp.interrupts_reg.prg_tail_del_pkt_cnt_o);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_PRG_TAIL_DEL_WORD_CNT_O_ID:
      field = &(regs->qdp.interrupts_reg.prg_tail_del_word_cnt_o);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_NV_PKT_CNT_O_ID:
      field = &(regs->qdp.interrupts_reg.nv_pkt_cnt_o);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_enqueue_discarded_total_words_counter_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_enqueue_discarded_total_words_counter_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_ENQUEUE_DISCARDED_TOTAL_WORDS_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_ENQUEUE_DISCARDED_TOTAL_WORDS_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_ENQUEUE_DISCARDED_TOTAL_WORDS_COUNTER_REG_REGISTER_ID:
      regis = &(regs->qdp.enqueue_discarded_total_words_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_ENQUEUE_DISCARDED_TOTAL_WORDS_COUNTER_REG_HD_DEL_WRD_CNT_ID:
      field = &(regs->qdp.enqueue_discarded_total_words_counter_reg.hd_del_wrd_cnt);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "qdp.enqueue_discarded_total_words_counter_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_enqueue_discarded_total_words_counter_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_enqueue_discarded_total_words_counter_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_ENQUEUE_DISCARDED_TOTAL_WORDS_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_ENQUEUE_DISCARDED_TOTAL_WORDS_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_ENQUEUE_DISCARDED_TOTAL_WORDS_COUNTER_REG_REGISTER_ID:
      regis = &(regs->qdp.enqueue_discarded_total_words_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_ENQUEUE_DISCARDED_TOTAL_WORDS_COUNTER_REG_HD_DEL_WRD_CNT_ID:
      field = &(regs->qdp.enqueue_discarded_total_words_counter_reg.hd_del_wrd_cnt);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_qid_overflow_block_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_qid_overflow_block_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_QID_OVERFLOW_BLOCK_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_QID_OVERFLOW_BLOCK_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_QID_OVERFLOW_BLOCK_REG_REGISTER_ID:
      regis = &(regs->qdp.qid_overflow_block_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_QID_OVERFLOW_BLOCK_REG_OVF_BUFF_QIDBLK_ID:
      field = &(regs->qdp.qid_overflow_block_reg.ovf_buff_qidblk);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "qdp.qid_overflow_block_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_qid_overflow_block_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_qid_overflow_block_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_QID_OVERFLOW_BLOCK_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_QID_OVERFLOW_BLOCK_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_QID_OVERFLOW_BLOCK_REG_REGISTER_ID:
      regis = &(regs->qdp.qid_overflow_block_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_QID_OVERFLOW_BLOCK_REG_OVF_BUFF_QIDBLK_ID:
      field = &(regs->qdp.qid_overflow_block_reg.ovf_buff_qidblk);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_initialization_trigger_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_initialization_trigger_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_INITIALIZATION_TRIGGER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_INITIALIZATION_TRIGGER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_INITIALIZATION_TRIGGER_REG_REGISTER_ID:
      regis = &(regs->qdp.initialization_trigger_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INITIALIZATION_TRIGGER_REG_INIT_TRG_ID:
      field = &(regs->qdp.initialization_trigger_reg.init_trg);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "qdp.initialization_trigger_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_initialization_trigger_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_initialization_trigger_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_INITIALIZATION_TRIGGER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_INITIALIZATION_TRIGGER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_INITIALIZATION_TRIGGER_REG_REGISTER_ID:
      regis = &(regs->qdp.initialization_trigger_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INITIALIZATION_TRIGGER_REG_INIT_TRG_ID:
      field = &(regs->qdp.initialization_trigger_reg.init_trg);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_mask_interrupts_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_mask_interrupts_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_REGISTER_ID:
      regis = &(regs->qdp.mask_interrupts_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_DPTF0_ID:
      field = &(regs->qdp.mask_interrupts_reg.dptf0);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_DPTF1_ID:
      field = &(regs->qdp.mask_interrupts_reg.dptf1);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_DPTF2_ID:
      field = &(regs->qdp.mask_interrupts_reg.dptf2);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_DPTF3_ID:
      field = &(regs->qdp.mask_interrupts_reg.dptf3);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_NVALID_HDR_ID:
      field = &(regs->qdp.mask_interrupts_reg.nvalid_hdr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_SFT_ERR_RD_DESC_MASK_INT_ID:
      field = &(regs->qdp.mask_interrupts_reg.sft_err_rd_desc_mask_int);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_SFT_ERR_WR_DESC_MASK_INT_ID:
      field = &(regs->qdp.mask_interrupts_reg.sft_err_wr_desc_mask_int);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_SFT_ERR_INS_QUE_SIZE_MASK_INT_ID:
      field = &(regs->qdp.mask_interrupts_reg.sft_err_ins_que_size_mask_int);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_SFT_ERR_QUEUE_EMPTY_MASK_INT_ID:
      field = &(regs->qdp.mask_interrupts_reg.sft_err_queue_empty_mask_int);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_QROLL_OVER_MASK_INT_ID:
      field = &(regs->qdp.mask_interrupts_reg.qroll_over_mask_int);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_SFT_ERR_ZBTMASK_INT_ID:
      field = &(regs->qdp.mask_interrupts_reg.sft_err_zbtmask_int);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_SFT_ERR_BUFF_QUE_IDMASK_INT_ID:
      field = &(regs->qdp.mask_interrupts_reg.sft_err_buff_que_idmask_int);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_OVF_BUFF_QID_ID:
      field = &(regs->qdp.mask_interrupts_reg.ovf_buff_qid);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_UDF_BUFF_QID_ID:
      field = &(regs->qdp.mask_interrupts_reg.udf_buff_qid);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_EN_QPKT_CNT_OMASK_INT_ID:
      field = &(regs->qdp.mask_interrupts_reg.en_qpkt_cnt_omask_int);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_EN_QWORD_CNT_OMASK_INT_ID:
      field = &(regs->qdp.mask_interrupts_reg.en_qword_cnt_omask_int);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_HD_DEL_PKT_CNT_OMASK_INT_ID:
      field = &(regs->qdp.mask_interrupts_reg.hd_del_pkt_cnt_omask_int);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_HD_DEL_WORD_CNT_OMASK_INT_ID:
      field = &(regs->qdp.mask_interrupts_reg.hd_del_word_cnt_omask_int);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_DE_QPKT_CNT_OMASK_INT_ID:
      field = &(regs->qdp.mask_interrupts_reg.de_qpkt_cnt_omask_int);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_DE_QPKT_WRD_OMASK_INT_ID:
      field = &(regs->qdp.mask_interrupts_reg.de_qpkt_wrd_omask_int);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_TAIL_DEL_PKT_CNT_OMASK_INT_ID:
      field = &(regs->qdp.mask_interrupts_reg.tail_del_pkt_cnt_omask_int);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_TAIL_DEL_WRD_CNT_OMASK_INT_ID:
      field = &(regs->qdp.mask_interrupts_reg.tail_del_wrd_cnt_omask_int);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_PRG_EN_QPKT_CNT_OMASK_INT_ID:
      field = &(regs->qdp.mask_interrupts_reg.prg_en_qpkt_cnt_omask_int);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_PRG_EN_QWRD_CNT_OMASK_INT_ID:
      field = &(regs->qdp.mask_interrupts_reg.prg_en_qwrd_cnt_omask_int);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_PRG_HD_DEL_PKT_CNT_OMASK_INT_ID:
      field = &(regs->qdp.mask_interrupts_reg.prg_hd_del_pkt_cnt_omask_int);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_PRG_HD_DEL_WRD_CNT_OMASK_INT_ID:
      field = &(regs->qdp.mask_interrupts_reg.prg_hd_del_wrd_cnt_omask_int);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_PRG_DE_QPKT_CNT_OMASK_INT_ID:
      field = &(regs->qdp.mask_interrupts_reg.prg_de_qpkt_cnt_omask_int);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_PRG_DE_QPKT_WRD_OMASK_INT_ID:
      field = &(regs->qdp.mask_interrupts_reg.prg_de_qpkt_wrd_omask_int);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_PRG_TAIL_DEL_PKT_CNT_OMASK_INT_ID:
      field = &(regs->qdp.mask_interrupts_reg.prg_tail_del_pkt_cnt_omask_int);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_NV_PKT_CNT_MASK_INT_ID:
      field = &(regs->qdp.mask_interrupts_reg.nv_pkt_cnt_mask_int);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "qdp.mask_interrupts_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_mask_interrupts_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_mask_interrupts_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_REGISTER_ID:
      regis = &(regs->qdp.mask_interrupts_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_DPTF0_ID:
      field = &(regs->qdp.mask_interrupts_reg.dptf0);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_DPTF1_ID:
      field = &(regs->qdp.mask_interrupts_reg.dptf1);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_DPTF2_ID:
      field = &(regs->qdp.mask_interrupts_reg.dptf2);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_DPTF3_ID:
      field = &(regs->qdp.mask_interrupts_reg.dptf3);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_NVALID_HDR_ID:
      field = &(regs->qdp.mask_interrupts_reg.nvalid_hdr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_SFT_ERR_RD_DESC_MASK_INT_ID:
      field = &(regs->qdp.mask_interrupts_reg.sft_err_rd_desc_mask_int);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_SFT_ERR_WR_DESC_MASK_INT_ID:
      field = &(regs->qdp.mask_interrupts_reg.sft_err_wr_desc_mask_int);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_SFT_ERR_INS_QUE_SIZE_MASK_INT_ID:
      field = &(regs->qdp.mask_interrupts_reg.sft_err_ins_que_size_mask_int);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_SFT_ERR_QUEUE_EMPTY_MASK_INT_ID:
      field = &(regs->qdp.mask_interrupts_reg.sft_err_queue_empty_mask_int);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_QROLL_OVER_MASK_INT_ID:
      field = &(regs->qdp.mask_interrupts_reg.qroll_over_mask_int);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_SFT_ERR_ZBTMASK_INT_ID:
      field = &(regs->qdp.mask_interrupts_reg.sft_err_zbtmask_int);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_SFT_ERR_BUFF_QUE_IDMASK_INT_ID:
      field = &(regs->qdp.mask_interrupts_reg.sft_err_buff_que_idmask_int);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_OVF_BUFF_QID_ID:
      field = &(regs->qdp.mask_interrupts_reg.ovf_buff_qid);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_UDF_BUFF_QID_ID:
      field = &(regs->qdp.mask_interrupts_reg.udf_buff_qid);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_EN_QPKT_CNT_OMASK_INT_ID:
      field = &(regs->qdp.mask_interrupts_reg.en_qpkt_cnt_omask_int);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_EN_QWORD_CNT_OMASK_INT_ID:
      field = &(regs->qdp.mask_interrupts_reg.en_qword_cnt_omask_int);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_HD_DEL_PKT_CNT_OMASK_INT_ID:
      field = &(regs->qdp.mask_interrupts_reg.hd_del_pkt_cnt_omask_int);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_HD_DEL_WORD_CNT_OMASK_INT_ID:
      field = &(regs->qdp.mask_interrupts_reg.hd_del_word_cnt_omask_int);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_DE_QPKT_CNT_OMASK_INT_ID:
      field = &(regs->qdp.mask_interrupts_reg.de_qpkt_cnt_omask_int);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_DE_QPKT_WRD_OMASK_INT_ID:
      field = &(regs->qdp.mask_interrupts_reg.de_qpkt_wrd_omask_int);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_TAIL_DEL_PKT_CNT_OMASK_INT_ID:
      field = &(regs->qdp.mask_interrupts_reg.tail_del_pkt_cnt_omask_int);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_TAIL_DEL_WRD_CNT_OMASK_INT_ID:
      field = &(regs->qdp.mask_interrupts_reg.tail_del_wrd_cnt_omask_int);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_PRG_EN_QPKT_CNT_OMASK_INT_ID:
      field = &(regs->qdp.mask_interrupts_reg.prg_en_qpkt_cnt_omask_int);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_PRG_EN_QWRD_CNT_OMASK_INT_ID:
      field = &(regs->qdp.mask_interrupts_reg.prg_en_qwrd_cnt_omask_int);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_PRG_HD_DEL_PKT_CNT_OMASK_INT_ID:
      field = &(regs->qdp.mask_interrupts_reg.prg_hd_del_pkt_cnt_omask_int);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_PRG_HD_DEL_WRD_CNT_OMASK_INT_ID:
      field = &(regs->qdp.mask_interrupts_reg.prg_hd_del_wrd_cnt_omask_int);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_PRG_DE_QPKT_CNT_OMASK_INT_ID:
      field = &(regs->qdp.mask_interrupts_reg.prg_de_qpkt_cnt_omask_int);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_PRG_DE_QPKT_WRD_OMASK_INT_ID:
      field = &(regs->qdp.mask_interrupts_reg.prg_de_qpkt_wrd_omask_int);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_PRG_TAIL_DEL_PKT_CNT_OMASK_INT_ID:
      field = &(regs->qdp.mask_interrupts_reg.prg_tail_del_pkt_cnt_omask_int);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_NV_PKT_CNT_MASK_INT_ID:
      field = &(regs->qdp.mask_interrupts_reg.nv_pkt_cnt_mask_int);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_programmable_dequeue_total_packets_counter_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_programmable_dequeue_total_packets_counter_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_DEQUEUE_TOTAL_PACKETS_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_DEQUEUE_TOTAL_PACKETS_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_DEQUEUE_TOTAL_PACKETS_COUNTER_REG_REGISTER_ID:
      regis = &(regs->qdp.programmable_dequeue_total_packets_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_DEQUEUE_TOTAL_PACKETS_COUNTER_REG_PRG_DE_QPKT_CNT_ID:
      field = &(regs->qdp.programmable_dequeue_total_packets_counter_reg.prg_de_qpkt_cnt);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "qdp.programmable_dequeue_total_packets_counter_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_programmable_dequeue_total_packets_counter_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_programmable_dequeue_total_packets_counter_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_DEQUEUE_TOTAL_PACKETS_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_DEQUEUE_TOTAL_PACKETS_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_DEQUEUE_TOTAL_PACKETS_COUNTER_REG_REGISTER_ID:
      regis = &(regs->qdp.programmable_dequeue_total_packets_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_DEQUEUE_TOTAL_PACKETS_COUNTER_REG_PRG_DE_QPKT_CNT_ID:
      field = &(regs->qdp.programmable_dequeue_total_packets_counter_reg.prg_de_qpkt_cnt);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_global_buffers_consumbtion_reject_enable_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_global_buffers_consumbtion_reject_enable_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_GLOBAL_BUFFERS_CONSUMBTION_REJECT_ENABLE_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_GLOBAL_BUFFERS_CONSUMBTION_REJECT_ENABLE_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_GLOBAL_BUFFERS_CONSUMBTION_REJECT_ENABLE_REG_REGISTER_ID:
      regis = &(regs->qdp.global_buffers_consumbtion_reject_enable_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_GLOBAL_BUFFERS_CONSUMBTION_REJECT_ENABLE_REG_REJECT_ENA_ID:
      field = &(regs->qdp.global_buffers_consumbtion_reject_enable_reg.reject_ena);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "qdp.global_buffers_consumbtion_reject_enable_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_global_buffers_consumbtion_reject_enable_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_global_buffers_consumbtion_reject_enable_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_GLOBAL_BUFFERS_CONSUMBTION_REJECT_ENABLE_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_GLOBAL_BUFFERS_CONSUMBTION_REJECT_ENABLE_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_GLOBAL_BUFFERS_CONSUMBTION_REJECT_ENABLE_REG_REGISTER_ID:
      regis = &(regs->qdp.global_buffers_consumbtion_reject_enable_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_GLOBAL_BUFFERS_CONSUMBTION_REJECT_ENABLE_REG_REJECT_ENA_ID:
      field = &(regs->qdp.global_buffers_consumbtion_reject_enable_reg.reject_ena);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_indirect_command_wr_data_reg_0_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_indirect_command_wr_data_reg_0_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_INDIRECT_COMMAND_WR_DATA_REG_0_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_INDIRECT_COMMAND_WR_DATA_REG_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_INDIRECT_COMMAND_WR_DATA_REG_0_REGISTER_ID:
      regis = &(regs->qdp.indirect_command_wr_data_reg_0.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INDIRECT_COMMAND_WR_DATA_REG_0_INDIRECT_COMMAND_WR_DATA_ID:
      field = &(regs->qdp.indirect_command_wr_data_reg_0.indirect_command_wr_data);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "qdp.indirect_command_wr_data_reg_0: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_indirect_command_wr_data_reg_0_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_indirect_command_wr_data_reg_0_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_INDIRECT_COMMAND_WR_DATA_REG_0_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_INDIRECT_COMMAND_WR_DATA_REG_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_INDIRECT_COMMAND_WR_DATA_REG_0_REGISTER_ID:
      regis = &(regs->qdp.indirect_command_wr_data_reg_0.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INDIRECT_COMMAND_WR_DATA_REG_0_INDIRECT_COMMAND_WR_DATA_ID:
      field = &(regs->qdp.indirect_command_wr_data_reg_0.indirect_command_wr_data);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_enqueue_discarded_total_packets_counter_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_enqueue_discarded_total_packets_counter_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_ENQUEUE_DISCARDED_TOTAL_PACKETS_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_ENQUEUE_DISCARDED_TOTAL_PACKETS_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_ENQUEUE_DISCARDED_TOTAL_PACKETS_COUNTER_REG_REGISTER_ID:
      regis = &(regs->qdp.enqueue_discarded_total_packets_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_ENQUEUE_DISCARDED_TOTAL_PACKETS_COUNTER_REG_HD_DEL_PKT_CNT_ID:
      field = &(regs->qdp.enqueue_discarded_total_packets_counter_reg.hd_del_pkt_cnt);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "qdp.enqueue_discarded_total_packets_counter_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_enqueue_discarded_total_packets_counter_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_enqueue_discarded_total_packets_counter_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_ENQUEUE_DISCARDED_TOTAL_PACKETS_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_ENQUEUE_DISCARDED_TOTAL_PACKETS_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_ENQUEUE_DISCARDED_TOTAL_PACKETS_COUNTER_REG_REGISTER_ID:
      regis = &(regs->qdp.enqueue_discarded_total_packets_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_ENQUEUE_DISCARDED_TOTAL_PACKETS_COUNTER_REG_HD_DEL_PKT_CNT_ID:
      field = &(regs->qdp.enqueue_discarded_total_packets_counter_reg.hd_del_pkt_cnt);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_indirect_command_rd_data_reg_0_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_indirect_command_rd_data_reg_0_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_INDIRECT_COMMAND_RD_DATA_REG_0_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_INDIRECT_COMMAND_RD_DATA_REG_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_INDIRECT_COMMAND_RD_DATA_REG_0_REGISTER_ID:
      regis = &(regs->qdp.indirect_command_rd_data_reg_0.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INDIRECT_COMMAND_RD_DATA_REG_0_INDIRECT_COMMAND_RD_DATA_ID:
      field = &(regs->qdp.indirect_command_rd_data_reg_0.indirect_command_rd_data);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "qdp.indirect_command_rd_data_reg_0: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_indirect_command_rd_data_reg_0_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_indirect_command_rd_data_reg_0_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_INDIRECT_COMMAND_RD_DATA_REG_0_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_INDIRECT_COMMAND_RD_DATA_REG_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_INDIRECT_COMMAND_RD_DATA_REG_0_REGISTER_ID:
      regis = &(regs->qdp.indirect_command_rd_data_reg_0.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INDIRECT_COMMAND_RD_DATA_REG_0_INDIRECT_COMMAND_RD_DATA_ID:
      field = &(regs->qdp.indirect_command_rd_data_reg_0.indirect_command_rd_data);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_dequeue_total_packets_counter_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_dequeue_total_packets_counter_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_DEQUEUE_TOTAL_PACKETS_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_DEQUEUE_TOTAL_PACKETS_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_DEQUEUE_TOTAL_PACKETS_COUNTER_REG_REGISTER_ID:
      regis = &(regs->qdp.dequeue_total_packets_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_DEQUEUE_TOTAL_PACKETS_COUNTER_REG_DE_QPKT_CNT_ID:
      field = &(regs->qdp.dequeue_total_packets_counter_reg.de_qpkt_cnt);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "qdp.dequeue_total_packets_counter_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_dequeue_total_packets_counter_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_dequeue_total_packets_counter_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_DEQUEUE_TOTAL_PACKETS_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_DEQUEUE_TOTAL_PACKETS_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_DEQUEUE_TOTAL_PACKETS_COUNTER_REG_REGISTER_ID:
      regis = &(regs->qdp.dequeue_total_packets_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_DEQUEUE_TOTAL_PACKETS_COUNTER_REG_DE_QPKT_CNT_ID:
      field = &(regs->qdp.dequeue_total_packets_counter_reg.de_qpkt_cnt);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_programmable_dequeue_discarded_total_packets_counter_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_programmable_dequeue_discarded_total_packets_counter_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_DEQUEUE_DISCARDED_TOTAL_PACKETS_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_DEQUEUE_DISCARDED_TOTAL_PACKETS_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_DEQUEUE_DISCARDED_TOTAL_PACKETS_COUNTER_REG_REGISTER_ID:
      regis = &(regs->qdp.programmable_dequeue_discarded_total_packets_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_DEQUEUE_DISCARDED_TOTAL_PACKETS_COUNTER_REG_PRG_TAIL_DEL_PKT_CNT_ID:
      field = &(regs->qdp.programmable_dequeue_discarded_total_packets_counter_reg.prg_tail_del_pkt_cnt);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "qdp.programmable_dequeue_discarded_total_packets_counter_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_programmable_dequeue_discarded_total_packets_counter_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_programmable_dequeue_discarded_total_packets_counter_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_DEQUEUE_DISCARDED_TOTAL_PACKETS_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_DEQUEUE_DISCARDED_TOTAL_PACKETS_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_DEQUEUE_DISCARDED_TOTAL_PACKETS_COUNTER_REG_REGISTER_ID:
      regis = &(regs->qdp.programmable_dequeue_discarded_total_packets_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_DEQUEUE_DISCARDED_TOTAL_PACKETS_COUNTER_REG_PRG_TAIL_DEL_PKT_CNT_ID:
      field = &(regs->qdp.programmable_dequeue_discarded_total_packets_counter_reg.prg_tail_del_pkt_cnt);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_buffers_consumption_block_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_buffers_consumption_block_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_BUFFERS_CONSUMPTION_BLOCK_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_BUFFERS_CONSUMPTION_BLOCK_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_BUFFERS_CONSUMPTION_BLOCK_REG_REGISTER_ID:
      regis = &(regs->qdp.buffers_consumption_block_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_BUFFERS_CONSUMPTION_BLOCK_REG_REJECT_STATE_ID:
      field = &(regs->qdp.buffers_consumption_block_reg.reject_state);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_BUFFERS_CONSUMPTION_BLOCK_REG_BLOCK0_STATE_ID:
      field = &(regs->qdp.buffers_consumption_block_reg.block0_state);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_BUFFERS_CONSUMPTION_BLOCK_REG_BLOCK1_STATE_ID:
      field = &(regs->qdp.buffers_consumption_block_reg.block1_state);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_BUFFERS_CONSUMPTION_BLOCK_REG_BLOCK2_STATE_ID:
      field = &(regs->qdp.buffers_consumption_block_reg.block2_state);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_BUFFERS_CONSUMPTION_BLOCK_REG_BLOCK3_STATE_ID:
      field = &(regs->qdp.buffers_consumption_block_reg.block3_state);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "qdp.buffers_consumption_block_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_buffers_consumption_block_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_buffers_consumption_block_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_BUFFERS_CONSUMPTION_BLOCK_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_BUFFERS_CONSUMPTION_BLOCK_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_BUFFERS_CONSUMPTION_BLOCK_REG_REGISTER_ID:
      regis = &(regs->qdp.buffers_consumption_block_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_BUFFERS_CONSUMPTION_BLOCK_REG_REJECT_STATE_ID:
      field = &(regs->qdp.buffers_consumption_block_reg.reject_state);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_BUFFERS_CONSUMPTION_BLOCK_REG_BLOCK0_STATE_ID:
      field = &(regs->qdp.buffers_consumption_block_reg.block0_state);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_BUFFERS_CONSUMPTION_BLOCK_REG_BLOCK1_STATE_ID:
      field = &(regs->qdp.buffers_consumption_block_reg.block1_state);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_BUFFERS_CONSUMPTION_BLOCK_REG_BLOCK2_STATE_ID:
      field = &(regs->qdp.buffers_consumption_block_reg.block2_state);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_BUFFERS_CONSUMPTION_BLOCK_REG_BLOCK3_STATE_ID:
      field = &(regs->qdp.buffers_consumption_block_reg.block3_state);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_total_buffers_consumed_block_0_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_total_buffers_consumed_block_0_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_TOTAL_BUFFERS_CONSUMED_BLOCK_0_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_TOTAL_BUFFERS_CONSUMED_BLOCK_0_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_TOTAL_BUFFERS_CONSUMED_BLOCK_0_REG_REGISTER_ID:
      regis = &(regs->qdp.total_buffers_consumed_block_0_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_TOTAL_BUFFERS_CONSUMED_BLOCK_0_REG_TOT_BUFFERS0_ID:
      field = &(regs->qdp.total_buffers_consumed_block_0_reg.tot_buffers0);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "qdp.total_buffers_consumed_block_0_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_total_buffers_consumed_block_0_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_total_buffers_consumed_block_0_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_TOTAL_BUFFERS_CONSUMED_BLOCK_0_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_TOTAL_BUFFERS_CONSUMED_BLOCK_0_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_TOTAL_BUFFERS_CONSUMED_BLOCK_0_REG_REGISTER_ID:
      regis = &(regs->qdp.total_buffers_consumed_block_0_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_TOTAL_BUFFERS_CONSUMED_BLOCK_0_REG_TOT_BUFFERS0_ID:
      field = &(regs->qdp.total_buffers_consumed_block_0_reg.tot_buffers0);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_last_queue_assigned_to_block_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_last_queue_assigned_to_block_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_LAST_QUEUE_ASSIGNED_TO_BLOCK_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_LAST_QUEUE_ASSIGNED_TO_BLOCK_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_LAST_QUEUE_ASSIGNED_TO_BLOCK_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_LAST_QUEUE_ASSIGNED_TO_BLOCK_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_LAST_QUEUE_ASSIGNED_TO_BLOCK_REG_REGISTER_ID:
      regis = &(regs->qdp.last_queue_assigned_to_block_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_LAST_QUEUE_ASSIGNED_TO_BLOCK_REG_TOP_QUE_BLK_ID:
      field = &(regs->qdp.last_queue_assigned_to_block_reg[offset].top_que_blk);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "qdp.last_queue_assigned_to_block_reg***: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_last_queue_assigned_to_block_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_last_queue_assigned_to_block_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_LAST_QUEUE_ASSIGNED_TO_BLOCK_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_LAST_QUEUE_ASSIGNED_TO_BLOCK_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_LAST_QUEUE_ASSIGNED_TO_BLOCK_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_LAST_QUEUE_ASSIGNED_TO_BLOCK_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_LAST_QUEUE_ASSIGNED_TO_BLOCK_REG_REGISTER_ID:
      regis = &(regs->qdp.last_queue_assigned_to_block_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_LAST_QUEUE_ASSIGNED_TO_BLOCK_REG_TOP_QUE_BLK_ID:
      field = &(regs->qdp.last_queue_assigned_to_block_reg[offset].top_que_blk);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_qid_under_flow_block_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_qid_under_flow_block_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_QID_UNDER_FLOW_BLOCK_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_QID_UNDER_FLOW_BLOCK_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_QID_UNDER_FLOW_BLOCK_REG_REGISTER_ID:
      regis = &(regs->qdp.qid_under_flow_block_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_QID_UNDER_FLOW_BLOCK_REG_UDF_BUFF_QIDBLK_ID:
      field = &(regs->qdp.qid_under_flow_block_reg.udf_buff_qidblk);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "qdp.qid_under_flow_block_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_qid_under_flow_block_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_qid_under_flow_block_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_QID_UNDER_FLOW_BLOCK_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_QID_UNDER_FLOW_BLOCK_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_QID_UNDER_FLOW_BLOCK_REG_REGISTER_ID:
      regis = &(regs->qdp.qid_under_flow_block_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_QID_UNDER_FLOW_BLOCK_REG_UDF_BUFF_QIDBLK_ID:
      field = &(regs->qdp.qid_under_flow_block_reg.udf_buff_qidblk);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_delete_total_words_counter_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_delete_total_words_counter_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_DELETE_TOTAL_WORDS_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_DELETE_TOTAL_WORDS_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_DELETE_TOTAL_WORDS_COUNTER_REG_REGISTER_ID:
      regis = &(regs->qdp.delete_total_words_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_DELETE_TOTAL_WORDS_COUNTER_REG_TAIL_DEL_WORD_CNT_ID:
      field = &(regs->qdp.delete_total_words_counter_reg.tail_del_word_cnt);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "qdp.delete_total_words_counter_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_delete_total_words_counter_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_delete_total_words_counter_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_DELETE_TOTAL_WORDS_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_DELETE_TOTAL_WORDS_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_DELETE_TOTAL_WORDS_COUNTER_REG_REGISTER_ID:
      regis = &(regs->qdp.delete_total_words_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_DELETE_TOTAL_WORDS_COUNTER_REG_TAIL_DEL_WORD_CNT_ID:
      field = &(regs->qdp.delete_total_words_counter_reg.tail_del_word_cnt);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_link_sram_configuration_mode_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_link_sram_configuration_mode_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_LINK_SRAM_CONFIGURATION_MODE_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_LINK_SRAM_CONFIGURATION_MODE_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_LINK_SRAM_CONFIGURATION_MODE_REG_REGISTER_ID:
      regis = &(regs->qdp.link_sram_configuration_mode_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_LINK_SRAM_CONFIGURATION_MODE_REG_NUM_BUFF_MEM_ID:
      field = &(regs->qdp.link_sram_configuration_mode_reg.num_buff_mem);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "qdp.link_sram_configuration_mode_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_link_sram_configuration_mode_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_link_sram_configuration_mode_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_LINK_SRAM_CONFIGURATION_MODE_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_LINK_SRAM_CONFIGURATION_MODE_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_LINK_SRAM_CONFIGURATION_MODE_REG_REGISTER_ID:
      regis = &(regs->qdp.link_sram_configuration_mode_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_LINK_SRAM_CONFIGURATION_MODE_REG_NUM_BUFF_MEM_ID:
      field = &(regs->qdp.link_sram_configuration_mode_reg.num_buff_mem);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_programmable_enqueue_discarded_total_packets_counter_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_programmable_enqueue_discarded_total_packets_counter_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_ENQUEUE_DISCARDED_TOTAL_PACKETS_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_ENQUEUE_DISCARDED_TOTAL_PACKETS_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_ENQUEUE_DISCARDED_TOTAL_PACKETS_COUNTER_REG_REGISTER_ID:
      regis = &(regs->qdp.programmable_enqueue_discarded_total_packets_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_ENQUEUE_DISCARDED_TOTAL_PACKETS_COUNTER_REG_PRG_HD_DEL_PKT_CNT_ID:
      field = &(regs->qdp.programmable_enqueue_discarded_total_packets_counter_reg.prg_hd_del_pkt_cnt);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "qdp.programmable_enqueue_discarded_total_packets_counter_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_programmable_enqueue_discarded_total_packets_counter_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_programmable_enqueue_discarded_total_packets_counter_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_ENQUEUE_DISCARDED_TOTAL_PACKETS_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_ENQUEUE_DISCARDED_TOTAL_PACKETS_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_ENQUEUE_DISCARDED_TOTAL_PACKETS_COUNTER_REG_REGISTER_ID:
      regis = &(regs->qdp.programmable_enqueue_discarded_total_packets_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_ENQUEUE_DISCARDED_TOTAL_PACKETS_COUNTER_REG_PRG_HD_DEL_PKT_CNT_ID:
      field = &(regs->qdp.programmable_enqueue_discarded_total_packets_counter_reg.prg_hd_del_pkt_cnt);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_programmable_enqueue_discarded_total_words_counter_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_programmable_enqueue_discarded_total_words_counter_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_ENQUEUE_DISCARDED_TOTAL_WORDS_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_ENQUEUE_DISCARDED_TOTAL_WORDS_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_ENQUEUE_DISCARDED_TOTAL_WORDS_COUNTER_REG_REGISTER_ID:
      regis = &(regs->qdp.programmable_enqueue_discarded_total_words_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_ENQUEUE_DISCARDED_TOTAL_WORDS_COUNTER_REG_PRG_HD_DEL_WRD_CNT_ID:
      field = &(regs->qdp.programmable_enqueue_discarded_total_words_counter_reg.prg_hd_del_wrd_cnt);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "qdp.programmable_enqueue_discarded_total_words_counter_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_programmable_enqueue_discarded_total_words_counter_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_programmable_enqueue_discarded_total_words_counter_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_ENQUEUE_DISCARDED_TOTAL_WORDS_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_ENQUEUE_DISCARDED_TOTAL_WORDS_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_ENQUEUE_DISCARDED_TOTAL_WORDS_COUNTER_REG_REGISTER_ID:
      regis = &(regs->qdp.programmable_enqueue_discarded_total_words_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_ENQUEUE_DISCARDED_TOTAL_WORDS_COUNTER_REG_PRG_HD_DEL_WRD_CNT_ID:
      field = &(regs->qdp.programmable_enqueue_discarded_total_words_counter_reg.prg_hd_del_wrd_cnt);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_last_lbp_descriptor_0_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_last_lbp_descriptor_0_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_LAST_LBP_DESCRIPTOR_0_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_LAST_LBP_DESCRIPTOR_0_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_LAST_LBP_DESCRIPTOR_0_REG_REGISTER_ID:
      regis = &(regs->qdp.last_lbp_descriptor_0_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_LAST_LBP_DESCRIPTOR_0_REG_LBPDESC0_ID:
      field = &(regs->qdp.last_lbp_descriptor_0_reg.lbpdesc0);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "qdp.last_lbp_descriptor_0_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_last_lbp_descriptor_0_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_last_lbp_descriptor_0_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_LAST_LBP_DESCRIPTOR_0_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_LAST_LBP_DESCRIPTOR_0_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_LAST_LBP_DESCRIPTOR_0_REG_REGISTER_ID:
      regis = &(regs->qdp.last_lbp_descriptor_0_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_LAST_LBP_DESCRIPTOR_0_REG_LBPDESC0_ID:
      field = &(regs->qdp.last_lbp_descriptor_0_reg.lbpdesc0);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_programmable_dequeue_discarded_total_words_counter_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_programmable_dequeue_discarded_total_words_counter_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_DEQUEUE_DISCARDED_TOTAL_WORDS_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_DEQUEUE_DISCARDED_TOTAL_WORDS_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_DEQUEUE_DISCARDED_TOTAL_WORDS_COUNTER_REG_REGISTER_ID:
      regis = &(regs->qdp.programmable_dequeue_discarded_total_words_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_DEQUEUE_DISCARDED_TOTAL_WORDS_COUNTER_REG_PRG_TAIL_DEL_WRD_CNT_ID:
      field = &(regs->qdp.programmable_dequeue_discarded_total_words_counter_reg.prg_tail_del_wrd_cnt);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "qdp.programmable_dequeue_discarded_total_words_counter_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_programmable_dequeue_discarded_total_words_counter_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_programmable_dequeue_discarded_total_words_counter_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_DEQUEUE_DISCARDED_TOTAL_WORDS_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_DEQUEUE_DISCARDED_TOTAL_WORDS_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_DEQUEUE_DISCARDED_TOTAL_WORDS_COUNTER_REG_REGISTER_ID:
      regis = &(regs->qdp.programmable_dequeue_discarded_total_words_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_DEQUEUE_DISCARDED_TOTAL_WORDS_COUNTER_REG_PRG_TAIL_DEL_WRD_CNT_ID:
      field = &(regs->qdp.programmable_dequeue_discarded_total_words_counter_reg.prg_tail_del_wrd_cnt);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_total_buffers_consumed_block_3_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_total_buffers_consumed_block_3_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_TOTAL_BUFFERS_CONSUMED_BLOCK_3_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_TOTAL_BUFFERS_CONSUMED_BLOCK_3_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_TOTAL_BUFFERS_CONSUMED_BLOCK_3_REG_REGISTER_ID:
      regis = &(regs->qdp.total_buffers_consumed_block_3_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_TOTAL_BUFFERS_CONSUMED_BLOCK_3_REG_TOT_BUFFERS3_ID:
      field = &(regs->qdp.total_buffers_consumed_block_3_reg.tot_buffers3);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "qdp.total_buffers_consumed_block_3_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_total_buffers_consumed_block_3_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_total_buffers_consumed_block_3_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_TOTAL_BUFFERS_CONSUMED_BLOCK_3_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_TOTAL_BUFFERS_CONSUMED_BLOCK_3_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_TOTAL_BUFFERS_CONSUMED_BLOCK_3_REG_REGISTER_ID:
      regis = &(regs->qdp.total_buffers_consumed_block_3_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_TOTAL_BUFFERS_CONSUMED_BLOCK_3_REG_TOT_BUFFERS3_ID:
      field = &(regs->qdp.total_buffers_consumed_block_3_reg.tot_buffers3);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_indirect_command_address_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_indirect_command_address_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_INDIRECT_COMMAND_ADDRESS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_INDIRECT_COMMAND_ADDRESS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_INDIRECT_COMMAND_ADDRESS_REG_REGISTER_ID:
      regis = &(regs->qdp.indirect_command_address_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INDIRECT_COMMAND_ADDRESS_REG_INDIRECT_COMMAND_ADDRESS_ID:
      field = &(regs->qdp.indirect_command_address_reg.indirect_command_address);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "qdp.indirect_command_address_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_indirect_command_address_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_indirect_command_address_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_INDIRECT_COMMAND_ADDRESS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_INDIRECT_COMMAND_ADDRESS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_INDIRECT_COMMAND_ADDRESS_REG_REGISTER_ID:
      regis = &(regs->qdp.indirect_command_address_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INDIRECT_COMMAND_ADDRESS_REG_INDIRECT_COMMAND_ADDRESS_ID:
      field = &(regs->qdp.indirect_command_address_reg.indirect_command_address);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_total_buffers_consumed_block_2_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_total_buffers_consumed_block_2_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_TOTAL_BUFFERS_CONSUMED_BLOCK_2_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_TOTAL_BUFFERS_CONSUMED_BLOCK_2_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_TOTAL_BUFFERS_CONSUMED_BLOCK_2_REG_REGISTER_ID:
      regis = &(regs->qdp.total_buffers_consumed_block_2_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_TOTAL_BUFFERS_CONSUMED_BLOCK_2_REG_TOT_BUFFERS2_ID:
      field = &(regs->qdp.total_buffers_consumed_block_2_reg.tot_buffers2);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "qdp.total_buffers_consumed_block_2_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_total_buffers_consumed_block_2_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_total_buffers_consumed_block_2_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_TOTAL_BUFFERS_CONSUMED_BLOCK_2_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_TOTAL_BUFFERS_CONSUMED_BLOCK_2_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_TOTAL_BUFFERS_CONSUMED_BLOCK_2_REG_REGISTER_ID:
      regis = &(regs->qdp.total_buffers_consumed_block_2_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_TOTAL_BUFFERS_CONSUMED_BLOCK_2_REG_TOT_BUFFERS2_ID:
      field = &(regs->qdp.total_buffers_consumed_block_2_reg.tot_buffers2);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_indirect_command_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_indirect_command_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_INDIRECT_COMMAND_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_INDIRECT_COMMAND_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_INDIRECT_COMMAND_REG_REGISTER_ID:
      regis = &(regs->qdp.indirect_command_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INDIRECT_COMMAND_REG_INDIRECT_COMMAND_ID:
      field = &(regs->qdp.indirect_command_reg.indirect_command);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "qdp.indirect_command_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_indirect_command_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_indirect_command_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_INDIRECT_COMMAND_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_INDIRECT_COMMAND_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_INDIRECT_COMMAND_REG_REGISTER_ID:
      regis = &(regs->qdp.indirect_command_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_INDIRECT_COMMAND_REG_INDIRECT_COMMAND_ID:
      field = &(regs->qdp.indirect_command_reg.indirect_command);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_non_valid_header_packet_counter_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_non_valid_header_packet_counter_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_NON_VALID_HEADER_PACKET_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_NON_VALID_HEADER_PACKET_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_NON_VALID_HEADER_PACKET_COUNTER_REG_REGISTER_ID:
      regis = &(regs->qdp.non_valid_header_packet_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_NON_VALID_HEADER_PACKET_COUNTER_REG_NV_PKT_CNT_ID:
      field = &(regs->qdp.non_valid_header_packet_counter_reg.nv_pkt_cnt);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "qdp.non_valid_header_packet_counter_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_non_valid_header_packet_counter_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_non_valid_header_packet_counter_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_NON_VALID_HEADER_PACKET_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_NON_VALID_HEADER_PACKET_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_NON_VALID_HEADER_PACKET_COUNTER_REG_REGISTER_ID:
      regis = &(regs->qdp.non_valid_header_packet_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_NON_VALID_HEADER_PACKET_COUNTER_REG_NV_PKT_CNT_ID:
      field = &(regs->qdp.non_valid_header_packet_counter_reg.nv_pkt_cnt);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_qdp_commands_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_qdp_commands_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_QDP_COMMANDS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_QDP_COMMANDS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_QDP_COMMANDS_REG_REGISTER_ID:
      regis = &(regs->qdp.qdp_commands_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_QDP_COMMANDS_REG_DISCARD_ALL_PKT_ID:
      field = &(regs->qdp.qdp_commands_reg.discard_all_pkt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_QDP_COMMANDS_REG_DIS_LBPRD_ID:
      field = &(regs->qdp.qdp_commands_reg.dis_lbprd);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_QDP_COMMANDS_REG_ENA_AVRG_UPD_ID:
      field = &(regs->qdp.qdp_commands_reg.ena_avrg_upd);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "qdp.qdp_commands_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_qdp_commands_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_qdp_commands_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_QDP_COMMANDS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_QDP_COMMANDS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_QDP_COMMANDS_REG_REGISTER_ID:
      regis = &(regs->qdp.qdp_commands_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_QDP_COMMANDS_REG_DISCARD_ALL_PKT_ID:
      field = &(regs->qdp.qdp_commands_reg.discard_all_pkt);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_QDP_COMMANDS_REG_DIS_LBPRD_ID:
      field = &(regs->qdp.qdp_commands_reg.dis_lbprd);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_QDP_COMMANDS_REG_ENA_AVRG_UPD_ID:
      field = &(regs->qdp.qdp_commands_reg.ena_avrg_upd);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_programmable_enqueue_queued_total_packets_counter_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_programmable_enqueue_queued_total_packets_counter_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_ENQUEUE_QUEUED_TOTAL_PACKETS_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_ENQUEUE_QUEUED_TOTAL_PACKETS_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_ENQUEUE_QUEUED_TOTAL_PACKETS_COUNTER_REG_REGISTER_ID:
      regis = &(regs->qdp.programmable_enqueue_queued_total_packets_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_ENQUEUE_QUEUED_TOTAL_PACKETS_COUNTER_REG_PRG_EN_QPKT_CNT_ID:
      field = &(regs->qdp.programmable_enqueue_queued_total_packets_counter_reg.prg_en_qpkt_cnt);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "qdp.programmable_enqueue_queued_total_packets_counter_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_programmable_enqueue_queued_total_packets_counter_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_programmable_enqueue_queued_total_packets_counter_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_ENQUEUE_QUEUED_TOTAL_PACKETS_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_ENQUEUE_QUEUED_TOTAL_PACKETS_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_ENQUEUE_QUEUED_TOTAL_PACKETS_COUNTER_REG_REGISTER_ID:
      regis = &(regs->qdp.programmable_enqueue_queued_total_packets_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_ENQUEUE_QUEUED_TOTAL_PACKETS_COUNTER_REG_PRG_EN_QPKT_CNT_ID:
      field = &(regs->qdp.programmable_enqueue_queued_total_packets_counter_reg.prg_en_qpkt_cnt);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_total_buffers_consumed_block_1_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_total_buffers_consumed_block_1_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_TOTAL_BUFFERS_CONSUMED_BLOCK_1_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_TOTAL_BUFFERS_CONSUMED_BLOCK_1_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_TOTAL_BUFFERS_CONSUMED_BLOCK_1_REG_REGISTER_ID:
      regis = &(regs->qdp.total_buffers_consumed_block_1_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_TOTAL_BUFFERS_CONSUMED_BLOCK_1_REG_TOT_BUFFERS1_ID:
      field = &(regs->qdp.total_buffers_consumed_block_1_reg.tot_buffers1);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "qdp.total_buffers_consumed_block_1_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_total_buffers_consumed_block_1_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_total_buffers_consumed_block_1_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_TOTAL_BUFFERS_CONSUMED_BLOCK_1_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_TOTAL_BUFFERS_CONSUMED_BLOCK_1_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_TOTAL_BUFFERS_CONSUMED_BLOCK_1_REG_REGISTER_ID:
      regis = &(regs->qdp.total_buffers_consumed_block_1_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_TOTAL_BUFFERS_CONSUMED_BLOCK_1_REG_TOT_BUFFERS1_ID:
      field = &(regs->qdp.total_buffers_consumed_block_1_reg.tot_buffers1);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_last_buffer_assigned_to_block_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_last_buffer_assigned_to_block_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_LAST_BUFFER_ASSIGNED_TO_BLOCK_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_LAST_BUFFER_ASSIGNED_TO_BLOCK_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_LAST_BUFFER_ASSIGNED_TO_BLOCK_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_LAST_BUFFER_ASSIGNED_TO_BLOCK_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_LAST_BUFFER_ASSIGNED_TO_BLOCK_REG_REGISTER_ID:
      regis = &(regs->qdp.last_buffer_assigned_to_block_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_LAST_BUFFER_ASSIGNED_TO_BLOCK_REG_TOP_BUFF_BLK_ID:
      field = &(regs->qdp.last_buffer_assigned_to_block_reg[offset].top_buff_blk);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "qdp.last_buffer_assigned_to_block_reg***: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_last_buffer_assigned_to_block_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_last_buffer_assigned_to_block_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_LAST_BUFFER_ASSIGNED_TO_BLOCK_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_LAST_BUFFER_ASSIGNED_TO_BLOCK_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_LAST_BUFFER_ASSIGNED_TO_BLOCK_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_LAST_BUFFER_ASSIGNED_TO_BLOCK_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_LAST_BUFFER_ASSIGNED_TO_BLOCK_REG_REGISTER_ID:
      regis = &(regs->qdp.last_buffer_assigned_to_block_reg[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_LAST_BUFFER_ASSIGNED_TO_BLOCK_REG_TOP_BUFF_BLK_ID:
      field = &(regs->qdp.last_buffer_assigned_to_block_reg[offset].top_buff_blk);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_queue_size_watermark_configuration_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_queue_size_watermark_configuration_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_QUEUE_SIZE_WATERMARK_CONFIGURATION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_QUEUE_SIZE_WATERMARK_CONFIGURATION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_QUEUE_SIZE_WATERMARK_CONFIGURATION_REG_REGISTER_ID:
      regis = &(regs->qdp.queue_size_watermark_configuration_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_QUEUE_SIZE_WATERMARK_CONFIGURATION_REG_PRG_PEAK_QUE_SIZE_ID:
      field = &(regs->qdp.queue_size_watermark_configuration_reg.prg_peak_que_size);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "qdp.queue_size_watermark_configuration_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_queue_size_watermark_configuration_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_queue_size_watermark_configuration_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_QUEUE_SIZE_WATERMARK_CONFIGURATION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_QUEUE_SIZE_WATERMARK_CONFIGURATION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_QUEUE_SIZE_WATERMARK_CONFIGURATION_REG_REGISTER_ID:
      regis = &(regs->qdp.queue_size_watermark_configuration_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_QUEUE_SIZE_WATERMARK_CONFIGURATION_REG_PRG_PEAK_QUE_SIZE_ID:
      field = &(regs->qdp.queue_size_watermark_configuration_reg.prg_peak_que_size);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_enqueue_queued_total_words_counter_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_enqueue_queued_total_words_counter_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_ENQUEUE_QUEUED_TOTAL_WORDS_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_ENQUEUE_QUEUED_TOTAL_WORDS_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_ENQUEUE_QUEUED_TOTAL_WORDS_COUNTER_REG_REGISTER_ID:
      regis = &(regs->qdp.enqueue_queued_total_words_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_ENQUEUE_QUEUED_TOTAL_WORDS_COUNTER_REG_EN_QWORD_CNT_ID:
      field = &(regs->qdp.enqueue_queued_total_words_counter_reg.en_qword_cnt);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "qdp.enqueue_queued_total_words_counter_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_enqueue_queued_total_words_counter_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_enqueue_queued_total_words_counter_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_ENQUEUE_QUEUED_TOTAL_WORDS_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_ENQUEUE_QUEUED_TOTAL_WORDS_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_ENQUEUE_QUEUED_TOTAL_WORDS_COUNTER_REG_REGISTER_ID:
      regis = &(regs->qdp.enqueue_queued_total_words_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_ENQUEUE_QUEUED_TOTAL_WORDS_COUNTER_REG_EN_QWORD_CNT_ID:
      field = &(regs->qdp.enqueue_queued_total_words_counter_reg.en_qword_cnt);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_programmable_dequeue_total_words_counter_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_programmable_dequeue_total_words_counter_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_DEQUEUE_TOTAL_WORDS_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_DEQUEUE_TOTAL_WORDS_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_DEQUEUE_TOTAL_WORDS_COUNTER_REG_REGISTER_ID:
      regis = &(regs->qdp.programmable_dequeue_total_words_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_DEQUEUE_TOTAL_WORDS_COUNTER_REG_PRG_DE_QWRD_CNT_ID:
      field = &(regs->qdp.programmable_dequeue_total_words_counter_reg.prg_de_qwrd_cnt);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "qdp.programmable_dequeue_total_words_counter_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_programmable_dequeue_total_words_counter_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_programmable_dequeue_total_words_counter_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_DEQUEUE_TOTAL_WORDS_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_DEQUEUE_TOTAL_WORDS_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_DEQUEUE_TOTAL_WORDS_COUNTER_REG_REGISTER_ID:
      regis = &(regs->qdp.programmable_dequeue_total_words_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_DEQUEUE_TOTAL_WORDS_COUNTER_REG_PRG_DE_QWRD_CNT_ID:
      field = &(regs->qdp.programmable_dequeue_total_words_counter_reg.prg_de_qwrd_cnt);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_dequeue_total_words_counter_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_dequeue_total_words_counter_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_DEQUEUE_TOTAL_WORDS_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_DEQUEUE_TOTAL_WORDS_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_DEQUEUE_TOTAL_WORDS_COUNTER_REG_REGISTER_ID:
      regis = &(regs->qdp.dequeue_total_words_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_DEQUEUE_TOTAL_WORDS_COUNTER_REG_DE_QWORD_CNT_ID:
      field = &(regs->qdp.dequeue_total_words_counter_reg.de_qword_cnt);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "qdp.dequeue_total_words_counter_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_dequeue_total_words_counter_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_dequeue_total_words_counter_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_DEQUEUE_TOTAL_WORDS_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_DEQUEUE_TOTAL_WORDS_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_DEQUEUE_TOTAL_WORDS_COUNTER_REG_REGISTER_ID:
      regis = &(regs->qdp.dequeue_total_words_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_DEQUEUE_TOTAL_WORDS_COUNTER_REG_DE_QWORD_CNT_ID:
      field = &(regs->qdp.dequeue_total_words_counter_reg.de_qword_cnt);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_delete_total_packets_counter_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_delete_total_packets_counter_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_DELETE_TOTAL_PACKETS_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_DELETE_TOTAL_PACKETS_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_DELETE_TOTAL_PACKETS_COUNTER_REG_REGISTER_ID:
      regis = &(regs->qdp.delete_total_packets_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_DELETE_TOTAL_PACKETS_COUNTER_REG_TAIL_DEL_PKT_CNT_ID:
      field = &(regs->qdp.delete_total_packets_counter_reg.tail_del_pkt_cnt);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "qdp.delete_total_packets_counter_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_delete_total_packets_counter_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_delete_total_packets_counter_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_DELETE_TOTAL_PACKETS_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_DELETE_TOTAL_PACKETS_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_DELETE_TOTAL_PACKETS_COUNTER_REG_REGISTER_ID:
      regis = &(regs->qdp.delete_total_packets_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_DELETE_TOTAL_PACKETS_COUNTER_REG_TAIL_DEL_PKT_CNT_ID:
      field = &(regs->qdp.delete_total_packets_counter_reg.tail_del_pkt_cnt);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_programmable_enqueue_queued_total_words_counter_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_programmable_enqueue_queued_total_words_counter_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_ENQUEUE_QUEUED_TOTAL_WORDS_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_ENQUEUE_QUEUED_TOTAL_WORDS_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_ENQUEUE_QUEUED_TOTAL_WORDS_COUNTER_REG_REGISTER_ID:
      regis = &(regs->qdp.programmable_enqueue_queued_total_words_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_ENQUEUE_QUEUED_TOTAL_WORDS_COUNTER_REG_PRG_EN_QWORD_CNT_ID:
      field = &(regs->qdp.programmable_enqueue_queued_total_words_counter_reg.prg_en_qword_cnt);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "qdp.programmable_enqueue_queued_total_words_counter_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_programmable_enqueue_queued_total_words_counter_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_programmable_enqueue_queued_total_words_counter_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_ENQUEUE_QUEUED_TOTAL_WORDS_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_ENQUEUE_QUEUED_TOTAL_WORDS_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_ENQUEUE_QUEUED_TOTAL_WORDS_COUNTER_REG_REGISTER_ID:
      regis = &(regs->qdp.programmable_enqueue_queued_total_words_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_ENQUEUE_QUEUED_TOTAL_WORDS_COUNTER_REG_PRG_EN_QWORD_CNT_ID:
      field = &(regs->qdp.programmable_enqueue_queued_total_words_counter_reg.prg_en_qword_cnt);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_last_lbp_descriptor_1_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_last_lbp_descriptor_1_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_LAST_LBP_DESCRIPTOR_1_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_LAST_LBP_DESCRIPTOR_1_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_LAST_LBP_DESCRIPTOR_1_REG_REGISTER_ID:
      regis = &(regs->qdp.last_lbp_descriptor_1_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_LAST_LBP_DESCRIPTOR_1_REG_LBPDESC1_ID:
      field = &(regs->qdp.last_lbp_descriptor_1_reg.lbpdesc1);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "qdp.last_lbp_descriptor_1_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_last_lbp_descriptor_1_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_last_lbp_descriptor_1_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_LAST_LBP_DESCRIPTOR_1_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_LAST_LBP_DESCRIPTOR_1_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_LAST_LBP_DESCRIPTOR_1_REG_REGISTER_ID:
      regis = &(regs->qdp.last_lbp_descriptor_1_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_LAST_LBP_DESCRIPTOR_1_REG_LBPDESC1_ID:
      field = &(regs->qdp.last_lbp_descriptor_1_reg.lbpdesc1);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_qid_global_pointers_distance_reg_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_qid_global_pointers_distance_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_QID_GLOBAL_POINTERS_DISTANCE_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_QID_GLOBAL_POINTERS_DISTANCE_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_QID_GLOBAL_POINTERS_DISTANCE_REG_REGISTER_ID:
      regis = &(regs->qdp.qid_global_pointers_distance_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_QID_GLOBAL_POINTERS_DISTANCE_REG_DEL_PTR_WR_PTR_DIST_ID:
      field = &(regs->qdp.qid_global_pointers_distance_reg.del_ptr_wr_ptr_dist);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "qdp.qid_global_pointers_distance_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_qid_global_pointers_distance_reg_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp");
  soc_sand_proc_name = "ui_fap21v_acc_qdp_qid_global_pointers_distance_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_QID_GLOBAL_POINTERS_DISTANCE_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_QDP_QID_GLOBAL_POINTERS_DISTANCE_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_QDP_QID_GLOBAL_POINTERS_DISTANCE_REG_REGISTER_ID:
      regis = &(regs->qdp.qid_global_pointers_distance_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_QDP_QID_GLOBAL_POINTERS_DISTANCE_REG_DEL_PTR_WR_PTR_DIST_ID:
      field = &(regs->qdp.qid_global_pointers_distance_reg.del_ptr_wr_ptr_dist);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_get(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp_get");

  if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_ZBT_PHYSICAL_INTERFACE_CONFIGURATION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_zbt_physical_interface_configuration_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_ENQUEUE_QUEUED_TOTAL_PACKETS_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_enqueue_queued_total_packets_counter_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_COUNTER_QUEUE_SELECTION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_programmable_counter_queue_selection_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_interrupts_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_ENQUEUE_DISCARDED_TOTAL_WORDS_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_enqueue_discarded_total_words_counter_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_QID_OVERFLOW_BLOCK_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_qid_overflow_block_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_INITIALIZATION_TRIGGER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_initialization_trigger_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_mask_interrupts_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_DEQUEUE_TOTAL_PACKETS_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_programmable_dequeue_total_packets_counter_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_GLOBAL_BUFFERS_CONSUMBTION_REJECT_ENABLE_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_global_buffers_consumbtion_reject_enable_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_INDIRECT_COMMAND_WR_DATA_REG_0_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_indirect_command_wr_data_reg_0_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_ENQUEUE_DISCARDED_TOTAL_PACKETS_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_enqueue_discarded_total_packets_counter_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_INDIRECT_COMMAND_RD_DATA_REG_0_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_indirect_command_rd_data_reg_0_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_DEQUEUE_TOTAL_PACKETS_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_dequeue_total_packets_counter_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_DEQUEUE_DISCARDED_TOTAL_PACKETS_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_programmable_dequeue_discarded_total_packets_counter_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_BUFFERS_CONSUMPTION_BLOCK_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_buffers_consumption_block_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_TOTAL_BUFFERS_CONSUMED_BLOCK_0_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_total_buffers_consumed_block_0_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_LAST_QUEUE_ASSIGNED_TO_BLOCK_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_last_queue_assigned_to_block_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_QID_UNDER_FLOW_BLOCK_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_qid_under_flow_block_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_DELETE_TOTAL_WORDS_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_delete_total_words_counter_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_LINK_SRAM_CONFIGURATION_MODE_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_link_sram_configuration_mode_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_ENQUEUE_DISCARDED_TOTAL_PACKETS_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_programmable_enqueue_discarded_total_packets_counter_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_ENQUEUE_DISCARDED_TOTAL_WORDS_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_programmable_enqueue_discarded_total_words_counter_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_LAST_LBP_DESCRIPTOR_0_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_last_lbp_descriptor_0_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_DEQUEUE_DISCARDED_TOTAL_WORDS_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_programmable_dequeue_discarded_total_words_counter_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_TOTAL_BUFFERS_CONSUMED_BLOCK_3_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_total_buffers_consumed_block_3_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_INDIRECT_COMMAND_ADDRESS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_indirect_command_address_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_TOTAL_BUFFERS_CONSUMED_BLOCK_2_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_total_buffers_consumed_block_2_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_INDIRECT_COMMAND_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_indirect_command_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_NON_VALID_HEADER_PACKET_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_non_valid_header_packet_counter_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_QDP_COMMANDS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_qdp_commands_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_ENQUEUE_QUEUED_TOTAL_PACKETS_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_programmable_enqueue_queued_total_packets_counter_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_TOTAL_BUFFERS_CONSUMED_BLOCK_1_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_total_buffers_consumed_block_1_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_LAST_BUFFER_ASSIGNED_TO_BLOCK_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_last_buffer_assigned_to_block_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_QUEUE_SIZE_WATERMARK_CONFIGURATION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_queue_size_watermark_configuration_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_ENQUEUE_QUEUED_TOTAL_WORDS_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_enqueue_queued_total_words_counter_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_DEQUEUE_TOTAL_WORDS_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_programmable_dequeue_total_words_counter_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_DEQUEUE_TOTAL_WORDS_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_dequeue_total_words_counter_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_DELETE_TOTAL_PACKETS_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_delete_total_packets_counter_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_ENQUEUE_QUEUED_TOTAL_WORDS_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_programmable_enqueue_queued_total_words_counter_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_LAST_LBP_DESCRIPTOR_1_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_last_lbp_descriptor_1_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_QID_GLOBAL_POINTERS_DISTANCE_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_qid_global_pointers_distance_reg_get(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after qdp***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_qdp_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_qdp_set");

  if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_ZBT_PHYSICAL_INTERFACE_CONFIGURATION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_zbt_physical_interface_configuration_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_ENQUEUE_QUEUED_TOTAL_PACKETS_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_enqueue_queued_total_packets_counter_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_COUNTER_QUEUE_SELECTION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_programmable_counter_queue_selection_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_INTERRUPTS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_interrupts_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_ENQUEUE_DISCARDED_TOTAL_WORDS_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_enqueue_discarded_total_words_counter_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_QID_OVERFLOW_BLOCK_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_qid_overflow_block_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_INITIALIZATION_TRIGGER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_initialization_trigger_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_MASK_INTERRUPTS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_mask_interrupts_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_DEQUEUE_TOTAL_PACKETS_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_programmable_dequeue_total_packets_counter_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_GLOBAL_BUFFERS_CONSUMBTION_REJECT_ENABLE_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_global_buffers_consumbtion_reject_enable_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_INDIRECT_COMMAND_WR_DATA_REG_0_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_indirect_command_wr_data_reg_0_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_ENQUEUE_DISCARDED_TOTAL_PACKETS_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_enqueue_discarded_total_packets_counter_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_INDIRECT_COMMAND_RD_DATA_REG_0_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_indirect_command_rd_data_reg_0_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_DEQUEUE_TOTAL_PACKETS_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_dequeue_total_packets_counter_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_DEQUEUE_DISCARDED_TOTAL_PACKETS_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_programmable_dequeue_discarded_total_packets_counter_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_BUFFERS_CONSUMPTION_BLOCK_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_buffers_consumption_block_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_TOTAL_BUFFERS_CONSUMED_BLOCK_0_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_total_buffers_consumed_block_0_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_LAST_QUEUE_ASSIGNED_TO_BLOCK_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_last_queue_assigned_to_block_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_QID_UNDER_FLOW_BLOCK_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_qid_under_flow_block_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_DELETE_TOTAL_WORDS_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_delete_total_words_counter_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_LINK_SRAM_CONFIGURATION_MODE_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_link_sram_configuration_mode_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_ENQUEUE_DISCARDED_TOTAL_PACKETS_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_programmable_enqueue_discarded_total_packets_counter_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_ENQUEUE_DISCARDED_TOTAL_WORDS_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_programmable_enqueue_discarded_total_words_counter_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_LAST_LBP_DESCRIPTOR_0_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_last_lbp_descriptor_0_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_DEQUEUE_DISCARDED_TOTAL_WORDS_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_programmable_dequeue_discarded_total_words_counter_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_TOTAL_BUFFERS_CONSUMED_BLOCK_3_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_total_buffers_consumed_block_3_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_INDIRECT_COMMAND_ADDRESS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_indirect_command_address_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_TOTAL_BUFFERS_CONSUMED_BLOCK_2_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_total_buffers_consumed_block_2_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_INDIRECT_COMMAND_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_indirect_command_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_NON_VALID_HEADER_PACKET_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_non_valid_header_packet_counter_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_QDP_COMMANDS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_qdp_commands_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_ENQUEUE_QUEUED_TOTAL_PACKETS_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_programmable_enqueue_queued_total_packets_counter_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_TOTAL_BUFFERS_CONSUMED_BLOCK_1_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_total_buffers_consumed_block_1_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_LAST_BUFFER_ASSIGNED_TO_BLOCK_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_last_buffer_assigned_to_block_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_QUEUE_SIZE_WATERMARK_CONFIGURATION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_queue_size_watermark_configuration_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_ENQUEUE_QUEUED_TOTAL_WORDS_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_enqueue_queued_total_words_counter_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_DEQUEUE_TOTAL_WORDS_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_programmable_dequeue_total_words_counter_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_DEQUEUE_TOTAL_WORDS_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_dequeue_total_words_counter_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_DELETE_TOTAL_PACKETS_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_delete_total_packets_counter_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_PROGRAMMABLE_ENQUEUE_QUEUED_TOTAL_WORDS_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_programmable_enqueue_queued_total_words_counter_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_LAST_LBP_DESCRIPTOR_1_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_last_lbp_descriptor_1_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_QDP_QID_GLOBAL_POINTERS_DISTANCE_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_qdp_qid_global_pointers_distance_reg_set(current_line, value);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after qdp***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}
