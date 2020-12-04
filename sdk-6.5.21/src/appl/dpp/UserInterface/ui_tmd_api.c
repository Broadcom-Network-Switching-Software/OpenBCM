/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#if LINK_TMD_LIBRARIES
/*
 * General include file for reference design.
 */
#include <appl/diag/dpp/ref_sys.h>

/*
 * Utilities include file.
 */

#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <appl/diag/dpp/utils_defi.h>
#include <appl/dpp/UserInterface/ui_defi.h>
/*#include <appl/dpp/UserInterface/ui_rom_defi_tmd_api.h> */
#include <appl/dpp/UserInterface/ui_pure_defi_tmd_api.h>

/*#include <soc/dpp/TMD/tmd_api_default_section.h>   */
#include <soc/dpp/TMD/tmd_api_mgmt.h>
#include <soc/dpp/TMD/tmd_api_general.h>
#include <soc/dpp/TMD/tmd_api_ports.h>

#include <soc/dpp/TMD/tmd_api_diagnostics.h>
#include <soc/dpp/TMD/tmd_api_ingress_packet_queuing.h>
#include <soc/dpp/TMD/tmd_api_ingress_traffic_mgmt.h>
#include <soc/dpp/TMD/tmd_api_ingress_scheduler.h>
#include <soc/dpp/TMD/tmd_api_egr_queuing.h>
#include <soc/dpp/TMD/tmd_api_ofp_rates.h>
#include <soc/dpp/TMD/tmd_api_end2end_scheduler.h>
#include <soc/dpp/TMD/tmd_api_multicast_ingress.h>
#include <soc/dpp/TMD/tmd_api_multicast_egress.h>
#include <soc/dpp/TMD/tmd_api_multicast_fabric.h>
#include <soc/dpp/TMD/tmd_api_fabric.h>
#include <soc/dpp/TMD/tmd_api_header_parsing_utils.h>

#include <soc/dpp/TMD/tmd_api_reg_access.h>
#include <soc/dpp/SAND/SAND_FM/sand_mem_access.h>
#include <soc/dpp/TMC/tmc_api_reg_access.h>
#include <soc/dpp/TMC/tmc_api_framework.h>

#include <soc/dpp/Petra/petra_sw_db.h>
#include <soc/dpp/Petra/petra_scheduler_elements.h>
#include <soc/dpp/Petra/petra_scheduler_end2end.h>

#ifndef __DUNE_SSF__
  #include <sweep/Petra/swp_p_tm_app_cpu_queue.h>
#endif
#include <soc/dpp/TMD/tmd_api_packet.h>


#include <soc/dpp/TMD/tmd_api_cell.h>
#include <soc/dpp/TMD/tmd_api_debug.h>

#include <soc/dpp/SAND/Management/sand_low_level.h>


#undef  UI_UNDEFED
#undef  UI_UNDEFED_TEMP
#define UI_UTILS
#define UI_MGMT
#define UI_PORTS
#define UI_INGRESS_PACKET_QUEUING
#define UI_INGRESS_TRAFFIC_MGMT
#define UI_INGRESS_SCHEDULER
#define UI_EGR_QUEUING
#define UI_OFP_RATES
#define UI_END2END_SCHEDULER
#define UI_MULTICAST_INGRESS
#define UI_MULTICAST_EGRESS
#define UI_MULTICAST_FABRIC
#define UI_FABRIC
#define UI_TMD_HEADER_PARSING_UTILS
#define UI_REG_ACCESS
#define UI_PACKET
#define UI_CELL
#define UI_DEBUG

static
  uint32
    Default_unit = 0;

void
  tmd_set_default_unit(uint32 dev_id)
{
  Default_unit = dev_id;
}

uint32
  tmd_get_default_unit()
{
  return Default_unit;
}

STATIC uint8 ui_tmd_param_is_initialized(
              uint32 param,
              char* param_str
            )
{
  uint8
    is_initialized = TRUE;

  if (param == 0xFFFFFFFF)
  {
    send_string_to_screen(" *** SW error - the following parameter must be set first:  ", TRUE);
    send_string_to_screen(param_str, TRUE);
    is_initialized = FALSE;
  }

  return is_initialized;
}

#define UI_PARAM_INIT_VALIDATE_OR_EXIT(ui_param, ui_param_str)  \
{                                                 \
  if (!ui_tmd_param_is_initialized(ui_param, ui_param_str))   \
  {                                               \
    goto exit;                                    \
  }                                               \
}

#ifdef UI_UTILS

int
  ui_tmd_api_set_default_unit(
    CURRENT_LINE *current_line
  )
{
  uint32
    prm_unit = 0;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_utils");
  soc_sand_proc_name = "tmd_set_default_unit";

  prm_unit = tmd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_UTILS_SET_DEFAULT_DEVICE_ID_DEVICE_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_UTILS_SET_DEFAULT_DEVICE_ID_DEVICE_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_unit = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  tmd_set_default_unit(
          prm_unit
        );

  soc_sand_os_printf( "unit: %d\n\r",prm_unit);


  goto exit;
exit:
  return ui_ret;
}

int
  ui_tmd_api_get_default_unit(
    CURRENT_LINE *current_line
  )
{
  uint32
    prm_unit = 0;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_utils");
  soc_sand_proc_name = "tmd_get_default_unit";

  unit = tmd_get_default_unit();

  prm_unit = unit;

  soc_sand_os_printf( "unit: %d\n\r",prm_unit);

  goto exit;
exit:
  return ui_ret;
}

#if UI_UNDEFED_TEMP
int
  ui_tmd_api_tmd_revision_get(
    CURRENT_LINE *current_line
  )
{
  TMD_REV
    prm_rev = TMD_NOF_REVS;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_utils");
  soc_sand_proc_name = "ui_tmd_api_tmd_revision_get";

  unit = tmd_get_default_unit();

  prm_rev = tmd_revision_get(
          unit
        );

  soc_sand_os_printf( "Tmd revision: %s\n\r",TMD_REV_to_string(prm_rev));

  goto exit;
exit:
  return ui_ret;
}

#endif
#endif /* UI_UNDEFED_TEMP */

#ifdef UI_UNDEFED
/********************************************************************
 *  Function handler: register_device (section mgmt)
 */
int
  ui_tmd_api_mgmt_register_device(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_base_address;
  SOC_SAND_RESET_DEVICE_FUNC_PTR
    prm_reset_device_ptr;
  uint32
    prm_unit_ptr;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_mgmt");
  soc_sand_proc_name = "tmd_register_device";

  unit = tmd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_REGISTER_DEVICE_REGISTER_DEVICE_BASE_ADDRESS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_REGISTER_DEVICE_REGISTER_DEVICE_BASE_ADDRESS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_base_address = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_REGISTER_DEVICE_REGISTER_DEVICE_RESET_DEVICE_PTR_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_REGISTER_DEVICE_REGISTER_DEVICE_RESET_DEVICE_PTR_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_reset_device_ptr = (SOC_SAND_RESET_DEVICE_FUNC_PTR)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_register_device(
          &prm_base_address,
          prm_reset_device_ptr,
          &prm_unit_ptr
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_register_device");
    goto exit;
  }

  soc_sand_os_printf( "unit_ptr: %d\n\r",prm_unit_ptr);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: unregister_device (section mgmt)
 */
int
  ui_tmd_api_mgmt_unregister_device(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_mgmt");
  soc_sand_proc_name = "tmd_unregister_device";

  unit = tmd_get_default_unit();

  /* Get parameters */

  /* Call function */
  ret = tmd_unregister_device(
          unit
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_unregister_device");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: operation_mode_set (section mgmt)
 */
int
  ui_tmd_api_mgmt_operation_mode_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_MGMT_OPERATION_MODE
    prm_op_mode;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_mgmt");
  soc_sand_proc_name = "tmd_mgmt_operation_mode_set";

  unit = tmd_get_default_unit();
  TMD_MGMT_OPERATION_MODE_clear(&prm_op_mode);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = tmd_mgmt_operation_mode_get(
          unit,
          &prm_op_mode
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_mgmt_operation_mode_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MGMT_OPERATION_MODE_SET_OPERATION_MODE_SET_OP_MODE_STAG_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MGMT_OPERATION_MODE_SET_OPERATION_MODE_SET_OP_MODE_STAG_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_op_mode.stag_enable = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MGMT_OPERATION_MODE_SET_OPERATION_MODE_SET_OP_MODE_IS_COMBO_NIF_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MGMT_OPERATION_MODE_SET_OPERATION_MODE_SET_OP_MODE_IS_COMBO_NIF_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_op_mode.is_combo_nif = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MGMT_OPERATION_MODE_SET_OPERATION_MODE_SET_OP_MODE_IS_FE200_FABRIC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MGMT_OPERATION_MODE_SET_OPERATION_MODE_SET_OP_MODE_IS_FE200_FABRIC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_op_mode.is_fe200_fabric = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MGMT_OPERATION_MODE_SET_OPERATION_MODE_SET_OP_MODE_IS_FAP20_IN_SYSTEM_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MGMT_OPERATION_MODE_SET_OPERATION_MODE_SET_OP_MODE_IS_FAP20_IN_SYSTEM_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_op_mode.is_fap20_in_system = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MGMT_OPERATION_MODE_SET_OPERATION_MODE_SET_OP_MODE_PP_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MGMT_OPERATION_MODE_SET_OPERATION_MODE_SET_OP_MODE_PP_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_op_mode.pp_enable = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MGMT_OPERATION_MODE_SET_OPERATION_MODE_SET_OP_MODE_DEVICE_SUB_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_MGMT_OPERATION_MODE_SET_OPERATION_MODE_SET_OP_MODE_DEVICE_SUB_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_op_mode.device_sub_type = param_val->numeric_equivalent;
  }


  /* Call function */
  ret = tmd_mgmt_operation_mode_set(
          unit,
          &prm_op_mode
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_mgmt_operation_mode_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: operation_mode_get (section mgmt)
 */
int
  ui_tmd_api_mgmt_operation_mode_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_MGMT_OPERATION_MODE
    prm_op_mode;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_mgmt");
  soc_sand_proc_name = "tmd_mgmt_operation_mode_get";

  unit = tmd_get_default_unit();
  TMD_MGMT_OPERATION_MODE_clear(&prm_op_mode);

  /* Get parameters */

  /* Call function */
  ret = tmd_mgmt_operation_mode_get(
          unit,
          &prm_op_mode
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_mgmt_operation_mode_get");
    goto exit;
  }

  TMD_MGMT_OPERATION_MODE_print(&prm_op_mode);


  goto exit;
exit:
  return ui_ret;
}
#endif

#ifdef UI_MGMT
/********************************************************************
 *  Function handler: credit_worth_set (section mgmt)
 */
int
  ui_tmd_api_mgmt_credit_worth_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_credit_worth;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_mgmt");
  soc_sand_proc_name = "tmd_mgmt_credit_worth_set";

  unit = tmd_get_default_unit();

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = tmd_mgmt_credit_worth_get(
          unit,
          &prm_credit_worth
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_mgmt_credit_worth_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MGMT_CREDIT_WORTH_SET_CREDIT_WORTH_SET_CREDIT_WORTH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MGMT_CREDIT_WORTH_SET_CREDIT_WORTH_SET_CREDIT_WORTH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_credit_worth = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_mgmt_credit_worth_set(
          unit,
          prm_credit_worth
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_mgmt_credit_worth_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: credit_worth_get (section mgmt)
 */
int
  ui_tmd_api_mgmt_credit_worth_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_credit_worth;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_mgmt");
  soc_sand_proc_name = "tmd_mgmt_credit_worth_get";

  unit = tmd_get_default_unit();

  /* Get parameters */

  /* Call function */
  ret = tmd_mgmt_credit_worth_get(
          unit,
          &prm_credit_worth
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_mgmt_credit_worth_get");
    goto exit;
  }

  soc_sand_os_printf( "credit_worth: %lu\n\r",prm_credit_worth);


  goto exit;
exit:
  return ui_ret;
}
#endif
#ifdef UI_UNDEFED

#endif

#ifdef UI_MGMT
/********************************************************************
 *  Function handler: all_ctrl_cells_enable_set (section mgmt)
 */
int
  ui_tmd_api_mgmt_all_ctrl_cells_enable_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint8
    prm_enable = 0;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_mgmt");
  soc_sand_proc_name = "tmd_mgmt_all_ctrl_cells_enable_set";

  unit = tmd_get_default_unit();

  /* Get parameters */

  /* This is a set function, so call GET function first */

  /*
  ret = tmd_mgmt_all_ctrl_cells_enable_get(
            unit,
            &prm_enable
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      soc_petra_disp_result(ret, "tmd_mgmt_all_ctrl_cells_enable_get");
      goto exit;
    } */


  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MGMT_ALL_CTRL_CELLS_ENABLE_SET_ALL_CTRL_CELLS_ENABLE_SET_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MGMT_ALL_CTRL_CELLS_ENABLE_SET_ALL_CTRL_CELLS_ENABLE_SET_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_enable = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_mgmt_all_ctrl_cells_enable_set(
          unit,
          prm_enable
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_mgmt_all_ctrl_cells_enable_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}
/********************************************************************
 *  Function handler: all_ctrl_cells_enable_get (section mgmt)
 */
int
  ui_tmd_api_mgmt_all_ctrl_cells_enable_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint8
    prm_enable;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_mgmt");
  soc_sand_proc_name = "tmd_mgmt_all_ctrl_cells_enable_get";

  unit = tmd_get_default_unit();

  /* Get parameters */

  /* Call function */
  ret = tmd_mgmt_all_ctrl_cells_enable_get(
          unit,
          &prm_enable
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_mgmt_all_ctrl_cells_enable_get");
    goto exit;
  }

  soc_sand_os_printf( "enable: %u\n\r",prm_enable);


  goto exit;
exit:
  return ui_ret;
}
/********************************************************************
 *  Function handler: enable_traffic_set (section mgmt)
 */
int
  ui_tmd_api_mgmt_enable_traffic_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint8
    prm_enable;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_mgmt");
  soc_sand_proc_name = "tmd_mgmt_enable_traffic_set";

  unit = tmd_get_default_unit();

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = tmd_mgmt_enable_traffic_get(
          unit,
          &prm_enable
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_mgmt_enable_traffic_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MGMT_ENABLE_TRAFFIC_SET_ENABLE_TRAFFIC_SET_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MGMT_ENABLE_TRAFFIC_SET_ENABLE_TRAFFIC_SET_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_enable = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_mgmt_enable_traffic_set(
          unit,
          prm_enable
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_mgmt_enable_traffic_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: enable_traffic_get (section mgmt)
 */
int
  ui_tmd_api_mgmt_enable_traffic_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint8
    prm_enable;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_mgmt");
  soc_sand_proc_name = "tmd_mgmt_enable_traffic_get";

  unit = tmd_get_default_unit();

  /* Get parameters */

  /* Call function */
  ret = tmd_mgmt_enable_traffic_get(
          unit,
          &prm_enable
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_mgmt_enable_traffic_get");
    goto exit;
  }

  soc_sand_os_printf( "enable: %u\n\r",prm_enable);


  goto exit;
exit:
  return ui_ret;
}
#ifdef UI_UNDEFED
/********************************************************************
 *  Function handler: pckt_size_range_set (section mgmt)
 */
int
  ui_tmd_api_mgmt_pckt_size_range_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_MGMT_PCKT_SIZE_CONF_MODE
    prm_conf_mode_ndx;
  TMD_MGMT_PCKT_SIZE
    prm_size_range;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_mgmt");
  soc_sand_proc_name = "tmd_mgmt_pckt_size_range_set";

  unit = tmd_get_default_unit();
  TMD_MGMT_PCKT_SIZE_clear(&prm_size_range);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MGMT_PCKT_SIZE_RANGE_SET_PCKT_SIZE_RANGE_SET_CONF_MODE_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_MGMT_PCKT_SIZE_RANGE_SET_PCKT_SIZE_RANGE_SET_CONF_MODE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_conf_mode_ndx = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter conf_mode_ndx after pckt_size_range_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = tmd_mgmt_pckt_size_range_get(
          unit,
          prm_conf_mode_ndx,
          &prm_size_range
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** tmd_mgmt_pckt_size_range_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "tmd_mgmt_pckt_size_range_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MGMT_PCKT_SIZE_RANGE_SET_PCKT_SIZE_RANGE_SET_SIZE_RANGE_MAX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MGMT_PCKT_SIZE_RANGE_SET_PCKT_SIZE_RANGE_SET_SIZE_RANGE_MAX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_size_range.max = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MGMT_PCKT_SIZE_RANGE_SET_PCKT_SIZE_RANGE_SET_SIZE_RANGE_MIN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MGMT_PCKT_SIZE_RANGE_SET_PCKT_SIZE_RANGE_SET_SIZE_RANGE_MIN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_size_range.min = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_mgmt_pckt_size_range_set(
          unit,
          prm_conf_mode_ndx,
          &prm_size_range
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** tmd_mgmt_pckt_size_range_set - FAIL", TRUE);
    soc_petra_disp_result(ret, "tmd_mgmt_pckt_size_range_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pckt_size_range_get (section mgmt)
 */
int
  ui_tmd_api_mgmt_pckt_size_range_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_MGMT_PCKT_SIZE_CONF_MODE
    prm_conf_mode_ndx;
  TMD_MGMT_PCKT_SIZE
    prm_size_range;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_mgmt");
  soc_sand_proc_name = "tmd_mgmt_pckt_size_range_get";

  unit = tmd_get_default_unit();
  TMD_MGMT_PCKT_SIZE_clear(&prm_size_range);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MGMT_PCKT_SIZE_RANGE_GET_PCKT_SIZE_RANGE_GET_CONF_MODE_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_MGMT_PCKT_SIZE_RANGE_GET_PCKT_SIZE_RANGE_GET_CONF_MODE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_conf_mode_ndx = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter conf_mode_ndx after pckt_size_range_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_mgmt_pckt_size_range_get(
          unit,
          prm_conf_mode_ndx,
          &prm_size_range
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** tmd_mgmt_pckt_size_range_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "tmd_mgmt_pckt_size_range_get");
    goto exit;
  }

  send_string_to_screen("--> size_range:", TRUE);
  TMD_MGMT_PCKT_SIZE_print(&prm_size_range);


  goto exit;
exit:
  return ui_ret;
}
#endif /* UI_UNDEFED */
/******************************************************************** 
 *  Function handler: core_frequency_get (section mgmt)
 */
int 
  ui_tmd_api_mgmt_core_frequency_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_clk_freq;
   
  UI_MACROS_INIT_FUNCTION("ui_tmd_api_mgmt"); 
  soc_sand_proc_name = "tmd_mgmt_core_frequency_get"; 
 
  unit = tmd_get_default_unit(); 
 
  /* Get parameters */ 

  /* Call function */
  ret = tmd_mgmt_core_frequency_get(
          unit,
          &prm_clk_freq
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** tmd_mgmt_core_frequency_get - FAIL", TRUE); 
    soc_petra_disp_result(ret, "tmd_mgmt_core_frequency_get");   
    goto exit; 
  } 

  soc_sand_os_printf( "clk_freq: %lu[MHz]\n\r",prm_clk_freq);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
#endif

#ifdef UI_PORTS
/********************************************************************
 *  Function handler: sys_phys_to_local_port_map_set (section ports)
 */
int
  ui_tmd_api_ports_sys_phys_to_local_port_map_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_sys_phys_port_ndx = 0;
  uint32
    prm_mapped_fap_id = 0;
  uint32
    prm_mapped_fap_port_id = 0;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ports");
  soc_sand_proc_name = "tmd_sys_phys_to_local_port_map_set";

  unit = tmd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SYS_PHYS_TO_LOCAL_PORT_MAP_SET_SYS_PHYS_TO_LOCAL_PORT_MAP_SET_SYS_PHYS_PORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SYS_PHYS_TO_LOCAL_PORT_MAP_SET_SYS_PHYS_TO_LOCAL_PORT_MAP_SET_SYS_PHYS_PORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sys_phys_port_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter sys_phys_port_ndx after sys_phys_to_local_port_map_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = tmd_sys_phys_to_local_port_map_get(
          unit,
          prm_sys_phys_port_ndx,
          &prm_mapped_fap_id,
          &prm_mapped_fap_port_id
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_sys_phys_to_local_port_map_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SYS_PHYS_TO_LOCAL_PORT_MAP_SET_SYS_PHYS_TO_LOCAL_PORT_MAP_SET_MAPPED_FAP_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SYS_PHYS_TO_LOCAL_PORT_MAP_SET_SYS_PHYS_TO_LOCAL_PORT_MAP_SET_MAPPED_FAP_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mapped_fap_id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SYS_PHYS_TO_LOCAL_PORT_MAP_SET_SYS_PHYS_TO_LOCAL_PORT_MAP_SET_MAPPED_FAP_PORT_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SYS_PHYS_TO_LOCAL_PORT_MAP_SET_SYS_PHYS_TO_LOCAL_PORT_MAP_SET_MAPPED_FAP_PORT_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mapped_fap_port_id = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_sys_phys_to_local_port_map_set(
          unit,
          prm_sys_phys_port_ndx,
          prm_mapped_fap_id,
          prm_mapped_fap_port_id
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_sys_phys_to_local_port_map_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: sys_phys_to_local_port_map_get (section ports)
 */
int
  ui_tmd_api_ports_sys_phys_to_local_port_map_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_sys_phys_port_ndx = 0;
  uint32
    prm_mapped_fap_id = 0;
  uint32
    prm_mapped_fap_port_id = 0;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ports");
  soc_sand_proc_name = "tmd_sys_phys_to_local_port_map_get";

  unit = tmd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SYS_PHYS_TO_LOCAL_PORT_MAP_GET_SYS_PHYS_TO_LOCAL_PORT_MAP_GET_SYS_PHYS_PORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SYS_PHYS_TO_LOCAL_PORT_MAP_GET_SYS_PHYS_TO_LOCAL_PORT_MAP_GET_SYS_PHYS_PORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sys_phys_port_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter sys_phys_port_ndx after sys_phys_to_local_port_map_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_sys_phys_to_local_port_map_get(
          unit,
          prm_sys_phys_port_ndx,
          &prm_mapped_fap_id,
          &prm_mapped_fap_port_id
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_sys_phys_to_local_port_map_get");
    goto exit;
  }

  soc_sand_os_printf( "mapped_fap_id: %d\n\r",prm_mapped_fap_id);

  soc_sand_os_printf( "mapped_fap_port_id: %d\n\r",prm_mapped_fap_port_id);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: local_to_sys_phys_port_map_get (section ports)
 */
int
  ui_tmd_api_ports_local_to_sys_phys_port_map_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_fap_ndx = 0;
  uint32
    prm_fap_local_port_ndx = 0;
  uint32
    prm_sys_phys_port_id = 0;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ports");
  soc_sand_proc_name = "tmd_local_to_sys_phys_port_map_get";

  unit = tmd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_LOCAL_TO_SYS_PHYS_PORT_MAP_GET_LOCAL_TO_SYS_PHYS_PORT_MAP_GET_FAP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_LOCAL_TO_SYS_PHYS_PORT_MAP_GET_LOCAL_TO_SYS_PHYS_PORT_MAP_GET_FAP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_fap_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter fap_ndx after local_to_sys_phys_port_map_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_LOCAL_TO_SYS_PHYS_PORT_MAP_GET_LOCAL_TO_SYS_PHYS_PORT_MAP_GET_FAP_LOCAL_PORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_LOCAL_TO_SYS_PHYS_PORT_MAP_GET_LOCAL_TO_SYS_PHYS_PORT_MAP_GET_FAP_LOCAL_PORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_fap_local_port_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter fap_local_port_ndx after local_to_sys_phys_port_map_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_local_to_sys_phys_port_map_get(
          unit,
          prm_fap_ndx,
          prm_fap_local_port_ndx,
          &prm_sys_phys_port_id
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_local_to_sys_phys_port_map_get");
    goto exit;
  }

  if (prm_sys_phys_port_id == SOC_PETRA_SYS_PHYS_PORT_INVALID)
  {
    soc_sand_os_printf( "sys_phys_port_id: Not mapped!\n\r");
  }
  else
  {
    soc_sand_os_printf( "sys_phys_port_id: %d\n\r",prm_sys_phys_port_id);
  }

  goto exit;

exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: port_to_interface_map_set (section ports)
 */
int
  ui_tmd_api_ports_port_to_interface_map_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_port_ndx = 0;
  TMD_PORT_DIRECTION
    prm_direction_ndx = TMD_PORT_DIRECTION_BOTH;
  TMD_PORT2IF_MAPPING_INFO
    prm_in_info,
    prm_out_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ports");
  soc_sand_proc_name = "tmd_port_to_interface_map_set";

  unit = tmd_get_default_unit();
  TMD_PORT2IF_MAPPING_INFO_clear(&prm_in_info);
  TMD_PORT2IF_MAPPING_INFO_clear(&prm_out_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORT_TO_INTERFACE_MAP_SET_PORT_TO_INTERFACE_MAP_SET_PORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORT_TO_INTERFACE_MAP_SET_PORT_TO_INTERFACE_MAP_SET_PORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter port_ndx after port_to_interface_map_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORT_TO_INTERFACE_MAP_SET_PORT_TO_INTERFACE_MAP_SET_DIRECTION_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_PORT_TO_INTERFACE_MAP_SET_PORT_TO_INTERFACE_MAP_SET_DIRECTION_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_direction_ndx = param_val->numeric_equivalent;
  }


  /* This is a set function, so call GET function first */
  ret = tmd_port_to_interface_map_get(
          unit,
          prm_port_ndx,
          &prm_in_info,
          &prm_out_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_port_to_interface_map_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORT_TO_INTERFACE_MAP_SET_PORT_TO_INTERFACE_MAP_SET_INFO_CHANNEL_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORT_TO_INTERFACE_MAP_SET_PORT_TO_INTERFACE_MAP_SET_INFO_CHANNEL_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_in_info.channel_id = (uint32)param_val->value.ulong_value;
    prm_out_info.channel_id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORT_TO_INTERFACE_MAP_SET_PORT_TO_INTERFACE_MAP_SET_INFO_IF_ID_ID,1))
  {
    UI_MACROS_GET_NUM_SYM_VAL(PARAM_TMD_PORT_TO_INTERFACE_MAP_SET_PORT_TO_INTERFACE_MAP_SET_INFO_IF_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    UI_MACROS_LOAD_LONG_VAL(prm_in_info.if_id);
  }


  /* Call function */
  ret = tmd_port_to_interface_map_set(
          unit,
          prm_port_ndx,
          prm_direction_ndx,
          &prm_in_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_port_to_interface_map_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: port_to_interface_map_get (section ports)
 */
int
  ui_tmd_api_ports_port_to_interface_map_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_port_ndx = 0;
  TMD_PORT2IF_MAPPING_INFO
    prm_in_info,
    prm_out_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ports");
  soc_sand_proc_name = "tmd_port_to_interface_map_get";

  unit = tmd_get_default_unit();
  TMD_PORT2IF_MAPPING_INFO_clear(&prm_in_info);
  TMD_PORT2IF_MAPPING_INFO_clear(&prm_out_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORT_TO_INTERFACE_MAP_GET_PORT_TO_INTERFACE_MAP_GET_PORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORT_TO_INTERFACE_MAP_GET_PORT_TO_INTERFACE_MAP_GET_PORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter port_ndx after port_to_interface_map_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_port_to_interface_map_get(
          unit,
          prm_port_ndx,
          &prm_in_info,
          &prm_out_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_port_to_interface_map_get");
    goto exit;
  }

  send_string_to_screen("--> Incoming: \n\r", TRUE);
  soc_sand_os_printf( "info: ");
  TMD_PORT2IF_MAPPING_INFO_print(&prm_in_info);
  soc_sand_os_printf( "\n\r");
  send_string_to_screen("--> Outgoing: \n\r", TRUE);
  soc_sand_os_printf( "info: ");
  TMD_PORT2IF_MAPPING_INFO_print(&prm_out_info);
  soc_sand_os_printf( "\n\r");


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: lag_set (section ports)
 */
int
  ui_tmd_api_ports_lag_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_PORT_DIRECTION
    prm_direction_ndx = TMD_PORT_DIRECTION_BOTH;
  uint32
    prm_lag_ndx = 0,
    indx;
  TMD_PORTS_LAG_INFO
    in_prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ports");
  soc_sand_proc_name = "tmd_ports_lag_set";

  unit = tmd_get_default_unit();
  TMD_PORTS_LAG_INFO_clear(&in_prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_LAG_SET_LAG_SET_DIRECTION_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_PORTS_LAG_SET_LAG_SET_DIRECTION_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_direction_ndx = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_LAG_SET_LAG_SET_LAG_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_LAG_SET_LAG_SET_LAG_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_lag_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter lag_ndx after lag_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_LAG_SET_LAG_SET_INFO_LAG_MEMBER_SYS_PORTS_ID,1))
  {
    in_prm_info.nof_entries = 0;
    for (indx = 0; indx < 80; ++in_prm_info.nof_entries, ++indx, ui_ret = 0)
    {
      UI_MACROS_GET_NUMMERIC_VAL_WITH_INDEX(PARAM_TMD_PORTS_LAG_SET_LAG_SET_INFO_LAG_MEMBER_SYS_PORTS_ID, indx + 1);
      if (ui_ret)
      {
        ui_ret = 0;
        break;
      }
      in_prm_info.lag_member_sys_ports[in_prm_info.nof_entries].sys_port = (uint32)param_val->value.ulong_value;
    }
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_LAG_SET_LAG_SET_INFO_LAG_MEMBER_SYS_PORTS_MEMBER_ID_ID,1))
  {
    in_prm_info.nof_entries = 0;
    for (indx = 0; indx < 80; ++in_prm_info.nof_entries, ++indx, ui_ret = 0)
    {
      UI_MACROS_GET_NUMMERIC_VAL_WITH_INDEX(PARAM_TMD_PORTS_LAG_SET_LAG_SET_INFO_LAG_MEMBER_SYS_PORTS_MEMBER_ID_ID, indx + 1);
      if (ui_ret)
      {
        ui_ret = 0;
        break;
      }
      in_prm_info.lag_member_sys_ports[in_prm_info.nof_entries].member_id = (uint32)param_val->value.ulong_value;
    }
  }

  /* Call function */
  ret = tmd_ports_lag_set(
          unit,
          prm_direction_ndx,
          prm_lag_ndx,
          &in_prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ports_lag_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: lag_get (section ports)
 */
int
  ui_tmd_api_ports_lag_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_lag_ndx = 0;
  TMD_PORTS_LAG_INFO
    in_prm_info,
    out_prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ports");
  soc_sand_proc_name = "tmd_ports_lag_get";

  unit = tmd_get_default_unit();
  TMD_PORTS_LAG_INFO_clear(&in_prm_info);
  TMD_PORTS_LAG_INFO_clear(&out_prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_LAG_GET_LAG_GET_LAG_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_LAG_GET_LAG_GET_LAG_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_lag_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter lag_ndx after lag_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_ports_lag_get(
          unit,
          prm_lag_ndx,
          &in_prm_info,
          &out_prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ports_lag_get");
    goto exit;
  }

  send_string_to_screen("--> Incoming: \n\r", TRUE);
  soc_sand_os_printf( "info: ");
  TMD_PORTS_LAG_INFO_print(&in_prm_info);
  soc_sand_os_printf( "\n\r");
  send_string_to_screen("--> Outgoing: \n\r", TRUE);
  soc_sand_os_printf( "info: ");
  TMD_PORTS_LAG_INFO_print(&out_prm_info);
  soc_sand_os_printf( "\n\r");


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: lag_add (section ports)
 */
int
  ui_tmd_api_ports_lag_add(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_PORT_DIRECTION
    prm_direction_ndx = TMD_CONNECTION_DIRECTION_BOTH;
  uint32
    prm_lag_ndx = 0,
    prm_sys_port = 0;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ports");
  soc_sand_proc_name = "tmd_ports_lag_add";

  unit = tmd_get_default_unit();
  prm_direction_ndx = TMD_PORT_DIRECTION_BOTH;

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_LAG_ADD_LAG_ADD_DIRECTION_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_PORTS_LAG_ADD_LAG_ADD_DIRECTION_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_direction_ndx = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_LAG_ADD_LAG_ADD_LAG_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_LAG_ADD_LAG_ADD_LAG_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_lag_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter lag_ndx after lag_add***", TRUE);
    goto exit;
  }


  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_LAG_ADD_LAG_ADD_INFO_LAG_MEMBER_SYS_PORTS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_LAG_ADD_LAG_ADD_INFO_LAG_MEMBER_SYS_PORTS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sys_port = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_ports_lag_sys_port_add(
          unit,
          prm_direction_ndx,
          prm_lag_ndx,
          prm_sys_port
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ports_lag_add");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: lag_member_add (section ports)
 */
int
  ui_tmd_api_ports_lag_member_add(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  TMD_PORT_DIRECTION
    prm_direction_ndx = TMD_CONNECTION_DIRECTION_BOTH;
  uint32
    prm_lag_ndx;
  TMD_PORTS_LAG_MEMBER
    prm_lag_member;
  uint8
    prm_success;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ports");
  soc_sand_proc_name = "tmd_ports_lag_member_add";

  unit = tmd_get_default_unit();
  prm_direction_ndx = TMD_PORT_DIRECTION_BOTH;
  TMD_PORTS_LAG_MEMBER_clear(&prm_lag_member);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_LAG_MEMBER_ADD_LAG_MEMBER_ADD_DIRECTION_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_PORTS_LAG_MEMBER_ADD_LAG_MEMBER_ADD_DIRECTION_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_direction_ndx = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_LAG_MEMBER_ADD_LAG_MEMBER_ADD_LAG_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_LAG_MEMBER_ADD_LAG_MEMBER_ADD_LAG_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_lag_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter lag_ndx after lag_member_add***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_LAG_MEMBER_ADD_LAG_MEMBER_ADD_LAG_MEMBER_MEMBER_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_LAG_MEMBER_ADD_LAG_MEMBER_ADD_LAG_MEMBER_MEMBER_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_lag_member.member_id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_LAG_MEMBER_ADD_LAG_MEMBER_ADD_LAG_MEMBER_SYS_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_LAG_MEMBER_ADD_LAG_MEMBER_ADD_LAG_MEMBER_SYS_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_lag_member.sys_port = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  ret = tmd_ports_lag_member_add(
          unit,
          prm_direction_ndx,
          prm_lag_ndx,
          &prm_lag_member,
          &prm_success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** tmd_ports_lag_member_add - FAIL", TRUE);
    soc_petra_disp_result(ret, "tmd_ports_lag_member_add");
    goto exit;
  }

  if (prm_success != TRUE)
  {
    send_string_to_screen("--> Adding failed (reached maximal members count):", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: lag_remove (section ports)
 */
int
  ui_tmd_api_ports_lag_remove(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_PORT_DIRECTION
    prm_direction_ndx = TMD_CONNECTION_DIRECTION_BOTH;
  uint32
    prm_lag_ndx = 0,
    prm_sys_port = 0;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ports");
  soc_sand_proc_name = "tmd_ports_lag_remove";

  unit = tmd_get_default_unit();
  prm_direction_ndx = TMD_PORT_DIRECTION_BOTH;

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_LAG_REMOVE_LAG_REMOVE_DIRECTION_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_PORTS_LAG_REMOVE_LAG_REMOVE_DIRECTION_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_direction_ndx = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_LAG_REMOVE_LAG_REMOVE_LAG_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_LAG_REMOVE_LAG_REMOVE_LAG_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_lag_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter lag_ndx after lag_remove***", TRUE);
    goto exit;
  }


  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_LAG_REMOVE_LAG_REMOVE_INFO_LAG_MEMBER_SYS_PORTS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_LAG_REMOVE_LAG_REMOVE_INFO_LAG_MEMBER_SYS_PORTS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sys_port = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  ret = tmd_ports_lag_sys_port_remove(
          unit,
          prm_direction_ndx,
          prm_lag_ndx,
          prm_sys_port
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ports_lag_remove");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

#ifdef UI_UNDEFED_TEMP
/********************************************************************
 *  Function handler: lag_sys_port_info_get (section ports)
 */
int
  ui_tmd_api_ports_lag_sys_port_info_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_PORT_LAG_SYS_PORT_INFO
    in_port_lag_info;
  uint32
    prm_sys_port = 0;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ports");
  soc_sand_proc_name = "tmd_ports_lag_sys_port_info_get";

  unit = tmd_get_default_unit();

  TMD_PORT_LAG_SYS_PORT_INFO_clear(&in_port_lag_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_LAG_SYS_PORT_INFO_GET_LAG_SYS_PORT_INFO_GET_INFO_LAG_MEMBER_SYS_PORTS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_LAG_SYS_PORT_INFO_GET_LAG_SYS_PORT_INFO_GET_INFO_LAG_MEMBER_SYS_PORTS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sys_port = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_ports_lag_sys_port_info_get(
          unit,
          prm_sys_port,
          &in_port_lag_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ports_lag_sys_port_info_get");
    goto exit;
  }

  send_string_to_screen("port_lag_info:", TRUE);
  TMD_PORT_LAG_SYS_PORT_INFO_print(&in_port_lag_info);
  soc_sand_os_printf( "\n\r");

  goto exit;
exit:
  return ui_ret;
}
#endif /* UI_UNDEFED_TEMP */

/********************************************************************
 *  Function handler: port_header_type_set (section ports)
 */
int
  ui_tmd_api_ports_port_header_type_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_port_ndx = 0;
  TMD_PORT_DIRECTION
    prm_direction_ndx = TMD_PORT_DIRECTION_BOTH;
  TMD_PORT_HEADER_TYPE
    in_prm_header_type,
    out_prm_header_type;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ports");
  soc_sand_proc_name = "tmd_port_header_type_set";

  unit = tmd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORT_HEADER_TYPE_SET_PORT_HEADER_TYPE_SET_PORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORT_HEADER_TYPE_SET_PORT_HEADER_TYPE_SET_PORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter port_ndx after port_header_type_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORT_HEADER_TYPE_SET_PORT_HEADER_TYPE_SET_DIRECTION_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_PORT_HEADER_TYPE_SET_PORT_HEADER_TYPE_SET_DIRECTION_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_direction_ndx = param_val->numeric_equivalent;
  }

  /* This is a set function, so call GET function first */
  ret = tmd_port_header_type_get(
          unit,
          prm_port_ndx,
          &in_prm_header_type,
          &out_prm_header_type
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_port_header_type_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORT_HEADER_TYPE_SET_PORT_HEADER_TYPE_SET_HEADER_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_PORT_HEADER_TYPE_SET_PORT_HEADER_TYPE_SET_HEADER_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    in_prm_header_type = param_val->numeric_equivalent;
    out_prm_header_type = param_val->numeric_equivalent;
  }


  /* Call function */
  ret = tmd_port_header_type_set(
          unit,
          prm_port_ndx,
          prm_direction_ndx,
          in_prm_header_type
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_port_header_type_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: port_header_type_get (section ports)
 */
int
  ui_tmd_api_ports_port_header_type_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_port_ndx = 0;
  TMD_PORT_HEADER_TYPE
    in_prm_header_type,
    out_prm_header_type;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ports");
  soc_sand_proc_name = "tmd_port_header_type_get";

  unit = tmd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORT_HEADER_TYPE_GET_PORT_HEADER_TYPE_GET_PORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORT_HEADER_TYPE_GET_PORT_HEADER_TYPE_GET_PORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter port_ndx after port_header_type_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_port_header_type_get(
          unit,
          prm_port_ndx,
          &in_prm_header_type,
          &out_prm_header_type
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_port_header_type_get");
    goto exit;
  }

  send_string_to_screen("--> Incoming: \n\r", TRUE);
  soc_sand_os_printf( "header_type: %s\n\r",TMD_PORT_HEADER_TYPE_to_string(in_prm_header_type));
  send_string_to_screen("--> Outgoing: \n\r", TRUE);
  soc_sand_os_printf( "header_type: %s\n\r",TMD_PORT_HEADER_TYPE_to_string(out_prm_header_type));


  goto exit;
exit:
  return ui_ret;
}

#ifdef UI_UNDEFED_TEMP
/********************************************************************
 *  Function handler: mirror_inbound_set (section ports)
 */
int
  ui_tmd_api_ports_mirror_inbound_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_ifp_ndx = 0;
  TMD_PORT_INBOUND_MIRROR_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ports");
  soc_sand_proc_name = "tmd_ports_mirror_inbound_set";

  unit = tmd_get_default_unit();
  TMD_PORT_INBOUND_MIRROR_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_MIRROR_INBOUND_SET_MIRROR_INBOUND_SET_IFP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_MIRROR_INBOUND_SET_MIRROR_INBOUND_SET_IFP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ifp_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter ifp_ndx after mirror_inbound_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = tmd_ports_mirror_inbound_get(
          unit,
          prm_ifp_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ports_mirror_inbound_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_MIRROR_INBOUND_SET_MIRROR_INBOUND_SET_INFO_TC_OVERRIDE_OVERRIDE_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_MIRROR_INBOUND_SET_MIRROR_INBOUND_SET_INFO_TC_OVERRIDE_OVERRIDE_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.tc_override.override_val = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_MIRROR_INBOUND_SET_MIRROR_INBOUND_SET_INFO_TC_OVERRIDE_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_MIRROR_INBOUND_SET_MIRROR_INBOUND_SET_INFO_TC_OVERRIDE_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.tc_override.enable = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_MIRROR_INBOUND_SET_MIRROR_INBOUND_SET_INFO_DP_OVERRIDE_OVERRIDE_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_MIRROR_INBOUND_SET_MIRROR_INBOUND_SET_INFO_DP_OVERRIDE_OVERRIDE_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.dp_override.override_val = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_MIRROR_INBOUND_SET_MIRROR_INBOUND_SET_INFO_DP_OVERRIDE_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_MIRROR_INBOUND_SET_MIRROR_INBOUND_SET_INFO_DP_OVERRIDE_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.dp_override.enable = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_MIRROR_INBOUND_SET_MIRROR_INBOUND_SET_INFO_DESTINATION_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_MIRROR_INBOUND_SET_MIRROR_INBOUND_SET_INFO_DESTINATION_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.destination.id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_MIRROR_INBOUND_SET_MIRROR_INBOUND_SET_INFO_DESTINATION_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_PORTS_MIRROR_INBOUND_SET_MIRROR_INBOUND_SET_INFO_DESTINATION_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.destination.type = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_MIRROR_INBOUND_SET_MIRROR_INBOUND_SET_INFO_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_MIRROR_INBOUND_SET_MIRROR_INBOUND_SET_INFO_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.enable = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_ports_mirror_inbound_set(
          unit,
          prm_ifp_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ports_mirror_inbound_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: mirror_inbound_get (section ports)
 */
int
  ui_tmd_api_ports_mirror_inbound_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_ifp_ndx = 0;
  TMD_PORT_INBOUND_MIRROR_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ports");
  soc_sand_proc_name = "tmd_ports_mirror_inbound_get";

  unit = tmd_get_default_unit();
  TMD_PORT_INBOUND_MIRROR_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_MIRROR_INBOUND_GET_MIRROR_INBOUND_GET_IFP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_MIRROR_INBOUND_GET_MIRROR_INBOUND_GET_IFP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ifp_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter ifp_ndx after mirror_inbound_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_ports_mirror_inbound_get(
          unit,
          prm_ifp_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ports_mirror_inbound_get");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  TMD_PORT_INBOUND_MIRROR_INFO_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}
#endif /* UI_UNDEFED_TEMP */

/********************************************************************
 *  Function handler: mirror_outbound_set (section ports)
 */
int
  ui_tmd_api_ports_mirror_outbound_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_ofp_ndx = 0;
  TMD_PORT_OUTBOUND_MIRROR_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ports");
  soc_sand_proc_name = "tmd_ports_mirror_outbound_set";

  unit = tmd_get_default_unit();
  TMD_PORT_OUTBOUND_MIRROR_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_MIRROR_OUTBOUND_SET_MIRROR_OUTBOUND_SET_OFP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_MIRROR_OUTBOUND_SET_MIRROR_OUTBOUND_SET_OFP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ofp_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter ofp_ndx after mirror_outbound_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = tmd_ports_mirror_outbound_get(
          unit,
          prm_ofp_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ports_mirror_outbound_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_MIRROR_OUTBOUND_SET_MIRROR_OUTBOUND_SET_INFO_IFP_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_MIRROR_OUTBOUND_SET_MIRROR_OUTBOUND_SET_INFO_IFP_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.ifp_id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_MIRROR_OUTBOUND_SET_MIRROR_OUTBOUND_SET_INFO_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_MIRROR_OUTBOUND_SET_MIRROR_OUTBOUND_SET_INFO_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.enable = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_ports_mirror_outbound_set(
          unit,
          prm_ofp_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ports_mirror_outbound_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: mirror_outbound_get (section ports)
 */
int
  ui_tmd_api_ports_mirror_outbound_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_ofp_ndx = 0;
  TMD_PORT_OUTBOUND_MIRROR_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ports");
  soc_sand_proc_name = "tmd_ports_mirror_outbound_get";

  unit = tmd_get_default_unit();
  TMD_PORT_OUTBOUND_MIRROR_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_MIRROR_OUTBOUND_GET_MIRROR_OUTBOUND_GET_OFP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_MIRROR_OUTBOUND_GET_MIRROR_OUTBOUND_GET_OFP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ofp_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter ofp_ndx after mirror_outbound_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_ports_mirror_outbound_get(
          unit,
          prm_ofp_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ports_mirror_outbound_get");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  TMD_PORT_OUTBOUND_MIRROR_INFO_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

#ifdef UI_UNDEFED_TEMP
/********************************************************************
 *  Function handler: snoop_set (section ports)
 */
int
  ui_tmd_api_ports_snoop_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_snoop_cmd_ndx = 0;
  TMD_PORT_SNOOP_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ports");
  soc_sand_proc_name = "tmd_ports_snoop_set";

  unit = tmd_get_default_unit();
  TMD_PORT_SNOOP_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_SNOOP_SET_SNOOP_SET_SNOOP_CMD_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_SNOOP_SET_SNOOP_SET_SNOOP_CMD_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_snoop_cmd_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter snoop_cmd_ndx after snoop_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = tmd_ports_snoop_get(
          unit,
          prm_snoop_cmd_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ports_snoop_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_SNOOP_SET_SNOOP_SET_INFO_TC_OVERRIDE_OVERRIDE_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_SNOOP_SET_SNOOP_SET_INFO_TC_OVERRIDE_OVERRIDE_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.tc_override.override_val = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_SNOOP_SET_SNOOP_SET_INFO_TC_OVERRIDE_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_SNOOP_SET_SNOOP_SET_INFO_TC_OVERRIDE_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.tc_override.enable = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_SNOOP_SET_SNOOP_SET_INFO_DP_OVERRIDE_OVERRIDE_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_SNOOP_SET_SNOOP_SET_INFO_DP_OVERRIDE_OVERRIDE_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.dp_override.override_val = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_SNOOP_SET_SNOOP_SET_INFO_DP_OVERRIDE_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_SNOOP_SET_SNOOP_SET_INFO_DP_OVERRIDE_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.dp_override.enable = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_SNOOP_SET_SNOOP_SET_INFO_DESTINATION_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_SNOOP_SET_SNOOP_SET_INFO_DESTINATION_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.destination.id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_SNOOP_SET_SNOOP_SET_INFO_DESTINATION_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_PORTS_SNOOP_SET_SNOOP_SET_INFO_DESTINATION_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.destination.type = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_SNOOP_SET_SNOOP_SET_INFO_SIZE_BYTES_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_PORTS_SNOOP_SET_SNOOP_SET_INFO_SIZE_BYTES_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.size_bytes = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_SNOOP_SET_SNOOP_SET_INFO_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_SNOOP_SET_SNOOP_SET_INFO_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.enable = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_ports_snoop_set(
          unit,
          prm_snoop_cmd_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ports_snoop_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: snoop_get (section ports)
 */
int
  ui_tmd_api_ports_snoop_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_snoop_cmd_ndx = 0;
  TMD_PORT_SNOOP_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ports");
  soc_sand_proc_name = "tmd_ports_snoop_get";

  unit = tmd_get_default_unit();
  TMD_PORT_SNOOP_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_SNOOP_GET_SNOOP_GET_SNOOP_CMD_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_SNOOP_GET_SNOOP_GET_SNOOP_CMD_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_snoop_cmd_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter snoop_cmd_ndx after snoop_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_ports_snoop_get(
          unit,
          prm_snoop_cmd_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ports_snoop_get");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  TMD_PORT_SNOOP_INFO_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}
#endif /* UI_UNDEFED_TEMP */

/********************************************************************
 *  Function handler: itmh_extension_set (section ports)
 */
int
  ui_tmd_api_ports_itmh_extension_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_FAP_PORT_ID
    prm_port_ndx = 0;
  uint8
    prm_ext_en;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ports");
  soc_sand_proc_name = "tmd_ports_itmh_extension_set";

  unit = tmd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_ITMH_EXTENSION_SET_ITMH_EXTENSION_SET_PORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_ITMH_EXTENSION_SET_ITMH_EXTENSION_SET_PORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter port_ndx after itmh_extension_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = tmd_ports_itmh_extension_get(
          unit,
          prm_port_ndx,
          &prm_ext_en
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ports_itmh_extension_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_ITMH_EXTENSION_SET_ITMH_EXTENSION_SET_EXT_EN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_ITMH_EXTENSION_SET_ITMH_EXTENSION_SET_EXT_EN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ext_en = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_ports_itmh_extension_set(
          unit,
          prm_port_ndx,
          prm_ext_en
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ports_itmh_extension_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: itmh_extension_get (section ports)
 */
int
  ui_tmd_api_ports_itmh_extension_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_FAP_PORT_ID
    prm_port_ndx = 0;
  uint8
    prm_ext_en;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ports");
  soc_sand_proc_name = "tmd_ports_itmh_extension_get";

  unit = tmd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_ITMH_EXTENSION_GET_ITMH_EXTENSION_GET_PORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_ITMH_EXTENSION_GET_ITMH_EXTENSION_GET_PORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter port_ndx after itmh_extension_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_ports_itmh_extension_get(
          unit,
          prm_port_ndx,
          &prm_ext_en
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ports_itmh_extension_get");
    goto exit;
  }

  soc_sand_os_printf( "ext_en: %u\n\r",prm_ext_en);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: shaping_header_set (section ports)
 */
int
  ui_tmd_api_ports_shaping_header_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_FAP_PORT_ID
    prm_port_ndx = 0;
  TMD_PORTS_ISP_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ports");
  soc_sand_proc_name = "tmd_ports_shaping_header_set";

  unit = tmd_get_default_unit();
  TMD_PORTS_ISP_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_SHAPING_HEADER_SET_SHAPING_HEADER_SET_PORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_SHAPING_HEADER_SET_SHAPING_HEADER_SET_PORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter port_ndx after shaping_header_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = tmd_ports_shaping_header_get(
          unit,
          prm_port_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ports_shaping_header_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_SHAPING_HEADER_SET_SHAPING_HEADER_SET_INFO_QUEUE_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_SHAPING_HEADER_SET_SHAPING_HEADER_SET_INFO_QUEUE_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.queue_id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_SHAPING_HEADER_SET_SHAPING_HEADER_SET_INFO_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_SHAPING_HEADER_SET_SHAPING_HEADER_SET_INFO_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.enable = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_ports_shaping_header_set(
          unit,
          prm_port_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ports_shaping_header_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: shaping_header_get (section ports)
 */
int
  ui_tmd_api_ports_shaping_header_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_FAP_PORT_ID
    prm_port_ndx = 0;
  TMD_PORTS_ISP_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ports");
  soc_sand_proc_name = "tmd_ports_shaping_header_get";

  unit = tmd_get_default_unit();
  TMD_PORTS_ISP_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_SHAPING_HEADER_GET_SHAPING_HEADER_GET_PORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_SHAPING_HEADER_GET_SHAPING_HEADER_GET_PORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter port_ndx after shaping_header_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_ports_shaping_header_get(
          unit,
          prm_port_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ports_shaping_header_get");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  TMD_PORTS_ISP_INFO_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: forwarding_header_set (section ports)
 */
int
  ui_tmd_api_ports_forwarding_header_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_FAP_PORT_ID
    prm_port_ndx = 0;
  TMD_PORTS_ITMH
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ports");
  soc_sand_proc_name = "tmd_ports_forwarding_header_set";

  unit = tmd_get_default_unit();
  TMD_PORTS_ITMH_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_FORWARDING_HEADER_SET_FORWARDING_HEADER_SET_PORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_FORWARDING_HEADER_SET_FORWARDING_HEADER_SET_PORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter port_ndx after forwarding_header_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = tmd_ports_forwarding_header_get(
          unit,
          prm_port_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ports_forwarding_header_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_FORWARDING_HEADER_SET_FORWARDING_HEADER_SET_INFO_EXTENSION_SRC_PORT_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_FORWARDING_HEADER_SET_FORWARDING_HEADER_SET_INFO_EXTENSION_SRC_PORT_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.extension.src_port.id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_FORWARDING_HEADER_SET_FORWARDING_HEADER_SET_INFO_EXTENSION_SRC_PORT_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_PORTS_FORWARDING_HEADER_SET_FORWARDING_HEADER_SET_INFO_EXTENSION_SRC_PORT_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.extension.src_port.type = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_FORWARDING_HEADER_SET_FORWARDING_HEADER_SET_INFO_EXTENSION_IS_SRC_SYS_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_FORWARDING_HEADER_SET_FORWARDING_HEADER_SET_INFO_EXTENSION_IS_SRC_SYS_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.extension.is_src_sys_port = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_FORWARDING_HEADER_SET_FORWARDING_HEADER_SET_INFO_EXTENSION_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_FORWARDING_HEADER_SET_FORWARDING_HEADER_SET_INFO_EXTENSION_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.extension.enable = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_FORWARDING_HEADER_SET_FORWARDING_HEADER_SET_INFO_BASE_DESTINATION_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_FORWARDING_HEADER_SET_FORWARDING_HEADER_SET_INFO_BASE_DESTINATION_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.base.destination.id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_FORWARDING_HEADER_SET_FORWARDING_HEADER_SET_INFO_BASE_DESTINATION_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_PORTS_FORWARDING_HEADER_SET_FORWARDING_HEADER_SET_INFO_BASE_DESTINATION_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.base.destination.type = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_FORWARDING_HEADER_SET_FORWARDING_HEADER_SET_INFO_BASE_DP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_FORWARDING_HEADER_SET_FORWARDING_HEADER_SET_INFO_BASE_DP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.base.dp = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_FORWARDING_HEADER_SET_FORWARDING_HEADER_SET_INFO_BASE_TR_CLS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_FORWARDING_HEADER_SET_FORWARDING_HEADER_SET_INFO_BASE_TR_CLS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.base.tr_cls = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_FORWARDING_HEADER_SET_FORWARDING_HEADER_SET_INFO_BASE_EXCLUDE_SRC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_FORWARDING_HEADER_SET_FORWARDING_HEADER_SET_INFO_BASE_EXCLUDE_SRC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.base.exclude_src = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_FORWARDING_HEADER_SET_FORWARDING_HEADER_SET_INFO_BASE_SNOOP_CMD_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_FORWARDING_HEADER_SET_FORWARDING_HEADER_SET_INFO_BASE_SNOOP_CMD_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.base.snoop_cmd_ndx = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_FORWARDING_HEADER_SET_FORWARDING_HEADER_SET_INFO_BASE_OUT_MIRROR_EN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_FORWARDING_HEADER_SET_FORWARDING_HEADER_SET_INFO_BASE_OUT_MIRROR_EN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.base.out_mirror_dis = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_FORWARDING_HEADER_SET_FORWARDING_HEADER_SET_INFO_BASE_PP_HEADER_PRESENT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_FORWARDING_HEADER_SET_FORWARDING_HEADER_SET_INFO_BASE_PP_HEADER_PRESENT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.base.pp_header_present = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_ports_forwarding_header_set(
          unit,
          prm_port_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ports_forwarding_header_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: forwarding_header_get (section ports)
 */
int
  ui_tmd_api_ports_forwarding_header_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_FAP_PORT_ID
    prm_port_ndx = 0;
  TMD_PORTS_ITMH
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ports");
  soc_sand_proc_name = "tmd_ports_forwarding_header_get";

  unit = tmd_get_default_unit();
  TMD_PORTS_ITMH_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_FORWARDING_HEADER_GET_FORWARDING_HEADER_GET_PORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_FORWARDING_HEADER_GET_FORWARDING_HEADER_GET_PORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter port_ndx after forwarding_header_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_ports_forwarding_header_get(
          unit,
          prm_port_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ports_forwarding_header_get");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  TMD_PORTS_ITMH_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: stat_tag_field_set (section ports)
 */
int
  ui_tmd_api_ports_stat_tag_field_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_FAP_PORT_ID
    prm_port_ndx = 0;
  TMD_PORTS_STAG_FIELDS
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ports");
  soc_sand_proc_name = "tmd_ports_stag_set";

  unit = tmd_get_default_unit();
  TMD_PORTS_STAG_FIELDS_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_STAG_FIELDS_SET_STAT_TAG_FIELD_SET_PORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_STAG_FIELDS_SET_STAT_TAG_FIELD_SET_PORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter port_ndx after stat_tag_field_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = tmd_ports_stag_get(
          unit,
          prm_port_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ports_stag_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_STAG_FIELDS_SET_STAT_TAG_FIELD_SET_INFO_DATA_TYPE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_STAG_FIELDS_SET_STAT_TAG_FIELD_SET_INFO_DATA_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.data_type = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_STAG_FIELDS_SET_STAT_TAG_FIELD_SET_INFO_DP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_STAG_FIELDS_SET_STAT_TAG_FIELD_SET_INFO_DP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.dp = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_STAG_FIELDS_SET_STAT_TAG_FIELD_SET_INFO_TR_CLS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_STAG_FIELDS_SET_STAT_TAG_FIELD_SET_INFO_TR_CLS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.tr_cls = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_STAG_FIELDS_SET_STAT_TAG_FIELD_SET_INFO_IFP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_STAG_FIELDS_SET_STAT_TAG_FIELD_SET_INFO_IFP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.ifp = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_STAG_FIELDS_SET_STAT_TAG_FIELD_SET_INFO_CID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_STAG_FIELDS_SET_STAT_TAG_FIELD_SET_INFO_CID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.cid = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_ports_stag_set(
          unit,
          prm_port_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ports_stag_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: stat_tag_field_get (section ports)
 */
int
  ui_tmd_api_ports_stat_tag_field_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_FAP_PORT_ID
    prm_port_ndx = 0;
  TMD_PORTS_STAG_FIELDS
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ports");
  soc_sand_proc_name = "tmd_ports_stag_get";

  unit = tmd_get_default_unit();
  TMD_PORTS_STAG_FIELDS_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_STAG_FIELDS_GET_STAT_TAG_FIELD_GET_PORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_STAG_FIELDS_GET_STAT_TAG_FIELD_GET_PORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter port_ndx after stat_tag_field_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_ports_stag_get(
          unit,
          prm_port_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ports_stag_get");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  TMD_PORTS_STAG_FIELDS_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ftmh_extension_set (section ports)
 */
int
  ui_tmd_api_ports_ftmh_extension_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_PORTS_FTMH_EXT_OUTLIF
    prm_ext_option;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ports");
  soc_sand_proc_name = "tmd_ports_ftmh_extension_set";

  unit = tmd_get_default_unit();

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = tmd_ports_ftmh_extension_get(
          unit,
          &prm_ext_option
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ports_ftmh_extension_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_FTMH_EXTENSION_SET_FTMH_EXTENSION_SET_EXT_OPTION_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_PORTS_FTMH_EXTENSION_SET_FTMH_EXTENSION_SET_EXT_OPTION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ext_option = param_val->numeric_equivalent;
  }


  /* Call function */
  ret = tmd_ports_ftmh_extension_set(
          unit,
          prm_ext_option
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ports_ftmh_extension_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ftmh_extension_get (section ports)
 */
int
  ui_tmd_api_ports_ftmh_extension_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_PORTS_FTMH_EXT_OUTLIF
    prm_ext_option;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ports");
  soc_sand_proc_name = "tmd_ports_ftmh_extension_get";

  unit = tmd_get_default_unit();

  /* Get parameters */

  /* Call function */
  ret = tmd_ports_ftmh_extension_get(
          unit,
          &prm_ext_option
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ports_ftmh_extension_get");
    goto exit;
  }

  soc_sand_os_printf( "ext_option: %s\n\r",TMD_PORTS_FTMH_EXT_OUTLIF_to_string(prm_ext_option));


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: otmh_extension_set (section ports)
 */
int
  ui_tmd_api_ports_otmh_extension_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_FAP_PORT_ID
    prm_port_ndx = 0;
  TMD_PORTS_OTMH_EXTENSIONS_EN
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ports");
  soc_sand_proc_name = "tmd_ports_otmh_extension_set";

  unit = tmd_get_default_unit();
  TMD_PORTS_OTMH_EXTENSIONS_EN_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_OTMH_EXTENSION_SET_OTMH_EXTENSION_SET_PORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_OTMH_EXTENSION_SET_OTMH_EXTENSION_SET_PORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter port_ndx after otmh_extension_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = tmd_ports_otmh_extension_get(
          unit,
          prm_port_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ports_otmh_extension_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_OTMH_EXTENSION_SET_OTMH_EXTENSION_SET_INFO_DEST_EXT_EN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_OTMH_EXTENSION_SET_OTMH_EXTENSION_SET_INFO_DEST_EXT_EN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.dest_ext_en = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_OTMH_EXTENSION_SET_OTMH_EXTENSION_SET_INFO_SRC_EXT_EN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_OTMH_EXTENSION_SET_OTMH_EXTENSION_SET_INFO_SRC_EXT_EN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.src_ext_en = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_OTMH_EXTENSION_SET_OTMH_EXTENSION_SET_INFO_OUTLIF_EXT_EN_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_PORTS_OTMH_EXTENSION_SET_OTMH_EXTENSION_SET_INFO_OUTLIF_EXT_EN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.outlif_ext_en = param_val->numeric_equivalent;
  }


  /* Call function */
  ret = tmd_ports_otmh_extension_set(
          unit,
          prm_port_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ports_otmh_extension_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: otmh_extension_get (section ports)
 */
int
  ui_tmd_api_ports_otmh_extension_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_FAP_PORT_ID
    prm_port_ndx = 0;
  TMD_PORTS_OTMH_EXTENSIONS_EN
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ports");
  soc_sand_proc_name = "tmd_ports_otmh_extension_get";

  unit = tmd_get_default_unit();
  TMD_PORTS_OTMH_EXTENSIONS_EN_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_OTMH_EXTENSION_GET_OTMH_EXTENSION_GET_PORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORTS_OTMH_EXTENSION_GET_OTMH_EXTENSION_GET_PORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter port_ndx after otmh_extension_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_ports_otmh_extension_get(
          unit,
          prm_port_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ports_otmh_extension_get");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  TMD_PORTS_OTMH_EXTENSIONS_EN_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: port_egr_hdr_credit_discount_type_set (section ports)
 */
int
  ui_tmd_api_ports_port_egr_hdr_credit_discount_type_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  TMD_PORT_HEADER_TYPE
    prm_port_hdr_type_ndx;
  TMD_PORT_EGR_HDR_CR_DISCOUNT_TYPE
    prm_cr_discnt_type_ndx;
  TMD_PORT_EGR_HDR_CR_DISCOUNT_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ports");
  soc_sand_proc_name = "tmd_port_egr_hdr_credit_discount_type_set";

  unit = tmd_get_default_unit();
  TMD_PORT_EGR_HDR_CR_DISCOUNT_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORT_EGR_HDR_CREDIT_DISCOUNT_TYPE_SET_PORT_EGR_HDR_CREDIT_DISCOUNT_TYPE_SET_PORT_HDR_TYPE_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_PORT_EGR_HDR_CREDIT_DISCOUNT_TYPE_SET_PORT_EGR_HDR_CREDIT_DISCOUNT_TYPE_SET_PORT_HDR_TYPE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_hdr_type_ndx = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter port_hdr_type_ndx after port_egr_hdr_credit_discount_type_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORT_EGR_HDR_CREDIT_DISCOUNT_TYPE_SET_PORT_EGR_HDR_CREDIT_DISCOUNT_TYPE_SET_CR_DISCNT_TYPE_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_PORT_EGR_HDR_CREDIT_DISCOUNT_TYPE_SET_PORT_EGR_HDR_CREDIT_DISCOUNT_TYPE_SET_CR_DISCNT_TYPE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_cr_discnt_type_ndx = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter cr_discnt_type_ndx after port_egr_hdr_credit_discount_type_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = tmd_port_egr_hdr_credit_discount_type_get(
          unit,
          prm_port_hdr_type_ndx,
          prm_cr_discnt_type_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** tmd_port_egr_hdr_credit_discount_type_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "tmd_port_egr_hdr_credit_discount_type_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORT_EGR_HDR_CREDIT_DISCOUNT_TYPE_SET_PORT_EGR_HDR_CREDIT_DISCOUNT_TYPE_SET_INFO_MC_CREDIT_DISCOUNT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORT_EGR_HDR_CREDIT_DISCOUNT_TYPE_SET_PORT_EGR_HDR_CREDIT_DISCOUNT_TYPE_SET_INFO_MC_CREDIT_DISCOUNT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.mc_credit_discount = (int32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORT_EGR_HDR_CREDIT_DISCOUNT_TYPE_SET_PORT_EGR_HDR_CREDIT_DISCOUNT_TYPE_SET_INFO_UC_CREDIT_DISCOUNT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORT_EGR_HDR_CREDIT_DISCOUNT_TYPE_SET_PORT_EGR_HDR_CREDIT_DISCOUNT_TYPE_SET_INFO_UC_CREDIT_DISCOUNT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.uc_credit_discount = (int32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_port_egr_hdr_credit_discount_type_set(
          unit,
          prm_port_hdr_type_ndx,
          prm_cr_discnt_type_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** tmd_port_egr_hdr_credit_discount_type_set - FAIL", TRUE);
    soc_petra_disp_result(ret, "tmd_port_egr_hdr_credit_discount_type_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: port_egr_hdr_credit_discount_type_get (section ports)
 */
int
  ui_tmd_api_ports_port_egr_hdr_credit_discount_type_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  TMD_PORT_HEADER_TYPE
    prm_port_hdr_type_ndx;
  TMD_PORT_EGR_HDR_CR_DISCOUNT_TYPE
    prm_cr_discnt_type_ndx;
  TMD_PORT_EGR_HDR_CR_DISCOUNT_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ports");
  soc_sand_proc_name = "tmd_port_egr_hdr_credit_discount_type_get";

  unit = tmd_get_default_unit();
  TMD_PORT_EGR_HDR_CR_DISCOUNT_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORT_EGR_HDR_CREDIT_DISCOUNT_TYPE_GET_PORT_EGR_HDR_CREDIT_DISCOUNT_TYPE_GET_PORT_HDR_TYPE_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_PORT_EGR_HDR_CREDIT_DISCOUNT_TYPE_GET_PORT_EGR_HDR_CREDIT_DISCOUNT_TYPE_GET_PORT_HDR_TYPE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_hdr_type_ndx = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter port_hdr_type_ndx after port_egr_hdr_credit_discount_type_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORT_EGR_HDR_CREDIT_DISCOUNT_TYPE_GET_PORT_EGR_HDR_CREDIT_DISCOUNT_TYPE_GET_CR_DISCNT_TYPE_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_PORT_EGR_HDR_CREDIT_DISCOUNT_TYPE_GET_PORT_EGR_HDR_CREDIT_DISCOUNT_TYPE_GET_CR_DISCNT_TYPE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_cr_discnt_type_ndx = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter cr_discnt_type_ndx after port_egr_hdr_credit_discount_type_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_port_egr_hdr_credit_discount_type_get(
          unit,
          prm_port_hdr_type_ndx,
          prm_cr_discnt_type_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** tmd_port_egr_hdr_credit_discount_type_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "tmd_port_egr_hdr_credit_discount_type_get");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  TMD_PORT_EGR_HDR_CR_DISCOUNT_INFO_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: port_egr_hdr_credit_discount_select_set (section ports)
 */
int
  ui_tmd_api_ports_port_egr_hdr_credit_discount_select_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_port_ndx;
  TMD_PORT_EGR_HDR_CR_DISCOUNT_TYPE
    prm_cr_discnt_type;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ports");
  soc_sand_proc_name = "tmd_port_egr_hdr_credit_discount_select_set";

  unit = tmd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORT_EGR_HDR_CREDIT_DISCOUNT_SELECT_SET_PORT_EGR_HDR_CREDIT_DISCOUNT_SELECT_SET_PORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORT_EGR_HDR_CREDIT_DISCOUNT_SELECT_SET_PORT_EGR_HDR_CREDIT_DISCOUNT_SELECT_SET_PORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter port_ndx after port_egr_hdr_credit_discount_select_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = tmd_port_egr_hdr_credit_discount_select_get(
          unit,
          prm_port_ndx,
          &prm_cr_discnt_type
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** tmd_port_egr_hdr_credit_discount_select_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "tmd_port_egr_hdr_credit_discount_select_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORT_EGR_HDR_CREDIT_DISCOUNT_SELECT_SET_PORT_EGR_HDR_CREDIT_DISCOUNT_SELECT_SET_CR_DISCNT_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_PORT_EGR_HDR_CREDIT_DISCOUNT_SELECT_SET_PORT_EGR_HDR_CREDIT_DISCOUNT_SELECT_SET_CR_DISCNT_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_cr_discnt_type = param_val->numeric_equivalent;
  }


  /* Call function */
  ret = tmd_port_egr_hdr_credit_discount_select_set(
          unit,
          prm_port_ndx,
          prm_cr_discnt_type
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** tmd_port_egr_hdr_credit_discount_select_set - FAIL", TRUE);
    soc_petra_disp_result(ret, "tmd_port_egr_hdr_credit_discount_select_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: port_egr_hdr_credit_discount_select_get (section ports)
 */
int
  ui_tmd_api_ports_port_egr_hdr_credit_discount_select_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_port_ndx;
  TMD_PORT_EGR_HDR_CR_DISCOUNT_TYPE
    prm_cr_discnt_type;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ports");
  soc_sand_proc_name = "tmd_port_egr_hdr_credit_discount_select_get";

  unit = tmd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORT_EGR_HDR_CREDIT_DISCOUNT_SELECT_GET_PORT_EGR_HDR_CREDIT_DISCOUNT_SELECT_GET_PORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PORT_EGR_HDR_CREDIT_DISCOUNT_SELECT_GET_PORT_EGR_HDR_CREDIT_DISCOUNT_SELECT_GET_PORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter port_ndx after port_egr_hdr_credit_discount_select_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_port_egr_hdr_credit_discount_select_get(
          unit,
          prm_port_ndx,
          &prm_cr_discnt_type
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** tmd_port_egr_hdr_credit_discount_select_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "tmd_port_egr_hdr_credit_discount_select_get");
    goto exit;
  }

  send_string_to_screen("--> cr_discnt_type:", TRUE);
  soc_sand_os_printf( "cr_discnt_type: %s\n\r",TMD_PORT_EGR_HDR_CR_DISCOUNT_TYPE_to_string(prm_cr_discnt_type));


  goto exit;
exit:
  return ui_ret;
}

#endif

#ifdef UI_INGRESS_PACKET_QUEUING
/********************************************************************
 *  Function handler: ipq_explicit_mapping_mode_info_set (section ingress_packet_queuing)
 */
int
  ui_tmd_api_ingress_packet_queuing_ipq_explicit_mapping_mode_info_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_IPQ_EXPLICIT_MAPPING_MODE_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_packet_queuing");
  soc_sand_proc_name = "tmd_ipq_explicit_mapping_mode_info_set";

  unit = tmd_get_default_unit();
  TMD_IPQ_EXPLICIT_MAPPING_MODE_INFO_clear(&prm_info);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = tmd_ipq_explicit_mapping_mode_info_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ipq_explicit_mapping_mode_info_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_IPQ_EXPLICIT_MAPPING_MODE_INFO_SET_IPQ_EXPLICIT_MAPPING_MODE_INFO_SET_INFO_QUEUE_ID_ADD_NOT_DECREMENT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_IPQ_EXPLICIT_MAPPING_MODE_INFO_SET_IPQ_EXPLICIT_MAPPING_MODE_INFO_SET_INFO_QUEUE_ID_ADD_NOT_DECREMENT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.queue_id_add_not_decrement = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_IPQ_EXPLICIT_MAPPING_MODE_INFO_SET_IPQ_EXPLICIT_MAPPING_MODE_INFO_SET_INFO_BASE_QUEUE_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_IPQ_EXPLICIT_MAPPING_MODE_INFO_SET_IPQ_EXPLICIT_MAPPING_MODE_INFO_SET_INFO_BASE_QUEUE_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.base_queue_id = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_ipq_explicit_mapping_mode_info_set(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ipq_explicit_mapping_mode_info_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ipq_explicit_mapping_mode_info_get (section ingress_packet_queuing)
 */
int
  ui_tmd_api_ingress_packet_queuing_ipq_explicit_mapping_mode_info_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_IPQ_EXPLICIT_MAPPING_MODE_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_packet_queuing");
  soc_sand_proc_name = "tmd_ipq_explicit_mapping_mode_info_get";

  unit = tmd_get_default_unit();
  TMD_IPQ_EXPLICIT_MAPPING_MODE_INFO_clear(&prm_info);

  /* Get parameters */

  /* Call function */
  ret = tmd_ipq_explicit_mapping_mode_info_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ipq_explicit_mapping_mode_info_get");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  TMD_IPQ_EXPLICIT_MAPPING_MODE_INFO_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ipq_traffic_class_map_set (section ingress_packet_queuing)
 */
int
  ui_tmd_api_ingress_packet_queuing_ipq_traffic_class_map_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_IPQ_TR_CLS
    prm_tr_cls_ndx = 0;
  TMD_IPQ_TR_CLS
    prm_new_class;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_packet_queuing");
  soc_sand_proc_name = "tmd_ipq_traffic_class_map_set";

  unit = tmd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_IPQ_TRAFFIC_CLASS_MAP_SET_IPQ_TRAFFIC_CLASS_MAP_SET_TR_CLS_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_IPQ_TRAFFIC_CLASS_MAP_SET_IPQ_TRAFFIC_CLASS_MAP_SET_TR_CLS_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tr_cls_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter tr_cls_ndx after ipq_traffic_class_map_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = tmd_ipq_traffic_class_map_get(
          unit,
          prm_tr_cls_ndx,
          &prm_new_class
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ipq_traffic_class_map_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_IPQ_TRAFFIC_CLASS_MAP_SET_IPQ_TRAFFIC_CLASS_MAP_SET_NEW_CLASS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_IPQ_TRAFFIC_CLASS_MAP_SET_IPQ_TRAFFIC_CLASS_MAP_SET_NEW_CLASS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_new_class = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_ipq_traffic_class_map_set(
          unit,
          prm_tr_cls_ndx,
          prm_new_class
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ipq_traffic_class_map_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ipq_traffic_class_map_get (section ingress_packet_queuing)
 */
int
  ui_tmd_api_ingress_packet_queuing_ipq_traffic_class_map_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_IPQ_TR_CLS
    prm_tr_cls_ndx = 0;
  TMD_IPQ_TR_CLS
    prm_new_class;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_packet_queuing");
  soc_sand_proc_name = "tmd_ipq_traffic_class_map_get";

  unit = tmd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_IPQ_TRAFFIC_CLASS_MAP_GET_IPQ_TRAFFIC_CLASS_MAP_GET_TR_CLS_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_IPQ_TRAFFIC_CLASS_MAP_GET_IPQ_TRAFFIC_CLASS_MAP_GET_TR_CLS_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tr_cls_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter tr_cls_ndx after ipq_traffic_class_map_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_ipq_traffic_class_map_get(
          unit,
          prm_tr_cls_ndx,
          &prm_new_class
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ipq_traffic_class_map_get");
    goto exit;
  }

  soc_sand_os_printf( "new_class: %d\n\r",prm_new_class);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ipq_destination_id_packets_base_queue_id_set (section ingress_packet_queuing)
 */
int
  ui_tmd_api_ingress_packet_queuing_ipq_destination_id_packets_base_queue_id_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_dest_ndx = 0;
  TMD_IPQ_BASEQ_MAP_INFO                  
    info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_packet_queuing");
  soc_sand_proc_name = "tmd_ipq_destination_id_packets_base_queue_id_set";

  unit = tmd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_IPQ_DESTINATION_ID_PACKETS_BASE_QUEUE_ID_SET_IPQ_DESTINATION_ID_PACKETS_BASE_QUEUE_ID_SET_DEST_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_IPQ_DESTINATION_ID_PACKETS_BASE_QUEUE_ID_SET_IPQ_DESTINATION_ID_PACKETS_BASE_QUEUE_ID_SET_DEST_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_dest_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter dest_ndx after ipq_destination_id_packets_base_queue_id_set***", TRUE);
    goto exit;
  }
  
  /* This is a set function, so call GET function first */
  ret = tmd_ipq_destination_id_packets_base_queue_id_get(
          unit,
          prm_dest_ndx,
          &info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ipq_destination_id_packets_base_queue_id_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_IPQ_DESTINATION_ID_PACKETS_BASE_QUEUE_ID_SET_IPQ_DESTINATION_ID_PACKETS_BASE_QUEUE_ID_SET_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_IPQ_DESTINATION_ID_PACKETS_BASE_QUEUE_ID_SET_IPQ_DESTINATION_ID_PACKETS_BASE_QUEUE_ID_SET_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    info.valid = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_IPQ_DESTINATION_ID_PACKETS_BASE_QUEUE_ID_SET_IPQ_DESTINATION_ID_PACKETS_BASE_QUEUE_ID_SET_BASE_QUEUE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_IPQ_DESTINATION_ID_PACKETS_BASE_QUEUE_ID_SET_IPQ_DESTINATION_ID_PACKETS_BASE_QUEUE_ID_SET_BASE_QUEUE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    info.base_queue = (uint32)param_val->value.ulong_value;
  }
  
  /* Call function */
  ret = tmd_ipq_destination_id_packets_base_queue_id_set(
          unit,
          prm_dest_ndx,
          &info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ipq_destination_id_packets_base_queue_id_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ipq_destination_id_packets_base_queue_id_get (section ingress_packet_queuing)
 */
int
  ui_tmd_api_ingress_packet_queuing_ipq_destination_id_packets_base_queue_id_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_dest_ndx = 0;
  TMD_IPQ_BASEQ_MAP_INFO                  
    info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_packet_queuing");
  soc_sand_proc_name = "tmd_ipq_destination_id_packets_base_queue_id_get";
  
  unit = tmd_get_default_unit();
  
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_IPQ_DESTINATION_ID_PACKETS_BASE_QUEUE_ID_GET_IPQ_DESTINATION_ID_PACKETS_BASE_QUEUE_ID_GET_DEST_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_IPQ_DESTINATION_ID_PACKETS_BASE_QUEUE_ID_GET_IPQ_DESTINATION_ID_PACKETS_BASE_QUEUE_ID_GET_DEST_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_dest_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter dest_ndx after ipq_destination_id_packets_base_queue_id_get***", TRUE);
    goto exit;
  }
  
  /* Call function */
  ret = tmd_ipq_destination_id_packets_base_queue_id_get(
          unit,
          prm_dest_ndx,
          &info
        );
  
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ipq_destination_id_packets_base_queue_id_get");
    goto exit;
  }

  soc_sand_os_printf( "valid: %u\n\r",info.valid);

  soc_sand_os_printf( "base_queue: %lu\n\r",info.base_queue);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ipq_queue_intedigitated_mode_set (section ingress_packet_queuing)
 */
int
  ui_tmd_api_ingress_packet_queuing_ipq_queue_intedigitated_mode_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_k_queue_ndx = 0;
  uint8
    prm_is_interdigitated;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_packet_queuing");
  soc_sand_proc_name = "tmd_ipq_queue_interdigitated_mode_set";

  unit = tmd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_IPQ_QUEUE_INTERDIGITATED_MODE_SET_IPQ_QUEUE_INTERDIGITATED_MODE_SET_K_QUEUE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_IPQ_QUEUE_INTERDIGITATED_MODE_SET_IPQ_QUEUE_INTERDIGITATED_MODE_SET_K_QUEUE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_k_queue_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter k_queue_ndx after ipq_queue_interdigitated_mode_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = tmd_ipq_queue_interdigitated_mode_get(
          unit,
          prm_k_queue_ndx,
          &prm_is_interdigitated
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ipq_queue_interdigitated_mode_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_IPQ_QUEUE_INTERDIGITATED_MODE_SET_IPQ_QUEUE_INTERDIGITATED_MODE_SET_IS_INTERDIGITATED_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_IPQ_QUEUE_INTERDIGITATED_MODE_SET_IPQ_QUEUE_INTERDIGITATED_MODE_SET_IS_INTERDIGITATED_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_is_interdigitated = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_ipq_queue_interdigitated_mode_set(
          unit,
          prm_k_queue_ndx,
          prm_is_interdigitated
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ipq_queue_interdigitated_mode_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ipq_queue_interdigitated_mode_get (section ingress_packet_queuing)
 */
int
  ui_tmd_api_ingress_packet_queuing_ipq_queue_interdigitated_mode_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_k_queue_ndx = 0;
  uint8
    prm_is_interdigitated;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_packet_queuing");
  soc_sand_proc_name = "tmd_ipq_queue_interdigitated_mode_get";

  unit = tmd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_IPQ_QUEUE_INTERDIGITATED_MODE_GET_IPQ_QUEUE_INTERDIGITATED_MODE_GET_K_QUEUE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_IPQ_QUEUE_INTERDIGITATED_MODE_GET_IPQ_QUEUE_INTERDIGITATED_MODE_GET_K_QUEUE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_k_queue_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter k_queue_ndx after ipq_queue_interdigitated_mode_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_ipq_queue_interdigitated_mode_get(
          unit,
          prm_k_queue_ndx,
          &prm_is_interdigitated
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ipq_queue_interdigitated_mode_get");
    goto exit;
  }

  soc_sand_os_printf( "is_interdigitated: %u\n\r",prm_is_interdigitated);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ipq_queue_to_flow_mapping_set (section ingress_packet_queuing)
 */
int
  ui_tmd_api_ingress_packet_queuing_ipq_queue_to_flow_mapping_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_queue_quartet_ndx = 0;
  TMD_IPQ_QUARTET_MAP_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_packet_queuing");
  soc_sand_proc_name = "tmd_ipq_queue_to_flow_mapping_set";

  unit = tmd_get_default_unit();
  TMD_IPQ_QUARTET_MAP_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_IPQ_QUEUE_TO_FLOW_MAPPING_SET_IPQ_QUEUE_TO_FLOW_MAPPING_SET_QUEUE_QUARTET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_IPQ_QUEUE_TO_FLOW_MAPPING_SET_IPQ_QUEUE_TO_FLOW_MAPPING_SET_QUEUE_QUARTET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_queue_quartet_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter queue_quartet_ndx after ipq_queue_to_flow_mapping_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = tmd_ipq_queue_to_flow_mapping_get(
          unit,
          prm_queue_quartet_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ipq_queue_to_flow_mapping_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_IPQ_QUEUE_TO_FLOW_MAPPING_SET_IPQ_QUEUE_TO_FLOW_MAPPING_SET_INFO_SYSTEM_PHYSICAL_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_IPQ_QUEUE_TO_FLOW_MAPPING_SET_IPQ_QUEUE_TO_FLOW_MAPPING_SET_INFO_SYSTEM_PHYSICAL_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.system_physical_port = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_IPQ_QUEUE_TO_FLOW_MAPPING_SET_IPQ_QUEUE_TO_FLOW_MAPPING_SET_INFO_IS_COMPOSITE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_IPQ_QUEUE_TO_FLOW_MAPPING_SET_IPQ_QUEUE_TO_FLOW_MAPPING_SET_INFO_IS_COMPOSITE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.is_composite = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_IPQ_QUEUE_TO_FLOW_MAPPING_SET_IPQ_QUEUE_TO_FLOW_MAPPING_SET_INFO_FLOW_QUARTET_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_IPQ_QUEUE_TO_FLOW_MAPPING_SET_IPQ_QUEUE_TO_FLOW_MAPPING_SET_INFO_FLOW_QUARTET_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.flow_quartet_index = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_ipq_queue_to_flow_mapping_set(
          unit,
          prm_queue_quartet_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ipq_queue_to_flow_mapping_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ipq_queue_to_flow_mapping_get (section ingress_packet_queuing)
 */
int
  ui_tmd_api_ingress_packet_queuing_ipq_queue_to_flow_mapping_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_queue_quartet_ndx = 0;
  TMD_IPQ_QUARTET_MAP_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_packet_queuing");
  soc_sand_proc_name = "tmd_ipq_queue_to_flow_mapping_get";

  unit = tmd_get_default_unit();
  TMD_IPQ_QUARTET_MAP_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_IPQ_QUEUE_TO_FLOW_MAPPING_GET_IPQ_QUEUE_TO_FLOW_MAPPING_GET_QUEUE_QUARTET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_IPQ_QUEUE_TO_FLOW_MAPPING_GET_IPQ_QUEUE_TO_FLOW_MAPPING_GET_QUEUE_QUARTET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_queue_quartet_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter queue_quartet_ndx after ipq_queue_to_flow_mapping_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_ipq_queue_to_flow_mapping_get(
          unit,
          prm_queue_quartet_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ipq_queue_to_flow_mapping_get");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  TMD_IPQ_QUARTET_MAP_INFO_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ipq_queue_qrtt_unmap (section ingress_packet_queuing)
 */
int 
  ui_tmd_api_ingress_packet_queuing_ipq_queue_qrtt_unmap(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_queue_quartet_ndx;
   
  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_packet_queuing"); 
  soc_sand_proc_name = "tmd_ipq_queue_qrtt_unmap"; 
 
  unit = tmd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_IPQ_QUEUE_QRTT_UNMAP_IPQ_QUEUE_QRTT_UNMAP_QUEUE_QUARTET_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_IPQ_QUEUE_QRTT_UNMAP_IPQ_QUEUE_QRTT_UNMAP_QUEUE_QUARTET_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_queue_quartet_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter queue_quartet_ndx after ipq_queue_qrtt_unmap***", TRUE); 
    goto exit; 
  } 


  /* Call function */  
  ret = tmd_ipq_queue_qrtt_unmap(
          unit,
          prm_queue_quartet_ndx
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** tmd_ipq_queue_qrtt_unmap - FAIL", TRUE); 
    soc_petra_disp_result(ret, "tmd_ipq_queue_qrtt_unmap");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/********************************************************************
 *  Function handler: ipq_quartet_reset (section ingress_packet_queuing)
 */
int
  ui_tmd_api_ingress_packet_queuing_ipq_quartet_reset(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_queue_quartet_ndx = 0;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_packet_queuing");
  soc_sand_proc_name = "tmd_ipq_quartet_reset";

  unit = tmd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_IPQ_QUARTET_RESET_IPQ_QUARTET_RESET_QUEUE_QUARTET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_IPQ_QUARTET_RESET_IPQ_QUARTET_RESET_QUEUE_QUARTET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_queue_quartet_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter queue_quartet_ndx after ipq_quartet_reset***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_ipq_quartet_reset(
          unit,
          prm_queue_quartet_ndx
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ipq_quartet_reset");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

#ifdef UI_UNDEFED_TEMP
  /********************************************************************
  *  Function handler: ipq_flow_id_packets_base_queue_id_set (section ingress_packet_queueing)
 */
  int
    ui_tmd_api_ingress_packet_queueing_ips_non_empty_queues_get(
    CURRENT_LINE *current_line
    )
  {
    uint32
      is_flow_also = FALSE;

    UI_MACROS_INIT_FUNCTION("ui_tmd_ipq_non_empty_queues_get");

    unit = tmd_get_default_unit();

    /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SEARCH_ALL_NON_EMPTY_QUEUES_FLOW_ALSO_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SEARCH_ALL_NON_EMPTY_QUEUES_FLOW_ALSO_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    is_flow_also = (uint32)param_val->value.ulong_value;
  }

    /* Call function */
    tmd_ips_non_empty_queues_print(
      unit,
      is_flow_also
      );

    goto exit;

exit:
    return ui_ret;
  }

#endif /* UI_UNDEFED_TEMP */
#endif

#ifdef UI_INGRESS_TRAFFIC_MGMT
/********************************************************************
 *  Function handler: itm_dram_buffs_set (section ingress_traffic_mgmt)
 */
/********************************************************************
 *  Function handler: itm_dram_buffs_get (section ingress_traffic_mgmt)
 */
int
  ui_tmd_api_ingress_traffic_mgmt_itm_dram_buffs_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_ITM_DRAM_BUFFERS_INFO
    prm_dram_buffs;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_traffic_mgmt");
  soc_sand_proc_name = "tmd_itm_dram_buffs_get";

  unit = tmd_get_default_unit();
  TMD_ITM_DRAM_BUFFERS_INFO_clear(&prm_dram_buffs);

  /* Call function */
  ret = tmd_itm_dram_buffs_get(
          unit,
          &prm_dram_buffs
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_dram_buffs_get");
    goto exit;
  }

  TMD_ITM_DRAM_BUFFERS_INFO_print(&prm_dram_buffs);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: itm_glob_rcs_fc_set (section ingress_traffic_mgmt)
 */
int
  ui_tmd_api_ingress_traffic_mgmt_itm_glob_rcs_fc_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_ITM_GLOB_RCS_FC_TH
    prm_info;
  TMD_ITM_GLOB_RCS_FC_TH
    prm_exact_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_traffic_mgmt");
  soc_sand_proc_name = "tmd_itm_glob_rcs_fc_set";

  unit = tmd_get_default_unit();
  TMD_ITM_GLOB_RCS_FC_TH_clear(&prm_info);
  TMD_ITM_GLOB_RCS_FC_TH_clear(&prm_exact_info);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = tmd_itm_glob_rcs_fc_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_glob_rcs_fc_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_GLOB_RCS_FC_SET_ITM_GLOB_RCS_FC_SET_INFO_FULL_MC_LP_CLEAR_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_GLOB_RCS_FC_SET_ITM_GLOB_RCS_FC_SET_INFO_FULL_MC_LP_CLEAR_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.full_mc.lp.clear = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_GLOB_RCS_FC_SET_ITM_GLOB_RCS_FC_SET_INFO_FULL_MC_LP_SET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_GLOB_RCS_FC_SET_ITM_GLOB_RCS_FC_SET_INFO_FULL_MC_LP_SET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.full_mc.lp.set = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_GLOB_RCS_FC_SET_ITM_GLOB_RCS_FC_SET_INFO_FULL_MC_HP_CLEAR_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_GLOB_RCS_FC_SET_ITM_GLOB_RCS_FC_SET_INFO_FULL_MC_HP_CLEAR_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.full_mc.hp.clear = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_GLOB_RCS_FC_SET_ITM_GLOB_RCS_FC_SET_INFO_FULL_MC_HP_SET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_GLOB_RCS_FC_SET_ITM_GLOB_RCS_FC_SET_INFO_FULL_MC_HP_SET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.full_mc.hp.set = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_GLOB_RCS_FC_SET_ITM_GLOB_RCS_FC_SET_INFO_UNICAST_LP_CLEAR_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_GLOB_RCS_FC_SET_ITM_GLOB_RCS_FC_SET_INFO_UNICAST_LP_CLEAR_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.unicast.lp.clear = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_GLOB_RCS_FC_SET_ITM_GLOB_RCS_FC_SET_INFO_UNICAST_LP_SET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_GLOB_RCS_FC_SET_ITM_GLOB_RCS_FC_SET_INFO_UNICAST_LP_SET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.unicast.lp.set = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_GLOB_RCS_FC_SET_ITM_GLOB_RCS_FC_SET_INFO_UNICAST_HP_CLEAR_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_GLOB_RCS_FC_SET_ITM_GLOB_RCS_FC_SET_INFO_UNICAST_HP_CLEAR_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.unicast.hp.clear = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_GLOB_RCS_FC_SET_ITM_GLOB_RCS_FC_SET_INFO_UNICAST_HP_SET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_GLOB_RCS_FC_SET_ITM_GLOB_RCS_FC_SET_INFO_UNICAST_HP_SET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.unicast.hp.set = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_GLOB_RCS_FC_SET_ITM_GLOB_RCS_FC_SET_INFO_BDBS_LP_CLEAR_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_GLOB_RCS_FC_SET_ITM_GLOB_RCS_FC_SET_INFO_BDBS_LP_CLEAR_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.bdbs.lp.clear = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_GLOB_RCS_FC_SET_ITM_GLOB_RCS_FC_SET_INFO_BDBS_LP_SET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_GLOB_RCS_FC_SET_ITM_GLOB_RCS_FC_SET_INFO_BDBS_LP_SET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.bdbs.lp.set = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_GLOB_RCS_FC_SET_ITM_GLOB_RCS_FC_SET_INFO_BDBS_HP_CLEAR_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_GLOB_RCS_FC_SET_ITM_GLOB_RCS_FC_SET_INFO_BDBS_HP_CLEAR_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.bdbs.hp.clear = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_GLOB_RCS_FC_SET_ITM_GLOB_RCS_FC_SET_INFO_BDBS_HP_SET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_GLOB_RCS_FC_SET_ITM_GLOB_RCS_FC_SET_INFO_BDBS_HP_SET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.bdbs.hp.set = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_itm_glob_rcs_fc_set(
          unit,
          &prm_info,
          &prm_exact_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_glob_rcs_fc_set");
    goto exit;
  }

  send_string_to_screen("--> exact_info:", TRUE);
  TMD_ITM_GLOB_RCS_FC_TH_print(&prm_exact_info);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: itm_glob_rcs_fc_get (section ingress_traffic_mgmt)
 */
int
  ui_tmd_api_ingress_traffic_mgmt_itm_glob_rcs_fc_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_ITM_GLOB_RCS_FC_TH
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_traffic_mgmt");
  soc_sand_proc_name = "tmd_itm_glob_rcs_fc_get";

  unit = tmd_get_default_unit();
  TMD_ITM_GLOB_RCS_FC_TH_clear(&prm_info);

  /* Get parameters */

  /* Call function */
  ret = tmd_itm_glob_rcs_fc_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_glob_rcs_fc_get");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  TMD_ITM_GLOB_RCS_FC_TH_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: itm_glob_rcs_drop_set (section ingress_traffic_mgmt)
 */
int
  ui_tmd_api_ingress_traffic_mgmt_itm_glob_rcs_drop_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_mini_mc_index = 0xFFFFFFFF;
  uint32
    prm_full_mc_index = 0xFFFFFFFF;
  uint32
    prm_unicast_index = 0xFFFFFFFF;
  uint32
    prm_bds_index = 0xFFFFFFFF;
  uint32
    prm_bdbs_index = 0xFFFFFFFF;
  TMD_ITM_GLOB_RCS_DROP_TH
    prm_info;
  TMD_ITM_GLOB_RCS_DROP_TH
    prm_exact_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_traffic_mgmt");
  soc_sand_proc_name = "tmd_itm_glob_rcs_drop_set";

  unit = tmd_get_default_unit();
  TMD_ITM_GLOB_RCS_DROP_TH_clear(&prm_info);
  TMD_ITM_GLOB_RCS_DROP_TH_clear(&prm_exact_info);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = tmd_itm_glob_rcs_drop_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_glob_rcs_drop_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_GLOB_RCS_DROP_SET_ITM_GLOB_RCS_DROP_SET_INFO_MINI_MC_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_GLOB_RCS_DROP_SET_ITM_GLOB_RCS_DROP_SET_INFO_MINI_MC_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mini_mc_index = (uint32)param_val->value.ulong_value;
  }

  if(prm_mini_mc_index != 0xFFFFFFFF)
  {
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_GLOB_RCS_DROP_SET_ITM_GLOB_RCS_DROP_SET_INFO_MINI_MC_CLEAR_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_GLOB_RCS_DROP_SET_ITM_GLOB_RCS_DROP_SET_INFO_MINI_MC_CLEAR_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.mini_mc[ prm_mini_mc_index].clear = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_GLOB_RCS_DROP_SET_ITM_GLOB_RCS_DROP_SET_INFO_MINI_MC_SET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_GLOB_RCS_DROP_SET_ITM_GLOB_RCS_DROP_SET_INFO_MINI_MC_SET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.mini_mc[ prm_mini_mc_index].set = (uint32)param_val->value.ulong_value;
  }

  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_GLOB_RCS_DROP_SET_ITM_GLOB_RCS_DROP_SET_INFO_FULL_MC_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_GLOB_RCS_DROP_SET_ITM_GLOB_RCS_DROP_SET_INFO_FULL_MC_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_full_mc_index = (uint32)param_val->value.ulong_value;
  }

  if(prm_full_mc_index != 0xFFFFFFFF)
  {
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_GLOB_RCS_DROP_SET_ITM_GLOB_RCS_DROP_SET_INFO_FULL_MC_CLEAR_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_GLOB_RCS_DROP_SET_ITM_GLOB_RCS_DROP_SET_INFO_FULL_MC_CLEAR_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.full_mc[ prm_full_mc_index].clear = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_GLOB_RCS_DROP_SET_ITM_GLOB_RCS_DROP_SET_INFO_FULL_MC_SET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_GLOB_RCS_DROP_SET_ITM_GLOB_RCS_DROP_SET_INFO_FULL_MC_SET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.full_mc[ prm_full_mc_index].set = (uint32)param_val->value.ulong_value;
  }

  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_GLOB_RCS_DROP_SET_ITM_GLOB_RCS_DROP_SET_INFO_UNICAST_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_GLOB_RCS_DROP_SET_ITM_GLOB_RCS_DROP_SET_INFO_UNICAST_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_unicast_index = (uint32)param_val->value.ulong_value;
  }

  if(prm_unicast_index != 0xFFFFFFFF)
  {
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_GLOB_RCS_DROP_SET_ITM_GLOB_RCS_DROP_SET_INFO_UNICAST_CLEAR_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_GLOB_RCS_DROP_SET_ITM_GLOB_RCS_DROP_SET_INFO_UNICAST_CLEAR_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.unicast[ prm_unicast_index].clear = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_GLOB_RCS_DROP_SET_ITM_GLOB_RCS_DROP_SET_INFO_UNICAST_SET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_GLOB_RCS_DROP_SET_ITM_GLOB_RCS_DROP_SET_INFO_UNICAST_SET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.unicast[ prm_unicast_index].set = (uint32)param_val->value.ulong_value;
  }

  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_GLOB_RCS_DROP_SET_ITM_GLOB_RCS_DROP_SET_INFO_BDS_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_GLOB_RCS_DROP_SET_ITM_GLOB_RCS_DROP_SET_INFO_BDS_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_bds_index = (uint32)param_val->value.ulong_value;
  }

  if(prm_bds_index != 0xFFFFFFFF)
  {

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_GLOB_RCS_DROP_SET_ITM_GLOB_RCS_DROP_SET_INFO_BDS_CLEAR_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_GLOB_RCS_DROP_SET_ITM_GLOB_RCS_DROP_SET_INFO_BDS_CLEAR_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.bds[ prm_bds_index].clear = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_GLOB_RCS_DROP_SET_ITM_GLOB_RCS_DROP_SET_INFO_BDS_SET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_GLOB_RCS_DROP_SET_ITM_GLOB_RCS_DROP_SET_INFO_BDS_SET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.bds[ prm_bds_index].set = (uint32)param_val->value.ulong_value;
  }

  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_GLOB_RCS_DROP_SET_ITM_GLOB_RCS_DROP_SET_INFO_BDBS_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_GLOB_RCS_DROP_SET_ITM_GLOB_RCS_DROP_SET_INFO_BDBS_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_bdbs_index = (uint32)param_val->value.ulong_value;
  }

  if(prm_bdbs_index != 0xFFFFFFFF)
  {

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_GLOB_RCS_DROP_SET_ITM_GLOB_RCS_DROP_SET_INFO_BDBS_CLEAR_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_GLOB_RCS_DROP_SET_ITM_GLOB_RCS_DROP_SET_INFO_BDBS_CLEAR_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.bdbs[ prm_bdbs_index].clear = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_GLOB_RCS_DROP_SET_ITM_GLOB_RCS_DROP_SET_INFO_BDBS_SET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_GLOB_RCS_DROP_SET_ITM_GLOB_RCS_DROP_SET_INFO_BDBS_SET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.bdbs[ prm_bdbs_index].set = (uint32)param_val->value.ulong_value;
  }

  }


  /* Call function */
  ret = tmd_itm_glob_rcs_drop_set(
          unit,
          &prm_info,
          &prm_exact_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_glob_rcs_drop_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: itm_glob_rcs_drop_get (section ingress_traffic_mgmt)
 */
int
  ui_tmd_api_ingress_traffic_mgmt_itm_glob_rcs_drop_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_ITM_GLOB_RCS_DROP_TH
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_traffic_mgmt");
  soc_sand_proc_name = "tmd_itm_glob_rcs_drop_get";

  unit = tmd_get_default_unit();
  TMD_ITM_GLOB_RCS_DROP_TH_clear(&prm_info);

  /* Get parameters */

  /* Call function */
  ret = tmd_itm_glob_rcs_drop_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_glob_rcs_drop_get");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  TMD_ITM_GLOB_RCS_DROP_TH_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: itm_category_rngs_set (section ingress_traffic_mgmt)
 */
int
  ui_tmd_api_ingress_traffic_mgmt_itm_vsq_category_rngs_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_ITM_CATEGORY_RNGS
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_traffic_mgmt");
  soc_sand_proc_name = "tmd_itm_category_rngs_set";

  unit = tmd_get_default_unit();
  TMD_ITM_CATEGORY_RNGS_clear(&prm_info);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = tmd_itm_category_rngs_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_category_rngs_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_CATEGORY_RNGS_SET_ITM_VSQ_CATEGORY_RNGS_SET_INFO_VSQ_CTGRY2_END_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_CATEGORY_RNGS_SET_ITM_VSQ_CATEGORY_RNGS_SET_INFO_VSQ_CTGRY2_END_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.vsq_ctgry2_end = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_CATEGORY_RNGS_SET_ITM_VSQ_CATEGORY_RNGS_SET_INFO_VSQ_CTGRY1_END_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_CATEGORY_RNGS_SET_ITM_VSQ_CATEGORY_RNGS_SET_INFO_VSQ_CTGRY1_END_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.vsq_ctgry1_end = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_CATEGORY_RNGS_SET_ITM_VSQ_CATEGORY_RNGS_SET_INFO_VSQ_CTGRY0_END_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_CATEGORY_RNGS_SET_ITM_VSQ_CATEGORY_RNGS_SET_INFO_VSQ_CTGRY0_END_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.vsq_ctgry0_end = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_itm_category_rngs_set(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_category_rngs_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: itm_vsq_category_rngs_get (section ingress_traffic_mgmt)
 */
int
  ui_tmd_api_ingress_traffic_mgmt_itm_vsq_category_rngs_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_ITM_CATEGORY_RNGS
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_traffic_mgmt");
  soc_sand_proc_name = "tmd_itm_category_rngs_get";

  unit = tmd_get_default_unit();
  TMD_ITM_CATEGORY_RNGS_clear(&prm_info);

  /* Get parameters */

  /* Call function */
  ret = tmd_itm_category_rngs_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_category_rngs_get");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  TMD_ITM_CATEGORY_RNGS_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: itm_admit_test_tmplt_set (section ingress_traffic_mgmt)
 */
int
  ui_tmd_api_ingress_traffic_mgmt_itm_admit_test_tmplt_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_admt_tst_ndx = 0;
  TMD_ITM_ADMIT_TEST_TMPLT_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_traffic_mgmt");
  soc_sand_proc_name = "tmd_itm_admit_test_tmplt_set";

  unit = tmd_get_default_unit();
  TMD_ITM_ADMIT_TEST_TMPLT_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_ADMIT_TEST_TMPLT_SET_ITM_ADMIT_TEST_TMPLT_SET_ADMT_TST_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_ADMIT_TEST_TMPLT_SET_ITM_ADMIT_TEST_TMPLT_SET_ADMT_TST_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_admt_tst_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter admt_tst_ndx after itm_admit_test_tmplt_set***", TRUE);
    goto exit;
  }

   /* Call function */
  ret = tmd_itm_admit_test_tmplt_get(
          unit,
          prm_admt_tst_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_admit_test_tmplt_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_ADMIT_TEST_TMPLT_SET_ITM_ADMIT_TEST_TMPLT_SET_INFO_TEST_B_STTSTCS_TAG_TEST_EN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_ADMIT_TEST_TMPLT_SET_ITM_ADMIT_TEST_TMPLT_SET_INFO_TEST_B_STTSTCS_TAG_TEST_EN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.test_b.sttstcs_tag_test_en = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_ADMIT_TEST_TMPLT_SET_ITM_ADMIT_TEST_TMPLT_SET_INFO_TEST_B_CTGRY2_3_CNCTN_TEST_EN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_ADMIT_TEST_TMPLT_SET_ITM_ADMIT_TEST_TMPLT_SET_INFO_TEST_B_CTGRY2_3_CNCTN_TEST_EN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.test_b.ctgry2_3_cnctn_test_en = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_ADMIT_TEST_TMPLT_SET_ITM_ADMIT_TEST_TMPLT_SET_INFO_TEST_B_CTGRY_TRFFC_TEST_EN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_ADMIT_TEST_TMPLT_SET_ITM_ADMIT_TEST_TMPLT_SET_INFO_TEST_B_CTGRY_TRFFC_TEST_EN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.test_b.ctgry_trffc_test_en = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_ADMIT_TEST_TMPLT_SET_ITM_ADMIT_TEST_TMPLT_SET_INFO_TEST_B_CTGRY_TEST_EN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_ADMIT_TEST_TMPLT_SET_ITM_ADMIT_TEST_TMPLT_SET_INFO_TEST_B_CTGRY_TEST_EN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.test_b.ctgry_test_en = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_ADMIT_TEST_TMPLT_SET_ITM_ADMIT_TEST_TMPLT_SET_INFO_TEST_A_STTSTCS_TAG_TEST_EN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_ADMIT_TEST_TMPLT_SET_ITM_ADMIT_TEST_TMPLT_SET_INFO_TEST_A_STTSTCS_TAG_TEST_EN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.test_a.sttstcs_tag_test_en = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_ADMIT_TEST_TMPLT_SET_ITM_ADMIT_TEST_TMPLT_SET_INFO_TEST_A_CTGRY2_3_CNCTN_TEST_EN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_ADMIT_TEST_TMPLT_SET_ITM_ADMIT_TEST_TMPLT_SET_INFO_TEST_A_CTGRY2_3_CNCTN_TEST_EN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.test_a.ctgry2_3_cnctn_test_en = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_ADMIT_TEST_TMPLT_SET_ITM_ADMIT_TEST_TMPLT_SET_INFO_TEST_A_CTGRY_TRFFC_TEST_EN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_ADMIT_TEST_TMPLT_SET_ITM_ADMIT_TEST_TMPLT_SET_INFO_TEST_A_CTGRY_TRFFC_TEST_EN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.test_a.ctgry_trffc_test_en = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_ADMIT_TEST_TMPLT_SET_ITM_ADMIT_TEST_TMPLT_SET_INFO_TEST_A_CTGRY_TEST_EN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_ADMIT_TEST_TMPLT_SET_ITM_ADMIT_TEST_TMPLT_SET_INFO_TEST_A_CTGRY_TEST_EN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.test_a.ctgry_test_en = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_itm_admit_test_tmplt_set(
          unit,
          prm_admt_tst_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_admit_test_tmplt_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: itm_admit_test_tmplt_get (section ingress_traffic_mgmt)
 */
int
  ui_tmd_api_ingress_traffic_mgmt_itm_admit_test_tmplt_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_admt_tst_ndx = 0;
  TMD_ITM_ADMIT_TEST_TMPLT_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_traffic_mgmt");
  soc_sand_proc_name = "tmd_itm_admit_test_tmplt_get";

  unit = tmd_get_default_unit();
  TMD_ITM_ADMIT_TEST_TMPLT_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_ADMIT_TEST_TMPLT_GET_ITM_ADMIT_TEST_TMPLT_GET_ADMT_TST_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_ADMIT_TEST_TMPLT_GET_ITM_ADMIT_TEST_TMPLT_GET_ADMT_TST_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_admt_tst_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter admt_tst_ndx after itm_admit_test_tmplt_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_itm_admit_test_tmplt_get(
          unit,
          prm_admt_tst_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_admit_test_tmplt_get");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  TMD_ITM_ADMIT_TEST_TMPLT_INFO_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: itm_cr_request_set (section ingress_traffic_mgmt)
 */
int
  ui_tmd_api_ingress_traffic_mgmt_itm_cr_request_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_qt_ndx = 0;
  TMD_ITM_CR_REQUEST_INFO
    prm_info;
  TMD_ITM_CR_REQUEST_INFO
    prm_exact_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_traffic_mgmt");
  soc_sand_proc_name = "tmd_itm_cr_request_set";

  unit = tmd_get_default_unit();
  TMD_ITM_CR_REQUEST_INFO_clear(&prm_info);
  TMD_ITM_CR_REQUEST_INFO_clear(&prm_exact_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_CR_REQUEST_SET_ITM_CR_REQUEST_SET_QT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_CR_REQUEST_SET_ITM_CR_REQUEST_SET_QT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_qt_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter qt_ndx after itm_cr_request_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = tmd_itm_cr_request_get(
          unit,
          prm_qt_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_cr_request_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_CR_REQUEST_SET_ITM_CR_REQUEST_SET_INFO_SATISFIED_TH_EMPTY_QUEUES_EXCEED_MAX_EMPTY_Q_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_CR_REQUEST_SET_ITM_CR_REQUEST_SET_INFO_SATISFIED_TH_EMPTY_QUEUES_EXCEED_MAX_EMPTY_Q_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.satisfied_th.empty_queues.exceed_max_empty_q = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_CR_REQUEST_SET_ITM_CR_REQUEST_SET_INFO_SATISFIED_TH_EMPTY_QUEUES_MAX_CREDIT_BALANCE_EMPTY_Q_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_CR_REQUEST_SET_ITM_CR_REQUEST_SET_INFO_SATISFIED_TH_EMPTY_QUEUES_MAX_CREDIT_BALANCE_EMPTY_Q_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.satisfied_th.empty_queues.max_credit_balance_empty_q = (int32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_CR_REQUEST_SET_ITM_CR_REQUEST_SET_INFO_SATISFIED_TH_EMPTY_QUEUES_SATISFIED_EMPTY_Q_TH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_CR_REQUEST_SET_ITM_CR_REQUEST_SET_INFO_SATISFIED_TH_EMPTY_QUEUES_SATISFIED_EMPTY_Q_TH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.satisfied_th.empty_queues.satisfied_empty_q_th = (int32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_CR_REQUEST_SET_ITM_CR_REQUEST_SET_INFO_SATISFIED_TH_BACKLOG_TH_BACKLOG_EXIT_TH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_CR_REQUEST_SET_ITM_CR_REQUEST_SET_INFO_SATISFIED_TH_BACKLOG_TH_BACKLOG_EXIT_TH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.satisfied_th.backlog_th.backlog_exit_th = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_CR_REQUEST_SET_ITM_CR_REQUEST_SET_INFO_SATISFIED_TH_BACKLOG_TH_BACKLOG_ENTER_TH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_CR_REQUEST_SET_ITM_CR_REQUEST_SET_INFO_SATISFIED_TH_BACKLOG_TH_BACKLOG_ENTER_TH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.satisfied_th.backlog_th.backlog_enter_th = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_CR_REQUEST_SET_ITM_CR_REQUEST_SET_INFO_SATISFIED_TH_BACKOFF_TH_BACKOFF_EXIT_TH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_CR_REQUEST_SET_ITM_CR_REQUEST_SET_INFO_SATISFIED_TH_BACKOFF_TH_BACKOFF_EXIT_TH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.satisfied_th.backoff_th.backoff_exit_th = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_CR_REQUEST_SET_ITM_CR_REQUEST_SET_INFO_SATISFIED_TH_BACKOFF_TH_BACKOFF_ENTER_TH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_CR_REQUEST_SET_ITM_CR_REQUEST_SET_INFO_SATISFIED_TH_BACKOFF_TH_BACKOFF_ENTER_TH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.satisfied_th.backoff_th.backoff_enter_th = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_CR_REQUEST_SET_ITM_CR_REQUEST_SET_INFO_HUNGRY_TH_MULTIPLIER_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_CR_REQUEST_SET_ITM_CR_REQUEST_SET_INFO_HUNGRY_TH_MULTIPLIER_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.hungry_th.multiplier = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_CR_REQUEST_SET_ITM_CR_REQUEST_SET_INFO_HUNGRY_TH_NORMAL_TO_SLOW_TH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_CR_REQUEST_SET_ITM_CR_REQUEST_SET_INFO_HUNGRY_TH_NORMAL_TO_SLOW_TH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.hungry_th.normal_to_slow_th = (int32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_CR_REQUEST_SET_ITM_CR_REQUEST_SET_INFO_HUNGRY_TH_SLOW_TO_NORMAL_TH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_CR_REQUEST_SET_ITM_CR_REQUEST_SET_INFO_HUNGRY_TH_SLOW_TO_NORMAL_TH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.hungry_th.slow_to_normal_th = (int32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_CR_REQUEST_SET_ITM_CR_REQUEST_SET_INFO_HUNGRY_TH_OFF_TO_NORMAL_TH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_CR_REQUEST_SET_ITM_CR_REQUEST_SET_INFO_HUNGRY_TH_OFF_TO_NORMAL_TH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.hungry_th.off_to_normal_th = (int32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_CR_REQUEST_SET_ITM_CR_REQUEST_SET_INFO_HUNGRY_TH_OFF_TO_SLOW_TH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_CR_REQUEST_SET_ITM_CR_REQUEST_SET_INFO_HUNGRY_TH_OFF_TO_SLOW_TH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.hungry_th.off_to_slow_th = (int32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_CR_REQUEST_SET_ITM_CR_REQUEST_SET_INFO_WD_TH_CR_WD_DLT_Q_TH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_CR_REQUEST_SET_ITM_CR_REQUEST_SET_INFO_WD_TH_CR_WD_DLT_Q_TH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.wd_th.cr_wd_dlt_q_th = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_CR_REQUEST_SET_ITM_CR_REQUEST_SET_INFO_WD_TH_CR_WD_STTS_MSG_GEN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_CR_REQUEST_SET_ITM_CR_REQUEST_SET_INFO_WD_TH_CR_WD_STTS_MSG_GEN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.wd_th.cr_wd_stts_msg_gen = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_itm_cr_request_set(
          unit,
          prm_qt_ndx,
          &prm_info,
          &prm_exact_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_cr_request_set");
    goto exit;
  }

  send_string_to_screen("--> exact_info:", TRUE);
  TMD_ITM_CR_REQUEST_INFO_print(&prm_exact_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: itm_cr_request_get (section ingress_traffic_mgmt)
 */
int
  ui_tmd_api_ingress_traffic_mgmt_itm_cr_request_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_qt_ndx = 0;
  TMD_ITM_CR_REQUEST_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_traffic_mgmt");
  soc_sand_proc_name = "tmd_itm_cr_request_get";

  unit = tmd_get_default_unit();
  TMD_ITM_CR_REQUEST_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_CR_REQUEST_GET_ITM_CR_REQUEST_GET_QT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_CR_REQUEST_GET_ITM_CR_REQUEST_GET_QT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_qt_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter qt_ndx after itm_cr_request_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_itm_cr_request_get(
          unit,
          prm_qt_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_cr_request_get");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  TMD_ITM_CR_REQUEST_INFO_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: itm_cr_discount_set (section ingress_traffic_mgmt)
 */
int
  ui_tmd_api_ingress_traffic_mgmt_itm_cr_discount_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_cr_cls_ndx = 0;
  TMD_ITM_CR_DISCOUNT_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_traffic_mgmt");
  soc_sand_proc_name = "tmd_itm_cr_discount_set";

  unit = tmd_get_default_unit();
  TMD_ITM_CR_DISCOUNT_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_CR_DISCOUNT_SET_ITM_CR_DISCOUNT_SET_CR_CLS_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_CR_DISCOUNT_SET_ITM_CR_DISCOUNT_SET_CR_CLS_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_cr_cls_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter cr_cls_ndx after itm_cr_discount_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = tmd_itm_cr_discount_get(
          unit,
          prm_cr_cls_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_cr_discount_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_CR_DISCOUNT_SET_ITM_CR_DISCOUNT_SET_INFO_DISCOUNT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_CR_DISCOUNT_SET_ITM_CR_DISCOUNT_SET_INFO_DISCOUNT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.discount = (int32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_itm_cr_discount_set(
          unit,
          prm_cr_cls_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_cr_discount_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: itm_cr_discount_get (section ingress_traffic_mgmt)
 */
int
  ui_tmd_api_ingress_traffic_mgmt_itm_cr_discount_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_cr_cls_ndx = 0;
  TMD_ITM_CR_DISCOUNT_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_traffic_mgmt");
  soc_sand_proc_name = "tmd_itm_cr_discount_get";

  unit = tmd_get_default_unit();
  TMD_ITM_CR_DISCOUNT_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_CR_DISCOUNT_GET_ITM_CR_DISCOUNT_GET_CR_CLS_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_CR_DISCOUNT_GET_ITM_CR_DISCOUNT_GET_CR_CLS_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_cr_cls_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter cr_cls_ndx after itm_cr_discount_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_itm_cr_discount_get(
          unit,
          prm_cr_cls_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_cr_discount_get");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  TMD_ITM_CR_DISCOUNT_INFO_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: itm_queue_test_tmplt_set (section ingress_traffic_mgmt)
 */
int
  ui_tmd_api_ingress_traffic_mgmt_itm_queue_test_tmplt_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_rt_cls_ndx = 0;
  uint32
    prm_drop_precedence_ndx = 0;
  TMD_ITM_ADMIT_TSTS
    prm_test_tmplt;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_traffic_mgmt");
  soc_sand_proc_name = "tmd_itm_queue_test_tmplt_set";

  unit = tmd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_QUEUE_TEST_TMPLT_SET_ITM_QUEUE_TEST_TMPLT_SET_RT_CLS_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_QUEUE_TEST_TMPLT_SET_ITM_QUEUE_TEST_TMPLT_SET_RT_CLS_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_rt_cls_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter rt_cls_ndx after itm_queue_test_tmplt_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_QUEUE_TEST_TMPLT_SET_ITM_QUEUE_TEST_TMPLT_SET_DROP_PRECEDENCE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_QUEUE_TEST_TMPLT_SET_ITM_QUEUE_TEST_TMPLT_SET_DROP_PRECEDENCE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_drop_precedence_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter drop_precedence_ndx after itm_queue_test_tmplt_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = tmd_itm_queue_test_tmplt_get(
          unit,
          prm_rt_cls_ndx,
          prm_drop_precedence_ndx,
          &prm_test_tmplt
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_queue_test_tmplt_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_QUEUE_TEST_TMPLT_SET_ITM_QUEUE_TEST_TMPLT_SET_TEST_TMPLT_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_ITM_QUEUE_TEST_TMPLT_SET_ITM_QUEUE_TEST_TMPLT_SET_TEST_TMPLT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_test_tmplt = param_val->numeric_equivalent;
  }


  /* Call function */
  ret = tmd_itm_queue_test_tmplt_set(
          unit,
          prm_rt_cls_ndx,
          prm_drop_precedence_ndx,
          prm_test_tmplt
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_queue_test_tmplt_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: itm_queue_test_tmplt_get (section ingress_traffic_mgmt)
 */
int
  ui_tmd_api_ingress_traffic_mgmt_itm_queue_test_tmplt_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_rt_cls_ndx = 0;
  uint32
    prm_drop_precedence_ndx = 0;
  TMD_ITM_ADMIT_TSTS
    prm_test_tmplt;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_traffic_mgmt");
  soc_sand_proc_name = "tmd_itm_queue_test_tmplt_get";

  unit = tmd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_QUEUE_TEST_TMPLT_GET_ITM_QUEUE_TEST_TMPLT_GET_RT_CLS_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_QUEUE_TEST_TMPLT_GET_ITM_QUEUE_TEST_TMPLT_GET_RT_CLS_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_rt_cls_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter rt_cls_ndx after itm_queue_test_tmplt_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_QUEUE_TEST_TMPLT_GET_ITM_QUEUE_TEST_TMPLT_GET_DROP_PRECEDENCE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_QUEUE_TEST_TMPLT_GET_ITM_QUEUE_TEST_TMPLT_GET_DROP_PRECEDENCE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_drop_precedence_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter drop_precedence_ndx after itm_queue_test_tmplt_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_itm_queue_test_tmplt_get(
          unit,
          prm_rt_cls_ndx,
          prm_drop_precedence_ndx,
          &prm_test_tmplt
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_queue_test_tmplt_get");
    goto exit;
  }

  soc_sand_os_printf( "test_tmplt: %s\n\r",TMD_ITM_ADMIT_TSTS_to_string(prm_test_tmplt));


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: itm_wred_exp_wq_set (section ingress_traffic_mgmt)
 */
int
  ui_tmd_api_ingress_traffic_mgmt_itm_wred_exp_wq_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_rt_cls_ndx = 0;
  uint32
    prm_exp_wq;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_traffic_mgmt");
  soc_sand_proc_name = "tmd_itm_wred_exp_wq_set";

  unit = tmd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_WRED_EXP_WQ_SET_ITM_WRED_EXP_WQ_SET_RT_CLS_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_WRED_EXP_WQ_SET_ITM_WRED_EXP_WQ_SET_RT_CLS_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_rt_cls_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter rt_cls_ndx after itm_wred_exp_wq_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = tmd_itm_wred_exp_wq_get(
          unit,
          prm_rt_cls_ndx,
          &prm_exp_wq
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_wred_exp_wq_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_WRED_EXP_WQ_SET_ITM_WRED_EXP_WQ_SET_EXP_WQ_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_WRED_EXP_WQ_SET_ITM_WRED_EXP_WQ_SET_EXP_WQ_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_exp_wq = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_itm_wred_exp_wq_set(
          unit,
          prm_rt_cls_ndx,
          prm_exp_wq
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_wred_exp_wq_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: itm_wred_exp_wq_get (section ingress_traffic_mgmt)
 */
int
  ui_tmd_api_ingress_traffic_mgmt_itm_wred_exp_wq_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_rt_cls_ndx = 0;
  uint32
    prm_exp_wq;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_traffic_mgmt");
  soc_sand_proc_name = "tmd_itm_wred_exp_wq_get";

  unit = tmd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_WRED_EXP_WQ_GET_ITM_WRED_EXP_WQ_GET_RT_CLS_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_WRED_EXP_WQ_GET_ITM_WRED_EXP_WQ_GET_RT_CLS_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_rt_cls_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter rt_cls_ndx after itm_wred_exp_wq_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_itm_wred_exp_wq_get(
          unit,
          prm_rt_cls_ndx,
          &prm_exp_wq
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_wred_exp_wq_get");
    goto exit;
  }

  soc_sand_os_printf( "exp_wq: %lu\n\r",prm_exp_wq);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: itm_wred_set (section ingress_traffic_mgmt)
 */
int
  ui_tmd_api_ingress_traffic_mgmt_itm_wred_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_rt_cls_ndx = 0;
  uint32
    prm_drop_precedence_ndx = 0;
  TMD_ITM_WRED_QT_DP_INFO
    prm_info;
  TMD_ITM_WRED_QT_DP_INFO
    prm_exact_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_traffic_mgmt");
  soc_sand_proc_name = "tmd_itm_wred_set";

  unit = tmd_get_default_unit();
  TMD_ITM_WRED_QT_DP_INFO_clear(&prm_info);
  TMD_ITM_WRED_QT_DP_INFO_clear(&prm_exact_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_WRED_SET_ITM_WRED_SET_RT_CLS_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_WRED_SET_ITM_WRED_SET_RT_CLS_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_rt_cls_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter rt_cls_ndx after itm_wred_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_WRED_SET_ITM_WRED_SET_DROP_PRECEDENCE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_WRED_SET_ITM_WRED_SET_DROP_PRECEDENCE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_drop_precedence_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter drop_precedence_ndx after itm_wred_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = tmd_itm_wred_get(
          unit,
          prm_rt_cls_ndx,
          prm_drop_precedence_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_wred_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_WRED_SET_ITM_WRED_SET_INFO_MAX_PROBABILITY_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_WRED_SET_ITM_WRED_SET_INFO_MAX_PROBABILITY_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.max_probability = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_WRED_SET_ITM_WRED_SET_INFO_MAX_PACKET_SIZE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_WRED_SET_ITM_WRED_SET_INFO_MAX_PACKET_SIZE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.max_packet_size = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_WRED_SET_ITM_WRED_SET_INFO_MAX_AVRG_TH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_WRED_SET_ITM_WRED_SET_INFO_MAX_AVRG_TH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.max_avrg_th = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_WRED_SET_ITM_WRED_SET_INFO_MIN_AVRG_TH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_WRED_SET_ITM_WRED_SET_INFO_MIN_AVRG_TH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.min_avrg_th = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_WRED_SET_ITM_WRED_SET_INFO_IGNORE_PACKET_SIZE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_WRED_SET_ITM_WRED_SET_INFO_IGNORE_PACKET_SIZE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.ignore_packet_size = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_WRED_SET_ITM_WRED_SET_INFO_WRED_EN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_WRED_SET_ITM_WRED_SET_INFO_WRED_EN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.wred_en = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_itm_wred_set(
          unit,
          prm_rt_cls_ndx,
          prm_drop_precedence_ndx,
          &prm_info,
          &prm_exact_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_wred_set");
    goto exit;
  }

  send_string_to_screen("--> exact_info:", TRUE);
  TMD_ITM_WRED_QT_DP_INFO_print(&prm_exact_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: itm_wred_get (section ingress_traffic_mgmt)
 */
int
  ui_tmd_api_ingress_traffic_mgmt_itm_wred_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_rt_cls_ndx = 0;
  uint32
    prm_drop_precedence_ndx = 0;
  TMD_ITM_WRED_QT_DP_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_traffic_mgmt");
  soc_sand_proc_name = "tmd_itm_wred_get";

  unit = tmd_get_default_unit();
  TMD_ITM_WRED_QT_DP_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_WRED_GET_ITM_WRED_GET_RT_CLS_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_WRED_GET_ITM_WRED_GET_RT_CLS_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_rt_cls_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter rt_cls_ndx after itm_wred_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_WRED_GET_ITM_WRED_GET_DROP_PRECEDENCE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_WRED_GET_ITM_WRED_GET_DROP_PRECEDENCE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_drop_precedence_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter drop_precedence_ndx after itm_wred_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_itm_wred_get(
          unit,
          prm_rt_cls_ndx,
          prm_drop_precedence_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_wred_get");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  TMD_ITM_WRED_QT_DP_INFO_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: itm_tail_drop_set (section ingress_traffic_mgmt)
 */
int
  ui_tmd_api_ingress_traffic_mgmt_itm_tail_drop_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_rt_cls_ndx = 0;
  uint32
    prm_drop_precedence_ndx = 0;
  TMD_ITM_TAIL_DROP_INFO
    prm_info;
  TMD_ITM_TAIL_DROP_INFO
    prm_exact_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_traffic_mgmt");
  soc_sand_proc_name = "tmd_itm_tail_drop_set";

  unit = tmd_get_default_unit();
  TMD_ITM_TAIL_DROP_INFO_clear(&prm_info);
  TMD_ITM_TAIL_DROP_INFO_clear(&prm_exact_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_TAIL_DROP_SET_ITM_TAIL_DROP_SET_RT_CLS_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_TAIL_DROP_SET_ITM_TAIL_DROP_SET_RT_CLS_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_rt_cls_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter rt_cls_ndx after itm_tail_drop_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_TAIL_DROP_SET_ITM_TAIL_DROP_SET_DROP_PRECEDENCE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_TAIL_DROP_SET_ITM_TAIL_DROP_SET_DROP_PRECEDENCE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_drop_precedence_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter drop_precedence_ndx after itm_tail_drop_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = tmd_itm_tail_drop_get(
          unit,
          prm_rt_cls_ndx,
          prm_drop_precedence_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_tail_drop_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_TAIL_DROP_SET_ITM_TAIL_DROP_SET_INFO_MAX_INST_Q_SIZE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_TAIL_DROP_SET_ITM_TAIL_DROP_SET_INFO_MAX_INST_Q_SIZE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.max_inst_q_size = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_itm_tail_drop_set(
          unit,
          prm_rt_cls_ndx,
          prm_drop_precedence_ndx,
          &prm_info,
          &prm_exact_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_tail_drop_set");
    goto exit;
  }

  send_string_to_screen("--> exact_info:", TRUE);
  TMD_ITM_TAIL_DROP_INFO_print(&prm_exact_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: itm_tail_drop_get (section ingress_traffic_mgmt)
 */
int
  ui_tmd_api_ingress_traffic_mgmt_itm_tail_drop_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_rt_cls_ndx = 0;
  uint32
    prm_drop_precedence_ndx = 0;
  TMD_ITM_TAIL_DROP_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_traffic_mgmt");
  soc_sand_proc_name = "tmd_itm_tail_drop_get";

  unit = tmd_get_default_unit();
  TMD_ITM_TAIL_DROP_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_TAIL_DROP_GET_ITM_TAIL_DROP_GET_RT_CLS_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_TAIL_DROP_GET_ITM_TAIL_DROP_GET_RT_CLS_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_rt_cls_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter rt_cls_ndx after itm_tail_drop_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_TAIL_DROP_GET_ITM_TAIL_DROP_GET_DROP_PRECEDENCE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_TAIL_DROP_GET_ITM_TAIL_DROP_GET_DROP_PRECEDENCE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_drop_precedence_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter drop_precedence_ndx after itm_tail_drop_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_itm_tail_drop_get(
          unit,
          prm_rt_cls_ndx,
          prm_drop_precedence_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_tail_drop_get");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  TMD_ITM_TAIL_DROP_INFO_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: itm_cr_wd_set (section ingress_traffic_mgmt)
 */
int
  ui_tmd_api_ingress_traffic_mgmt_itm_cr_wd_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_ITM_CR_WD_INFO
    prm_info;
  TMD_ITM_CR_WD_INFO
    prm_exact_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_traffic_mgmt");
  soc_sand_proc_name = "tmd_itm_cr_wd_set";

  unit = tmd_get_default_unit();
  TMD_ITM_CR_WD_INFO_clear(&prm_info);
  TMD_ITM_CR_WD_INFO_clear(&prm_exact_info);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = tmd_itm_cr_wd_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_cr_wd_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_CR_WD_SET_ITM_CR_WD_SET_INFO_TOP_QUEUE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_CR_WD_SET_ITM_CR_WD_SET_INFO_TOP_QUEUE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.top_queue = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_CR_WD_SET_ITM_CR_WD_SET_INFO_BOTTOM_QUEUE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_CR_WD_SET_ITM_CR_WD_SET_INFO_BOTTOM_QUEUE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.bottom_queue = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_CR_WD_SET_ITM_CR_WD_SET_INFO_MAX_FLOW_MSG_GEN_RATE_NANO_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_CR_WD_SET_ITM_CR_WD_SET_INFO_MAX_FLOW_MSG_GEN_RATE_NANO_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.max_flow_msg_gen_rate_nano = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_CR_WD_SET_ITM_CR_WD_SET_INFO_MIN_SCAN_CYCLE_PERIOD_MICRO_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_CR_WD_SET_ITM_CR_WD_SET_INFO_MIN_SCAN_CYCLE_PERIOD_MICRO_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.min_scan_cycle_period_micro = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_itm_cr_wd_set(
          unit,
          &prm_info,
          &prm_exact_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_cr_wd_set");
    goto exit;
  }

  send_string_to_screen("--> exact_info:", TRUE);
  TMD_ITM_CR_WD_INFO_print(&prm_exact_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: itm_cr_wd_get (section ingress_traffic_mgmt)
 */
int
  ui_tmd_api_ingress_traffic_mgmt_itm_cr_wd_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_ITM_CR_WD_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_traffic_mgmt");
  soc_sand_proc_name = "tmd_itm_cr_wd_get";

  unit = tmd_get_default_unit();
  TMD_ITM_CR_WD_INFO_clear(&prm_info);

  /* Get parameters */

  /* Call function */
  ret = tmd_itm_cr_wd_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_cr_wd_get");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  TMD_ITM_CR_WD_INFO_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: itm_vsq_qt_rt_cls_set (section ingress_traffic_mgmt)
 */
int
  ui_tmd_api_ingress_traffic_mgmt_itm_vsq_qt_rt_cls_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_ITM_VSQ_GROUP
    prm_vsq_group_ndx = 0;
  TMD_ITM_VSQ_NDX
    prm_vsq_in_group_ndx = 0;
  uint32
    prm_vsq_rt_cls;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_traffic_mgmt");
  soc_sand_proc_name = "tmd_itm_vsq_qt_rt_cls_set";

  unit = tmd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_VSQ_QT_RT_CLS_SET_ITM_VSQ_QT_RT_CLS_SET_VSQ_GROUP_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_ITM_VSQ_QT_RT_CLS_SET_ITM_VSQ_QT_RT_CLS_SET_VSQ_GROUP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vsq_group_ndx = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vsq_group_ndx after itm_vsq_qt_rt_cls_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_VSQ_QT_RT_CLS_SET_ITM_VSQ_QT_RT_CLS_SET_VSQ_IN_GROUP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_VSQ_QT_RT_CLS_SET_ITM_VSQ_QT_RT_CLS_SET_VSQ_IN_GROUP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vsq_in_group_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vsq_in_group_ndx after itm_vsq_qt_rt_cls_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = tmd_itm_vsq_qt_rt_cls_get(
          unit,
          prm_vsq_group_ndx,
          prm_vsq_in_group_ndx,
          &prm_vsq_rt_cls
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_vsq_qt_rt_cls_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_VSQ_QT_RT_CLS_SET_ITM_VSQ_QT_RT_CLS_SET_VSQ_RT_CLS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_VSQ_QT_RT_CLS_SET_ITM_VSQ_QT_RT_CLS_SET_VSQ_RT_CLS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vsq_rt_cls = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_itm_vsq_qt_rt_cls_set(
          unit,
          prm_vsq_group_ndx,
          prm_vsq_in_group_ndx,
          prm_vsq_rt_cls
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_vsq_qt_rt_cls_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: itm_vsq_qt_rt_cls_get (section ingress_traffic_mgmt)
 */
int
  ui_tmd_api_ingress_traffic_mgmt_itm_vsq_qt_rt_cls_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_ITM_VSQ_GROUP
    prm_vsq_group_ndx = 0;
  TMD_ITM_VSQ_NDX
    prm_vsq_in_group_ndx = 0;
  uint32
    prm_vsq_rt_cls;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_traffic_mgmt");
  soc_sand_proc_name = "tmd_itm_vsq_qt_rt_cls_get";

  unit = tmd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_VSQ_QT_RT_CLS_GET_ITM_VSQ_QT_RT_CLS_GET_VSQ_GROUP_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_ITM_VSQ_QT_RT_CLS_GET_ITM_VSQ_QT_RT_CLS_GET_VSQ_GROUP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vsq_group_ndx = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vsq_group_ndx after itm_vsq_qt_rt_cls_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_VSQ_QT_RT_CLS_GET_ITM_VSQ_QT_RT_CLS_GET_VSQ_IN_GROUP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_VSQ_QT_RT_CLS_GET_ITM_VSQ_QT_RT_CLS_GET_VSQ_IN_GROUP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vsq_in_group_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vsq_in_group_ndx after itm_vsq_qt_rt_cls_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_itm_vsq_qt_rt_cls_get(
          unit,
          prm_vsq_group_ndx,
          prm_vsq_in_group_ndx,
          &prm_vsq_rt_cls
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_vsq_qt_rt_cls_get");
    goto exit;
  }

  soc_sand_os_printf( "vsq_rt_cls: %d\n\r",prm_vsq_rt_cls);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: itm_vsq_fc_set (section ingress_traffic_mgmt)
 */
int
  ui_tmd_api_ingress_traffic_mgmt_itm_vsq_fc_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_ITM_VSQ_GROUP
    prm_vsq_group_ndx = 0;
  uint32
    prm_vsq_rt_cls_ndx = 0;
  TMD_ITM_VSQ_FC_INFO
    prm_info;
  TMD_ITM_VSQ_FC_INFO
    prm_exact_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_traffic_mgmt");
  soc_sand_proc_name = "tmd_itm_vsq_fc_set";

  unit = tmd_get_default_unit();
  TMD_ITM_VSQ_FC_INFO_clear(&prm_info);
  TMD_ITM_VSQ_FC_INFO_clear(&prm_exact_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_VSQ_FC_SET_ITM_VSQ_FC_SET_VSQ_GROUP_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_ITM_VSQ_FC_SET_ITM_VSQ_FC_SET_VSQ_GROUP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vsq_group_ndx = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vsq_group_ndx after itm_vsq_fc_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_VSQ_FC_SET_ITM_VSQ_FC_SET_VSQ_RT_CLS_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_VSQ_FC_SET_ITM_VSQ_FC_SET_VSQ_RT_CLS_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vsq_rt_cls_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vsq_rt_cls_ndx after itm_vsq_fc_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = tmd_itm_vsq_fc_get(
          unit,
          prm_vsq_group_ndx,
          prm_vsq_rt_cls_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_vsq_fc_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_VSQ_FC_SET_ITM_VSQ_FC_SET_INFO_BD_SIZE_FC_CLEAR_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_VSQ_FC_SET_ITM_VSQ_FC_SET_INFO_BD_SIZE_FC_CLEAR_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.bd_size_fc.clear = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_VSQ_FC_SET_ITM_VSQ_FC_SET_INFO_BD_SIZE_FC_SET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_VSQ_FC_SET_ITM_VSQ_FC_SET_INFO_BD_SIZE_FC_SET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.bd_size_fc.set = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_VSQ_FC_SET_ITM_VSQ_FC_SET_INFO_Q_SIZE_FC_CLEAR_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_VSQ_FC_SET_ITM_VSQ_FC_SET_INFO_Q_SIZE_FC_CLEAR_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.q_size_fc.clear = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_VSQ_FC_SET_ITM_VSQ_FC_SET_INFO_Q_SIZE_FC_SET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_VSQ_FC_SET_ITM_VSQ_FC_SET_INFO_Q_SIZE_FC_SET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.q_size_fc.set = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_itm_vsq_fc_set(
          unit,
          prm_vsq_group_ndx,
          prm_vsq_rt_cls_ndx,
          &prm_info,
          &prm_exact_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_vsq_fc_set");
    goto exit;
  }

  send_string_to_screen("--> exact_info:", TRUE);
  TMD_ITM_VSQ_FC_INFO_print(&prm_exact_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: itm_vsq_fc_get (section ingress_traffic_mgmt)
 */
int
  ui_tmd_api_ingress_traffic_mgmt_itm_vsq_fc_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_ITM_VSQ_GROUP
    prm_vsq_group_ndx = 0;
  uint32
    prm_vsq_rt_cls_ndx = 0;
  TMD_ITM_VSQ_FC_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_traffic_mgmt");
  soc_sand_proc_name = "tmd_itm_vsq_fc_get";

  unit = tmd_get_default_unit();
  TMD_ITM_VSQ_FC_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_VSQ_FC_GET_ITM_VSQ_FC_GET_VSQ_GROUP_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_ITM_VSQ_FC_GET_ITM_VSQ_FC_GET_VSQ_GROUP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vsq_group_ndx = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vsq_group_ndx after itm_vsq_fc_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_VSQ_FC_GET_ITM_VSQ_FC_GET_VSQ_RT_CLS_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_VSQ_FC_GET_ITM_VSQ_FC_GET_VSQ_RT_CLS_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vsq_rt_cls_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vsq_rt_cls_ndx after itm_vsq_fc_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_itm_vsq_fc_get(
          unit,
          prm_vsq_group_ndx,
          prm_vsq_rt_cls_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_vsq_fc_get");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  TMD_ITM_VSQ_FC_INFO_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: itm_vsq_tail_drop_set (section ingress_traffic_mgmt)
 */
int
  ui_tmd_api_ingress_traffic_mgmt_itm_vsq_tail_drop_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_ITM_VSQ_GROUP
    prm_vsq_group_ndx = 0;
  uint32
    prm_vsq_rt_cls_ndx = 0;
  uint32
    prm_drop_precedence_ndx = 0;
  TMD_ITM_VSQ_TAIL_DROP_INFO
    prm_info;
  TMD_ITM_VSQ_TAIL_DROP_INFO
    prm_exact_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_traffic_mgmt");
  soc_sand_proc_name = "tmd_itm_vsq_tail_drop_set";

  unit = tmd_get_default_unit();
  TMD_ITM_VSQ_TAIL_DROP_INFO_clear(&prm_info);
  TMD_ITM_VSQ_TAIL_DROP_INFO_clear(&prm_exact_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_VSQ_TAIL_DROP_SET_ITM_VSQ_TAIL_DROP_SET_VSQ_GROUP_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_ITM_VSQ_TAIL_DROP_SET_ITM_VSQ_TAIL_DROP_SET_VSQ_GROUP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vsq_group_ndx = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vsq_group_ndx after itm_vsq_tail_drop_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_VSQ_TAIL_DROP_SET_ITM_VSQ_TAIL_DROP_SET_VSQ_RT_CLS_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_VSQ_TAIL_DROP_SET_ITM_VSQ_TAIL_DROP_SET_VSQ_RT_CLS_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vsq_rt_cls_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vsq_rt_cls_ndx after itm_vsq_tail_drop_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_VSQ_TAIL_DROP_SET_ITM_VSQ_TAIL_DROP_SET_DROP_PRECEDENCE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_VSQ_TAIL_DROP_SET_ITM_VSQ_TAIL_DROP_SET_DROP_PRECEDENCE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_drop_precedence_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter drop_precedence_ndx after itm_vsq_tail_drop_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = tmd_itm_vsq_tail_drop_get(
          unit,
          prm_vsq_group_ndx,
          prm_vsq_rt_cls_ndx,
          prm_drop_precedence_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_vsq_tail_drop_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_VSQ_TAIL_DROP_SET_ITM_VSQ_TAIL_DROP_SET_INFO_MAX_INST_Q_SIZE_BDS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_VSQ_TAIL_DROP_SET_ITM_VSQ_TAIL_DROP_SET_INFO_MAX_INST_Q_SIZE_BDS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.max_inst_q_size_bds = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_VSQ_TAIL_DROP_SET_ITM_VSQ_TAIL_DROP_SET_INFO_MAX_INST_Q_SIZE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_VSQ_TAIL_DROP_SET_ITM_VSQ_TAIL_DROP_SET_INFO_MAX_INST_Q_SIZE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.max_inst_q_size = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_itm_vsq_tail_drop_set(
          unit,
          prm_vsq_group_ndx,
          prm_vsq_rt_cls_ndx,
          prm_drop_precedence_ndx,
          &prm_info,
          &prm_exact_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_vsq_tail_drop_set");
    goto exit;
  }

  send_string_to_screen("--> exact_info:", TRUE);
  TMD_ITM_VSQ_TAIL_DROP_INFO_print(&prm_exact_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: itm_vsq_tail_drop_get (section ingress_traffic_mgmt)
 */
int
  ui_tmd_api_ingress_traffic_mgmt_itm_vsq_tail_drop_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_ITM_VSQ_GROUP
    prm_vsq_group_ndx = 0;
  uint32
    prm_vsq_rt_cls_ndx = 0;
  uint32
    prm_drop_precedence_ndx = 0;
  TMD_ITM_VSQ_TAIL_DROP_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_traffic_mgmt");
  soc_sand_proc_name = "tmd_itm_vsq_tail_drop_get";

  unit = tmd_get_default_unit();
  TMD_ITM_VSQ_TAIL_DROP_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_VSQ_TAIL_DROP_GET_ITM_VSQ_TAIL_DROP_GET_VSQ_GROUP_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_ITM_VSQ_TAIL_DROP_GET_ITM_VSQ_TAIL_DROP_GET_VSQ_GROUP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vsq_group_ndx = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vsq_group_ndx after itm_vsq_tail_drop_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_VSQ_TAIL_DROP_GET_ITM_VSQ_TAIL_DROP_GET_VSQ_RT_CLS_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_VSQ_TAIL_DROP_GET_ITM_VSQ_TAIL_DROP_GET_VSQ_RT_CLS_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vsq_rt_cls_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vsq_rt_cls_ndx after itm_vsq_tail_drop_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_VSQ_TAIL_DROP_GET_ITM_VSQ_TAIL_DROP_GET_DROP_PRECEDENCE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_VSQ_TAIL_DROP_GET_ITM_VSQ_TAIL_DROP_GET_DROP_PRECEDENCE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_drop_precedence_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter drop_precedence_ndx after itm_vsq_tail_drop_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_itm_vsq_tail_drop_get(
          unit,
          prm_vsq_group_ndx,
          prm_vsq_rt_cls_ndx,
          prm_drop_precedence_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_vsq_tail_drop_get");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  TMD_ITM_VSQ_TAIL_DROP_INFO_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: itm_vsq_wred_gen_set (section ingress_traffic_mgmt)
 */
int
  ui_tmd_api_ingress_traffic_mgmt_itm_vsq_wred_gen_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_ITM_VSQ_GROUP
    prm_vsq_group_ndx = 0;
  uint32
    prm_vsq_rt_cls_ndx = 0;
  TMD_ITM_VSQ_WRED_GEN_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_traffic_mgmt");
  soc_sand_proc_name = "tmd_itm_vsq_wred_gen_set";

  unit = tmd_get_default_unit();
  TMD_ITM_VSQ_WRED_GEN_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_VSQ_WRED_GEN_SET_ITM_VSQ_WRED_GEN_SET_VSQ_GROUP_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_ITM_VSQ_WRED_GEN_SET_ITM_VSQ_WRED_GEN_SET_VSQ_GROUP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vsq_group_ndx = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vsq_group_ndx after itm_vsq_wred_gen_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_VSQ_WRED_GEN_SET_ITM_VSQ_WRED_GEN_SET_VSQ_RT_CLS_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_VSQ_WRED_GEN_SET_ITM_VSQ_WRED_GEN_SET_VSQ_RT_CLS_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vsq_rt_cls_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vsq_rt_cls_ndx after itm_vsq_wred_gen_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = tmd_itm_vsq_wred_gen_get(
          unit,
          prm_vsq_group_ndx,
          prm_vsq_rt_cls_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_vsq_wred_gen_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_VSQ_WRED_GEN_SET_ITM_VSQ_WRED_GEN_SET_INFO_EXP_WQ_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_VSQ_WRED_GEN_SET_ITM_VSQ_WRED_GEN_SET_INFO_EXP_WQ_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.exp_wq = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_VSQ_WRED_GEN_SET_ITM_VSQ_WRED_GEN_SET_INFO_WRED_EN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_VSQ_WRED_GEN_SET_ITM_VSQ_WRED_GEN_SET_INFO_WRED_EN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.wred_en = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_itm_vsq_wred_gen_set(
          unit,
          prm_vsq_group_ndx,
          prm_vsq_rt_cls_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_vsq_wred_gen_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: itm_vsq_wred_gen_get (section ingress_traffic_mgmt)
 */
int
  ui_tmd_api_ingress_traffic_mgmt_itm_vsq_wred_gen_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_ITM_VSQ_GROUP
    prm_vsq_group_ndx = 0;
  uint32
    prm_vsq_rt_cls_ndx = 0;
  TMD_ITM_VSQ_WRED_GEN_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_traffic_mgmt");
  soc_sand_proc_name = "tmd_itm_vsq_wred_gen_get";

  unit = tmd_get_default_unit();
  TMD_ITM_VSQ_WRED_GEN_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_VSQ_WRED_GEN_GET_ITM_VSQ_WRED_GEN_GET_VSQ_GROUP_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_ITM_VSQ_WRED_GEN_GET_ITM_VSQ_WRED_GEN_GET_VSQ_GROUP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vsq_group_ndx = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vsq_group_ndx after itm_vsq_wred_gen_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_VSQ_WRED_GEN_GET_ITM_VSQ_WRED_GEN_GET_VSQ_RT_CLS_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_VSQ_WRED_GEN_GET_ITM_VSQ_WRED_GEN_GET_VSQ_RT_CLS_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vsq_rt_cls_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vsq_rt_cls_ndx after itm_vsq_wred_gen_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_itm_vsq_wred_gen_get(
          unit,
          prm_vsq_group_ndx,
          prm_vsq_rt_cls_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_vsq_wred_gen_get");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  TMD_ITM_VSQ_WRED_GEN_INFO_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: itm_vsq_wred_set (section ingress_traffic_mgmt)
 */
int
  ui_tmd_api_ingress_traffic_mgmt_itm_vsq_wred_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_ITM_VSQ_GROUP
    prm_vsq_group_ndx = 0;
  uint32
    prm_vsq_rt_cls_ndx = 0;
  uint32
    prm_drop_precedence_ndx = 0;
  TMD_ITM_WRED_QT_DP_INFO
    prm_info;
  TMD_ITM_WRED_QT_DP_INFO
    prm_exact_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_traffic_mgmt");
  soc_sand_proc_name = "tmd_itm_vsq_wred_set";

  unit = tmd_get_default_unit();
  TMD_ITM_WRED_QT_DP_INFO_clear(&prm_info);
  TMD_ITM_WRED_QT_DP_INFO_clear(&prm_exact_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_VSQ_WRED_SET_ITM_VSQ_WRED_SET_VSQ_GROUP_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_ITM_VSQ_WRED_SET_ITM_VSQ_WRED_SET_VSQ_GROUP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vsq_group_ndx = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vsq_group_ndx after itm_vsq_wred_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_VSQ_WRED_SET_ITM_VSQ_WRED_SET_VSQ_RT_CLS_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_VSQ_WRED_SET_ITM_VSQ_WRED_SET_VSQ_RT_CLS_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vsq_rt_cls_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vsq_rt_cls_ndx after itm_vsq_wred_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_VSQ_WRED_SET_ITM_VSQ_WRED_SET_DROP_PRECEDENCE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_VSQ_WRED_SET_ITM_VSQ_WRED_SET_DROP_PRECEDENCE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_drop_precedence_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter drop_precedence_ndx after itm_vsq_wred_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = tmd_itm_vsq_wred_get(
          unit,
          prm_vsq_group_ndx,
          prm_vsq_rt_cls_ndx,
          prm_drop_precedence_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_vsq_wred_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_VSQ_WRED_SET_ITM_VSQ_WRED_SET_INFO_MAX_PROBABILITY_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_VSQ_WRED_SET_ITM_VSQ_WRED_SET_INFO_MAX_PROBABILITY_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.max_probability = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_VSQ_WRED_SET_ITM_VSQ_WRED_SET_INFO_MAX_PACKET_SIZE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_VSQ_WRED_SET_ITM_VSQ_WRED_SET_INFO_MAX_PACKET_SIZE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.max_packet_size = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_VSQ_WRED_SET_ITM_VSQ_WRED_SET_INFO_MAX_AVRG_TH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_VSQ_WRED_SET_ITM_VSQ_WRED_SET_INFO_MAX_AVRG_TH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.max_avrg_th = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_VSQ_WRED_SET_ITM_VSQ_WRED_SET_INFO_MIN_AVRG_TH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_VSQ_WRED_SET_ITM_VSQ_WRED_SET_INFO_MIN_AVRG_TH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.min_avrg_th = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_VSQ_WRED_SET_ITM_VSQ_WRED_SET_INFO_IGNORE_PACKET_SIZE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_VSQ_WRED_SET_ITM_VSQ_WRED_SET_INFO_IGNORE_PACKET_SIZE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.ignore_packet_size = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_VSQ_WRED_SET_ITM_VSQ_WRED_SET_INFO_WRED_EN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_VSQ_WRED_SET_ITM_VSQ_WRED_SET_INFO_WRED_EN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.wred_en = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_itm_vsq_wred_set(
          unit,
          prm_vsq_group_ndx,
          prm_vsq_rt_cls_ndx,
          prm_drop_precedence_ndx,
          &prm_info,
          &prm_exact_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_vsq_wred_set");
    goto exit;
  }

  send_string_to_screen("--> exact_info:", TRUE);
  TMD_ITM_WRED_QT_DP_INFO_print(&prm_exact_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: itm_vsq_wred_get (section ingress_traffic_mgmt)
 */
int
  ui_tmd_api_ingress_traffic_mgmt_itm_vsq_wred_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_ITM_VSQ_GROUP
    prm_vsq_group_ndx = 0;
  uint32
    prm_vsq_rt_cls_ndx = 0;
  uint32
    prm_drop_precedence_ndx = 0;
  TMD_ITM_WRED_QT_DP_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_traffic_mgmt");
  soc_sand_proc_name = "tmd_itm_vsq_wred_get";

  unit = tmd_get_default_unit();
  TMD_ITM_WRED_QT_DP_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_VSQ_WRED_GET_ITM_VSQ_WRED_GET_VSQ_GROUP_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_ITM_VSQ_WRED_GET_ITM_VSQ_WRED_GET_VSQ_GROUP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vsq_group_ndx = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vsq_group_ndx after itm_vsq_wred_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_VSQ_WRED_GET_ITM_VSQ_WRED_GET_VSQ_RT_CLS_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_VSQ_WRED_GET_ITM_VSQ_WRED_GET_VSQ_RT_CLS_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vsq_rt_cls_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vsq_rt_cls_ndx after itm_vsq_wred_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_VSQ_WRED_GET_ITM_VSQ_WRED_GET_DROP_PRECEDENCE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_VSQ_WRED_GET_ITM_VSQ_WRED_GET_DROP_PRECEDENCE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_drop_precedence_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter drop_precedence_ndx after itm_vsq_wred_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_itm_vsq_wred_get(
          unit,
          prm_vsq_group_ndx,
          prm_vsq_rt_cls_ndx,
          prm_drop_precedence_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_vsq_wred_get");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  TMD_ITM_WRED_QT_DP_INFO_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

#ifdef UI_UNDEFED_TEMP
/********************************************************************
 *  Function handler: itm_vsq_stt_tag_set (section ingress_traffic_mgmt)
 */
int
  ui_tmd_api_ingress_traffic_mgmt_itm_stag_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  TMD_ITM_STAG_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_traffic_mgmt");
  soc_sand_proc_name = "tmd_itm_stag_set";

  unit = tmd_get_default_unit();
  TMD_ITM_STAG_INFO_clear(&prm_info);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = tmd_itm_stag_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** tmd_itm_stag_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "tmd_itm_stag_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_STAG_SET_ITM_STAG_SET_INFO_DROPP_LSB_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_STAG_SET_ITM_STAG_SET_INFO_DROPP_LSB_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.dropp_lsb = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_STAG_SET_ITM_STAG_SET_INFO_DROPP_EN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_STAG_SET_ITM_STAG_SET_INFO_DROPP_EN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.dropp_en = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_STAG_SET_ITM_STAG_SET_INFO_VSQ_INDEX_LSB_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_STAG_SET_ITM_STAG_SET_INFO_VSQ_INDEX_LSB_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.vsq_index_lsb = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_STAG_SET_ITM_STAG_SET_INFO_VSQ_INDEX_MSB_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_STAG_SET_ITM_STAG_SET_INFO_VSQ_INDEX_MSB_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.vsq_index_msb = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_STAG_SET_ITM_STAG_SET_INFO_ENABLE_MODE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_ITM_STAG_SET_ITM_STAG_SET_INFO_ENABLE_MODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.enable_mode = param_val->numeric_equivalent;
  }


  /* Call function */
  ret = tmd_itm_stag_set(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** tmd_itm_stag_set - FAIL", TRUE);
    soc_petra_disp_result(ret, "tmd_itm_stag_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: itm_stag_get (section ingress_traffic_mgmt)
 */
int
  ui_tmd_api_ingress_traffic_mgmt_itm_stag_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  TMD_ITM_STAG_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_traffic_mgmt");
  soc_sand_proc_name = "tmd_itm_stag_get";

  unit = tmd_get_default_unit();
  TMD_ITM_STAG_INFO_clear(&prm_info);

  /* Get parameters */

  /* Call function */
  ret = tmd_itm_stag_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** tmd_itm_stag_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "tmd_itm_stag_get");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  TMD_ITM_STAG_INFO_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: itm_vsq_counter_set (section ingress_traffic_mgmt)
 */
int
  ui_tmd_api_ingress_traffic_mgmt_itm_vsq_counter_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  TMD_ITM_VSQ_GROUP
    prm_vsq_group_ndx;
  TMD_ITM_VSQ_NDX
    prm_vsq_in_group_ndx;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_traffic_mgmt");
  soc_sand_proc_name = "tmd_itm_vsq_counter_set";

  unit = tmd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_VSQ_COUNTER_SET_ITM_VSQ_COUNTER_SET_VSQ_GROUP_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_ITM_VSQ_COUNTER_SET_ITM_VSQ_COUNTER_SET_VSQ_GROUP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vsq_group_ndx = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vsq_group_ndx after itm_vsq_counter_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_VSQ_COUNTER_SET_ITM_VSQ_COUNTER_SET_VSQ_IN_GROUP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_VSQ_COUNTER_SET_ITM_VSQ_COUNTER_SET_VSQ_IN_GROUP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vsq_in_group_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vsq_in_group_ndx after itm_vsq_counter_set***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_itm_vsq_counter_set(
          unit,
          prm_vsq_group_ndx,
          prm_vsq_in_group_ndx
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** tmd_itm_vsq_counter_set - FAIL", TRUE);
    soc_petra_disp_result(ret, "tmd_itm_vsq_counter_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: itm_vsq_counter_get (section ingress_traffic_mgmt)
 */
int
  ui_tmd_api_ingress_traffic_mgmt_itm_vsq_counter_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  TMD_ITM_VSQ_GROUP
    prm_vsq_group_ndx;
  TMD_ITM_VSQ_NDX
    vsq_glbl_ndx,
    prm_vsq_in_group_ndx;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_traffic_mgmt");
  soc_sand_proc_name = "tmd_itm_vsq_counter_get";

  unit = tmd_get_default_unit();

  /* Call function */
  ret = tmd_itm_vsq_counter_get(
          unit,
          &prm_vsq_group_ndx,
          &prm_vsq_in_group_ndx
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** tmd_itm_vsq_counter_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "tmd_itm_vsq_counter_get");
    goto exit;
  }

  ret = tmd_itm_vsq_index_group2global(
          prm_vsq_group_ndx,
          prm_vsq_in_group_ndx,
          &vsq_glbl_ndx
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** tmd_itm_vsq_index_group2global - FAIL", TRUE);
    soc_petra_disp_result(ret, "tmd_itm_vsq_index_group2global");
    goto exit;
  }

  send_string_to_screen("--> VSQ Counter:", TRUE);
  soc_sand_os_printf( "Group: %s, in-group index %u, VSQ %u\n\r",TMD_ITM_VSQ_GROUP_to_string(prm_vsq_group_ndx), prm_vsq_in_group_ndx, vsq_glbl_ndx);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: itm_vsq_counter_read (section ingress_traffic_mgmt)
 */
int
  ui_tmd_api_ingress_traffic_mgmt_itm_vsq_counter_read(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_pckt_count;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_traffic_mgmt");
  soc_sand_proc_name = "tmd_itm_vsq_counter_read";

  unit = tmd_get_default_unit();

  /* Get parameters */

  /* Call function */
  ret = tmd_itm_vsq_counter_read(
          unit,
          &prm_pckt_count
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** tmd_itm_vsq_counter_read - FAIL", TRUE);
    soc_petra_disp_result(ret, "tmd_itm_vsq_counter_read");
    goto exit;
  }

  send_string_to_screen("--> pckt_count:", TRUE);
  soc_sand_os_printf( "pckt_count: %lu\n\r",prm_pckt_count);


  goto exit;
exit:
  return ui_ret;
}
#endif /* UI_UNDEFED_TEMP */

/********************************************************************
 *  Function handler: itm_queue_info_set (section ingress_traffic_mgmt)
 */
int
  ui_tmd_api_ingress_traffic_mgmt_itm_queue_info_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_queue_ndx = 0;
  TMD_ITM_QUEUE_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_traffic_mgmt");
  soc_sand_proc_name = "tmd_itm_queue_info_set";

  unit = tmd_get_default_unit();
  TMD_ITM_QUEUE_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_QUEUE_INFO_SET_ITM_QUEUE_INFO_SET_QUEUE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_QUEUE_INFO_SET_ITM_QUEUE_INFO_SET_QUEUE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_queue_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter queue_ndx after itm_queue_info_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = tmd_itm_queue_info_get(
          unit,
          prm_queue_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_queue_info_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_QUEUE_INFO_SET_ITM_QUEUE_INFO_SET_INFO_SIGNATURE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_QUEUE_INFO_SET_ITM_QUEUE_INFO_SET_INFO_SIGNATURE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.signature = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_QUEUE_INFO_SET_ITM_QUEUE_INFO_SET_INFO_VSQ_TRAFFIC_CLS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_QUEUE_INFO_SET_ITM_QUEUE_INFO_SET_INFO_VSQ_TRAFFIC_CLS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.vsq_traffic_cls = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_QUEUE_INFO_SET_ITM_QUEUE_INFO_SET_INFO_VSQ_CONNECTION_CLS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_QUEUE_INFO_SET_ITM_QUEUE_INFO_SET_INFO_VSQ_CONNECTION_CLS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.vsq_connection_cls = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_QUEUE_INFO_SET_ITM_QUEUE_INFO_SET_INFO_RATE_CLS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_QUEUE_INFO_SET_ITM_QUEUE_INFO_SET_INFO_RATE_CLS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.rate_cls = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_QUEUE_INFO_SET_ITM_QUEUE_INFO_SET_INFO_CREDIT_CLS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_QUEUE_INFO_SET_ITM_QUEUE_INFO_SET_INFO_CREDIT_CLS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.credit_cls = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_QUEUE_INFO_SET_ITM_QUEUE_INFO_SET_INFO_CR_REQ_TYPE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_QUEUE_INFO_SET_ITM_QUEUE_INFO_SET_INFO_CR_REQ_TYPE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.cr_req_type_ndx = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_itm_queue_info_set(
          unit,
          prm_queue_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_queue_info_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: itm_queue_info_get (section ingress_traffic_mgmt)
 */
int
  ui_tmd_api_ingress_traffic_mgmt_itm_queue_info_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_queue_ndx = 0;
  TMD_ITM_QUEUE_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_traffic_mgmt");
  soc_sand_proc_name = "tmd_itm_queue_info_get";

  unit = tmd_get_default_unit();
  TMD_ITM_QUEUE_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_QUEUE_INFO_GET_ITM_QUEUE_INFO_GET_QUEUE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_QUEUE_INFO_GET_ITM_QUEUE_INFO_GET_QUEUE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_queue_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter queue_ndx after itm_queue_info_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_itm_queue_info_get(
          unit,
          prm_queue_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_queue_info_get");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  TMD_ITM_QUEUE_INFO_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: itm_ingress_shape_set (section ingress_traffic_mgmt)
 */
int
  ui_tmd_api_ingress_traffic_mgmt_itm_ingress_shape_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_ITM_INGRESS_SHAPE_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_traffic_mgmt");
  soc_sand_proc_name = "tmd_itm_ingress_shape_set";

  unit = tmd_get_default_unit();
  TMD_ITM_INGRESS_SHAPE_INFO_clear(&prm_info);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = tmd_itm_ingress_shape_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_ingress_shape_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_INGRESS_SHAPE_SET_ITM_INGRESS_SHAPE_SET_INFO_SCH_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_INGRESS_SHAPE_SET_ITM_INGRESS_SHAPE_SET_INFO_SCH_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.sch_port = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_INGRESS_SHAPE_SET_ITM_INGRESS_SHAPE_SET_INFO_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_INGRESS_SHAPE_SET_ITM_INGRESS_SHAPE_SET_INFO_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.rate = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_INGRESS_SHAPE_SET_ITM_INGRESS_SHAPE_SET_INFO_Q_RANGE_Q_NUM_HIGH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_INGRESS_SHAPE_SET_ITM_INGRESS_SHAPE_SET_INFO_Q_RANGE_Q_NUM_HIGH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.q_range.q_num_high = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_INGRESS_SHAPE_SET_ITM_INGRESS_SHAPE_SET_INFO_Q_RANGE_Q_NUM_LOW_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_INGRESS_SHAPE_SET_ITM_INGRESS_SHAPE_SET_INFO_Q_RANGE_Q_NUM_LOW_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.q_range.q_num_low = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_INGRESS_SHAPE_SET_ITM_INGRESS_SHAPE_SET_INFO_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_INGRESS_SHAPE_SET_ITM_INGRESS_SHAPE_SET_INFO_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.enable = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_itm_ingress_shape_set(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_ingress_shape_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: itm_ingress_shape_get (section ingress_traffic_mgmt)
 */
int
  ui_tmd_api_ingress_traffic_mgmt_itm_ingress_shape_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_ITM_INGRESS_SHAPE_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_traffic_mgmt");
  soc_sand_proc_name = "tmd_itm_ingress_shape_get";

  unit = tmd_get_default_unit();
  TMD_ITM_INGRESS_SHAPE_INFO_clear(&prm_info);

  /* Get parameters */

  /* Call function */
  ret = tmd_itm_ingress_shape_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_ingress_shape_get");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  TMD_ITM_INGRESS_SHAPE_INFO_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: itm_priority_map_tmplt_set (section ingress_traffic_mgmt)
 */
int
  ui_tmd_api_ingress_traffic_mgmt_itm_priority_map_tmplt_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_map_ndx = 0;
  uint32
    prm_map_index = 0xFFFFFFFF;
  TMD_ITM_PRIORITY_MAP_TMPLT
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_traffic_mgmt");
  soc_sand_proc_name = "tmd_itm_priority_map_tmplt_set";

  unit = tmd_get_default_unit();
  TMD_ITM_PRIORITY_MAP_TMPLT_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_PRIORITY_MAP_TMPLT_SET_ITM_PRIORITY_MAP_TMPLT_SET_MAP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_PRIORITY_MAP_TMPLT_SET_ITM_PRIORITY_MAP_TMPLT_SET_MAP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_map_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter map_ndx after itm_priority_map_tmplt_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = tmd_itm_priority_map_tmplt_get(
          unit,
          prm_map_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_priority_map_tmplt_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_PRIORITY_MAP_TMPLT_SET_ITM_PRIORITY_MAP_TMPLT_SET_INFO_MAP_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_PRIORITY_MAP_TMPLT_SET_ITM_PRIORITY_MAP_TMPLT_SET_INFO_MAP_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_map_index = (uint32)param_val->value.ulong_value;
  }

  if(prm_map_index != 0xFFFFFFFF)
  {

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_PRIORITY_MAP_TMPLT_SET_ITM_PRIORITY_MAP_TMPLT_SET_INFO_MAP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_PRIORITY_MAP_TMPLT_SET_ITM_PRIORITY_MAP_TMPLT_SET_INFO_MAP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.map[ prm_map_index] = (uint32)param_val->value.ulong_value;
  }

  }


  /* Call function */
  ret = tmd_itm_priority_map_tmplt_set(
          unit,
          prm_map_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_priority_map_tmplt_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: itm_priority_map_tmplt_get (section ingress_traffic_mgmt)
 */
int
  ui_tmd_api_ingress_traffic_mgmt_itm_priority_map_tmplt_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_map_ndx = 0;
  TMD_ITM_PRIORITY_MAP_TMPLT
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_traffic_mgmt");
  soc_sand_proc_name = "tmd_itm_priority_map_tmplt_get";

  unit = tmd_get_default_unit();
  TMD_ITM_PRIORITY_MAP_TMPLT_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_PRIORITY_MAP_TMPLT_GET_ITM_PRIORITY_MAP_TMPLT_GET_MAP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_PRIORITY_MAP_TMPLT_GET_ITM_PRIORITY_MAP_TMPLT_GET_MAP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_map_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter map_ndx after itm_priority_map_tmplt_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_itm_priority_map_tmplt_get(
          unit,
          prm_map_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_priority_map_tmplt_get");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  TMD_ITM_PRIORITY_MAP_TMPLT_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: itm_priority_map_tmplt_select_set (section ingress_traffic_mgmt)
 */
int
  ui_tmd_api_ingress_traffic_mgmt_itm_priority_map_tmplt_select_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_queue_64_ndx = 0;
  uint32
    prm_priority_map;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_traffic_mgmt");
  soc_sand_proc_name = "tmd_itm_priority_map_tmplt_select_set";

  unit = tmd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_PRIORITY_MAP_TMPLT_SELECT_SET_ITM_PRIORITY_MAP_TMPLT_SELECT_SET_QUEUE_64_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_PRIORITY_MAP_TMPLT_SELECT_SET_ITM_PRIORITY_MAP_TMPLT_SELECT_SET_QUEUE_64_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_queue_64_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter queue_64_ndx after itm_priority_map_tmplt_select_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = tmd_itm_priority_map_tmplt_select_get(
          unit,
          prm_queue_64_ndx,
          &prm_priority_map
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_priority_map_tmplt_select_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_PRIORITY_MAP_TMPLT_SELECT_SET_ITM_PRIORITY_MAP_TMPLT_SELECT_SET_PRIORITY_MAP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_PRIORITY_MAP_TMPLT_SELECT_SET_ITM_PRIORITY_MAP_TMPLT_SELECT_SET_PRIORITY_MAP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_priority_map = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_itm_priority_map_tmplt_select_set(
          unit,
          prm_queue_64_ndx,
          prm_priority_map
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_priority_map_tmplt_select_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: itm_priority_map_tmplt_select_get (section ingress_traffic_mgmt)
 */
int
  ui_tmd_api_ingress_traffic_mgmt_itm_priority_map_tmplt_select_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_queue_64_ndx = 0;
  uint32
    prm_priority_map;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_traffic_mgmt");
  soc_sand_proc_name = "tmd_itm_priority_map_tmplt_select_get";

  unit = tmd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_PRIORITY_MAP_TMPLT_SELECT_GET_ITM_PRIORITY_MAP_TMPLT_SELECT_GET_QUEUE_64_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_PRIORITY_MAP_TMPLT_SELECT_GET_ITM_PRIORITY_MAP_TMPLT_SELECT_GET_QUEUE_64_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_queue_64_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter queue_64_ndx after itm_priority_map_tmplt_select_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_itm_priority_map_tmplt_select_get(
          unit,
          prm_queue_64_ndx,
          &prm_priority_map
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_priority_map_tmplt_select_get");
    goto exit;
  }

  soc_sand_os_printf( "priority_map: %d\n\r",prm_priority_map);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: itm_sys_red_drop_prob_set (section ingress_traffic_mgmt)
 */
int
  ui_tmd_api_ingress_traffic_mgmt_itm_sys_red_drop_prob_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_drop_probs_index = 0xFFFFFFFF;
  TMD_ITM_SYS_RED_DROP_PROB
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_traffic_mgmt");
  soc_sand_proc_name = "tmd_itm_sys_red_drop_prob_set";

  unit = tmd_get_default_unit();
  TMD_ITM_SYS_RED_DROP_PROB_clear(&prm_info);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = tmd_itm_sys_red_drop_prob_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_sys_red_drop_prob_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_SYS_RED_DROP_PROB_SET_ITM_SYS_RED_DROP_PROB_SET_INFO_DROP_PROBS_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_SYS_RED_DROP_PROB_SET_ITM_SYS_RED_DROP_PROB_SET_INFO_DROP_PROBS_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_drop_probs_index = (uint32)param_val->value.ulong_value;
  }

  if(prm_drop_probs_index != 0xFFFFFFFF)
  {
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_SYS_RED_DROP_PROB_SET_ITM_SYS_RED_DROP_PROB_SET_INFO_DROP_PROBS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_SYS_RED_DROP_PROB_SET_ITM_SYS_RED_DROP_PROB_SET_INFO_DROP_PROBS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.drop_probs[ prm_drop_probs_index] = (uint32)param_val->value.ulong_value;
  }

  }


  /* Call function */
  ret = tmd_itm_sys_red_drop_prob_set(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_sys_red_drop_prob_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: itm_sys_red_drop_prob_get (section ingress_traffic_mgmt)
 */
int
  ui_tmd_api_ingress_traffic_mgmt_itm_sys_red_drop_prob_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_ITM_SYS_RED_DROP_PROB
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_traffic_mgmt");
  soc_sand_proc_name = "tmd_itm_sys_red_drop_prob_get";

  unit = tmd_get_default_unit();
  TMD_ITM_SYS_RED_DROP_PROB_clear(&prm_info);

  /* Get parameters */

  /* Call function */
  ret = tmd_itm_sys_red_drop_prob_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_sys_red_drop_prob_get");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  TMD_ITM_SYS_RED_DROP_PROB_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: itm_sys_red_queue_size_boundaries_set (section ingress_traffic_mgmt)
 */
int
  ui_tmd_api_ingress_traffic_mgmt_itm_sys_red_queue_size_boundaries_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_rt_cls_ndx = 0;
  uint32
    prm_queue_size_boundaries_index = 0xFFFFFFFF;
  TMD_ITM_SYS_RED_QT_INFO
    prm_info;
  TMD_ITM_SYS_RED_QT_INFO
    prm_exact_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_traffic_mgmt");
  soc_sand_proc_name = "tmd_itm_sys_red_queue_size_boundaries_set";

  unit = tmd_get_default_unit();
  TMD_ITM_SYS_RED_QT_INFO_clear(&prm_info);
  TMD_ITM_SYS_RED_QT_INFO_clear(&prm_exact_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_SYS_RED_QUEUE_SIZE_BOUNDARIES_SET_ITM_SYS_RED_QUEUE_SIZE_BOUNDARIES_SET_RT_CLS_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_SYS_RED_QUEUE_SIZE_BOUNDARIES_SET_ITM_SYS_RED_QUEUE_SIZE_BOUNDARIES_SET_RT_CLS_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_rt_cls_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter rt_cls_ndx after itm_sys_red_queue_size_boundaries_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = tmd_itm_sys_red_queue_size_boundaries_get(
          unit,
          prm_rt_cls_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_sys_red_queue_size_boundaries_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_SYS_RED_QUEUE_SIZE_BOUNDARIES_SET_ITM_SYS_RED_QUEUE_SIZE_BOUNDARIES_SET_INFO_QUEUE_SIZE_BOUNDARIES_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_SYS_RED_QUEUE_SIZE_BOUNDARIES_SET_ITM_SYS_RED_QUEUE_SIZE_BOUNDARIES_SET_INFO_QUEUE_SIZE_BOUNDARIES_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_queue_size_boundaries_index = (uint32)param_val->value.ulong_value;
  }

  if(prm_queue_size_boundaries_index != 0xFFFFFFFF)
  {
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_SYS_RED_QUEUE_SIZE_BOUNDARIES_SET_ITM_SYS_RED_QUEUE_SIZE_BOUNDARIES_SET_INFO_QUEUE_SIZE_BOUNDARIES_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_SYS_RED_QUEUE_SIZE_BOUNDARIES_SET_ITM_SYS_RED_QUEUE_SIZE_BOUNDARIES_SET_INFO_QUEUE_SIZE_BOUNDARIES_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.queue_size_boundaries[ prm_queue_size_boundaries_index] = (uint32)param_val->value.ulong_value;
  }

  }


  /* Call function */
  ret = tmd_itm_sys_red_queue_size_boundaries_set(
          unit,
          prm_rt_cls_ndx,
          &prm_info,
          &prm_exact_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_sys_red_queue_size_boundaries_set");
    goto exit;
  }

  send_string_to_screen("--> exact_info:", TRUE);
  TMD_ITM_SYS_RED_QT_INFO_print(&prm_exact_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: itm_sys_red_queue_size_boundaries_get (section ingress_traffic_mgmt)
 */
int
  ui_tmd_api_ingress_traffic_mgmt_itm_sys_red_queue_size_boundaries_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_rt_cls_ndx = 0;
  TMD_ITM_SYS_RED_QT_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_traffic_mgmt");
  soc_sand_proc_name = "tmd_itm_sys_red_queue_size_boundaries_get";

  unit = tmd_get_default_unit();
  TMD_ITM_SYS_RED_QT_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_SYS_RED_QUEUE_SIZE_BOUNDARIES_GET_ITM_SYS_RED_QUEUE_SIZE_BOUNDARIES_GET_RT_CLS_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_SYS_RED_QUEUE_SIZE_BOUNDARIES_GET_ITM_SYS_RED_QUEUE_SIZE_BOUNDARIES_GET_RT_CLS_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_rt_cls_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter rt_cls_ndx after itm_sys_red_queue_size_boundaries_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_itm_sys_red_queue_size_boundaries_get(
          unit,
          prm_rt_cls_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_sys_red_queue_size_boundaries_get");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  TMD_ITM_SYS_RED_QT_INFO_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: itm_sys_red_info_set (section ingress_traffic_mgmt)
 */
int
  ui_tmd_api_ingress_traffic_mgmt_itm_sys_red_info_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_rt_cls_ndx = 0;
  uint32
    prm_sys_red_dp_ndx = 0;
  TMD_ITM_SYS_RED_QT_DP_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_traffic_mgmt");
  soc_sand_proc_name = "tmd_itm_sys_red_q_based_set";

  unit = tmd_get_default_unit();
  TMD_ITM_SYS_RED_QT_DP_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_SYS_RED_Q_BASED_SET_ITM_SYS_RED_INFO_SET_RT_CLS_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_SYS_RED_Q_BASED_SET_ITM_SYS_RED_INFO_SET_RT_CLS_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_rt_cls_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter rt_cls_ndx after itm_sys_red_info_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_SYS_RED_Q_BASED_SET_ITM_SYS_RED_INFO_SET_SYS_RED_DP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_SYS_RED_Q_BASED_SET_ITM_SYS_RED_INFO_SET_SYS_RED_DP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sys_red_dp_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter sys_red_dp_ndx after itm_sys_red_info_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = tmd_itm_sys_red_q_based_get(
          unit,
          prm_rt_cls_ndx,
          prm_sys_red_dp_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_sys_red_q_based_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_SYS_RED_Q_BASED_SET_ITM_SYS_RED_INFO_SET_INFO_DRP_PROB_HIGH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_SYS_RED_Q_BASED_SET_ITM_SYS_RED_INFO_SET_INFO_DRP_PROB_HIGH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.drp_prob_high = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_SYS_RED_Q_BASED_SET_ITM_SYS_RED_INFO_SET_INFO_DRP_PROB_LOW_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_SYS_RED_Q_BASED_SET_ITM_SYS_RED_INFO_SET_INFO_DRP_PROB_LOW_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.drp_prob_low = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_SYS_RED_Q_BASED_SET_ITM_SYS_RED_INFO_SET_INFO_DRP_TH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_SYS_RED_Q_BASED_SET_ITM_SYS_RED_INFO_SET_INFO_DRP_TH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.drp_th = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_SYS_RED_Q_BASED_SET_ITM_SYS_RED_INFO_SET_INFO_PROB_TH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_SYS_RED_Q_BASED_SET_ITM_SYS_RED_INFO_SET_INFO_PROB_TH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.prob_th = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_SYS_RED_Q_BASED_SET_ITM_SYS_RED_INFO_SET_INFO_ADM_TH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_SYS_RED_Q_BASED_SET_ITM_SYS_RED_INFO_SET_INFO_ADM_TH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.adm_th = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_SYS_RED_Q_BASED_SET_ITM_SYS_RED_INFO_SET_INFO_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_SYS_RED_Q_BASED_SET_ITM_SYS_RED_INFO_SET_INFO_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.enable = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_itm_sys_red_q_based_set(
          unit,
          prm_rt_cls_ndx,
          prm_sys_red_dp_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_sys_red_q_based_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: itm_sys_red_info_get (section ingress_traffic_mgmt)
 */
int
  ui_tmd_api_ingress_traffic_mgmt_itm_sys_red_info_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_rt_cls_ndx = 0;
  uint32
    prm_sys_red_dp_ndx = 0;
  TMD_ITM_SYS_RED_QT_DP_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_traffic_mgmt");
  soc_sand_proc_name = "tmd_itm_sys_red_q_based_get";

  unit = tmd_get_default_unit();
  TMD_ITM_SYS_RED_QT_DP_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_SYS_RED_Q_BASED_GET_ITM_SYS_RED_INFO_GET_RT_CLS_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_SYS_RED_Q_BASED_GET_ITM_SYS_RED_INFO_GET_RT_CLS_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_rt_cls_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter rt_cls_ndx after itm_sys_red_info_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_SYS_RED_Q_BASED_GET_ITM_SYS_RED_INFO_GET_SYS_RED_DP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_SYS_RED_Q_BASED_GET_ITM_SYS_RED_INFO_GET_SYS_RED_DP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sys_red_dp_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter sys_red_dp_ndx after itm_sys_red_info_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_itm_sys_red_q_based_get(
          unit,
          prm_rt_cls_ndx,
          prm_sys_red_dp_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_sys_red_q_based_get");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  TMD_ITM_SYS_RED_QT_DP_INFO_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: itm_sys_red_eg_set (section ingress_traffic_mgmt)
 */
int
  ui_tmd_api_ingress_traffic_mgmt_itm_sys_red_eg_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_ITM_SYS_RED_EG_INFO
    prm_info;
  TMD_ITM_SYS_RED_EG_INFO
    prm_exact_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_traffic_mgmt");
  soc_sand_proc_name = "tmd_itm_sys_red_eg_set";

  unit = tmd_get_default_unit();
  TMD_ITM_SYS_RED_EG_INFO_clear(&prm_info);
  TMD_ITM_SYS_RED_EG_INFO_clear(&prm_exact_info);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = tmd_itm_sys_red_eg_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_sys_red_eg_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_SYS_RED_EG_SET_ITM_SYS_RED_EG_SET_INFO_AGING_ONLY_DEC_Q_SIZE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_SYS_RED_EG_SET_ITM_SYS_RED_EG_SET_INFO_AGING_ONLY_DEC_Q_SIZE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.aging_only_dec_q_size = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_SYS_RED_EG_SET_ITM_SYS_RED_EG_SET_INFO_RESET_EXPIRED_Q_SIZE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_SYS_RED_EG_SET_ITM_SYS_RED_EG_SET_INFO_RESET_EXPIRED_Q_SIZE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.reset_expired_q_size = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_SYS_RED_EG_SET_ITM_SYS_RED_EG_SET_INFO_AGING_TIMER_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_SYS_RED_EG_SET_ITM_SYS_RED_EG_SET_INFO_AGING_TIMER_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.aging_timer = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_SYS_RED_EG_SET_ITM_SYS_RED_EG_SET_INFO_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_SYS_RED_EG_SET_ITM_SYS_RED_EG_SET_INFO_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.enable = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_itm_sys_red_eg_set(
          unit,
          &prm_info,
          &prm_exact_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_sys_red_eg_set");
    goto exit;
  }

  send_string_to_screen("--> exact_info:", TRUE);
  TMD_ITM_SYS_RED_EG_INFO_print(&prm_exact_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: itm_sys_red_eg_get (section ingress_traffic_mgmt)
 */
int
  ui_tmd_api_ingress_traffic_mgmt_itm_sys_red_eg_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_ITM_SYS_RED_EG_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_traffic_mgmt");
  soc_sand_proc_name = "tmd_itm_sys_red_eg_get";

  unit = tmd_get_default_unit();
  TMD_ITM_SYS_RED_EG_INFO_clear(&prm_info);

  /* Get parameters */

  /* Call function */
  ret = tmd_itm_sys_red_eg_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_sys_red_eg_get");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  TMD_ITM_SYS_RED_EG_INFO_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: itm_sys_red_glob_rcs_set (section ingress_traffic_mgmt)
 */
int
  ui_tmd_api_ingress_traffic_mgmt_itm_sys_red_glob_rcs_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_ITM_SYS_RED_GLOB_RCS_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_traffic_mgmt");
  soc_sand_proc_name = "tmd_itm_sys_red_glob_rcs_set";

  unit = tmd_get_default_unit();
  TMD_ITM_SYS_RED_GLOB_RCS_INFO_clear(&prm_info);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = tmd_itm_sys_red_glob_rcs_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_sys_red_glob_rcs_get");
    goto exit;
  }
  /*
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_SYS_RED_GLOB_RCS_SET_ITM_SYS_RED_GLOB_RCS_SET_INFO_VALUES_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_SYS_RED_GLOB_RCS_SET_ITM_SYS_RED_GLOB_RCS_SET_INFO_VALUES_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.values = param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_SYS_RED_GLOB_RCS_SET_ITM_SYS_RED_GLOB_RCS_SET_INFO_THRESHOLDS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ITM_SYS_RED_GLOB_RCS_SET_ITM_SYS_RED_GLOB_RCS_SET_INFO_THRESHOLDS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.thresholds = (TMD_ITM_SYS_RED_GLOB_RCS_THS)param_val->value.ulong_value;
  }
*/

  /* Call function */
  ret = tmd_itm_sys_red_glob_rcs_set(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_sys_red_glob_rcs_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: itm_sys_red_glob_rcs_get (section ingress_traffic_mgmt)
 */
int
  ui_tmd_api_ingress_traffic_mgmt_itm_sys_red_glob_rcs_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_ITM_SYS_RED_GLOB_RCS_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_traffic_mgmt");
  soc_sand_proc_name = "tmd_itm_sys_red_glob_rcs_get";

  unit = tmd_get_default_unit();
  TMD_ITM_SYS_RED_GLOB_RCS_INFO_clear(&prm_info);

  /* Get parameters */

  /* Call function */
  ret = tmd_itm_sys_red_glob_rcs_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_itm_sys_red_glob_rcs_get");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  TMD_ITM_SYS_RED_GLOB_RCS_INFO_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

#endif

#ifdef UI_INGRESS_SCHEDULER
/********************************************************************
 *  Function handler: mesh_set (section ingress_scheduler)
 */
int
  ui_tmd_api_ingress_scheduler_mesh_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_contexts_index = 0xFFFFFFFF;
  TMD_ING_SCH_MESH_INFO
    prm_mesh_info;
  TMD_ING_SCH_MESH_INFO
    prm_exact_mesh_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_scheduler");
  soc_sand_proc_name = "tmd_ingress_scheduler_mesh_set";

  unit = tmd_get_default_unit();
  TMD_ING_SCH_MESH_INFO_clear(&prm_mesh_info);
  TMD_ING_SCH_MESH_INFO_clear(&prm_exact_mesh_info);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = tmd_ingress_scheduler_mesh_get(
          unit,
          &prm_mesh_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ingress_scheduler_mesh_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_INGRESS_SCHEDULER_MESH_SET_MESH_SET_MESH_INFO_TOTAL_RATE_SHAPER_MAX_BURST_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_INGRESS_SCHEDULER_MESH_SET_MESH_SET_MESH_INFO_TOTAL_RATE_SHAPER_MAX_BURST_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mesh_info.total_rate_shaper.max_burst = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_INGRESS_SCHEDULER_MESH_SET_MESH_SET_MESH_INFO_TOTAL_RATE_SHAPER_MAX_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_INGRESS_SCHEDULER_MESH_SET_MESH_SET_MESH_INFO_TOTAL_RATE_SHAPER_MAX_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mesh_info.total_rate_shaper.max_rate = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_INGRESS_SCHEDULER_MESH_SET_MESH_SET_MESH_INFO_NOF_ENTRIES_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_INGRESS_SCHEDULER_MESH_SET_MESH_SET_MESH_INFO_NOF_ENTRIES_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mesh_info.nof_entries = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_INGRESS_SCHEDULER_MESH_SET_MESH_SET_MESH_INFO_CONTEXTS_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_INGRESS_SCHEDULER_MESH_SET_MESH_SET_MESH_INFO_CONTEXTS_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_contexts_index = (uint32)param_val->value.ulong_value;
  }

  if(prm_contexts_index != 0xFFFFFFFF)
  {
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_INGRESS_SCHEDULER_MESH_SET_MESH_SET_MESH_INFO_CONTEXTS_ID_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_INGRESS_SCHEDULER_MESH_SET_MESH_SET_MESH_INFO_CONTEXTS_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mesh_info.contexts[ prm_contexts_index].id = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_INGRESS_SCHEDULER_MESH_SET_MESH_SET_MESH_INFO_CONTEXTS_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_INGRESS_SCHEDULER_MESH_SET_MESH_SET_MESH_INFO_CONTEXTS_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mesh_info.contexts[ prm_contexts_index].weight = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_INGRESS_SCHEDULER_MESH_SET_MESH_SET_MESH_INFO_CONTEXTS_SHAPER_MAX_BURST_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_INGRESS_SCHEDULER_MESH_SET_MESH_SET_MESH_INFO_CONTEXTS_SHAPER_MAX_BURST_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mesh_info.contexts[ prm_contexts_index].shaper.max_burst = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_INGRESS_SCHEDULER_MESH_SET_MESH_SET_MESH_INFO_CONTEXTS_SHAPER_MAX_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_INGRESS_SCHEDULER_MESH_SET_MESH_SET_MESH_INFO_CONTEXTS_SHAPER_MAX_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mesh_info.contexts[ prm_contexts_index].shaper.max_rate = (uint32)param_val->value.ulong_value;
  }

  }


  /* Call function */
  ret = tmd_ingress_scheduler_mesh_set(
          unit,
          &prm_mesh_info,
          &prm_exact_mesh_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ingress_scheduler_mesh_set");
    goto exit;
  }

  send_string_to_screen("--> exact_mesh_info:", TRUE);
  TMD_ING_SCH_MESH_INFO_print(&prm_exact_mesh_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: mesh_get (section ingress_scheduler)
 */
int
  ui_tmd_api_ingress_scheduler_mesh_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_ING_SCH_MESH_INFO
    prm_mesh_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_scheduler");
  soc_sand_proc_name = "tmd_ingress_scheduler_mesh_get";

  unit = tmd_get_default_unit();
  TMD_ING_SCH_MESH_INFO_clear(&prm_mesh_info);

  /* Get parameters */

  /* Call function */
  ret = tmd_ingress_scheduler_mesh_get(
          unit,
          &prm_mesh_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ingress_scheduler_mesh_get");
    goto exit;
  }

  send_string_to_screen("--> mesh_info:", TRUE);
  TMD_ING_SCH_MESH_INFO_print(&prm_mesh_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: clos_set (section ingress_scheduler)
 */
int
  ui_tmd_api_ingress_scheduler_clos_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_ING_SCH_CLOS_INFO
    prm_clos_info;
  TMD_ING_SCH_CLOS_INFO
    prm_exact_clos_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_scheduler");
  soc_sand_proc_name = "tmd_ingress_scheduler_clos_set";

  unit = tmd_get_default_unit();
  TMD_ING_SCH_CLOS_INFO_clear(&prm_clos_info);
  TMD_ING_SCH_CLOS_INFO_clear(&prm_exact_clos_info);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = tmd_ingress_scheduler_clos_get(
          unit,
          &prm_clos_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ingress_scheduler_clos_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_INGRESS_SCHEDULER_CLOS_SET_CLOS_SET_CLOS_INFO_WEIGHTS_GLOBAL_LP_WEIGHT2_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_INGRESS_SCHEDULER_CLOS_SET_CLOS_SET_CLOS_INFO_WEIGHTS_GLOBAL_LP_WEIGHT2_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_clos_info.weights.global_lp.weight2 = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_INGRESS_SCHEDULER_CLOS_SET_CLOS_SET_CLOS_INFO_WEIGHTS_GLOBAL_LP_WEIGHT1_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_INGRESS_SCHEDULER_CLOS_SET_CLOS_SET_CLOS_INFO_WEIGHTS_GLOBAL_LP_WEIGHT1_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_clos_info.weights.global_lp.weight1 = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_INGRESS_SCHEDULER_CLOS_SET_CLOS_SET_CLOS_INFO_WEIGHTS_GLOBAL_HP_WEIGHT2_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_INGRESS_SCHEDULER_CLOS_SET_CLOS_SET_CLOS_INFO_WEIGHTS_GLOBAL_HP_WEIGHT2_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_clos_info.weights.global_hp.weight2 = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_INGRESS_SCHEDULER_CLOS_SET_CLOS_SET_CLOS_INFO_WEIGHTS_GLOBAL_HP_WEIGHT1_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_INGRESS_SCHEDULER_CLOS_SET_CLOS_SET_CLOS_INFO_WEIGHTS_GLOBAL_HP_WEIGHT1_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_clos_info.weights.global_hp.weight1 = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_INGRESS_SCHEDULER_CLOS_SET_CLOS_SET_CLOS_INFO_WEIGHTS_FABRIC_LP_WEIGHT2_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_INGRESS_SCHEDULER_CLOS_SET_CLOS_SET_CLOS_INFO_WEIGHTS_FABRIC_LP_WEIGHT2_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_clos_info.weights.fabric_lp.weight2 = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_INGRESS_SCHEDULER_CLOS_SET_CLOS_SET_CLOS_INFO_WEIGHTS_FABRIC_LP_WEIGHT1_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_INGRESS_SCHEDULER_CLOS_SET_CLOS_SET_CLOS_INFO_WEIGHTS_FABRIC_LP_WEIGHT1_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_clos_info.weights.fabric_lp.weight1 = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_INGRESS_SCHEDULER_CLOS_SET_CLOS_SET_CLOS_INFO_WEIGHTS_FABRIC_HP_WEIGHT2_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_INGRESS_SCHEDULER_CLOS_SET_CLOS_SET_CLOS_INFO_WEIGHTS_FABRIC_HP_WEIGHT2_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_clos_info.weights.fabric_hp.weight2 = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_INGRESS_SCHEDULER_CLOS_SET_CLOS_SET_CLOS_INFO_WEIGHTS_FABRIC_HP_WEIGHT1_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_INGRESS_SCHEDULER_CLOS_SET_CLOS_SET_CLOS_INFO_WEIGHTS_FABRIC_HP_WEIGHT1_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_clos_info.weights.fabric_hp.weight1 = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_INGRESS_SCHEDULER_CLOS_SET_CLOS_SET_CLOS_INFO_SHAPERS_FABRIC_MAX_BURST_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_INGRESS_SCHEDULER_CLOS_SET_CLOS_SET_CLOS_INFO_SHAPERS_FABRIC_MAX_BURST_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_clos_info.shapers.fabric.max_burst = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_INGRESS_SCHEDULER_CLOS_SET_CLOS_SET_CLOS_INFO_SHAPERS_FABRIC_MAX_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_INGRESS_SCHEDULER_CLOS_SET_CLOS_SET_CLOS_INFO_SHAPERS_FABRIC_MAX_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_clos_info.shapers.fabric.max_rate = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_INGRESS_SCHEDULER_CLOS_SET_CLOS_SET_CLOS_INFO_SHAPERS_LOCAL_MAX_BURST_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_INGRESS_SCHEDULER_CLOS_SET_CLOS_SET_CLOS_INFO_SHAPERS_LOCAL_MAX_BURST_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_clos_info.shapers.local.max_burst = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_INGRESS_SCHEDULER_CLOS_SET_CLOS_SET_CLOS_INFO_SHAPERS_LOCAL_MAX_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_INGRESS_SCHEDULER_CLOS_SET_CLOS_SET_CLOS_INFO_SHAPERS_LOCAL_MAX_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_clos_info.shapers.local.max_rate = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  ret = tmd_ingress_scheduler_clos_set(
          unit,
          &prm_clos_info,
          &prm_exact_clos_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ingress_scheduler_clos_set");
    goto exit;
  }

  send_string_to_screen("--> exact_clos_info:", TRUE);
  TMD_ING_SCH_CLOS_INFO_print(&prm_exact_clos_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: clos_get (section ingress_scheduler)
 */
int
  ui_tmd_api_ingress_scheduler_clos_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_ING_SCH_CLOS_INFO
    prm_clos_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_scheduler");
  soc_sand_proc_name = "tmd_ingress_scheduler_clos_get";

  unit = tmd_get_default_unit();
  TMD_ING_SCH_CLOS_INFO_clear(&prm_clos_info);

  /* Get parameters */

  /* Call function */
  ret = tmd_ingress_scheduler_clos_get(
          unit,
          &prm_clos_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ingress_scheduler_clos_get");
    goto exit;
  }

  send_string_to_screen("--> clos_info:", TRUE);
  TMD_ING_SCH_CLOS_INFO_print(&prm_clos_info);


  goto exit;
exit:
  return ui_ret;
}

#endif

#ifdef UI_EGR_QUEUING
/********************************************************************
 *  Function handler: egr_ofp_thresh_type_set (section egr_queuing)
 */
int
  ui_tmd_api_egr_queuing_egr_ofp_thresh_type_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_FAP_PORT_ID
    prm_ofp_ndx = 0;
  TMD_EGR_PORT_THRESH_TYPE
    prm_ofp_thresh_type;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_egr_queuing");
  soc_sand_proc_name = "tmd_egr_ofp_thresh_type_set";

  unit = tmd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_OFP_THRESH_TYPE_SET_EGR_OFP_THRESH_TYPE_SET_OFP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_EGR_OFP_THRESH_TYPE_SET_EGR_OFP_THRESH_TYPE_SET_OFP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ofp_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter ofp_ndx after egr_ofp_thresh_type_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = tmd_egr_ofp_thresh_type_get(
          unit,
          prm_ofp_ndx,
          &prm_ofp_thresh_type
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_egr_ofp_thresh_type_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_OFP_THRESH_TYPE_SET_EGR_OFP_THRESH_TYPE_SET_OFP_THRESH_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_EGR_OFP_THRESH_TYPE_SET_EGR_OFP_THRESH_TYPE_SET_OFP_THRESH_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ofp_thresh_type = param_val->numeric_equivalent;
  }


  /* Call function */
  ret = tmd_egr_ofp_thresh_type_set(
          unit,
          prm_ofp_ndx,
          prm_ofp_thresh_type
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_egr_ofp_thresh_type_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: egr_ofp_thresh_type_get (section egr_queuing)
 */
int
  ui_tmd_api_egr_queuing_egr_ofp_thresh_type_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_FAP_PORT_ID
    prm_ofp_ndx = 0;
  TMD_EGR_PORT_THRESH_TYPE
    prm_ofp_thresh_type;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_egr_queuing");
  soc_sand_proc_name = "tmd_egr_ofp_thresh_type_get";

  unit = tmd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_OFP_THRESH_TYPE_GET_EGR_OFP_THRESH_TYPE_GET_OFP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_EGR_OFP_THRESH_TYPE_GET_EGR_OFP_THRESH_TYPE_GET_OFP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ofp_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter ofp_ndx after egr_ofp_thresh_type_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_egr_ofp_thresh_type_get(
          unit,
          prm_ofp_ndx,
          &prm_ofp_thresh_type
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_egr_ofp_thresh_type_get");
    goto exit;
  }

  soc_sand_os_printf( "ofp_thresh_type: %s\n\r",TMD_EGR_PORT_THRESH_TYPE_to_string(prm_ofp_thresh_type));


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: egr_sched_drop_set (section egr_queuing)
 */
int
  ui_tmd_api_egr_queuing_egr_sched_drop_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_EGR_Q_PRIO
    prm_prio_ndx = 0;
  uint32
    prm_queue_words_consumed_index = 0xFFFFFFFF;
  TMD_EGR_DROP_THRESH
    prm_thresh;
  TMD_EGR_DROP_THRESH
    prm_exact_thresh;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_egr_queuing");
  soc_sand_proc_name = "tmd_egr_sched_drop_set";

  unit = tmd_get_default_unit();
  TMD_EGR_DROP_THRESH_clear(&prm_thresh);
  TMD_EGR_DROP_THRESH_clear(&prm_exact_thresh);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_SCHED_DROP_SET_EGR_SCHED_DROP_SET_PRIO_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_EGR_SCHED_DROP_SET_EGR_SCHED_DROP_SET_PRIO_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_prio_ndx = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter prio_ndx after egr_sched_drop_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = tmd_egr_sched_drop_get(
          unit,
          prm_prio_ndx,
          &prm_thresh
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_egr_sched_drop_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_SCHED_DROP_SET_EGR_SCHED_DROP_SET_THRESH_QUEUE_WORDS_CONSUMED_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_EGR_SCHED_DROP_SET_EGR_SCHED_DROP_SET_THRESH_QUEUE_WORDS_CONSUMED_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_queue_words_consumed_index = (uint32)param_val->value.ulong_value;
  }

  if(prm_queue_words_consumed_index != 0xFFFFFFFF)
  {
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_SCHED_DROP_SET_EGR_SCHED_DROP_SET_THRESH_QUEUE_WORDS_CONSUMED_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_EGR_SCHED_DROP_SET_EGR_SCHED_DROP_SET_THRESH_QUEUE_WORDS_CONSUMED_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_thresh.queue_words_consumed[ prm_queue_words_consumed_index] = (uint32)param_val->value.ulong_value;
  }

  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_SCHED_DROP_SET_EGR_SCHED_DROP_SET_THRESH_BUFFERS_AVAIL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_EGR_SCHED_DROP_SET_EGR_SCHED_DROP_SET_THRESH_BUFFERS_AVAIL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_thresh.buffers_avail = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_SCHED_DROP_SET_EGR_SCHED_DROP_SET_THRESH_DESCRIPTORS_AVAIL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_EGR_SCHED_DROP_SET_EGR_SCHED_DROP_SET_THRESH_DESCRIPTORS_AVAIL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_thresh.descriptors_avail = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_SCHED_DROP_SET_EGR_SCHED_DROP_SET_THRESH_PACKETS_CONSUMED_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_EGR_SCHED_DROP_SET_EGR_SCHED_DROP_SET_THRESH_PACKETS_CONSUMED_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_thresh.packets_consumed = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_SCHED_DROP_SET_EGR_SCHED_DROP_SET_THRESH_WORDS_CONSUMED_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_EGR_SCHED_DROP_SET_EGR_SCHED_DROP_SET_THRESH_WORDS_CONSUMED_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_thresh.words_consumed = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_egr_sched_drop_set(
          unit,
          prm_prio_ndx,
          &prm_thresh,
          &prm_exact_thresh
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_egr_sched_drop_set");
    goto exit;
  }

  send_string_to_screen("--> exact_thresh:", TRUE);
  TMD_EGR_DROP_THRESH_print(&prm_exact_thresh);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: egr_sched_drop_get (section egr_queuing)
 */
int
  ui_tmd_api_egr_queuing_egr_sched_drop_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_EGR_Q_PRIO
    prm_prio_ndx = 0;
  TMD_EGR_DROP_THRESH
    prm_thresh;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_egr_queuing");
  soc_sand_proc_name = "tmd_egr_sched_drop_get";

  unit = tmd_get_default_unit();
  TMD_EGR_DROP_THRESH_clear(&prm_thresh);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_SCHED_DROP_GET_EGR_SCHED_DROP_GET_PRIO_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_EGR_SCHED_DROP_GET_EGR_SCHED_DROP_GET_PRIO_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_prio_ndx = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter prio_ndx after egr_sched_drop_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_egr_sched_drop_get(
          unit,
          prm_prio_ndx,
          &prm_thresh
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_egr_sched_drop_get");
    goto exit;
  }

  send_string_to_screen("--> thresh:", TRUE);
  TMD_EGR_DROP_THRESH_print(&prm_thresh);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: egr_unsched_drop_set (section egr_queuing)
 */
int
  ui_tmd_api_egr_queuing_egr_unsched_drop_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_EGR_Q_PRIO
    prm_prio_ndx = 0;
  uint32
    prm_dp_ndx = 0;
  uint32
    prm_queue_words_consumed_index = 0xFFFFFFFF;
  TMD_EGR_DROP_THRESH
    prm_thresh;
  TMD_EGR_DROP_THRESH
    prm_exact_thresh;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_egr_queuing");
  soc_sand_proc_name = "tmd_egr_unsched_drop_set";

  unit = tmd_get_default_unit();
  TMD_EGR_DROP_THRESH_clear(&prm_thresh);
  TMD_EGR_DROP_THRESH_clear(&prm_exact_thresh);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_UNSCHED_DROP_SET_EGR_UNSCHED_DROP_SET_PRIO_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_EGR_UNSCHED_DROP_SET_EGR_UNSCHED_DROP_SET_PRIO_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_prio_ndx = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter prio_ndx after egr_unsched_drop_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_UNSCHED_DROP_SET_EGR_UNSCHED_DROP_SET_DP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_EGR_UNSCHED_DROP_SET_EGR_UNSCHED_DROP_SET_DP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_dp_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter dp_ndx after egr_unsched_drop_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = tmd_egr_unsched_drop_get(
          unit,
          prm_prio_ndx,
          prm_dp_ndx,
          &prm_thresh
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_egr_unsched_drop_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_UNSCHED_DROP_SET_EGR_UNSCHED_DROP_SET_THRESH_QUEUE_WORDS_CONSUMED_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_EGR_UNSCHED_DROP_SET_EGR_UNSCHED_DROP_SET_THRESH_QUEUE_WORDS_CONSUMED_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_queue_words_consumed_index = (uint32)param_val->value.ulong_value;
  }

  if(prm_queue_words_consumed_index != 0xFFFFFFFF)
  {
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_UNSCHED_DROP_SET_EGR_UNSCHED_DROP_SET_THRESH_QUEUE_WORDS_CONSUMED_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_EGR_UNSCHED_DROP_SET_EGR_UNSCHED_DROP_SET_THRESH_QUEUE_WORDS_CONSUMED_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_thresh.queue_words_consumed[ prm_queue_words_consumed_index] = (uint32)param_val->value.ulong_value;
  }

  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_UNSCHED_DROP_SET_EGR_UNSCHED_DROP_SET_THRESH_BUFFERS_AVAIL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_EGR_UNSCHED_DROP_SET_EGR_UNSCHED_DROP_SET_THRESH_BUFFERS_AVAIL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_thresh.buffers_avail = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_UNSCHED_DROP_SET_EGR_UNSCHED_DROP_SET_THRESH_DESCRIPTORS_AVAIL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_EGR_UNSCHED_DROP_SET_EGR_UNSCHED_DROP_SET_THRESH_DESCRIPTORS_AVAIL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_thresh.descriptors_avail = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_UNSCHED_DROP_SET_EGR_UNSCHED_DROP_SET_THRESH_PACKETS_CONSUMED_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_EGR_UNSCHED_DROP_SET_EGR_UNSCHED_DROP_SET_THRESH_PACKETS_CONSUMED_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_thresh.packets_consumed = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_UNSCHED_DROP_SET_EGR_UNSCHED_DROP_SET_THRESH_WORDS_CONSUMED_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_EGR_UNSCHED_DROP_SET_EGR_UNSCHED_DROP_SET_THRESH_WORDS_CONSUMED_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_thresh.words_consumed = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_egr_unsched_drop_set(
          unit,
          prm_prio_ndx,
          prm_dp_ndx,
          &prm_thresh,
          &prm_exact_thresh
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_egr_unsched_drop_set");
    goto exit;
  }

  send_string_to_screen("--> exact_thresh:", TRUE);
  TMD_EGR_DROP_THRESH_print(&prm_exact_thresh);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: egr_unsched_drop_get (section egr_queuing)
 */
int
  ui_tmd_api_egr_queuing_egr_unsched_drop_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_EGR_Q_PRIO
    prm_prio_ndx = 0;
  uint32
    prm_dp_ndx = 0;
  TMD_EGR_DROP_THRESH
    prm_thresh;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_egr_queuing");
  soc_sand_proc_name = "tmd_egr_unsched_drop_get";

  unit = tmd_get_default_unit();
  TMD_EGR_DROP_THRESH_clear(&prm_thresh);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_UNSCHED_DROP_GET_EGR_UNSCHED_DROP_GET_PRIO_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_EGR_UNSCHED_DROP_GET_EGR_UNSCHED_DROP_GET_PRIO_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_prio_ndx = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter prio_ndx after egr_unsched_drop_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_UNSCHED_DROP_GET_EGR_UNSCHED_DROP_GET_DP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_EGR_UNSCHED_DROP_GET_EGR_UNSCHED_DROP_GET_DP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_dp_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter dp_ndx after egr_unsched_drop_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_egr_unsched_drop_get(
          unit,
          prm_prio_ndx,
          prm_dp_ndx,
          &prm_thresh
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_egr_unsched_drop_get");
    goto exit;
  }

  send_string_to_screen("--> thresh:", TRUE);
  TMD_EGR_DROP_THRESH_print(&prm_thresh);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: egr_dev_fc_set (section egr_queuing)
 */
int
  ui_tmd_api_egr_queuing_egr_dev_fc_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_EGR_FC_DEVICE_THRESH
    prm_thresh;
  TMD_EGR_FC_DEVICE_THRESH
    prm_exact_thresh;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_egr_queuing");
  soc_sand_proc_name = "tmd_egr_dev_fc_set";

  unit = tmd_get_default_unit();
  TMD_EGR_FC_DEVICE_THRESH_clear(&prm_thresh);
  TMD_EGR_FC_DEVICE_THRESH_clear(&prm_exact_thresh);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = tmd_egr_dev_fc_get(
          unit,
          &prm_thresh
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_egr_dev_fc_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_DEV_FC_SET_EGR_DEV_FC_SET_THRESH_SCHEDULED_DESCRIPTORS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_EGR_DEV_FC_SET_EGR_DEV_FC_SET_THRESH_SCHEDULED_DESCRIPTORS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_thresh.scheduled.descriptors = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_DEV_FC_SET_EGR_DEV_FC_SET_THRESH_SCHEDULED_BUFFERS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_EGR_DEV_FC_SET_EGR_DEV_FC_SET_THRESH_SCHEDULED_BUFFERS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_thresh.scheduled.buffers = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_DEV_FC_SET_EGR_DEV_FC_SET_THRESH_SCHEDULED_WORDS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_EGR_DEV_FC_SET_EGR_DEV_FC_SET_THRESH_SCHEDULED_WORDS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_thresh.scheduled.words = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_DEV_FC_SET_EGR_DEV_FC_SET_THRESH_GLOBAL_DESCRIPTORS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_EGR_DEV_FC_SET_EGR_DEV_FC_SET_THRESH_GLOBAL_DESCRIPTORS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_thresh.global.descriptors = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_DEV_FC_SET_EGR_DEV_FC_SET_THRESH_GLOBAL_BUFFERS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_EGR_DEV_FC_SET_EGR_DEV_FC_SET_THRESH_GLOBAL_BUFFERS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_thresh.global.buffers = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_DEV_FC_SET_EGR_DEV_FC_SET_THRESH_GLOBAL_WORDS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_EGR_DEV_FC_SET_EGR_DEV_FC_SET_THRESH_GLOBAL_WORDS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_thresh.global.words = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_egr_dev_fc_set(
          unit,
          &prm_thresh,
          &prm_exact_thresh
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_egr_dev_fc_set");
    goto exit;
  }

  send_string_to_screen("--> exact_thresh:", TRUE);
  TMD_EGR_FC_DEVICE_THRESH_print(&prm_exact_thresh);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: egr_dev_fc_get (section egr_queuing)
 */
int
  ui_tmd_api_egr_queuing_egr_dev_fc_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_EGR_FC_DEVICE_THRESH
    prm_thresh;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_egr_queuing");
  soc_sand_proc_name = "tmd_egr_dev_fc_get";

  unit = tmd_get_default_unit();
  TMD_EGR_FC_DEVICE_THRESH_clear(&prm_thresh);

  /* Get parameters */

  /* Call function */
  ret = tmd_egr_dev_fc_get(
          unit,
          &prm_thresh
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_egr_dev_fc_get");
    goto exit;
  }

  send_string_to_screen("--> thresh:", TRUE);
  TMD_EGR_FC_DEVICE_THRESH_print(&prm_thresh);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: egr_xaui_spaui_fc_set (section egr_queuing)
 */
int
  ui_tmd_api_egr_queuing_egr_xaui_spaui_fc_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_INTERFACE_ID
    prm_if_ndx = 0;
  TMD_EGR_FC_CHNIF_THRESH
    prm_thresh;
  TMD_EGR_FC_CHNIF_THRESH
    prm_exact_thresh;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_egr_queuing");
  soc_sand_proc_name = "tmd_egr_xaui_spaui_fc_set";

  unit = tmd_get_default_unit();
  TMD_EGR_FC_CHNIF_THRESH_clear(&prm_thresh);
  TMD_EGR_FC_CHNIF_THRESH_clear(&prm_exact_thresh);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_CHNIF_FC_SET_EGR_CHNIF_FC_SET_IF_NDX_ID,1))
  {
    UI_MACROS_GET_NUM_SYM_VAL(PARAM_TMD_EGR_CHNIF_FC_SET_EGR_CHNIF_FC_SET_IF_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    UI_MACROS_LOAD_LONG_VAL(prm_if_ndx);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter if_ndx after egr_xaui_spaui_fc_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = tmd_egr_xaui_spaui_fc_get(
          unit,
          prm_if_ndx,
          &prm_thresh
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_egr_xaui_spaui_fc_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_CHNIF_FC_SET_EGR_CHNIF_FC_SET_THRESH_PACKETS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_EGR_CHNIF_FC_SET_EGR_CHNIF_FC_SET_THRESH_PACKETS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_thresh.packets = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_CHNIF_FC_SET_EGR_CHNIF_FC_SET_THRESH_WORDS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_EGR_CHNIF_FC_SET_EGR_CHNIF_FC_SET_THRESH_WORDS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_thresh.words = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_egr_xaui_spaui_fc_set(
          unit,
          prm_if_ndx,
          &prm_thresh,
          &prm_exact_thresh
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_egr_xaui_spaui_fc_set");
    goto exit;
  }

  send_string_to_screen("--> exact_thresh:", TRUE);
  TMD_EGR_FC_CHNIF_THRESH_print(&prm_exact_thresh);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: egr_xaui_spaui_fc_get (section egr_queuing)
 */
int
  ui_tmd_api_egr_queuing_egr_xaui_spaui_fc_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_INTERFACE_ID
    prm_if_ndx = 0;
  TMD_EGR_FC_CHNIF_THRESH
    prm_thresh;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_egr_queuing");
  soc_sand_proc_name = "tmd_egr_xaui_spaui_fc_get";

  unit = tmd_get_default_unit();
  TMD_EGR_FC_CHNIF_THRESH_clear(&prm_thresh);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_CHNIF_FC_GET_EGR_CHNIF_FC_GET_IF_NDX_ID,1))
  {
    UI_MACROS_GET_NUM_SYM_VAL(PARAM_TMD_EGR_CHNIF_FC_GET_EGR_CHNIF_FC_GET_IF_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    UI_MACROS_LOAD_LONG_VAL(prm_if_ndx);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter if_ndx after egr_xaui_spaui_fc_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_egr_xaui_spaui_fc_get(
          unit,
          prm_if_ndx,
          &prm_thresh
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_egr_xaui_spaui_fc_get");
    goto exit;
  }

  send_string_to_screen("--> thresh:", TRUE);
  TMD_EGR_FC_CHNIF_THRESH_print(&prm_thresh);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: egr_ofp_fc_set (section egr_queuing)
 */
int
  ui_tmd_api_egr_queuing_egr_ofp_fc_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_EGR_Q_PRIO
    prm_prio_ndx = 0;
  TMD_EGR_PORT_THRESH_TYPE
    prm_ofp_type_ndx = 0;
  TMD_EGR_FC_OFP_THRESH
    prm_thresh;
  TMD_EGR_FC_OFP_THRESH
    prm_exact_thresh;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_egr_queuing");
  soc_sand_proc_name = "tmd_egr_ofp_fc_set";

  unit = tmd_get_default_unit();
  TMD_EGR_FC_OFP_THRESH_clear(&prm_thresh);
  TMD_EGR_FC_OFP_THRESH_clear(&prm_exact_thresh);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_OFP_FC_SET_EGR_OFP_FC_SET_PRIO_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_EGR_OFP_FC_SET_EGR_OFP_FC_SET_PRIO_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_prio_ndx = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter prio_ndx after egr_ofp_fc_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_OFP_FC_SET_EGR_OFP_FC_SET_OFP_TYPE_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_EGR_OFP_FC_SET_EGR_OFP_FC_SET_OFP_TYPE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ofp_type_ndx = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter ofp_type_ndx after egr_ofp_fc_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = tmd_egr_ofp_fc_get(
          unit,
          prm_prio_ndx,
          prm_ofp_type_ndx,
          &prm_thresh
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_egr_ofp_fc_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_OFP_FC_SET_EGR_OFP_FC_SET_THRESH_WORDS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_EGR_OFP_FC_SET_EGR_OFP_FC_SET_THRESH_WORDS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_thresh.words = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_egr_ofp_fc_set(
          unit,
          prm_prio_ndx,
          prm_ofp_type_ndx,
          &prm_thresh,
          &prm_exact_thresh
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_egr_ofp_fc_set");
    goto exit;
  }

  send_string_to_screen("--> exact_thresh:", TRUE);
  TMD_EGR_FC_OFP_THRESH_print(&prm_exact_thresh);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: egr_ofp_fc_get (section egr_queuing)
 */
int
  ui_tmd_api_egr_queuing_egr_ofp_fc_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_EGR_Q_PRIO
    prm_prio_ndx = 0;
  TMD_EGR_PORT_THRESH_TYPE
    prm_ofp_type_ndx = 0;
  TMD_EGR_FC_OFP_THRESH
    prm_thresh;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_egr_queuing");
  soc_sand_proc_name = "tmd_egr_ofp_fc_get";

  unit = tmd_get_default_unit();
  TMD_EGR_FC_OFP_THRESH_clear(&prm_thresh);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_OFP_FC_GET_EGR_OFP_FC_GET_PRIO_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_EGR_OFP_FC_GET_EGR_OFP_FC_GET_PRIO_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_prio_ndx = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter prio_ndx after egr_ofp_fc_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_OFP_FC_GET_EGR_OFP_FC_GET_OFP_TYPE_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_EGR_OFP_FC_GET_EGR_OFP_FC_GET_OFP_TYPE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ofp_type_ndx = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter ofp_type_ndx after egr_ofp_fc_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_egr_ofp_fc_get(
          unit,
          prm_prio_ndx,
          prm_ofp_type_ndx,
          &prm_thresh
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_egr_ofp_fc_get");
    goto exit;
  }

  send_string_to_screen("--> thresh:", TRUE);
  TMD_EGR_FC_OFP_THRESH_print(&prm_thresh);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: egr_mci_fc_set (section egr_queuing)
 */
int
  ui_tmd_api_egr_queuing_egr_mci_fc_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_EGR_MCI_PRIO
    prm_mci_prio_ndx = 0;
  TMD_EGR_FC_MCI_THRESH
    prm_thresh;
  TMD_EGR_FC_MCI_THRESH
    prm_exact_thresh;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_egr_queuing");
  soc_sand_proc_name = "tmd_egr_mci_fc_set";

  unit = tmd_get_default_unit();
  TMD_EGR_FC_MCI_THRESH_clear(&prm_thresh);
  TMD_EGR_FC_MCI_THRESH_clear(&prm_exact_thresh);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_MCI_FC_SET_EGR_MCI_FC_SET_MCI_PRIO_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_EGR_MCI_FC_SET_EGR_MCI_FC_SET_MCI_PRIO_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mci_prio_ndx = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mci_prio_ndx after egr_mci_fc_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = tmd_egr_mci_fc_get(
          unit,
          prm_mci_prio_ndx,
          &prm_thresh
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_egr_mci_fc_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_MCI_FC_SET_EGR_MCI_FC_SET_THRESH_DESCRIPTORS_TOTAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_EGR_MCI_FC_SET_EGR_MCI_FC_SET_THRESH_DESCRIPTORS_TOTAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_thresh.descriptors_total = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_MCI_FC_SET_EGR_MCI_FC_SET_THRESH_BUFFERS_UNSCH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_EGR_MCI_FC_SET_EGR_MCI_FC_SET_THRESH_BUFFERS_UNSCH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_thresh.buffers_unsch = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_MCI_FC_SET_EGR_MCI_FC_SET_THRESH_PACKETS_UNSCH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_EGR_MCI_FC_SET_EGR_MCI_FC_SET_THRESH_PACKETS_UNSCH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_thresh.packets_unsch = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_MCI_FC_SET_EGR_MCI_FC_SET_THRESH_WORDS_UNSCH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_EGR_MCI_FC_SET_EGR_MCI_FC_SET_THRESH_WORDS_UNSCH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_thresh.words_unsch = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  ret = tmd_egr_mci_fc_set(
          unit,
          prm_mci_prio_ndx,
          &prm_thresh,
          &prm_exact_thresh
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_egr_mci_fc_set");
    goto exit;
  }

  send_string_to_screen("--> exact_thresh:", TRUE);
  TMD_EGR_FC_MCI_THRESH_print(&prm_exact_thresh);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: egr_mci_fc_get (section egr_queuing)
 */
int
  ui_tmd_api_egr_queuing_egr_mci_fc_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_EGR_MCI_PRIO
    prm_mci_prio_ndx = 0;
  TMD_EGR_FC_MCI_THRESH
    prm_thresh;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_egr_queuing");
  soc_sand_proc_name = "tmd_egr_mci_fc_get";

  unit = tmd_get_default_unit();
  TMD_EGR_FC_MCI_THRESH_clear(&prm_thresh);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_MCI_FC_GET_EGR_MCI_FC_GET_MCI_PRIO_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_EGR_MCI_FC_GET_EGR_MCI_FC_GET_MCI_PRIO_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mci_prio_ndx = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mci_prio_ndx after egr_mci_fc_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_egr_mci_fc_get(
          unit,
          prm_mci_prio_ndx,
          &prm_thresh
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_egr_mci_fc_get");
    goto exit;
  }

  send_string_to_screen("--> thresh:", TRUE);
  TMD_EGR_FC_MCI_THRESH_print(&prm_thresh);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: egr_mci_fc_enable_set (section egr_queuing)
 */
int
  ui_tmd_api_egr_queuing_egr_mci_fc_enable_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_EGR_MCI_PRIO
    prm_mci_prio_ndx = 0;
  uint8
    prm_mci_enable;
  uint8
    prm_erp_enable;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_egr_queuing");
  soc_sand_proc_name = "tmd_egr_mci_fc_enable_set";

  unit = tmd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_MCI_FC_ENABLE_SET_EGR_MCI_FC_ENABLE_SET_MCI_PRIO_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_EGR_MCI_FC_ENABLE_SET_EGR_MCI_FC_ENABLE_SET_MCI_PRIO_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mci_prio_ndx = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mci_prio_ndx after egr_mci_fc_enable_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = tmd_egr_mci_fc_enable_get(
          unit,
          prm_mci_prio_ndx,
          &prm_mci_enable,
          &prm_erp_enable
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_egr_mci_fc_enable_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_MCI_FC_ENABLE_SET_EGR_MCI_FC_ENABLE_SET_MCI_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_EGR_MCI_FC_ENABLE_SET_EGR_MCI_FC_ENABLE_SET_MCI_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mci_enable = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_MCI_FC_ENABLE_SET_EGR_MCI_FC_ENABLE_SET_ERP_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_EGR_MCI_FC_ENABLE_SET_EGR_MCI_FC_ENABLE_SET_ERP_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_erp_enable = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_egr_mci_fc_enable_set(
          unit,
          prm_mci_prio_ndx,
          prm_mci_enable,
          prm_erp_enable
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_egr_mci_fc_enable_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: egr_mci_fc_enable_get (section egr_queuing)
 */
int
  ui_tmd_api_egr_queuing_egr_mci_fc_enable_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_EGR_MCI_PRIO
    prm_mci_prio_ndx = 0;
  uint8
    prm_mci_enable;
  uint8
    prm_erp_enable;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_egr_queuing");
  soc_sand_proc_name = "tmd_egr_mci_fc_enable_get";

  unit = tmd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_MCI_FC_ENABLE_GET_EGR_MCI_FC_ENABLE_GET_MCI_PRIO_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_EGR_MCI_FC_ENABLE_GET_EGR_MCI_FC_ENABLE_GET_MCI_PRIO_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mci_prio_ndx = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mci_prio_ndx after egr_mci_fc_enable_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_egr_mci_fc_enable_get(
          unit,
          prm_mci_prio_ndx,
          &prm_mci_enable,
          &prm_erp_enable
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_egr_mci_fc_enable_get");
    goto exit;
  }

  soc_sand_os_printf( "mci_enable: %u\n\r",prm_mci_enable);

  soc_sand_os_printf( "erp_enable: %u\n\r",prm_erp_enable);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: egr_ofp_sch_mode_set (section egr_queuing)
 */
int
  ui_tmd_api_egr_queuing_egr_ofp_sch_mode_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_EGR_OFP_SCH_MODE
    prm_sch_mode;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_egr_queuing");
  soc_sand_proc_name = "tmd_egr_ofp_sch_mode_set";

  unit = tmd_get_default_unit();

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = tmd_egr_ofp_sch_mode_get(
          unit,
          &prm_sch_mode
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_egr_ofp_sch_mode_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_OFP_SCH_MODE_SET_EGR_OFP_SCH_MODE_SET_SCH_MODE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_EGR_OFP_SCH_MODE_SET_EGR_OFP_SCH_MODE_SET_SCH_MODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sch_mode = param_val->numeric_equivalent;
  }


  /* Call function */
  ret = tmd_egr_ofp_sch_mode_set(
          unit,
          &prm_sch_mode
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_egr_ofp_sch_mode_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: egr_ofp_sch_mode_get (section egr_queuing)
 */
int
  ui_tmd_api_egr_queuing_egr_ofp_sch_mode_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_EGR_OFP_SCH_MODE
    prm_sch_mode;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_egr_queuing");
  soc_sand_proc_name = "tmd_egr_ofp_sch_mode_get";

  unit = tmd_get_default_unit();

  /* Get parameters */

  /* Call function */
  ret = tmd_egr_ofp_sch_mode_get(
          unit,
          &prm_sch_mode
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_egr_ofp_sch_mode_get");
    goto exit;
  }

  soc_sand_os_printf( "sch_mode: %s\n\r",TMD_EGR_OFP_SCH_MODE_to_string(prm_sch_mode));


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: egr_ofp_scheduling_set (section egr_queuing)
 */
int
  ui_tmd_api_egr_queuing_egr_ofp_scheduling_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_FAP_PORT_ID
    prm_ofp_ndx = 0;
  TMD_EGR_OFP_SCH_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_egr_queuing");
  soc_sand_proc_name = "tmd_egr_ofp_scheduling_set";

  unit = tmd_get_default_unit();
  TMD_EGR_OFP_SCH_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_OFP_SCHEDULING_SET_EGR_OFP_SCHEDULING_SET_OFP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_EGR_OFP_SCHEDULING_SET_EGR_OFP_SCHEDULING_SET_OFP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ofp_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter ofp_ndx after egr_ofp_scheduling_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = tmd_egr_ofp_scheduling_get(
          unit,
          prm_ofp_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_egr_ofp_scheduling_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_OFP_SCHEDULING_SET_EGR_OFP_SCHEDULING_SET_INFO_OFP_WFQ_SCHED_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_EGR_OFP_SCHEDULING_SET_EGR_OFP_SCHEDULING_SET_INFO_OFP_WFQ_SCHED_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.ofp_wfq.sched_weight = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_OFP_SCHEDULING_SET_EGR_OFP_SCHEDULING_SET_INFO_OFP_WFQ_UNSCHED_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_EGR_OFP_SCHEDULING_SET_EGR_OFP_SCHEDULING_SET_INFO_OFP_WFQ_UNSCHED_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.ofp_wfq.unsched_weight = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_OFP_SCHEDULING_SET_EGR_OFP_SCHEDULING_SET_INFO_NIF_PRIORITY_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_EGR_OFP_SCHEDULING_SET_EGR_OFP_SCHEDULING_SET_INFO_NIF_PRIORITY_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.nif_priority = param_val->numeric_equivalent;
  }


  /* Call function */
  ret = tmd_egr_ofp_scheduling_set(
          unit,
          prm_ofp_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_egr_ofp_scheduling_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: egr_ofp_scheduling_get (section egr_queuing)
 */
int
  ui_tmd_api_egr_queuing_egr_ofp_scheduling_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_FAP_PORT_ID
    prm_ofp_ndx = 0;
  TMD_EGR_OFP_SCH_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_egr_queuing");
  soc_sand_proc_name = "tmd_egr_ofp_scheduling_get";

  unit = tmd_get_default_unit();
  TMD_EGR_OFP_SCH_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_OFP_SCHEDULING_GET_EGR_OFP_SCHEDULING_GET_OFP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_EGR_OFP_SCHEDULING_GET_EGR_OFP_SCHEDULING_GET_OFP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ofp_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter ofp_ndx after egr_ofp_scheduling_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_egr_ofp_scheduling_get(
          unit,
          prm_ofp_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_egr_ofp_scheduling_get");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  TMD_EGR_OFP_SCH_INFO_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: egr_q_prio_set (section egr_queuing)
 */
int
  ui_tmd_api_egr_queuing_egr_q_prio_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_EGR_Q_PRIO_MAPPING_TYPE
    prm_map_type_ndx = 0;
  uint32
    prm_tc_ndx = 0;
  uint32
    prm_dp_ndx = 0,
    prm_map_profile_ndx = 0;
  TMD_EGR_Q_PRIORITY
    prm_prio;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_egr_queuing");
  soc_sand_proc_name = "tmd_egr_q_prio_set";
  
  unit = tmd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_Q_PRIO_SET_EGR_Q_PRIO_SET_MAP_TYPE_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_EGR_Q_PRIO_SET_EGR_Q_PRIO_SET_MAP_TYPE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_map_type_ndx = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter map_type_ndx after egr_q_prio_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_Q_PRIO_SET_EGR_Q_PRIO_SET_TC_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_EGR_Q_PRIO_SET_EGR_Q_PRIO_SET_TC_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tc_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter tc_ndx after egr_q_prio_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_Q_PRIO_SET_EGR_Q_PRIO_SET_DP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_EGR_Q_PRIO_SET_EGR_Q_PRIO_SET_DP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_dp_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter dp_ndx after egr_q_prio_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_Q_PRIO_SET_EGR_Q_PRIO_SET_MAP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_EGR_Q_PRIO_SET_EGR_Q_PRIO_SET_MAP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_map_profile_ndx  = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter map_profile_ndx after egr_q_prio_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = tmd_egr_q_prio_get(
          unit,
          prm_map_type_ndx,
          prm_tc_ndx,
          prm_dp_ndx,
          prm_map_profile_ndx,
          &prm_prio
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_egr_q_prio_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_Q_PRIO_SET_EGR_Q_PRIO_SET_PRIO_TC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_EGR_Q_PRIO_SET_EGR_Q_PRIO_SET_PRIO_TC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_prio.tc = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_Q_PRIO_SET_EGR_Q_PRIO_SET_PRIO_DP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_EGR_Q_PRIO_SET_EGR_Q_PRIO_SET_PRIO_DP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_prio.dp = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_egr_q_prio_set(
          unit,
          prm_map_type_ndx,
          prm_tc_ndx,
          prm_dp_ndx,
          prm_map_profile_ndx,
          &prm_prio
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_egr_q_prio_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: egr_q_prio_get (section egr_queuing)
 */
int
  ui_tmd_api_egr_queuing_egr_q_prio_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_EGR_Q_PRIO_MAPPING_TYPE
    prm_map_type_ndx = 0;
  uint32
    prm_tc_ndx = 0;
  uint32
    prm_dp_ndx = 0,
    prm_map_profile_ndx = 0;
  TMD_EGR_Q_PRIORITY
    prm_prio;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_egr_queuing");
  soc_sand_proc_name = "tmd_egr_q_prio_get";

  unit = tmd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_Q_PRIO_GET_EGR_Q_PRIO_GET_MAP_TYPE_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_EGR_Q_PRIO_GET_EGR_Q_PRIO_GET_MAP_TYPE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_map_type_ndx = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter map_type_ndx after egr_q_prio_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_Q_PRIO_GET_EGR_Q_PRIO_GET_TC_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_EGR_Q_PRIO_GET_EGR_Q_PRIO_GET_TC_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tc_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter tc_ndx after egr_q_prio_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_Q_PRIO_GET_EGR_Q_PRIO_GET_DP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_EGR_Q_PRIO_GET_EGR_Q_PRIO_GET_DP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_dp_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter dp_ndx after egr_q_prio_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_Q_PRIO_SET_EGR_Q_PRIO_GET_MAP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_EGR_Q_PRIO_SET_EGR_Q_PRIO_GET_MAP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_map_profile_ndx  = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter map_profile_ndx after egr_q_prio_set***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_egr_q_prio_get(
          unit,
          prm_map_type_ndx,
          prm_tc_ndx,
          prm_dp_ndx,
          prm_map_profile_ndx,
          &prm_prio
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_egr_q_prio_get");
    goto exit;
  }

  TMD_EGR_Q_PRIORITY_print(&prm_prio);


  goto exit;
exit:
  return ui_ret;
}

#endif

#ifdef UI_OFP_RATES
#ifdef UI_UNDEFED_TEMP
/********************************************************************
 *  Function handler: set (section ofp_rates)
 */
int
  ui_tmd_api_ofp_egq_calendar_validate(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ofp_rates");
  soc_sand_proc_name = "tmd_ofp_rates_egq_calendar_validate";

  unit = tmd_get_default_unit();

  ret = tmd_ofp_rates_egq_calendar_validate(
          unit
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ofp_rates_egq_calendar_validate");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: set (section ofp_rates)
 */
int
  ui_tmd_api_ofp_test(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ofp_rates");
  soc_sand_proc_name = "ui_tmd_api_ofp_test";

  unit = tmd_get_default_unit();

  ret = tmd_ofp_rates_test_random_unsafe(
          unit,
          FALSE
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ofp_rates_test_random");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: set (section ofp_rates)
 */
int
  ui_tmd_api_ofp_all_rates_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_OFP_RATES_TBL_INFO
    prm_ofp_rate_tbl;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ofp_rates");
  soc_sand_proc_name = "tmd_ofp_all_rates_get";

  unit = tmd_get_default_unit();

  TMD_OFP_RATES_TBL_INFO_clear(&prm_ofp_rate_tbl);
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_OFP_ALL_RATES_GET_GET_GET_EXACT_ID,1))
  {
    ret = tmd_ofp_all_ofp_rates_get(unit, TRUE, &prm_ofp_rate_tbl);
  }
  else
  {
    ret = tmd_ofp_all_ofp_rates_get(unit, FALSE, &prm_ofp_rate_tbl);
  }

  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ofp_all_ofp_rates_get");
    goto exit;
  }

  send_string_to_screen("--> ofp_rate_tbl:", TRUE);
  TMD_OFP_RATES_TBL_INFO_print(&prm_ofp_rate_tbl);


  goto exit;
exit:
  return ui_ret;
}
#endif /* UI_UNDEFED_TEMP */

int
  ui_tmd_api_ofp_rates_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint8
    found = FALSE;
  uint32
    nof_entries,
    idx,
    prm_mal_ndx = 0;
  TMD_OFP_RATES_MAL_SHPR_INFO
    prm_shaper;
  uint32
    entry_idx = 0,
    prm_ofp_ndx = 0xFFFFFFFF;
  TMD_OFP_RATES_TBL_INFO
    prm_ofp_rate_tbl;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ofp_rates");
  soc_sand_proc_name = "tmd_ofp_rates_set";

  unit = tmd_get_default_unit();
  TMD_OFP_RATES_MAL_SHPR_INFO_clear(&prm_shaper);
  TMD_OFP_RATES_TBL_INFO_clear(&prm_ofp_rate_tbl);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_OFP_RATES_SET_SET_MAL_NDX_ID,1))
  {
    UI_MACROS_GET_NUM_SYM_VAL(PARAM_TMD_OFP_RATES_SET_SET_MAL_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    UI_MACROS_LOAD_LONG_VAL(prm_mal_ndx);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mal_ndx after set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_OFP_RATES_SET_SET_OFP_RATE_TBL_RATES_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_OFP_RATES_SET_SET_OFP_RATE_TBL_RATES_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ofp_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter ofp_ndx after set mal_ndx***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = tmd_ofp_rates_get(
          unit,
          prm_mal_ndx,
          &prm_shaper,
          &prm_ofp_rate_tbl
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ofp_rates_get");
    goto exit;
  }

  nof_entries = prm_ofp_rate_tbl.nof_valid_entries;

  for (idx = 0; (idx < nof_entries) && !(found); idx++)
  {
    if (prm_ofp_rate_tbl.rates[idx].port_id == prm_ofp_ndx)
    {
      entry_idx = idx;
      found = TRUE;
    }
  }

  if (!found)
  {
    entry_idx = nof_entries;
    nof_entries++;
    prm_ofp_rate_tbl.nof_valid_entries = nof_entries;   
    prm_ofp_rate_tbl.rates[entry_idx].port_id = prm_ofp_ndx;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_OFP_RATES_SET_SET_SHAPER_EGQ_SHAPER_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_OFP_RATES_SET_SET_SHAPER_EGQ_SHAPER_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_shaper.egq_shaper.rate = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_OFP_RATES_SET_SET_SHAPER_EGQ_SHAPER_RATE_UPDATE_MODE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_OFP_RATES_SET_SET_SHAPER_EGQ_SHAPER_RATE_UPDATE_MODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_shaper.egq_shaper.rate_update_mode = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_OFP_RATES_SET_SET_SHAPER_SCH_SHAPER_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_OFP_RATES_SET_SET_SHAPER_SCH_SHAPER_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_shaper.sch_shaper.rate = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_OFP_RATES_SET_SET_SHAPER_SCH_SHAPER_RATE_UPDATE_MODE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_OFP_RATES_SET_SET_SHAPER_SCH_SHAPER_RATE_UPDATE_MODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_shaper.sch_shaper.rate_update_mode = param_val->numeric_equivalent;
  }

  if(prm_ofp_ndx != 0xFFFFFFFF)
  {

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_OFP_RATES_SET_SET_OFP_RATE_TBL_RATES_MAX_BURST_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_OFP_RATES_SET_SET_OFP_RATE_TBL_RATES_MAX_BURST_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ofp_rate_tbl.rates[entry_idx].max_burst = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_OFP_RATES_SET_SET_OFP_RATE_TBL_RATES_EGQ_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_OFP_RATES_SET_SET_OFP_RATE_TBL_RATES_EGQ_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ofp_rate_tbl.rates[entry_idx].egq_rate = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_OFP_RATES_SET_SET_OFP_RATE_TBL_RATES_SCH_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_OFP_RATES_SET_SET_OFP_RATE_TBL_RATES_SCH_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ofp_rate_tbl.rates[entry_idx].sch_rate = (uint32)param_val->value.ulong_value;
  }
  
  }

  /* Call function */
  ret = tmd_ofp_rates_set(
          unit,
          prm_mal_ndx,
          &prm_shaper,
          &prm_ofp_rate_tbl
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ofp_rates_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: get (section ofp_rates)
 */
int
  ui_tmd_api_ofp_rates_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_mal_ndx = 0;
  TMD_OFP_RATES_MAL_SHPR_INFO
    prm_shaper;
  TMD_OFP_RATES_TBL_INFO
    prm_ofp_rate_tbl;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ofp_rates");
  soc_sand_proc_name = "tmd_ofp_rates_get";

  unit = tmd_get_default_unit();
  TMD_OFP_RATES_MAL_SHPR_INFO_clear(&prm_shaper);
  TMD_OFP_RATES_TBL_INFO_clear(&prm_ofp_rate_tbl);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_OFP_RATES_GET_GET_MAL_NDX_ID,1))
  {
    UI_MACROS_GET_NUM_SYM_VAL(PARAM_TMD_OFP_RATES_GET_GET_MAL_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    UI_MACROS_LOAD_LONG_VAL(prm_mal_ndx);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mal_ndx after get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_ofp_rates_get(
          unit,
          prm_mal_ndx,
          &prm_shaper,
          &prm_ofp_rate_tbl
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ofp_rates_get");
    goto exit;
  }

  send_string_to_screen("--> shaper:", TRUE);
  TMD_OFP_RATES_MAL_SHPR_INFO_print(&prm_shaper);

  send_string_to_screen("--> ofp_rate_tbl:", TRUE);
  TMD_OFP_RATES_TBL_INFO_print(&prm_ofp_rate_tbl);


  goto exit;
exit:
  return ui_ret;
}


/********************************************************************
 *  Function handler: update (section ofp_rates)
 */
int
  ui_tmd_api_ofp_rates_update(
    CURRENT_LINE *current_line
  )
{
  uint32
    mal_id = 0;
  uint32
    ret = 0;
  uint32
    prm_ofp_ndx = 0;
  TMD_OFP_RATES_MAL_SHPR_INFO
    prm_shaper;
  TMD_OFP_RATE_INFO
    prm_ofp_rate;
  SOC_PETRA_SW_DB_DEV_EGR_RATE
    sw_db_rates;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ofp_rates");
  soc_sand_proc_name = "tmd_ofp_rates_single_port_set";

  unit = tmd_get_default_unit();
  TMD_OFP_RATES_MAL_SHPR_INFO_clear(&prm_shaper);
  TMD_OFP_RATE_INFO_clear(&prm_ofp_rate);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_OFP_RATES_UPDATE_UPDATE_OFP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_OFP_RATES_UPDATE_UPDATE_OFP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ofp_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter ofp_ndx after update***", TRUE);
    goto exit;
  }
  
  tmd_port_ofp_mal_get(
    unit,
    prm_ofp_ndx,
    &mal_id
    );

   
  soc_petra_sw_db_egr_ports_get(
    unit,
    mal_id,
    prm_ofp_ndx,
    &sw_db_rates
  );

  prm_ofp_rate.egq_rate = sw_db_rates.egq_rates;
  prm_ofp_rate.sch_rate = sw_db_rates.sch_rates;
  prm_ofp_rate.max_burst = sw_db_rates.egq_bursts;
  prm_ofp_rate.port_id = prm_ofp_ndx;

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_OFP_RATES_UPDATE_UPDATE_SHAPER_EGQ_SHAPER_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_OFP_RATES_UPDATE_UPDATE_SHAPER_EGQ_SHAPER_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_shaper.egq_shaper.rate = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_OFP_RATES_UPDATE_UPDATE_SHAPER_EGQ_SHAPER_RATE_UPDATE_MODE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_OFP_RATES_UPDATE_UPDATE_SHAPER_EGQ_SHAPER_RATE_UPDATE_MODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_shaper.egq_shaper.rate_update_mode = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_OFP_RATES_UPDATE_UPDATE_SHAPER_SCH_SHAPER_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_OFP_RATES_UPDATE_UPDATE_SHAPER_SCH_SHAPER_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_shaper.sch_shaper.rate = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_OFP_RATES_UPDATE_UPDATE_SHAPER_SCH_SHAPER_RATE_UPDATE_MODE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_OFP_RATES_UPDATE_UPDATE_SHAPER_SCH_SHAPER_RATE_UPDATE_MODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_shaper.sch_shaper.rate_update_mode = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_OFP_RATES_UPDATE_UPDATE_OFP_RATE_MAX_BURST_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_OFP_RATES_UPDATE_UPDATE_OFP_RATE_MAX_BURST_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ofp_rate.max_burst = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_OFP_RATES_UPDATE_UPDATE_OFP_RATE_EGQ_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_OFP_RATES_UPDATE_UPDATE_OFP_RATE_EGQ_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ofp_rate.egq_rate = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_OFP_RATES_UPDATE_UPDATE_OFP_RATE_SCH_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_OFP_RATES_UPDATE_UPDATE_OFP_RATE_SCH_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ofp_rate.sch_rate = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  ret = tmd_ofp_rates_single_port_set(
          unit,
          prm_ofp_ndx,
          &prm_shaper,
          &prm_ofp_rate
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
      soc_petra_disp_result(ret, "tmd_ofp_rates_single_port_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}


/********************************************************************
 *  Function handler: single_port_get (section ofp_rates)
 */
int
  ui_tmd_api_ofp_rates_single_port_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_ofp_ndx = 0;
  TMD_OFP_RATES_MAL_SHPR_INFO
    prm_shaper;
  TMD_OFP_RATE_INFO
    prm_ofp;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ofp_rates");
  soc_sand_proc_name = "tmd_ofp_rates_single_port_get";

  unit = tmd_get_default_unit();
  TMD_OFP_RATE_INFO_clear(&prm_ofp);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_OFP_RATES_SINGLE_PORT_GET_SINGLE_PORT_GET_OFP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_OFP_RATES_SINGLE_PORT_GET_SINGLE_PORT_GET_OFP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ofp_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter ofp_ndx after single_port_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_ofp_rates_single_port_get(
          unit,
          prm_ofp_ndx,
          &prm_shaper,
          &prm_ofp
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ofp_rates_single_port_get");
    goto exit;
  }

  send_string_to_screen("--> ofp:", TRUE);
  TMD_OFP_RATE_INFO_print(&prm_ofp);
  send_string_to_screen("--> shaper:", TRUE);
  TMD_OFP_RATES_MAL_SHPR_INFO_print(&prm_shaper);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: mal_shaper_set (section ofp_rates)
 */
int
  ui_tmd_api_ofp_rates_mal_shaper_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_mal_ndx = 0;
  TMD_OFP_RATES_MAL_SHPR_INFO
    prm_shaper;
  TMD_OFP_RATES_MAL_SHPR_INFO
    prm_exact_shaper;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ofp_rates");
  soc_sand_proc_name = "tmd_ofp_rates_mal_shaper_set";

  unit = tmd_get_default_unit();
  TMD_OFP_RATES_MAL_SHPR_INFO_clear(&prm_shaper);
  TMD_OFP_RATES_MAL_SHPR_INFO_clear(&prm_exact_shaper);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_OFP_RATES_MAL_SHAPER_SET_MAL_SHAPER_SET_MAL_NDX_ID,1))
  {
    UI_MACROS_GET_NUM_SYM_VAL(PARAM_TMD_OFP_RATES_MAL_SHAPER_SET_MAL_SHAPER_SET_MAL_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    UI_MACROS_LOAD_LONG_VAL(prm_mal_ndx);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mal_ndx after mal_shaper_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = tmd_ofp_rates_mal_shaper_get(
          unit,
          prm_mal_ndx,
          &prm_shaper
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ofp_rates_mal_shaper_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_OFP_RATES_MAL_SHAPER_SET_MAL_SHAPER_SET_SHAPER_EGQ_SHAPER_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_OFP_RATES_MAL_SHAPER_SET_MAL_SHAPER_SET_SHAPER_EGQ_SHAPER_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_shaper.egq_shaper.rate = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_OFP_RATES_MAL_SHAPER_SET_MAL_SHAPER_SET_SHAPER_EGQ_SHAPER_RATE_UPDATE_MODE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_OFP_RATES_MAL_SHAPER_SET_MAL_SHAPER_SET_SHAPER_EGQ_SHAPER_RATE_UPDATE_MODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_shaper.egq_shaper.rate_update_mode = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_OFP_RATES_MAL_SHAPER_SET_MAL_SHAPER_SET_SHAPER_SCH_SHAPER_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_OFP_RATES_MAL_SHAPER_SET_MAL_SHAPER_SET_SHAPER_SCH_SHAPER_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_shaper.sch_shaper.rate = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_OFP_RATES_MAL_SHAPER_SET_MAL_SHAPER_SET_SHAPER_SCH_SHAPER_RATE_UPDATE_MODE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_OFP_RATES_MAL_SHAPER_SET_MAL_SHAPER_SET_SHAPER_SCH_SHAPER_RATE_UPDATE_MODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_shaper.sch_shaper.rate_update_mode = param_val->numeric_equivalent;
  }


  /* Call function */
  ret = tmd_ofp_rates_mal_shaper_set(
          unit,
          prm_mal_ndx,
          &prm_shaper,
          &prm_exact_shaper
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ofp_rates_mal_shaper_set");
    goto exit;
  }

  send_string_to_screen("--> exact_shaper:", TRUE);
  TMD_OFP_RATES_MAL_SHPR_INFO_print(&prm_exact_shaper);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: mal_shaper_get (section ofp_rates)
 */
int
  ui_tmd_api_ofp_rates_mal_shaper_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_mal_ndx = 0;
  TMD_OFP_RATES_MAL_SHPR_INFO
    prm_shaper;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ofp_rates");
  soc_sand_proc_name = "tmd_ofp_rates_mal_shaper_get";

  unit = tmd_get_default_unit();
  TMD_OFP_RATES_MAL_SHPR_INFO_clear(&prm_shaper);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_OFP_RATES_MAL_SHAPER_GET_MAL_SHAPER_GET_MAL_NDX_ID,1))
  {
    UI_MACROS_GET_NUM_SYM_VAL(PARAM_TMD_OFP_RATES_MAL_SHAPER_GET_MAL_SHAPER_GET_MAL_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    UI_MACROS_LOAD_LONG_VAL(prm_mal_ndx);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mal_ndx after mal_shaper_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_ofp_rates_mal_shaper_get(
          unit,
          prm_mal_ndx,
          &prm_shaper
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ofp_rates_mal_shaper_get");
    goto exit;
  }

  send_string_to_screen("--> shaper:", TRUE);
  TMD_OFP_RATES_MAL_SHPR_INFO_print(&prm_shaper);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ofp_fat_pipe_rate_set (section ofp_rates)
 */
int
  ui_tmd_api_ofp_rates_ofp_fat_pipe_rate_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_OFP_FAT_PIPE_RATE_INFO
    prm_fatp_rate;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ofp_rates");
  soc_sand_proc_name = "tmd_ofp_fat_pipe_rate_set";

  unit = tmd_get_default_unit();
  TMD_OFP_FAT_PIPE_RATE_INFO_clear(&prm_fatp_rate);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = tmd_ofp_fat_pipe_rate_get(
          unit,
          &prm_fatp_rate
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ofp_fat_pipe_rate_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_OFP_FAT_PIPE_RATE_SET_OFP_FAT_PIPE_RATE_SET_FATP_RATE_MAX_BURST_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_OFP_FAT_PIPE_RATE_SET_OFP_FAT_PIPE_RATE_SET_FATP_RATE_MAX_BURST_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_fatp_rate.max_burst = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_OFP_FAT_PIPE_RATE_SET_OFP_FAT_PIPE_RATE_SET_FATP_RATE_EGQ_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_OFP_FAT_PIPE_RATE_SET_OFP_FAT_PIPE_RATE_SET_FATP_RATE_EGQ_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_fatp_rate.egq_rate = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_OFP_FAT_PIPE_RATE_SET_OFP_FAT_PIPE_RATE_SET_FATP_RATE_SCH_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_OFP_FAT_PIPE_RATE_SET_OFP_FAT_PIPE_RATE_SET_FATP_RATE_SCH_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_fatp_rate.sch_rate = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_ofp_fat_pipe_rate_set(
          unit,
          &prm_fatp_rate
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ofp_fat_pipe_rate_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ofp_fat_pipe_rate_get (section ofp_rates)
 */
int
  ui_tmd_api_ofp_rates_ofp_fat_pipe_rate_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_OFP_FAT_PIPE_RATE_INFO
    prm_fatp_rate;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ofp_rates");
  soc_sand_proc_name = "tmd_ofp_fat_pipe_rate_get";

  unit = tmd_get_default_unit();
  TMD_OFP_FAT_PIPE_RATE_INFO_clear(&prm_fatp_rate);

  /* Get parameters */

  /* Call function */
  ret = tmd_ofp_fat_pipe_rate_get(
          unit,
          &prm_fatp_rate
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_ofp_fat_pipe_rate_get");
    goto exit;
  }

  send_string_to_screen("--> fatp_rate:", TRUE);
  TMD_OFP_FAT_PIPE_RATE_INFO_print(&prm_fatp_rate);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: update_device_set (section ofp_rates)
 */
int
  ui_tmd_api_ofp_rates_update_device_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint8
    prm_is_device_updated;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ofp_rates");
  soc_sand_proc_name = "tmd_ofp_rates_update_device_set";

  unit = tmd_get_default_unit();

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = tmd_ofp_rates_update_device_get(
          unit,
          &prm_is_device_updated
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** tmd_ofp_rates_update_device_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "tmd_ofp_rates_update_device_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_OFP_RATES_UPDATE_DEVICE_SET_UPDATE_DEVICE_SET_IS_DEVICE_UPDATED_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_OFP_RATES_UPDATE_DEVICE_SET_UPDATE_DEVICE_SET_IS_DEVICE_UPDATED_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_is_device_updated = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_ofp_rates_update_device_set(
          unit,
          prm_is_device_updated
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** tmd_ofp_rates_update_device_set - FAIL", TRUE);
    soc_petra_disp_result(ret, "tmd_ofp_rates_update_device_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: update_device_get (section ofp_rates)
 */
int
  ui_tmd_api_ofp_rates_update_device_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint8
    prm_is_device_updated;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ofp_rates");
  soc_sand_proc_name = "tmd_ofp_rates_update_device_get";

  unit = tmd_get_default_unit();

  /* Get parameters */

  /* Call function */
  ret = tmd_ofp_rates_update_device_get(
          unit,
          &prm_is_device_updated
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** tmd_ofp_rates_update_device_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "tmd_ofp_rates_update_device_get");
    goto exit;
  }

  send_string_to_screen("--> is_device_updated:", TRUE);
  soc_sand_os_printf( "is_device_updated: %u\n\r",prm_is_device_updated);


  goto exit;
exit:
  return ui_ret;
}

#endif

#ifdef UI_END2END_SCHEDULER
/********************************************************************
 *  Function handler: sch_device_rate_entry_set (section end2end_scheduler)
 */
int
  ui_tmd_api_end2end_scheduler_sch_device_rate_entry_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_rci_level_ndx = 0;
  uint32
    prm_nof_active_links_ndx = 0;
  uint32
    prm_rate;
  uint32
    prm_exact_rate;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_end2end_scheduler");
  soc_sand_proc_name = "tmd_sch_device_rate_entry_set";

  unit = tmd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_DEVICE_RATE_ENTRY_SET_SCH_DEVICE_RATE_ENTRY_SET_RCI_LEVEL_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_DEVICE_RATE_ENTRY_SET_SCH_DEVICE_RATE_ENTRY_SET_RCI_LEVEL_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_rci_level_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter rci_level_ndx after sch_device_rate_entry_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_DEVICE_RATE_ENTRY_SET_SCH_DEVICE_RATE_ENTRY_SET_NOF_ACTIVE_LINKS_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_DEVICE_RATE_ENTRY_SET_SCH_DEVICE_RATE_ENTRY_SET_NOF_ACTIVE_LINKS_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_nof_active_links_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter nof_active_links_ndx after sch_device_rate_entry_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = tmd_sch_device_rate_entry_get(
          unit,
          prm_rci_level_ndx,
          prm_nof_active_links_ndx,
          &prm_rate
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_sch_device_rate_entry_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_DEVICE_RATE_ENTRY_SET_SCH_DEVICE_RATE_ENTRY_SET_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_DEVICE_RATE_ENTRY_SET_SCH_DEVICE_RATE_ENTRY_SET_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_rate = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_sch_device_rate_entry_set(
          unit,
          prm_rci_level_ndx,
          prm_nof_active_links_ndx,
          prm_rate,
          &prm_exact_rate
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_sch_device_rate_entry_set");
    goto exit;
  }

  soc_sand_os_printf( "exact_rate: %lu\n\r",prm_exact_rate);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: sch_device_rate_entry_get (section end2end_scheduler)
 */
int
  ui_tmd_api_end2end_scheduler_sch_device_rate_entry_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_rci_level_ndx = 0;
  uint32
    prm_nof_active_links_ndx = 0;
  uint32
    prm_rate;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_end2end_scheduler");
  soc_sand_proc_name = "tmd_sch_device_rate_entry_get";

  unit = tmd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_DEVICE_RATE_ENTRY_GET_SCH_DEVICE_RATE_ENTRY_GET_RCI_LEVEL_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_DEVICE_RATE_ENTRY_GET_SCH_DEVICE_RATE_ENTRY_GET_RCI_LEVEL_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_rci_level_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter rci_level_ndx after sch_device_rate_entry_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_DEVICE_RATE_ENTRY_GET_SCH_DEVICE_RATE_ENTRY_GET_NOF_ACTIVE_LINKS_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_DEVICE_RATE_ENTRY_GET_SCH_DEVICE_RATE_ENTRY_GET_NOF_ACTIVE_LINKS_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_nof_active_links_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter nof_active_links_ndx after sch_device_rate_entry_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_sch_device_rate_entry_get(
          unit,
          prm_rci_level_ndx,
          prm_nof_active_links_ndx,
          &prm_rate
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_sch_device_rate_entry_get");
    goto exit;
  }

  soc_sand_os_printf( "rate: %lu\n\r",prm_rate);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: sch_device_if_weight_idx_set (section end2end_scheduler)
 */
int
  ui_tmd_api_end2end_scheduler_sch_device_if_weight_idx_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_INTERFACE_ID
    prm_if_ndx = 0;
  uint32
    prm_weight_index;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_end2end_scheduler");
  soc_sand_proc_name = "tmd_sch_device_if_weight_idx_set";

  unit = tmd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_DEVICE_IF_WEIGHT_IDX_SET_SCH_DEVICE_IF_WEIGHT_IDX_SET_IF_NDX_ID,1))
  {
    UI_MACROS_GET_NUM_SYM_VAL(PARAM_TMD_SCH_DEVICE_IF_WEIGHT_IDX_SET_SCH_DEVICE_IF_WEIGHT_IDX_SET_IF_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    UI_MACROS_LOAD_LONG_VAL(prm_if_ndx);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter if_ndx after sch_device_if_weight_idx_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = tmd_sch_device_if_weight_idx_get(
          unit,
          prm_if_ndx,
          &prm_weight_index
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_sch_device_if_weight_idx_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_DEVICE_IF_WEIGHT_IDX_SET_SCH_DEVICE_IF_WEIGHT_IDX_SET_WEIGHT_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_DEVICE_IF_WEIGHT_IDX_SET_SCH_DEVICE_IF_WEIGHT_IDX_SET_WEIGHT_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_weight_index = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_sch_device_if_weight_idx_set(
          unit,
          prm_if_ndx,
          prm_weight_index
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_sch_device_if_weight_idx_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: sch_device_if_weight_idx_get (section end2end_scheduler)
 */
int
  ui_tmd_api_end2end_scheduler_sch_device_if_weight_idx_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_INTERFACE_ID
    prm_if_ndx = 0;
  uint32
    prm_weight_index;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_end2end_scheduler");
  soc_sand_proc_name = "tmd_sch_device_if_weight_idx_get";

  unit = tmd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_DEVICE_IF_WEIGHT_IDX_GET_SCH_DEVICE_IF_WEIGHT_IDX_GET_IF_NDX_ID,1))
  {
    UI_MACROS_GET_NUM_SYM_VAL(PARAM_TMD_SCH_DEVICE_IF_WEIGHT_IDX_GET_SCH_DEVICE_IF_WEIGHT_IDX_GET_IF_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    UI_MACROS_LOAD_LONG_VAL(prm_if_ndx);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter if_ndx after sch_device_if_weight_idx_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_sch_device_if_weight_idx_get(
          unit,
          prm_if_ndx,
          &prm_weight_index
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_sch_device_if_weight_idx_get");
    goto exit;
  }

  soc_sand_os_printf( "weight_index: %lu\n\r",prm_weight_index);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: sch_if_weight_conf_set (section end2end_scheduler)
 */
int
  ui_tmd_api_end2end_scheduler_sch_if_weight_conf_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_weight_index = 0xFFFFFFFF;
  TMD_SCH_IF_WEIGHTS
    prm_if_weights;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_end2end_scheduler");
  soc_sand_proc_name = "tmd_sch_if_weight_conf_set";

  unit = tmd_get_default_unit();
  TMD_SCH_IF_WEIGHTS_clear(&prm_if_weights);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = tmd_sch_if_weight_conf_get(
          unit,
          &prm_if_weights
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_sch_if_weight_conf_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_IF_WEIGHT_CONF_SET_SCH_IF_WEIGHT_CONF_SET_IF_WEIGHTS_NOF_ENTIES_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_IF_WEIGHT_CONF_SET_SCH_IF_WEIGHT_CONF_SET_IF_WEIGHTS_NOF_ENTIES_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_if_weights.nof_enties = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_IF_WEIGHT_CONF_SET_SCH_IF_WEIGHT_CONF_SET_IF_WEIGHTS_WEIGHT_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_IF_WEIGHT_CONF_SET_SCH_IF_WEIGHT_CONF_SET_IF_WEIGHTS_WEIGHT_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_weight_index = (uint32)param_val->value.ulong_value;
  }

  if(prm_weight_index != 0xFFFFFFFF)
  {
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_IF_WEIGHT_CONF_SET_SCH_IF_WEIGHT_CONF_SET_IF_WEIGHTS_WEIGHT_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_IF_WEIGHT_CONF_SET_SCH_IF_WEIGHT_CONF_SET_IF_WEIGHTS_WEIGHT_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_if_weights.weight[ prm_weight_index].val = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_IF_WEIGHT_CONF_SET_SCH_IF_WEIGHT_CONF_SET_IF_WEIGHTS_WEIGHT_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_IF_WEIGHT_CONF_SET_SCH_IF_WEIGHT_CONF_SET_IF_WEIGHTS_WEIGHT_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_if_weights.weight[ prm_weight_index].id = (uint32)param_val->value.ulong_value;
  }

  }


  /* Call function */
  ret = tmd_sch_if_weight_conf_set(
          unit,
          &prm_if_weights
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_sch_if_weight_conf_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: sch_if_weight_conf_get (section end2end_scheduler)
 */
int
  ui_tmd_api_end2end_scheduler_sch_if_weight_conf_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_SCH_IF_WEIGHTS
    prm_if_weights;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_end2end_scheduler");
  soc_sand_proc_name = "tmd_sch_if_weight_conf_get";

  unit = tmd_get_default_unit();
  TMD_SCH_IF_WEIGHTS_clear(&prm_if_weights);

  /* Get parameters */

  /* Call function */
  ret = tmd_sch_if_weight_conf_get(
          unit,
          &prm_if_weights
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_sch_if_weight_conf_get");
    goto exit;
  }

  send_string_to_screen("--> if_weights:", TRUE);
  TMD_SCH_IF_WEIGHTS_print(&prm_if_weights);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: sch_flow2se_id (section end2end_scheduler)
 */
int
  ui_tmd_api_end2end_scheduler_sch_flow2se_id(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_SCH_FLOW_ID
    prm_flow_id = 0;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_end2end_scheduler");
  soc_sand_proc_name = "tmd_sch_flow2se_id";

  unit = tmd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_FLOW2SE_ID_SCH_FLOW2SE_ID_FLOW_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_FLOW2SE_ID_SCH_FLOW2SE_ID_FLOW_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_flow_id = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_sch_flow2se_id(
          unit,
          prm_flow_id
        );
  if (!soc_petra_sch_is_flow_id_valid(unit, prm_flow_id))
  {
    soc_sand_os_printf( "The flow id is invalid.");
  }
  else if(ret == TMD_SCH_SE_ID_INVALID)
  {
    soc_sand_os_printf( "Not a scheduler element.");
  }
  else
  {
    soc_sand_os_printf( "Se_id: %lu\n\r",ret);
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: sch_se2flow_id (section end2end_scheduler)
 */
int
  ui_tmd_api_end2end_scheduler_sch_se2flow_id(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_SCH_SE_ID
    prm_se_id = 0;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_end2end_scheduler");
  soc_sand_proc_name = "tmd_sch_se2flow_id";

  unit = tmd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_SE2FLOW_ID_SCH_SE2FLOW_ID_SE_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_SE2FLOW_ID_SCH_SE2FLOW_ID_SE_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_se_id = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_sch_se2flow_id(
          unit,
          prm_se_id
        );
  
  if (!soc_petra_sch_is_se_id_valid(unit, prm_se_id))
  {
    soc_sand_os_printf( "The scheduler element id is invalid.");
  }
  else if(ret == TMD_SCH_FLOW_ID_INVALID)
  {
    soc_sand_os_printf( "The flow id is invalid.");
  }
  else
  {
    soc_sand_os_printf( "Flow_id: %lu\n\r",ret);
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: sch_se2port_id (section end2end_scheduler)
 */
int
  ui_tmd_api_end2end_scheduler_sch_se2port_id(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_SCH_SE_ID
    prm_se_id = 0;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_end2end_scheduler");
  soc_sand_proc_name = "tmd_sch_se2port_id";

  unit = tmd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_SE2PORT_ID_SCH_SE2PORT_ID_SE_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_SE2PORT_ID_SCH_SE2PORT_ID_SE_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_se_id = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_sch_se2port_id(
          unit,
          prm_se_id
        );
  
  if (!soc_petra_sch_is_se_id_valid(unit, prm_se_id))
  {
    soc_sand_os_printf( "The scheduler element id is invalid.");
  }
  else if(ret == TMD_SCH_PORT_ID_INVALID)
  {
    soc_sand_os_printf( "The port id is invalid.");
  }
  else
  {
    soc_sand_os_printf( "Port_id: %lu\n\r",ret);
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: sch_port2se_id (section end2end_scheduler)
 */
int
  ui_tmd_api_end2end_scheduler_sch_port2se_id(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_SCH_PORT_ID
    prm_port_id = 0;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_end2end_scheduler");
  soc_sand_proc_name = "tmd_sch_port2se_id";

  unit = tmd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_PORT2SE_ID_SCH_PORT2SE_ID_PORT_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_PORT2SE_ID_SCH_PORT2SE_ID_PORT_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_id = (TMD_SCH_PORT_ID)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_sch_port2se_id(
          unit,
          prm_port_id
        );
  
  if (!soc_petra_sch_is_port_id_valid(unit, prm_port_id))
  {
    soc_sand_os_printf( "The port id is invalid.");
  }
  else if(ret == TMD_SCH_SE_ID_INVALID)
  {
    soc_sand_os_printf( "Not a scheduler element.");
  }
  else
  {
    soc_sand_os_printf( "Se_id: %lu\n\r",ret);
  }

  goto exit;
exit:
  return ui_ret;
}


/********************************************************************
 *  Function handler: egress_ports_qos_set (section end2end_scheduler)
 */
/*
int
  ui_tmd_api_end2end_scheduler_egress_ports_qos_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_ports_index = 0xFFFFFFFF;
  TMD_EGRESS_PORT_QOS_TABLE
    prm_ports_qos;
  uint32
    prm_ports_index = 0xFFFFFFFF;
  TMD_EGRESS_PORT_QOS_TABLE
    prm_exact_ports_qos;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_end2end_scheduler");
  soc_sand_proc_name = "tmd_egress_ports_qos_set";

  unit = tmd_get_default_unit();
  TMD_EGRESS_PORT_QOS_TABLE_clear(&prm_ports_qos);
  TMD_EGRESS_PORT_QOS_TABLE_clear(&prm_exact_ports_qos);

  * Get parameters *

  * This is a set function, so call GET function first *
  ret = tmd_egress_ports_qos_get(
          unit,
          &prm_ports_qos,
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_egress_ports_qos_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGRESS_PORTS_QOS_SET_EGRESS_PORTS_QOS_SET_PORTS_QOS_PORTS_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_EGRESS_PORTS_QOS_SET_EGRESS_PORTS_QOS_SET_PORTS_QOS_PORTS_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ports_index = (uint32)param_val->value.ulong_value;
  }

  if(prm_ports_index != 0xFFFFFFFF)
  {

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGRESS_PORTS_QOS_SET_EGRESS_PORTS_QOS_SET_PORTS_QOS_PORTS_PRIORITY_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_EGRESS_PORTS_QOS_SET_EGRESS_PORTS_QOS_SET_PORTS_QOS_PORTS_PRIORITY_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ports_qos.ports[ prm_ports_index].priority = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGRESS_PORTS_QOS_SET_EGRESS_PORTS_QOS_SET_PORTS_QOS_PORTS_CREDIT_BANDWIDTH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_EGRESS_PORTS_QOS_SET_EGRESS_PORTS_QOS_SET_PORTS_QOS_PORTS_CREDIT_BANDWIDTH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ports_qos.ports[ prm_ports_index].credit_bandwidth = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGRESS_PORTS_QOS_SET_EGRESS_PORTS_QOS_SET_PORTS_QOS_PORTS_MAX_BANDWIDTH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_EGRESS_PORTS_QOS_SET_EGRESS_PORTS_QOS_SET_PORTS_QOS_PORTS_MAX_BANDWIDTH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ports_qos.ports[ prm_ports_index].max_bandwidth = (uint32)param_val->value.ulong_value;
  }

  }


  * Call function *
  ret = tmd_egress_ports_qos_set(
          unit,
          &prm_ports_qos,
          &prm_exact_ports_qos
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_egress_ports_qos_set");
    goto exit;
  }

  send_string_to_screen("--> exact_ports_qos:", TRUE);
  TMD_EGRESS_PORT_QOS_TABLE_print(&prm_exact_ports_qos);


  goto exit;
exit:
  return ui_ret;
}             */


/********************************************************************
 *  Function handler: egress_ports_qos_get (section end2end_scheduler)
 ********************************************************************
int
  ui_tmd_api_end2end_scheduler_egress_ports_qos_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_EGRESS_PORT_QOS_TABLE
    prm_ports_qos;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_end2end_scheduler");
  soc_sand_proc_name = "tmd_egress_ports_qos_get";

  unit = tmd_get_default_unit();
  TMD_EGRESS_PORT_QOS_TABLE_clear(&prm_ports_qos);

  * Get parameters *

  * Call function *
  ret = tmd_egress_ports_qos_get(
          unit,
          &prm_ports_qos
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_egress_ports_qos_get");
    goto exit;
  }

  send_string_to_screen("--> ports_qos:", TRUE);
  TMD_EGRESS_PORT_QOS_TABLE_print(&prm_ports_qos);


  goto exit;
exit:
  return ui_ret;
}

********************************************************************
 *  Function handler: one_egress_port_qos_set (section end2end_scheduler)
 ********************************************************************
int
  ui_tmd_api_end2end_scheduler_one_egress_port_qos_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_ofp_ndx = 0;
  TMD_EGRESS_PORT_QOS
    prm_port_qos;
  TMD_EGRESS_PORT_QOS
    prm_exact_port_qos;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_end2end_scheduler");
  soc_sand_proc_name = "tmd_one_egress_port_qos_set";

  unit = tmd_get_default_unit();
  TMD_EGRESS_PORT_QOS_clear(&prm_port_qos);
  TMD_EGRESS_PORT_QOS_clear(&prm_exact_port_qos);

   * Get parameters *
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ONE_EGRESS_PORT_QOS_SET_ONE_EGRESS_PORT_QOS_SET_OFP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ONE_EGRESS_PORT_QOS_SET_ONE_EGRESS_PORT_QOS_SET_OFP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ofp_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter ofp_ndx after one_egress_port_qos_set***", TRUE);
    goto exit;
  }


  / * This is a set function, so call GET function first * /
  ret = tmd_one_egress_port_qos_get(
          unit,
          prm_ofp_ndx,
          &prm_port_qos,
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_one_egress_port_qos_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ONE_EGRESS_PORT_QOS_SET_ONE_EGRESS_PORT_QOS_SET_PORT_QOS_PRIORITY_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ONE_EGRESS_PORT_QOS_SET_ONE_EGRESS_PORT_QOS_SET_PORT_QOS_PRIORITY_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_qos.priority = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ONE_EGRESS_PORT_QOS_SET_ONE_EGRESS_PORT_QOS_SET_PORT_QOS_CREDIT_BANDWIDTH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ONE_EGRESS_PORT_QOS_SET_ONE_EGRESS_PORT_QOS_SET_PORT_QOS_CREDIT_BANDWIDTH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_qos.credit_bandwidth = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ONE_EGRESS_PORT_QOS_SET_ONE_EGRESS_PORT_QOS_SET_PORT_QOS_MAX_BANDWIDTH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ONE_EGRESS_PORT_QOS_SET_ONE_EGRESS_PORT_QOS_SET_PORT_QOS_MAX_BANDWIDTH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_qos.max_bandwidth = (uint32)param_val->value.ulong_value;
  }


  / * Call function * /
  ret = tmd_one_egress_port_qos_set(
          unit,
          prm_ofp_ndx,
          &prm_port_qos,
          &prm_exact_port_qos
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_one_egress_port_qos_set");
    goto exit;
  }

  send_string_to_screen("--> exact_port_qos:", TRUE);
  TMD_EGRESS_PORT_QOS_print(&prm_exact_port_qos);


  goto exit;
exit:
  return ui_ret;
}                   */


/********************************************************************
 *  Function handler: one_egress_port_qos_get (section end2end_scheduler)
 */
/*
int
  ui_tmd_api_end2end_scheduler_one_egress_port_qos_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_ofp_ndx = 0;
  TMD_EGRESS_PORT_QOS
    prm_port_qos;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_end2end_scheduler");
  soc_sand_proc_name = "tmd_one_egress_port_qos_get";

  unit = tmd_get_default_unit();
  TMD_EGRESS_PORT_QOS_clear(&prm_port_qos);

  / * Get parameters * /
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ONE_EGRESS_PORT_QOS_GET_ONE_EGRESS_PORT_QOS_GET_OFP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_ONE_EGRESS_PORT_QOS_GET_ONE_EGRESS_PORT_QOS_GET_OFP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ofp_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter ofp_ndx after one_egress_port_qos_get***", TRUE);
    goto exit;
  }


  / * Call function * /
  ret = tmd_one_egress_port_qos_get(
          unit,
          prm_ofp_ndx,
          &prm_port_qos
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_one_egress_port_qos_get");
    goto exit;
  }

  send_string_to_screen("--> port_qos:", TRUE);
  TMD_EGRESS_PORT_QOS_print(&prm_port_qos);


  goto exit;
exit:
  return ui_ret;
}
                   */

/********************************************************************
 *  Function handler: sch_class_type_params_set (section end2end_scheduler)
 */
int
  ui_tmd_api_end2end_scheduler_sch_class_type_params_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_SCH_CL_CLASS_TYPE_ID
    prm_cl_type_ndx = 0;
  uint32
    prm_weight_index = 0xFFFFFFFF;
  TMD_SCH_SE_CL_CLASS_INFO
    prm_class_type;
  TMD_SCH_SE_CL_CLASS_INFO
    prm_exact_class_type;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_end2end_scheduler");
  soc_sand_proc_name = "tmd_sch_class_type_params_set";

  unit = tmd_get_default_unit();
  TMD_SCH_SE_CL_CLASS_INFO_clear(&prm_class_type);
  TMD_SCH_SE_CL_CLASS_INFO_clear(&prm_exact_class_type);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_CLASS_TYPE_PARAMS_SET_SCH_CLASS_TYPE_PARAMS_SET_CL_TYPE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_CLASS_TYPE_PARAMS_SET_SCH_CLASS_TYPE_PARAMS_SET_CL_TYPE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_cl_type_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter cl_type_ndx after sch_class_type_params_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = tmd_sch_class_type_params_get(
          unit,
          prm_cl_type_ndx,
          &prm_class_type
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_sch_class_type_params_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_CLASS_TYPE_PARAMS_SET_SCH_CLASS_TYPE_PARAMS_SET_CLASS_TYPE_ENHANCED_MODE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_SCH_CLASS_TYPE_PARAMS_SET_SCH_CLASS_TYPE_PARAMS_SET_CLASS_TYPE_ENHANCED_MODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_class_type.enhanced_mode = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_CLASS_TYPE_PARAMS_SET_SCH_CLASS_TYPE_PARAMS_SET_CLASS_TYPE_WEIGHT_MODE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_SCH_CLASS_TYPE_PARAMS_SET_SCH_CLASS_TYPE_PARAMS_SET_CLASS_TYPE_WEIGHT_MODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_class_type.weight_mode = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_CLASS_TYPE_PARAMS_SET_SCH_CLASS_TYPE_PARAMS_SET_CLASS_TYPE_WEIGHT_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_CLASS_TYPE_PARAMS_SET_SCH_CLASS_TYPE_PARAMS_SET_CLASS_TYPE_WEIGHT_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_weight_index = (uint32)param_val->value.ulong_value;
  }

  if(prm_weight_index != 0xFFFFFFFF)
  {

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_CLASS_TYPE_PARAMS_SET_SCH_CLASS_TYPE_PARAMS_SET_CLASS_TYPE_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_CLASS_TYPE_PARAMS_SET_SCH_CLASS_TYPE_PARAMS_SET_CLASS_TYPE_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_class_type.weight[ prm_weight_index] = (uint32)param_val->value.ulong_value;
  }

  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_CLASS_TYPE_PARAMS_SET_SCH_CLASS_TYPE_PARAMS_SET_CLASS_TYPE_MODE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_SCH_CLASS_TYPE_PARAMS_SET_SCH_CLASS_TYPE_PARAMS_SET_CLASS_TYPE_MODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_class_type.mode = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_CLASS_TYPE_PARAMS_SET_SCH_CLASS_TYPE_PARAMS_SET_CLASS_TYPE_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_CLASS_TYPE_PARAMS_SET_SCH_CLASS_TYPE_PARAMS_SET_CLASS_TYPE_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_class_type.id = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_sch_class_type_params_set(
          unit,
          prm_cl_type_ndx,
          &prm_class_type,
          &prm_exact_class_type
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_sch_class_type_params_set");
    goto exit;
  }

  send_string_to_screen("--> exact_class_type:", TRUE);
  TMD_SCH_SE_CL_CLASS_INFO_print(&prm_exact_class_type);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: sch_class_type_params_get (section end2end_scheduler)
 */
int
  ui_tmd_api_end2end_scheduler_sch_class_type_params_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_SCH_CL_CLASS_TYPE_ID
    prm_cl_type_ndx = 0;
  TMD_SCH_SE_CL_CLASS_INFO
    prm_class_type;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_end2end_scheduler");
  soc_sand_proc_name = "tmd_sch_class_type_params_get";

  unit = tmd_get_default_unit();
  TMD_SCH_SE_CL_CLASS_INFO_clear(&prm_class_type);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_CLASS_TYPE_PARAMS_GET_SCH_CLASS_TYPE_PARAMS_GET_CL_TYPE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_CLASS_TYPE_PARAMS_GET_SCH_CLASS_TYPE_PARAMS_GET_CL_TYPE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_cl_type_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter cl_type_ndx after sch_class_type_params_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_sch_class_type_params_get(
          unit,
          prm_cl_type_ndx,
          &prm_class_type
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_sch_class_type_params_get");
    goto exit;
  }

  send_string_to_screen("--> class_type:", TRUE);
  TMD_SCH_SE_CL_CLASS_INFO_print(&prm_class_type);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: sch_class_type_params_table_set (section end2end_scheduler)
 */
int
  ui_tmd_api_end2end_scheduler_sch_class_type_params_table_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_class_types_index = 0xFFFFFFFF;
  uint32
    prm_weight_index = 0xFFFFFFFF;
  TMD_SCH_SE_CL_CLASS_TABLE
    prm_sct;
  TMD_SCH_SE_CL_CLASS_TABLE
    prm_exact_sct;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_end2end_scheduler");
  soc_sand_proc_name = "tmd_sch_class_type_params_table_set";

  unit = tmd_get_default_unit();
  TMD_SCH_SE_CL_CLASS_TABLE_clear(&prm_sct);
  TMD_SCH_SE_CL_CLASS_TABLE_clear(&prm_exact_sct);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = tmd_sch_class_type_params_table_get(
          unit,
          &prm_sct
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_sch_class_type_params_table_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_CLASS_TYPE_PARAMS_TABLE_SET_SCH_CLASS_TYPE_PARAMS_TABLE_SET_SCT_CLASS_TYPES_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_CLASS_TYPE_PARAMS_TABLE_SET_SCH_CLASS_TYPE_PARAMS_TABLE_SET_SCT_CLASS_TYPES_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_class_types_index = (uint32)param_val->value.ulong_value;
  }

  if(prm_class_types_index != 0xFFFFFFFF)
  {

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_CLASS_TYPE_PARAMS_TABLE_SET_SCH_CLASS_TYPE_PARAMS_TABLE_SET_SCT_CLASS_TYPES_ENHANCED_MODE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_SCH_CLASS_TYPE_PARAMS_TABLE_SET_SCH_CLASS_TYPE_PARAMS_TABLE_SET_SCT_CLASS_TYPES_ENHANCED_MODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sct.class_types[ prm_class_types_index].enhanced_mode = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_CLASS_TYPE_PARAMS_TABLE_SET_SCH_CLASS_TYPE_PARAMS_TABLE_SET_SCT_CLASS_TYPES_WEIGHT_MODE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_SCH_CLASS_TYPE_PARAMS_TABLE_SET_SCH_CLASS_TYPE_PARAMS_TABLE_SET_SCT_CLASS_TYPES_WEIGHT_MODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sct.class_types[ prm_class_types_index].weight_mode = param_val->numeric_equivalent;
  }

  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_CLASS_TYPE_PARAMS_TABLE_SET_SCH_CLASS_TYPE_PARAMS_TABLE_SET_SCT_CLASS_TYPES_WEIGHT_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_CLASS_TYPE_PARAMS_TABLE_SET_SCH_CLASS_TYPE_PARAMS_TABLE_SET_SCT_CLASS_TYPES_WEIGHT_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_weight_index = (uint32)param_val->value.ulong_value;
  }

  if(prm_weight_index != 0xFFFFFFFF)
  {

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_CLASS_TYPE_PARAMS_TABLE_SET_SCH_CLASS_TYPE_PARAMS_TABLE_SET_SCT_CLASS_TYPES_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_CLASS_TYPE_PARAMS_TABLE_SET_SCH_CLASS_TYPE_PARAMS_TABLE_SET_SCT_CLASS_TYPES_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sct.class_types[ prm_class_types_index].weight[ prm_weight_index] = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_CLASS_TYPE_PARAMS_TABLE_SET_SCH_CLASS_TYPE_PARAMS_TABLE_SET_SCT_CLASS_TYPES_MODE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_SCH_CLASS_TYPE_PARAMS_TABLE_SET_SCH_CLASS_TYPE_PARAMS_TABLE_SET_SCT_CLASS_TYPES_MODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sct.class_types[ prm_class_types_index].mode = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_CLASS_TYPE_PARAMS_TABLE_SET_SCH_CLASS_TYPE_PARAMS_TABLE_SET_SCT_CLASS_TYPES_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_CLASS_TYPE_PARAMS_TABLE_SET_SCH_CLASS_TYPE_PARAMS_TABLE_SET_SCT_CLASS_TYPES_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sct.class_types[ prm_class_types_index].id = (uint32)param_val->value.ulong_value;
  }

  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_CLASS_TYPE_PARAMS_TABLE_SET_SCH_CLASS_TYPE_PARAMS_TABLE_SET_SCT_NOF_CLASS_TYPES_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_CLASS_TYPE_PARAMS_TABLE_SET_SCH_CLASS_TYPE_PARAMS_TABLE_SET_SCT_NOF_CLASS_TYPES_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sct.nof_class_types = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_sch_class_type_params_table_set(
          unit,
          &prm_sct,
          &prm_exact_sct
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_sch_class_type_params_table_set");
    goto exit;
  }

  send_string_to_screen("--> exact_sct:", TRUE);
  TMD_SCH_SE_CL_CLASS_TABLE_print(&prm_exact_sct);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: sch_class_type_params_table_get (section end2end_scheduler)
 */
int
  ui_tmd_api_end2end_scheduler_sch_class_type_params_table_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_SCH_SE_CL_CLASS_TABLE
    prm_sct;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_end2end_scheduler");
  soc_sand_proc_name = "tmd_sch_class_type_params_table_get";

  unit = tmd_get_default_unit();
  TMD_SCH_SE_CL_CLASS_TABLE_clear(&prm_sct);

  /* Get parameters */

  /* Call function */
  ret = tmd_sch_class_type_params_table_get(
          unit,
          &prm_sct
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_sch_class_type_params_table_get");
    goto exit;
  }

  send_string_to_screen("--> sct:", TRUE);
  TMD_SCH_SE_CL_CLASS_TABLE_print(&prm_sct);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: sch_slow_max_rates_set (section end2end_scheduler)
 */
int
  ui_tmd_api_end2end_scheduler_sch_slow_max_rates_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_rates_index = 0xFFFFFFFF;
  TMD_SCH_SLOW_RATE
    prm_slow_max_rates;
  TMD_SCH_SLOW_RATE
    prm_exact_slow_max_rates;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_end2end_scheduler");
  soc_sand_proc_name = "tmd_sch_slow_max_rates_set";

  unit = tmd_get_default_unit();
  TMD_SCH_SLOW_RATE_clear(&prm_slow_max_rates);
  TMD_SCH_SLOW_RATE_clear(&prm_exact_slow_max_rates);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = tmd_sch_slow_max_rates_get(
          unit,
          &prm_slow_max_rates
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_sch_slow_max_rates_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_SLOW_MAX_RATES_SET_SCH_SLOW_MAX_RATES_SET_SLOW_MAX_RATES_RATES_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_SLOW_MAX_RATES_SET_SCH_SLOW_MAX_RATES_SET_SLOW_MAX_RATES_RATES_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_rates_index = (uint32)param_val->value.ulong_value;
  }

  if(prm_rates_index != 0xFFFFFFFF)
  {

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_SLOW_MAX_RATES_SET_SCH_SLOW_MAX_RATES_SET_SLOW_MAX_RATES_RATES_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_SLOW_MAX_RATES_SET_SCH_SLOW_MAX_RATES_SET_SLOW_MAX_RATES_RATES_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_slow_max_rates.rates[ prm_rates_index] = (uint32)param_val->value.ulong_value;
  }

  }


  /* Call function */
  ret = tmd_sch_slow_max_rates_set(
          unit,
          &prm_slow_max_rates,
          &prm_exact_slow_max_rates
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_sch_slow_max_rates_set");
    goto exit;
  }

  send_string_to_screen("--> exact_slow_max_rates:", TRUE);
  TMD_SCH_SLOW_RATE_print(&prm_exact_slow_max_rates);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: sch_slow_max_rates_get (section end2end_scheduler)
 */
int
  ui_tmd_api_end2end_scheduler_sch_slow_max_rates_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_SCH_SLOW_RATE
    prm_slow_max_rates;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_end2end_scheduler");
  soc_sand_proc_name = "tmd_sch_slow_max_rates_get";

  unit = tmd_get_default_unit();
  TMD_SCH_SLOW_RATE_clear(&prm_slow_max_rates);

  /* Get parameters */

  /* Call function */
  ret = tmd_sch_slow_max_rates_get(
          unit,
          &prm_slow_max_rates
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_sch_slow_max_rates_get");
    goto exit;
  }

  send_string_to_screen("--> slow_max_rates:", TRUE);
  TMD_SCH_SLOW_RATE_print(&prm_slow_max_rates);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: sch_port_sched_set (section end2end_scheduler)
 */
int
  ui_tmd_api_end2end_scheduler_sch_port_sched_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_FAP_PORT_ID
    prm_port_ndx = 0;
  TMD_SCH_PORT_INFO
    prm_port_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_end2end_scheduler");
  soc_sand_proc_name = "tmd_sch_port_sched_set";

  unit = tmd_get_default_unit();
  TMD_SCH_PORT_INFO_clear(&prm_port_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_PORT_SCHED_SET_SCH_PORT_SCHED_SET_PORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_PORT_SCHED_SET_SCH_PORT_SCHED_SET_PORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter port_ndx after sch_port_sched_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = tmd_sch_port_sched_get(
          unit,
          prm_port_ndx,
          &prm_port_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_sch_port_sched_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_PORT_SCHED_SET_SCH_PORT_SCHED_SET_PORT_INFO_LOWEST_HP_CLASS_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_SCH_PORT_SCHED_SET_SCH_PORT_SCHED_SET_PORT_INFO_LOWEST_HP_CLASS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_info.lowest_hp_class = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_PORT_SCHED_SET_SCH_PORT_SCHED_SET_PORT_INFO_HR_MODE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_SCH_PORT_SCHED_SET_SCH_PORT_SCHED_SET_PORT_INFO_HR_MODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_info.hr_mode = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_PORT_SCHED_SET_SCH_PORT_SCHED_SET_PORT_INFO_MAX_EXPECTED_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_PORT_SCHED_SET_SCH_PORT_SCHED_SET_PORT_INFO_MAX_EXPECTED_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_info.max_expected_rate = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_PORT_SCHED_SET_SCH_PORT_SCHED_SET_PORT_INFO_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_PORT_SCHED_SET_SCH_PORT_SCHED_SET_PORT_INFO_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_info.enable = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_sch_port_sched_set(
          unit,
          prm_port_ndx,
          &prm_port_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_sch_port_sched_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: sch_port_sched_get (section end2end_scheduler)
 */
int
  ui_tmd_api_end2end_scheduler_sch_port_sched_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_FAP_PORT_ID
    prm_port_ndx = 0;
  TMD_SCH_PORT_INFO
    prm_port_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_end2end_scheduler");
  soc_sand_proc_name = "tmd_sch_port_sched_get";

  unit = tmd_get_default_unit();
  TMD_SCH_PORT_INFO_clear(&prm_port_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_PORT_SCHED_GET_SCH_PORT_SCHED_GET_PORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_PORT_SCHED_GET_SCH_PORT_SCHED_GET_PORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter port_ndx after sch_port_sched_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_sch_port_sched_get(
          unit,
          prm_port_ndx,
          &prm_port_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_sch_port_sched_get");
    goto exit;
  }

  send_string_to_screen("--> port_info:", TRUE);
  TMD_SCH_PORT_INFO_print(&prm_port_info, prm_port_ndx);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: sch_port_hp_class_conf_set (section end2end_scheduler)
 */
int
  ui_tmd_api_end2end_scheduler_sch_port_hp_class_conf_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_lowest_hp_class_index = 0xFFFFFFFF;
  TMD_SCH_PORT_HP_CLASS_INFO
    prm_hp_class_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_end2end_scheduler");
  soc_sand_proc_name = "tmd_sch_port_hp_class_conf_set";

  unit = tmd_get_default_unit();
  TMD_SCH_PORT_HP_CLASS_INFO_clear(&prm_hp_class_info);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = tmd_sch_port_hp_class_conf_get(
            unit,
            &prm_hp_class_info
          );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_sch_port_hp_class_conf_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_PORT_HP_CLASS_CONF_SET_SCH_PORT_HP_CLASS_CONF_SET_HP_CLASS_INFO_LOWEST_HP_CLASS_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_PORT_HP_CLASS_CONF_SET_SCH_PORT_HP_CLASS_CONF_SET_HP_CLASS_INFO_LOWEST_HP_CLASS_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_lowest_hp_class_index = (uint32)param_val->value.ulong_value;
  }

  if(prm_lowest_hp_class_index != 0xFFFFFFFF)
  {
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_PORT_HP_CLASS_CONF_SET_SCH_PORT_HP_CLASS_CONF_SET_HP_CLASS_INFO_LOWEST_HP_CLASS_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_SCH_PORT_HP_CLASS_CONF_SET_SCH_PORT_HP_CLASS_CONF_SET_HP_CLASS_INFO_LOWEST_HP_CLASS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_hp_class_info.lowest_hp_class[ prm_lowest_hp_class_index] = param_val->numeric_equivalent;
  }

  }


  /* Call function */
  ret = tmd_sch_port_hp_class_conf_set(
          unit,
          &prm_hp_class_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_sch_port_hp_class_conf_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: sch_port_hp_class_conf_get (section end2end_scheduler)
 */

int
  ui_tmd_api_end2end_scheduler_sch_port_hp_class_conf_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_SCH_PORT_HP_CLASS_INFO
    prm_hp_class_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_end2end_scheduler");
  soc_sand_proc_name = "tmd_sch_port_hp_class_conf_get";

  unit = tmd_get_default_unit();
  TMD_SCH_PORT_HP_CLASS_INFO_clear(&prm_hp_class_info);

  /* Get parameters */

  /* Call function */
  ret = tmd_sch_port_hp_class_conf_get(
          unit,
          &prm_hp_class_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_sch_port_hp_class_conf_get");
    goto exit;
  }

  send_string_to_screen("--> hp_class_info:", TRUE);
  TMD_SCH_PORT_HP_CLASS_INFO_print(&prm_hp_class_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: sch_aggregate_set (section end2end_scheduler)
 */
int
  ui_tmd_api_end2end_scheduler_sch_aggregate_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_SCH_SE_ID
    prm_se_ndx = 0;
  TMD_SCH_FLOW_ID
    prm_flow_ndx = 0;
  TMD_SCH_SE_INFO
    prm_se;
  uint32
    prm_sub_flow_index = 0xFFFFFFFF;
  TMD_SCH_FLOW
    prm_flow;
  TMD_SCH_FLOW
    prm_exact_flow;
  TMD_SCH_GLOBAL_PER1K_INFO
    global_per1k_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_end2end_scheduler");
  soc_sand_proc_name = "tmd_sch_aggregate_set";

  unit = tmd_get_default_unit();
  TMD_SCH_SE_INFO_clear(&prm_se);
  TMD_SCH_FLOW_clear(&prm_flow);
  TMD_SCH_FLOW_clear(&prm_exact_flow);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_AGGREGATE_SET_SCH_AGGREGATE_SET_SE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_AGGREGATE_SET_SCH_AGGREGATE_SET_SE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_se_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter se_ndx after sch_aggregate_set ***", TRUE);
    goto exit;
  }

  prm_se.id = prm_se_ndx;
  
  prm_se.type = soc_petra_sch_se_get_type_by_id(prm_se_ndx);

  soc_sand_os_printf( "SE Type: %s\n\r", TMD_SCH_SE_TYPE_to_string(prm_se.type));

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_AGGREGATE_SET_SCH_AGGREGATE_SET_FLOW_SUB_FLOW_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_AGGREGATE_SET_SCH_AGGREGATE_SET_FLOW_SUB_FLOW_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sub_flow_index = (uint32)param_val->value.ulong_value;
  }

  /* This is a set function, so call GET function first */
  ret = tmd_sch_aggregate_get(
          unit,
          prm_se_ndx,
          &prm_se,
          &prm_flow
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_sch_aggregate_get");
    goto exit;
  }

  prm_flow.flow_type = TMD_FLOW_AGGREGATE;

  prm_flow_ndx = tmd_sch_se2flow_id(unit,prm_se_ndx);

  ret = tmd_sch_per1k_info_get(
          unit,
          TMD_SCH_FLOW_TO_1K_ID(prm_flow_ndx),
          &global_per1k_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_sch_aggregate_get");
    goto exit;
  }

  prm_flow.sub_flow[0].id = prm_flow_ndx;
  prm_flow.sub_flow[1].id = prm_flow_ndx + (global_per1k_info.is_odd_even?1:2);
  prm_flow.sub_flow[0].is_valid = TRUE;

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_AGGREGATE_SET_SCH_AGGREGATE_SET_SE_GROUP_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_SCH_AGGREGATE_SET_SCH_AGGREGATE_SET_SE_GROUP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_se.group = (TMD_SCH_GROUP)param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_AGGREGATE_SET_SCH_AGGREGATE_SET_SE_IS_DUAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_AGGREGATE_SET_SCH_AGGREGATE_SET_SE_IS_DUAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_se.is_dual = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_AGGREGATE_SET_SCH_AGGREGATE_SET_SE_TYPE_INFO_CL_ID_ID,1))
  {
    if (prm_se.type != TMD_SCH_SE_TYPE_CL)
    {
      send_string_to_screen(" *** Invalid parameter - not a CL scheduling element", TRUE);
      goto exit;
    }
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_AGGREGATE_SET_SCH_AGGREGATE_SET_SE_TYPE_INFO_CL_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_se.type_info.cl.id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_AGGREGATE_SET_SCH_AGGREGATE_SET_SE_TYPE_INFO_HR_MODE_ID,1))
  {
    if (prm_se.type != TMD_SCH_SE_TYPE_HR)
    {
      send_string_to_screen(" *** Invalid parameter not a HR scheduling element", TRUE);
      goto exit;
    }
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_SCH_AGGREGATE_SET_SCH_AGGREGATE_SET_SE_TYPE_INFO_HR_MODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_se.type_info.hr.mode = (TMD_SCH_SE_HR_MODE)param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_AGGREGATE_SET_SCH_AGGREGATE_SET_SE_STATE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_SCH_AGGREGATE_SET_SCH_AGGREGATE_SET_SE_STATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_se.state = (TMD_SCH_SE_STATE)param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_AGGREGATE_SET_SCH_AGGREGATE_SET_FLOW_IS_SLOW_ENABLED_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_AGGREGATE_SET_SCH_AGGREGATE_SET_FLOW_IS_SLOW_ENABLED_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_flow.is_slow_enabled = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_AGGREGATE_SET_SCH_AGGREGATE_SET_FLOW_FLOW_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_SCH_AGGREGATE_SET_SCH_AGGREGATE_SET_FLOW_FLOW_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_flow.flow_type = (TMD_SCH_FLOW_TYPE)param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_AGGREGATE_SET_SCH_AGGREGATE_SET_FLOW_SUB_FLOW_CREDIT_SOURCE_ID_ID,1))
  {
    UI_PARAM_INIT_VALIDATE_OR_EXIT(prm_sub_flow_index, "sub_flow_index");
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_AGGREGATE_SET_SCH_AGGREGATE_SET_FLOW_SUB_FLOW_CREDIT_SOURCE_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_flow.sub_flow[prm_sub_flow_index].credit_source.id = (uint32)param_val->value.ulong_value;
    
    prm_flow.sub_flow[prm_sub_flow_index].credit_source.se_type = soc_petra_sch_se_get_type_by_id(prm_flow.sub_flow[prm_sub_flow_index].credit_source.id);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_AGGREGATE_SET_SCH_AGGREGATE_SET_FLOW_SUB_FLOW_CREDIT_SOURCE_SE_INFO_CL_WEIGHT_ID,1))
  {
    if (prm_flow.sub_flow[prm_sub_flow_index].credit_source.se_type != TMD_SCH_SE_TYPE_CL)
    {
      send_string_to_screen(" *** Invalid parameter - not a CL scheduling element", TRUE);
      goto exit;
    }

    UI_PARAM_INIT_VALIDATE_OR_EXIT(prm_sub_flow_index, "sub_flow_index");
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_AGGREGATE_SET_SCH_AGGREGATE_SET_FLOW_SUB_FLOW_CREDIT_SOURCE_SE_INFO_CL_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_flow.sub_flow[prm_sub_flow_index].credit_source.se_info.cl.weight = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_AGGREGATE_SET_SCH_AGGREGATE_SET_FLOW_SUB_FLOW_CREDIT_SOURCE_SE_INFO_CL_SP_CLASS_ID,1))
  {
    if (prm_flow.sub_flow[prm_sub_flow_index].credit_source.se_type != TMD_SCH_SE_TYPE_CL)
    {
      send_string_to_screen(" *** Invalid parameter - not a CL scheduling element", TRUE);
      goto exit;
    }

    UI_PARAM_INIT_VALIDATE_OR_EXIT(prm_sub_flow_index, "sub_flow_index");
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_SCH_AGGREGATE_SET_SCH_AGGREGATE_SET_FLOW_SUB_FLOW_CREDIT_SOURCE_SE_INFO_CL_SP_CLASS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_flow.sub_flow[prm_sub_flow_index].credit_source.se_info.cl.sp_class = (TMD_SCH_SUB_FLOW_CL_CLASS)param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_AGGREGATE_SET_SCH_AGGREGATE_SET_FLOW_SUB_FLOW_CREDIT_SOURCE_SE_INFO_HR_WEIGHT_ID,1))
  {
    if (prm_flow.sub_flow[prm_sub_flow_index].credit_source.se_type != TMD_SCH_SE_TYPE_HR)
    {
      send_string_to_screen(" *** Invalid parameter - not a HR scheduling element", TRUE);
      goto exit;
    }

    UI_PARAM_INIT_VALIDATE_OR_EXIT(prm_sub_flow_index, "sub_flow_index");
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_AGGREGATE_SET_SCH_AGGREGATE_SET_FLOW_SUB_FLOW_CREDIT_SOURCE_SE_INFO_HR_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_flow.sub_flow[prm_sub_flow_index].credit_source.se_info.hr.weight = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_AGGREGATE_SET_SCH_AGGREGATE_SET_FLOW_SUB_FLOW_CREDIT_SOURCE_SE_INFO_HR_SP_CLASS_ID,1))
  {
    if (prm_flow.sub_flow[prm_sub_flow_index].credit_source.se_type != TMD_SCH_SE_TYPE_HR)
    {
      send_string_to_screen(" *** Invalid parameter - not a HR scheduling element", TRUE);
      goto exit;
    }

    UI_PARAM_INIT_VALIDATE_OR_EXIT(prm_sub_flow_index, "sub_flow_index");
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_SCH_AGGREGATE_SET_SCH_AGGREGATE_SET_FLOW_SUB_FLOW_CREDIT_SOURCE_SE_INFO_HR_SP_CLASS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_flow.sub_flow[prm_sub_flow_index].credit_source.se_info.hr.sp_class = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_AGGREGATE_SET_SCH_AGGREGATE_SET_FLOW_SUB_FLOW_SLOW_RATE_NDX_ID,1))
  {
    UI_PARAM_INIT_VALIDATE_OR_EXIT(prm_sub_flow_index, "sub_flow_index");
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_SCH_AGGREGATE_SET_SCH_AGGREGATE_SET_FLOW_SUB_FLOW_SLOW_RATE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_flow.sub_flow[prm_sub_flow_index].slow_rate_ndx = (TMD_SCH_SLOW_RATE_NDX)param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_AGGREGATE_SET_SCH_AGGREGATE_SET_FLOW_SUB_FLOW_SHAPER_MAX_BURST_ID,1))
  {
    UI_PARAM_INIT_VALIDATE_OR_EXIT(prm_sub_flow_index, "sub_flow_index");
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_AGGREGATE_SET_SCH_AGGREGATE_SET_FLOW_SUB_FLOW_SHAPER_MAX_BURST_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_flow.sub_flow[prm_sub_flow_index].shaper.max_burst = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_AGGREGATE_SET_SCH_AGGREGATE_SET_FLOW_SUB_FLOW_SHAPER_MAX_RATE_ID,1))
  {
    UI_PARAM_INIT_VALIDATE_OR_EXIT(prm_sub_flow_index, "sub_flow_index");
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_AGGREGATE_SET_SCH_AGGREGATE_SET_FLOW_SUB_FLOW_SHAPER_MAX_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_flow.sub_flow[ prm_sub_flow_index].shaper.max_rate = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_AGGREGATE_SET_SCH_AGGREGATE_SET_FLOW_SUB_FLOW_IS_VALID_ID,1))
  {
    UI_PARAM_INIT_VALIDATE_OR_EXIT(prm_sub_flow_index, "sub_flow_index");
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_AGGREGATE_SET_SCH_AGGREGATE_SET_FLOW_SUB_FLOW_IS_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_flow.sub_flow[prm_sub_flow_index].is_valid = (uint8)param_val->value.ulong_value;
  }

  /* Call function */
  ret = tmd_sch_aggregate_set(
          unit,
          prm_se_ndx,
          &prm_se,
          &prm_flow,
          &prm_exact_flow
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_sch_aggregate_set");
    goto exit;
  }

  send_string_to_screen("--> exact_flow:", TRUE);
  TMD_SCH_FLOW_print(&prm_exact_flow, TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: sch_aggregate_get (section end2end_scheduler)
 */
int
  ui_tmd_api_end2end_scheduler_sch_aggregate_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_SCH_SE_ID
    prm_se_ndx = 0;
  TMD_SCH_SE_INFO
    prm_se;
  TMD_SCH_FLOW
    prm_flow;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_end2end_scheduler");
  soc_sand_proc_name = "tmd_sch_aggregate_get";

  unit = tmd_get_default_unit();
  TMD_SCH_SE_INFO_clear(&prm_se);
  TMD_SCH_FLOW_clear(&prm_flow);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_AGGREGATE_GET_SCH_AGGREGATE_GET_SE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_AGGREGATE_GET_SCH_AGGREGATE_GET_SE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_se_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter se_ndx after sch_aggregate_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_sch_aggregate_get(
          unit,
          prm_se_ndx,
          &prm_se,
          &prm_flow
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_sch_aggregate_get");
    goto exit;
  }

  send_string_to_screen("--> se:", TRUE);
  TMD_SCH_SE_INFO_print(&prm_se);

  send_string_to_screen("--> flow:", TRUE);
  TMD_SCH_FLOW_print(&prm_flow, TRUE);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: sch_flow_set (section end2end_scheduler)
 */
int
  ui_tmd_api_end2end_scheduler_sch_flow_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_SCH_FLOW_ID
    prm_flow_ndx = 0;
  uint32
    prm_sub_flow_index = 0xFFFFFFFF;
  TMD_SCH_FLOW
    prm_flow;
  TMD_SCH_FLOW
    prm_exact_flow;
  TMD_SCH_GLOBAL_PER1K_INFO
    global_per1k_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_end2end_scheduler");
  soc_sand_proc_name = "tmd_sch_flow_set";

  unit = tmd_get_default_unit();
  TMD_SCH_FLOW_clear(&prm_flow);
  TMD_SCH_FLOW_clear(&prm_exact_flow);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_FLOW_SET_SCH_FLOW_SET_FLOW_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_FLOW_SET_SCH_FLOW_SET_FLOW_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_flow_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter flow_ndx after sch_flow_set***", TRUE);
    goto exit;
  }

  ret = tmd_sch_per1k_info_get(
          unit,
          TMD_SCH_FLOW_TO_1K_ID(prm_flow_ndx),
          &global_per1k_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_sch_aggregate_get");
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = tmd_sch_flow_get(
          unit,
          prm_flow_ndx,
          &prm_flow
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_sch_flow_get");
    goto exit;
  }

  prm_flow.sub_flow[0].id = prm_flow_ndx;
  prm_flow.sub_flow[1].id = prm_flow_ndx + (global_per1k_info.is_odd_even?1:2);
  prm_flow.sub_flow[0].is_valid = TRUE;

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_FLOW_SET_SCH_FLOW_SET_FLOW_SUB_FLOW_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_FLOW_SET_SCH_FLOW_SET_FLOW_SUB_FLOW_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sub_flow_index = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_FLOW_SET_SCH_FLOW_SET_FLOW_IS_SLOW_ENABLED_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_FLOW_SET_SCH_FLOW_SET_FLOW_IS_SLOW_ENABLED_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_flow.is_slow_enabled = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_FLOW_SET_SCH_FLOW_SET_FLOW_FLOW_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_SCH_FLOW_SET_SCH_FLOW_SET_FLOW_FLOW_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_flow.flow_type = (TMD_SCH_FLOW_TYPE)param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_FLOW_SET_SCH_FLOW_SET_FLOW_SUB_FLOW_CREDIT_SOURCE_ID_ID,1))
  {
    UI_PARAM_INIT_VALIDATE_OR_EXIT(prm_sub_flow_index, "sub_flow_index");
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_FLOW_SET_SCH_FLOW_SET_FLOW_SUB_FLOW_CREDIT_SOURCE_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_flow.sub_flow[prm_sub_flow_index].credit_source.id = (uint32)param_val->value.ulong_value;
    
    prm_flow.sub_flow[prm_sub_flow_index].credit_source.se_type = soc_petra_sch_se_get_type_by_id(prm_flow.sub_flow[prm_sub_flow_index].credit_source.id);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_FLOW_SET_SCH_FLOW_SET_FLOW_SUB_FLOW_CREDIT_SOURCE_SE_INFO_CL_WEIGHT_ID,1))
  {
    if (prm_flow.sub_flow[prm_sub_flow_index].credit_source.se_type != TMD_SCH_SE_TYPE_CL)
    {
      send_string_to_screen(" *** Invalid parameter - not a CL scheduling element", TRUE);
      goto exit;
    }

    UI_PARAM_INIT_VALIDATE_OR_EXIT(prm_sub_flow_index, "sub_flow_index");
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_FLOW_SET_SCH_FLOW_SET_FLOW_SUB_FLOW_CREDIT_SOURCE_SE_INFO_CL_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_flow.sub_flow[prm_sub_flow_index].credit_source.se_info.cl.weight = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_FLOW_SET_SCH_FLOW_SET_FLOW_SUB_FLOW_CREDIT_SOURCE_SE_INFO_CL_SP_CLASS_ID,1))
  {
    if (prm_flow.sub_flow[prm_sub_flow_index].credit_source.se_type != TMD_SCH_SE_TYPE_CL)
    {
      send_string_to_screen(" *** Invalid parameter - not a CL scheduling element", TRUE);
      goto exit;
    }

    UI_PARAM_INIT_VALIDATE_OR_EXIT(prm_sub_flow_index, "sub_flow_index");
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_SCH_FLOW_SET_SCH_FLOW_SET_FLOW_SUB_FLOW_CREDIT_SOURCE_SE_INFO_CL_SP_CLASS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_flow.sub_flow[prm_sub_flow_index].credit_source.se_info.cl.sp_class = (TMD_SCH_SUB_FLOW_CL_CLASS)param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_FLOW_SET_SCH_FLOW_SET_FLOW_SUB_FLOW_CREDIT_SOURCE_SE_INFO_HR_WEIGHT_ID,1))
  {
    if (prm_flow.sub_flow[prm_sub_flow_index].credit_source.se_type != TMD_SCH_SE_TYPE_HR)
    {
      send_string_to_screen(" *** Invalid parameter - not a HR scheduling element", TRUE);
      goto exit;
    }

    UI_PARAM_INIT_VALIDATE_OR_EXIT(prm_sub_flow_index, "sub_flow_index");
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_FLOW_SET_SCH_FLOW_SET_FLOW_SUB_FLOW_CREDIT_SOURCE_SE_INFO_HR_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_flow.sub_flow[prm_sub_flow_index].credit_source.se_info.hr.weight = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_FLOW_SET_SCH_FLOW_SET_FLOW_SUB_FLOW_CREDIT_SOURCE_SE_INFO_HR_SP_CLASS_ID,1))
  {
    if (prm_flow.sub_flow[prm_sub_flow_index].credit_source.se_type != TMD_SCH_SE_TYPE_HR)
    {
      send_string_to_screen(" *** Invalid parameter - not a HR scheduling element", TRUE);
      goto exit;
    }

    UI_PARAM_INIT_VALIDATE_OR_EXIT(prm_sub_flow_index, "sub_flow_index");
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_SCH_FLOW_SET_SCH_FLOW_SET_FLOW_SUB_FLOW_CREDIT_SOURCE_SE_INFO_HR_SP_CLASS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_flow.sub_flow[prm_sub_flow_index].credit_source.se_info.hr.sp_class = (TMD_SCH_SUB_FLOW_HR_CLASS)param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_FLOW_SET_SCH_FLOW_SET_FLOW_SUB_FLOW_SLOW_RATE_NDX_ID,1))
  {
    UI_PARAM_INIT_VALIDATE_OR_EXIT(prm_sub_flow_index, "sub_flow_index");
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_SCH_FLOW_SET_SCH_FLOW_SET_FLOW_SUB_FLOW_SLOW_RATE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_flow.sub_flow[prm_sub_flow_index].slow_rate_ndx = (TMD_SCH_SLOW_RATE_NDX)param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_FLOW_SET_SCH_FLOW_SET_FLOW_SUB_FLOW_SHAPER_MAX_BURST_ID,1))
  {
    UI_PARAM_INIT_VALIDATE_OR_EXIT(prm_sub_flow_index, "sub_flow_index");
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_FLOW_SET_SCH_FLOW_SET_FLOW_SUB_FLOW_SHAPER_MAX_BURST_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_flow.sub_flow[prm_sub_flow_index].shaper.max_burst = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_FLOW_SET_SCH_FLOW_SET_FLOW_SUB_FLOW_SHAPER_MAX_RATE_ID,1))
  {
    UI_PARAM_INIT_VALIDATE_OR_EXIT(prm_sub_flow_index, "sub_flow_index");
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_FLOW_SET_SCH_FLOW_SET_FLOW_SUB_FLOW_SHAPER_MAX_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_flow.sub_flow[prm_sub_flow_index].shaper.max_rate = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_FLOW_SET_SCH_FLOW_SET_FLOW_SUB_FLOW_IS_VALID_ID,1))
  {
    UI_PARAM_INIT_VALIDATE_OR_EXIT(prm_sub_flow_index, "sub_flow_index");
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_FLOW_SET_SCH_FLOW_SET_FLOW_SUB_FLOW_IS_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_flow.sub_flow[prm_sub_flow_index].is_valid = (uint8)param_val->value.ulong_value;
  }

  /* Call function */
  ret = tmd_sch_flow_set(
          unit,
          prm_flow_ndx,
          &prm_flow,
          &prm_exact_flow
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_sch_flow_set");
    goto exit;
  }

  send_string_to_screen("--> exact_flow:", TRUE);
  TMD_SCH_FLOW_print(&prm_exact_flow, TRUE);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: sch_flow_get (section end2end_scheduler)
 */
int
  ui_tmd_api_end2end_scheduler_sch_flow_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_SCH_FLOW_ID
    prm_flow_ndx = 0;
  TMD_SCH_FLOW
    prm_flow;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_end2end_scheduler");
  soc_sand_proc_name = "tmd_sch_flow_get";

  unit = tmd_get_default_unit();
  TMD_SCH_FLOW_clear(&prm_flow);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_FLOW_GET_SCH_FLOW_GET_FLOW_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_FLOW_GET_SCH_FLOW_GET_FLOW_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_flow_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter flow_ndx after sch_flow_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = tmd_sch_flow_get(
          unit,
          prm_flow_ndx,
          &prm_flow
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_sch_flow_get");
    goto exit;
  }

  send_string_to_screen("--> flow:", TRUE);
  TMD_SCH_FLOW_print(&prm_flow, TRUE);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: print_flow_and_up (section end2end_scheduler)
 */
int
  ui_tmd_api_end2end_scheduler_print_flow_and_up(
    CURRENT_LINE *current_line
  )
{
  TMD_SCH_FLOW_ID
    prm_flow_ndx = 0;
  TMD_SCH_FLOW
    prm_flow;
  uint8
    print_status = 1;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_end2end_scheduler");
  soc_sand_proc_name = "print_flow_and_up";

  unit = tmd_get_default_unit();
  TMD_SCH_FLOW_clear(&prm_flow);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_FLOW_GET_SCH_FLOW_GET_FLOW_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_FLOW_GET_SCH_FLOW_GET_FLOW_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_flow_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter flow_ndx after print_flow_and_up***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_FLOW_GET_SCH_FLOW_GET_PRINT_STATUS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_FLOW_GET_SCH_FLOW_GET_PRINT_STATUS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    print_status = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  tmd_sch_flow_and_up_print(
    unit,
    prm_flow_ndx,
    print_status,
    FALSE
   );

  goto exit;
exit:
  return ui_ret;
}


/********************************************************************
 *  Function handler: sch_flow_status_set (section end2end_scheduler)
 */
int
  ui_tmd_api_end2end_scheduler_sch_flow_status_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_SCH_FLOW_ID
    prm_flow_ndx = 0;
  TMD_SCH_FLOW_STATUS
    prm_state = TMD_SCH_FLOW_OFF;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_end2end_scheduler");
  soc_sand_proc_name = "tmd_sch_flow_status_set";

  unit = tmd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_FLOW_STATUS_SET_SCH_FLOW_STATUS_SET_FLOW_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_FLOW_STATUS_SET_SCH_FLOW_STATUS_SET_FLOW_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_flow_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter flow_ndx after sch_flow_status_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_FLOW_STATUS_SET_SCH_FLOW_STATUS_SET_STATE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_SCH_FLOW_STATUS_SET_SCH_FLOW_STATUS_SET_STATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_state = (TMD_SCH_FLOW_STATUS)param_val->numeric_equivalent;
  }


  /* Call function */
  ret = tmd_sch_flow_status_set(
          unit,
          prm_flow_ndx,
          prm_state
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_sch_flow_status_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}


/********************************************************************
 *  Function handler: sch_per1k_info_set (section end2end_scheduler)
 */
int
  ui_tmd_api_end2end_scheduler_sch_per1k_info_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_k_flow_ndx = 0;
  TMD_SCH_GLOBAL_PER1K_INFO
    prm_per1k_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_end2end_scheduler");
  soc_sand_proc_name = "tmd_sch_per1k_info_set";

  unit = tmd_get_default_unit();
  TMD_SCH_GLOBAL_PER1K_INFO_clear(&prm_per1k_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_PER1K_INFO_SET_SCH_PER1K_INFO_SET_K_FLOW_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_PER1K_INFO_SET_SCH_PER1K_INFO_SET_K_FLOW_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_k_flow_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter k_flow_ndx after sch_per1k_info_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = tmd_sch_per1k_info_get(
          unit,
          prm_k_flow_ndx,
          &prm_per1k_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_sch_per1k_info_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_PER1K_INFO_SET_SCH_PER1K_INFO_SET_PER1K_INFO_IS_CL_CIR_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_PER1K_INFO_SET_SCH_PER1K_INFO_SET_PER1K_INFO_IS_CL_CIR_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_per1k_info.is_cl_cir = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_PER1K_INFO_SET_SCH_PER1K_INFO_SET_PER1K_INFO_IS_ODD_EVEN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_PER1K_INFO_SET_SCH_PER1K_INFO_SET_PER1K_INFO_IS_ODD_EVEN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_per1k_info.is_odd_even = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_PER1K_INFO_SET_SCH_PER1K_INFO_SET_PER1K_INFO_IS_INTERDIGITATED_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_PER1K_INFO_SET_SCH_PER1K_INFO_SET_PER1K_INFO_IS_INTERDIGITATED_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_per1k_info.is_interdigitated = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_sch_per1k_info_set(
          unit,
          prm_k_flow_ndx,
          &prm_per1k_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_sch_per1k_info_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: sch_per1k_info_get (section end2end_scheduler)
 */
int
  ui_tmd_api_end2end_scheduler_sch_per1k_info_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_k_flow_ndx = 0;
  TMD_SCH_GLOBAL_PER1K_INFO
    prm_per1k_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_end2end_scheduler");
  soc_sand_proc_name = "tmd_sch_per1k_info_get";

  unit = tmd_get_default_unit();
  TMD_SCH_GLOBAL_PER1K_INFO_clear(&prm_per1k_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_PER1K_INFO_GET_SCH_PER1K_INFO_GET_K_FLOW_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_PER1K_INFO_GET_SCH_PER1K_INFO_GET_K_FLOW_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_k_flow_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter k_flow_ndx after sch_per1k_info_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_sch_per1k_info_get(
          unit,
          prm_k_flow_ndx,
          &prm_per1k_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_sch_per1k_info_get");
    goto exit;
  }

  send_string_to_screen("--> per1k_info:", TRUE);
  TMD_SCH_GLOBAL_PER1K_INFO_print(&prm_per1k_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: sch_flow_to_queue_mapping_set (section end2end_scheduler)
 */
int
  ui_tmd_api_end2end_scheduler_sch_flow_to_queue_mapping_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_quartet_ndx,
    flow_ndx,
    k_flow_ndx = 0;
  uint32
    prm_nof_quartets_to_map;
  TMD_SCH_QUARTET_MAPPING_INFO
    prm_quartet_flow_info;
  TMD_SCH_GLOBAL_PER1K_INFO
    per1k_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_end2end_scheduler");
  soc_sand_proc_name = "tmd_sch_flow_to_queue_mapping_set";

  unit = tmd_get_default_unit();
  TMD_SCH_QUARTET_MAPPING_INFO_clear(&prm_quartet_flow_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_FLOW_TO_QUEUE_MAPPING_SET_SCH_FLOW_TO_QUEUE_MAPPING_SET_QUARTET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_FLOW_TO_QUEUE_MAPPING_SET_SCH_FLOW_TO_QUEUE_MAPPING_SET_QUARTET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_quartet_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter quartet_ndx after sch_flow_to_queue_mapping_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = tmd_sch_flow_to_queue_mapping_get(
          unit,
          prm_quartet_ndx,
          &prm_quartet_flow_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_sch_flow_to_queue_mapping_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_FLOW_TO_QUEUE_MAPPING_SET_SCH_FLOW_TO_QUEUE_MAPPING_SET_QUARTET_FLOW_INFO_FIP_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_FLOW_TO_QUEUE_MAPPING_SET_SCH_FLOW_TO_QUEUE_MAPPING_SET_QUARTET_FLOW_INFO_FIP_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_quartet_flow_info.fip_id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_FLOW_TO_QUEUE_MAPPING_SET_SCH_FLOW_TO_QUEUE_MAPPING_SET_QUARTET_FLOW_INFO_IS_COMPOSITE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_FLOW_TO_QUEUE_MAPPING_SET_SCH_FLOW_TO_QUEUE_MAPPING_SET_QUARTET_FLOW_INFO_IS_COMPOSITE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_quartet_flow_info.is_composite = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_FLOW_TO_QUEUE_MAPPING_SET_SCH_FLOW_TO_QUEUE_MAPPING_SET_QUARTET_FLOW_INFO_BASE_QUEUE_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_FLOW_TO_QUEUE_MAPPING_SET_SCH_FLOW_TO_QUEUE_MAPPING_SET_QUARTET_FLOW_INFO_BASE_QUEUE_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_quartet_flow_info.base_q_qrtt_id = (uint32)param_val->value.ulong_value;
  }


  flow_ndx = SOC_PETRA_SCH_QRTT_TO_FLOW_ID(prm_quartet_ndx);
  k_flow_ndx = SOC_PETRA_SCH_FLOW_TO_1K_ID(flow_ndx);
  
  ret = soc_petra_sch_per1k_info_get_unsafe(
          unit,
          k_flow_ndx,
          &per1k_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_sch_per1k_info_get_unsafe");
    goto exit;
  }

  
  ret = soc_petra_sch_nof_quartets_to_map_get(
          unit,
          per1k_info.is_interdigitated,
          prm_quartet_flow_info.is_composite,
          &prm_nof_quartets_to_map
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_sch_nof_quartets_to_map_get");
    goto exit;
  }
  /* Call function */
  ret = tmd_sch_flow_to_queue_mapping_set(
          unit,
          prm_quartet_ndx,
          prm_nof_quartets_to_map,
          &prm_quartet_flow_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_sch_flow_to_queue_mapping_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}


/********************************************************************
 *  Function handler: sch_flow_to_queue_mapping_get (section end2end_scheduler)
 */
int
  ui_tmd_api_end2end_scheduler_sch_flow_to_queue_mapping_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_quartet_ndx = 0;
  TMD_SCH_QUARTET_MAPPING_INFO
    prm_quartet_flow_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_end2end_scheduler");
  soc_sand_proc_name = "tmd_sch_flow_to_queue_mapping_get";

  unit = tmd_get_default_unit();
  TMD_SCH_QUARTET_MAPPING_INFO_clear(&prm_quartet_flow_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_FLOW_TO_QUEUE_MAPPING_GET_SCH_FLOW_TO_QUEUE_MAPPING_GET_QUARTET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SCH_FLOW_TO_QUEUE_MAPPING_GET_SCH_FLOW_TO_QUEUE_MAPPING_GET_QUARTET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_quartet_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter quartet_ndx after sch_flow_to_queue_mapping_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_sch_flow_to_queue_mapping_get(
          unit,
          prm_quartet_ndx,
          &prm_quartet_flow_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_sch_flow_to_queue_mapping_get");
    goto exit;
  }


  send_string_to_screen("--> quartet_flow_info:", TRUE);
  TMD_SCH_QUARTET_MAPPING_INFO_print(&prm_quartet_flow_info);


  goto exit;
exit:
  return ui_ret;
}

#endif

#ifdef UI_MULTICAST_INGRESS
/********************************************************************
 *  Function handler: mult_ing_traffic_class_map_set (section multicast_ingress)
 */
int
  ui_tmd_api_multicast_ingress_mult_ing_traffic_class_map_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_map_index = 0xFFFFFFFF;
  TMD_MULT_ING_TR_CLS_MAP
    prm_map;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_multicast_ingress");
  soc_sand_proc_name = "tmd_mult_ing_traffic_class_map_set";

  unit = tmd_get_default_unit();
  TMD_MULT_ING_TR_CLS_MAP_clear(&prm_map);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = tmd_mult_ing_traffic_class_map_get(
          unit,
          &prm_map
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_mult_ing_traffic_class_map_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_ING_TRAFFIC_CLASS_MAP_SET_MULT_ING_TRAFFIC_CLASS_MAP_SET_MAP_MAP_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_ING_TRAFFIC_CLASS_MAP_SET_MULT_ING_TRAFFIC_CLASS_MAP_SET_MAP_MAP_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_map_index = (uint32)param_val->value.ulong_value;
  }

  if(prm_map_index != 0xFFFFFFFF)
  {

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_ING_TRAFFIC_CLASS_MAP_SET_MULT_ING_TRAFFIC_CLASS_MAP_SET_MAP_MAP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_ING_TRAFFIC_CLASS_MAP_SET_MULT_ING_TRAFFIC_CLASS_MAP_SET_MAP_MAP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_map.map[ prm_map_index] = (TMD_TR_CLS)param_val->value.ulong_value;
  }

  }


  /* Call function */
  ret = tmd_mult_ing_traffic_class_map_set(
          unit,
          &prm_map
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_mult_ing_traffic_class_map_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: mult_ing_traffic_class_map_get (section multicast_ingress)
 */
int
  ui_tmd_api_multicast_ingress_mult_ing_traffic_class_map_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_MULT_ING_TR_CLS_MAP
    prm_map;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_multicast_ingress");
  soc_sand_proc_name = "tmd_mult_ing_traffic_class_map_get";

  unit = tmd_get_default_unit();
  TMD_MULT_ING_TR_CLS_MAP_clear(&prm_map);

  /* Get parameters */

  /* Call function */
  ret = tmd_mult_ing_traffic_class_map_get(
          unit,
          &prm_map
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_mult_ing_traffic_class_map_get");
    goto exit;
  }

  send_string_to_screen("--> map:", TRUE);
  TMD_MULT_ING_TR_CLS_MAP_print(&prm_map);


  goto exit;
exit:
  return ui_ret;
}


/********************************************************************
 *  Function handler: mult_ing_group_update (section multicast_ingress)
 */
int
  ui_tmd_api_multicast_ingress_mult_ing_group_update(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_MULT_ID
    prm_multicast_id_ndx = 0;
  TMD_MULT_ING_ENTRY
    prm_mc_group;
  uint32
    prm_mc_group_size = 0;
  uint8
    prm_insufficient_memory;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_multicast_ingress");
  soc_sand_proc_name = "tmd_mult_ing_group_update";

  unit = tmd_get_default_unit();
  TMD_MULT_ING_ENTRY_clear(&prm_mc_group);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_ING_GROUP_UPDATE_MULT_ING_GROUP_UPDATE_MULTICAST_ID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_ING_GROUP_UPDATE_MULT_ING_GROUP_UPDATE_MULTICAST_ID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_id_ndx = (TMD_MULT_ID)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter multicast_id_ndx after mult_ing_group_update***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_ING_GROUP_UPDATE_MULT_ING_GROUP_UPDATE_MC_GROUP_OUTLIF_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_ING_GROUP_UPDATE_MULT_ING_GROUP_UPDATE_MC_GROUP_OUTLIF_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mc_group.cud = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_ING_GROUP_UPDATE_MULT_ING_GROUP_UPDATE_MC_GROUP_DESTINATION_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_ING_GROUP_UPDATE_MULT_ING_GROUP_UPDATE_MC_GROUP_DESTINATION_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mc_group.destination.id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_ING_GROUP_UPDATE_MULT_ING_GROUP_UPDATE_MC_GROUP_DESTINATION_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_MULT_ING_GROUP_UPDATE_MULT_ING_GROUP_UPDATE_MC_GROUP_DESTINATION_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mc_group.destination.type = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_ING_GROUP_UPDATE_MULT_ING_GROUP_UPDATE_MC_GROUP_SIZE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_ING_GROUP_UPDATE_MULT_ING_GROUP_UPDATE_MC_GROUP_SIZE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mc_group_size = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_mult_ing_group_update(
          unit,
          prm_multicast_id_ndx,
          &prm_mc_group,
          prm_mc_group_size,
          &prm_insufficient_memory
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_mult_ing_group_update");
    goto exit;
  }

  soc_sand_os_printf( "insufficient_memory: %u\n\r",prm_insufficient_memory);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: mult_ing_group_close (section multicast_ingress)
 */
int
  ui_tmd_api_multicast_ingress_mult_ing_group_close(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_MULT_ID
    prm_multicast_id_ndx = 0;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_multicast_ingress");
  soc_sand_proc_name = "tmd_mult_ing_group_close";

  unit = tmd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_ING_GROUP_CLOSE_MULT_ING_GROUP_CLOSE_MULTICAST_ID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_ING_GROUP_CLOSE_MULT_ING_GROUP_CLOSE_MULTICAST_ID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_id_ndx = (TMD_MULT_ID)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter multicast_id_ndx after mult_ing_group_close***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_mult_ing_group_close(
          unit,
          prm_multicast_id_ndx
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_mult_ing_group_close");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: mult_ing_destination_add (section multicast_ingress)
 */
int
  ui_tmd_api_multicast_ingress_mult_ing_destination_add(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_MULT_ID
    prm_multicast_id_ndx = 0;
  TMD_MULT_ING_ENTRY
    prm_entry;
  uint8
    prm_insufficient_memory;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_multicast_ingress");
  soc_sand_proc_name = "tmd_mult_ing_destination_add";

  unit = tmd_get_default_unit();
  TMD_MULT_ING_ENTRY_clear(&prm_entry);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_ING_DESTINATION_ADD_MULT_ING_DESTINATION_ADD_MULTICAST_ID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_ING_DESTINATION_ADD_MULT_ING_DESTINATION_ADD_MULTICAST_ID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_id_ndx = (TMD_MULT_ID)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter multicast_id_ndx after mult_ing_destination_add***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_ING_DESTINATION_ADD_MULT_ING_DESTINATION_ADD_ENTRY_OUTLIF_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_ING_DESTINATION_ADD_MULT_ING_DESTINATION_ADD_ENTRY_OUTLIF_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry.cud = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_ING_DESTINATION_ADD_MULT_ING_DESTINATION_ADD_ENTRY_DESTINATION_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_ING_DESTINATION_ADD_MULT_ING_DESTINATION_ADD_ENTRY_DESTINATION_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry.destination.id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_ING_DESTINATION_ADD_MULT_ING_DESTINATION_ADD_ENTRY_DESTINATION_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_MULT_ING_DESTINATION_ADD_MULT_ING_DESTINATION_ADD_ENTRY_DESTINATION_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry.destination.type = param_val->numeric_equivalent;
  }


  /* Call function */
  ret = tmd_mult_ing_destination_add(
          unit,
          prm_multicast_id_ndx,
          &prm_entry,
          &prm_insufficient_memory
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_mult_ing_destination_add");
    goto exit;
  }

  soc_sand_os_printf( "insufficient_memory: %u\n\r",prm_insufficient_memory);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: mult_ing_destination_remove (section multicast_ingress)
 */
int
  ui_tmd_api_multicast_ingress_mult_ing_destination_remove(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_MULT_ID
    prm_multicast_id_ndx = 0;
  TMD_MULT_ING_ENTRY
    prm_entry;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_multicast_ingress");
  soc_sand_proc_name = "tmd_mult_ing_destination_remove";

  unit = tmd_get_default_unit();
  TMD_MULT_ING_ENTRY_clear(&prm_entry);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_ING_DESTINATION_REMOVE_MULT_ING_DESTINATION_REMOVE_MULTICAST_ID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_ING_DESTINATION_REMOVE_MULT_ING_DESTINATION_REMOVE_MULTICAST_ID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_id_ndx = (TMD_MULT_ID)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter multicast_id_ndx after mult_ing_destination_remove***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_ING_DESTINATION_REMOVE_MULT_ING_DESTINATION_REMOVE_ENTRY_OUTLIF_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_ING_DESTINATION_REMOVE_MULT_ING_DESTINATION_REMOVE_ENTRY_OUTLIF_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry.cud = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_ING_DESTINATION_REMOVE_MULT_ING_DESTINATION_REMOVE_ENTRY_DESTINATION_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_ING_DESTINATION_REMOVE_MULT_ING_DESTINATION_REMOVE_ENTRY_DESTINATION_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry.destination.id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_ING_DESTINATION_REMOVE_MULT_ING_DESTINATION_REMOVE_ENTRY_DESTINATION_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_MULT_ING_DESTINATION_REMOVE_MULT_ING_DESTINATION_REMOVE_ENTRY_DESTINATION_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry.destination.type = param_val->numeric_equivalent;
  }


  /* Call function */
  ret = tmd_mult_ing_destination_remove(
          unit,
          prm_multicast_id_ndx,
          &prm_entry
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_mult_ing_destination_remove");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: mult_ing_group_size_get (section multicast_ingress)
 */
int
  ui_tmd_api_multicast_ingress_mult_ing_group_size_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_MULT_ID
    prm_multicast_id_ndx = 0;
  uint32
    prm_mc_group_size;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_multicast_ingress");
  soc_sand_proc_name = "tmd_mult_ing_group_size_get";

  unit = tmd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_ING_GROUP_SIZE_GET_MULT_ING_GROUP_SIZE_GET_MULTICAST_ID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_ING_GROUP_SIZE_GET_MULT_ING_GROUP_SIZE_GET_MULTICAST_ID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_id_ndx = (TMD_MULT_ID)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter multicast_id_ndx after mult_ing_group_size_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_mult_ing_group_size_get(
          unit,
          prm_multicast_id_ndx,
          &prm_mc_group_size
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_mult_ing_group_size_get");
    goto exit;
  }

  soc_sand_os_printf( "mc_group_size: %d\n\r",prm_mc_group_size);


  goto exit;
exit:
  return ui_ret;
}


/********************************************************************
 *  Function handler: mult_ing_all_groups_close (section multicast_ingress)
 */
int
  ui_tmd_api_multicast_ingress_mult_ing_all_groups_close(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_multicast_ingress");
  soc_sand_proc_name = "tmd_mult_ing_all_groups_close";

  unit = tmd_get_default_unit();

  /* Get parameters */

  /* Call function */
  ret = tmd_mult_ing_all_groups_close(
          unit
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_mult_ing_all_groups_close");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

#endif

#ifdef UI_MULTICAST_EGRESS
/********************************************************************
 *  Function handler: mult_eg_vlan_membership_group_range_set (section multicast_egress)
 */
int
  ui_tmd_api_multicast_egress_mult_eg_vlan_membership_group_range_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_multicast_egress");
  soc_sand_proc_name = "tmd_mult_eg_vlan_membership_group_range_set";

  unit = tmd_get_default_unit();
  TMD_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE_clear(&prm_info);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = tmd_mult_eg_vlan_membership_group_range_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_mult_eg_vlan_membership_group_range_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE_SET_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE_SET_INFO_MC_ID_HIGH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE_SET_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE_SET_INFO_MC_ID_HIGH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.mc_id_high = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE_SET_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE_SET_INFO_MC_ID_LOW_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE_SET_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE_SET_INFO_MC_ID_LOW_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.mc_id_low = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_mult_eg_vlan_membership_group_range_set(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_mult_eg_vlan_membership_group_range_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: mult_eg_vlan_membership_group_range_get (section multicast_egress)
 */
int
  ui_tmd_api_multicast_egress_mult_eg_vlan_membership_group_range_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_multicast_egress");
  soc_sand_proc_name = "tmd_mult_eg_vlan_membership_group_range_get";

  unit = tmd_get_default_unit();
  TMD_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE_clear(&prm_info);

  /* Get parameters */

  /* Call function */
  ret = tmd_mult_eg_vlan_membership_group_range_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_mult_eg_vlan_membership_group_range_get");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  TMD_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: mult_eg_group_update (section multicast_egress)
 */
int
  ui_tmd_api_multicast_egress_mult_eg_group_update(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_MULT_ID
    prm_multicast_id_ndx = 0;
  TMD_MULT_EG_ENTRY
    prm_mc_group;
  uint32
    prm_mc_group_size = 0;
  uint8
    prm_insufficient_memory;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_multicast_egress");
  soc_sand_proc_name = "tmd_mult_eg_group_update";

  unit = tmd_get_default_unit();
  TMD_MULT_EG_ENTRY_clear(&prm_mc_group);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_EG_GROUP_UPDATE_MULT_EG_GROUP_UPDATE_MULTICAST_ID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_EG_GROUP_UPDATE_MULT_EG_GROUP_UPDATE_MULTICAST_ID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_id_ndx = (TMD_MULT_ID)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter multicast_id_ndx after mult_eg_group_update***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_EG_GROUP_UPDATE_MULT_EG_GROUP_UPDATE_MC_GROUP_OUTLIF_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_EG_GROUP_UPDATE_MULT_EG_GROUP_UPDATE_MC_GROUP_OUTLIF_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mc_group.cud = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_EG_GROUP_UPDATE_MULT_EG_GROUP_UPDATE_MC_GROUP_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_EG_GROUP_UPDATE_MULT_EG_GROUP_UPDATE_MC_GROUP_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mc_group.port = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_EG_GROUP_UPDATE_MULT_EG_GROUP_UPDATE_MC_GROUP_SIZE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_EG_GROUP_UPDATE_MULT_EG_GROUP_UPDATE_MC_GROUP_SIZE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mc_group_size = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_mult_eg_group_update(
          unit,
          prm_multicast_id_ndx,
          &prm_mc_group,
          prm_mc_group_size,
          &prm_insufficient_memory
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_mult_eg_group_update");
    goto exit;
  }

  soc_sand_os_printf( "insufficient_memory: %u\n\r",prm_insufficient_memory);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: mult_eg_group_close (section multicast_egress)
 */
int
  ui_tmd_api_multicast_egress_mult_eg_group_close(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_MULT_ID
    prm_multicast_id_ndx = 0;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_multicast_egress");
  soc_sand_proc_name = "tmd_mult_eg_group_close";

  unit = tmd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_EG_GROUP_CLOSE_MULT_EG_GROUP_CLOSE_MULTICAST_ID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_EG_GROUP_CLOSE_MULT_EG_GROUP_CLOSE_MULTICAST_ID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_id_ndx = (TMD_MULT_ID)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter multicast_id_ndx after mult_eg_group_close***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_mult_eg_group_close(
          unit,
          prm_multicast_id_ndx
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_mult_eg_group_close");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: mult_eg_port_add (section multicast_egress)
 */
int
  ui_tmd_api_multicast_egress_mult_eg_port_add(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_MULT_ID
    prm_multicast_id_ndx = 0;
  TMD_MULT_EG_ENTRY
    prm_entry;
  uint8
    prm_insufficient_memory;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_multicast_egress");
  soc_sand_proc_name = "tmd_mult_eg_port_add";

  unit = tmd_get_default_unit();
  TMD_MULT_EG_ENTRY_clear(&prm_entry);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_EG_PORT_ADD_MULT_EG_PORT_ADD_MULTICAST_ID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_EG_PORT_ADD_MULT_EG_PORT_ADD_MULTICAST_ID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_id_ndx = (TMD_MULT_ID)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter multicast_id_ndx after mult_eg_port_add***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_EG_PORT_ADD_MULT_EG_PORT_ADD_ENTRY_OUTLIF_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_EG_PORT_ADD_MULT_EG_PORT_ADD_ENTRY_OUTLIF_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry.cud = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_EG_PORT_ADD_MULT_EG_PORT_ADD_ENTRY_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_EG_PORT_ADD_MULT_EG_PORT_ADD_ENTRY_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry.port = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_mult_eg_port_add(
          unit,
          prm_multicast_id_ndx,
          &prm_entry,
          &prm_insufficient_memory
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_mult_eg_port_add");
    goto exit;
  }

  soc_sand_os_printf( "insufficient_memory: %u\n\r",prm_insufficient_memory);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: mult_eg_port_remove (section multicast_egress)
 */
int
  ui_tmd_api_multicast_egress_mult_eg_port_remove(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_MULT_ID
    prm_multicast_id_ndx = 0;
  TMD_MULT_EG_ENTRY
    prm_entry;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_multicast_egress");
  soc_sand_proc_name = "tmd_mult_eg_port_remove";

  unit = tmd_get_default_unit();
  TMD_MULT_EG_ENTRY_clear(&prm_entry);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_EG_PORT_REMOVE_MULT_EG_PORT_REMOVE_MULTICAST_ID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_EG_PORT_REMOVE_MULT_EG_PORT_REMOVE_MULTICAST_ID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_id_ndx = (TMD_MULT_ID)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter multicast_id_ndx after mult_eg_port_remove***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_EG_PORT_REMOVE_MULT_EG_PORT_REMOVE_ENTRY_OUTLIF_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_EG_PORT_REMOVE_MULT_EG_PORT_REMOVE_ENTRY_OUTLIF_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry.cud = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_EG_PORT_REMOVE_MULT_EG_PORT_REMOVE_ENTRY_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_EG_PORT_REMOVE_MULT_EG_PORT_REMOVE_ENTRY_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry.port = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_mult_eg_port_remove(
          unit,
          prm_multicast_id_ndx,
          &prm_entry
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_mult_eg_port_remove");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: mult_eg_group_size_get (section multicast_egress)
 */
int
  ui_tmd_api_multicast_egress_mult_eg_group_size_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_MULT_ID
    prm_multicast_id_ndx = 0;
  uint32
    prm_mc_group_size;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_multicast_egress");
  soc_sand_proc_name = "tmd_mult_eg_group_size_get";

  unit = tmd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_EG_GROUP_SIZE_GET_MULT_EG_GROUP_SIZE_GET_MULTICAST_ID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_EG_GROUP_SIZE_GET_MULT_EG_GROUP_SIZE_GET_MULTICAST_ID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_id_ndx = (TMD_MULT_ID)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter multicast_id_ndx after mult_eg_group_size_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_mult_eg_group_size_get(
          unit,
          prm_multicast_id_ndx,
          &prm_mc_group_size
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_mult_eg_group_size_get");
    goto exit;
  }

  soc_sand_os_printf( "mc_group_size: %d\n\r",prm_mc_group_size);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: mult_eg_all_groups_close (section multicast_egress)
 */
int
  ui_tmd_api_multicast_egress_mult_eg_all_groups_close(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_multicast_egress");
  soc_sand_proc_name = "tmd_mult_eg_all_groups_close";

  unit = tmd_get_default_unit();

  /* Get parameters */

  /* Call function */
  ret = tmd_mult_eg_all_groups_close(
          unit
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_mult_eg_all_groups_close");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: mult_eg_vlan_membership_group_open (section multicast_egress)
 */
int
  ui_tmd_api_multicast_egress_mult_eg_vlan_membership_group_open(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_MULT_ID
    prm_multicast_id_ndx = 0;
  uint32
    prm_bitmap_index = 0xFFFFFFFF;
  TMD_MULT_EG_VLAN_MEMBERSHIP_GROUP
    prm_group;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_multicast_egress");
  soc_sand_proc_name = "tmd_mult_eg_vlan_membership_group_open";

  unit = tmd_get_default_unit();
  TMD_MULT_EG_VLAN_MEMBERSHIP_GROUP_clear(&prm_group);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_EG_VLAN_MEMBERSHIP_GROUP_OPEN_MULT_EG_VLAN_MEMBERSHIP_GROUP_OPEN_MULTICAST_ID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_EG_VLAN_MEMBERSHIP_GROUP_OPEN_MULT_EG_VLAN_MEMBERSHIP_GROUP_OPEN_MULTICAST_ID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_id_ndx = (TMD_MULT_ID)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter multicast_id_ndx after mult_eg_vlan_membership_group_open***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_EG_VLAN_MEMBERSHIP_GROUP_OPEN_MULT_EG_VLAN_MEMBERSHIP_GROUP_OPEN_GROUP_BITMAP_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_EG_VLAN_MEMBERSHIP_GROUP_OPEN_MULT_EG_VLAN_MEMBERSHIP_GROUP_OPEN_GROUP_BITMAP_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_bitmap_index = (uint32)param_val->value.ulong_value;
  }

  if(prm_bitmap_index != 0xFFFFFFFF)
  {

    if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_EG_VLAN_MEMBERSHIP_GROUP_OPEN_MULT_EG_VLAN_MEMBERSHIP_GROUP_OPEN_GROUP_BITMAP_ID,1))
    {
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_EG_VLAN_MEMBERSHIP_GROUP_OPEN_MULT_EG_VLAN_MEMBERSHIP_GROUP_OPEN_GROUP_BITMAP_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      prm_group.bitmap[ prm_bitmap_index] = (uint32)param_val->value.ulong_value;
    }

  }


  /* Call function */
  ret = tmd_mult_eg_vlan_membership_group_open(
          unit,
          prm_multicast_id_ndx,
          &prm_group
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_mult_eg_vlan_membership_group_open");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: mult_eg_vlan_membership_group_update (section multicast_egress)
 */
int
  ui_tmd_api_multicast_egress_mult_eg_vlan_membership_group_update(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_MULT_ID
    prm_multicast_id_ndx = 0;
  uint32
    prm_bitmap_index = 0xFFFFFFFF;
  TMD_MULT_EG_VLAN_MEMBERSHIP_GROUP
    prm_group;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_multicast_egress");
  soc_sand_proc_name = "tmd_mult_eg_vlan_membership_group_update";

  unit = tmd_get_default_unit();
  TMD_MULT_EG_VLAN_MEMBERSHIP_GROUP_clear(&prm_group);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_EG_VLAN_MEMBERSHIP_GROUP_UPDATE_MULT_EG_VLAN_MEMBERSHIP_GROUP_UPDATE_MULTICAST_ID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_EG_VLAN_MEMBERSHIP_GROUP_UPDATE_MULT_EG_VLAN_MEMBERSHIP_GROUP_UPDATE_MULTICAST_ID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_id_ndx = (TMD_MULT_ID)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter multicast_id_ndx after mult_eg_vlan_membership_group_update***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_EG_VLAN_MEMBERSHIP_GROUP_UPDATE_MULT_EG_VLAN_MEMBERSHIP_GROUP_UPDATE_GROUP_BITMAP_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_EG_VLAN_MEMBERSHIP_GROUP_UPDATE_MULT_EG_VLAN_MEMBERSHIP_GROUP_UPDATE_GROUP_BITMAP_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_bitmap_index = (uint32)param_val->value.ulong_value;
  }

  if(prm_bitmap_index != 0xFFFFFFFF)
  {

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_EG_VLAN_MEMBERSHIP_GROUP_UPDATE_MULT_EG_VLAN_MEMBERSHIP_GROUP_UPDATE_GROUP_BITMAP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_EG_VLAN_MEMBERSHIP_GROUP_UPDATE_MULT_EG_VLAN_MEMBERSHIP_GROUP_UPDATE_GROUP_BITMAP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_group.bitmap[ prm_bitmap_index] = (uint32)param_val->value.ulong_value;
  }

  }


  /* Call function */
  ret = tmd_mult_eg_vlan_membership_group_update(
          unit,
          prm_multicast_id_ndx,
          &prm_group
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_mult_eg_vlan_membership_group_update");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: mult_eg_vlan_membership_group_close (section multicast_egress)
 */
int
  ui_tmd_api_multicast_egress_mult_eg_vlan_membership_group_close(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_MULT_ID
    prm_multicast_id_ndx = 0;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_multicast_egress");
  soc_sand_proc_name = "tmd_mult_eg_vlan_membership_group_close";

  unit = tmd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_EG_VLAN_MEMBERSHIP_GROUP_CLOSE_MULT_EG_VLAN_MEMBERSHIP_GROUP_CLOSE_MULTICAST_ID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_EG_VLAN_MEMBERSHIP_GROUP_CLOSE_MULT_EG_VLAN_MEMBERSHIP_GROUP_CLOSE_MULTICAST_ID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_id_ndx = (TMD_MULT_ID)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter multicast_id_ndx after mult_eg_vlan_membership_group_close***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_mult_eg_vlan_membership_group_close(
          unit,
          prm_multicast_id_ndx
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_mult_eg_vlan_membership_group_close");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: mult_eg_vlan_membership_port_add (section multicast_egress)
 */
int
  ui_tmd_api_multicast_egress_mult_eg_vlan_membership_port_add(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_MULT_ID
    prm_multicast_id_ndx = 0;
  TMD_FAP_PORT_ID
    prm_port = 0;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_multicast_egress");
  soc_sand_proc_name = "tmd_mult_eg_vlan_membership_port_add";

  unit = tmd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_EG_VLAN_MEMBERSHIP_PORT_ADD_MULT_EG_VLAN_MEMBERSHIP_PORT_ADD_MULTICAST_ID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_EG_VLAN_MEMBERSHIP_PORT_ADD_MULT_EG_VLAN_MEMBERSHIP_PORT_ADD_MULTICAST_ID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_id_ndx = (TMD_MULT_ID)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter multicast_id_ndx after mult_eg_vlan_membership_port_add***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_EG_VLAN_MEMBERSHIP_PORT_ADD_MULT_EG_VLAN_MEMBERSHIP_PORT_ADD_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_EG_VLAN_MEMBERSHIP_PORT_ADD_MULT_EG_VLAN_MEMBERSHIP_PORT_ADD_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_mult_eg_vlan_membership_port_add(
          unit,
          prm_multicast_id_ndx,
          prm_port
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_mult_eg_vlan_membership_port_add");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: mult_eg_vlan_membership_port_remove (section multicast_egress)
 */
int
  ui_tmd_api_multicast_egress_mult_eg_vlan_membership_port_remove(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_MULT_ID
    prm_multicast_id_ndx = 0;
  TMD_FAP_PORT_ID
    prm_port = 0;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_multicast_egress");
  soc_sand_proc_name = "tmd_mult_eg_vlan_membership_port_remove";

  unit = tmd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_EG_VLAN_MEMBERSHIP_PORT_REMOVE_MULT_EG_VLAN_MEMBERSHIP_PORT_REMOVE_MULTICAST_ID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_EG_VLAN_MEMBERSHIP_PORT_REMOVE_MULT_EG_VLAN_MEMBERSHIP_PORT_REMOVE_MULTICAST_ID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_id_ndx = (TMD_MULT_ID)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter multicast_id_ndx after mult_eg_vlan_membership_port_remove***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_EG_VLAN_MEMBERSHIP_PORT_REMOVE_MULT_EG_VLAN_MEMBERSHIP_PORT_REMOVE_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_EG_VLAN_MEMBERSHIP_PORT_REMOVE_MULT_EG_VLAN_MEMBERSHIP_PORT_REMOVE_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_mult_eg_vlan_membership_port_remove(
          unit,
          prm_multicast_id_ndx,
          prm_port
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_mult_eg_vlan_membership_port_remove");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: mult_eg_vlan_membership_group_get (section multicast_egress)
 */
int
  ui_tmd_api_multicast_egress_mult_eg_vlan_membership_group_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_MULT_ID
    prm_multicast_id_ndx = 0;
  TMD_MULT_EG_VLAN_MEMBERSHIP_GROUP
    prm_group;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_multicast_egress");
  soc_sand_proc_name = "tmd_mult_eg_vlan_membership_group_get";

  unit = tmd_get_default_unit();
  TMD_MULT_EG_VLAN_MEMBERSHIP_GROUP_clear(&prm_group);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_EG_VLAN_MEMBERSHIP_GROUP_GET_MULT_EG_VLAN_MEMBERSHIP_GROUP_GET_MULTICAST_ID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_EG_VLAN_MEMBERSHIP_GROUP_GET_MULT_EG_VLAN_MEMBERSHIP_GROUP_GET_MULTICAST_ID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_id_ndx = (TMD_MULT_ID)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter multicast_id_ndx after mult_eg_vlan_membership_group_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_mult_eg_vlan_membership_group_get(
          unit,
          prm_multicast_id_ndx,
          &prm_group
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_mult_eg_vlan_membership_group_get");
    goto exit;
  }

  send_string_to_screen("--> group:", TRUE);
  TMD_MULT_EG_VLAN_MEMBERSHIP_GROUP_print(&prm_group);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: mult_eg_vlan_membership_all_groups_close (section multicast_egress)
 */
int
  ui_tmd_api_multicast_egress_mult_eg_vlan_membership_all_groups_close(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_multicast_egress");
  soc_sand_proc_name = "tmd_mult_eg_vlan_membership_all_groups_close";

  unit = tmd_get_default_unit();

  /* Get parameters */

  /* Call function */
  ret = tmd_mult_eg_vlan_membership_all_groups_close(
          unit
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_mult_eg_vlan_membership_all_groups_close");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

#endif

#ifdef UI_MULTICAST_FABRIC
/********************************************************************
 *  Function handler: mult_fabric_traffic_class_to_multicast_cls_map_set (section multicast_fabric)
 */
int
  ui_tmd_api_multicast_fabric_mult_fabric_traffic_class_to_multicast_cls_map_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_TR_CLS
    prm_tr_cls_ndx = 0;
  TMD_MULT_FABRIC_CLS
    prm_new_mult_cls;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_multicast_fabric");
  soc_sand_proc_name = "tmd_mult_fabric_traffic_class_to_multicast_cls_map_set";

  unit = tmd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_FABRIC_TRAFFIC_CLASS_TO_MULTICAST_CLS_MAP_SET_MULT_FABRIC_TRAFFIC_CLASS_TO_MULTICAST_CLS_MAP_SET_TR_CLS_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_FABRIC_TRAFFIC_CLASS_TO_MULTICAST_CLS_MAP_SET_MULT_FABRIC_TRAFFIC_CLASS_TO_MULTICAST_CLS_MAP_SET_TR_CLS_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tr_cls_ndx = (TMD_TR_CLS)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter tr_cls_ndx after mult_fabric_traffic_class_to_multicast_cls_map_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = tmd_mult_fabric_traffic_class_to_multicast_cls_map_get(
          unit,
          prm_tr_cls_ndx,
          &prm_new_mult_cls
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_mult_fabric_traffic_class_to_multicast_cls_map_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_FABRIC_TRAFFIC_CLASS_TO_MULTICAST_CLS_MAP_SET_MULT_FABRIC_TRAFFIC_CLASS_TO_MULTICAST_CLS_MAP_SET_NEW_MULT_CLS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_FABRIC_TRAFFIC_CLASS_TO_MULTICAST_CLS_MAP_SET_MULT_FABRIC_TRAFFIC_CLASS_TO_MULTICAST_CLS_MAP_SET_NEW_MULT_CLS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_new_mult_cls = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_mult_fabric_traffic_class_to_multicast_cls_map_set(
          unit,
          prm_tr_cls_ndx,
          prm_new_mult_cls
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_mult_fabric_traffic_class_to_multicast_cls_map_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: mult_fabric_traffic_class_to_multicast_cls_map_get (section multicast_fabric)
 */
int
  ui_tmd_api_multicast_fabric_mult_fabric_traffic_class_to_multicast_cls_map_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_TR_CLS
    prm_tr_cls_ndx = 0;
  TMD_MULT_FABRIC_CLS
    prm_new_mult_cls;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_multicast_fabric");
  soc_sand_proc_name = "tmd_mult_fabric_traffic_class_to_multicast_cls_map_get";

  unit = tmd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_FABRIC_TRAFFIC_CLASS_TO_MULTICAST_CLS_MAP_GET_MULT_FABRIC_TRAFFIC_CLASS_TO_MULTICAST_CLS_MAP_GET_TR_CLS_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_FABRIC_TRAFFIC_CLASS_TO_MULTICAST_CLS_MAP_GET_MULT_FABRIC_TRAFFIC_CLASS_TO_MULTICAST_CLS_MAP_GET_TR_CLS_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tr_cls_ndx = (TMD_TR_CLS)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter tr_cls_ndx after mult_fabric_traffic_class_to_multicast_cls_map_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_mult_fabric_traffic_class_to_multicast_cls_map_get(
          unit,
          prm_tr_cls_ndx,
          &prm_new_mult_cls
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_mult_fabric_traffic_class_to_multicast_cls_map_get");
    goto exit;
  }

  soc_sand_os_printf( "new_mult_cls: %d\n\r",prm_new_mult_cls);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: mult_fabric_base_queue_set (section multicast_fabric)
 */
int
  ui_tmd_api_multicast_fabric_mult_fabric_base_queue_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_queue_id = 0;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_multicast_fabric");
  soc_sand_proc_name = "tmd_mult_fabric_base_queue_set";

  unit = tmd_get_default_unit();

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = tmd_mult_fabric_base_queue_get(
          unit,
          &prm_queue_id
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_mult_fabric_base_queue_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_FABRIC_BASE_QUEUE_SET_MULT_FABRIC_BASE_QUEUE_SET_QUEUE_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_FABRIC_BASE_QUEUE_SET_MULT_FABRIC_BASE_QUEUE_SET_QUEUE_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_queue_id = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_mult_fabric_base_queue_set(
          unit,
          prm_queue_id
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_mult_fabric_base_queue_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: mult_fabric_base_queue_get (section multicast_fabric)
 */
int
  ui_tmd_api_multicast_fabric_mult_fabric_base_queue_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_queue_id = 0;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_multicast_fabric");
  soc_sand_proc_name = "tmd_mult_fabric_base_queue_get";

  unit = tmd_get_default_unit();

  /* Get parameters */

  /* Call function */
  ret = tmd_mult_fabric_base_queue_get(
          unit,
          &prm_queue_id
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_mult_fabric_base_queue_get");
    goto exit;
  }

  soc_sand_os_printf( "queue_id: %lu\n\r",prm_queue_id);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: mult_fabric_credit_source_set (section multicast_fabric)
 */
int
  ui_tmd_api_multicast_fabric_mult_fabric_credit_source_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_be_sch_port_index = 0xFFFFFFFF;
  TMD_MULT_FABRIC_INFO
    prm_info;
  TMD_MULT_FABRIC_INFO
    prm_exact_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_multicast_fabric");
  soc_sand_proc_name = "tmd_mult_fabric_credit_source_set";

  unit = tmd_get_default_unit();
  TMD_MULT_FABRIC_INFO_clear(&prm_info);
  TMD_MULT_FABRIC_INFO_clear(&prm_exact_info);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = tmd_mult_fabric_credit_source_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_mult_fabric_credit_source_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_FABRIC_CREDIT_SOURCE_SET_MULT_FABRIC_CREDIT_SOURCE_SET_INFO_CREDITS_VIA_SCH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_FABRIC_CREDIT_SOURCE_SET_MULT_FABRIC_CREDIT_SOURCE_SET_INFO_CREDITS_VIA_SCH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.credits_via_sch = (uint8)param_val->value.ulong_value;
    prm_info.guaranteed.gr_sch_port.multicast_class_valid =
    prm_info.best_effort.be_sch_port[0].be_sch_port.multicast_class_valid =
    prm_info.best_effort.be_sch_port[1].be_sch_port.multicast_class_valid =
    prm_info.best_effort.be_sch_port[2].be_sch_port.multicast_class_valid =
    (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_FABRIC_CREDIT_SOURCE_SET_MULT_FABRIC_CREDIT_SOURCE_SET_INFO_GUARANTEED_GUARANTEED_MULTICAST_CLASS_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_FABRIC_CREDIT_SOURCE_SET_MULT_FABRIC_CREDIT_SOURCE_SET_INFO_GUARANTEED_GUARANTEED_MULTICAST_CLASS_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.guaranteed.gr_sch_port.multicast_class_valid = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_FABRIC_CREDIT_SOURCE_SET_MULT_FABRIC_CREDIT_SOURCE_SET_INFO_MAX_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_FABRIC_CREDIT_SOURCE_SET_MULT_FABRIC_CREDIT_SOURCE_SET_INFO_MAX_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.max_rate = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_FABRIC_CREDIT_SOURCE_SET_MULT_FABRIC_CREDIT_SOURCE_SET_INFO_BEST_EFFORT_BEST_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_FABRIC_CREDIT_SOURCE_SET_MULT_FABRIC_CREDIT_SOURCE_SET_INFO_BEST_EFFORT_BEST_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_be_sch_port_index = (uint32)param_val->value.ulong_value;
  }

  if(prm_be_sch_port_index != 0xFFFFFFFF)
  {
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_FABRIC_CREDIT_SOURCE_SET_MULT_FABRIC_CREDIT_SOURCE_SET_INFO_BEST_EFFORT_BEST_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_FABRIC_CREDIT_SOURCE_SET_MULT_FABRIC_CREDIT_SOURCE_SET_INFO_BEST_EFFORT_BEST_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.best_effort.be_sch_port[ prm_be_sch_port_index].weight = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_FABRIC_CREDIT_SOURCE_SET_MULT_FABRIC_CREDIT_SOURCE_SET_INFO_BEST_EFFORT_BEST_BEST_MULTICAST_CLASS_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_FABRIC_CREDIT_SOURCE_SET_MULT_FABRIC_CREDIT_SOURCE_SET_INFO_BEST_EFFORT_BEST_BEST_MULTICAST_CLASS_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.best_effort.be_sch_port[ prm_be_sch_port_index].be_sch_port.multicast_class_valid = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_FABRIC_CREDIT_SOURCE_SET_MULT_FABRIC_CREDIT_SOURCE_SET_INFO_BEST_EFFORT_BEST_BEST_MCAST_CLASS_PORT_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_FABRIC_CREDIT_SOURCE_SET_MULT_FABRIC_CREDIT_SOURCE_SET_INFO_BEST_EFFORT_BEST_BEST_MCAST_CLASS_PORT_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.best_effort.be_sch_port[ prm_be_sch_port_index].be_sch_port.mcast_class_port_id = (uint32)param_val->value.ulong_value;
  }

  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_FABRIC_CREDIT_SOURCE_SET_MULT_FABRIC_CREDIT_SOURCE_SET_INFO_BEST_EFFORT_WFQ_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_FABRIC_CREDIT_SOURCE_SET_MULT_FABRIC_CREDIT_SOURCE_SET_INFO_BEST_EFFORT_WFQ_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.best_effort.wfq_enable = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_FABRIC_CREDIT_SOURCE_SET_MULT_FABRIC_CREDIT_SOURCE_SET_INFO_BEST_EFFORT_BE_SHAPER_MAX_BURST_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_FABRIC_CREDIT_SOURCE_SET_MULT_FABRIC_CREDIT_SOURCE_SET_INFO_BEST_EFFORT_BE_SHAPER_MAX_BURST_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.best_effort.be_shaper.max_burst = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_FABRIC_CREDIT_SOURCE_SET_MULT_FABRIC_CREDIT_SOURCE_SET_INFO_BEST_EFFORT_BE_SHAPER_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_FABRIC_CREDIT_SOURCE_SET_MULT_FABRIC_CREDIT_SOURCE_SET_INFO_BEST_EFFORT_BE_SHAPER_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.best_effort.be_shaper.rate = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_FABRIC_CREDIT_SOURCE_SET_MULT_FABRIC_CREDIT_SOURCE_SET_INFO_GUARANTEED_GUARANTEED_MCAST_CLASS_PORT_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_FABRIC_CREDIT_SOURCE_SET_MULT_FABRIC_CREDIT_SOURCE_SET_INFO_GUARANTEED_GUARANTEED_MCAST_CLASS_PORT_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.guaranteed.gr_sch_port.mcast_class_port_id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_FABRIC_CREDIT_SOURCE_SET_MULT_FABRIC_CREDIT_SOURCE_SET_INFO_GUARANTEED_GR_SHAPER_MAX_BURST_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_FABRIC_CREDIT_SOURCE_SET_MULT_FABRIC_CREDIT_SOURCE_SET_INFO_GUARANTEED_GR_SHAPER_MAX_BURST_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.guaranteed.gr_shaper.max_burst = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_FABRIC_CREDIT_SOURCE_SET_MULT_FABRIC_CREDIT_SOURCE_SET_INFO_GUARANTEED_GR_SHAPER_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_FABRIC_CREDIT_SOURCE_SET_MULT_FABRIC_CREDIT_SOURCE_SET_INFO_GUARANTEED_GR_SHAPER_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.guaranteed.gr_shaper.rate = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_mult_fabric_credit_source_set(
          unit,
          &prm_info,
          &prm_exact_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_mult_fabric_credit_source_set");
    goto exit;
  }

  send_string_to_screen("--> exact_info:", TRUE);
  TMD_MULT_FABRIC_INFO_print(&prm_exact_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: mult_fabric_credit_source_get (section multicast_fabric)
 */
int
  ui_tmd_api_multicast_fabric_mult_fabric_credit_source_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_MULT_FABRIC_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_multicast_fabric");
  soc_sand_proc_name = "tmd_mult_fabric_credit_source_get";

  unit = tmd_get_default_unit();
  TMD_MULT_FABRIC_INFO_clear(&prm_info);

  /* Get parameters */

  /* Call function */
  ret = tmd_mult_fabric_credit_source_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_mult_fabric_credit_source_get");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  TMD_MULT_FABRIC_INFO_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: mult_fabric_enhanced_set (section multicast_fabric)
 */
int
  ui_tmd_api_multicast_fabric_mult_fabric_enhanced_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_U32_RANGE
    prm_queue_range;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_multicast_fabric");
  soc_sand_proc_name = "tmd_mult_fabric_enhanced_set";

  unit = tmd_get_default_unit();

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = tmd_mult_fabric_enhanced_get(
          unit,
          &prm_queue_range
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** tmd_mult_fabric_enhanced_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "tmd_mult_fabric_enhanced_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_FABRIC_ENHANCED_SET_MULT_FABRIC_ENHANCED_SET_QUEUE_RANGE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_FABRIC_ENHANCED_SET_MULT_FABRIC_ENHANCED_SET_QUEUE_RANGE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_queue_range.start = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_FABRIC_ENHANCED_SET_MULT_FABRIC_ENHANCED_SET_QUEUE_RANGE_MAX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_FABRIC_ENHANCED_SET_MULT_FABRIC_ENHANCED_SET_QUEUE_RANGE_MAX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_queue_range.end = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  ret = tmd_mult_fabric_enhanced_set(
          unit,
          &prm_queue_range
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** tmd_mult_fabric_enhanced_set - FAIL", TRUE);
    soc_petra_disp_result(ret, "tmd_mult_fabric_enhanced_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: mult_fabric_enhanced_get (section multicast_fabric)
 */
int
  ui_tmd_api_multicast_fabric_mult_fabric_enhanced_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_U32_RANGE
    prm_queue_range;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_multicast_fabric");
  soc_sand_proc_name = "tmd_mult_fabric_enhanced_get";

  unit = tmd_get_default_unit();

  /* Get parameters */

  /* Call function */
  ret = tmd_mult_fabric_enhanced_get(
          unit,
          &prm_queue_range
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** tmd_mult_fabric_enhanced_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "tmd_mult_fabric_enhanced_get");
    goto exit;
  }

  send_string_to_screen("--> queue_range:", TRUE);
  soc_sand_os_printf( "queue_range_min: %lu\n\r",prm_queue_range.start);
  soc_sand_os_printf( "queue_range_max: %lu\n\r",prm_queue_range.end);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: mult_fabric_active_links_set (section multicast_fabric)
 */
int
  ui_tmd_api_multicast_fabric_mult_fabric_active_links_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_bitmap_index = 0xFFFFFFFF;
  TMD_MULT_FABRIC_ACTIVE_LINKS
    prm_links;
  uint8
    prm_tbl_refresh_enable;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_multicast_fabric");
  soc_sand_proc_name = "tmd_mult_fabric_active_links_set";

  unit = tmd_get_default_unit();
  TMD_MULT_FABRIC_ACTIVE_LINKS_clear(&prm_links);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = tmd_mult_fabric_active_links_get(
          unit,
          &prm_links,
          &prm_tbl_refresh_enable
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_mult_fabric_active_links_get");
    goto exit;
  }

  prm_links.bitmap[1] &= SOC_SAND_BITS_MASK((TMD_FBR_NOF_LINKS - 32) - 1, 0);

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_FABRIC_ACTIVE_LINKS_SET_MULT_FABRIC_ACTIVE_LINKS_SET_LINKS_BITMAP_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_FABRIC_ACTIVE_LINKS_SET_MULT_FABRIC_ACTIVE_LINKS_SET_LINKS_BITMAP_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_bitmap_index = (uint32)param_val->value.ulong_value;
  }

  if(prm_bitmap_index != 0xFFFFFFFF)
  {
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_FABRIC_ACTIVE_LINKS_SET_MULT_FABRIC_ACTIVE_LINKS_SET_LINKS_BITMAP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_FABRIC_ACTIVE_LINKS_SET_MULT_FABRIC_ACTIVE_LINKS_SET_LINKS_BITMAP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_links.bitmap[ prm_bitmap_index] = (uint32)param_val->value.ulong_value;
  }

  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_FABRIC_ACTIVE_LINKS_SET_MULT_FABRIC_ACTIVE_LINKS_SET_TBL_REFRESH_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_MULT_FABRIC_ACTIVE_LINKS_SET_MULT_FABRIC_ACTIVE_LINKS_SET_TBL_REFRESH_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_refresh_enable = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_mult_fabric_active_links_set(
          unit,
          &prm_links,
          prm_tbl_refresh_enable
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_mult_fabric_active_links_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: mult_fabric_active_links_get (section multicast_fabric)
 */
int
  ui_tmd_api_multicast_fabric_mult_fabric_active_links_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_MULT_FABRIC_ACTIVE_LINKS
    prm_links;
  uint8
    prm_tbl_refresh_enable;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_multicast_fabric");
  soc_sand_proc_name = "tmd_mult_fabric_active_links_get";

  unit = tmd_get_default_unit();
  TMD_MULT_FABRIC_ACTIVE_LINKS_clear(&prm_links);

  /* Get parameters */

  /* Call function */
  ret = tmd_mult_fabric_active_links_get(
          unit,
          &prm_links,
          &prm_tbl_refresh_enable
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_mult_fabric_active_links_get");
    goto exit;
  }

  send_string_to_screen("--> links:", TRUE);
  TMD_MULT_FABRIC_ACTIVE_LINKS_print(&prm_links);

  soc_sand_os_printf( "tbl_refresh_enable: %u\n\r",prm_tbl_refresh_enable);


  goto exit;
exit:
  return ui_ret;
}

#endif

#ifdef UI_FABRIC
/********************************************************************
 *  Function handler: srd_qrtt_reset (section fabric)
 */
int
  ui_tmd_api_fabric_srd_qrtt_reset(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_srd_qrtt_ndx;
  uint8
    prm_is_oor=FALSE;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_fabric");
  soc_sand_proc_name = "tmd_fabric_srd_qrtt_reset";

  unit = tmd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_FABRIC_SRD_QRTT_RESET_SRD_QRTT_RESET_SRD_QRTT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_FABRIC_SRD_QRTT_RESET_SRD_QRTT_RESET_SRD_QRTT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_srd_qrtt_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter srd_qrtt_ndx after srd_qrtt_reset***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_FABRIC_SRD_QRTT_RESET_SRD_QRTT_RESET_IS_OOR_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_FABRIC_SRD_QRTT_RESET_SRD_QRTT_RESET_IS_OOR_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_is_oor = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_fabric_srd_qrtt_reset(
          unit,
          prm_srd_qrtt_ndx,
          prm_is_oor
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** tmd_fabric_srd_qrtt_reset - FAIL", TRUE);
    soc_petra_disp_result(ret, "tmd_fabric_srd_qrtt_reset");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: fc_enable_set (section fabric)
 */
int
  ui_tmd_api_fabric_fc_enable_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_CONNECTION_DIRECTION
    prm_direction_ndx = TMD_CONNECTION_DIRECTION_BOTH;
  uint32
    prm_enable_index = 0xFFFFFFFF;
  TMD_FABRIC_FC
    prm_rx_info,
    prm_tx_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_fabric");
  soc_sand_proc_name = "tmd_fabric_fc_enable_set";

  unit = tmd_get_default_unit();
  TMD_FABRIC_FC_clear(&prm_rx_info);
  TMD_FABRIC_FC_clear(&prm_tx_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_FABRIC_FC_ENABLE_SET_FC_ENABLE_SET_DIRECTION_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_FABRIC_FC_ENABLE_SET_FC_ENABLE_SET_DIRECTION_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_direction_ndx = param_val->numeric_equivalent;
  }


  /* This is a set function, so call GET function first */
  ret = tmd_fabric_fc_enable_get(
          unit,
          &prm_rx_info,
          &prm_tx_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_fabric_fc_enable_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_FABRIC_FC_ENABLE_SET_FC_ENABLE_SET_INFO_ENABLE_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_FABRIC_FC_ENABLE_SET_FC_ENABLE_SET_INFO_ENABLE_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_enable_index = (uint32)param_val->value.ulong_value;
  }

  if(prm_enable_index != 0xFFFFFFFF)
  {

    if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_FABRIC_FC_ENABLE_SET_FC_ENABLE_SET_INFO_ENABLE_ID,1))
    {
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_FABRIC_FC_ENABLE_SET_FC_ENABLE_SET_INFO_ENABLE_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;

      prm_rx_info.enable[ prm_enable_index] = (uint8)param_val->value.ulong_value;
      prm_tx_info.enable[ prm_enable_index] = (uint8)param_val->value.ulong_value;
    }

  }


  /* Call function */
  switch(prm_direction_ndx)
  {
  case TMD_CONNECTION_DIRECTION_RX:
    ret = tmd_fabric_fc_enable_set(
            unit,
            prm_direction_ndx,
            &prm_rx_info
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      soc_petra_disp_result(ret, "tmd_fabric_fc_enable_set");
      goto exit;
    }

    break;

  case TMD_CONNECTION_DIRECTION_TX:
    ret = tmd_fabric_fc_enable_set(
            unit,
            prm_direction_ndx,
            &prm_tx_info
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      soc_petra_disp_result(ret, "tmd_fabric_fc_enable_set");
      goto exit;
    }

    break;

  case TMD_CONNECTION_DIRECTION_BOTH:
    ret = tmd_fabric_fc_enable_set(
            unit,
            prm_direction_ndx,
            &prm_rx_info
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      soc_petra_disp_result(ret, "tmd_fabric_fc_enable_set");
      goto exit;
    }

    ret = tmd_fabric_fc_enable_set(
            unit,
            prm_direction_ndx,
            &prm_tx_info
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      soc_petra_disp_result(ret, "tmd_fabric_fc_enable_set");
      goto exit;
    }

    break;

  default:
      goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: fc_enable_get (section fabric)
 */
int
  ui_tmd_api_fabric_fc_enable_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_FABRIC_FC
    prm_rx_info,
    prm_tx_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_fabric");
  soc_sand_proc_name = "tmd_fabric_fc_enable_get";

  unit = tmd_get_default_unit();
  TMD_FABRIC_FC_clear(&prm_rx_info);
  TMD_FABRIC_FC_clear(&prm_tx_info);


  /* Call function */
  ret = tmd_fabric_fc_enable_get(
          unit,
          &prm_rx_info,
          &prm_tx_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_fabric_fc_enable_get");
    goto exit;
  }

  send_string_to_screen("--> RX: \n\r", TRUE);
  send_string_to_screen("info:", TRUE);
  TMD_FABRIC_FC_print(&prm_rx_info);
  send_string_to_screen("\n\r", TRUE);
  send_string_to_screen("--> TX: \n\r", TRUE);
  send_string_to_screen("info:", TRUE);
  TMD_FABRIC_FC_print(&prm_tx_info);
  send_string_to_screen("\n\r", TRUE);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: cell_format_get (section fabric)
 */
int
  ui_tmd_api_fabric_cell_format_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_FABRIC_CELL_FORMAT
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_fabric");
  soc_sand_proc_name = "tmd_fabric_cell_format_get";

  unit = tmd_get_default_unit();
  TMD_FABRIC_CELL_FORMAT_clear(&prm_info);

  /* Get parameters */

  /* Call function */
  ret = tmd_fabric_cell_format_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_fabric_cell_format_get");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  TMD_FABRIC_CELL_FORMAT_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: coexist_set (section fabric)
 */
int
  ui_tmd_api_fabric_coexist_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_fap_id_odd_link_map_index = 0xFFFFFFFF;
  uint32
    prm_coexist_dest_map_index = 0xFFFFFFFF;
  TMD_FABRIC_COEXIST_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_fabric");
  soc_sand_proc_name = "tmd_fabric_coexist_set";

  unit = tmd_get_default_unit();
  TMD_FABRIC_COEXIST_INFO_clear(&prm_info);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = tmd_fabric_coexist_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_fabric_coexist_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_FABRIC_COEXIST_SET_COEXIST_SET_INFO_FAP_ID_ODD_LINK_MAP_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_FABRIC_COEXIST_SET_COEXIST_SET_INFO_FAP_ID_ODD_LINK_MAP_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_fap_id_odd_link_map_index = (uint32)param_val->value.ulong_value;
  }

  if(prm_fap_id_odd_link_map_index != 0xFFFFFFFF)
  {

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_FABRIC_COEXIST_SET_COEXIST_SET_INFO_FAP_ID_ODD_LINK_MAP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_FABRIC_COEXIST_SET_COEXIST_SET_INFO_FAP_ID_ODD_LINK_MAP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.fap_id_odd_link_map[ prm_fap_id_odd_link_map_index] = (uint8)param_val->value.ulong_value;
  }

  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_FABRIC_COEXIST_SET_COEXIST_SET_INFO_COEXIST_DEST_MAP_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_FABRIC_COEXIST_SET_COEXIST_SET_INFO_COEXIST_DEST_MAP_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_coexist_dest_map_index = (uint32)param_val->value.ulong_value;
  }

  if(prm_coexist_dest_map_index != 0xFFFFFFFF)
  {

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_FABRIC_COEXIST_SET_COEXIST_SET_INFO_COEXIST_DEST_MAP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_FABRIC_COEXIST_SET_COEXIST_SET_INFO_COEXIST_DEST_MAP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.coexist_dest_map[ prm_coexist_dest_map_index] = (uint8)param_val->value.ulong_value;
  }

  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_FABRIC_COEXIST_SET_COEXIST_SET_INFO_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_FABRIC_COEXIST_SET_COEXIST_SET_INFO_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.enable = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_fabric_coexist_set(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_fabric_coexist_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: coexist_get (section fabric)
 */
int
  ui_tmd_api_fabric_coexist_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_FABRIC_COEXIST_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_fabric");
  soc_sand_proc_name = "tmd_fabric_coexist_get";

  unit = tmd_get_default_unit();
  TMD_FABRIC_COEXIST_INFO_clear(&prm_info);

  /* Get parameters */

  /* Call function */
  ret = tmd_fabric_coexist_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_fabric_coexist_get");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  TMD_FABRIC_COEXIST_INFO_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: stand_alone_fap_mode_get (section fabric)
 */
int
  ui_tmd_api_fabric_stand_alone_fap_mode_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint8
    prm_is_single_fap_mode;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_fabric");
  soc_sand_proc_name = "tmd_fabric_stand_alone_fap_mode_get";

  unit = tmd_get_default_unit();

  /* Get parameters */

  /* Call function */
  ret = tmd_fabric_stand_alone_fap_mode_get(
          unit,
          &prm_is_single_fap_mode
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_fabric_stand_alone_fap_mode_get");
    goto exit;
  }

  soc_sand_os_printf( "is_single_fap_mode: %u\n\r",prm_is_single_fap_mode);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: connect_mode_set (section fabric)
 */
int
  ui_tmd_api_fabric_connect_mode_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_FABRIC_CONNECT_MODE
    prm_fabric_mode;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_fabric");
  soc_sand_proc_name = "tmd_fabric_connect_mode_set";

  unit = tmd_get_default_unit();

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = tmd_fabric_connect_mode_get(
          unit,
          &prm_fabric_mode
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_fabric_connect_mode_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_FABRIC_CONNECT_MODE_SET_CONNECT_MODE_SET_FABRIC_MODE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_FABRIC_CONNECT_MODE_SET_CONNECT_MODE_SET_FABRIC_MODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_fabric_mode = param_val->numeric_equivalent;
  }


  /* Call function */
  ret = tmd_fabric_connect_mode_set(
          unit,
          prm_fabric_mode
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_fabric_connect_mode_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: connect_mode_get (section fabric)
 */
int
  ui_tmd_api_fabric_connect_mode_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_FABRIC_CONNECT_MODE
    prm_fabric_mode;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_fabric");
  soc_sand_proc_name = "tmd_fabric_connect_mode_get";

  unit = tmd_get_default_unit();

  /* Get parameters */

  /* Call function */
  ret = tmd_fabric_connect_mode_get(
          unit,
          &prm_fabric_mode
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_fabric_connect_mode_get");
    goto exit;
  }

  soc_sand_os_printf( "fabric_mode: %s\n\r",TMD_FABRIC_CONNECT_MODE_to_string(prm_fabric_mode));


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: fap20_map_set (section fabric)
 */
int
  ui_tmd_api_fabric_fap20_map_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_other_device_ndx = 0;
  uint8
    prm_is_fap20_device;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_fabric");
  soc_sand_proc_name = "tmd_fabric_fap20_map_set";

  unit = tmd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_FABRIC_FAP20_MAP_SET_FAP20_MAP_SET_OTHER_DEVICE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_FABRIC_FAP20_MAP_SET_FAP20_MAP_SET_OTHER_DEVICE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_other_device_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter other_device_ndx after fap20_map_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = tmd_fabric_fap20_map_get(
          unit,
          prm_other_device_ndx,
          &prm_is_fap20_device
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_fabric_fap20_map_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_FABRIC_FAP20_MAP_SET_FAP20_MAP_SET_IS_FAP20_DEVICE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_FABRIC_FAP20_MAP_SET_FAP20_MAP_SET_IS_FAP20_DEVICE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_is_fap20_device = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_fabric_fap20_map_set(
          unit,
          prm_other_device_ndx,
          prm_is_fap20_device
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_fabric_fap20_map_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: fap20_map_get (section fabric)
 */
int
  ui_tmd_api_fabric_fap20_map_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_other_device_ndx = 0;
  uint8
    prm_is_fap20_device;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_fabric");
  soc_sand_proc_name = "tmd_fabric_fap20_map_get";

  unit = tmd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_FABRIC_FAP20_MAP_GET_FAP20_MAP_GET_OTHER_DEVICE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_FABRIC_FAP20_MAP_GET_FAP20_MAP_GET_OTHER_DEVICE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_other_device_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter other_device_ndx after fap20_map_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_fabric_fap20_map_get(
          unit,
          prm_other_device_ndx,
          &prm_is_fap20_device
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_fabric_fap20_map_get");
    goto exit;
  }

  soc_sand_os_printf( "is_fap20_device: %u\n\r",prm_is_fap20_device);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: topology_status_connectivity_get (section fabric)
 */
int
  ui_tmd_api_fabric_topology_status_connectivity_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_FABRIC_LINKS_CON_STAT_INFO_ARR
    prm_connectivity_map;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_fabric");
  soc_sand_proc_name = "tmd_fabric_topology_status_connectivity_get";

  unit = tmd_get_default_unit();
  TMD_FABRIC_LINKS_CON_STAT_INFO_ARR_clear(&prm_connectivity_map);

  /* Get parameters */

  /* Call function */
  ret = tmd_fabric_topology_status_connectivity_get(
          unit,
          &prm_connectivity_map
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_fabric_topology_status_connectivity_get");
    goto exit;
  }

  send_string_to_screen("--> connectivity_map:", TRUE);
  tmd_fabric_topology_status_connectivity_print(&prm_connectivity_map);


  goto exit;
exit:
  return ui_ret;
}


/********************************************************************
 *  Function handler: links_status_get (section fabric)
 */
int
  ui_tmd_api_fabric_links_status_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_FABRIC_LINKS_STATUS_ALL
    prm_links_status;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_fabric");
  soc_sand_proc_name = "tmd_fabric_links_status_get";

  unit = tmd_get_default_unit();
  TMD_FABRIC_LINKS_STATUS_ALL_clear(&prm_links_status);

  /* Get parameters */

  /* Call function */
  ret = tmd_fabric_links_status_get(
          unit,
          TRUE,
          &prm_links_status
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_fabric_links_status_get");
    goto exit;
  }

  send_string_to_screen("--> links_status:", TRUE);
  TMD_FABRIC_LINKS_STATUS_ALL_print(&prm_links_status, SOC_SAND_PRINT_FLAVORS_SHORT);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: link_on_off_set (section fabric)
 */
int
  ui_tmd_api_fabric_link_on_off_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_link_ndx = 0;
  TMD_LINK_STATE_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_fabric");
  soc_sand_proc_name = "tmd_link_on_off_set";

  unit = tmd_get_default_unit();
  TMD_LINK_STATE_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_LINK_ON_OFF_SET_LINK_ON_OFF_SET_LINK_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_LINK_ON_OFF_SET_LINK_ON_OFF_SET_LINK_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_link_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter link_ndx after link_on_off_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = tmd_link_on_off_get(
          unit,
          prm_link_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_link_on_off_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_LINK_ON_OFF_SET_LINK_ON_OFF_SET_INFO_SERDES_ALSO_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_LINK_ON_OFF_SET_LINK_ON_OFF_SET_INFO_SERDES_ALSO_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.serdes_also = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_LINK_ON_OFF_SET_LINK_ON_OFF_SET_INFO_ON_OFF_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_LINK_ON_OFF_SET_LINK_ON_OFF_SET_INFO_ON_OFF_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.on_off = param_val->numeric_equivalent;
  }


  /* Call function */
  ret = tmd_link_on_off_set(
          unit,
          prm_link_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_link_on_off_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: link_on_off_get (section fabric)
 */
int
  ui_tmd_api_fabric_link_on_off_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_link_ndx = 0;
  TMD_LINK_STATE_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_fabric");
  soc_sand_proc_name = "tmd_link_on_off_get";

  unit = tmd_get_default_unit();
  TMD_LINK_STATE_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_LINK_ON_OFF_GET_LINK_ON_OFF_GET_LINK_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_LINK_ON_OFF_GET_LINK_ON_OFF_GET_LINK_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_link_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter link_ndx after link_on_off_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = tmd_link_on_off_get(
          unit,
          prm_link_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_link_on_off_get");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  TMD_LINK_STATE_INFO_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

#endif

#ifdef UI_CELL
/********************************************************************
*  Function handler: tmd_read_from_fe600 (section cell)
 */
int
ui_tmd_api_cell_tmd_read_from_fe600(
  CURRENT_LINE *current_line
  )
{
  uint32
    indx = 0,
    param_i = 0,
    offset = 0,
    data_out = 0 ,
    ret = 0;
  TMD_SR_CELL_LINK_LIST
    sr_link_list ;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_cell");
  soc_sand_proc_name = "tmd_read_from_fe600";

  unit = tmd_get_default_unit();

  for (indx = 0; indx < TMD_CELL_NOF_LINKS_IN_PATH_LINKS; ++indx)
  {
    sr_link_list.path_links[indx] = 0;
  }

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_CELL_READ_DIRECT_DEST_ENTITY_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_CELL_READ_DIRECT_DEST_ENTITY_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    sr_link_list.dest_entity_type = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter dest_entity_type after tmd_read_from_fe600*** ", TRUE);
    goto exit;
  }

  for (param_i=1; param_i<=TMD_CELL_NOF_LINKS_IN_PATH_LINKS; ++param_i)
  {
    ret = search_param_val_pairs(current_line,&match_index,PARAM_TMD_CELL_READ_DIRECT_LINK_FIP_TO_FE_ID,param_i);
    if (0 != ret)
    {
      ret=0;
      break;
    }
    UI_MACROS_GET_NUMMERIC_VAL_WITH_INDEX(PARAM_TMD_CELL_READ_DIRECT_LINK_FIP_TO_FE_ID,param_i);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    sr_link_list.path_links[param_i - 1] = (uint8) param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_CELL_READ_DIRECT_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_CELL_READ_DIRECT_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_read_from_fe600(
          unit,
          &sr_link_list,
          sizeof(uint32),
          offset,
          &data_out
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** tmd_read_from_fe600 - FAIL", TRUE);
    soc_petra_disp_result(ret, "tmd_read_from_fe600");
    goto exit;
  }

  soc_sand_os_printf(  "data_out: %lx\n\r" , data_out ) ;


exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: tmd_write_to_fe600 (section cell)
 */
int
ui_tmd_api_cell_tmd_write_to_fe600(
  CURRENT_LINE *current_line
  )
{
  uint32
    indx = 0,
    param_i = 0,
    offset = 0,
    data_in = 0,
    ret = 0;
  TMD_SR_CELL_LINK_LIST
    sr_link_list ;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_cell");
  soc_sand_proc_name = "tmd_write_to_fe600";

  unit = tmd_get_default_unit();

  for (indx = 0; indx < TMD_CELL_NOF_LINKS_IN_PATH_LINKS; ++indx)
  {
    sr_link_list.path_links[indx] = 0;
  }

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_CELL_WRITE_DIRECT_DEST_ENTITY_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_CELL_WRITE_DIRECT_DEST_ENTITY_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    sr_link_list.dest_entity_type = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter dest_entity_type after tmd_write_to_fe600*** ", TRUE);
    goto exit;
  }

  for (param_i=1; param_i<=TMD_CELL_NOF_LINKS_IN_PATH_LINKS; ++param_i)
  {
    ret = search_param_val_pairs(current_line,&match_index,PARAM_TMD_CELL_WRITE_DIRECT_LINK_FIP_TO_FE_ID,param_i);
    if (0 != ret)
    {
      ret=0;
      break;
    }
    UI_MACROS_GET_NUMMERIC_VAL_WITH_INDEX(PARAM_TMD_CELL_WRITE_DIRECT_LINK_FIP_TO_FE_ID,param_i);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    sr_link_list.path_links[param_i - 1] = (uint8) param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_CELL_WRITE_DIRECT_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_CELL_WRITE_DIRECT_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_CELL_WRITE_DIRECT_DATA_IN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_CELL_WRITE_DIRECT_DATA_IN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data_in = (uint32) param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_write_to_fe600(
          unit,
          &sr_link_list,
          sizeof(uint32),
          offset,
          &data_in
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** tmd_write_to_fe600 - FAIL", TRUE);
    soc_petra_disp_result(ret, "tmd_write_to_fe600");
    goto exit;
  }

exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: tmd_indirect_read_from_fe600 (section cell)
 */
int
ui_tmd_api_cell_tmd_indirect_read_from_fe600(
  CURRENT_LINE *current_line
  )
{
  uint32
    indx = 0,
    param_i = 0,
    offset = 0,
    data_out[TMD_CELL_NOF_DATA_WORDS_IN_INDIRECT_CELL] = {0,0,0} ,
    ret = 0;
  TMD_SR_CELL_LINK_LIST
    sr_link_list ;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_cell");
  soc_sand_proc_name = "tmd_indirect_read_from_fe600";

  unit = tmd_get_default_unit();

  for (indx = 0; indx < TMD_CELL_NOF_LINKS_IN_PATH_LINKS; ++indx)
  {
    sr_link_list.path_links[indx] = 0;
  }

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_CELL_READ_INDIRECT_DEST_ENTITY_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_CELL_READ_INDIRECT_DEST_ENTITY_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    sr_link_list.dest_entity_type = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter dest_entity_type after tmd_indirect_read_from_fe600*** ", TRUE);
    goto exit;
  }

  for (param_i=1; param_i<=TMD_CELL_NOF_LINKS_IN_PATH_LINKS; ++param_i)
  {
    ret = search_param_val_pairs(current_line,&match_index,PARAM_TMD_CELL_READ_INDIRECT_LINK_FIP_TO_FE_ID,param_i);
    if (0 != ret)
    {
      ret=0;
      break;
    }
    UI_MACROS_GET_NUMMERIC_VAL_WITH_INDEX(PARAM_TMD_CELL_READ_INDIRECT_LINK_FIP_TO_FE_ID,param_i);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    sr_link_list.path_links[param_i - 1] = (uint8) param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_CELL_READ_INDIRECT_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_CELL_READ_INDIRECT_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_indirect_read_from_fe600(
          unit,
          &sr_link_list,
          sizeof(uint32),
          offset,
          data_out
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** tmd_indirect_read_from_fe600 - FAIL", TRUE);
    soc_petra_disp_result(ret, "tmd_indirect_read_from_fe600");
    goto exit;
  }

  soc_sand_os_printf(  "data_out[0]: %lx\n\r" , data_out[0] ) ;
  soc_sand_os_printf(  "data_out[1]: %lx\n\r" , data_out[1] ) ;
  soc_sand_os_printf(  "data_out[2]: %lx\n\r" , data_out[2] ) ;


exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: tmd_indirect_write_to_fe600 (section cell)
 */
int
ui_tmd_api_cell_tmd_indirect_write_to_fe600(
  CURRENT_LINE *current_line
  )
{
  uint32
    offset = 0,
    indx = 0,
    param_i = 0,
    data_in ,
    ret = 0;
  TMD_SR_CELL_LINK_LIST
    sr_link_list ;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_cell");
  soc_sand_proc_name = "tmd_indirect_write_from_fe600";

  unit = tmd_get_default_unit();

  for (indx = 0; indx < TMD_CELL_NOF_LINKS_IN_PATH_LINKS; ++indx)
  {
    sr_link_list.path_links[indx] = 0;
  }

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_CELL_WRITE_INDIRECT_DEST_ENTITY_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_CELL_WRITE_INDIRECT_DEST_ENTITY_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    sr_link_list.dest_entity_type = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter dest_entity_type after tmd_indirect_write_from_fe600*** ", TRUE);
    goto exit;
  }

  for (param_i=1; param_i<=TMD_CELL_NOF_LINKS_IN_PATH_LINKS; ++param_i)
  {
    ret = search_param_val_pairs(current_line,&match_index,PARAM_TMD_CELL_WRITE_INDIRECT_LINK_FIP_TO_FE_ID,param_i);
    if (0 != ret)
    {
      ret=0;
      break;
    }
    UI_MACROS_GET_NUMMERIC_VAL_WITH_INDEX(PARAM_TMD_CELL_WRITE_INDIRECT_LINK_FIP_TO_FE_ID,param_i);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    sr_link_list.path_links[param_i - 1] = (uint8) param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_CELL_WRITE_INDIRECT_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_CELL_WRITE_INDIRECT_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_CELL_WRITE_INDIRECT_DATA_IN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_CELL_WRITE_INDIRECT_DATA_IN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data_in = param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_indirect_write_to_fe600(
          unit,
          &sr_link_list,
          sizeof(uint32),
          offset,
          &data_in
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** tmd_indirect_write_to_fe600 - FAIL", TRUE);
    soc_petra_disp_result(ret, "tmd_indirect_write_to_fe600");
    goto exit;
  }

exit:
  return ui_ret;
}


/********************************************************************
*  Function handler: tmd_cpu2cpu_write (section cell)
 */
int
  ui_tmd_api_cell_tmd_cpu2cpu_write(
    CURRENT_LINE *current_line
  )
{
  uint32
    param_i,
    data_in[SOC_SAND_DATA_CELL_PAYLOAD_IN_UINT32S],
    indx,
    ret = 0;
  TMD_SR_CELL_LINK_LIST
    sr_link_list ;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_cell");
  soc_sand_proc_name = "tmd_cpu2cpu_write";

  unit = tmd_get_default_unit();

  /* Initialization */
  for (indx = 0; indx < SOC_SAND_DATA_CELL_PAYLOAD_IN_UINT32S; ++indx)
  {
    data_in[indx] = 0;
  }
  for (indx = 0; indx < TMD_CELL_NOF_LINKS_IN_PATH_LINKS; ++indx)
  {
    sr_link_list.path_links[indx] = 0;
  }

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_CELL_CPU2CPU_DEST_ENTITY_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_CELL_CPU2CPU_DEST_ENTITY_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    sr_link_list.dest_entity_type = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter dest_entity_type after tmd_read_from_fe600*** ", TRUE);
    goto exit;
  }

  for (param_i=1; param_i<=TMD_CELL_NOF_LINKS_IN_PATH_LINKS; ++param_i)
  {
    ret = search_param_val_pairs(current_line,&match_index,PARAM_TMD_CELL_CPU2CPU_LINK_FIP_TO_FE_ID,param_i);
    if (0 != ret)
    {
      ret=0;
      break;
    }
    UI_MACROS_GET_NUMMERIC_VAL_WITH_INDEX(PARAM_TMD_CELL_CPU2CPU_LINK_FIP_TO_FE_ID,param_i);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    sr_link_list.path_links[param_i - 1] = (uint8) param_val->value.ulong_value;
  }

  for (param_i=1; param_i<=SOC_SAND_DATA_CELL_PAYLOAD_IN_UINT32S; ++param_i)
  {
    ret = search_param_val_pairs(current_line,&match_index,PARAM_TMD_CELL_CPU2CPU_DATA_IN_ID,param_i);
    if (0 != ret)
    {
      ret=0;
      break;
    }
    UI_MACROS_GET_NUMMERIC_VAL_WITH_INDEX(PARAM_TMD_CELL_CPU2CPU_DATA_IN_ID,param_i);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data_in[param_i - 1] = (uint32) param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_cpu2cpu_write(
          unit,
          &sr_link_list,
          SOC_SAND_DATA_CELL_PAYLOAD_IN_BYTES,
          data_in
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** tmd_cpu2cpu_write - FAIL", TRUE);
    soc_petra_disp_result(ret, "tmd_cpu2cpu_write");
    goto exit;
  }

exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: tmd_cpu2cpu_read (section cell)
 */
int
  ui_tmd_api_cell_tmd_cpu2cpu_read(
    CURRENT_LINE *current_line
  )
{
  uint32
    data_out[SOC_SAND_DATA_CELL_PAYLOAD_IN_UINT32S],
    indx,
    ret = 0;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_cell");
  soc_sand_proc_name = "tmd_cpu2cpu_write";

  unit = tmd_get_default_unit();

  /* Initialization */
  for (indx = 0; indx < SOC_SAND_DATA_CELL_PAYLOAD_IN_UINT32S; ++indx)
  {
    data_out[indx] = 0;
  }

  /* Call function */
  ret = tmd_cpu2cpu_read(
          unit,
          data_out
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** tmd_cpu2cpu_read - FAIL", TRUE);
    soc_petra_disp_result(ret, "tmd_cpu2cpu_read");
    goto exit;
  }

  soc_sand_os_printf(  "data_out: %lu " , (data_out[0]) ) ;
  for (indx = 1; indx < SOC_SAND_DATA_CELL_PAYLOAD_IN_UINT32S; ++indx)
  {
    soc_sand_os_printf(  "%lu " , (data_out[indx]) ) ;
  }
  soc_sand_os_printf(  "\n\r" ) ;

exit:
  return ui_ret;
}
/******************************************************************** 
 *  Function handler: mc_tbl_write (section cell)
 */
int 
  ui_tmd_api_cell_mc_tbl_write(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_mc_id_ndx;
  uint32 
    prm_filter_index = 0xFFFFFFFF;  
  uint32 
    prm_path_links_index = 0xFFFFFFFF;  
  TMD_CELL_MC_TBL_INFO   
    prm_info;
  uint32 
    prm_data_index = 0xFFFFFFFF;  
  TMD_CELL_MC_TBL_DATA   
    prm_tbl_data;
   
  UI_MACROS_INIT_FUNCTION("ui_tmd_api_cell"); 
  soc_sand_proc_name = "tmd_cell_mc_tbl_write"; 
 
  unit = tmd_get_default_unit(); 
  TMD_CELL_MC_TBL_INFO_clear(&prm_info);
  TMD_CELL_MC_TBL_DATA_clear(&prm_tbl_data);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_CELL_MC_TBL_WRITE_MC_TBL_WRITE_MC_ID_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_CELL_MC_TBL_WRITE_MC_TBL_WRITE_MC_ID_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_mc_id_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter mc_id_ndx after mc_tbl_write***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_CELL_MC_TBL_WRITE_MC_TBL_WRITE_INFO_FILTER_INDEX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_CELL_MC_TBL_WRITE_MC_TBL_WRITE_INFO_FILTER_INDEX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_filter_index = (uint32)param_val->value.ulong_value;
  } 

  if(prm_filter_index != 0xFFFFFFFF)          
  { 

    if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_CELL_MC_TBL_WRITE_MC_TBL_WRITE_INFO_FILTER_ID,1)) 
    { 
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_CELL_MC_TBL_WRITE_MC_TBL_WRITE_INFO_FILTER_ID);  
      UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
      prm_info.filter[ prm_filter_index] = (uint8)param_val->value.ulong_value;
    } 

  } 
  else
  {
    for (prm_filter_index = 0; prm_filter_index < TMD_CELL_MC_NOF_CHANGES; ++prm_filter_index)
    {
      prm_info.filter[ prm_filter_index] = TRUE;
    }
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_CELL_MC_TBL_WRITE_MC_TBL_WRITE_INFO_PATH_LINKS_INDEX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_CELL_MC_TBL_WRITE_MC_TBL_WRITE_INFO_PATH_LINKS_INDEX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_path_links_index = (uint32)param_val->value.ulong_value;
  } 

  if(prm_path_links_index != 0xFFFFFFFF)          
  { 

    if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_CELL_MC_TBL_WRITE_MC_TBL_WRITE_INFO_PATH_LINKS_ID,1)) 
    { 
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_CELL_MC_TBL_WRITE_MC_TBL_WRITE_INFO_PATH_LINKS_ID);  
      UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
      prm_info.path_links[ prm_path_links_index] = (uint32)param_val->value.ulong_value;
    } 

  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_CELL_MC_TBL_WRITE_MC_TBL_WRITE_INFO_FE_LOCATION_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_CELL_MC_TBL_WRITE_MC_TBL_WRITE_INFO_FE_LOCATION_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.fe_location = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_CELL_MC_TBL_WRITE_MC_TBL_WRITE_TBL_DATA_DATA_INDEX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_CELL_MC_TBL_WRITE_MC_TBL_WRITE_TBL_DATA_DATA_INDEX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_data_index = (uint32)param_val->value.ulong_value;
  } 

  if(prm_data_index != 0xFFFFFFFF)          
  { 

    if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_CELL_MC_TBL_WRITE_MC_TBL_WRITE_TBL_DATA_DATA_ID,1)) 
    { 
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_CELL_MC_TBL_WRITE_MC_TBL_WRITE_TBL_DATA_DATA_ID);  
      UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
      prm_tbl_data.data[ prm_data_index] = (uint32)param_val->value.ulong_value;
    } 

  }   


  /* Call function */
  ret = tmd_cell_mc_tbl_write(
          unit,
          prm_mc_id_ndx,
          &prm_info,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** tmd_cell_mc_tbl_write - FAIL", TRUE); 
    soc_petra_disp_result(ret, "tmd_cell_mc_tbl_write");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: mc_tbl_read (section cell)
 */
int 
  ui_tmd_api_cell_mc_tbl_read(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_mc_id_ndx;
  uint32 
    prm_filter_index = 0xFFFFFFFF;  
  uint32 
    prm_path_links_index = 0xFFFFFFFF;  
  TMD_CELL_MC_TBL_INFO   
    prm_info;
  uint32 
    prm_data_index = 0xFFFFFFFF;  
  TMD_CELL_MC_TBL_DATA   
    prm_tbl_data;
   
  UI_MACROS_INIT_FUNCTION("ui_tmd_api_cell"); 
  soc_sand_proc_name = "tmd_cell_mc_tbl_read"; 
 
  unit = tmd_get_default_unit(); 
  TMD_CELL_MC_TBL_INFO_clear(&prm_info);
  TMD_CELL_MC_TBL_DATA_clear(&prm_tbl_data);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_CELL_MC_TBL_READ_MC_TBL_READ_MC_ID_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_CELL_MC_TBL_READ_MC_TBL_READ_MC_ID_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_mc_id_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter mc_id_ndx after mc_tbl_read***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_CELL_MC_TBL_READ_MC_TBL_READ_INFO_FILTER_INDEX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_CELL_MC_TBL_READ_MC_TBL_READ_INFO_FILTER_INDEX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_filter_index = (uint32)param_val->value.ulong_value;
  } 

  if(prm_filter_index != 0xFFFFFFFF)          
  { 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_CELL_MC_TBL_READ_MC_TBL_READ_INFO_FILTER_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_CELL_MC_TBL_READ_MC_TBL_READ_INFO_FILTER_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.filter[ prm_filter_index] = (uint8)param_val->value.ulong_value;
  } 

  } 
  else
  {
    for (prm_filter_index = 0; prm_filter_index < TMD_CELL_MC_NOF_CHANGES; ++prm_filter_index)
    {
      prm_info.filter[ prm_filter_index] = TRUE;
    }
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_CELL_MC_TBL_READ_MC_TBL_READ_INFO_PATH_LINKS_INDEX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_CELL_MC_TBL_READ_MC_TBL_READ_INFO_PATH_LINKS_INDEX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_path_links_index = (uint32)param_val->value.ulong_value;
  } 

  if(prm_path_links_index != 0xFFFFFFFF)          
  { 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_CELL_MC_TBL_READ_MC_TBL_READ_INFO_PATH_LINKS_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_CELL_MC_TBL_READ_MC_TBL_READ_INFO_PATH_LINKS_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.path_links[ prm_path_links_index] = (uint32)param_val->value.ulong_value;
  } 

  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_CELL_MC_TBL_READ_MC_TBL_READ_INFO_FE_LOCATION_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_CELL_MC_TBL_READ_MC_TBL_READ_INFO_FE_LOCATION_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.fe_location = param_val->numeric_equivalent;
  } 


  /* Call function */
  ret = tmd_cell_mc_tbl_read(
          unit,
          prm_mc_id_ndx,
          &prm_info,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** tmd_cell_mc_tbl_read - FAIL", TRUE); 
    soc_petra_disp_result(ret, "tmd_cell_mc_tbl_read");   
    goto exit; 
  } 

  TMD_CELL_MC_TBL_DATA_print(&prm_tbl_data);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
#endif

#ifdef UI_TMD_HEADER_PARSING_UTILS
/********************************************************************
 *  Function handler: hpu_itmh_build (section tmd_header_parsing_utils)
 */
int
  ui_tmd_header_parsing_utils_hpu_itmh_build(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_PORTS_ITMH
    prm_info;
  TMD_HPU_ITMH_HDR
    prm_itmh;

  UI_MACROS_INIT_FUNCTION("ui_tmd_header_parsing_utils");
  soc_sand_proc_name = "tmd_hpu_itmh_build";

  unit = tmd_get_default_unit();
  TMD_PORTS_ITMH_clear(&prm_info);
  TMD_HPU_ITMH_HDR_clear(&prm_itmh);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_HPU_ITMH_BUILD_HPU_ITMH_BUILD_INFO_EXTENSION_SRC_PORT_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_HPU_ITMH_BUILD_HPU_ITMH_BUILD_INFO_EXTENSION_SRC_PORT_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.extension.src_port.id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_HPU_ITMH_BUILD_HPU_ITMH_BUILD_INFO_EXTENSION_SRC_PORT_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_HPU_ITMH_BUILD_HPU_ITMH_BUILD_INFO_EXTENSION_SRC_PORT_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.extension.src_port.type = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_HPU_ITMH_BUILD_HPU_ITMH_BUILD_INFO_EXTENSION_IS_SRC_SYS_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_HPU_ITMH_BUILD_HPU_ITMH_BUILD_INFO_EXTENSION_IS_SRC_SYS_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.extension.is_src_sys_port = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_HPU_ITMH_BUILD_HPU_ITMH_BUILD_INFO_EXTENSION_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_HPU_ITMH_BUILD_HPU_ITMH_BUILD_INFO_EXTENSION_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.extension.enable = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_HPU_ITMH_BUILD_HPU_ITMH_BUILD_INFO_BASE_DESTINATION_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_HPU_ITMH_BUILD_HPU_ITMH_BUILD_INFO_BASE_DESTINATION_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.base.destination.id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_HPU_ITMH_BUILD_HPU_ITMH_BUILD_INFO_BASE_DESTINATION_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_HPU_ITMH_BUILD_HPU_ITMH_BUILD_INFO_BASE_DESTINATION_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.base.destination.type = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_HPU_ITMH_BUILD_HPU_ITMH_BUILD_INFO_BASE_DP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_HPU_ITMH_BUILD_HPU_ITMH_BUILD_INFO_BASE_DP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.base.dp = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_HPU_ITMH_BUILD_HPU_ITMH_BUILD_INFO_BASE_TR_CLS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_HPU_ITMH_BUILD_HPU_ITMH_BUILD_INFO_BASE_TR_CLS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.base.tr_cls = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_HPU_ITMH_BUILD_HPU_ITMH_BUILD_INFO_BASE_EXCLUDE_SRC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_HPU_ITMH_BUILD_HPU_ITMH_BUILD_INFO_BASE_EXCLUDE_SRC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.base.exclude_src = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_HPU_ITMH_BUILD_HPU_ITMH_BUILD_INFO_BASE_SNOOP_CMD_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_HPU_ITMH_BUILD_HPU_ITMH_BUILD_INFO_BASE_SNOOP_CMD_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.base.snoop_cmd_ndx = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_HPU_ITMH_BUILD_HPU_ITMH_BUILD_INFO_BASE_OUT_MIRROR_EN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_HPU_ITMH_BUILD_HPU_ITMH_BUILD_INFO_BASE_OUT_MIRROR_EN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.base.out_mirror_dis = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_HPU_ITMH_BUILD_HPU_ITMH_BUILD_INFO_BASE_PP_HEADER_PRESENT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_HPU_ITMH_BUILD_HPU_ITMH_BUILD_INFO_BASE_PP_HEADER_PRESENT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.base.pp_header_present = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_hpu_itmh_build(
          &prm_info,
          &prm_itmh
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_hpu_itmh_build");
    goto exit;
  }

  send_string_to_screen("--> itmh:", TRUE);
  TMD_HPU_ITMH_HDR_print(&prm_itmh);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: hpu_itmh_parse (section tmd_header_parsing_utils)
 */
int
  ui_tmd_header_parsing_utils_hpu_itmh_parse(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_HPU_ITMH_HDR
    prm_itmh;
  TMD_PORTS_ITMH
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_header_parsing_utils");
  soc_sand_proc_name = "tmd_hpu_itmh_parse";

  unit = tmd_get_default_unit();
  TMD_HPU_ITMH_HDR_clear(&prm_itmh);
  TMD_PORTS_ITMH_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_HPU_ITMH_PARSE_HPU_ITMH_PARSE_ITMH_EXTENTION_SRC_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_HPU_ITMH_PARSE_HPU_ITMH_PARSE_ITMH_EXTENTION_SRC_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_itmh.extention_src_port = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_HPU_ITMH_PARSE_HPU_ITMH_PARSE_ITMH_BASE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_HPU_ITMH_PARSE_HPU_ITMH_PARSE_ITMH_BASE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_itmh.base = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_hpu_itmh_parse(
          &prm_itmh,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_hpu_itmh_parse");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  TMD_PORTS_ITMH_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: hpu_ftmh_build (section tmd_header_parsing_utils)
 */
int
  ui_tmd_header_parsing_utils_hpu_ftmh_build(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_HPU_FTMH
    prm_info;
  TMD_HPU_FTMH_HDR
    prm_ftmh;

  UI_MACROS_INIT_FUNCTION("ui_tmd_header_parsing_utils");
  soc_sand_proc_name = "tmd_hpu_ftmh_build";

  unit = tmd_get_default_unit();
  TMD_HPU_FTMH_clear(&prm_info);
  TMD_HPU_FTMH_HDR_clear(&prm_ftmh);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_HPU_FTMH_BUILD_HPU_FTMH_BUILD_INFO_EXTENSION_OUTLIF_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_HPU_FTMH_BUILD_HPU_FTMH_BUILD_INFO_EXTENSION_OUTLIF_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.extension.outlif = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_HPU_FTMH_BUILD_HPU_FTMH_BUILD_INFO_EXTENSION_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_HPU_FTMH_BUILD_HPU_FTMH_BUILD_INFO_EXTENSION_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.extension.enable = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_HPU_FTMH_BUILD_HPU_FTMH_BUILD_INFO_BASE_MULTICAST_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_HPU_FTMH_BUILD_HPU_FTMH_BUILD_INFO_BASE_MULTICAST_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.base.multicast = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_HPU_FTMH_BUILD_HPU_FTMH_BUILD_INFO_BASE_EXCLUDE_SRC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_HPU_FTMH_BUILD_HPU_FTMH_BUILD_INFO_BASE_EXCLUDE_SRC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.base.exclude_src = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_HPU_FTMH_BUILD_HPU_FTMH_BUILD_INFO_BASE_OUT_MIRROR_EN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_HPU_FTMH_BUILD_HPU_FTMH_BUILD_INFO_BASE_OUT_MIRROR_EN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.base.out_mirror_dis = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_HPU_FTMH_BUILD_HPU_FTMH_BUILD_INFO_BASE_PP_HEADER_PRESENT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_HPU_FTMH_BUILD_HPU_FTMH_BUILD_INFO_BASE_PP_HEADER_PRESENT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.base.pp_header_present = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_HPU_FTMH_BUILD_HPU_FTMH_BUILD_INFO_BASE_SIGNATURE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_HPU_FTMH_BUILD_HPU_FTMH_BUILD_INFO_BASE_SIGNATURE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.base.signature = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_HPU_FTMH_BUILD_HPU_FTMH_BUILD_INFO_BASE_DP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_HPU_FTMH_BUILD_HPU_FTMH_BUILD_INFO_BASE_DP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.base.dp = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_HPU_FTMH_BUILD_HPU_FTMH_BUILD_INFO_BASE_OFP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_HPU_FTMH_BUILD_HPU_FTMH_BUILD_INFO_BASE_OFP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.base.ofp = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_HPU_FTMH_BUILD_HPU_FTMH_BUILD_INFO_BASE_SRC_SYS_PORT_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_HPU_FTMH_BUILD_HPU_FTMH_BUILD_INFO_BASE_SRC_SYS_PORT_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.base.src_sys_port.id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_HPU_FTMH_BUILD_HPU_FTMH_BUILD_INFO_BASE_SRC_SYS_PORT_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_HPU_FTMH_BUILD_HPU_FTMH_BUILD_INFO_BASE_SRC_SYS_PORT_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.base.src_sys_port.type = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_HPU_FTMH_BUILD_HPU_FTMH_BUILD_INFO_BASE_TR_CLS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_HPU_FTMH_BUILD_HPU_FTMH_BUILD_INFO_BASE_TR_CLS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.base.tr_cls = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_HPU_FTMH_BUILD_HPU_FTMH_BUILD_INFO_BASE_PACKET_SIZE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_HPU_FTMH_BUILD_HPU_FTMH_BUILD_INFO_BASE_PACKET_SIZE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.base.packet_size = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_hpu_ftmh_build(
          &prm_info,
          &prm_ftmh
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_hpu_ftmh_build");
    goto exit;
  }

  send_string_to_screen("--> ftmh:", TRUE);
  TMD_HPU_FTMH_HDR_print(&prm_ftmh);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: hpu_ftmh_parse (section tmd_header_parsing_utils)
 */
int
  ui_tmd_header_parsing_utils_hpu_ftmh_parse(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_base_index = 0xFFFFFFFF;
  TMD_HPU_FTMH_HDR
    prm_ftmh;
  TMD_HPU_FTMH
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_header_parsing_utils");
  soc_sand_proc_name = "tmd_hpu_ftmh_parse";

  unit = tmd_get_default_unit();
  TMD_HPU_FTMH_HDR_clear(&prm_ftmh);
  TMD_HPU_FTMH_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_HPU_FTMH_PARSE_HPU_FTMH_PARSE_FTMH_EXTENSION_OUTLIF_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_HPU_FTMH_PARSE_HPU_FTMH_PARSE_FTMH_EXTENSION_OUTLIF_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ftmh.extension_outlif = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_HPU_FTMH_PARSE_HPU_FTMH_PARSE_FTMH_BASE_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_HPU_FTMH_PARSE_HPU_FTMH_PARSE_FTMH_BASE_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_base_index = (uint32)param_val->value.ulong_value;
  }

  if(prm_base_index != 0xFFFFFFFF)
  {

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_HPU_FTMH_PARSE_HPU_FTMH_PARSE_FTMH_BASE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_HPU_FTMH_PARSE_HPU_FTMH_PARSE_FTMH_BASE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ftmh.base[ prm_base_index] = (uint32)param_val->value.ulong_value;
  }

  }


  /* Call function */
  ret = tmd_hpu_ftmh_parse(
          &prm_ftmh,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_hpu_ftmh_parse");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  TMD_HPU_FTMH_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: hpu_otmh_build (section tmd_header_parsing_utils)
 */
int
  ui_tmd_header_parsing_utils_hpu_otmh_build(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_HPU_OTMH
    prm_info;
  TMD_HPU_OTMH_HDR
    prm_otmh;

  UI_MACROS_INIT_FUNCTION("ui_tmd_header_parsing_utils");
  soc_sand_proc_name = "tmd_hpu_otmh_build";

  unit = tmd_get_default_unit();
  TMD_HPU_OTMH_clear(&prm_info);
  TMD_HPU_OTMH_HDR_clear(&prm_otmh);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_HPU_OTMH_BUILD_HPU_OTMH_BUILD_INFO_EXTENSION_DEST_DEST_SYS_PORT_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_HPU_OTMH_BUILD_HPU_OTMH_BUILD_INFO_EXTENSION_DEST_DEST_SYS_PORT_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.extension.dest.dest_sys_port.id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_HPU_OTMH_BUILD_HPU_OTMH_BUILD_INFO_EXTENSION_DEST_DEST_SYS_PORT_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_HPU_OTMH_BUILD_HPU_OTMH_BUILD_INFO_EXTENSION_DEST_DEST_SYS_PORT_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.extension.dest.dest_sys_port.type = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_HPU_OTMH_BUILD_HPU_OTMH_BUILD_INFO_EXTENSION_DEST_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_HPU_OTMH_BUILD_HPU_OTMH_BUILD_INFO_EXTENSION_DEST_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.extension.dest.enable = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_HPU_OTMH_BUILD_HPU_OTMH_BUILD_INFO_EXTENSION_SRC_SRC_SYS_PORT_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_HPU_OTMH_BUILD_HPU_OTMH_BUILD_INFO_EXTENSION_SRC_SRC_SYS_PORT_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.extension.src.src_sys_port.id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_HPU_OTMH_BUILD_HPU_OTMH_BUILD_INFO_EXTENSION_SRC_SRC_SYS_PORT_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_HPU_OTMH_BUILD_HPU_OTMH_BUILD_INFO_EXTENSION_SRC_SRC_SYS_PORT_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.extension.src.src_sys_port.type = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_HPU_OTMH_BUILD_HPU_OTMH_BUILD_INFO_EXTENSION_SRC_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_HPU_OTMH_BUILD_HPU_OTMH_BUILD_INFO_EXTENSION_SRC_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.extension.src.enable = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_HPU_OTMH_BUILD_HPU_OTMH_BUILD_INFO_EXTENSION_OUTLIF_OUTLIF_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_HPU_OTMH_BUILD_HPU_OTMH_BUILD_INFO_EXTENSION_OUTLIF_OUTLIF_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.extension.outlif.outlif = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_HPU_OTMH_BUILD_HPU_OTMH_BUILD_INFO_EXTENSION_OUTLIF_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_HPU_OTMH_BUILD_HPU_OTMH_BUILD_INFO_EXTENSION_OUTLIF_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.extension.outlif.enable = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_HPU_OTMH_BUILD_HPU_OTMH_BUILD_INFO_BASE_OUTLIF_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_HPU_OTMH_BUILD_HPU_OTMH_BUILD_INFO_BASE_OUTLIF_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.base.outlif = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_HPU_OTMH_BUILD_HPU_OTMH_BUILD_INFO_BASE_TR_CLS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_HPU_OTMH_BUILD_HPU_OTMH_BUILD_INFO_BASE_TR_CLS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.base.tr_cls = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_HPU_OTMH_BUILD_HPU_OTMH_BUILD_INFO_BASE_PACKET_SIZE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_HPU_OTMH_BUILD_HPU_OTMH_BUILD_INFO_BASE_PACKET_SIZE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.base.packet_size = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_HPU_OTMH_BUILD_HPU_OTMH_BUILD_INFO_BASE_DP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_HPU_OTMH_BUILD_HPU_OTMH_BUILD_INFO_BASE_DP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.base.dp = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_HPU_OTMH_BUILD_HPU_OTMH_BUILD_INFO_BASE_MULTICAST_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_HPU_OTMH_BUILD_HPU_OTMH_BUILD_INFO_BASE_MULTICAST_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.base.multicast = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_HPU_OTMH_BUILD_HPU_OTMH_BUILD_INFO_BASE_SIGNATURE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_HPU_OTMH_BUILD_HPU_OTMH_BUILD_INFO_BASE_SIGNATURE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.base.signature = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_HPU_OTMH_BUILD_HPU_OTMH_BUILD_INFO_BASE_PP_HEADER_PRESENT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_HPU_OTMH_BUILD_HPU_OTMH_BUILD_INFO_BASE_PP_HEADER_PRESENT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.base.pp_header_present = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_hpu_otmh_build(
          &prm_info,
          &prm_otmh
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_hpu_otmh_build");
    goto exit;
  }

  send_string_to_screen("--> otmh:", TRUE);
  TMD_HPU_OTMH_HDR_print(&prm_otmh);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: hpu_otmh_parse (section tmd_header_parsing_utils)
 */
int
  ui_tmd_header_parsing_utils_hpu_otmh_parse(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_HPU_OTMH_HDR
    prm_otmh;
  TMD_HPU_OTMH
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_header_parsing_utils");
  soc_sand_proc_name = "tmd_hpu_otmh_parse";

  unit = tmd_get_default_unit();
  TMD_HPU_OTMH_HDR_clear(&prm_otmh);
  TMD_HPU_OTMH_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_HPU_OTMH_PARSE_HPU_OTMH_PARSE_OTMH_EXTENSION_DEST_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_HPU_OTMH_PARSE_HPU_OTMH_PARSE_OTMH_EXTENSION_DEST_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_otmh.extension_dest_port = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_HPU_OTMH_PARSE_HPU_OTMH_PARSE_OTMH_EXTENSION_SRC_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_HPU_OTMH_PARSE_HPU_OTMH_PARSE_OTMH_EXTENSION_SRC_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_otmh.extension_src_port = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_HPU_OTMH_PARSE_HPU_OTMH_PARSE_OTMH_EXTENSION_OUTLIF_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_HPU_OTMH_PARSE_HPU_OTMH_PARSE_OTMH_EXTENSION_OUTLIF_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_otmh.extension_outlif = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_HPU_OTMH_PARSE_HPU_OTMH_PARSE_OTMH_BASE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_HPU_OTMH_PARSE_HPU_OTMH_PARSE_OTMH_BASE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_otmh.base = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_hpu_otmh_parse(
          &prm_otmh,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_hpu_otmh_parse");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  TMD_HPU_OTMH_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

#endif

#ifdef UI_REG_ACCESS
STATIC uint32
  tmd_ui_read_fld(
    SOC_SAND_IN  int      unit,
    SOC_SAND_IN  TMD_REG_FIELD*  field,
    SOC_SAND_OUT uint32*         val
  )
{
  uint32
    res = SOC_SAND_OK,
    offset = 0,
    reg_val = 0,
    fld_val = 0;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_READ_FLD_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(field);
  SOC_SAND_CHECK_NULL_INPUT(val);


  offset = field->addr.base;

  res = soc_sand_mem_read_unsafe(
          unit,
          &(reg_val),
          offset,
          sizeof(uint32),
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_tmcfield_from_reg_get(
          &(reg_val),
          field,
          &(fld_val)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  *val = fld_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in tmd_ui_read_fld",0,0);
}

STATIC uint32
  tmd_ui_write_fld(
    SOC_SAND_IN  int      unit,
    SOC_SAND_IN  TMD_REG_FIELD*  field,
    SOC_SAND_IN  uint32          val
  )
{
  uint32
    max_fld_val,
    res = SOC_SAND_OK;
  uint32
    offset = 0;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_WRITE_FLD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(field);

  max_fld_val = SOC_TMC_FLD_MAX(*field);
  
  SOC_SAND_ERR_IF_ABOVE_MAX(
    val, max_fld_val,
    SOC_TMC_REGS_FIELD_VAL_OUT_OF_RANGE_ERR, 10, exit
  );


  offset = field->addr.base;

  res = soc_sand_read_modify_write(
          soc_sand_get_chip_descriptor_base_addr(unit),
          offset,
          field->lsb,
          SOC_SAND_BITS_MASK(field->msb, field->lsb),
          val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in tmd_ui_write_fld",offset,val);
}

/********************************************************************
 *  Function handler: read_fld (section reg_access)
 */
int
  ui_tmd_api_reg_access_read_fld(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_REG_FIELD
    prm_field;
  uint32
    prm_val = 0;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_reg_access");
  soc_sand_proc_name = "tmd_read_fld";

  unit = tmd_get_default_unit();
  prm_field.addr.step = 0;
  prm_field.lsb = 0;
  prm_field.msb = 31;

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_READ_FLD_READ_FLD_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_READ_FLD_READ_FLD_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_field.addr.base = (uint32)param_val->value.ulong_value;
  }

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_READ_FLD_READ_FLD_FIELD_LSB_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_READ_FLD_READ_FLD_FIELD_LSB_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_field.lsb = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_READ_FLD_READ_FLD_FIELD_MSB_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_READ_FLD_READ_FLD_FIELD_MSB_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_field.msb = (uint8)param_val->value.ulong_value;
  }

  prm_field.addr.base *= 4;

  /* Call function */
  ret = tmd_ui_read_fld(
          unit,
          &prm_field,
          &prm_val
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_read_fld");
    goto exit;
  }

  soc_sand_os_printf( "val: 0x%.8lX\n\r",prm_val);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: write_fld (section reg_access)
 */
int
  ui_tmd_api_reg_access_write_fld(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_REG_FIELD
    prm_field;
  uint32
    prm_val = 0;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_reg_access");
  soc_sand_proc_name = "tmd_write_fld";

  unit = tmd_get_default_unit();
  prm_field.addr.step = 0;
  prm_field.lsb = 0;
  prm_field.msb = 31;

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_WRITE_FLD_WRITE_FLD_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_WRITE_FLD_WRITE_FLD_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_field.addr.base = (uint32)param_val->value.ulong_value;
  }

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_WRITE_FLD_WRITE_FLD_FIELD_LSB_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_WRITE_FLD_WRITE_FLD_FIELD_LSB_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_field.lsb = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_WRITE_FLD_WRITE_FLD_FIELD_MSB_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_WRITE_FLD_WRITE_FLD_FIELD_MSB_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_field.msb = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_WRITE_FLD_WRITE_FLD_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_WRITE_FLD_WRITE_FLD_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_val = (uint32)param_val->value.ulong_value;
  }

  prm_field.addr.base *= 4;

  /* Call function */
  ret = tmd_ui_write_fld(
          unit,
          &prm_field,
          prm_val
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_write_fld");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: read_reg (section reg_access)
 */
int
  ui_tmd_api_reg_access_read_reg(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_REG_FIELD
    prm_field;
  uint32
    prm_val,
    do_ndx,
    nof_occurs = 1,
    wait_time=1,
    sec[3],
    nano[3];

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_reg_access");
  soc_sand_proc_name = "tmd_read_reg";

  unit = tmd_get_default_unit();
  prm_field.addr.step = 0;
  prm_field.lsb = 0;
  prm_field.msb = 31;
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_READ_REG_READ_REG_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_READ_REG_READ_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_field.addr.base = (uint32)param_val->value.ulong_value;
  }

  /* Get parameters */
  prm_field.addr.base *= 4;

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_DO_TWICE_ID, 1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_DO_TWICE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    nof_occurs = 2;
    wait_time = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  for(do_ndx=0;do_ndx<nof_occurs;do_ndx++)
  {
    ret = tmd_ui_read_fld(
            unit,
            &prm_field,
            &prm_val
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      soc_petra_disp_result(ret, "tmd_read_reg");
      goto exit;
    }

    if(nof_occurs==2)
    {
      soc_sand_os_get_time(&sec[do_ndx],&nano[do_ndx]);
      if(do_ndx == 0)
      {
        sal_msleep(wait_time);
      }
      else
      {
        soc_sand_os_get_time_diff(sec[0],nano[0],sec[1],nano[1],&sec[2],&nano[2]);
        soc_sand_os_printf( " Waited %d seconds and %d nanoseconds\n\r", sec[2],nano[2]);
      }
    }
    soc_sand_os_printf( "val: 0x%.8lX\n\r",prm_val);
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: write_reg (section reg_access)
 */
int
  ui_tmd_api_reg_access_write_reg(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_REG_FIELD
    prm_field;
  uint32
    prm_val = 0;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_reg_access");
  soc_sand_proc_name = "tmd_write_reg";

  unit = tmd_get_default_unit();
  prm_field.addr.step = 0;
  prm_field.lsb = 0;
  prm_field.msb = 31;

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_WRITE_REG_WRITE_REG_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_WRITE_REG_WRITE_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_field.addr.base = (uint32)param_val->value.ulong_value;
  }

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_WRITE_REG_WRITE_REG_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_WRITE_REG_WRITE_REG_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_val = (uint32)param_val->value.ulong_value;
  }

  prm_field.addr.base *= 4;

  /* Call function */
  ret = tmd_ui_write_fld(
          unit,
          &prm_field,
          prm_val
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** tmd_write_reg - FAIL", TRUE);
    soc_petra_disp_result(ret, "tmd_write_reg");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}



int
  ui_tmd_iread(
    CURRENT_LINE *current_line
  )
{
  uint32
    addr=0,
    data[3],
    ret=0,
    size=0,
    do_ndx,
    nof_occurs = 1,
    wait_time=0,
    sec[3],
    nano[3],
    module_id = 0;

  UI_MACROS_INIT_FUNCTION("ui_tmd_iread");
  soc_sand_proc_name = "ui_tmd_iread";

  unit = tmd_get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_IRW_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_IRW_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    addr = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SIZE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SIZE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    size = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_DO_TWICE_ID, 1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_DO_TWICE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    nof_occurs = 2;
    wait_time = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_IACC_MODULE_ID_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_IACC_MODULE_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    module_id = param_val->numeric_equivalent;
  }

  for(do_ndx=0;do_ndx<nof_occurs;do_ndx++)
  {
    ret = soc_sand_tbl_read(
      unit,
      data,
      addr,
      size * sizeof(uint32),
      module_id,
      size * sizeof(uint32)
      );
    if(nof_occurs==2)
    {
      soc_sand_os_get_time(&sec[do_ndx],&nano[do_ndx]);
      if(do_ndx == 0)
      {
        sal_msleep(wait_time);
      }
      else
      {
        soc_sand_os_get_time_diff(sec[0],nano[0],sec[1],nano[1],&sec[2],&nano[2]);
        soc_sand_os_printf( " Waited %d: %d \n\r", sec[2],nano[2]);
      }
    }
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** soc_sand_mem_iread - FAIL", TRUE);
      soc_petra_disp_result(ret, "soc_sand_mem_iread");
      goto exit;
    }
    soc_sand_os_printf( "addr 0x%x data 0x%x 0x%x 0x%x", addr, data[2], data[1], data[0]);
  }

  goto exit;
exit:
  return ui_ret;
}

int
  ui_tmd_iwrite(
    CURRENT_LINE *current_line
  )
{
  uint32
    addr=0,
    param_i,
    data[3],
    ret,
    vals[3],
    size=0,
    do_ndx,
    nof_occurs = 1,
    wait_time=0,
    sec[3],
    nano[3],
    module_id = 0;

  UI_MACROS_INIT_FUNCTION("ui_tmd_iwrite");
  soc_sand_proc_name = "ui_tmd_iwrite";

  unit = tmd_get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_IRW_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_IRW_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    addr = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SIZE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_SIZE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    size = (uint32)param_val->value.ulong_value;
  }

  for (param_i=1; param_i<=3; ++param_i)
  {
    ret = search_param_val_pairs(current_line,&match_index,PARAM_TMD_ACC_IWRITE_VAL_ID,param_i);
    if (0 != ret)
    {
      ret=0;
      break;
    }
    UI_MACROS_GET_NUMMERIC_VAL_WITH_INDEX(PARAM_TMD_ACC_IWRITE_VAL_ID,param_i);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    vals[param_i-1] = param_val->value.ulong_value;
  }


  if(size == 1)
  {
    data[0] = vals[0];
  }
  else if(size == 2)
  {
    data[1] = vals[0];
    data[0] = vals[1];
  }
 else if(size == 3)
  {
    data[2] = vals[0];
    data[1] = vals[1];
    data[0] = vals[2];
  }
  else
  {
    soc_sand_os_printf( "size %d is invalid\n\r", size);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_DO_TWICE_ID, 1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_DO_TWICE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    nof_occurs = 2;
    wait_time = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_IACC_MODULE_ID_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_IACC_MODULE_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    module_id = param_val->numeric_equivalent;
  }

  for(do_ndx=0;do_ndx<nof_occurs;do_ndx++)
  {
    ret = soc_sand_tbl_write(
      unit,
      data,
      addr,
      size * sizeof(uint32),
      module_id,
      size * sizeof(uint32)
      );
    if(nof_occurs==2)
    {
      soc_sand_os_get_time(&sec[do_ndx],&nano[do_ndx]);
      if(do_ndx == 0)
      {
        sal_msleep(wait_time);
      }
      else
      {
        soc_sand_os_get_time_diff(sec[0],nano[0],sec[1],nano[1],&sec[2],&nano[2]);
        soc_sand_os_printf( " Waited %d: %d \n\r", sec[2],nano[2]);
      }
    }
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** soc_sand_mem_iwrite - FAIL", TRUE);
      soc_petra_disp_result(ret, "soc_sand_mem_iwrite");
      goto exit;
    }
  }


  goto exit;
exit:
  return ui_ret;
}

#endif

#ifdef UI_DEBUG
/********************************************************************
 *  Function handler: dbg_route_force_set (section debug)
 */
int
  ui_tmd_api_debug_dbg_route_force_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_DBG_FORCE_MODE
    prm_force_mode;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_debug");
  soc_sand_proc_name = "tmd_dbg_route_force_set";

  unit = tmd_get_default_unit();

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = tmd_dbg_route_force_get(
          unit,
          &prm_force_mode
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** tmd_dbg_route_force_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "tmd_dbg_route_force_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_DBG_ROUTE_FORCE_SET_DBG_ROUTE_FORCE_SET_FORCE_MODE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_DBG_ROUTE_FORCE_SET_DBG_ROUTE_FORCE_SET_FORCE_MODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_force_mode = param_val->numeric_equivalent;
  }


  /* Call function */
  ret = tmd_dbg_route_force_set(
          unit,
          prm_force_mode
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** tmd_dbg_route_force_set - FAIL", TRUE);
    soc_petra_disp_result(ret, "tmd_dbg_route_force_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: dbg_route_force_get (section debug)
 */
int
  ui_tmd_api_debug_dbg_route_force_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_DBG_FORCE_MODE
    prm_force_mode;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_debug");
  soc_sand_proc_name = "tmd_dbg_route_force_get";

  unit = tmd_get_default_unit();

  /* Get parameters */

  /* Call function */
  ret = tmd_dbg_route_force_get(
          unit,
          &prm_force_mode
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** tmd_dbg_route_force_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "tmd_dbg_route_force_get");
    goto exit;
  }

  soc_sand_os_printf( "force_mode: %s\n\r",TMD_DBG_FORCE_MODE_to_string(prm_force_mode));


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: dbg_autocredit_set (section debug)
 */
int
  ui_tmd_api_debug_dbg_autocredit_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_DBG_AUTOCREDIT_INFO
    prm_info;
  uint32
    prm_exact_rate;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_debug");
  soc_sand_proc_name = "tmd_dbg_autocredit_set";

  unit = tmd_get_default_unit();
  TMD_DBG_AUTOCREDIT_INFO_clear(&prm_info);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = tmd_dbg_autocredit_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** tmd_dbg_autocredit_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "tmd_dbg_autocredit_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_DBG_AUTOCREDIT_SET_DBG_AUTOCREDIT_SET_INFO_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_DBG_AUTOCREDIT_SET_DBG_AUTOCREDIT_SET_INFO_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.rate = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_DBG_AUTOCREDIT_SET_DBG_AUTOCREDIT_SET_INFO_LAST_QUEUE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_DBG_AUTOCREDIT_SET_DBG_AUTOCREDIT_SET_INFO_LAST_QUEUE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.last_queue = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_DBG_AUTOCREDIT_SET_DBG_AUTOCREDIT_SET_INFO_FIRST_QUEUE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_DBG_AUTOCREDIT_SET_DBG_AUTOCREDIT_SET_INFO_FIRST_QUEUE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.first_queue = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_dbg_autocredit_set(
          unit,
          &prm_info,
          &prm_exact_rate
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** tmd_dbg_autocredit_set - FAIL", TRUE);
    soc_petra_disp_result(ret, "tmd_dbg_autocredit_set");
    goto exit;
  }

  soc_sand_os_printf( "exact_rate: %lu[Mbps]\n\r",prm_exact_rate);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: dbg_autocredit_get (section debug)
 */
int
  ui_tmd_api_debug_dbg_autocredit_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  TMD_DBG_AUTOCREDIT_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_debug");
  soc_sand_proc_name = "tmd_dbg_autocredit_get";

  unit = tmd_get_default_unit();
  TMD_DBG_AUTOCREDIT_INFO_clear(&prm_info);

  /* Get parameters */

  /* Call function */
  ret = tmd_dbg_autocredit_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** tmd_dbg_autocredit_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "tmd_dbg_autocredit_get");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  TMD_DBG_AUTOCREDIT_INFO_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: dbg_egress_shaping_enable_set (section debug)
 */
int
  ui_tmd_api_debug_dbg_egress_shaping_enable_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint8
    prm_enable;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_debug");
  soc_sand_proc_name = "tmd_dbg_egress_shaping_enable_set";

  unit = tmd_get_default_unit();

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = tmd_dbg_egress_shaping_enable_get(
          unit,
          &prm_enable
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** tmd_dbg_egress_shaping_enable_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "tmd_dbg_egress_shaping_enable_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_DBG_EGRESS_SHAPING_ENABLE_SET_DBG_EGRESS_SHAPING_ENABLE_SET_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_DBG_EGRESS_SHAPING_ENABLE_SET_DBG_EGRESS_SHAPING_ENABLE_SET_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_enable = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_dbg_egress_shaping_enable_set(
          unit,
          prm_enable
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** tmd_dbg_egress_shaping_enable_set - FAIL", TRUE);
    soc_petra_disp_result(ret, "tmd_dbg_egress_shaping_enable_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: dbg_egress_shaping_enable_get (section debug)
 */
int
  ui_tmd_api_debug_dbg_egress_shaping_enable_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint8
    prm_enable;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_debug");
  soc_sand_proc_name = "tmd_dbg_egress_shaping_enable_get";

  unit = tmd_get_default_unit();

  /* Get parameters */

  /* Call function */
  ret = tmd_dbg_egress_shaping_enable_get(
          unit,
          &prm_enable
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** tmd_dbg_egress_shaping_enable_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "tmd_dbg_egress_shaping_enable_get");
    goto exit;
  }

  soc_sand_os_printf( "enable: %u\n\r",prm_enable);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: flow_control_enable_set (section debug)
 */
int
  ui_tmd_api_dbg_flow_control_enable_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint8
    prm_enable;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_debug");
  soc_sand_proc_name = "tmd_dbg_flow_control_enable_set";

  unit = tmd_get_default_unit();

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = tmd_dbg_flow_control_enable_get(
          unit,
          &prm_enable
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** tmd_dbg_flow_control_enable_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "tmd_dbg_flow_control_enable_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_DBG_FLOW_CONTROL_ENABLE_SET_FLOW_CONTROL_ENABLE_SET_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_DBG_FLOW_CONTROL_ENABLE_SET_FLOW_CONTROL_ENABLE_SET_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_enable = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = tmd_dbg_flow_control_enable_set(
          unit,
          prm_enable
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** tmd_dbg_flow_control_enable_set - FAIL", TRUE);
    soc_petra_disp_result(ret, "tmd_dbg_flow_control_enable_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: flow_control_enable_get (section debug)
 */
int
  ui_tmd_api_dbg_flow_control_enable_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint8
    prm_enable;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_debug");
  soc_sand_proc_name = "tmd_dbg_flow_control_enable_get";

  unit = tmd_get_default_unit();

  /* Get parameters */

  /* Call function */
  ret = tmd_dbg_flow_control_enable_get(
          unit,
          &prm_enable
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** tmd_dbg_flow_control_enable_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "tmd_dbg_flow_control_enable_get");
    goto exit;
  }

  soc_sand_os_printf( "enable: %u\n\r",prm_enable);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: dbg_queue_flush (section debug)
 */
int
  ui_tmd_api_debug_dbg_queue_flush(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_queue_ndx;
  TMD_DBG_FLUSH_MODE
    prm_mode;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_debug");
  soc_sand_proc_name = "tmd_dbg_queue_flush";

  unit = tmd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_DBG_QUEUE_FLUSH_DBG_QUEUE_FLUSH_QUEUE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_DBG_QUEUE_FLUSH_DBG_QUEUE_FLUSH_QUEUE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_queue_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter queue_ndx after dbg_queue_flush***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_DBG_QUEUE_FLUSH_DBG_QUEUE_FLUSH_MODE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_DBG_QUEUE_FLUSH_DBG_QUEUE_FLUSH_MODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mode = param_val->numeric_equivalent;
  }


  /* Call function */
  ret = tmd_dbg_queue_flush(
          unit,
          prm_queue_ndx,
          prm_mode
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** tmd_dbg_queue_flush - FAIL", TRUE);
    soc_petra_disp_result(ret, "tmd_dbg_queue_flush");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: dbg_queue_flush_all (section debug)
 */
int
  ui_tmd_api_debug_dbg_queue_flush_all(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  TMD_DBG_FLUSH_MODE
    prm_mode;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_debug");
  soc_sand_proc_name = "tmd_dbg_queue_flush_all";

  unit = tmd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_DBG_QUEUE_FLUSH_ALL_DBG_QUEUE_FLUSH_ALL_MODE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_DBG_QUEUE_FLUSH_ALL_DBG_QUEUE_FLUSH_ALL_MODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mode = param_val->numeric_equivalent;
  }


  /* Call function */
  ret = tmd_dbg_queue_flush_all(
          unit,
          prm_mode
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** tmd_dbg_queue_flush_all - FAIL", TRUE);
    soc_petra_disp_result(ret, "tmd_dbg_queue_flush_all");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: dbg_ingr_reset (section debug)
 */
int
  ui_tmd_api_debug_dbg_ingr_reset(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_debug");
  soc_sand_proc_name = "tmd_dbg_ingr_reset";

  unit = tmd_get_default_unit();

  /* Get parameters */

  /* Call function */
  ret = tmd_dbg_ingr_reset(
          unit
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** tmd_dbg_ingr_reset - FAIL", TRUE);
    soc_petra_disp_result(ret, "tmd_dbg_ingr_reset");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}


#endif

#ifdef UI_UTILS/* { ui_utils*/
/********************************************************************
 *  Section handler: ui_utils
 */
int
  ui_tmd_api_utils(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_tmd_api_utils");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_UTILS_SET_DEFAULT_DEVICE_ID,1))
  {
    ui_ret = ui_tmd_api_set_default_unit(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_UTILS_GET_DEFAULT_DEVICE_ID,1))
  {
    ui_ret = ui_tmd_api_get_default_unit(current_line);
  }
#if UI_UNDEFED_TEMP
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_UTILS_TMD_REVISION_GET,1))
  {
    ui_ret = ui_tmd_api_tmd_revision_get(current_line);
  }
#endif /* UI_UNDEFED_TEMP */

  goto exit;
exit:
  return ui_ret;
}

#endif/* { */ /* } mgmt*/

#ifdef UI_PACKET
/********************************************************************
 *  Function handler: send (section packet)
 */
int
  ui_tmd_api_packet_send(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    ind;
  TMD_PKT_TX_PACKET_INFO
    *prm_packet;
  TMD_PORTS_ITMH
    itmh;
  uint32
    tmp;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_packet");
  soc_sand_proc_name = "tmd_packet_send";

  unit = tmd_get_default_unit();


  prm_packet = (TMD_PKT_TX_PACKET_INFO*)sal_alloc(sizeof(TMD_PKT_TX_PACKET_INFO));
  if (prm_packet == NULL)
  {
    ui_ret = 1;
    goto exit;
  }
  TMD_PKT_TX_PACKET_INFO_clear(prm_packet);
  prm_packet->path_type = TMD_PACKET_SEND_PATH_TYPE_INGRESS;
  TMD_PORTS_ITMH_clear(&itmh);


  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PACKET_SEND_SEND_PACKET_UNICAST_INDICATOR_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PACKET_SEND_SEND_PACKET_UNICAST_INDICATOR_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    tmp = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PACKET_SEND_SEND_PACKET_DESTINATION_FAP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PACKET_SEND_SEND_PACKET_DESTINATION_FAP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    tmp = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PACKET_SEND_SEND_PACKET_PATH_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_TMD_PACKET_SEND_SEND_PACKET_PATH_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_packet->path_type = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PACKET_SEND_SEND_PACKET_PAYLOAD_BYTE_SIZE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PACKET_SEND_SEND_PACKET_PAYLOAD_BYTE_SIZE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_packet->packet.data_byte_size = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter payload_byte_size after send***", TRUE);
    goto exit;
  }

  if (prm_packet->packet.data_byte_size < 32 )
  {
    prm_packet->packet.data_byte_size = 32;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PACKET_SEND_SEND_PACKET_PAYLOAD_0_ID,1))
  {
    for (ind = 0; ind < TMD_PKT_MAX_CPU_PACKET_BYTE_SIZE; ++ind, ui_ret = 0)
    {
        UI_MACROS_GET_NUMMERIC_VAL_WITH_INDEX(PARAM_TMD_PACKET_SEND_SEND_PACKET_PAYLOAD_0_ID, ind + 1);
        prm_packet->packet.data[TMD_PKT_MAX_CPU_PACKET_BYTE_SIZE - 1 -  ind] = ui_ret ? 0 : (uint8)param_val->value.ulong_value;
    }
  }

  /* Call function */
  ret = tmd_pkt_packet_send(
          unit,
          prm_packet
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_packet_send");
    goto exit;
  }

exit:

  sal_free(prm_packet);
  return ui_ret;
}

/********************************************************************
 *  Function handler: recv (section packet)
 */
int
  ui_tmd_api_packet_recv(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    data_byte_size=0;
  TMD_PKT_RX_PACKET_INFO
    *prm_packet = NULL;

  UI_MACROS_INIT_FUNCTION("ui_tmd_api_packet");
  soc_sand_proc_name = "tmd_packet_recv";

  unit = tmd_get_default_unit();

  prm_packet = (TMD_PKT_RX_PACKET_INFO*)sal_alloc(sizeof(TMD_PKT_RX_PACKET_INFO));
  if (prm_packet == NULL)
  {
    ui_ret = 1;
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PACKET_RECV_RECV_SIZE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_TMD_PACKET_RECV_RECV_SIZE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data_byte_size = (uint32)param_val->value.ulong_value;
  }

  TMD_PKT_RX_PACKET_INFO_clear(prm_packet);

  /* Get parameters */

  /* Call function */
  ret = tmd_pkt_packet_recv(
          unit,
          data_byte_size,
          prm_packet
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "tmd_packet_recv");
    goto exit;
  }

  send_string_to_screen("--> packet:", TRUE);
  TMD_PKT_RX_PACKET_INFO_print(prm_packet);

exit:
  sal_free(prm_packet);
  return ui_ret;
}
/********************************************************************
 *  Function handler: recv (section packet)
 */
int
  ui_tmd_api_packet_proc_create(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;


  UI_MACROS_INIT_FUNCTION("ui_tmd_api_packet_proc_create");
  soc_sand_proc_name = "tmd_api_packet_proc_create";

  unit = tmd_get_default_unit();



  /* Call function */
#ifndef __DUNE_SSF__
#endif
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "ui_tmd_api_packet_proc_create");
    goto exit;
  }

  send_string_to_screen("--> task created", TRUE);

exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: recv (section packet)
 */
int
  ui_tmd_api_packet_proc_delete(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;


  UI_MACROS_INIT_FUNCTION("ui_tmd_api_packet_proc_delete");
  soc_sand_proc_name = "tmd_api_packet_proc_delete";

  unit = tmd_get_default_unit();



  /* Call function */
#ifndef __DUNE_SSF__
  ret = swp_p_tm_app_cpu_queue_task_delete(unit);
#endif
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    soc_petra_disp_result(ret, "ui_tmd_api_packet_proc_delete");
    goto exit;
  }
  send_string_to_screen("--> task deleted", TRUE);

exit:
  return ui_ret;
}

#endif

#ifdef UI_CELL/* { cell */
/********************************************************************
 *  Section handler: cell
 */
int
  ui_tmd_api_cell(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_tmd_api_cell");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_CELL_READ_DIRECT_ID,1))
  {
    ui_ret = ui_tmd_api_cell_tmd_read_from_fe600(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_CELL_WRITE_DIRECT_ID,1))
  {
    ui_ret = ui_tmd_api_cell_tmd_write_to_fe600(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_CELL_READ_INDIRECT_ID,1))
  {
    ui_ret = ui_tmd_api_cell_tmd_indirect_read_from_fe600(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_CELL_WRITE_INDIRECT_ID,1))
  {
    ui_ret = ui_tmd_api_cell_tmd_indirect_write_to_fe600(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_CELL_CPU2CPU_ID,1))
  {
    ui_ret = ui_tmd_api_cell_tmd_cpu2cpu_write(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_CELL_CPU2CPU_READ_ID,1))
  {
    ui_ret = ui_tmd_api_cell_tmd_cpu2cpu_read(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_CELL_MC_TBL_WRITE_MC_TBL_WRITE_ID,1)) 
  { 
    ui_ret = ui_tmd_api_cell_mc_tbl_write(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_CELL_MC_TBL_READ_MC_TBL_READ_ID,1)) 
  { 
    ui_ret = ui_tmd_api_cell_mc_tbl_read(current_line); 
  } 
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after cell***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif/* { cell*/

#ifdef UI_PACKET/* { packet*/
/********************************************************************
 *  Section handler: packet
 */
int
  ui_tmd_api_packet(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_tmd_api_packet");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PACKET_SEND_SEND_ID,1))
  {
    ui_ret = ui_tmd_api_packet_send(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PACKET_RECV_RECV_ID,1))
  {
    ui_ret = ui_tmd_api_packet_recv(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PACKET_RECV_PROC_CREATE_ID,1))
  {
    ui_ret = ui_tmd_api_packet_proc_create(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PACKET_RECV_PROC_DELETE_ID,1))
  {
    ui_ret = ui_tmd_api_packet_proc_delete(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after packet***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif/* { packet*/

#ifdef UI_MGMT/* { mgmt*/
/********************************************************************
 *  Section handler: mgmt
 */
int
  ui_tmd_api_mgmt(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_tmd_api_mgmt");
#ifdef UI_UNDEFED
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_REGISTER_DEVICE_REGISTER_DEVICE_ID,1))
  {
    ui_ret = ui_tmd_api_mgmt_register_device(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_UNREGISTER_DEVICE_UNREGISTER_DEVICE_ID,1))
  {
    ui_ret = ui_tmd_api_mgmt_unregister_device(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MGMT_OPERATION_MODE_SET_OPERATION_MODE_SET_ID,1))
  {
    ui_ret = ui_tmd_api_mgmt_operation_mode_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MGMT_OPERATION_MODE_GET_OPERATION_MODE_GET_ID,1))
  {
    ui_ret = ui_tmd_api_mgmt_operation_mode_get(current_line);
  }
#endif
  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MGMT_CREDIT_WORTH_SET_CREDIT_WORTH_SET_ID,1))
  {
    ui_ret = ui_tmd_api_mgmt_credit_worth_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MGMT_CREDIT_WORTH_GET_CREDIT_WORTH_GET_ID,1))
  {
    ui_ret = ui_tmd_api_mgmt_credit_worth_get(current_line);
  }
#ifdef UI_UNDEFED
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_ID,1))
  {
    ui_ret = ui_tmd_api_mgmt_init_sequence_phase1(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MGMT_INIT_SEQUENCE_PHASE2_INIT_SEQUENCE_PHASE2_ID,1))
  {
    ui_ret = ui_tmd_api_mgmt_init_sequence_phase2(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MGMT_SYSTEM_FAP_ID_SET_SYSTEM_FAP_ID_SET_ID,1))
  {
    ui_ret = ui_tmd_api_mgmt_system_fap_id_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MGMT_SYSTEM_FAP_ID_GET_SYSTEM_FAP_ID_GET_ID,1))
  {
    ui_ret = ui_tmd_api_mgmt_system_fap_id_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MGMT_HW_INTERFACES_SET_HW_INTERFACES_SET_ID,1))
  {
    ui_ret = ui_tmd_api_mgmt_hw_interfaces_set(current_line);
  }
  /*
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MGMT_HW_INTERFACES_GET_HW_INTERFACES_GET_ID,1))
    {
      ui_ret = ui_tmd_api_mgmt_hw_interfaces_get(current_line);
    }
  */
#endif
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MGMT_ALL_CTRL_CELLS_ENABLE_SET_ALL_CTRL_CELLS_ENABLE_SET_ID,1))
  {
    ui_ret = ui_tmd_api_mgmt_all_ctrl_cells_enable_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MGMT_ALL_CTRL_CELLS_ENABLE_GET_ALL_CTRL_CELLS_ENABLE_GET_ID,1))
    {
      ui_ret = ui_tmd_api_mgmt_all_ctrl_cells_enable_get(current_line);
    }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MGMT_ENABLE_TRAFFIC_SET_ENABLE_TRAFFIC_SET_ID,1))
  {
    ui_ret = ui_tmd_api_mgmt_enable_traffic_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MGMT_ENABLE_TRAFFIC_GET_ENABLE_TRAFFIC_GET_ID,1))
  {
    ui_ret = ui_tmd_api_mgmt_enable_traffic_get(current_line);
  }
  #ifdef UI_UNDEFED
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MGMT_PCKT_SIZE_RANGE_SET_PCKT_SIZE_RANGE_SET_ID,1))
  {
    ui_ret = ui_tmd_api_mgmt_pckt_size_range_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MGMT_PCKT_SIZE_RANGE_GET_PCKT_SIZE_RANGE_GET_ID,1))
  {
    ui_ret = ui_tmd_api_mgmt_pckt_size_range_get(current_line);
  }
  #endif /* UI_UNDEFED */
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MGMT_CORE_FREQUENCY_GET_CORE_FREQUENCY_GET_ID,1)) 
  { 
    ui_ret = ui_tmd_api_mgmt_core_frequency_get(current_line); 
  } 
  /*else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MGMT_HW_WAIT_FOR_INIT_HW_WAIT_FOR_INIT_ID,1))
  {
    ui_ret = ui_tmd_api_mgmt_hw_wait_for_init(current_line);
  }  
                                                                                                */
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after mgmt***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif/* { */ /* } mgmt*/


#ifdef UI_PORTS/* { ports*/
/********************************************************************
 *  Section handler: ports
 */
int
  ui_tmd_api_ports(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ports");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SYS_PHYS_TO_LOCAL_PORT_MAP_SET_SYS_PHYS_TO_LOCAL_PORT_MAP_SET_ID,1))
  {
    ui_ret = ui_tmd_api_ports_sys_phys_to_local_port_map_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SYS_PHYS_TO_LOCAL_PORT_MAP_GET_SYS_PHYS_TO_LOCAL_PORT_MAP_GET_ID,1))
  {
    ui_ret = ui_tmd_api_ports_sys_phys_to_local_port_map_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_LOCAL_TO_SYS_PHYS_PORT_MAP_GET_LOCAL_TO_SYS_PHYS_PORT_MAP_GET_ID,1))
  {
    ui_ret = ui_tmd_api_ports_local_to_sys_phys_port_map_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORT_TO_INTERFACE_MAP_SET_PORT_TO_INTERFACE_MAP_SET_ID,1))
  {
    ui_ret = ui_tmd_api_ports_port_to_interface_map_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORT_TO_INTERFACE_MAP_GET_PORT_TO_INTERFACE_MAP_GET_ID,1))
  {
    ui_ret = ui_tmd_api_ports_port_to_interface_map_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_LAG_SET_LAG_SET_ID,1))
  {
    ui_ret = ui_tmd_api_ports_lag_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_LAG_ADD_LAG_ADD_ID,1))
  {
    ui_ret = ui_tmd_api_ports_lag_add(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_LAG_MEMBER_ADD_LAG_MEMBER_ADD_ID,1))
  {
    ui_ret = ui_tmd_api_ports_lag_member_add(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_LAG_REMOVE_LAG_REMOVE_ID,1))
  {
    ui_ret = ui_tmd_api_ports_lag_remove(current_line);
  }
#ifdef UI_UNDEFED_TEMP  
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_LAG_SYS_PORT_INFO_GET_LAG_SYS_PORT_INFO_GET_ID,1))
  {
    ui_ret = ui_tmd_api_ports_lag_sys_port_info_get(current_line);
  }
#endif /* UI_UNDEFED_TEMP */
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_LAG_SET_LAG_SET_ID,1))
  {
    ui_ret = ui_tmd_api_ports_lag_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_LAG_GET_LAG_GET_ID,1))
  {
    ui_ret = ui_tmd_api_ports_lag_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORT_HEADER_TYPE_SET_PORT_HEADER_TYPE_SET_ID,1))
  {
    ui_ret = ui_tmd_api_ports_port_header_type_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORT_HEADER_TYPE_GET_PORT_HEADER_TYPE_GET_ID,1))
  {
    ui_ret = ui_tmd_api_ports_port_header_type_get(current_line);
  }
#ifdef UI_UNDEFED_TEMP
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_MIRROR_INBOUND_SET_MIRROR_INBOUND_SET_ID,1))
  {
    ui_ret = ui_tmd_api_ports_mirror_inbound_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_MIRROR_INBOUND_GET_MIRROR_INBOUND_GET_ID,1))
  {
    ui_ret = ui_tmd_api_ports_mirror_inbound_get(current_line);
  }
#endif /* UI_UNDEFED_TEMP */
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_MIRROR_OUTBOUND_SET_MIRROR_OUTBOUND_SET_ID,1))
  {
    ui_ret = ui_tmd_api_ports_mirror_outbound_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_MIRROR_OUTBOUND_GET_MIRROR_OUTBOUND_GET_ID,1))
  {
    ui_ret = ui_tmd_api_ports_mirror_outbound_get(current_line);
  }
#ifdef UI_UNDEFED_TEMP
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_SNOOP_SET_SNOOP_SET_ID,1))
  {
    ui_ret = ui_tmd_api_ports_snoop_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_SNOOP_GET_SNOOP_GET_ID,1))
  {
    ui_ret = ui_tmd_api_ports_snoop_get(current_line);
  }
#endif /* UI_UNDEFED_TEMP */
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_ITMH_EXTENSION_SET_ITMH_EXTENSION_SET_ID,1))
  {
    ui_ret = ui_tmd_api_ports_itmh_extension_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_ITMH_EXTENSION_GET_ITMH_EXTENSION_GET_ID,1))
  {
    ui_ret = ui_tmd_api_ports_itmh_extension_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_SHAPING_HEADER_SET_SHAPING_HEADER_SET_ID,1))
  {
    ui_ret = ui_tmd_api_ports_shaping_header_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_SHAPING_HEADER_GET_SHAPING_HEADER_GET_ID,1))
  {
    ui_ret = ui_tmd_api_ports_shaping_header_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_FORWARDING_HEADER_SET_FORWARDING_HEADER_SET_ID,1))
  {
    ui_ret = ui_tmd_api_ports_forwarding_header_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_FORWARDING_HEADER_GET_FORWARDING_HEADER_GET_ID,1))
  {
    ui_ret = ui_tmd_api_ports_forwarding_header_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_STAG_FIELDS_SET_STAT_TAG_FIELD_SET_ID,1))
  {
    ui_ret = ui_tmd_api_ports_stat_tag_field_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_STAG_FIELDS_GET_STAT_TAG_FIELD_GET_ID,1))
  {
    ui_ret = ui_tmd_api_ports_stat_tag_field_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_FTMH_EXTENSION_SET_FTMH_EXTENSION_SET_ID,1))
  {
    ui_ret = ui_tmd_api_ports_ftmh_extension_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_FTMH_EXTENSION_GET_FTMH_EXTENSION_GET_ID,1))
  {
    ui_ret = ui_tmd_api_ports_ftmh_extension_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_OTMH_EXTENSION_SET_OTMH_EXTENSION_SET_ID,1))
  {
    ui_ret = ui_tmd_api_ports_otmh_extension_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORTS_OTMH_EXTENSION_GET_OTMH_EXTENSION_GET_ID,1))
  {
    ui_ret = ui_tmd_api_ports_otmh_extension_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORT_EGR_HDR_CREDIT_DISCOUNT_TYPE_SET_PORT_EGR_HDR_CREDIT_DISCOUNT_TYPE_SET_ID,1))
  {
    ui_ret = ui_tmd_api_ports_port_egr_hdr_credit_discount_type_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORT_EGR_HDR_CREDIT_DISCOUNT_TYPE_GET_PORT_EGR_HDR_CREDIT_DISCOUNT_TYPE_GET_ID,1))
  {
    ui_ret = ui_tmd_api_ports_port_egr_hdr_credit_discount_type_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORT_EGR_HDR_CREDIT_DISCOUNT_SELECT_SET_PORT_EGR_HDR_CREDIT_DISCOUNT_SELECT_SET_ID,1))
  {
    ui_ret = ui_tmd_api_ports_port_egr_hdr_credit_discount_select_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_PORT_EGR_HDR_CREDIT_DISCOUNT_SELECT_GET_PORT_EGR_HDR_CREDIT_DISCOUNT_SELECT_GET_ID,1))
  {
    ui_ret = ui_tmd_api_ports_port_egr_hdr_credit_discount_select_get(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after ports***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif/* { */ /*} ports*/

#ifdef UI_INGRESS_PACKET_QUEUING/* { ingress_packet_queuing*/
/********************************************************************
 *  Section handler: ingress_packet_queuing
 */
int
  ui_tmd_api_ingress_packet_queuing(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_packet_queuing");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_IPQ_EXPLICIT_MAPPING_MODE_INFO_SET_IPQ_EXPLICIT_MAPPING_MODE_INFO_SET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_packet_queuing_ipq_explicit_mapping_mode_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_IPQ_EXPLICIT_MAPPING_MODE_INFO_GET_IPQ_EXPLICIT_MAPPING_MODE_INFO_GET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_packet_queuing_ipq_explicit_mapping_mode_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_IPQ_TRAFFIC_CLASS_MAP_SET_IPQ_TRAFFIC_CLASS_MAP_SET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_packet_queuing_ipq_traffic_class_map_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_IPQ_TRAFFIC_CLASS_MAP_GET_IPQ_TRAFFIC_CLASS_MAP_GET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_packet_queuing_ipq_traffic_class_map_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_IPQ_DESTINATION_ID_PACKETS_BASE_QUEUE_ID_SET_IPQ_DESTINATION_ID_PACKETS_BASE_QUEUE_ID_SET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_packet_queuing_ipq_destination_id_packets_base_queue_id_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_IPQ_DESTINATION_ID_PACKETS_BASE_QUEUE_ID_GET_IPQ_DESTINATION_ID_PACKETS_BASE_QUEUE_ID_GET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_packet_queuing_ipq_destination_id_packets_base_queue_id_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_IPQ_QUEUE_INTERDIGITATED_MODE_SET_IPQ_QUEUE_INTERDIGITATED_MODE_SET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_packet_queuing_ipq_queue_intedigitated_mode_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_IPQ_QUEUE_INTERDIGITATED_MODE_GET_IPQ_QUEUE_INTERDIGITATED_MODE_GET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_packet_queuing_ipq_queue_interdigitated_mode_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_IPQ_QUEUE_TO_FLOW_MAPPING_SET_IPQ_QUEUE_TO_FLOW_MAPPING_SET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_packet_queuing_ipq_queue_to_flow_mapping_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_IPQ_QUEUE_TO_FLOW_MAPPING_GET_IPQ_QUEUE_TO_FLOW_MAPPING_GET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_packet_queuing_ipq_queue_to_flow_mapping_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_IPQ_QUARTET_RESET_IPQ_QUARTET_RESET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_packet_queuing_ipq_quartet_reset(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_IPQ_QUEUE_QRTT_UNMAP_IPQ_QUEUE_QRTT_UNMAP_ID,1)) 
  { 
    ui_ret = ui_tmd_api_ingress_packet_queuing_ipq_queue_qrtt_unmap(current_line); 
  } 
#ifdef UI_UNDEFED_TEMP
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SEARCH_ALL_NON_EMPTY_QUEUES_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_packet_queueing_ips_non_empty_queues_get(current_line);
  }
#endif /* UI_UNDEFED_TEMP */
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after ingress_packet_queuing***", TRUE);
  }
  goto exit;
exit:
  return ui_ret;
}

#endif/* { */ /*} ingress_packet_queuing*/

#ifdef UI_INGRESS_TRAFFIC_MGMT/* { ingress_traffic_mgmt*/
/********************************************************************
 *  Section handler: ingress_traffic_mgmt
 */
int
  ui_tmd_api_ingress_traffic_mgmt(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_traffic_mgmt");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_DRAM_BUFFS_GET_ITM_DRAM_BUFFS_GET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_traffic_mgmt_itm_dram_buffs_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_GLOB_RCS_FC_SET_ITM_GLOB_RCS_FC_SET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_traffic_mgmt_itm_glob_rcs_fc_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_GLOB_RCS_FC_GET_ITM_GLOB_RCS_FC_GET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_traffic_mgmt_itm_glob_rcs_fc_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_GLOB_RCS_DROP_SET_ITM_GLOB_RCS_DROP_SET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_traffic_mgmt_itm_glob_rcs_drop_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_GLOB_RCS_DROP_GET_ITM_GLOB_RCS_DROP_GET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_traffic_mgmt_itm_glob_rcs_drop_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_CATEGORY_RNGS_SET_ITM_CATEGORY_RNGS_SET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_traffic_mgmt_itm_vsq_category_rngs_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_CATEGORY_RNGS_GET_ITM_VSQ_CATEGORY_RNGS_GET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_traffic_mgmt_itm_vsq_category_rngs_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_ADMIT_TEST_TMPLT_SET_VSQ_ITM_ADMIT_TEST_TMPLT_SET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_traffic_mgmt_itm_admit_test_tmplt_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_ADMIT_TEST_TMPLT_GET_ITM_ADMIT_TEST_TMPLT_GET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_traffic_mgmt_itm_admit_test_tmplt_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_CR_REQUEST_SET_ITM_CR_REQUEST_SET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_traffic_mgmt_itm_cr_request_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_CR_REQUEST_GET_ITM_CR_REQUEST_GET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_traffic_mgmt_itm_cr_request_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_CR_DISCOUNT_SET_ITM_CR_DISCOUNT_SET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_traffic_mgmt_itm_cr_discount_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_CR_DISCOUNT_GET_ITM_CR_DISCOUNT_GET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_traffic_mgmt_itm_cr_discount_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_QUEUE_TEST_TMPLT_SET_ITM_QUEUE_TEST_TMPLT_SET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_traffic_mgmt_itm_queue_test_tmplt_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_QUEUE_TEST_TMPLT_GET_ITM_QUEUE_TEST_TMPLT_GET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_traffic_mgmt_itm_queue_test_tmplt_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_WRED_EXP_WQ_SET_ITM_WRED_EXP_WQ_SET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_traffic_mgmt_itm_wred_exp_wq_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_WRED_EXP_WQ_GET_ITM_WRED_EXP_WQ_GET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_traffic_mgmt_itm_wred_exp_wq_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_WRED_SET_ITM_WRED_SET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_traffic_mgmt_itm_wred_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_WRED_GET_ITM_WRED_GET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_traffic_mgmt_itm_wred_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_TAIL_DROP_SET_ITM_TAIL_DROP_SET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_traffic_mgmt_itm_tail_drop_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_TAIL_DROP_GET_ITM_TAIL_DROP_GET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_traffic_mgmt_itm_tail_drop_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_CR_WD_SET_ITM_CR_WD_SET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_traffic_mgmt_itm_cr_wd_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_CR_WD_GET_ITM_CR_WD_GET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_traffic_mgmt_itm_cr_wd_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_VSQ_QT_RT_CLS_SET_ITM_VSQ_QT_RT_CLS_SET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_traffic_mgmt_itm_vsq_qt_rt_cls_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_VSQ_QT_RT_CLS_GET_ITM_VSQ_QT_RT_CLS_GET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_traffic_mgmt_itm_vsq_qt_rt_cls_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_VSQ_FC_SET_ITM_VSQ_FC_SET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_traffic_mgmt_itm_vsq_fc_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_VSQ_FC_GET_ITM_VSQ_FC_GET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_traffic_mgmt_itm_vsq_fc_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_VSQ_TAIL_DROP_SET_ITM_VSQ_TAIL_DROP_SET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_traffic_mgmt_itm_vsq_tail_drop_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_VSQ_TAIL_DROP_GET_ITM_VSQ_TAIL_DROP_GET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_traffic_mgmt_itm_vsq_tail_drop_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_VSQ_WRED_GEN_SET_ITM_VSQ_WRED_GEN_SET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_traffic_mgmt_itm_vsq_wred_gen_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_VSQ_WRED_GEN_GET_ITM_VSQ_WRED_GEN_GET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_traffic_mgmt_itm_vsq_wred_gen_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_VSQ_WRED_SET_ITM_VSQ_WRED_SET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_traffic_mgmt_itm_vsq_wred_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_VSQ_WRED_GET_ITM_VSQ_WRED_GET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_traffic_mgmt_itm_vsq_wred_get(current_line);
  }
#ifdef UI_UNDEFED_TEMP
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_STAG_SET_ITM_STAG_SET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_traffic_mgmt_itm_stag_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_STAG_GET_ITM_STAG_GET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_traffic_mgmt_itm_stag_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_VSQ_COUNTER_SET_ITM_VSQ_COUNTER_SET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_traffic_mgmt_itm_vsq_counter_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_VSQ_COUNTER_GET_ITM_VSQ_COUNTER_GET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_traffic_mgmt_itm_vsq_counter_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_VSQ_COUNTER_READ_ITM_VSQ_COUNTER_READ_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_traffic_mgmt_itm_vsq_counter_read(current_line);
  }
#endif /* UI_UNDEFED_TEMP */
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_QUEUE_INFO_SET_ITM_QUEUE_INFO_SET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_traffic_mgmt_itm_queue_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_QUEUE_INFO_GET_ITM_QUEUE_INFO_GET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_traffic_mgmt_itm_queue_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_INGRESS_SHAPE_SET_ITM_INGRESS_SHAPE_SET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_traffic_mgmt_itm_ingress_shape_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_INGRESS_SHAPE_GET_ITM_INGRESS_SHAPE_GET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_traffic_mgmt_itm_ingress_shape_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_PRIORITY_MAP_TMPLT_SET_ITM_PRIORITY_MAP_TMPLT_SET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_traffic_mgmt_itm_priority_map_tmplt_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_PRIORITY_MAP_TMPLT_GET_ITM_PRIORITY_MAP_TMPLT_GET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_traffic_mgmt_itm_priority_map_tmplt_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_PRIORITY_MAP_TMPLT_SELECT_SET_ITM_PRIORITY_MAP_TMPLT_SELECT_SET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_traffic_mgmt_itm_priority_map_tmplt_select_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_PRIORITY_MAP_TMPLT_SELECT_GET_ITM_PRIORITY_MAP_TMPLT_SELECT_GET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_traffic_mgmt_itm_priority_map_tmplt_select_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_SYS_RED_DROP_PROB_SET_ITM_SYS_RED_DROP_PROB_SET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_traffic_mgmt_itm_sys_red_drop_prob_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_SYS_RED_DROP_PROB_GET_ITM_SYS_RED_DROP_PROB_GET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_traffic_mgmt_itm_sys_red_drop_prob_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_SYS_RED_QUEUE_SIZE_BOUNDARIES_SET_ITM_SYS_RED_QUEUE_SIZE_BOUNDARIES_SET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_traffic_mgmt_itm_sys_red_queue_size_boundaries_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_SYS_RED_QUEUE_SIZE_BOUNDARIES_GET_ITM_SYS_RED_QUEUE_SIZE_BOUNDARIES_GET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_traffic_mgmt_itm_sys_red_queue_size_boundaries_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_SYS_RED_Q_BASED_SET_ITM_SYS_RED_INFO_SET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_traffic_mgmt_itm_sys_red_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_SYS_RED_Q_BASED_GET_ITM_SYS_RED_INFO_GET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_traffic_mgmt_itm_sys_red_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_SYS_RED_EG_SET_ITM_SYS_RED_EG_SET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_traffic_mgmt_itm_sys_red_eg_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_SYS_RED_EG_GET_ITM_SYS_RED_EG_GET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_traffic_mgmt_itm_sys_red_eg_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_SYS_RED_GLOB_RCS_SET_ITM_SYS_RED_GLOB_RCS_SET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_traffic_mgmt_itm_sys_red_glob_rcs_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ITM_SYS_RED_GLOB_RCS_GET_ITM_SYS_RED_GLOB_RCS_GET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_traffic_mgmt_itm_sys_red_glob_rcs_get(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after ingress_traffic_mgmt***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif/* { */ /*} ingress_traffic_mgmt*/

#ifdef UI_INGRESS_SCHEDULER/* { ingress_scheduler*/
/********************************************************************
 *  Section handler: ingress_scheduler
 */
int
  ui_tmd_api_ingress_scheduler(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ingress_scheduler");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_INGRESS_SCHEDULER_MESH_SET_MESH_SET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_scheduler_mesh_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_INGRESS_SCHEDULER_MESH_GET_MESH_GET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_scheduler_mesh_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_INGRESS_SCHEDULER_CLOS_SET_CLOS_SET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_scheduler_clos_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_INGRESS_SCHEDULER_CLOS_GET_CLOS_GET_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_scheduler_clos_get(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after ingress_scheduler***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif/* { */ /*} ingress_scheduler*/

#ifdef UI_EGR_QUEUING/* { egr_queuing*/
/********************************************************************
 *  Section handler: egr_queuing
 */
int
  ui_tmd_api_egr_queuing(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_tmd_api_egr_queuing");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_OFP_THRESH_TYPE_SET_EGR_OFP_THRESH_TYPE_SET_ID,1))
  {
    ui_ret = ui_tmd_api_egr_queuing_egr_ofp_thresh_type_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_OFP_THRESH_TYPE_GET_EGR_OFP_THRESH_TYPE_GET_ID,1))
  {
    ui_ret = ui_tmd_api_egr_queuing_egr_ofp_thresh_type_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_SCHED_DROP_SET_EGR_SCHED_DROP_SET_ID,1))
  {
    ui_ret = ui_tmd_api_egr_queuing_egr_sched_drop_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_SCHED_DROP_GET_EGR_SCHED_DROP_GET_ID,1))
  {
    ui_ret = ui_tmd_api_egr_queuing_egr_sched_drop_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_UNSCHED_DROP_SET_EGR_UNSCHED_DROP_SET_ID,1))
  {
    ui_ret = ui_tmd_api_egr_queuing_egr_unsched_drop_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_UNSCHED_DROP_GET_EGR_UNSCHED_DROP_GET_ID,1))
  {
    ui_ret = ui_tmd_api_egr_queuing_egr_unsched_drop_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_DEV_FC_SET_EGR_DEV_FC_SET_ID,1))
  {
    ui_ret = ui_tmd_api_egr_queuing_egr_dev_fc_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_DEV_FC_GET_EGR_DEV_FC_GET_ID,1))
  {
    ui_ret = ui_tmd_api_egr_queuing_egr_dev_fc_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_CHNIF_FC_SET_EGR_CHNIF_FC_SET_ID,1))
  {
    ui_ret = ui_tmd_api_egr_queuing_egr_xaui_spaui_fc_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_CHNIF_FC_GET_EGR_CHNIF_FC_GET_ID,1))
  {
    ui_ret = ui_tmd_api_egr_queuing_egr_xaui_spaui_fc_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_OFP_FC_SET_EGR_OFP_FC_SET_ID,1))
  {
    ui_ret = ui_tmd_api_egr_queuing_egr_ofp_fc_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_OFP_FC_GET_EGR_OFP_FC_GET_ID,1))
  {
    ui_ret = ui_tmd_api_egr_queuing_egr_ofp_fc_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_MCI_FC_SET_EGR_MCI_FC_SET_ID,1))
  {
    ui_ret = ui_tmd_api_egr_queuing_egr_mci_fc_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_MCI_FC_GET_EGR_MCI_FC_GET_ID,1))
  {
    ui_ret = ui_tmd_api_egr_queuing_egr_mci_fc_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_MCI_FC_ENABLE_SET_EGR_MCI_FC_ENABLE_SET_ID,1))
  {
    ui_ret = ui_tmd_api_egr_queuing_egr_mci_fc_enable_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_MCI_FC_ENABLE_GET_EGR_MCI_FC_ENABLE_GET_ID,1))
  {
    ui_ret = ui_tmd_api_egr_queuing_egr_mci_fc_enable_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_OFP_SCH_MODE_SET_EGR_OFP_SCH_MODE_SET_ID,1))
  {
    ui_ret = ui_tmd_api_egr_queuing_egr_ofp_sch_mode_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_OFP_SCH_MODE_GET_EGR_OFP_SCH_MODE_GET_ID,1))
  {
    ui_ret = ui_tmd_api_egr_queuing_egr_ofp_sch_mode_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_OFP_SCHEDULING_SET_EGR_OFP_SCHEDULING_SET_ID,1))
  {
    ui_ret = ui_tmd_api_egr_queuing_egr_ofp_scheduling_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_OFP_SCHEDULING_GET_EGR_OFP_SCHEDULING_GET_ID,1))
  {
    ui_ret = ui_tmd_api_egr_queuing_egr_ofp_scheduling_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_Q_PRIO_SET_EGR_Q_PRIO_SET_ID,1))
  {
    ui_ret = ui_tmd_api_egr_queuing_egr_q_prio_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGR_Q_PRIO_GET_EGR_Q_PRIO_GET_ID,1))
  {
    ui_ret = ui_tmd_api_egr_queuing_egr_q_prio_get(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after egr_queuing***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif/* { *//* } egr_queuing*/

#ifdef UI_OFP_RATES/* { ofp_rates*/
/********************************************************************
 *  Section handler: ofp_rates
 */
int
  ui_tmd_api_ofp_rates(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_tmd_api_ofp_rates");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_OFP_RATES_SET_SET_ID,1))
  {
    ui_ret = ui_tmd_api_ofp_rates_set(current_line);
  }
#ifdef UI_UNDEFED_TEMP
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_OFP_ALL_RATES_GET_GET_ID,1))
  {
    ui_ret = ui_tmd_api_ofp_all_rates_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_OFP_RATES_VALIDATE_ID,1))
  {
    ui_ret = ui_tmd_api_ofp_egq_calendar_validate(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_OFP_RATES_TEST_ID,1))
  {
    ui_ret = ui_tmd_api_ofp_test(current_line);
  }
#endif /* UI_UNDEFED_TEMP */
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_OFP_RATES_GET_GET_ID,1))
  {
    ui_ret = ui_tmd_api_ofp_rates_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_OFP_RATES_UPDATE_UPDATE_ID,1))
  {
    ui_ret = ui_tmd_api_ofp_rates_update(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_OFP_RATES_SINGLE_PORT_GET_SINGLE_PORT_GET_ID,1))
  {
    ui_ret = ui_tmd_api_ofp_rates_single_port_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_OFP_RATES_MAL_SHAPER_SET_MAL_SHAPER_SET_ID,1))
  {
    ui_ret = ui_tmd_api_ofp_rates_mal_shaper_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_OFP_RATES_MAL_SHAPER_GET_MAL_SHAPER_GET_ID,1))
  {
    ui_ret = ui_tmd_api_ofp_rates_mal_shaper_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_OFP_FAT_PIPE_RATE_SET_OFP_FAT_PIPE_RATE_SET_ID,1))
  {
    ui_ret = ui_tmd_api_ofp_rates_ofp_fat_pipe_rate_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_OFP_FAT_PIPE_RATE_GET_OFP_FAT_PIPE_RATE_GET_ID,1))
  {
    ui_ret = ui_tmd_api_ofp_rates_ofp_fat_pipe_rate_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_OFP_RATES_UPDATE_DEVICE_SET_UPDATE_DEVICE_SET_ID,1))
  {
    ui_ret = ui_tmd_api_ofp_rates_update_device_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_OFP_RATES_UPDATE_DEVICE_GET_UPDATE_DEVICE_GET_ID,1))
  {
    ui_ret = ui_tmd_api_ofp_rates_update_device_get(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after ofp_rates***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif/* { */ /* } ofp_rates*/

#ifdef UI_END2END_SCHEDULER/* { end2end_scheduler*/
/********************************************************************
 *  Section handler: end2end_scheduler
 */
int
  ui_tmd_api_end2end_scheduler(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_tmd_api_end2end_scheduler");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_DEVICE_RATE_ENTRY_SET_SCH_DEVICE_RATE_ENTRY_SET_ID,1))
  {
    ui_ret = ui_tmd_api_end2end_scheduler_sch_device_rate_entry_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_DEVICE_RATE_ENTRY_GET_SCH_DEVICE_RATE_ENTRY_GET_ID,1))
  {
    ui_ret = ui_tmd_api_end2end_scheduler_sch_device_rate_entry_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_DEVICE_IF_WEIGHT_IDX_SET_SCH_DEVICE_IF_WEIGHT_IDX_SET_ID,1))
  {
    ui_ret = ui_tmd_api_end2end_scheduler_sch_device_if_weight_idx_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_DEVICE_IF_WEIGHT_IDX_GET_SCH_DEVICE_IF_WEIGHT_IDX_GET_ID,1))
  {
    ui_ret = ui_tmd_api_end2end_scheduler_sch_device_if_weight_idx_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_IF_WEIGHT_CONF_SET_SCH_IF_WEIGHT_CONF_SET_ID,1))
  {
    ui_ret = ui_tmd_api_end2end_scheduler_sch_if_weight_conf_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_IF_WEIGHT_CONF_GET_SCH_IF_WEIGHT_CONF_GET_ID,1))
  {
    ui_ret = ui_tmd_api_end2end_scheduler_sch_if_weight_conf_get(current_line);
  }
#ifdef UI_UNDEFED_TEMP
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_FLOW2SE_ID_SCH_FLOW2SE_ID_ID,1))
  {
    ui_ret = ui_tmd_api_end2end_scheduler_sch_flow2se_id(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_SE2FLOW_ID_SCH_SE2FLOW_ID_ID,1))
  {
    ui_ret = ui_tmd_api_end2end_scheduler_sch_se2flow_id(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_SE2PORT_ID_SCH_SE2PORT_ID_ID,1))
  {
    ui_ret = ui_tmd_api_end2end_scheduler_sch_se2port_id(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_PORT2SE_ID_SCH_PORT2SE_ID_ID,1))
  {
    ui_ret = ui_tmd_api_end2end_scheduler_sch_port2se_id(current_line);
  }
#endif /* UI_UNDEFED_TEMP */
/*
    else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGRESS_PORTS_QOS_SET_EGRESS_PORTS_QOS_SET_ID,1))
    {
      ui_ret = ui_tmd_api_end2end_scheduler_egress_ports_qos_set(current_line);
    }
    else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_EGRESS_PORTS_QOS_GET_EGRESS_PORTS_QOS_GET_ID,1))
    {
      ui_ret = ui_tmd_api_end2end_scheduler_egress_ports_qos_get(current_line);
    }
    else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ONE_EGRESS_PORT_QOS_SET_ONE_EGRESS_PORT_QOS_SET_ID,1))
    {
      ui_ret = ui_tmd_api_end2end_scheduler_one_egress_port_qos_set(current_line);
    }
    else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_ONE_EGRESS_PORT_QOS_GET_ONE_EGRESS_PORT_QOS_GET_ID,1))
    {
      ui_ret = ui_tmd_api_end2end_scheduler_one_egress_port_qos_get(current_line);
    }*/

  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_CLASS_TYPE_PARAMS_SET_SCH_CLASS_TYPE_PARAMS_SET_ID,1))
  {
    ui_ret = ui_tmd_api_end2end_scheduler_sch_class_type_params_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_CLASS_TYPE_PARAMS_GET_SCH_CLASS_TYPE_PARAMS_GET_ID,1))
  {
    ui_ret = ui_tmd_api_end2end_scheduler_sch_class_type_params_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_CLASS_TYPE_PARAMS_TABLE_SET_SCH_CLASS_TYPE_PARAMS_TABLE_SET_ID,1))
  {
    ui_ret = ui_tmd_api_end2end_scheduler_sch_class_type_params_table_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_CLASS_TYPE_PARAMS_TABLE_GET_SCH_CLASS_TYPE_PARAMS_TABLE_GET_ID,1))
  {
    ui_ret = ui_tmd_api_end2end_scheduler_sch_class_type_params_table_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_SLOW_MAX_RATES_SET_SCH_SLOW_MAX_RATES_SET_ID,1))
  {
    ui_ret = ui_tmd_api_end2end_scheduler_sch_slow_max_rates_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_SLOW_MAX_RATES_GET_SCH_SLOW_MAX_RATES_GET_ID,1))
  {
    ui_ret = ui_tmd_api_end2end_scheduler_sch_slow_max_rates_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_PORT_SCHED_SET_SCH_PORT_SCHED_SET_ID,1))
  {
    ui_ret = ui_tmd_api_end2end_scheduler_sch_port_sched_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_PORT_SCHED_GET_SCH_PORT_SCHED_GET_ID,1))
  {
    ui_ret = ui_tmd_api_end2end_scheduler_sch_port_sched_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_PORT_HP_CLASS_CONF_SET_SCH_PORT_HP_CLASS_CONF_SET_ID,1))
  {
    ui_ret = ui_tmd_api_end2end_scheduler_sch_port_hp_class_conf_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_PORT_HP_CLASS_CONF_GET_SCH_PORT_HP_CLASS_CONF_GET_ID,1))
  {
    ui_ret = ui_tmd_api_end2end_scheduler_sch_port_hp_class_conf_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_CLASS_TYPE_PARAMS_SET_SCH_CLASS_TYPE_PARAMS_SET_ID,1))
  {
    ui_ret = ui_tmd_api_end2end_scheduler_sch_class_type_params_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_CLASS_TYPE_PARAMS_GET_SCH_CLASS_TYPE_PARAMS_GET_ID,1))
  {
    ui_ret = ui_tmd_api_end2end_scheduler_sch_class_type_params_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_AGGREGATE_SET_SCH_AGGREGATE_SET_ID,1))
  {
    ui_ret = ui_tmd_api_end2end_scheduler_sch_aggregate_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_AGGREGATE_GET_SCH_AGGREGATE_GET_ID,1))
  {
    ui_ret = ui_tmd_api_end2end_scheduler_sch_aggregate_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_FLOW_SET_SCH_FLOW_SET_ID,1))
  {
    ui_ret = ui_tmd_api_end2end_scheduler_sch_flow_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_FLOW_GET_SCH_FLOW_GET_ID,1))
  {
    ui_ret = ui_tmd_api_end2end_scheduler_sch_flow_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_FLOW_STATUS_SET_SCH_FLOW_STATUS_SET_ID,1))
  {
    ui_ret = ui_tmd_api_end2end_scheduler_sch_flow_status_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_PER1K_INFO_SET_SCH_PER1K_INFO_SET_ID,1))
  {
    ui_ret = ui_tmd_api_end2end_scheduler_sch_per1k_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_PER1K_INFO_GET_SCH_PER1K_INFO_GET_ID,1))
  {
    ui_ret = ui_tmd_api_end2end_scheduler_sch_per1k_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_FLOW_TO_QUEUE_MAPPING_SET_SCH_FLOW_TO_QUEUE_MAPPING_SET_ID,1))
  {
    ui_ret = ui_tmd_api_end2end_scheduler_sch_flow_to_queue_mapping_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_FLOW_TO_QUEUE_MAPPING_GET_SCH_FLOW_TO_QUEUE_MAPPING_GET_ID,1))
  {
    ui_ret = ui_tmd_api_end2end_scheduler_sch_flow_to_queue_mapping_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_SCH_PRINT_FLOW_AND_UP_ID,1))
  {
    ui_ret = ui_tmd_api_end2end_scheduler_print_flow_and_up(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after end2end_scheduler***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif/* { */ /* } end2end_scheduler*/

#ifdef UI_MULTICAST_INGRESS/* { multicast_ingress*/
/********************************************************************
 *  Section handler: multicast_ingress
 */
int
  ui_tmd_api_multicast_ingress(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_tmd_api_multicast_ingress");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_ING_TRAFFIC_CLASS_MAP_SET_MULT_ING_TRAFFIC_CLASS_MAP_SET_ID,1))
  {
    ui_ret = ui_tmd_api_multicast_ingress_mult_ing_traffic_class_map_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_ING_TRAFFIC_CLASS_MAP_GET_MULT_ING_TRAFFIC_CLASS_MAP_GET_ID,1))
  {
    ui_ret = ui_tmd_api_multicast_ingress_mult_ing_traffic_class_map_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_ING_GROUP_CLOSE_MULT_ING_GROUP_CLOSE_ID,1))
  {
    ui_ret = ui_tmd_api_multicast_ingress_mult_ing_group_close(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_ING_DESTINATION_ADD_MULT_ING_DESTINATION_ADD_ID,1))
  {
    ui_ret = ui_tmd_api_multicast_ingress_mult_ing_destination_add(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_ING_DESTINATION_REMOVE_MULT_ING_DESTINATION_REMOVE_ID,1))
  {
    ui_ret = ui_tmd_api_multicast_ingress_mult_ing_destination_remove(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_ING_GROUP_SIZE_GET_MULT_ING_GROUP_SIZE_GET_ID,1))
  {
    ui_ret = ui_tmd_api_multicast_ingress_mult_ing_group_size_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_ING_ALL_GROUPS_CLOSE_MULT_ING_ALL_GROUPS_CLOSE_ID,1))
  {
    ui_ret = ui_tmd_api_multicast_ingress_mult_ing_all_groups_close(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after multicast_ingress***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif/* { */ /* } multicast_ingress*/

#ifdef UI_MULTICAST_EGRESS /* { multicast_egress*/
/********************************************************************
 *  Section handler: multicast_egress
 */
int
  ui_tmd_api_multicast_egress(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_tmd_api_multicast_egress");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE_SET_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE_SET_ID,1))
  {
    ui_ret = ui_tmd_api_multicast_egress_mult_eg_vlan_membership_group_range_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE_GET_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE_GET_ID,1))
  {
    ui_ret = ui_tmd_api_multicast_egress_mult_eg_vlan_membership_group_range_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_EG_GROUP_CLOSE_MULT_EG_GROUP_CLOSE_ID,1))
  {
    ui_ret = ui_tmd_api_multicast_egress_mult_eg_group_close(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_EG_PORT_ADD_MULT_EG_PORT_ADD_ID,1))
  {
    ui_ret = ui_tmd_api_multicast_egress_mult_eg_port_add(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_EG_PORT_REMOVE_MULT_EG_PORT_REMOVE_ID,1))
  {
    ui_ret = ui_tmd_api_multicast_egress_mult_eg_port_remove(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_EG_GROUP_SIZE_GET_MULT_EG_GROUP_SIZE_GET_ID,1))
  {
    ui_ret = ui_tmd_api_multicast_egress_mult_eg_group_size_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_EG_ALL_GROUPS_CLOSE_MULT_EG_ALL_GROUPS_CLOSE_ID,1))
  {
    ui_ret = ui_tmd_api_multicast_egress_mult_eg_all_groups_close(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_EG_VLAN_MEMBERSHIP_GROUP_OPEN_MULT_EG_VLAN_MEMBERSHIP_GROUP_OPEN_ID,1))
  {
    ui_ret = ui_tmd_api_multicast_egress_mult_eg_vlan_membership_group_open(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_EG_VLAN_MEMBERSHIP_GROUP_UPDATE_MULT_EG_VLAN_MEMBERSHIP_GROUP_UPDATE_ID,1))
  {
    ui_ret = ui_tmd_api_multicast_egress_mult_eg_vlan_membership_group_update(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_EG_VLAN_MEMBERSHIP_GROUP_CLOSE_MULT_EG_VLAN_MEMBERSHIP_GROUP_CLOSE_ID,1))
  {
    ui_ret = ui_tmd_api_multicast_egress_mult_eg_vlan_membership_group_close(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_EG_VLAN_MEMBERSHIP_PORT_ADD_MULT_EG_VLAN_MEMBERSHIP_PORT_ADD_ID,1))
  {
    ui_ret = ui_tmd_api_multicast_egress_mult_eg_vlan_membership_port_add(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_EG_VLAN_MEMBERSHIP_PORT_REMOVE_MULT_EG_VLAN_MEMBERSHIP_PORT_REMOVE_ID,1))
  {
    ui_ret = ui_tmd_api_multicast_egress_mult_eg_vlan_membership_port_remove(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_EG_VLAN_MEMBERSHIP_GROUP_GET_MULT_EG_VLAN_MEMBERSHIP_GROUP_GET_ID,1))
  {
    ui_ret = ui_tmd_api_multicast_egress_mult_eg_vlan_membership_group_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_EG_VLAN_MEMBERSHIP_ALL_GROUPS_CLOSE_MULT_EG_VLAN_MEMBERSHIP_ALL_GROUPS_CLOSE_ID,1))
  {
    ui_ret = ui_tmd_api_multicast_egress_mult_eg_vlan_membership_all_groups_close(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after multicast_egress***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif/* { */ /* } multicast_egress*/

#ifdef UI_MULTICAST_FABRIC/* { multicast_fabric*/
/********************************************************************
 *  Section handler: multicast_fabric
 */
int
  ui_tmd_api_multicast_fabric(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_tmd_api_multicast_fabric");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_FABRIC_TRAFFIC_CLASS_TO_MULTICAST_CLS_MAP_SET_MULT_FABRIC_TRAFFIC_CLASS_TO_MULTICAST_CLS_MAP_SET_ID,1))
  {
    ui_ret = ui_tmd_api_multicast_fabric_mult_fabric_traffic_class_to_multicast_cls_map_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_FABRIC_TRAFFIC_CLASS_TO_MULTICAST_CLS_MAP_GET_MULT_FABRIC_TRAFFIC_CLASS_TO_MULTICAST_CLS_MAP_GET_ID,1))
  {
    ui_ret = ui_tmd_api_multicast_fabric_mult_fabric_traffic_class_to_multicast_cls_map_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_FABRIC_BASE_QUEUE_SET_MULT_FABRIC_BASE_QUEUE_SET_ID,1))
  {
    ui_ret = ui_tmd_api_multicast_fabric_mult_fabric_base_queue_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_FABRIC_BASE_QUEUE_GET_MULT_FABRIC_BASE_QUEUE_GET_ID,1))
  {
    ui_ret = ui_tmd_api_multicast_fabric_mult_fabric_base_queue_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_FABRIC_CREDIT_SOURCE_SET_MULT_FABRIC_CREDIT_SOURCE_SET_ID,1))
  {
    ui_ret = ui_tmd_api_multicast_fabric_mult_fabric_credit_source_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_FABRIC_CREDIT_SOURCE_GET_MULT_FABRIC_CREDIT_SOURCE_GET_ID,1))
  {
    ui_ret = ui_tmd_api_multicast_fabric_mult_fabric_credit_source_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_FABRIC_ENHANCED_SET_MULT_FABRIC_ENHANCED_SET_ID,1))
  {
    ui_ret = ui_tmd_api_multicast_fabric_mult_fabric_enhanced_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_FABRIC_ENHANCED_GET_MULT_FABRIC_ENHANCED_GET_ID,1))
  {
    ui_ret = ui_tmd_api_multicast_fabric_mult_fabric_enhanced_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_FABRIC_ACTIVE_LINKS_SET_MULT_FABRIC_ACTIVE_LINKS_SET_ID,1))
  {
    ui_ret = ui_tmd_api_multicast_fabric_mult_fabric_active_links_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_MULT_FABRIC_ACTIVE_LINKS_GET_MULT_FABRIC_ACTIVE_LINKS_GET_ID,1))
  {
    ui_ret = ui_tmd_api_multicast_fabric_mult_fabric_active_links_get(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after multicast_fabric***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif/* { */ /* } multicast_fabric*/

#ifdef UI_FABRIC/* { fabric*/
/********************************************************************
 *  Section handler: fabric
 */
int
  ui_tmd_api_fabric(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_tmd_api_fabric");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_FABRIC_SRD_QRTT_RESET_SRD_QRTT_RESET_ID,1))
  {
    ui_ret = ui_tmd_api_fabric_srd_qrtt_reset(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_FABRIC_FC_ENABLE_SET_FC_ENABLE_SET_ID,1))
  {
    ui_ret = ui_tmd_api_fabric_fc_enable_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_FABRIC_FC_ENABLE_GET_FC_ENABLE_GET_ID,1))
  {
    ui_ret = ui_tmd_api_fabric_fc_enable_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_FABRIC_CELL_FORMAT_GET_CELL_FORMAT_GET_ID,1))
  {
    ui_ret = ui_tmd_api_fabric_cell_format_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_FABRIC_COEXIST_SET_COEXIST_SET_ID,1))
  {
    ui_ret = ui_tmd_api_fabric_coexist_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_FABRIC_COEXIST_GET_COEXIST_GET_ID,1))
  {
    ui_ret = ui_tmd_api_fabric_coexist_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_FABRIC_STAND_ALONE_FAP_MODE_GET_STAND_ALONE_FAP_MODE_GET_ID,1))
  {
    ui_ret = ui_tmd_api_fabric_stand_alone_fap_mode_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_FABRIC_CONNECT_MODE_SET_CONNECT_MODE_SET_ID,1))
  {
    ui_ret = ui_tmd_api_fabric_connect_mode_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_FABRIC_CONNECT_MODE_GET_CONNECT_MODE_GET_ID,1))
  {
    ui_ret = ui_tmd_api_fabric_connect_mode_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_FABRIC_FAP20_MAP_SET_FAP20_MAP_SET_ID,1))
  {
    ui_ret = ui_tmd_api_fabric_fap20_map_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_FABRIC_FAP20_MAP_GET_FAP20_MAP_GET_ID,1))
  {
    ui_ret = ui_tmd_api_fabric_fap20_map_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_TOPOLOGY_STATUS_CONNECTIVITY_GET_TOPOLOGY_STATUS_CONNECTIVITY_GET_ID,1))
  {
    ui_ret = ui_tmd_api_fabric_topology_status_connectivity_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_LINKS_STATUS_GET_LINKS_STATUS_GET_ID,1))
  {
    ui_ret = ui_tmd_api_fabric_links_status_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_LINK_ON_OFF_SET_LINK_ON_OFF_SET_ID,1))
  {
    ui_ret = ui_tmd_api_fabric_link_on_off_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_LINK_ON_OFF_GET_LINK_ON_OFF_GET_ID,1))
  {
    ui_ret = ui_tmd_api_fabric_link_on_off_get(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after fabric***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif/* { fabric*/

#ifdef UI_TMD_HEADER_PARSING_UTILS/* { tmd_header_parsing_utils*/
/********************************************************************
 *  Section handler: tmd_header_parsing_utils
 */
int
  ui_tmd_header_parsing_utils(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_tmd_header_parsing_utils");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_HPU_ITMH_BUILD_HPU_ITMH_BUILD_ID,1))
  {
    ui_ret = ui_tmd_header_parsing_utils_hpu_itmh_build(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_HPU_ITMH_PARSE_HPU_ITMH_PARSE_ID,1))
  {
    ui_ret = ui_tmd_header_parsing_utils_hpu_itmh_parse(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_HPU_FTMH_BUILD_HPU_FTMH_BUILD_ID,1))
  {
    ui_ret = ui_tmd_header_parsing_utils_hpu_ftmh_build(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_HPU_FTMH_PARSE_HPU_FTMH_PARSE_ID,1))
  {
    ui_ret = ui_tmd_header_parsing_utils_hpu_ftmh_parse(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_HPU_OTMH_BUILD_HPU_OTMH_BUILD_ID,1))
  {
    ui_ret = ui_tmd_header_parsing_utils_hpu_otmh_build(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_HPU_OTMH_PARSE_HPU_OTMH_PARSE_ID,1))
  {
    ui_ret = ui_tmd_header_parsing_utils_hpu_otmh_parse(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after tmd_header_parsing_utils***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif/* { */ /* } tmd_header_parsing_utils*/

#ifdef UI_REG_ACCESS/* { reg_access*/
/********************************************************************
 *  Section handler: reg_access
 */
int
  ui_tmd_api_reg_access(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_tmd_api_reg_access");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_READ_FLD_READ_FLD_ID,1))
  {
    ui_ret = ui_tmd_api_reg_access_read_fld(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_WRITE_FLD_WRITE_FLD_ID,1))
  {
    ui_ret = ui_tmd_api_reg_access_write_fld(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_READ_REG_READ_REG_ID,1))
  {
    ui_ret = ui_tmd_api_reg_access_read_reg(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_WRITE_REG_WRITE_REG_ID,1))
  {
    ui_ret = ui_tmd_api_reg_access_write_reg(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_IREAD_ID,1))
  {
    ui_ret = ui_tmd_iread(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_IWRITE_ID,1))
  {
    ui_ret = ui_tmd_iwrite(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after reg_access***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif /* { reg_access*/

#ifdef UI_DEBUG/* { debug*/
/********************************************************************
 *  Section handler: debug
 */
int
  ui_tmd_api_debug(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_tmd_api_debug");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_DBG_ROUTE_FORCE_SET_DBG_ROUTE_FORCE_SET_ID,1))
  {
    ui_ret = ui_tmd_api_debug_dbg_route_force_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_DBG_ROUTE_FORCE_GET_DBG_ROUTE_FORCE_GET_ID,1))
  {
    ui_ret = ui_tmd_api_debug_dbg_route_force_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_DBG_AUTOCREDIT_SET_DBG_AUTOCREDIT_SET_ID,1))
  {
    ui_ret = ui_tmd_api_debug_dbg_autocredit_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_DBG_AUTOCREDIT_GET_DBG_AUTOCREDIT_GET_ID,1))
  {
    ui_ret = ui_tmd_api_debug_dbg_autocredit_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_DBG_EGRESS_SHAPING_ENABLE_SET_DBG_EGRESS_SHAPING_ENABLE_SET_ID,1))
  {
    ui_ret = ui_tmd_api_debug_dbg_egress_shaping_enable_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_DBG_EGRESS_SHAPING_ENABLE_GET_DBG_EGRESS_SHAPING_ENABLE_GET_ID,1))
  {
    ui_ret = ui_tmd_api_debug_dbg_egress_shaping_enable_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_DBG_FLOW_CONTROL_ENABLE_SET_FLOW_CONTROL_ENABLE_SET_ID,1))
  {
    ui_ret = ui_tmd_api_dbg_flow_control_enable_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_DBG_FLOW_CONTROL_ENABLE_GET_FLOW_CONTROL_ENABLE_GET_ID,1))
  {
    ui_ret = ui_tmd_api_dbg_flow_control_enable_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_DBG_QUEUE_FLUSH_DBG_QUEUE_FLUSH_ID,1))
  {
    ui_ret = ui_tmd_api_debug_dbg_queue_flush(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_DBG_QUEUE_FLUSH_ALL_DBG_QUEUE_FLUSH_ALL_ID,1))
  {
    ui_ret = ui_tmd_api_debug_dbg_queue_flush_all(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_DBG_INGR_RESET_DBG_INGR_RESET_ID,1))
  {
    ui_ret = ui_tmd_api_debug_dbg_ingr_reset(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after debug***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif/* } debug */

/*****************************************************
*NAME
*  subject_tmd_api
*TYPE: PROC
*DATE: 29/DEC/2002
*FUNCTION:
*  Process input line which has an 'subject_tmd_api' subject.
*  Input line is assumed to have been checked and
*  found to be of right format.
*CALLING SEQUENCE:
*  subject_tmd_api(current_line,current_line_ptr)
*INPUT:
*  SOC_SAND_DIRECT:
*    CURRENT_LINE *current_line -
*      Pointer to prompt line to process.
*    CURRENT_LINE **current_line_ptr -
*      Pointer to prompt line to be displayed after
*      this procedure finishes execution. Caller
*      points this variable to the pointer to
*      the next line to display. If called function wishes
*      to set the next line to display, it replaces
*      the pointer to the next line to display.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non zero then some error has occurred.
*  SOC_SAND_INDIRECT:
*    Processing results. See 'current_line_ptr'.
*REMARKS:
*  This procedure should be carried out under 'task_safe'
*  state (i.e., task can not be deleted while this
*  procedure is being carried out).
*SEE ALSO:
 */
int
  subject_tmd_api(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  )
{
  unsigned int
    match_index;
  int
    ui_ret = 0;
  unsigned int
    unit = 0;
  char
    *proc_name ;

  proc_name = "subject_tmd_api" ;
  ui_ret = FALSE ;
  unit = 0;


  /*
   * the rest of the line is empty
   */
  if (current_line->num_param_names == 0)
  {
    send_string_to_screen("\n\r",FALSE) ;
    send_string_to_screen("'subject_tmd_api()' function was called with no parameters.\n\r",FALSE) ;
    goto exit ;
  }
  /*
   * Else, there are parameters on the line (not just 'subject_tmd_api').
   */

  send_array_to_screen("\n\r",2) ;



  if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_API_MGMT_ID,1))
  {
    ui_ret = ui_tmd_api_mgmt(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_API_PORTS_ID,1))
  {
    ui_ret = ui_tmd_api_ports(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_API_INGRESS_PACKET_QUEUING_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_packet_queuing(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_API_INGRESS_TRAFFIC_MGMT_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_traffic_mgmt(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_API_INGRESS_SCHEDULER_ID,1))
  {
    ui_ret = ui_tmd_api_ingress_scheduler(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_API_EGR_QUEUING_ID,1))
  {
    ui_ret = ui_tmd_api_egr_queuing(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_API_OFP_RATES_ID,1))
  {
    ui_ret = ui_tmd_api_ofp_rates(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_API_END2END_SCHEDULER_ID,1))
  {
    ui_ret = ui_tmd_api_end2end_scheduler(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_API_MULTICAST_INGRESS_ID,1))
  {
    ui_ret = ui_tmd_api_multicast_ingress(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_API_MULTICAST_EGRESS_ID,1))
  {
    ui_ret = ui_tmd_api_multicast_egress(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_API_MULTICAST_FABRIC_ID,1))
  {
    ui_ret = ui_tmd_api_multicast_fabric(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_API_FABRIC_ID,1))
  {
    ui_ret = ui_tmd_api_fabric(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_HEADER_PARSING_UTILS_ID,1))
  {
    ui_ret = ui_tmd_header_parsing_utils(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_API_REG_ACCESS_ID,1))
  {
    ui_ret = ui_tmd_api_reg_access(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_API_UTILS_ID,1))
  {
    ui_ret = ui_tmd_api_utils(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_API_PACKET_ID,1))
  {
    ui_ret = ui_tmd_api_packet(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_API_CELLS_ID,1))
  {
    ui_ret = ui_tmd_api_cell(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_TMD_API_DEBUG_ID,1))
  {
    ui_ret = ui_tmd_api_debug(current_line);
  }
  else
  {
    /*
     * Enter if an unknown request.
     */
    send_string_to_screen(
      "\n\r"
      "*** tmd_api command with unknown parameters'.\n\r"
      "    Syntax error/sw error...\n\r",
      TRUE)  ;
    ui_ret = TRUE  ;
    goto exit  ;
  }
exit:
  return (ui_ret);
}

#endif
