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
  ui_fap21v_acc_lbp_packet_header_filter_a_reg_4_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    buffer,
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_packet_header_filter_a_reg_4_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_4_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_4_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_4_REGISTER_ID:
      regis = &(regs->lbp.packet_header_filter_a_reg_4.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_4_FILTER_HDR_A_79_64_ID:
      field = &(regs->lbp.packet_header_filter_a_reg_4.filter_hdr_a_79_64);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "lbp.packet_header_filter_a_reg_4: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_packet_header_filter_a_reg_4_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  uint32
    ret;
  uint32
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_packet_header_filter_a_reg_4_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_4_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_4_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_4_REGISTER_ID:
      regis = &(regs->lbp.packet_header_filter_a_reg_4.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_4_FILTER_HDR_A_79_64_ID:
      field = &(regs->lbp.packet_header_filter_a_reg_4.filter_hdr_a_79_64);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_class2to3_mapping_reg_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    buffer,
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_class2to3_mapping_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_CLASS2TO3_MAPPING_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_CLASS2TO3_MAPPING_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_CLASS2TO3_MAPPING_REG_REGISTER_ID:
      regis = &(regs->lbp.class2to3_mapping_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_CLASS2TO3_MAPPING_REG_MAP_CLASS2TO3_0_ID:
      field = &(regs->lbp.class2to3_mapping_reg.map_class2to3_0);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_CLASS2TO3_MAPPING_REG_MAP_CLASS2TO3_1_ID:
      field = &(regs->lbp.class2to3_mapping_reg.map_class2to3_1);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_CLASS2TO3_MAPPING_REG_MAP_CLASS2TO3_2_ID:
      field = &(regs->lbp.class2to3_mapping_reg.map_class2to3_2);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_CLASS2TO3_MAPPING_REG_MAP_CLASS2TO3_3_ID:
      field = &(regs->lbp.class2to3_mapping_reg.map_class2to3_3);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "lbp.class2to3_mapping_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_class2to3_mapping_reg_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  uint32
    ret;
  uint32
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_class2to3_mapping_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_CLASS2TO3_MAPPING_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_CLASS2TO3_MAPPING_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_CLASS2TO3_MAPPING_REG_REGISTER_ID:
      regis = &(regs->lbp.class2to3_mapping_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_CLASS2TO3_MAPPING_REG_MAP_CLASS2TO3_0_ID:
      field = &(regs->lbp.class2to3_mapping_reg.map_class2to3_0);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_CLASS2TO3_MAPPING_REG_MAP_CLASS2TO3_1_ID:
      field = &(regs->lbp.class2to3_mapping_reg.map_class2to3_1);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_CLASS2TO3_MAPPING_REG_MAP_CLASS2TO3_2_ID:
      field = &(regs->lbp.class2to3_mapping_reg.map_class2to3_2);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_CLASS2TO3_MAPPING_REG_MAP_CLASS2TO3_3_ID:
      field = &(regs->lbp.class2to3_mapping_reg.map_class2to3_3);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_lag_configuration_reg_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    buffer,
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_lag_configuration_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_LAG_CONFIGURATION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_LAG_CONFIGURATION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_LAG_CONFIGURATION_REG_REGISTER_ID:
      regis = &(regs->lbp.lag_configuration_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_LAG_CONFIGURATION_REG_LAG_DEST_INDICATION_ID:
      field = &(regs->lbp.lag_configuration_reg.lag_dest_indication);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_LAG_CONFIGURATION_REG_LAG_HASH_POS_ID:
      field = &(regs->lbp.lag_configuration_reg.lag_hash_pos);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_LAG_CONFIGURATION_REG_LAG_HASH_POS_SHP_ID:
      field = &(regs->lbp.lag_configuration_reg.lag_hash_pos_shp);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "lbp.lag_configuration_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_lag_configuration_reg_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  uint32
    ret;
  uint32
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_lag_configuration_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_LAG_CONFIGURATION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_LAG_CONFIGURATION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_LAG_CONFIGURATION_REG_REGISTER_ID:
      regis = &(regs->lbp.lag_configuration_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_LAG_CONFIGURATION_REG_LAG_DEST_INDICATION_ID:
      field = &(regs->lbp.lag_configuration_reg.lag_dest_indication);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_LAG_CONFIGURATION_REG_LAG_HASH_POS_ID:
      field = &(regs->lbp.lag_configuration_reg.lag_hash_pos);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_LAG_CONFIGURATION_REG_LAG_HASH_POS_SHP_ID:
      field = &(regs->lbp.lag_configuration_reg.lag_hash_pos_shp);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_interrupts_reg_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    buffer,
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_interrupts_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_INTERRUPTS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_INTERRUPTS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_INTERRUPTS_REG_REGISTER_ID:
      regis = &(regs->lbp.interrupts_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_INTERRUPTS_REG_TREE_LVL_CNT_ERR_ID:
      field = &(regs->lbp.interrupts_reg.tree_lvl_cnt_err);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_INTERRUPTS_REG_QU_NUM_OVR_FLOW_ID:
      field = &(regs->lbp.interrupts_reg.qu_num_ovr_flow);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_INTERRUPTS_REG_UN_INIT_ROUT_ID:
      field = &(regs->lbp.interrupts_reg.un_init_rout);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_INTERRUPTS_REG_SMCAST_ERR_ID:
      field = &(regs->lbp.interrupts_reg.smcast_err);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_INTERRUPTS_REG_PKT_SIZE_ERR_INQ_ID:
      field = &(regs->lbp.interrupts_reg.pkt_size_err_inq);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_INTERRUPTS_REG_PKT_SIZE_ERR_EGQ_ID:
      field = &(regs->lbp.interrupts_reg.pkt_size_err_egq);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_INTERRUPTS_REG_PKT_PAR_ERR_ID:
      field = &(regs->lbp.interrupts_reg.pkt_par_err);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_INTERRUPTS_REG_PKT_CNT_O_ID:
      field = &(regs->lbp.interrupts_reg.pkt_cnt_o);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_INTERRUPTS_REG_ECC_ERR_ID:
      field = &(regs->lbp.interrupts_reg.ecc_err);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_INTERRUPTS_REG_UN_INIT_LAG_FWD_ID:
      field = &(regs->lbp.interrupts_reg.un_init_lag_fwd);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_INTERRUPTS_REG_UN_INIT_LAG_SCND_ID:
      field = &(regs->lbp.interrupts_reg.un_init_lag_scnd);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_INTERRUPTS_REG_SM_CAST_QUE_ERR1_ID:
      field = &(regs->lbp.interrupts_reg.sm_cast_que_err1);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_INTERRUPTS_REG_PKT_CNT_DSCRD_O_ID:
      field = &(regs->lbp.interrupts_reg.pkt_cnt_dscrd_o);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "lbp.interrupts_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_interrupts_reg_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  uint32
    ret;
  uint32
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_interrupts_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_INTERRUPTS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_INTERRUPTS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_INTERRUPTS_REG_REGISTER_ID:
      regis = &(regs->lbp.interrupts_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_INTERRUPTS_REG_TREE_LVL_CNT_ERR_ID:
      field = &(regs->lbp.interrupts_reg.tree_lvl_cnt_err);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_INTERRUPTS_REG_QU_NUM_OVR_FLOW_ID:
      field = &(regs->lbp.interrupts_reg.qu_num_ovr_flow);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_INTERRUPTS_REG_UN_INIT_ROUT_ID:
      field = &(regs->lbp.interrupts_reg.un_init_rout);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_INTERRUPTS_REG_SMCAST_ERR_ID:
      field = &(regs->lbp.interrupts_reg.smcast_err);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_INTERRUPTS_REG_PKT_SIZE_ERR_INQ_ID:
      field = &(regs->lbp.interrupts_reg.pkt_size_err_inq);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_INTERRUPTS_REG_PKT_SIZE_ERR_EGQ_ID:
      field = &(regs->lbp.interrupts_reg.pkt_size_err_egq);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_INTERRUPTS_REG_PKT_PAR_ERR_ID:
      field = &(regs->lbp.interrupts_reg.pkt_par_err);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_INTERRUPTS_REG_PKT_CNT_O_ID:
      field = &(regs->lbp.interrupts_reg.pkt_cnt_o);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_INTERRUPTS_REG_ECC_ERR_ID:
      field = &(regs->lbp.interrupts_reg.ecc_err);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_INTERRUPTS_REG_UN_INIT_LAG_FWD_ID:
      field = &(regs->lbp.interrupts_reg.un_init_lag_fwd);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_INTERRUPTS_REG_UN_INIT_LAG_SCND_ID:
      field = &(regs->lbp.interrupts_reg.un_init_lag_scnd);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_INTERRUPTS_REG_SM_CAST_QUE_ERR1_ID:
      field = &(regs->lbp.interrupts_reg.sm_cast_que_err1);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_INTERRUPTS_REG_PKT_CNT_DSCRD_O_ID:
      field = &(regs->lbp.interrupts_reg.pkt_cnt_dscrd_o);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_un_initialized_lag_fwd_address_reg_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    buffer,
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_un_initialized_lag_fwd_address_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_UN_INITIALIZED_LAG_FWD_ADDRESS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_UN_INITIALIZED_LAG_FWD_ADDRESS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_UN_INITIALIZED_LAG_FWD_ADDRESS_REG_REGISTER_ID:
      regis = &(regs->lbp.un_initialized_lag_fwd_address_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_UN_INITIALIZED_LAG_FWD_ADDRESS_REG_UN_INIT_LAG_FWD_ADDRESS_ID:
      field = &(regs->lbp.un_initialized_lag_fwd_address_reg.un_init_lag_fwd_address);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "lbp.un_initialized_lag_fwd_address_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_un_initialized_lag_fwd_address_reg_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  uint32
    ret;
  uint32
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_un_initialized_lag_fwd_address_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_UN_INITIALIZED_LAG_FWD_ADDRESS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_UN_INITIALIZED_LAG_FWD_ADDRESS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_UN_INITIALIZED_LAG_FWD_ADDRESS_REG_REGISTER_ID:
      regis = &(regs->lbp.un_initialized_lag_fwd_address_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_UN_INITIALIZED_LAG_FWD_ADDRESS_REG_UN_INIT_LAG_FWD_ADDRESS_ID:
      field = &(regs->lbp.un_initialized_lag_fwd_address_reg.un_init_lag_fwd_address);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_lbp_configuration_2_reg_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    buffer,
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_lbp_configuration_2_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_LBP_CONFIGURATION_2_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_LBP_CONFIGURATION_2_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_LBP_CONFIGURATION_2_REG_REGISTER_ID:
      regis = &(regs->lbp.lbp_configuration_2_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_LBP_CONFIGURATION_2_REG_FLOW_BASE_QNUM_ID:
      field = &(regs->lbp.lbp_configuration_2_reg.flow_base_qnum);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_LBP_CONFIGURATION_2_REG_FLOW_BASE_QNUM_ADD_ID:
      field = &(regs->lbp.lbp_configuration_2_reg.flow_base_qnum_add);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "lbp.lbp_configuration_2_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_lbp_configuration_2_reg_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  uint32
    ret;
  uint32
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_lbp_configuration_2_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_LBP_CONFIGURATION_2_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_LBP_CONFIGURATION_2_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_LBP_CONFIGURATION_2_REG_REGISTER_ID:
      regis = &(regs->lbp.lbp_configuration_2_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_LBP_CONFIGURATION_2_REG_FLOW_BASE_QNUM_ID:
      field = &(regs->lbp.lbp_configuration_2_reg.flow_base_qnum);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_LBP_CONFIGURATION_2_REG_FLOW_BASE_QNUM_ADD_ID:
      field = &(regs->lbp.lbp_configuration_2_reg.flow_base_qnum_add);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_mask_interrupts_reg_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    buffer,
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_mask_interrupts_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_MASK_INTERRUPTS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_MASK_INTERRUPTS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_MASK_INTERRUPTS_REG_REGISTER_ID:
      regis = &(regs->lbp.mask_interrupts_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_MASK_INTERRUPTS_REG_MTREE_LVL_CNT_ERR_ID:
      field = &(regs->lbp.mask_interrupts_reg.mtree_lvl_cnt_err);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_MASK_INTERRUPTS_REG_MQU_NUM_OVR_FLOW_ID:
      field = &(regs->lbp.mask_interrupts_reg.mqu_num_ovr_flow);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_MASK_INTERRUPTS_REG_MUN_INIT_ROUT_ID:
      field = &(regs->lbp.mask_interrupts_reg.mun_init_rout);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_MASK_INTERRUPTS_REG_MSMCAST_ERR_ID:
      field = &(regs->lbp.mask_interrupts_reg.msmcast_err);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_MASK_INTERRUPTS_REG_MPKT_SIZE_ERR_INQ_ID:
      field = &(regs->lbp.mask_interrupts_reg.mpkt_size_err_inq);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_MASK_INTERRUPTS_REG_MPKT_SIZE_ERR_EGQ_ID:
      field = &(regs->lbp.mask_interrupts_reg.mpkt_size_err_egq);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_MASK_INTERRUPTS_REG_MPKT_PAR_ERR_ID:
      field = &(regs->lbp.mask_interrupts_reg.mpkt_par_err);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_MASK_INTERRUPTS_REG_MPKT_CNT_O_ID:
      field = &(regs->lbp.mask_interrupts_reg.mpkt_cnt_o);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_MASK_INTERRUPTS_REG_MECC_ERR_ID:
      field = &(regs->lbp.mask_interrupts_reg.mecc_err);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_MASK_INTERRUPTS_REG_UN_INIT_LAG_FWD_ID:
      field = &(regs->lbp.mask_interrupts_reg.un_init_lag_fwd);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_MASK_INTERRUPTS_REG_UN_INIT_LAG_SCND_ID:
      field = &(regs->lbp.mask_interrupts_reg.un_init_lag_scnd);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_MASK_INTERRUPTS_REG_SM_CAST_QUE_ERR1_MASK_INT_ID:
      field = &(regs->lbp.mask_interrupts_reg.sm_cast_que_err1_mask_int);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_MASK_INTERRUPTS_REG_PKT_CNT_DSCRD_MASK_INT_ID:
      field = &(regs->lbp.mask_interrupts_reg.pkt_cnt_dscrd_mask_int);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "lbp.mask_interrupts_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_mask_interrupts_reg_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  uint32
    ret;
  uint32
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_mask_interrupts_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_MASK_INTERRUPTS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_MASK_INTERRUPTS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_MASK_INTERRUPTS_REG_REGISTER_ID:
      regis = &(regs->lbp.mask_interrupts_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_MASK_INTERRUPTS_REG_MTREE_LVL_CNT_ERR_ID:
      field = &(regs->lbp.mask_interrupts_reg.mtree_lvl_cnt_err);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_MASK_INTERRUPTS_REG_MQU_NUM_OVR_FLOW_ID:
      field = &(regs->lbp.mask_interrupts_reg.mqu_num_ovr_flow);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_MASK_INTERRUPTS_REG_MUN_INIT_ROUT_ID:
      field = &(regs->lbp.mask_interrupts_reg.mun_init_rout);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_MASK_INTERRUPTS_REG_MSMCAST_ERR_ID:
      field = &(regs->lbp.mask_interrupts_reg.msmcast_err);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_MASK_INTERRUPTS_REG_MPKT_SIZE_ERR_INQ_ID:
      field = &(regs->lbp.mask_interrupts_reg.mpkt_size_err_inq);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_MASK_INTERRUPTS_REG_MPKT_SIZE_ERR_EGQ_ID:
      field = &(regs->lbp.mask_interrupts_reg.mpkt_size_err_egq);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_MASK_INTERRUPTS_REG_MPKT_PAR_ERR_ID:
      field = &(regs->lbp.mask_interrupts_reg.mpkt_par_err);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_MASK_INTERRUPTS_REG_MPKT_CNT_O_ID:
      field = &(regs->lbp.mask_interrupts_reg.mpkt_cnt_o);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_MASK_INTERRUPTS_REG_MECC_ERR_ID:
      field = &(regs->lbp.mask_interrupts_reg.mecc_err);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_MASK_INTERRUPTS_REG_UN_INIT_LAG_FWD_ID:
      field = &(regs->lbp.mask_interrupts_reg.un_init_lag_fwd);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_MASK_INTERRUPTS_REG_UN_INIT_LAG_SCND_ID:
      field = &(regs->lbp.mask_interrupts_reg.un_init_lag_scnd);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_MASK_INTERRUPTS_REG_SM_CAST_QUE_ERR1_MASK_INT_ID:
      field = &(regs->lbp.mask_interrupts_reg.sm_cast_que_err1_mask_int);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_MASK_INTERRUPTS_REG_PKT_CNT_DSCRD_MASK_INT_ID:
      field = &(regs->lbp.mask_interrupts_reg.pkt_cnt_dscrd_mask_int);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_snoop_commands_get(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_snoop_commands_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_SNOOP_COMMANDS_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_SNOOP_COMMANDS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_SNOOP_COMMANDS_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_SNOOP_COMMANDS_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_SNOOP_COMMANDS_REGISTER_ID:
      regis = &(regs->lbp.snoop_commands[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_SNOOP_COMMANDS_ACT_MODE_ID:
      field = &(regs->lbp.snoop_commands[offset].act_mode);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_SNOOP_COMMANDS_DEST_ID_ID:
      field = &(regs->lbp.snoop_commands[offset].dest_id);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_SNOOP_COMMANDS_QUE_NUM_VAL_ID:
      field = &(regs->lbp.snoop_commands[offset].que_num_val);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_SNOOP_COMMANDS_QUE_NUM_ID:
      field = &(regs->lbp.snoop_commands[offset].que_num);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_SNOOP_COMMANDS_FULL_PACKET_ID:
      field = &(regs->lbp.snoop_commands[offset].full_packet);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_SNOOP_COMMANDS_PRIORITY_ID:
      field = &(regs->lbp.snoop_commands[offset].priority);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_SNOOP_COMMANDS_SIZE_ID:
      field = &(regs->lbp.snoop_commands[offset].size);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_SNOOP_COMMANDS_OV_CLASS_ENA_ID:
      field = &(regs->lbp.snoop_commands[offset].ov_class_ena);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_SNOOP_COMMANDS_OV_CLASS_ID:
      field = &(regs->lbp.snoop_commands[offset].ov_class);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_SNOOP_COMMANDS_OV_DP_ENA_ID:
      field = &(regs->lbp.snoop_commands[offset].ov_dp_ena);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_SNOOP_COMMANDS_OV_DP_ID:
      field = &(regs->lbp.snoop_commands[offset].ov_dp);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "lbp.snoop_commands***: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_snoop_commands_set(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_snoop_commands_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_SNOOP_COMMANDS_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_SNOOP_COMMANDS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_SNOOP_COMMANDS_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_SNOOP_COMMANDS_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_SNOOP_COMMANDS_REGISTER_ID:
      regis = &(regs->lbp.snoop_commands[offset].addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_SNOOP_COMMANDS_ACT_MODE_ID:
      field = &(regs->lbp.snoop_commands[offset].act_mode);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_SNOOP_COMMANDS_DEST_ID_ID:
      field = &(regs->lbp.snoop_commands[offset].dest_id);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_SNOOP_COMMANDS_QUE_NUM_VAL_ID:
      field = &(regs->lbp.snoop_commands[offset].que_num_val);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_SNOOP_COMMANDS_QUE_NUM_ID:
      field = &(regs->lbp.snoop_commands[offset].que_num);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_SNOOP_COMMANDS_FULL_PACKET_ID:
      field = &(regs->lbp.snoop_commands[offset].full_packet);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_SNOOP_COMMANDS_PRIORITY_ID:
      field = &(regs->lbp.snoop_commands[offset].priority);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_SNOOP_COMMANDS_SIZE_ID:
      field = &(regs->lbp.snoop_commands[offset].size);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_SNOOP_COMMANDS_OV_CLASS_ENA_ID:
      field = &(regs->lbp.snoop_commands[offset].ov_class_ena);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_SNOOP_COMMANDS_OV_CLASS_ID:
      field = &(regs->lbp.snoop_commands[offset].ov_class);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_SNOOP_COMMANDS_OV_DP_ENA_ID:
      field = &(regs->lbp.snoop_commands[offset].ov_dp_ena);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_SNOOP_COMMANDS_OV_DP_ID:
      field = &(regs->lbp.snoop_commands[offset].ov_dp);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_un_initialized_lag_scnd_address_reg_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    buffer,
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_un_initialized_lag_scnd_address_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_UN_INITIALIZED_LAG_SCND_ADDRESS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_UN_INITIALIZED_LAG_SCND_ADDRESS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_UN_INITIALIZED_LAG_SCND_ADDRESS_REG_REGISTER_ID:
      regis = &(regs->lbp.un_initialized_lag_scnd_address_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_UN_INITIALIZED_LAG_SCND_ADDRESS_REG_UN_INIT_LAG_SCND_ADDRESS_ID:
      field = &(regs->lbp.un_initialized_lag_scnd_address_reg.un_init_lag_scnd_address);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "lbp.un_initialized_lag_scnd_address_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_un_initialized_lag_scnd_address_reg_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  uint32
    ret;
  uint32
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_un_initialized_lag_scnd_address_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_UN_INITIALIZED_LAG_SCND_ADDRESS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_UN_INITIALIZED_LAG_SCND_ADDRESS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_UN_INITIALIZED_LAG_SCND_ADDRESS_REG_REGISTER_ID:
      regis = &(regs->lbp.un_initialized_lag_scnd_address_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_UN_INITIALIZED_LAG_SCND_ADDRESS_REG_UN_INIT_LAG_SCND_ADDRESS_ID:
      field = &(regs->lbp.un_initialized_lag_scnd_address_reg.un_init_lag_scnd_address);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_packet_header_filter_b_reg_0_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    buffer,
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_packet_header_filter_b_reg_0_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_0_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_0_REGISTER_ID:
      regis = &(regs->lbp.packet_header_filter_b_reg_0.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_0_FILTER_HDR_B_31_0_ID:
      field = &(regs->lbp.packet_header_filter_b_reg_0.filter_hdr_b_31_0);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "lbp.packet_header_filter_b_reg_0: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_packet_header_filter_b_reg_0_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  uint32
    ret;
  uint32
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_packet_header_filter_b_reg_0_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_0_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_0_REGISTER_ID:
      regis = &(regs->lbp.packet_header_filter_b_reg_0.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_0_FILTER_HDR_B_31_0_ID:
      field = &(regs->lbp.packet_header_filter_b_reg_0.filter_hdr_b_31_0);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_packet_header_filter_a_reg_3_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    buffer,
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_packet_header_filter_a_reg_3_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_3_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_3_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_3_REGISTER_ID:
      regis = &(regs->lbp.packet_header_filter_a_reg_3.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_3_FILTER_MASK_A_63_32_ID:
      field = &(regs->lbp.packet_header_filter_a_reg_3.filter_mask_a_63_32);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "lbp.packet_header_filter_a_reg_3: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_packet_header_filter_a_reg_3_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  uint32
    ret;
  uint32
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_packet_header_filter_a_reg_3_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_3_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_3_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_3_REGISTER_ID:
      regis = &(regs->lbp.packet_header_filter_a_reg_3.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_3_FILTER_MASK_A_63_32_ID:
      field = &(regs->lbp.packet_header_filter_a_reg_3.filter_mask_a_63_32);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_indirect_command_wr_data_reg_0_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    buffer,
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_indirect_command_wr_data_reg_0_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_INDIRECT_COMMAND_WR_DATA_REG_0_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_INDIRECT_COMMAND_WR_DATA_REG_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_INDIRECT_COMMAND_WR_DATA_REG_0_REGISTER_ID:
      regis = &(regs->lbp.indirect_command_wr_data_reg_0.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_INDIRECT_COMMAND_WR_DATA_REG_0_INDIRECT_COMMAND_WR_DATA_ID:
      field = &(regs->lbp.indirect_command_wr_data_reg_0.indirect_command_wr_data);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "lbp.indirect_command_wr_data_reg_0: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_indirect_command_wr_data_reg_0_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  uint32
    ret;
  uint32
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_indirect_command_wr_data_reg_0_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_INDIRECT_COMMAND_WR_DATA_REG_0_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_INDIRECT_COMMAND_WR_DATA_REG_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_INDIRECT_COMMAND_WR_DATA_REG_0_REGISTER_ID:
      regis = &(regs->lbp.indirect_command_wr_data_reg_0.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_INDIRECT_COMMAND_WR_DATA_REG_0_INDIRECT_COMMAND_WR_DATA_ID:
      field = &(regs->lbp.indirect_command_wr_data_reg_0.indirect_command_wr_data);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_packet_header_filter_a_reg_2_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    buffer,
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_packet_header_filter_a_reg_2_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_2_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_2_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_2_REGISTER_ID:
      regis = &(regs->lbp.packet_header_filter_a_reg_2.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_2_FILTER_HDR_A_63_32_ID:
      field = &(regs->lbp.packet_header_filter_a_reg_2.filter_hdr_a_63_32);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "lbp.packet_header_filter_a_reg_2: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_packet_header_filter_a_reg_2_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  uint32
    ret;
  uint32
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_packet_header_filter_a_reg_2_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_2_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_2_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_2_REGISTER_ID:
      regis = &(regs->lbp.packet_header_filter_a_reg_2.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_2_FILTER_HDR_A_63_32_ID:
      field = &(regs->lbp.packet_header_filter_a_reg_2.filter_hdr_a_63_32);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_fabric_multicast_queues_range_reg_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    buffer,
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_fabric_multicast_queues_range_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_FABRIC_MULTICAST_QUEUES_RANGE_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_FABRIC_MULTICAST_QUEUES_RANGE_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_FABRIC_MULTICAST_QUEUES_RANGE_REG_REGISTER_ID:
      regis = &(regs->lbp.fabric_multicast_queues_range_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_FABRIC_MULTICAST_QUEUES_RANGE_REG_FMC_QNUM_LOW_ID:
      field = &(regs->lbp.fabric_multicast_queues_range_reg.fmc_qnum_low);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_FABRIC_MULTICAST_QUEUES_RANGE_REG_FMC_QNUM_HIGH_ID:
      field = &(regs->lbp.fabric_multicast_queues_range_reg.fmc_qnum_high);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "lbp.fabric_multicast_queues_range_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_fabric_multicast_queues_range_reg_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  uint32
    ret;
  uint32
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_fabric_multicast_queues_range_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_FABRIC_MULTICAST_QUEUES_RANGE_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_FABRIC_MULTICAST_QUEUES_RANGE_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_FABRIC_MULTICAST_QUEUES_RANGE_REG_REGISTER_ID:
      regis = &(regs->lbp.fabric_multicast_queues_range_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_FABRIC_MULTICAST_QUEUES_RANGE_REG_FMC_QNUM_LOW_ID:
      field = &(regs->lbp.fabric_multicast_queues_range_reg.fmc_qnum_low);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_FABRIC_MULTICAST_QUEUES_RANGE_REG_FMC_QNUM_HIGH_ID:
      field = &(regs->lbp.fabric_multicast_queues_range_reg.fmc_qnum_high);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_prestera_related_reg_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    buffer,
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_prestera_related_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PRESTERA_RELATED_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_PRESTERA_RELATED_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_PRESTERA_RELATED_REG_REGISTER_ID:
      regis = &(regs->lbp.prestera_related_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_PRESTERA_RELATED_REG_CAP_MPTPORT63_ID:
      field = &(regs->lbp.prestera_related_reg.cap_mptport63);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_PRESTERA_RELATED_REG_CAP_MPTTRAP2_CPU_ID:
      field = &(regs->lbp.prestera_related_reg.cap_mpttrap2_cpu);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_PRESTERA_RELATED_REG_CAP_MPTCOPY_ID:
      field = &(regs->lbp.prestera_related_reg.cap_mptcopy);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "lbp.prestera_related_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_prestera_related_reg_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  uint32
    ret;
  uint32
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_prestera_related_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PRESTERA_RELATED_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_PRESTERA_RELATED_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_PRESTERA_RELATED_REG_REGISTER_ID:
      regis = &(regs->lbp.prestera_related_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_PRESTERA_RELATED_REG_CAP_MPTPORT63_ID:
      field = &(regs->lbp.prestera_related_reg.cap_mptport63);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_PRESTERA_RELATED_REG_CAP_MPTTRAP2_CPU_ID:
      field = &(regs->lbp.prestera_related_reg.cap_mpttrap2_cpu);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_PRESTERA_RELATED_REG_CAP_MPTCOPY_ID:
      field = &(regs->lbp.prestera_related_reg.cap_mptcopy);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_lst_crp_hdr_2_reg_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    buffer,
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_lst_crp_hdr_2_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_LST_CRP_HDR_2_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_LST_CRP_HDR_2_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_LST_CRP_HDR_2_REG_REGISTER_ID:
      regis = &(regs->lbp.lst_crp_hdr_2_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_LST_CRP_HDR_2_REG_LST_HDR_CRP_HDR_2_ID:
      field = &(regs->lbp.lst_crp_hdr_2_reg.lst_hdr_crp_hdr_2);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "lbp.lst_crp_hdr_2_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_lst_crp_hdr_2_reg_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  uint32
    ret;
  uint32
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_lst_crp_hdr_2_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_LST_CRP_HDR_2_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_LST_CRP_HDR_2_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_LST_CRP_HDR_2_REG_REGISTER_ID:
      regis = &(regs->lbp.lst_crp_hdr_2_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_LST_CRP_HDR_2_REG_LST_HDR_CRP_HDR_2_ID:
      field = &(regs->lbp.lst_crp_hdr_2_reg.lst_hdr_crp_hdr_2);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_packet_header_filter_b_reg_1_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    buffer,
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_packet_header_filter_b_reg_1_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_1_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_1_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_1_REGISTER_ID:
      regis = &(regs->lbp.packet_header_filter_b_reg_1.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_1_FILTER_MASK_B_31_0_ID:
      field = &(regs->lbp.packet_header_filter_b_reg_1.filter_mask_b_31_0);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "lbp.packet_header_filter_b_reg_1: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_packet_header_filter_b_reg_1_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  uint32
    ret;
  uint32
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_packet_header_filter_b_reg_1_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_1_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_1_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_1_REGISTER_ID:
      regis = &(regs->lbp.packet_header_filter_b_reg_1.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_1_FILTER_MASK_B_31_0_ID:
      field = &(regs->lbp.packet_header_filter_b_reg_1.filter_mask_b_31_0);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_indirect_command_rd_data_reg_0_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    buffer,
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_indirect_command_rd_data_reg_0_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_INDIRECT_COMMAND_RD_DATA_REG_0_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_INDIRECT_COMMAND_RD_DATA_REG_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_INDIRECT_COMMAND_RD_DATA_REG_0_REGISTER_ID:
      regis = &(regs->lbp.indirect_command_rd_data_reg_0.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_INDIRECT_COMMAND_RD_DATA_REG_0_INDIRECT_COMMAND_RD_DATA_ID:
      field = &(regs->lbp.indirect_command_rd_data_reg_0.indirect_command_rd_data);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "lbp.indirect_command_rd_data_reg_0: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_indirect_command_rd_data_reg_0_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  uint32
    ret;
  uint32
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_indirect_command_rd_data_reg_0_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_INDIRECT_COMMAND_RD_DATA_REG_0_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_INDIRECT_COMMAND_RD_DATA_REG_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_INDIRECT_COMMAND_RD_DATA_REG_0_REGISTER_ID:
      regis = &(regs->lbp.indirect_command_rd_data_reg_0.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_INDIRECT_COMMAND_RD_DATA_REG_0_INDIRECT_COMMAND_RD_DATA_ID:
      field = &(regs->lbp.indirect_command_rd_data_reg_0.indirect_command_rd_data);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_petra_ftmh_reg_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    buffer,
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_petra_ftmh_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PETRA_FTMH_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_PETRA_FTMH_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_PETRA_FTMH_REG_REGISTER_ID:
      regis = &(regs->lbp.soc_petra_ftmh_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_PETRA_FTMH_REG_PETRA_FTMHEXT_UCMSB_ID:
      field = &(regs->lbp.soc_petra_ftmh_reg.soc_petra_ftmhext_ucmsb);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_PETRA_FTMH_REG_PETRA_FTMHEXT_FLWMSB_ID:
      field = &(regs->lbp.soc_petra_ftmh_reg.soc_petra_ftmhext_flwmsb);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_PETRA_FTMH_REG_PETRA_FTMHEXT_MCMSB_ID:
      field = &(regs->lbp.soc_petra_ftmh_reg.soc_petra_ftmhext_mcmsb);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_PETRA_FTMH_REG_PETRA_FTMHMC64_FAPS_ID:
      field = &(regs->lbp.soc_petra_ftmh_reg.soc_petra_ftmhmc64_faps);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "lbp.soc_petra_ftmh_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_petra_ftmh_reg_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  uint32
    ret;
  uint32
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_petra_ftmh_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PETRA_FTMH_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_PETRA_FTMH_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_PETRA_FTMH_REG_REGISTER_ID:
      regis = &(regs->lbp.soc_petra_ftmh_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_PETRA_FTMH_REG_PETRA_FTMHEXT_UCMSB_ID:
      field = &(regs->lbp.soc_petra_ftmh_reg.soc_petra_ftmhext_ucmsb);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_PETRA_FTMH_REG_PETRA_FTMHEXT_FLWMSB_ID:
      field = &(regs->lbp.soc_petra_ftmh_reg.soc_petra_ftmhext_flwmsb);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_PETRA_FTMH_REG_PETRA_FTMHEXT_MCMSB_ID:
      field = &(regs->lbp.soc_petra_ftmh_reg.soc_petra_ftmhext_mcmsb);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_PETRA_FTMH_REG_PETRA_FTMHMC64_FAPS_ID:
      field = &(regs->lbp.soc_petra_ftmh_reg.soc_petra_ftmhmc64_faps);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_packet_header_filter_b_reg_4_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    buffer,
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_packet_header_filter_b_reg_4_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_4_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_4_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_4_REGISTER_ID:
      regis = &(regs->lbp.packet_header_filter_b_reg_4.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_4_FILTER_HDR_B_79_64_ID:
      field = &(regs->lbp.packet_header_filter_b_reg_4.filter_hdr_b_79_64);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "lbp.packet_header_filter_b_reg_4: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_packet_header_filter_b_reg_4_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  uint32
    ret;
  uint32
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_packet_header_filter_b_reg_4_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_4_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_4_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_4_REGISTER_ID:
      regis = &(regs->lbp.packet_header_filter_b_reg_4.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_4_FILTER_HDR_B_79_64_ID:
      field = &(regs->lbp.packet_header_filter_b_reg_4.filter_hdr_b_79_64);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_packet_header_filter_a_reg_1_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    buffer,
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_packet_header_filter_a_reg_1_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_1_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_1_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_1_REGISTER_ID:
      regis = &(regs->lbp.packet_header_filter_a_reg_1.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_1_FILTER_MASK_A_31_0_ID:
      field = &(regs->lbp.packet_header_filter_a_reg_1.filter_mask_a_31_0);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "lbp.packet_header_filter_a_reg_1: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_packet_header_filter_a_reg_1_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  uint32
    ret;
  uint32
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_packet_header_filter_a_reg_1_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_1_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_1_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_1_REGISTER_ID:
      regis = &(regs->lbp.packet_header_filter_a_reg_1.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_1_FILTER_MASK_A_31_0_ID:
      field = &(regs->lbp.packet_header_filter_a_reg_1.filter_mask_a_31_0);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_discarde_packet_counter_reg_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    buffer,
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_discarde_packet_counter_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_DISCARDE_PACKET_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_DISCARDE_PACKET_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_DISCARDE_PACKET_COUNTER_REG_REGISTER_ID:
      regis = &(regs->lbp.discarde_packet_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_DISCARDE_PACKET_COUNTER_REG_PKT_CNT_DSCRD_TARGET_Q_ID:
      field = &(regs->lbp.discarde_packet_counter_reg.pkt_cnt_dscrd_target_q);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "lbp.discarde_packet_counter_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_discarde_packet_counter_reg_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  uint32
    ret;
  uint32
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_discarde_packet_counter_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_DISCARDE_PACKET_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_DISCARDE_PACKET_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_DISCARDE_PACKET_COUNTER_REG_REGISTER_ID:
      regis = &(regs->lbp.discarde_packet_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_DISCARDE_PACKET_COUNTER_REG_PKT_CNT_DSCRD_TARGET_Q_ID:
      field = &(regs->lbp.discarde_packet_counter_reg.pkt_cnt_dscrd_target_q);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_class3to2_mapping_reg_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    buffer,
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_class3to2_mapping_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_CLASS3TO2_MAPPING_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_CLASS3TO2_MAPPING_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_CLASS3TO2_MAPPING_REG_REGISTER_ID:
      regis = &(regs->lbp.class3to2_mapping_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_CLASS3TO2_MAPPING_REG_MAP_CLASS3TO2_0_ID:
      field = &(regs->lbp.class3to2_mapping_reg.map_class3to2_0);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_CLASS3TO2_MAPPING_REG_MAP_CLASS3TO2_1_ID:
      field = &(regs->lbp.class3to2_mapping_reg.map_class3to2_1);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_CLASS3TO2_MAPPING_REG_MAP_CLASS3TO2_2_ID:
      field = &(regs->lbp.class3to2_mapping_reg.map_class3to2_2);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_CLASS3TO2_MAPPING_REG_MAP_CLASS3TO2_3_ID:
      field = &(regs->lbp.class3to2_mapping_reg.map_class3to2_3);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_CLASS3TO2_MAPPING_REG_MAP_CLASS3TO2_4_ID:
      field = &(regs->lbp.class3to2_mapping_reg.map_class3to2_4);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_CLASS3TO2_MAPPING_REG_MAP_CLASS3TO2_5_ID:
      field = &(regs->lbp.class3to2_mapping_reg.map_class3to2_5);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_CLASS3TO2_MAPPING_REG_MAP_CLASS3TO2_6_ID:
      field = &(regs->lbp.class3to2_mapping_reg.map_class3to2_6);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_CLASS3TO2_MAPPING_REG_MAP_CLASS3TO2_7_ID:
      field = &(regs->lbp.class3to2_mapping_reg.map_class3to2_7);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "lbp.class3to2_mapping_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_class3to2_mapping_reg_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  uint32
    ret;
  uint32
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_class3to2_mapping_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_CLASS3TO2_MAPPING_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_CLASS3TO2_MAPPING_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_CLASS3TO2_MAPPING_REG_REGISTER_ID:
      regis = &(regs->lbp.class3to2_mapping_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_CLASS3TO2_MAPPING_REG_MAP_CLASS3TO2_0_ID:
      field = &(regs->lbp.class3to2_mapping_reg.map_class3to2_0);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_CLASS3TO2_MAPPING_REG_MAP_CLASS3TO2_1_ID:
      field = &(regs->lbp.class3to2_mapping_reg.map_class3to2_1);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_CLASS3TO2_MAPPING_REG_MAP_CLASS3TO2_2_ID:
      field = &(regs->lbp.class3to2_mapping_reg.map_class3to2_2);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_CLASS3TO2_MAPPING_REG_MAP_CLASS3TO2_3_ID:
      field = &(regs->lbp.class3to2_mapping_reg.map_class3to2_3);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_CLASS3TO2_MAPPING_REG_MAP_CLASS3TO2_4_ID:
      field = &(regs->lbp.class3to2_mapping_reg.map_class3to2_4);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_CLASS3TO2_MAPPING_REG_MAP_CLASS3TO2_5_ID:
      field = &(regs->lbp.class3to2_mapping_reg.map_class3to2_5);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_CLASS3TO2_MAPPING_REG_MAP_CLASS3TO2_6_ID:
      field = &(regs->lbp.class3to2_mapping_reg.map_class3to2_6);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_CLASS3TO2_MAPPING_REG_MAP_CLASS3TO2_7_ID:
      field = &(regs->lbp.class3to2_mapping_reg.map_class3to2_7);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_packet_header_filter_b_reg_3_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    buffer,
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_packet_header_filter_b_reg_3_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_3_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_3_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_3_REGISTER_ID:
      regis = &(regs->lbp.packet_header_filter_b_reg_3.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_3_FILTER_MASK_B_63_32_ID:
      field = &(regs->lbp.packet_header_filter_b_reg_3.filter_mask_b_63_32);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "lbp.packet_header_filter_b_reg_3: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_packet_header_filter_b_reg_3_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  uint32
    ret;
  uint32
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_packet_header_filter_b_reg_3_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_3_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_3_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_3_REGISTER_ID:
      regis = &(regs->lbp.packet_header_filter_b_reg_3.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_3_FILTER_MASK_B_63_32_ID:
      field = &(regs->lbp.packet_header_filter_b_reg_3.filter_mask_b_63_32);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_packet_header_filter_b_reg_2_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    buffer,
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_packet_header_filter_b_reg_2_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_2_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_2_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_2_REGISTER_ID:
      regis = &(regs->lbp.packet_header_filter_b_reg_2.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_2_FILTER_HDR_B_63_32_ID:
      field = &(regs->lbp.packet_header_filter_b_reg_2.filter_hdr_b_63_32);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "lbp.packet_header_filter_b_reg_2: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_packet_header_filter_b_reg_2_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  uint32
    ret;
  uint32
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_packet_header_filter_b_reg_2_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_2_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_2_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_2_REGISTER_ID:
      regis = &(regs->lbp.packet_header_filter_b_reg_2.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_2_FILTER_HDR_B_63_32_ID:
      field = &(regs->lbp.packet_header_filter_b_reg_2.filter_hdr_b_63_32);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_packet_header_filter_a_reg_5_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    buffer,
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_packet_header_filter_a_reg_5_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_5_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_5_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_5_REGISTER_ID:
      regis = &(regs->lbp.packet_header_filter_a_reg_5.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_5_FILTER_MASK_A_79_64_ID:
      field = &(regs->lbp.packet_header_filter_a_reg_5.filter_mask_a_79_64);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "lbp.packet_header_filter_a_reg_5: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_packet_header_filter_a_reg_5_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  uint32
    ret;
  uint32
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_packet_header_filter_a_reg_5_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_5_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_5_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_5_REGISTER_ID:
      regis = &(regs->lbp.packet_header_filter_a_reg_5.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_5_FILTER_MASK_A_79_64_ID:
      field = &(regs->lbp.packet_header_filter_a_reg_5.filter_mask_a_79_64);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_packet_counter_configuration_reg_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    buffer,
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_packet_counter_configuration_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_COUNTER_CONFIGURATION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_COUNTER_CONFIGURATION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_COUNTER_CONFIGURATION_REG_REGISTER_ID:
      regis = &(regs->lbp.packet_counter_configuration_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_COUNTER_CONFIGURATION_REG_PRG_CNT_TARGET_Q_ID:
      field = &(regs->lbp.packet_counter_configuration_reg.prg_cnt_target_q);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_COUNTER_CONFIGURATION_REG_PRG_CNT_TARGET_QMASK_ID:
      field = &(regs->lbp.packet_counter_configuration_reg.prg_cnt_target_qmask);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "lbp.packet_counter_configuration_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_packet_counter_configuration_reg_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  uint32
    ret;
  uint32
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_packet_counter_configuration_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_COUNTER_CONFIGURATION_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_COUNTER_CONFIGURATION_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_COUNTER_CONFIGURATION_REG_REGISTER_ID:
      regis = &(regs->lbp.packet_counter_configuration_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_COUNTER_CONFIGURATION_REG_PRG_CNT_TARGET_Q_ID:
      field = &(regs->lbp.packet_counter_configuration_reg.prg_cnt_target_q);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_COUNTER_CONFIGURATION_REG_PRG_CNT_TARGET_QMASK_ID:
      field = &(regs->lbp.packet_counter_configuration_reg.prg_cnt_target_qmask);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_lst_crp_hdr_1_reg_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    buffer,
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_lst_crp_hdr_1_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_LST_CRP_HDR_1_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_LST_CRP_HDR_1_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_LST_CRP_HDR_1_REG_REGISTER_ID:
      regis = &(regs->lbp.lst_crp_hdr_1_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_LST_CRP_HDR_1_REG_LST_HDR_CRP_HDR_1_ID:
      field = &(regs->lbp.lst_crp_hdr_1_reg.lst_hdr_crp_hdr_1);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "lbp.lst_crp_hdr_1_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_lst_crp_hdr_1_reg_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  uint32
    ret;
  uint32
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_lst_crp_hdr_1_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_LST_CRP_HDR_1_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_LST_CRP_HDR_1_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_LST_CRP_HDR_1_REG_REGISTER_ID:
      regis = &(regs->lbp.lst_crp_hdr_1_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_LST_CRP_HDR_1_REG_LST_HDR_CRP_HDR_1_ID:
      field = &(regs->lbp.lst_crp_hdr_1_reg.lst_hdr_crp_hdr_1);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_un_init_entry_reg_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    buffer,
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_un_init_entry_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_UN_INIT_ENTRY_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_UN_INIT_ENTRY_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_UN_INIT_ENTRY_REG_REGISTER_ID:
      regis = &(regs->lbp.un_init_entry_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_UN_INIT_ENTRY_REG_UN_INITADDRESS_ID:
      field = &(regs->lbp.un_init_entry_reg.un_initaddress);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_UN_INIT_ENTRY_REG_UN_INIT_PKT_TYPE_ID:
      field = &(regs->lbp.un_init_entry_reg.un_init_pkt_type);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "lbp.un_init_entry_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_un_init_entry_reg_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  uint32
    ret;
  uint32
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_un_init_entry_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_UN_INIT_ENTRY_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_UN_INIT_ENTRY_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_UN_INIT_ENTRY_REG_REGISTER_ID:
      regis = &(regs->lbp.un_init_entry_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_UN_INIT_ENTRY_REG_UN_INITADDRESS_ID:
      field = &(regs->lbp.un_init_entry_reg.un_initaddress);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_UN_INIT_ENTRY_REG_UN_INIT_PKT_TYPE_ID:
      field = &(regs->lbp.un_init_entry_reg.un_init_pkt_type);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_lst_crp_hdr_0_reg_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    buffer,
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_lst_crp_hdr_0_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_LST_CRP_HDR_0_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_LST_CRP_HDR_0_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_LST_CRP_HDR_0_REG_REGISTER_ID:
      regis = &(regs->lbp.lst_crp_hdr_0_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_LST_CRP_HDR_0_REG_LST_HDR_CRP_HDR_0_ID:
      field = &(regs->lbp.lst_crp_hdr_0_reg.lst_hdr_crp_hdr_0);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "lbp.lst_crp_hdr_0_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_lst_crp_hdr_0_reg_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  uint32
    ret;
  uint32
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_lst_crp_hdr_0_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_LST_CRP_HDR_0_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_LST_CRP_HDR_0_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_LST_CRP_HDR_0_REG_REGISTER_ID:
      regis = &(regs->lbp.lst_crp_hdr_0_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_LST_CRP_HDR_0_REG_LST_HDR_CRP_HDR_0_ID:
      field = &(regs->lbp.lst_crp_hdr_0_reg.lst_hdr_crp_hdr_0);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_indirect_command_address_reg_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    buffer,
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_indirect_command_address_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_INDIRECT_COMMAND_ADDRESS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_INDIRECT_COMMAND_ADDRESS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_INDIRECT_COMMAND_ADDRESS_REG_REGISTER_ID:
      regis = &(regs->lbp.indirect_command_address_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_INDIRECT_COMMAND_ADDRESS_REG_INDIRECT_COMMAND_ADDRESS_ID:
      field = &(regs->lbp.indirect_command_address_reg.indirect_command_address);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "lbp.indirect_command_address_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_indirect_command_address_reg_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  uint32
    ret;
  uint32
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_indirect_command_address_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_INDIRECT_COMMAND_ADDRESS_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_INDIRECT_COMMAND_ADDRESS_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_INDIRECT_COMMAND_ADDRESS_REG_REGISTER_ID:
      regis = &(regs->lbp.indirect_command_address_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_INDIRECT_COMMAND_ADDRESS_REG_INDIRECT_COMMAND_ADDRESS_ID:
      field = &(regs->lbp.indirect_command_address_reg.indirect_command_address);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_packet_counter_reg_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    buffer,
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_packet_counter_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_COUNTER_REG_REGISTER_ID:
      regis = &(regs->lbp.packet_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_COUNTER_REG_PKT_CNT_ID:
      field = &(regs->lbp.packet_counter_reg.pkt_cnt);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "lbp.packet_counter_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_packet_counter_reg_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  uint32
    ret;
  uint32
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_packet_counter_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_COUNTER_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_COUNTER_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_COUNTER_REG_REGISTER_ID:
      regis = &(regs->lbp.packet_counter_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_COUNTER_REG_PKT_CNT_ID:
      field = &(regs->lbp.packet_counter_reg.pkt_cnt);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_lbp_configuration_1_reg_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    buffer,
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_lbp_configuration_1_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_LBP_CONFIGURATION_1_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_LBP_CONFIGURATION_1_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_LBP_CONFIGURATION_1_REG_REGISTER_ID:
      regis = &(regs->lbp.lbp_configuration_1_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_LBP_CONFIGURATION_1_REG_DSCRD_ALL_DATA_ID:
      field = &(regs->lbp.lbp_configuration_1_reg.dscrd_all_data);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_LBP_CONFIGURATION_1_REG_TREE_LVL_CNT_TH_ID:
      field = &(regs->lbp.lbp_configuration_1_reg.tree_lvl_cnt_th);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_LBP_CONFIGURATION_1_REG_PKT_CNT_HDR_TYPE_ID:
      field = &(regs->lbp.lbp_configuration_1_reg.pkt_cnt_hdr_type);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_LBP_CONFIGURATION_1_REG_DSCRD_ALL_TDM_ID:
      field = &(regs->lbp.lbp_configuration_1_reg.dscrd_all_tdm);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_LBP_CONFIGURATION_1_REG_OVR_LSBEN_ID:
      field = &(regs->lbp.lbp_configuration_1_reg.ovr_lsben);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_LBP_CONFIGURATION_1_REG_INBOUND_MIRROR_CLASS_EN_ID:
      field = &(regs->lbp.lbp_configuration_1_reg.inbound_mirror_class_en);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_LBP_CONFIGURATION_1_REG_OUTBOUND_MIRROR_CLASS_EN_ID:
      field = &(regs->lbp.lbp_configuration_1_reg.outbound_mirror_class_en);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_LBP_CONFIGURATION_1_REG_ENA_FAP21_NPU_ID:
      field = &(regs->lbp.lbp_configuration_1_reg.ena_fap21_npu);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_LBP_CONFIGURATION_1_REG_ISHP_PETRA_ITMH_EXT_ENA_ID:
      field = &(regs->lbp.lbp_configuration_1_reg.ishp_petra_itmh_ext_ena);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_LBP_CONFIGURATION_1_REG_FILTER_ENA_ID:
      field = &(regs->lbp.lbp_configuration_1_reg.filter_ena);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_LBP_CONFIGURATION_1_REG_FLOW_CLASS_ENA_ID:
      field = &(regs->lbp.lbp_configuration_1_reg.flow_class_ena);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "lbp.lbp_configuration_1_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_lbp_configuration_1_reg_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  uint32
    ret;
  uint32
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_lbp_configuration_1_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_LBP_CONFIGURATION_1_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_LBP_CONFIGURATION_1_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_LBP_CONFIGURATION_1_REG_REGISTER_ID:
      regis = &(regs->lbp.lbp_configuration_1_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_LBP_CONFIGURATION_1_REG_DSCRD_ALL_DATA_ID:
      field = &(regs->lbp.lbp_configuration_1_reg.dscrd_all_data);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_LBP_CONFIGURATION_1_REG_TREE_LVL_CNT_TH_ID:
      field = &(regs->lbp.lbp_configuration_1_reg.tree_lvl_cnt_th);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_LBP_CONFIGURATION_1_REG_PKT_CNT_HDR_TYPE_ID:
      field = &(regs->lbp.lbp_configuration_1_reg.pkt_cnt_hdr_type);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_LBP_CONFIGURATION_1_REG_DSCRD_ALL_TDM_ID:
      field = &(regs->lbp.lbp_configuration_1_reg.dscrd_all_tdm);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_LBP_CONFIGURATION_1_REG_OVR_LSBEN_ID:
      field = &(regs->lbp.lbp_configuration_1_reg.ovr_lsben);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_LBP_CONFIGURATION_1_REG_INBOUND_MIRROR_CLASS_EN_ID:
      field = &(regs->lbp.lbp_configuration_1_reg.inbound_mirror_class_en);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_LBP_CONFIGURATION_1_REG_OUTBOUND_MIRROR_CLASS_EN_ID:
      field = &(regs->lbp.lbp_configuration_1_reg.outbound_mirror_class_en);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_LBP_CONFIGURATION_1_REG_ENA_FAP21_NPU_ID:
      field = &(regs->lbp.lbp_configuration_1_reg.ena_fap21_npu);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_LBP_CONFIGURATION_1_REG_ISHP_PETRA_ITMH_EXT_ENA_ID:
      field = &(regs->lbp.lbp_configuration_1_reg.ishp_petra_itmh_ext_ena);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_LBP_CONFIGURATION_1_REG_FILTER_ENA_ID:
      field = &(regs->lbp.lbp_configuration_1_reg.filter_ena);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_LBP_CONFIGURATION_1_REG_FLOW_CLASS_ENA_ID:
      field = &(regs->lbp.lbp_configuration_1_reg.flow_class_ena);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_packet_header_filter_a_reg_0_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    buffer,
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_packet_header_filter_a_reg_0_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_0_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_0_REGISTER_ID:
      regis = &(regs->lbp.packet_header_filter_a_reg_0.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_0_FILTER_HDR_A_31_0_ID:
      field = &(regs->lbp.packet_header_filter_a_reg_0.filter_hdr_a_31_0);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "lbp.packet_header_filter_a_reg_0: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_packet_header_filter_a_reg_0_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  uint32
    ret;
  uint32
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_packet_header_filter_a_reg_0_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_0_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_0_REGISTER_ID:
      regis = &(regs->lbp.packet_header_filter_a_reg_0.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_0_FILTER_HDR_A_31_0_ID:
      field = &(regs->lbp.packet_header_filter_a_reg_0.filter_hdr_a_31_0);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_indirect_command_reg_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    buffer,
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_indirect_command_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_INDIRECT_COMMAND_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_INDIRECT_COMMAND_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_INDIRECT_COMMAND_REG_REGISTER_ID:
      regis = &(regs->lbp.indirect_command_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_INDIRECT_COMMAND_REG_INDIRECT_COMMAND_ID:
      field = &(regs->lbp.indirect_command_reg.indirect_command);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "lbp.indirect_command_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_indirect_command_reg_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  uint32
    ret;
  uint32
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_indirect_command_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_INDIRECT_COMMAND_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_INDIRECT_COMMAND_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_INDIRECT_COMMAND_REG_REGISTER_ID:
      regis = &(regs->lbp.indirect_command_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_INDIRECT_COMMAND_REG_INDIRECT_COMMAND_ID:
      field = &(regs->lbp.indirect_command_reg.indirect_command);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_inbound_mirror_enable_b_63_to_32_reg_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    buffer,
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_inbound_mirror_enable_b_63_to_32_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_INBOUND_MIRROR_ENABLE_B_63_TO_32_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_INBOUND_MIRROR_ENABLE_B_63_TO_32_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_INBOUND_MIRROR_ENABLE_B_63_TO_32_REG_REGISTER_ID:
      regis = &(regs->lbp.inbound_mirror_enable_b_63_to_32_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_INBOUND_MIRROR_ENABLE_B_63_TO_32_REG_IN_BND_MIR_EN_ID:
      field = &(regs->lbp.inbound_mirror_enable_b_63_to_32_reg.in_bnd_mir_en);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "lbp.inbound_mirror_enable_b_63_to_32_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_inbound_mirror_enable_b_63_to_32_reg_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  uint32
    ret;
  uint32
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_inbound_mirror_enable_b_63_to_32_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_INBOUND_MIRROR_ENABLE_B_63_TO_32_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_INBOUND_MIRROR_ENABLE_B_63_TO_32_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_INBOUND_MIRROR_ENABLE_B_63_TO_32_REG_REGISTER_ID:
      regis = &(regs->lbp.inbound_mirror_enable_b_63_to_32_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_INBOUND_MIRROR_ENABLE_B_63_TO_32_REG_IN_BND_MIR_EN_ID:
      field = &(regs->lbp.inbound_mirror_enable_b_63_to_32_reg.in_bnd_mir_en);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_inbound_mirror_enable_a_31_to_0_reg_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    buffer,
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_inbound_mirror_enable_a_31_to_0_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_INBOUND_MIRROR_ENABLE_A_31_TO_0_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_INBOUND_MIRROR_ENABLE_A_31_TO_0_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_INBOUND_MIRROR_ENABLE_A_31_TO_0_REG_REGISTER_ID:
      regis = &(regs->lbp.inbound_mirror_enable_a_31_to_0_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_INBOUND_MIRROR_ENABLE_A_31_TO_0_REG_IN_BND_MIR_EN_ID:
      field = &(regs->lbp.inbound_mirror_enable_a_31_to_0_reg.in_bnd_mir_en);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "lbp.inbound_mirror_enable_a_31_to_0_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_inbound_mirror_enable_a_31_to_0_reg_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  uint32
    ret;
  uint32
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_inbound_mirror_enable_a_31_to_0_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_INBOUND_MIRROR_ENABLE_A_31_TO_0_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_INBOUND_MIRROR_ENABLE_A_31_TO_0_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_INBOUND_MIRROR_ENABLE_A_31_TO_0_REG_REGISTER_ID:
      regis = &(regs->lbp.inbound_mirror_enable_a_31_to_0_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_INBOUND_MIRROR_ENABLE_A_31_TO_0_REG_IN_BND_MIR_EN_ID:
      field = &(regs->lbp.inbound_mirror_enable_a_31_to_0_reg.in_bnd_mir_en);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_initialize_tables_reg_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    buffer,
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_initialize_tables_reg_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_INITIALIZE_TABLES_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_INITIALIZE_TABLES_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_INITIALIZE_TABLES_REG_REGISTER_ID:
      regis = &(regs->lbp.initialize_tables_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_INITIALIZE_TABLES_REG_INIT_TRG_ID:
      field = &(regs->lbp.initialize_tables_reg.init_trg);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "lbp.initialize_tables_reg: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_initialize_tables_reg_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  uint32
    ret;
  uint32
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_initialize_tables_reg_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_INITIALIZE_TABLES_REG_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_INITIALIZE_TABLES_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_INITIALIZE_TABLES_REG_REGISTER_ID:
      regis = &(regs->lbp.initialize_tables_reg.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_INITIALIZE_TABLES_REG_INIT_TRG_ID:
      field = &(regs->lbp.initialize_tables_reg.init_trg);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_packet_header_filter_b_reg_5_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    buffer,
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_packet_header_filter_b_reg_5_get";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_5_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_5_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_5_REGISTER_ID:
      regis = &(regs->lbp.packet_header_filter_b_reg_5.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_5_FILTER_MASK_B_79_64_ID:
      field = &(regs->lbp.packet_header_filter_b_reg_5.filter_mask_b_79_64);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_read_fld(
            unit,
            field,
            0,
            &buffer
          );
  }
  else
  {
    ret = fap21v_read_reg(
            unit,
            regis,
            0,
            &buffer
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);
  soc_sand_os_printf( "lbp.packet_header_filter_b_reg_5: %lX\n",buffer);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_packet_header_filter_b_reg_5_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  uint32
    ret;
  uint32
    field_val = 0;
  FAP21V_REGS
    *regs = fap21v_regs();
  FAP21V_REG_FIELD
    *field = NULL;
  FAP21V_REG_ADDR
    *regis = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp");
  soc_sand_proc_name = "ui_fap21v_acc_lbp_packet_header_filter_b_reg_5_set";

  unit = get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_5_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_5_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    field_val = param_val->numeric_equivalent;
  }
  switch (field_val)
  {
    case PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_5_REGISTER_ID:
      regis = &(regs->lbp.packet_header_filter_b_reg_5.addr);
      break;
    case PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_5_FILTER_MASK_B_79_64_ID:
      field = &(regs->lbp.packet_header_filter_b_reg_5.filter_mask_b_79_64);
      break;
    default :
      break;
  }
  if (regis == NULL)
  {
    ret = fap21v_write_fld(
            unit,
            field,
            0,
            value
          );
  }
  else
  {
    ret = fap21v_write_reg(
            unit,
            regis,
            0,
            value
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fap21v_read_reg - FAIL", TRUE);
    fap21v_disp_result(ret, "fap21v_read_reg");
    goto exit;
  }

  send_string_to_screen(" *** fap21v_read_reg - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_get(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp_get");

  if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_4_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_packet_header_filter_a_reg_4_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_CLASS2TO3_MAPPING_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_class2to3_mapping_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_LAG_CONFIGURATION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_lag_configuration_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_INTERRUPTS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_interrupts_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_UN_INITIALIZED_LAG_FWD_ADDRESS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_un_initialized_lag_fwd_address_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_LBP_CONFIGURATION_2_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_lbp_configuration_2_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_MASK_INTERRUPTS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_mask_interrupts_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_SNOOP_COMMANDS_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_snoop_commands_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_UN_INITIALIZED_LAG_SCND_ADDRESS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_un_initialized_lag_scnd_address_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_0_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_packet_header_filter_b_reg_0_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_3_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_packet_header_filter_a_reg_3_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_INDIRECT_COMMAND_WR_DATA_REG_0_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_indirect_command_wr_data_reg_0_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_2_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_packet_header_filter_a_reg_2_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_FABRIC_MULTICAST_QUEUES_RANGE_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_fabric_multicast_queues_range_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PRESTERA_RELATED_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_prestera_related_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_LST_CRP_HDR_2_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_lst_crp_hdr_2_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_1_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_packet_header_filter_b_reg_1_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_INDIRECT_COMMAND_RD_DATA_REG_0_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_indirect_command_rd_data_reg_0_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PETRA_FTMH_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_petra_ftmh_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_4_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_packet_header_filter_b_reg_4_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_1_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_packet_header_filter_a_reg_1_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_DISCARDE_PACKET_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_discarde_packet_counter_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_CLASS3TO2_MAPPING_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_class3to2_mapping_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_3_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_packet_header_filter_b_reg_3_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_2_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_packet_header_filter_b_reg_2_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_5_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_packet_header_filter_a_reg_5_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_COUNTER_CONFIGURATION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_packet_counter_configuration_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_LST_CRP_HDR_1_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_lst_crp_hdr_1_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_UN_INIT_ENTRY_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_un_init_entry_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_LST_CRP_HDR_0_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_lst_crp_hdr_0_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_INDIRECT_COMMAND_ADDRESS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_indirect_command_address_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_packet_counter_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_LBP_CONFIGURATION_1_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_lbp_configuration_1_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_0_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_packet_header_filter_a_reg_0_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_INDIRECT_COMMAND_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_indirect_command_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_INBOUND_MIRROR_ENABLE_B_63_TO_32_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_inbound_mirror_enable_b_63_to_32_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_INBOUND_MIRROR_ENABLE_A_31_TO_0_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_inbound_mirror_enable_a_31_to_0_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_INITIALIZE_TABLES_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_initialize_tables_reg_get(current_line);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_5_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_packet_header_filter_b_reg_5_get(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after lbp***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}
int
  ui_fap21v_acc_lbp_set(
    CURRENT_LINE *current_line,
    uint32     value
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_acc_lbp_set");

  if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_4_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_packet_header_filter_a_reg_4_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_CLASS2TO3_MAPPING_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_class2to3_mapping_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_LAG_CONFIGURATION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_lag_configuration_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_INTERRUPTS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_interrupts_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_UN_INITIALIZED_LAG_FWD_ADDRESS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_un_initialized_lag_fwd_address_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_LBP_CONFIGURATION_2_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_lbp_configuration_2_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_MASK_INTERRUPTS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_mask_interrupts_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_SNOOP_COMMANDS_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_snoop_commands_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_UN_INITIALIZED_LAG_SCND_ADDRESS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_un_initialized_lag_scnd_address_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_0_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_packet_header_filter_b_reg_0_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_3_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_packet_header_filter_a_reg_3_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_INDIRECT_COMMAND_WR_DATA_REG_0_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_indirect_command_wr_data_reg_0_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_2_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_packet_header_filter_a_reg_2_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_FABRIC_MULTICAST_QUEUES_RANGE_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_fabric_multicast_queues_range_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PRESTERA_RELATED_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_prestera_related_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_LST_CRP_HDR_2_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_lst_crp_hdr_2_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_1_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_packet_header_filter_b_reg_1_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_INDIRECT_COMMAND_RD_DATA_REG_0_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_indirect_command_rd_data_reg_0_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PETRA_FTMH_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_petra_ftmh_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_4_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_packet_header_filter_b_reg_4_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_1_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_packet_header_filter_a_reg_1_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_DISCARDE_PACKET_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_discarde_packet_counter_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_CLASS3TO2_MAPPING_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_class3to2_mapping_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_3_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_packet_header_filter_b_reg_3_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_2_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_packet_header_filter_b_reg_2_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_5_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_packet_header_filter_a_reg_5_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_COUNTER_CONFIGURATION_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_packet_counter_configuration_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_LST_CRP_HDR_1_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_lst_crp_hdr_1_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_UN_INIT_ENTRY_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_un_init_entry_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_LST_CRP_HDR_0_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_lst_crp_hdr_0_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_INDIRECT_COMMAND_ADDRESS_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_indirect_command_address_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_COUNTER_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_packet_counter_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_LBP_CONFIGURATION_1_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_lbp_configuration_1_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_A_REG_0_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_packet_header_filter_a_reg_0_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_INDIRECT_COMMAND_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_indirect_command_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_INBOUND_MIRROR_ENABLE_B_63_TO_32_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_inbound_mirror_enable_b_63_to_32_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_INBOUND_MIRROR_ENABLE_A_31_TO_0_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_inbound_mirror_enable_a_31_to_0_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_INITIALIZE_TABLES_REG_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_initialize_tables_reg_set(current_line, value);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_ACC_DIRECT_LBP_PACKET_HEADER_FILTER_B_REG_5_ID,1))
  {
    ui_ret = ui_fap21v_acc_lbp_packet_header_filter_b_reg_5_set(current_line, value);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after lbp***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}
