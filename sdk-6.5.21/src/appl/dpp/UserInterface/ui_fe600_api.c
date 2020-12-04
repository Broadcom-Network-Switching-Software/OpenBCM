/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#if !DUNE_BCM

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

#include <appl/dpp/UserInterface/ui_pure_defi_fe600_api.h>
/*
#include <sweep/SOC_SAND_FE600/sweep_fe600_ssr.h>
*/
#include <soc/dpp/SOC_SAND_FE600/fe600_api_general.h>
#include <soc/dpp/SOC_SAND_FE600/fe600_api_mgmt.h>
#include <soc/dpp/SOC_SAND_FE600/fe600_api_links_topology.h>
#include <soc/dpp/SOC_SAND_FE600/fe600_api_links.h>
#include <soc/dpp/SOC_SAND_FE600/fe600_links.h>
#include <soc/dpp/SOC_SAND_FE600/fe600_api_multicast.h>
#include <soc/dpp/SOC_SAND_FE600/fe600_api_status.h>
#include <soc/dpp/SOC_SAND_FE600/fe600_api_serdes.h>
#include <soc/dpp/SOC_SAND_FE600/fe600_api_serdes_utils.h>
#include <soc/dpp/SOC_SAND_FE600/fe600_api_reg_access.h>
#include <soc/dpp/SOC_SAND_FE600/fe600_chip_tbls.h>
#include <soc/dpp/SOC_SAND_FE600/fe600_general.h>
#include <soc/dpp/SOC_SAND_FE600/fe600_api_cell.h>
#include <appl/dpp/sweep/SOC_SAND_FE600/sweep_fe600_ssr.h>
#include <soc/dpp/SAND/SAND_FM/sand_mem_access.h>
static
  uint32
    Default_unit = 0;

void
  fe600_set_default_unit(uint32 dev_id)
{
  Default_unit = dev_id;
}

uint32
  fe600_get_default_unit()
{
  return Default_unit;
}

#ifdef UI_FE600_UTILS
int
  ui_fe600_api_set_default_unit(
    CURRENT_LINE *current_line
  )
{
  uint32
    prm_unit = 0;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_utils");
  soc_sand_proc_name = "ui_fe600_api_set_default_unit";

  prm_unit = fe600_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_UTILS_SET_DEFAULT_DEVICE_ID_DEVICE_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_UTILS_SET_DEFAULT_DEVICE_ID_DEVICE_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_unit = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  fe600_set_default_unit(
    prm_unit
    );

  soc_sand_os_printf( "unit: %d\n\r",prm_unit);


  goto exit;
exit:
  return ui_ret;
}

int
  ui_fe600_api_get_default_unit(
    CURRENT_LINE *current_line
  )
{
  uint32
    prm_unit = 0;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_utils");
  soc_sand_proc_name = "fe600_get_default_unit";

  unit = fe600_get_default_unit();

  prm_unit = unit;

  soc_sand_os_printf( "unit: %d\n\r",prm_unit);

  goto exit;
exit:
  return ui_ret;
}

#endif /* #ifdef UI_FE600_UTILS */

#ifdef UI_FE600_MGMT
/********************************************************************
 *  Function handler: register_device (section mgmt)
 */
int
  ui_fe600_api_mgmt_register_device(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_base_address;
  SOC_SAND_RESET_DEVICE_FUNC_PTR
    prm_reset_device_ptr=NULL;
  uint32
    prm_unit_ptr;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_mgmt");
  soc_sand_proc_name = "fe600_register_device";

  unit = fe600_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_REGISTER_DEVICE_REGISTER_DEVICE_BASE_ADDRESS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_REGISTER_DEVICE_REGISTER_DEVICE_BASE_ADDRESS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_base_address = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_REGISTER_DEVICE_REGISTER_DEVICE_RESET_DEVICE_PTR_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_REGISTER_DEVICE_REGISTER_DEVICE_RESET_DEVICE_PTR_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_reset_device_ptr = (SOC_SAND_RESET_DEVICE_FUNC_PTR)param_val->value.ulong_value;
  }


  /* Call function */
  ret = fe600_register_device(
          &prm_base_address,
          prm_reset_device_ptr,
          &prm_unit_ptr
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_register_device - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_register_device");
    goto exit;
  }

  send_string_to_screen(" ---> fe600_register_device - SUCCEEDED", TRUE);
  send_string_to_screen("--> unit_ptr:", TRUE);
  soc_sand_os_printf( "unit_ptr: %d\n\r",prm_unit_ptr);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: unregister_device (section mgmt)
 */
int
  ui_fe600_api_mgmt_unregister_device(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  UI_MACROS_INIT_FUNCTION("ui_fe600_api_mgmt");
  soc_sand_proc_name = "fe600_unregister_device";

  unit = fe600_get_default_unit();

  /* Get parameters */

  /* Call function */
  ret = fe600_unregister_device(
          unit
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_unregister_device - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_unregister_device");
    goto exit;
  }

  send_string_to_screen(" ---> fe600_unregister_device - SUCCEEDED", TRUE);
/*
  soc_sand_os_printf( "SSR Test device\n\r");
  sweep_fe600_ssr_test_xor_check(unit, FALSE);
*/
  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: operation_mode_set (section mgmt)
 */
int
  ui_fe600_api_mgmt_operation_mode_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  FE600_OPERATION_MODE
    prm_operation_mode;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_mgmt");
  soc_sand_proc_name = "fe600_mgmt_operation_mode_set";

  unit = fe600_get_default_unit();
  fe600_FE600_OPERATION_MODE_clear(&prm_operation_mode);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = fe600_mgmt_operation_mode_get(
          unit,
          &prm_operation_mode
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_mgmt_operation_mode_get - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_mgmt_operation_mode_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_MGMT_OPERATION_MODE_SET_OPERATION_MODE_SET_OPERATION_MODE_ENABLE_16K_MULTICAST_GROUPS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_MGMT_OPERATION_MODE_SET_OPERATION_MODE_SET_OPERATION_MODE_ENABLE_16K_MULTICAST_GROUPS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_operation_mode.enable_16k_multicast_groups = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_MGMT_OPERATION_MODE_SET_OPERATION_MODE_SET_OPERATION_MODE_IS_64_LINKS_MODE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_MGMT_OPERATION_MODE_SET_OPERATION_MODE_SET_OPERATION_MODE_IS_64_LINKS_MODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_operation_mode.is_64_links_mode = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_MGMT_OPERATION_MODE_SET_OPERATION_MODE_SET_OPERATION_MODE_IS_TDM_ONLY_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_MGMT_OPERATION_MODE_SET_OPERATION_MODE_SET_OPERATION_MODE_IS_TDM_ONLY_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_operation_mode.is_TDM_only = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_MGMT_OPERATION_MODE_SET_OPERATION_MODE_SET_OPERATION_MODE_DIRECT_MULTICAST_MODE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_MGMT_OPERATION_MODE_SET_OPERATION_MODE_SET_OPERATION_MODE_DIRECT_MULTICAST_MODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_operation_mode.direct_multicast_mode = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_MGMT_OPERATION_MODE_SET_OPERATION_MODE_SET_OPERATION_MODE_LOAD_BALANCE_MTHD_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FE600_MGMT_OPERATION_MODE_SET_OPERATION_MODE_SET_OPERATION_MODE_LOAD_BALANCE_MTHD_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_operation_mode.load_balance_mthd = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_MGMT_OPERATION_MODE_SET_OPERATION_MODE_SET_OPERATION_MODE_SUPPORT_INBAND_CONFIGURATION_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_MGMT_OPERATION_MODE_SET_OPERATION_MODE_SET_OPERATION_MODE_SUPPORT_INBAND_CONFIGURATION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_operation_mode.support_inband_configuration = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_MGMT_OPERATION_MODE_SET_OPERATION_MODE_SET_OPERATION_MODE_IS_VARIABLE_CELL_SIZE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_MGMT_OPERATION_MODE_SET_OPERATION_MODE_SET_OPERATION_MODE_IS_VARIABLE_CELL_SIZE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_operation_mode.is_variable_cell_size = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_MGMT_OPERATION_MODE_SET_OPERATION_MODE_SET_OPERATION_MODE_FABRIC_MODE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FE600_MGMT_OPERATION_MODE_SET_OPERATION_MODE_SET_OPERATION_MODE_FABRIC_MODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_operation_mode.fabric_mode = param_val->numeric_equivalent;
  }


  /* Call function */
  ret = fe600_mgmt_operation_mode_set(
          unit,
          &prm_operation_mode
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_mgmt_operation_mode_set - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_mgmt_operation_mode_set");
    goto exit;
  }

  send_string_to_screen(" ---> fe600_mgmt_operation_mode_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: operation_mode_get (section mgmt)
 */
int
  ui_fe600_api_mgmt_operation_mode_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  FE600_OPERATION_MODE
    prm_operation_mode;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_mgmt");
  soc_sand_proc_name = "fe600_mgmt_operation_mode_get";

  unit = fe600_get_default_unit();
  fe600_FE600_OPERATION_MODE_clear(&prm_operation_mode);

  /* Get parameters */

  /* Call function */
  ret = fe600_mgmt_operation_mode_get(
          unit,
          &prm_operation_mode
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_mgmt_operation_mode_get - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_mgmt_operation_mode_get");
    goto exit;
  }

  send_string_to_screen(" ---> fe600_mgmt_operation_mode_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> operation_mode:", TRUE);
  fe600_FE600_OPERATION_MODE_print(&prm_operation_mode);


  goto exit;
exit:
  return ui_ret;
}


/********************************************************************
 *  Function handler: init_sequence_phase2 (section mgmt)
 */
int
  ui_fe600_api_mgmt_init_sequence_phase2(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_mgmt");
  soc_sand_proc_name = "fe600_mgmt_init_sequence_phase2";

  unit = fe600_get_default_unit();

  /* Get parameters */

  /* Call function */
  ret = fe600_mgmt_init_sequence_phase2(
          unit
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_mgmt_init_sequence_phase2 - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_mgmt_init_sequence_phase2");
    goto exit;
  }

  send_string_to_screen(" ---> fe600_mgmt_init_sequence_phase2 - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: system_fe_id_set (section mgmt)
 */
int
  ui_fe600_api_mgmt_system_fe_id_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_fabric_id;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_mgmt");
  soc_sand_proc_name = "fe600_mgmt_system_fe_id_set";

  unit = fe600_get_default_unit();

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = fe600_mgmt_system_fe_id_get(
          unit,
          &prm_fabric_id
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_mgmt_system_fe_id_get - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_mgmt_system_fe_id_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_MGMT_SYSTEM_FE_ID_SET_SYSTEM_FE_ID_SET_FABRIC_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_MGMT_SYSTEM_FE_ID_SET_SYSTEM_FE_ID_SET_FABRIC_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_fabric_id = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = fe600_mgmt_system_fe_id_set(
          unit,
          prm_fabric_id
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_mgmt_system_fe_id_set - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_mgmt_system_fe_id_set");
    goto exit;
  }

  send_string_to_screen(" ---> fe600_mgmt_system_fe_id_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: system_fe_id_get (section mgmt)
 */
int
  ui_fe600_api_mgmt_system_fe_id_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_fabric_id;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_mgmt");
  soc_sand_proc_name = "fe600_mgmt_system_fe_id_get";

  unit = fe600_get_default_unit();

  /* Get parameters */

  /* Call function */
  ret = fe600_mgmt_system_fe_id_get(
          unit,
          &prm_fabric_id
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_mgmt_system_fe_id_get - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_mgmt_system_fe_id_get");
    goto exit;
  }

  send_string_to_screen(" ---> fe600_mgmt_system_fe_id_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> fabric_id:", TRUE);
  soc_sand_os_printf( "fabric_id: %lu\n\r",prm_fabric_id);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: all_ctrl_cells_enable (section mgmt)
 */
int
  ui_fe600_api_mgmt_all_ctrl_cells_enable(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint8
    prm_is_enabled=TRUE;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_mgmt");
  soc_sand_proc_name = "fe600_mgmt_all_ctrl_cells_enable";

  unit = fe600_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_MGMT_ALL_CTRL_CELLS_ENABLE_ALL_CTRL_CELLS_ENABLE_IS_ENABLED_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_MGMT_ALL_CTRL_CELLS_ENABLE_ALL_CTRL_CELLS_ENABLE_IS_ENABLED_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_is_enabled = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = fe600_mgmt_all_ctrl_cells_enable(
          unit,
          prm_is_enabled
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_mgmt_all_ctrl_cells_enable - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_mgmt_all_ctrl_cells_enable");
    goto exit;
  }

  send_string_to_screen(" ---> fe600_mgmt_all_ctrl_cells_enable - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: enable_traffic_set (section mgmt)
 */
int
  ui_fe600_api_mgmt_enable_traffic_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint8
    prm_is_enabled;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_mgmt");
  soc_sand_proc_name = "fe600_mgmt_enable_traffic_set";

  unit = fe600_get_default_unit();

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = fe600_mgmt_enable_traffic_get(
          unit,
          &prm_is_enabled
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_mgmt_enable_traffic_get - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_mgmt_enable_traffic_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_MGMT_ENABLE_TRAFFIC_SET_ENABLE_TRAFFIC_SET_IS_ENABLED_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_MGMT_ENABLE_TRAFFIC_SET_ENABLE_TRAFFIC_SET_IS_ENABLED_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_is_enabled = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = fe600_mgmt_enable_traffic_set(
          unit,
          prm_is_enabled
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_mgmt_enable_traffic_set - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_mgmt_enable_traffic_set");
    goto exit;
  }

  send_string_to_screen(" ---> fe600_mgmt_enable_traffic_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: enable_traffic_get (section mgmt)
 */
int
  ui_fe600_api_mgmt_enable_traffic_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint8
    prm_is_enabled;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_mgmt");
  soc_sand_proc_name = "fe600_mgmt_enable_traffic_get";

  unit = fe600_get_default_unit();

  /* Get parameters */

  /* Call function */
  ret = fe600_mgmt_enable_traffic_get(
          unit,
          &prm_is_enabled
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_mgmt_enable_traffic_get - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_mgmt_enable_traffic_get");
    goto exit;
  }

  send_string_to_screen(" ---> fe600_mgmt_enable_traffic_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> is_enabled:", TRUE);
  soc_sand_os_printf( "is_enabled: %u\n\r",prm_is_enabled);


  goto exit;
exit:
  return ui_ret;
}

int
  ui_fe600_api_ssr(
  CURRENT_LINE *current_line
  )
{

  UI_MACROS_INIT_FUNCTION("ui_swp_p_ssr");
  soc_sand_os_printf( "Removed from CLI\r\n");
  goto exit;
exit:
  return ui_ret;
}

#endif
#ifdef UI_FE600_LINKS_TOPOLOGY
/********************************************************************
 *  Function handler: topology_const_connectivity_set (section links_topology)
 */
int
  ui_fe600_api_links_topology_topology_const_connectivity_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_far_unit_index = 0xFFFFFFFF;
  FE600_TOPOLOGY_MAP
    prm_sys_topology;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_links_topology");
  soc_sand_proc_name = "fe600_topology_const_connectivity_set";

  unit = fe600_get_default_unit();
  fe600_FE600_TOPOLOGY_MAP_clear(&prm_sys_topology);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = fe600_topology_const_connectivity_get(
          unit,
          &prm_sys_topology
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_topology_const_connectivity_get - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_topology_const_connectivity_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_TOPOLOGY_CONST_CONNECTIVITY_SET_TOPOLOGY_CONST_CONNECTIVITY_SET_SYS_TOPOLOGY_FAR_DEVICE_ID_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_TOPOLOGY_CONST_CONNECTIVITY_SET_TOPOLOGY_CONST_CONNECTIVITY_SET_SYS_TOPOLOGY_FAR_DEVICE_ID_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_far_unit_index = (uint32)param_val->value.ulong_value;
  }

  if(prm_far_unit_index != 0xFFFFFFFF)
  {

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_TOPOLOGY_CONST_CONNECTIVITY_SET_TOPOLOGY_CONST_CONNECTIVITY_SET_SYS_TOPOLOGY_FAR_DEVICE_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_TOPOLOGY_CONST_CONNECTIVITY_SET_TOPOLOGY_CONST_CONNECTIVITY_SET_SYS_TOPOLOGY_FAR_DEVICE_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sys_topology.far_unit[ prm_far_unit_index] = (uint32)param_val->value.ulong_value;
  }

  }


  /* Call function */
  ret = fe600_topology_const_connectivity_set(
          unit,
          &prm_sys_topology
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_topology_const_connectivity_set - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_topology_const_connectivity_set");
    goto exit;
  }

  send_string_to_screen(" ---> fe600_topology_const_connectivity_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: topology_const_connectivity_get (section links_topology)
 */
int
  ui_fe600_api_links_topology_topology_const_connectivity_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  FE600_TOPOLOGY_MAP
    prm_sys_topology;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_links_topology");
  soc_sand_proc_name = "fe600_topology_const_connectivity_get";

  unit = fe600_get_default_unit();
  fe600_FE600_TOPOLOGY_MAP_clear(&prm_sys_topology);

  /* Get parameters */

  /* Call function */
  ret = fe600_topology_const_connectivity_get(
          unit,
          &prm_sys_topology
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_topology_const_connectivity_get - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_topology_const_connectivity_get");
    goto exit;
  }

  send_string_to_screen(" ---> fe600_topology_const_connectivity_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> sys_topology:", TRUE);
  fe600_FE600_TOPOLOGY_MAP_print(&prm_sys_topology);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: topology_const_connectivity_update (section links_topology)
 */
int
  ui_fe600_api_links_topology_topology_const_connectivity_update(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_far_unit_index = 0xFFFFFFFF;
  FE600_TOPOLOGY_MAP
    prm_sys_topology;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_links_topology");
  soc_sand_proc_name = "fe600_topology_const_connectivity_update";

  unit = fe600_get_default_unit();
  fe600_FE600_TOPOLOGY_MAP_clear(&prm_sys_topology);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_TOPOLOGY_CONST_CONNECTIVITY_UPDATE_TOPOLOGY_CONST_CONNECTIVITY_UPDATE_SYS_TOPOLOGY_FAR_DEVICE_ID_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_TOPOLOGY_CONST_CONNECTIVITY_UPDATE_TOPOLOGY_CONST_CONNECTIVITY_UPDATE_SYS_TOPOLOGY_FAR_DEVICE_ID_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_far_unit_index = (uint32)param_val->value.ulong_value;
  }

  if(prm_far_unit_index != 0xFFFFFFFF)
  {

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_TOPOLOGY_CONST_CONNECTIVITY_UPDATE_TOPOLOGY_CONST_CONNECTIVITY_UPDATE_SYS_TOPOLOGY_FAR_DEVICE_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_TOPOLOGY_CONST_CONNECTIVITY_UPDATE_TOPOLOGY_CONST_CONNECTIVITY_UPDATE_SYS_TOPOLOGY_FAR_DEVICE_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sys_topology.far_unit[ prm_far_unit_index] = (uint32)param_val->value.ulong_value;
  }

  }


  /* Call function */
  ret = fe600_topology_const_connectivity_update(
          unit,
          &prm_sys_topology
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_topology_const_connectivity_update - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_topology_const_connectivity_update");
    goto exit;
  }

  send_string_to_screen(" ---> fe600_topology_const_connectivity_update - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: topology_set_const_topology_as_current (section links_topology)
 */
int
  ui_fe600_api_links_topology_topology_set_const_topology_as_current(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_links_topology");
  soc_sand_proc_name = "fe600_topology_set_const_topology_as_current";

  unit = fe600_get_default_unit();

  /* Get parameters */

  /* Call function */
  ret = fe600_topology_set_const_topology_as_current(
          unit
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_topology_set_const_topology_as_current - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_topology_set_const_topology_as_current");
    goto exit;
  }

  send_string_to_screen(" ---> fe600_topology_set_const_topology_as_current - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: topology_repeater_set (section links_topology)
 */
int
  ui_fe600_api_links_topology_topology_repeater_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_destination_links_index = 0xFFFFFFFF;
  uint8
    prm_destination_links[FE600_SRD_NOF_LANES];

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_links_topology");
  soc_sand_proc_name = "fe600_topology_repeater_set";

  unit = fe600_get_default_unit();

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = fe600_topology_repeater_get(
          unit,
          prm_destination_links
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_topology_repeater_get - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_topology_repeater_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_TOPOLOGY_REPEATER_SET_TOPOLOGY_REPEATER_SET_DESTINATION_LINKS_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_TOPOLOGY_REPEATER_SET_TOPOLOGY_REPEATER_SET_DESTINATION_LINKS_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_destination_links_index = (uint32)param_val->value.ulong_value;
  }

  if(prm_destination_links_index != 0xFFFFFFFF)
  {

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_TOPOLOGY_REPEATER_SET_TOPOLOGY_REPEATER_SET_DESTINATION_LINKS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_TOPOLOGY_REPEATER_SET_TOPOLOGY_REPEATER_SET_DESTINATION_LINKS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_destination_links[prm_destination_links_index] = (uint8)param_val->value.ulong_value;
  }

  }


  /* Call function */
  ret = fe600_topology_repeater_set(
          unit,
          prm_destination_links
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_topology_repeater_set - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_topology_repeater_set");
    goto exit;
  }

  send_string_to_screen(" ---> fe600_topology_repeater_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: topology_repeater_get (section links_topology)
 */
int
  ui_fe600_api_links_topology_topology_repeater_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    i,
    ret;
  uint8
    prm_destination_links[FE600_SRD_NOF_LANES];

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_links_topology");
  soc_sand_proc_name = "fe600_topology_repeater_get";

  unit = fe600_get_default_unit();

  /* Get parameters */

  /* Call function */
  ret = fe600_topology_repeater_get(
          unit,
          prm_destination_links
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_topology_repeater_get - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_topology_repeater_get");
    goto exit;
  }

  send_string_to_screen(" ---> fe600_topology_repeater_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> destination_links:", TRUE);
  soc_sand_os_printf( "destination_links <link_id,dest_id>: \n\r");

  for(i=0; i<FE600_SRD_NOF_LANES;i++)
  {
    soc_sand_os_printf( "<%d,%d>",i, prm_destination_links[i]);
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: topology_graceful_shutdown (section links_topology)
 */
int
  ui_fe600_api_links_topology_topology_graceful_shutdown(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  FE600_TOPOLOGY_STATE
    prm_state;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_links_topology");
  soc_sand_proc_name = "fe600_topology_graceful_shutdown";

  unit = fe600_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_TOPOLOGY_GRACEFUL_SHUTDOWN_TOPOLOGY_GRACEFUL_SHUTDOWN_STATE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FE600_TOPOLOGY_GRACEFUL_SHUTDOWN_TOPOLOGY_GRACEFUL_SHUTDOWN_STATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_state = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter cells_q_ndx after sr_data_cell_read***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = fe600_topology_graceful_shutdown(
          unit,
          prm_state
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_topology_graceful_shutdown - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_topology_graceful_shutdown");
    goto exit;
  }

  send_string_to_screen(" ---> fe600_topology_graceful_shutdown - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: topology_status_reachability_map_get (section links_topology)
 */
int
  ui_fe600_api_links_topology_topology_status_reachability_map_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ex=0,
    exit_place=0,
    res=0,
    no_err=0,
    ret;
  FE600_FAP_ID
    prm_highest_fap_id = 16;
  FE600_LINKS_TO_DEST_FAP
    *prm_p_reach_map = NULL;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_links_topology");
  soc_sand_proc_name = "fe600_topology_status_reachability_map_get";

  unit = fe600_get_default_unit();
  fe600_FE600_LINKS_TO_DEST_FAP_clear(prm_p_reach_map);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_TOPOLOGY_STATUS_REACHABILITY_MAP_GET_TOPOLOGY_STATUS_REACHABILITY_MAP_GET_HIGHEST_FAP_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_TOPOLOGY_STATUS_REACHABILITY_MAP_GET_TOPOLOGY_STATUS_REACHABILITY_MAP_GET_HIGHEST_FAP_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_highest_fap_id = (uint32)param_val->value.ulong_value;
  }
  FE600_ALLOC(prm_p_reach_map, FE600_LINKS_TO_DEST_FAP, prm_highest_fap_id + 1);

  /* Call function */
  ret = fe600_topology_status_reachability_map_get(
          unit,
          prm_highest_fap_id,
          prm_p_reach_map
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_topology_status_reachability_map_get - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_topology_status_reachability_map_get");
    goto exit;
  }
  fe600_topology_status_reachability_map_print(
    unit,
    prm_highest_fap_id,
    0,
    prm_highest_fap_id,
    prm_p_reach_map
    );

  goto exit;
exit:
  FE600_FREE(prm_p_reach_map);

  return ui_ret;
}

/********************************************************************
 *  Function handler: topology_status_reachability_map_print (section links_topology)
 */
int
  ui_fe600_api_links_topology_topology_status_reachability_map_print(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  FE600_TOPOLOGY_STAT_REACH_PRINT_FLAV
    prm_flavor=FE600_TOPOLOGY_STAT_REACH_PRINT_FLAV_ALL;
  FE600_FAP_ID
    prm_highest_fap_id=0;
  FE600_FAP_ID
    prm_first_fap_id=0;
  FE600_FAP_ID
    prm_last_fap_id=0;
  uint32
    prm_bm_links_index = 0xFFFFFFFF;
  FE600_LINKS_TO_DEST_FAP
    prm_p_reach_map;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_links_topology");
  soc_sand_proc_name = "fe600_topology_status_reachability_map_print";

  unit = fe600_get_default_unit();
  fe600_FE600_LINKS_TO_DEST_FAP_clear(&prm_p_reach_map);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_TOPOLOGY_STATUS_REACHABILITY_MAP_PRINT_TOPOLOGY_STATUS_REACHABILITY_MAP_PRINT_FLAVOR_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FE600_TOPOLOGY_STATUS_REACHABILITY_MAP_PRINT_TOPOLOGY_STATUS_REACHABILITY_MAP_PRINT_FLAVOR_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_flavor = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_TOPOLOGY_STATUS_REACHABILITY_MAP_PRINT_TOPOLOGY_STATUS_REACHABILITY_MAP_PRINT_HIGHEST_FAP_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_TOPOLOGY_STATUS_REACHABILITY_MAP_PRINT_TOPOLOGY_STATUS_REACHABILITY_MAP_PRINT_HIGHEST_FAP_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_highest_fap_id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_TOPOLOGY_STATUS_REACHABILITY_MAP_PRINT_TOPOLOGY_STATUS_REACHABILITY_MAP_PRINT_FIRST_FAP_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_TOPOLOGY_STATUS_REACHABILITY_MAP_PRINT_TOPOLOGY_STATUS_REACHABILITY_MAP_PRINT_FIRST_FAP_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_first_fap_id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_TOPOLOGY_STATUS_REACHABILITY_MAP_PRINT_TOPOLOGY_STATUS_REACHABILITY_MAP_PRINT_LAST_FAP_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_TOPOLOGY_STATUS_REACHABILITY_MAP_PRINT_TOPOLOGY_STATUS_REACHABILITY_MAP_PRINT_LAST_FAP_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_last_fap_id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_TOPOLOGY_STATUS_REACHABILITY_MAP_PRINT_TOPOLOGY_STATUS_REACHABILITY_MAP_PRINT_P_REACH_MAP_BM_LINKS_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_TOPOLOGY_STATUS_REACHABILITY_MAP_PRINT_TOPOLOGY_STATUS_REACHABILITY_MAP_PRINT_P_REACH_MAP_BM_LINKS_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_bm_links_index = (uint32)param_val->value.ulong_value;
  }

  if(prm_bm_links_index != 0xFFFFFFFF)
  {

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_TOPOLOGY_STATUS_REACHABILITY_MAP_PRINT_TOPOLOGY_STATUS_REACHABILITY_MAP_PRINT_P_REACH_MAP_BM_LINKS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_TOPOLOGY_STATUS_REACHABILITY_MAP_PRINT_TOPOLOGY_STATUS_REACHABILITY_MAP_PRINT_P_REACH_MAP_BM_LINKS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_p_reach_map.bm_links[ prm_bm_links_index] = (uint32)param_val->value.ulong_value;
  }

  }


  /* Call function */
  ret = fe600_topology_status_reachability_map_print(
          prm_flavor,
          prm_highest_fap_id,
          prm_first_fap_id,
          prm_last_fap_id,
          &prm_p_reach_map
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_topology_status_reachability_map_print - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_topology_status_reachability_map_print");
    goto exit;
  }

  send_string_to_screen(" ---> fe600_topology_status_reachability_map_print - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: topology_status_cmp_with_curr_connectivity_get (section links_topology)
 */
int
  ui_fe600_api_links_topology_topology_status_cmp_with_curr_connectivity_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint8
    prm_is_topology_valid;
  FE600_LINKS_TO_DEST_FAP
    prm_p_fault_map;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_links_topology");
  soc_sand_proc_name = "fe600_topology_status_cmp_with_curr_connectivity_get";

  unit = fe600_get_default_unit();
  fe600_FE600_LINKS_TO_DEST_FAP_clear(&prm_p_fault_map);

  /* Get parameters */

  /* Call function */
  ret = fe600_topology_status_cmp_with_curr_connectivity_get(
          unit,
          &prm_is_topology_valid,
          &prm_p_fault_map
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_topology_status_cmp_with_curr_connectivity_get - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_topology_status_cmp_with_curr_connectivity_get");
    goto exit;
  }

  send_string_to_screen(" ---> fe600_topology_status_cmp_with_curr_connectivity_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> is_topology_valid:", TRUE);
  soc_sand_os_printf( "is_topology_valid: %u\n\r",prm_is_topology_valid);

  send_string_to_screen("--> p_fault_map:", TRUE);
  fe600_FE600_LINKS_TO_DEST_FAP_print(&prm_p_fault_map);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: topology_status_connectivity_get (section links_topology)
 */
int
  ui_fe600_api_links_topology_topology_status_connectivity_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  FE600_LINKS_CON_STAT_INFO_ARR
    prm_connectivity_map;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_links_topology");
  soc_sand_proc_name = "fe600_topology_status_connectivity_get";

  unit = fe600_get_default_unit();
  fe600_FE600_LINKS_CON_STAT_INFO_ARR_clear(&prm_connectivity_map);

  /* Get parameters */

  /* Call function */
  ret = fe600_topology_status_connectivity_get(
          unit,
          &prm_connectivity_map
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_topology_status_connectivity_get - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_topology_status_connectivity_get");
    goto exit;
  }

  send_string_to_screen(" ---> fe600_topology_status_connectivity_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> connectivity_map:", TRUE);
  fe600_topology_status_connectivity_print(&prm_connectivity_map);


  goto exit;
exit:
  return ui_ret;
}

#endif
#ifdef UI_FE600_LINKS
/********************************************************************
 *  Function handler: srd_qrtt_reset (section fabric)
 */
int
  ui_fe600_api_fabric_srd_qrtt_reset(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_srd_qrtt_ndx;
  uint8
    prm_is_oor;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_fabric");
  soc_sand_proc_name = "fe600_fabric_srd_qrtt_reset";

  unit = fe600_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_FABRIC_SRD_QRTT_RESET_SRD_QRTT_RESET_SRD_QRTT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_FABRIC_SRD_QRTT_RESET_SRD_QRTT_RESET_SRD_QRTT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_srd_qrtt_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter srd_qrtt_ndx after srd_qrtt_reset***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_FABRIC_SRD_QRTT_RESET_SRD_QRTT_RESET_IS_OOR_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_FABRIC_SRD_QRTT_RESET_SRD_QRTT_RESET_IS_OOR_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_is_oor = (uint8)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter srd_qrtt_ndx after srd_qrtt_reset***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = fe600_links_srd_qrtt_reset(
          unit,
          prm_srd_qrtt_ndx,
          prm_is_oor
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_fabric_srd_qrtt_reset - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_fabric_srd_qrtt_reset");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: reset_set (section links)
 */
int
  ui_fe600_api_link_on_off_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  FE600_LINK_ID
    prm_link_ndx = 0;
  FE600_LINK_STATE_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_links");
  soc_sand_proc_name = "fe600_link_on_off_set";

  unit = fe600_get_default_unit();
  fe600_FE600_LINK_STATE_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_LINKS_RESET_SET_RESET_SET_LINK_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_LINKS_RESET_SET_RESET_SET_LINK_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_link_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter link_ndx after reset_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = fe600_link_on_off_get(
          unit,
          prm_link_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_link_on_off_get - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_link_on_off_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_LINKS_RESET_SET_RESET_SET_IS_ENABLED_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_LINKS_RESET_SET_RESET_SET_IS_ENABLED_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.serdes_also = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_LINK_ON_OFF_SET_LINK_ON_OFF_SET_INFO_ON_OFF_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FE600_LINK_ON_OFF_SET_LINK_ON_OFF_SET_INFO_ON_OFF_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.on_off = param_val->numeric_equivalent;
  }

  /* Call function */
  ret = fe600_link_on_off_set(
          unit,
          prm_link_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_link_on_off_set - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_link_on_off_set");
    goto exit;
  }

  send_string_to_screen(" ---> fe600_link_on_off_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: reset_get (section links)
 */
int
  ui_fe600_api_link_on_off_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  FE600_LINK_ID
    prm_link_ndx;
  FE600_LINK_STATE_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_links");
  soc_sand_proc_name = "fe600_link_on_off_get";

  unit = fe600_get_default_unit();
  fe600_FE600_LINK_STATE_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_LINKS_RESET_GET_RESET_GET_LINK_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_LINKS_RESET_GET_RESET_GET_LINK_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_link_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter link_ndx after reset_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = fe600_link_on_off_get(
          unit,
          prm_link_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_link_on_off_get - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_link_on_off_get");
    goto exit;
  }

  send_string_to_screen(" ---> fe600_link_on_off_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> info:", TRUE);
  fe600_FE600_LINK_STATE_INFO_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: is_tdm_only_set (section links)
 */
int
  ui_fe600_api_links_is_tdm_only_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  FE600_LINK_ID
    prm_link_ndx;
  uint8
    prm_is_tdm_only;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_links");
  soc_sand_proc_name = "fe600_links_is_tdm_only_set";

  unit = fe600_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_LINKS_IS_TDM_ONLY_SET_IS_TDM_ONLY_SET_LINK_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_LINKS_IS_TDM_ONLY_SET_IS_TDM_ONLY_SET_LINK_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_link_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter link_ndx after is_tdm_only_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = fe600_links_is_tdm_only_get(
          unit,
          prm_link_ndx,
          &prm_is_tdm_only
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_links_is_tdm_only_get - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_links_is_tdm_only_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_LINKS_IS_TDM_ONLY_SET_IS_TDM_ONLY_SET_IS_TDM_ONLY_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_LINKS_IS_TDM_ONLY_SET_IS_TDM_ONLY_SET_IS_TDM_ONLY_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_is_tdm_only = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = fe600_links_is_tdm_only_set(
          unit,
          prm_link_ndx,
          prm_is_tdm_only
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_links_is_tdm_only_set - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_links_is_tdm_only_set");
    goto exit;
  }

  send_string_to_screen(" ---> fe600_links_is_tdm_only_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: is_tdm_only_get (section links)
 */
int
  ui_fe600_api_links_is_tdm_only_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  FE600_LINK_ID
    prm_link_ndx;
  uint8
    prm_is_tdm_only;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_links");
  soc_sand_proc_name = "fe600_links_is_tdm_only_get";

  unit = fe600_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_LINKS_IS_TDM_ONLY_GET_IS_TDM_ONLY_GET_LINK_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_LINKS_IS_TDM_ONLY_GET_IS_TDM_ONLY_GET_LINK_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_link_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter link_ndx after is_tdm_only_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = fe600_links_is_tdm_only_get(
          unit,
          prm_link_ndx,
          &prm_is_tdm_only
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_links_is_tdm_only_get - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_links_is_tdm_only_get");
    goto exit;
  }

  send_string_to_screen(" ---> fe600_links_is_tdm_only_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> is_tdm_only:", TRUE);
  soc_sand_os_printf( "is_tdm_only: %u\n\r",prm_is_tdm_only);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: link_level_fc_enable_set (section links)
 */
int
  ui_fe600_api_links_link_level_fc_enable_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  FE600_LINK_ID
    prm_link_ndx;
  uint8
    prm_is_enabled;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_links");
  soc_sand_proc_name = "fe600_links_link_level_fc_enable_set";

  unit = fe600_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_LINKS_LINK_LEVEL_FC_ENABLE_SET_LINK_LEVEL_FC_ENABLE_SET_LINK_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_LINKS_LINK_LEVEL_FC_ENABLE_SET_LINK_LEVEL_FC_ENABLE_SET_LINK_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_link_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter link_ndx after link_level_fc_enable_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = fe600_links_link_level_fc_enable_get(
          unit,
          prm_link_ndx,
          &prm_is_enabled
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_links_link_level_fc_enable_get - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_links_link_level_fc_enable_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_LINKS_LINK_LEVEL_FC_ENABLE_SET_LINK_LEVEL_FC_ENABLE_SET_IS_ENABLED_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_LINKS_LINK_LEVEL_FC_ENABLE_SET_LINK_LEVEL_FC_ENABLE_SET_IS_ENABLED_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_is_enabled = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = fe600_links_link_level_fc_enable_set(
          unit,
          prm_link_ndx,
          prm_is_enabled
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_links_link_level_fc_enable_set - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_links_link_level_fc_enable_set");
    goto exit;
  }

  send_string_to_screen(" ---> fe600_links_link_level_fc_enable_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: link_level_fc_enable_get (section links)
 */
int
  ui_fe600_api_links_link_level_fc_enable_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  FE600_LINK_ID
    prm_link_ndx;
  uint8
    prm_is_enabled;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_links");
  soc_sand_proc_name = "fe600_links_link_level_fc_enable_get";

  unit = fe600_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_LINKS_LINK_LEVEL_FC_ENABLE_GET_LINK_LEVEL_FC_ENABLE_GET_LINK_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_LINKS_LINK_LEVEL_FC_ENABLE_GET_LINK_LEVEL_FC_ENABLE_GET_LINK_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_link_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter link_ndx after link_level_fc_enable_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = fe600_links_link_level_fc_enable_get(
          unit,
          prm_link_ndx,
          &prm_is_enabled
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_links_link_level_fc_enable_get - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_links_link_level_fc_enable_get");
    goto exit;
  }

  send_string_to_screen(" ---> fe600_links_link_level_fc_enable_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> is_enabled:", TRUE);
  soc_sand_os_printf( "is_enabled: %u\n\r",prm_is_enabled);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: fec_enable_set (section links)
 */
int
  ui_fe600_api_links_fec_enable_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  FE600_LINK_ID
    prm_link_ndx;
  uint8
    prm_is_enabled;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_links");
  soc_sand_proc_name = "fe600_links_fec_enable_set";

  unit = fe600_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_LINKS_FEC_ENABLE_SET_FEC_ENABLE_SET_LINK_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_LINKS_FEC_ENABLE_SET_FEC_ENABLE_SET_LINK_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_link_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter link_ndx after fec_enable_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = fe600_links_fec_enable_get(
          unit,
          prm_link_ndx,
          &prm_is_enabled
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_links_fec_enable_get - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_links_fec_enable_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_LINKS_FEC_ENABLE_SET_FEC_ENABLE_SET_IS_ENABLED_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_LINKS_FEC_ENABLE_SET_FEC_ENABLE_SET_IS_ENABLED_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_is_enabled = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = fe600_links_fec_enable_set(
          unit,
          prm_link_ndx,
          prm_is_enabled
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_links_fec_enable_set - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_links_fec_enable_set");
    goto exit;
  }

  send_string_to_screen(" ---> fe600_links_fec_enable_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: fec_enable_get (section links)
 */
int
  ui_fe600_api_links_fec_enable_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  FE600_LINK_ID
    prm_link_ndx;
  uint8
    prm_is_enabled;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_links");
  soc_sand_proc_name = "fe600_links_fec_enable_get";

  unit = fe600_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_LINKS_FEC_ENABLE_GET_FEC_ENABLE_GET_LINK_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_LINKS_FEC_ENABLE_GET_FEC_ENABLE_GET_LINK_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_link_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter link_ndx after fec_enable_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = fe600_links_fec_enable_get(
          unit,
          prm_link_ndx,
          &prm_is_enabled
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_links_fec_enable_get - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_links_fec_enable_get");
    goto exit;
  }

  send_string_to_screen(" ---> fe600_links_fec_enable_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> is_enabled:", TRUE);
  soc_sand_os_printf( "is_enabled: %u\n\r",prm_is_enabled);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: type_set (section links)
 */
int
  ui_fe600_api_links_type_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  FE600_LINK_ID
    prm_link_ndx;
  FE600_LINK_TYPE
    prm_link_type;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_links");
  soc_sand_proc_name = "fe600_links_type_set";

  unit = fe600_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_LINKS_TYPE_SET_TYPE_SET_LINK_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_LINKS_TYPE_SET_TYPE_SET_LINK_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_link_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter link_ndx after type_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = fe600_links_type_get(
          unit,
          prm_link_ndx,
          &prm_link_type
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_links_type_get - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_links_type_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_LINKS_TYPE_SET_TYPE_SET_LINK_TYPE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_LINKS_TYPE_SET_TYPE_SET_LINK_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_link_type = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = fe600_links_type_set(
          unit,
          prm_link_ndx,
          prm_link_type
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_links_type_set - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_links_type_set");
    goto exit;
  }

  send_string_to_screen(" ---> fe600_links_type_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: type_get (section links)
 */
int
  ui_fe600_api_links_type_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  FE600_LINK_ID
    prm_link_ndx;
  FE600_LINK_TYPE
    prm_link_type;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_links");
  soc_sand_proc_name = "fe600_links_type_get";

  unit = fe600_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_LINKS_TYPE_GET_TYPE_GET_LINK_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_LINKS_TYPE_GET_TYPE_GET_LINK_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_link_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter link_ndx after type_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = fe600_links_type_get(
          unit,
          prm_link_ndx,
          &prm_link_type
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_links_type_get - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_links_type_get");
    goto exit;
  }

  send_string_to_screen(" ---> fe600_links_type_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> link_type:", TRUE);
  soc_sand_os_printf( "link_type: %d\n\r",prm_link_type);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: per_type_params_set (section links)
 */
int
  ui_fe600_api_links_per_type_params_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  FE600_LINK_TYPE
    prm_link_type_ndx;
  uint32
    prm_gci_fc_th_index = 0xFFFFFFFF;
  FE600_LINKS_PER_TYPE_PARAMS
    prm_type_params;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_links");
  soc_sand_proc_name = "fe600_links_per_type_params_set";

  unit = fe600_get_default_unit();
  fe600_FE600_LINKS_PER_TYPE_PARAMS_clear(&prm_type_params);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_LINKS_PER_TYPE_PARAMS_SET_PER_TYPE_PARAMS_SET_LINK_TYPE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_LINKS_PER_TYPE_PARAMS_SET_PER_TYPE_PARAMS_SET_LINK_TYPE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_link_type_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter link_type_ndx after per_type_params_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = fe600_links_per_type_params_get(
          unit,
          prm_link_type_ndx,
          &prm_type_params
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_links_per_type_params_get - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_links_per_type_params_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_LINKS_PER_TYPE_PARAMS_SET_PER_TYPE_PARAMS_SET_TYPE_PARAMS_GCI_FC_TH_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_LINKS_PER_TYPE_PARAMS_SET_PER_TYPE_PARAMS_SET_TYPE_PARAMS_GCI_FC_TH_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_gci_fc_th_index = (uint32)param_val->value.ulong_value;
  }

  if(prm_gci_fc_th_index != 0xFFFFFFFF)
  {

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_LINKS_PER_TYPE_PARAMS_SET_PER_TYPE_PARAMS_SET_TYPE_PARAMS_GCI_FC_TH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_LINKS_PER_TYPE_PARAMS_SET_PER_TYPE_PARAMS_SET_TYPE_PARAMS_GCI_FC_TH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_type_params.gci_fc_th[ prm_gci_fc_th_index] = (uint32)param_val->value.ulong_value;
  }

  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_LINKS_PER_TYPE_PARAMS_SET_PER_TYPE_PARAMS_SET_TYPE_PARAMS_RCI_FC_TH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_LINKS_PER_TYPE_PARAMS_SET_PER_TYPE_PARAMS_SET_TYPE_PARAMS_RCI_FC_TH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_type_params.rci_fc_th = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_LINKS_PER_TYPE_PARAMS_SET_PER_TYPE_PARAMS_SET_TYPE_PARAMS_LL_FC_TH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_LINKS_PER_TYPE_PARAMS_SET_PER_TYPE_PARAMS_SET_TYPE_PARAMS_LL_FC_TH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_type_params.link_level_fc_th = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_LINKS_PER_TYPE_PARAMS_SET_PER_TYPE_PARAMS_SET_TYPE_PARAMS_DROP_NON_TDM_TH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_LINKS_PER_TYPE_PARAMS_SET_PER_TYPE_PARAMS_SET_TYPE_PARAMS_DROP_NON_TDM_TH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_type_params.drop_non_tdm_th = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_LINKS_PER_TYPE_PARAMS_SET_PER_TYPE_PARAMS_SET_TYPE_PARAMS_ALMOST_FULL_TH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_LINKS_PER_TYPE_PARAMS_SET_PER_TYPE_PARAMS_SET_TYPE_PARAMS_ALMOST_FULL_TH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_type_params.almost_full_th = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = fe600_links_per_type_params_set(
          unit,
          prm_link_type_ndx,
          &prm_type_params
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_links_per_type_params_set - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_links_per_type_params_set");
    goto exit;
  }

  send_string_to_screen(" ---> fe600_links_per_type_params_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: per_type_params_get (section links)
 */
int
  ui_fe600_api_links_per_type_params_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  FE600_LINK_TYPE
    prm_link_type_ndx;
  FE600_LINKS_PER_TYPE_PARAMS
    prm_type_params;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_links");
  soc_sand_proc_name = "fe600_links_per_type_params_get";

  unit = fe600_get_default_unit();
  fe600_FE600_LINKS_PER_TYPE_PARAMS_clear(&prm_type_params);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_LINKS_PER_TYPE_PARAMS_GET_PER_TYPE_PARAMS_GET_LINK_TYPE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_LINKS_PER_TYPE_PARAMS_GET_PER_TYPE_PARAMS_GET_LINK_TYPE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_link_type_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter link_type_ndx after per_type_params_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = fe600_links_per_type_params_get(
          unit,
          prm_link_type_ndx,
          &prm_type_params
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_links_per_type_params_get - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_links_per_type_params_get");
    goto exit;
  }

  send_string_to_screen(" ---> fe600_links_per_type_params_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> type_params:", TRUE);
  fe600_FE600_LINKS_PER_TYPE_PARAMS_print(&prm_type_params);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: status_get (section links)
 */
int
  ui_fe600_api_links_status_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  FE600_LINKS_STATUS_ALL
    prm_links_status;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_links");
  soc_sand_proc_name = "fe600_links_status_get";

  unit = fe600_get_default_unit();
  fe600_FE600_LINKS_STATUS_ALL_clear(&prm_links_status);

  /* Get parameters */

  /* Call function */
  ret = fe600_links_status_get(
          unit,
          TRUE,
          &prm_links_status
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_links_status_get - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_links_status_get");
    goto exit;
  }

  send_string_to_screen(" ---> fe600_links_status_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> links_status:", TRUE);
  fe600_FE600_LINKS_STATUS_ALL_print(&prm_links_status, SOC_SAND_PRINT_FLAVORS_SHORT);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: status_get (section links)
 */
int
  ui_fe600_links_accept_cell_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint8
    accepting_cells;
  FE600_LINK_ID
    prm_link_ndx;

  UI_MACROS_INIT_FUNCTION("ui_fe600_links_accept_cell_get");
  soc_sand_proc_name = "fe600_links_accept_cell_get";

  unit = fe600_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAN_FE600_LINKS_ACCEPT_CELLS_GET_LINK_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAN_FE600_LINKS_ACCEPT_CELLS_GET_LINK_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_link_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter link_ndx after reset_set***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = fe600_links_accept_cells_get(
          unit,
          prm_link_ndx,
          &accepting_cells
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_links_accept_cells_get - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_links_accept_cells_get");
    goto exit;
  }

  soc_sand_os_printf( "--> accepting_cells: %d\n\r", accepting_cells);

  goto exit;
exit:
  return ui_ret;
}
  
/******************************************************************** 
 *  Function handler: q_max_occupancy_get (section links)
 */
int 
  ui_fe600_api_links_q_max_occupancy_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  FE600_LINKS_Q_LOCATION   
    prm_q_location;
  FE600_LINKS_Q_OCCUPANCIES   
    prm_q_max_occ;
   
  UI_MACROS_INIT_FUNCTION("ui_fe600_api_links"); 
  soc_sand_proc_name = "fe600_links_q_max_occupancy_get"; 
 
  unit = fe600_get_default_unit(); 
  fe600_FE600_LINKS_Q_LOCATION_clear(&prm_q_location);
  fe600_FE600_LINKS_Q_OCCUPANCIES_clear(&prm_q_max_occ);
  prm_q_location.is_dcl_enabled = TRUE;
  prm_q_location.is_dch_enabled = FALSE;
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_LINKS_Q_MAX_OCCUPANCY_GET_Q_MAX_OCCUPANCY_GET_Q_LOCATION_IS_DCH_ENABLED_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_LINKS_Q_MAX_OCCUPANCY_GET_Q_MAX_OCCUPANCY_GET_Q_LOCATION_IS_DCH_ENABLED_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_q_location.is_dch_enabled = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_LINKS_Q_MAX_OCCUPANCY_GET_Q_MAX_OCCUPANCY_GET_Q_LOCATION_IS_DCL_ENABLED_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_LINKS_Q_MAX_OCCUPANCY_GET_Q_MAX_OCCUPANCY_GET_Q_LOCATION_IS_DCL_ENABLED_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_q_location.is_dcl_enabled = (uint8)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = fe600_links_q_max_occupancy_get(
          unit,
          &prm_q_location,
          &prm_q_max_occ
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** fe600_links_q_max_occupancy_get - FAIL", TRUE); 
    fe600_disp_result(ret, "fe600_links_q_max_occupancy_get");   
    goto exit; 
  } 

  soc_sand_os_printf( "[Link id, Queue size]:\n\r");
  fe600_FE600_LINKS_Q_OCCUPANCIES_print(
    &prm_q_location,
    &prm_q_max_occ
  );

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
#endif

#ifdef UI_FE600_CELL
/********************************************************************
*  Function handler: fe600_read_from_fe600 (section cell)
 */
int
ui_fe600_api_cell_fe600_read_from_fe600(
  CURRENT_LINE *current_line
  )
{
  uint32
    indx = 0,
    param_i = 0,
    offset ,
    data_out = 0 ,
    ret;
  FE600_SR_CELL_LINK_LIST
    sr_link_list ;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_cell");
  soc_sand_proc_name = "fe600_read_from_fe600";

  unit = fe600_get_default_unit();

  for (indx = 0; indx < FE600_CELL_NOF_LINKS_IN_PATH_LINKS; ++indx)
  {
    sr_link_list.path_links[indx] = 0;
  }

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_CELL_READ_DIRECT_SRC_ENTITY_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FE600_CELL_READ_DIRECT_SRC_ENTITY_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    sr_link_list.src_entity_type = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter dest_entity_type after fe600_read_from_fe600*** ", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_CELL_READ_DIRECT_DEST_ENTITY_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FE600_CELL_READ_DIRECT_DEST_ENTITY_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    sr_link_list.dest_entity_type = param_val->numeric_equivalent;
  }

  for (param_i=1; param_i<=FE600_CELL_NOF_LINKS_IN_PATH_LINKS; ++param_i)
  {
    ret = search_param_val_pairs(current_line,&match_index,PARAM_FE600_CELL_READ_DIRECT_LINK_FIP_TO_FE_ID,param_i);
    if (0 != ret)
    {
      ret=0;
      break;
    }
    UI_MACROS_GET_NUMMERIC_VAL_WITH_INDEX(PARAM_FE600_CELL_READ_DIRECT_LINK_FIP_TO_FE_ID,param_i);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    sr_link_list.path_links[param_i - 1] = (uint8) param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_CELL_READ_DIRECT_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_CELL_READ_DIRECT_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter srd_qrtt_ndx after srd_qrtt_reset***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = fe600_read_from_fe600(
          unit,
          &sr_link_list,
          sizeof(uint32),
          offset,
          &data_out
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_read_from_fe600 - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_read_from_fe600");
    goto exit;
  }

  soc_sand_os_printf(  "data_out: %lx\n\r" , data_out ) ;


exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: fe600_write_to_fe600 (section cell)
 */
int
ui_fe600_api_cell_fe600_write_to_fe600(
  CURRENT_LINE *current_line
  )
{
  uint32
    indx = 0,
    param_i = 0,
    offset = 0,
    data_in = 0,
    ret = 0;
  FE600_SR_CELL_LINK_LIST
    sr_link_list ;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_cell");
  soc_sand_proc_name = "fe600_write_to_fe600";

  unit = fe600_get_default_unit();

  for (indx = 0; indx < FE600_CELL_NOF_LINKS_IN_PATH_LINKS; ++indx)
  {
    sr_link_list.path_links[indx] = 0;
  }

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_CELL_WRITE_DIRECT_SRC_ENTITY_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FE600_CELL_WRITE_DIRECT_SRC_ENTITY_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    sr_link_list.src_entity_type = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter dest_entity_type after fe600_write_to_fe600*** ", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_CELL_WRITE_DIRECT_DEST_ENTITY_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FE600_CELL_WRITE_DIRECT_DEST_ENTITY_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    sr_link_list.dest_entity_type = param_val->numeric_equivalent;
  }

  for (param_i=1; param_i<=FE600_CELL_NOF_LINKS_IN_PATH_LINKS; ++param_i)
  {
    ret = search_param_val_pairs(current_line,&match_index,PARAM_FE600_CELL_WRITE_DIRECT_LINK_FIP_TO_FE_ID,param_i);
    if (0 != ret)
    {
      ret=0;
      break;
    }
    UI_MACROS_GET_NUMMERIC_VAL_WITH_INDEX(PARAM_FE600_CELL_WRITE_DIRECT_LINK_FIP_TO_FE_ID,param_i);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    sr_link_list.path_links[param_i - 1] = (uint8) param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_CELL_WRITE_DIRECT_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_CELL_WRITE_DIRECT_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_CELL_WRITE_DIRECT_DATA_IN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_CELL_WRITE_DIRECT_DATA_IN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data_in = (uint32) param_val->value.ulong_value;
  }


  /* Call function */
  ret = fe600_write_to_fe600(
          unit,
          &sr_link_list,
          sizeof(uint32),
          offset,
          &data_in
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_write_to_fe600 - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_write_to_fe600");
    goto exit;
  }

exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: fe600_indirect_read_from_fe600 (section cell)
 */
int
ui_fe600_api_cell_fe600_indirect_read_from_fe600(
  CURRENT_LINE *current_line
  )
{
  uint32
    indx = 0,
    param_i = 0,
    offset ,
    data_out[FE600_CELL_NOF_DATA_WORDS_IN_INDIRECT_CELL] = {0,0,0} ,
    ret;
  FE600_SR_CELL_LINK_LIST
    sr_link_list ;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_cell");
  soc_sand_proc_name = "fe600_indirect_read_from_fe600";

  unit = fe600_get_default_unit();

  for (indx = 0; indx < FE600_CELL_NOF_LINKS_IN_PATH_LINKS; ++indx)
  {
    sr_link_list.path_links[indx] = 0;
  }

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_CELL_READ_INDIRECT_SRC_ENTITY_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FE600_CELL_READ_INDIRECT_SRC_ENTITY_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    sr_link_list.src_entity_type = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter dest_entity_type after fe600_indirect_read_from_fe600*** ", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_CELL_READ_INDIRECT_DEST_ENTITY_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FE600_CELL_READ_INDIRECT_DEST_ENTITY_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    sr_link_list.dest_entity_type = param_val->numeric_equivalent;
  }

  for (param_i=1; param_i<=FE600_CELL_NOF_LINKS_IN_PATH_LINKS; ++param_i)
  {
    ret = search_param_val_pairs(current_line,&match_index,PARAM_FE600_CELL_READ_INDIRECT_LINK_FIP_TO_FE_ID,param_i);
    if (0 != ret)
    {
      ret=0;
      break;
    }
    UI_MACROS_GET_NUMMERIC_VAL_WITH_INDEX(PARAM_FE600_CELL_READ_INDIRECT_LINK_FIP_TO_FE_ID,param_i);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    sr_link_list.path_links[param_i - 1] = (uint8) param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_CELL_READ_INDIRECT_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_CELL_READ_INDIRECT_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter srd_qrtt_ndx after srd_qrtt_reset***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = fe600_indirect_read_from_fe600(
          unit,
          &sr_link_list,
          sizeof(uint32),
          offset,
          data_out
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_indirect_read_from_fe600 - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_indirect_read_from_fe600");
    goto exit;
  }

  soc_sand_os_printf(  "data_out[0]: %lx\n\r" , data_out[0] ) ;
  soc_sand_os_printf(  "data_out[1]: %lx\n\r" , data_out[1] ) ;
  soc_sand_os_printf(  "data_out[2]: %lx\n\r" , data_out[2] ) ;


exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: fe600_indirect_write_to_fe600 (section cell)
 */
int
ui_fe600_api_cell_fe600_indirect_write_to_fe600(
  CURRENT_LINE *current_line
  )
{
  uint32
    offset ,
    indx = 0,
    param_i = 0,
    data_in ,
    ret;
  FE600_SR_CELL_LINK_LIST
    sr_link_list ;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_cell");
  soc_sand_proc_name = "fe600_indirect_write_from_fe600";

  unit = fe600_get_default_unit();

  for (indx = 0; indx < FE600_CELL_NOF_LINKS_IN_PATH_LINKS; ++indx)
  {
    sr_link_list.path_links[indx] = 0;
  }

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_CELL_WRITE_INDIRECT_SRC_ENTITY_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FE600_CELL_WRITE_INDIRECT_SRC_ENTITY_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    sr_link_list.src_entity_type = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter dest_entity_type after fe600_indirect_write_from_fe600*** ", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_CELL_WRITE_INDIRECT_DEST_ENTITY_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FE600_CELL_WRITE_INDIRECT_DEST_ENTITY_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    sr_link_list.dest_entity_type = param_val->numeric_equivalent;
  }

  for (param_i=1; param_i<=FE600_CELL_NOF_LINKS_IN_PATH_LINKS; ++param_i)
  {
    ret = search_param_val_pairs(current_line,&match_index,PARAM_FE600_CELL_WRITE_INDIRECT_LINK_FIP_TO_FE_ID,param_i);
    if (0 != ret)
    {
      ret=0;
      break;
    }
    UI_MACROS_GET_NUMMERIC_VAL_WITH_INDEX(PARAM_FE600_CELL_WRITE_INDIRECT_LINK_FIP_TO_FE_ID,param_i);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    sr_link_list.path_links[param_i - 1] = (uint8) param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_CELL_WRITE_INDIRECT_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_CELL_WRITE_INDIRECT_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset = param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter srd_qrtt_ndx after srd_qrtt_reset***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_CELL_WRITE_INDIRECT_DATA_IN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_CELL_WRITE_INDIRECT_DATA_IN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data_in = param_val->value.ulong_value;
  }


  /* Call function */
  ret = fe600_indirect_write_to_fe600(
          unit,
          &sr_link_list,
          sizeof(uint32),
          offset,
          &data_in
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_indirect_write_to_fe600 - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_indirect_write_to_fe600");
    goto exit;
  }

exit:
  return ui_ret;
}


/********************************************************************
*  Function handler: fe600_cpu2cpu_write (section cell)
 */
int
  ui_fe600_api_cell_fe600_cpu2cpu_write(
    CURRENT_LINE *current_line
  )
{
  uint32
    param_i,
    data_in[SOC_SAND_DATA_CELL_PAYLOAD_IN_UINT32S],
    indx = 0,
    ret;
  FE600_SR_CELL_LINK_LIST
    sr_link_list ;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_cell");
  soc_sand_proc_name = "fe600_cpu2cpu_write";

  unit = fe600_get_default_unit();

  /* Initialization */
  for (indx = 0; indx < SOC_SAND_DATA_CELL_PAYLOAD_IN_UINT32S; ++indx)
  {
    data_in[indx] = 0;
  }
  for (indx = 0; indx < FE600_CELL_NOF_LINKS_IN_PATH_LINKS; ++indx)
  {
    sr_link_list.path_links[indx] = 0;
  }

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_CELL_CPU2CPU_SRC_ENTITY_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FE600_CELL_CPU2CPU_SRC_ENTITY_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    sr_link_list.src_entity_type = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter dest_entity_type after fe600_read_from_fe600*** ", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_CELL_CPU2CPU_DEST_ENTITY_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FE600_CELL_CPU2CPU_DEST_ENTITY_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    sr_link_list.dest_entity_type = param_val->numeric_equivalent;
  }


  for (param_i=1; param_i<=FE600_CELL_NOF_LINKS_IN_PATH_LINKS; ++param_i)
  {
    ret = search_param_val_pairs(current_line,&match_index,PARAM_FE600_CELL_CPU2CPU_LINK_FIP_TO_FE_ID,param_i);
    if (0 != ret)
    {
      ret=0;
      break;
    }
    UI_MACROS_GET_NUMMERIC_VAL_WITH_INDEX(PARAM_FE600_CELL_CPU2CPU_LINK_FIP_TO_FE_ID,param_i);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    sr_link_list.path_links[param_i - 1] = (uint8) param_val->value.ulong_value;
  }

  for (param_i=1; param_i<=SOC_SAND_DATA_CELL_PAYLOAD_IN_UINT32S; ++param_i)
  {
    ret = search_param_val_pairs(current_line,&match_index,PARAM_FE600_CELL_CPU2CPU_DATA_IN_ID,param_i);
    if (0 != ret)
    {
      ret=0;
      break;
    }
    UI_MACROS_GET_NUMMERIC_VAL_WITH_INDEX(PARAM_FE600_CELL_CPU2CPU_DATA_IN_ID,param_i);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data_in[param_i - 1] = (uint32) param_val->value.ulong_value;
  }


  /* Call function */
  ret = fe600_cpu2cpu_write(
          unit,
          &sr_link_list,
          SOC_SAND_DATA_CELL_PAYLOAD_IN_BYTES,
          data_in
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_cpu2cpu_write - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_cpu2cpu_write");
    goto exit;
  }

exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: fe600_cpu2cpu_read (section cell)
 */
int
  ui_fe600_api_cell_fe600_cpu2cpu_read(
    CURRENT_LINE *current_line
  )
{
  uint32
    data_out[SOC_SAND_DATA_CELL_PAYLOAD_IN_UINT32S],
    indx = 0,
    ret;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_cell");
  soc_sand_proc_name = "fe600_cpu2cpu_write";

  unit = fe600_get_default_unit();

  /* Initialization */
  for (indx = 0; indx < SOC_SAND_DATA_CELL_PAYLOAD_IN_UINT32S; ++indx)
  {
    data_out[indx] = 0;
  }

  /* Call function */
  ret = fe600_cpu2cpu_read(
          unit,
          data_out
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_cpu2cpu_read - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_cpu2cpu_read");
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
 *  Function handler: snake_test_init (section cell)
 */
int
  ui_fe600_api_cell_snake_test_init(
    CURRENT_LINE *current_line
  )
{
  uint32
    ind,
    ret;
  uint32
    prm_is_link_enabled_index = 0xFFFFFFFF;
  FE600_CELL_LINK_BITMAP
    prm_enabled_links;
  uint8
    prm_is_loopback_ext;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_cell");
  soc_sand_proc_name = "fe600_cell_snake_test_init";

  unit = fe600_get_default_unit();
  fe600_FE600_CELL_LINK_BITMAP_clear(&prm_enabled_links);
  for (ind=0; ind<FE600_SRD_NOF_LANES; ++ind)
  {
    prm_enabled_links.is_link_enabled[ind] = TRUE;
  }

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_CELL_SNAKE_TEST_INIT_SNAKE_TEST_INIT_ENABLED_LINKS_ENABLED_LINK_BITMAP_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_CELL_SNAKE_TEST_INIT_SNAKE_TEST_INIT_ENABLED_LINKS_ENABLED_LINK_BITMAP_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_is_link_enabled_index = (uint32)param_val->value.ulong_value;
  }

  if(prm_is_link_enabled_index != 0xFFFFFFFF)
  {
    if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_CELL_SNAKE_TEST_INIT_SNAKE_TEST_INIT_ENABLED_LINKS_ENABLED_LINK_BITMAP_ID,1))
    {
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_CELL_SNAKE_TEST_INIT_SNAKE_TEST_INIT_ENABLED_LINKS_ENABLED_LINK_BITMAP_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      prm_enabled_links.is_link_enabled[ prm_is_link_enabled_index] = (uint8)param_val->value.ulong_value;
    }
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_CELL_SNAKE_TEST_INIT_SNAKE_TEST_INIT_IS_LOOPBACK_EXT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_CELL_SNAKE_TEST_INIT_SNAKE_TEST_INIT_IS_LOOPBACK_EXT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_is_loopback_ext = (uint8)param_val->value.ulong_value;
  }

  /* Call function */
  ret = fe600_cell_snake_test_init(
          unit,
          &prm_enabled_links,
          prm_is_loopback_ext
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_cell_snake_test_init - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_cell_snake_test_init");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: snake_test_rate_get (section cell)
 */
int
  ui_fe600_api_cell_snake_test_rate_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    link_ndx,
    ret;
  uint32
    prm_rate_mbps_glbl;
  FE600_CELL_LINK_STATUS
    prm_link_status;
  uint8
    prm_test_error;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_cell");
  soc_sand_proc_name = "fe600_cell_snake_test_rate_get";

  unit = fe600_get_default_unit();

  fe600_FE600_CELL_LINK_STATUS_clear(&prm_link_status);

  /* Get parameters */

  /* Call function */
  ret = fe600_cell_snake_test_rate_get(
          unit,
          &prm_rate_mbps_glbl,
          &prm_link_status,
          &prm_test_error
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_cell_snake_test_rate_get - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_cell_snake_test_rate_get");
    goto exit;
  }

  send_string_to_screen("--> rate_mbps_glbl:", TRUE);
  soc_sand_os_printf( "rate_mbps_glbl: %lu\n\r",prm_rate_mbps_glbl);

  send_string_to_screen("-->prm_link_status: \n\r", TRUE);
/*  fe600_FE600_CELL_LINK_STATUS_print(&prm_link_status); */
  for (link_ndx=0; link_ndx < FE600_SRD_NOF_LANES; ++link_ndx)
  {
    if (prm_link_status.is_traffic_pass[link_ndx] != TRUE)
    {
      soc_sand_os_printf( "is_traffic_pass[%u]: %lu\n\r",link_ndx,prm_link_status.is_traffic_pass[link_ndx]);
    }
  }

  for (link_ndx=0; link_ndx < FE600_SRD_NOF_LANES; ++link_ndx)
  {
    if (prm_link_status.is_crc_errors[link_ndx] == TRUE)
    {
      soc_sand_os_printf( "is_crc_errors[%u]: %lu\n\r",link_ndx,prm_link_status.is_crc_errors[link_ndx]);
    }
  }

  send_string_to_screen("--> test_error:", TRUE);
  soc_sand_os_printf( "test_error: %u\n\r",prm_test_error);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: snake_test_stop (section cell)
 */
int
  ui_fe600_api_cell_snake_test_stop(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_cell");
  soc_sand_proc_name = "fe600_cell_snake_test_stop";

  unit = fe600_get_default_unit();

  /* Get parameters */

  /* Call function */
  ret = fe600_cell_snake_test_stop(
          unit
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_cell_snake_test_stop - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_cell_snake_test_stop");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

#endif

#ifdef UI_FE600_MULTICAST
/********************************************************************
 *  Function handler: congestion_info_set (section multicast)
 */
int
  ui_fe600_api_multicast_congestion_info_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  FE600_MUTICAST_CONGESTION_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_multicast");
  soc_sand_proc_name = "fe600_multicast_congestion_info_set";

  unit = fe600_get_default_unit();
  fe600_FE600_MUTICAST_CONGESTION_INFO_clear(&prm_info);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = fe600_multicast_congestion_info_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_multicast_congestion_info_get - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_multicast_congestion_info_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_MULTICAST_CONGESTION_INFO_SET_CONGESTION_INFO_SET_INFO_MCI1_TH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_MULTICAST_CONGESTION_INFO_SET_CONGESTION_INFO_SET_INFO_MCI1_TH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.mci1_th = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_MULTICAST_CONGESTION_INFO_SET_CONGESTION_INFO_SET_INFO_MCI0_TH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_MULTICAST_CONGESTION_INFO_SET_CONGESTION_INFO_SET_INFO_MCI0_TH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.mci0_th = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_MULTICAST_CONGESTION_INFO_SET_CONGESTION_INFO_SET_INFO_ENABLE_GCI_GENERATION_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_MULTICAST_CONGESTION_INFO_SET_CONGESTION_INFO_SET_INFO_ENABLE_GCI_GENERATION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.enable_gci_generation = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_MULTICAST_CONGESTION_INFO_SET_CONGESTION_INFO_SET_INFO_ENABLE_MCI_GENERATION_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_MULTICAST_CONGESTION_INFO_SET_CONGESTION_INFO_SET_INFO_ENABLE_MCI_GENERATION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.enable_mci_generation = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_MULTICAST_CONGESTION_INFO_SET_CONGESTION_INFO_SET_INFO_ENABLE_PASS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_MULTICAST_CONGESTION_INFO_SET_CONGESTION_INFO_SET_INFO_ENABLE_PASS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.enable_pass = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = fe600_multicast_congestion_info_set(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_multicast_congestion_info_set - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_multicast_congestion_info_set");
    goto exit;
  }

  send_string_to_screen(" ---> fe600_multicast_congestion_info_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: congestion_info_get (section multicast)
 */
int
  ui_fe600_api_multicast_congestion_info_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  FE600_MUTICAST_CONGESTION_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_multicast");
  soc_sand_proc_name = "fe600_multicast_congestion_info_get";

  unit = fe600_get_default_unit();
  fe600_FE600_MUTICAST_CONGESTION_INFO_clear(&prm_info);

  /* Get parameters */

  /* Call function */
  ret = fe600_multicast_congestion_info_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_multicast_congestion_info_get - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_multicast_congestion_info_get");
    goto exit;
  }

  send_string_to_screen(" ---> fe600_multicast_congestion_info_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> info:", TRUE);
  fe600_FE600_MUTICAST_CONGESTION_INFO_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: low_prio_set (section multicast)
 */
int
  ui_fe600_api_multicast_low_prio_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  FE600_MUTICAST_LOW_PRIO_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_multicast");
  soc_sand_proc_name = "fe600_multicast_low_prio_set";

  unit = fe600_get_default_unit();
  fe600_FE600_MUTICAST_LOW_PRIO_INFO_clear(&prm_info);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = fe600_multicast_low_prio_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_multicast_low_prio_get - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_multicast_low_prio_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_MULTICAST_LOW_PRIO_SET_LOW_PRIO_SET_INFO_LOW_PRIO_MULT_TH_DOWN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_MULTICAST_LOW_PRIO_SET_LOW_PRIO_SET_INFO_LOW_PRIO_MULT_TH_DOWN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.low_prio_mult_th_down = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_MULTICAST_LOW_PRIO_SET_LOW_PRIO_SET_INFO_LOW_PRIO_MULT_TH_UP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_MULTICAST_LOW_PRIO_SET_LOW_PRIO_SET_INFO_LOW_PRIO_MULT_TH_UP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.low_prio_mult_th_up = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_MULTICAST_LOW_PRIO_SET_LOW_PRIO_SET_INFO_LAST_MID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_MULTICAST_LOW_PRIO_SET_LOW_PRIO_SET_INFO_LAST_MID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.last_mid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_MULTICAST_LOW_PRIO_SET_LOW_PRIO_SET_INFO_FIRST_MID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_MULTICAST_LOW_PRIO_SET_LOW_PRIO_SET_INFO_FIRST_MID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.first_mid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_MULTICAST_LOW_PRIO_SET_LOW_PRIO_SET_INFO_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_MULTICAST_LOW_PRIO_SET_LOW_PRIO_SET_INFO_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.enable = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = fe600_multicast_low_prio_set(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_multicast_low_prio_set - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_multicast_low_prio_set");
    goto exit;
  }

  send_string_to_screen(" ---> fe600_multicast_low_prio_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: low_prio_get (section multicast)
 */
int
  ui_fe600_api_multicast_low_prio_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  FE600_MUTICAST_LOW_PRIO_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_multicast");
  soc_sand_proc_name = "fe600_multicast_low_prio_get";

  unit = fe600_get_default_unit();
  fe600_FE600_MUTICAST_LOW_PRIO_INFO_clear(&prm_info);

  /* Get parameters */

  /* Call function */
  ret = fe600_multicast_low_prio_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_multicast_low_prio_get - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_multicast_low_prio_get");
    goto exit;
  }

  send_string_to_screen(" ---> fe600_multicast_low_prio_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> info:", TRUE);
  fe600_FE600_MUTICAST_LOW_PRIO_INFO_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: group_direct_set (section multicast)
 */
int
  ui_fe600_api_multicast_group_direct_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  FE600_MULTICAST_ID
    prm_multicast_id_ndx;
  uint32
    prm_faps_index = 0xFFFFFFFF;
  uint32
    prm_faps[FE600_FAPS_BM_SIZE];

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_multicast");
  soc_sand_proc_name = "fe600_multicast_group_direct_set";

  unit = fe600_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_MULTICAST_GROUP_DIRECT_SET_GROUP_DIRECT_SET_MULTICAST_ID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_MULTICAST_GROUP_DIRECT_SET_GROUP_DIRECT_SET_MULTICAST_ID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_id_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter multicast_id_ndx after group_direct_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = fe600_multicast_group_direct_get(
          unit,
          prm_multicast_id_ndx,
          prm_faps
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_multicast_group_direct_get - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_multicast_group_direct_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_MULTICAST_GROUP_DIRECT_SET_GROUP_DIRECT_SET_FAPS_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_MULTICAST_GROUP_DIRECT_SET_GROUP_DIRECT_SET_FAPS_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_faps_index = (uint32)param_val->value.ulong_value;
  }

  if(prm_faps_index != 0xFFFFFFFF)
  {

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_MULTICAST_GROUP_DIRECT_SET_GROUP_DIRECT_SET_FAPS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_MULTICAST_GROUP_DIRECT_SET_GROUP_DIRECT_SET_FAPS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_faps[prm_faps_index] = (uint32)param_val->value.ulong_value;
  }

  }


  /* Call function */
  ret = fe600_multicast_group_direct_set(
          unit,
          prm_multicast_id_ndx,
          prm_faps
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_multicast_group_direct_set - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_multicast_group_direct_set");
    goto exit;
  }

  send_string_to_screen(" ---> fe600_multicast_group_direct_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: group_direct_get (section multicast)
 */
int
  ui_fe600_api_multicast_group_direct_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  FE600_MULTICAST_ID
    prm_multicast_id_ndx;
  uint32
    prm_faps[FE600_FAPS_BM_SIZE];

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_multicast");
  soc_sand_proc_name = "fe600_multicast_group_direct_get";

  unit = fe600_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_MULTICAST_GROUP_DIRECT_GET_GROUP_DIRECT_GET_MULTICAST_ID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_MULTICAST_GROUP_DIRECT_GET_GROUP_DIRECT_GET_MULTICAST_ID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_id_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter multicast_id_ndx after group_direct_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = fe600_multicast_group_direct_get(
          unit,
          prm_multicast_id_ndx,
          prm_faps
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_multicast_group_direct_get - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_multicast_group_direct_get");
    goto exit;
  }

  send_string_to_screen(" ---> fe600_multicast_group_direct_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> faps:", TRUE);
  soc_sand_os_printf( "faps: 0x%x 0x%x 0x%x\n\r",prm_faps[2],prm_faps[1],prm_faps[0]);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: group_indirect_set (section multicast)
 */
int
  ui_fe600_api_multicast_group_indirect_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  FE600_MULTICAST_ID
    prm_multicast_id_ndx;
  uint32
    prm_ids_index = 0xFFFFFFFF;
  uint32
    prm_ids[FE600_LINKS_BM_SIZE];

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_multicast");
  soc_sand_proc_name = "fe600_multicast_group_indirect_set";

  unit = fe600_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_MULTICAST_GROUP_INDIRECT_SET_GROUP_INDIRECT_SET_MULTICAST_ID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_MULTICAST_GROUP_INDIRECT_SET_GROUP_INDIRECT_SET_MULTICAST_ID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_id_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter multicast_id_ndx after group_indirect_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = fe600_multicast_group_indirect_get(
          unit,
          prm_multicast_id_ndx,
          prm_ids
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_multicast_group_indirect_get - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_multicast_group_indirect_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_MULTICAST_GROUP_INDIRECT_SET_GROUP_INDIRECT_SET_LINKS_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_MULTICAST_GROUP_INDIRECT_SET_GROUP_INDIRECT_SET_LINKS_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ids_index = (uint32)param_val->value.ulong_value;
  }

  if(prm_ids_index != 0xFFFFFFFF)
  {

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_MULTICAST_GROUP_INDIRECT_SET_GROUP_INDIRECT_SET_LINKS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_MULTICAST_GROUP_INDIRECT_SET_GROUP_INDIRECT_SET_LINKS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ids[ prm_ids_index] = (uint32)param_val->value.ulong_value;
  }

  }


  /* Call function */
  ret = fe600_multicast_group_indirect_set(
          unit,
          prm_multicast_id_ndx,
          prm_ids
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_multicast_group_indirect_set - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_multicast_group_indirect_set");
    goto exit;
  }

  send_string_to_screen(" ---> fe600_multicast_group_indirect_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: group_indirect_get (section multicast)
 */
int
  ui_fe600_api_multicast_group_indirect_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  FE600_MULTICAST_ID
    prm_multicast_id_ndx;
  uint32
    prm_ids[FE600_FAPS_BM_SIZE];

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_multicast");
  soc_sand_proc_name = "fe600_multicast_group_indirect_get";

  unit = fe600_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_MULTICAST_GROUP_INDIRECT_GET_GROUP_INDIRECT_GET_MULTICAST_ID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_MULTICAST_GROUP_INDIRECT_GET_GROUP_INDIRECT_GET_MULTICAST_ID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_id_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter multicast_id_ndx after group_indirect_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = fe600_multicast_group_indirect_get(
          unit,
          prm_multicast_id_ndx,
          prm_ids
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_multicast_group_indirect_get - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_multicast_group_indirect_get");
    goto exit;
  }

  send_string_to_screen(" ---> fe600_multicast_group_indirect_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> ids:", TRUE);
  soc_sand_os_printf( "ids: 0x%x 0x%x 0x%x\n\r",prm_ids[2],prm_ids[1],prm_ids[0]);
  fe600_multicast_id_to_links_mapping_print(unit,prm_multicast_id_ndx);

  goto exit;
exit:
  return ui_ret;
}

#endif
#ifdef UI_FE600_STATUS
/********************************************************************
 *  Function handler: counters_and_interrupts_get (section status)
 */
int
  ui_fe600_api_status_counters_and_interrupts_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  FE600_STAT_COUNTER_FLAVOR
    prm_flavor = FE600_STAT_COUNTER_FLAVOR_ALL;
  FE600_STAT_ALL_CNTR_AND_INTERRUPTS
    prm_data;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_status");
  soc_sand_proc_name = "fe600_status_counters_and_interrupts_get";

  unit = fe600_get_default_unit();
  fe600_FE600_STAT_ALL_CNTR_AND_INTERRUPTS_clear(&prm_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_STATUS_COUNTERS_AND_INTERRUPTS_GET_COUNTERS_AND_INTERRUPTS_GET_FLAVOR_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FE600_STATUS_COUNTERS_AND_INTERRUPTS_GET_COUNTERS_AND_INTERRUPTS_GET_FLAVOR_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_flavor = param_val->numeric_equivalent;
  }


  /* Call function */
  ret = fe600_status_counters_and_interrupts_get(
          unit,
          &prm_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_status_counters_and_interrupts_get - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_status_counters_and_interrupts_get");
    goto exit;
  }

  send_string_to_screen(" ---> fe600_status_counters_and_interrupts_get - SUCCEEDED", TRUE);
  ret = fe600_status_counters_and_interrupts_print(
    unit,
    prm_flavor,
    &prm_data
    );


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: stat_regs_dump (section status)
 */
int
  ui_fe600_api_status_stat_regs_dump(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_start=0;
  uint32
    prm_end=0x34ff;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_status");
  soc_sand_proc_name = "fe600_stat_regs_dump";

  unit = fe600_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_STAT_REGS_DUMP_STAT_REGS_DUMP_START_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_STAT_REGS_DUMP_STAT_REGS_DUMP_START_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_STAT_REGS_DUMP_STAT_REGS_DUMP_END_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_STAT_REGS_DUMP_STAT_REGS_DUMP_END_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_end = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  ret = fe600_stat_regs_dump(
          unit,
          prm_start,
          prm_end
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_stat_regs_dump - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_stat_regs_dump");
    goto exit;
  }

  send_string_to_screen(" ---> fe600_stat_regs_dump - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: stat_tbls_dump (section status)
 */
int
  ui_fe600_api_status_stat_tbls_dump(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint8
    prm_print_zero=0;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_status");
  soc_sand_proc_name = "fe600_stat_tbls_dump";

  unit = fe600_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_STAT_TBLS_DUMP_STAT_TBLS_DUMP_PRINT_ZERO_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_STAT_TBLS_DUMP_STAT_TBLS_DUMP_PRINT_ZERO_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_print_zero = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = fe600_stat_tbls_dump(
          unit,
          prm_print_zero
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_stat_tbls_dump - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_stat_tbls_dump");
    goto exit;
  }

  send_string_to_screen(" ---> fe600_stat_tbls_dump - SUCCEEDED", TRUE);
#ifndef SAND_LOW_LEVEL_SIMULATION
  fe600_srd_diag_regs_dump(unit);
#endif
  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: dbg_eci_access_tst (section debug)
 */
int
  ui_fe600_api_status_eci_access_tst(
    CURRENT_LINE *current_line
  )
{
  uint32 
    ret;   
  uint32   
    prm_nof_k_iters;
  uint8   
    prm_use_dflt_tst_reg;
  uint32   
    prm_reg1_addr_longs;
  uint32   
    prm_reg2_addr_longs;
  uint8   
    prm_is_valid;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_debug");
  soc_sand_proc_name = "fe600_stat_eci_access_tst";

  unit = fe600_get_default_unit();

  /* Get parameters */

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_STAT_ECI_ACCESS_TST_DBG_ECI_ACCESS_TST_NOF_K_ITERS_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_STAT_ECI_ACCESS_TST_DBG_ECI_ACCESS_TST_NOF_K_ITERS_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_nof_k_iters = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_STAT_ECI_ACCESS_TST_DBG_ECI_ACCESS_TST_USE_DFLT_TST_REG_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_STAT_ECI_ACCESS_TST_DBG_ECI_ACCESS_TST_USE_DFLT_TST_REG_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_use_dflt_tst_reg = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_STAT_ECI_ACCESS_TST_DBG_ECI_ACCESS_TST_REG1_ADDR_LONGS_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_STAT_ECI_ACCESS_TST_DBG_ECI_ACCESS_TST_REG1_ADDR_LONGS_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_reg1_addr_longs = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_STAT_ECI_ACCESS_TST_DBG_ECI_ACCESS_TST_REG2_ADDR_LONGS_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_STAT_ECI_ACCESS_TST_DBG_ECI_ACCESS_TST_REG2_ADDR_LONGS_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_reg2_addr_longs = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = fe600_stat_eci_access_tst(
          unit,
          prm_nof_k_iters,
          prm_use_dflt_tst_reg,
          prm_reg1_addr_longs,
          prm_reg2_addr_longs,
          &prm_is_valid
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** fe600_stat_eci_access_tst - FAIL", TRUE); 
    fe600_disp_result(ret, "fe600_stat_eci_access_tst");   
    goto exit; 
  } 

  soc_sand_os_printf( "is_valid: %u\n\r",prm_is_valid);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: diag_soft_error_test_start (section diagnostics)
 */
int
  ui_fe600_api_diagnostics_diag_soft_error_test_start(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  FE600_DIAG_SOFT_ERR_INFO
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_fe600_api_diagnostics");
  soc_sand_proc_name = "fe600_diag_soft_error_test_start";
 
  unit = fe600_get_default_unit();
  fe600_FE600_DIAG_SOFT_ERR_INFO_clear(unit, &prm_info);
 
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_DIAG_SOFT_ERROR_TEST_START_DIAG_SOFT_ERROR_TEST_START_INFO_SMS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_DIAG_SOFT_ERROR_TEST_START_DIAG_SOFT_ERROR_TEST_START_INFO_SMS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.sms = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_DIAG_SOFT_ERROR_TEST_START_DIAG_SOFT_ERROR_TEST_START_INFO_PATTERN_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FE600_DIAG_SOFT_ERROR_TEST_START_DIAG_SOFT_ERROR_TEST_START_INFO_PATTERN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.pattern = param_val->numeric_equivalent;
  }


  /* Call function */
  ret = fe600_diag_soft_error_test_start(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_diag_soft_error_test_start - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "fe600_diag_soft_error_test_start");
    goto exit;
  }

  
  goto exit;
exit:
  return ui_ret;
}
  
/********************************************************************
 *  Function handler: diag_soft_error_test_result_get (section diagnostics)
 */
int
  ui_fe600_api_diagnostics_diag_soft_error_test_result_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  FE600_DIAG_SOFT_ERR_INFO
    prm_info;
  FE600_DIAG_SOFT_COUNT_TYPE
    prm_count_type;
  FE600_DIAG_SOFT_SMS_RESULT
    *prm_sms_result[FE600_DIAG_SMS_MAX][FE600_DIAG_NOF_SONE_SAVED_PER_SMS_MAX];
  FE600_DIAG_SOFT_ERR_RESULT
    prm_glbl_result;
  uint32
    sms_ndx,
    sone_ndx;
   
  UI_MACROS_INIT_FUNCTION("ui_fe600_api_diagnostics");
  soc_sand_proc_name = "fe600_diag_soft_error_test_result_get";

  sal_memset(prm_sms_result, 0x0, FE600_DIAG_SMS_MAX * FE600_DIAG_NOF_SONE_SAVED_PER_SMS_MAX * sizeof(FE600_DIAG_SOFT_SMS_RESULT*));

  unit = fe600_get_default_unit();
  fe600_FE600_DIAG_SOFT_ERR_INFO_clear(unit, &prm_info);
  fe600_FE600_DIAG_SOFT_ERR_RESULT_clear(&prm_glbl_result);
 
  for (sms_ndx = 0; sms_ndx < FE600_DIAG_NOF_SMS; ++sms_ndx)
  {
    for (sone_ndx = 0; sone_ndx < FE600_DIAG_NOF_SONE_SAVED_PER_SMS_MAX; ++sone_ndx)
    {
      prm_sms_result[sms_ndx][sone_ndx] = sal_alloc_any_size(sizeof(FE600_DIAG_SOFT_SMS_RESULT));
      fe600_FE600_DIAG_SOFT_SMS_RESULT_clear(prm_sms_result[sms_ndx][sone_ndx]);
    }
  }

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_DIAG_SOFT_ERROR_TEST_RESULT_GET_DIAG_SOFT_ERROR_TEST_RESULT_GET_INFO_SMS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_DIAG_SOFT_ERROR_TEST_RESULT_GET_DIAG_SOFT_ERROR_TEST_RESULT_GET_INFO_SMS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.sms = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_DIAG_SOFT_ERROR_TEST_RESULT_GET_DIAG_SOFT_ERROR_TEST_RESULT_GET_INFO_PATTERN_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FE600_DIAG_SOFT_ERROR_TEST_RESULT_GET_DIAG_SOFT_ERROR_TEST_RESULT_GET_INFO_PATTERN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.pattern = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_DIAG_SOFT_ERROR_TEST_RESULT_GET_DIAG_SOFT_ERROR_TEST_RESULT_GET_COUNT_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FE600_DIAG_SOFT_ERROR_TEST_RESULT_GET_DIAG_SOFT_ERROR_TEST_RESULT_GET_COUNT_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_count_type = param_val->numeric_equivalent;
  }


  /* Call function */
  ret = fe600_diag_soft_error_test_result_get(
          unit,
          &prm_info,
          prm_count_type,
          prm_sms_result,
          &prm_glbl_result
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_diag_soft_error_test_result_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "fe600_diag_soft_error_test_result_get");
    goto exit;
  }

  for (sms_ndx = 0; sms_ndx < FE600_DIAG_NOF_SMS; ++sms_ndx)
  {
    for (sone_ndx = 0; sone_ndx < FE600_DIAG_NOF_SONE_SAVED_PER_SMS_MAX; ++sone_ndx)
    {
        fe600_FE600_DIAG_SOFT_SMS_RESULT_print(unit, sms_ndx + 1, sone_ndx, prm_sms_result[sms_ndx][sone_ndx]);
    }
  }
  fe600_FE600_DIAG_SOFT_ERR_RESULT_print(&prm_glbl_result);

  
  goto exit;
exit:
  for (sms_ndx = 0; sms_ndx < FE600_DIAG_NOF_SMS; ++sms_ndx)
  {
    for (sone_ndx = 0; sone_ndx < FE600_DIAG_NOF_SONE_SAVED_PER_SMS_MAX; ++sone_ndx)
    {
      if (prm_sms_result[sms_ndx][sone_ndx] != NULL)
      {
        sal_free(prm_sms_result[sms_ndx][sone_ndx]);
      }
    }
  }
  return ui_ret;
}

/********************************************************************
 *  Function handler: fe600_api_diagnostics_mbist_run (section diagnostics)
 */
int
  ui_fe600_api_diagnostics_mbist_run(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    sms = FE600_DIAG_SMS_ALL;
  FE600_DIAG_SOFT_SMS_RESULT
    *prm_sms_result[FE600_DIAG_SMS_MAX];
  uint32
    sms_ndx;
   
  UI_MACROS_INIT_FUNCTION("ui_fe600_api_diagnostics_mbist_run");
  soc_sand_proc_name = "fe600_api_diagnostics_mbist_run";
  sal_memset(prm_sms_result, 0x0, FE600_DIAG_SMS_MAX * sizeof(FE600_DIAG_SOFT_SMS_RESULT*));

  for (sms_ndx = 0; sms_ndx < FE600_DIAG_NOF_SMS; ++sms_ndx)
  {
    prm_sms_result[sms_ndx] = sal_alloc_any_size(sizeof(FE600_DIAG_SOFT_SMS_RESULT));
    fe600_FE600_DIAG_SOFT_SMS_RESULT_clear(prm_sms_result[sms_ndx]);
  }

  unit = fe600_get_default_unit();
 
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_DIAG_MBIST_RUN_INFO_SMS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_DIAG_MBIST_RUN_INFO_SMS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    sms = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  ret = fe600_diag_mbist_run(
          unit,
          sms,
          prm_sms_result
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_diag_mbist_run - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "fe600_diag_mbist_run");
    goto exit;
  }

  soc_sand_os_printf( "Errors (if any):\n\r");
  for (sms_ndx = 1; sms_ndx <= FE600_DIAG_NOF_SMS; ++sms_ndx)
  {
    fe600_FE600_DIAG_SOFT_SMS_RESULT_print(unit, sms_ndx, SOC_SAND_INTERN_VAL_INVALID_32, prm_sms_result[sms_ndx-1]);
  }
  
  goto exit;
exit:
  for (sms_ndx = 0; sms_ndx < FE600_DIAG_NOF_SMS; ++sms_ndx)
  {
    if (prm_sms_result[sms_ndx] != NULL)
    {
      sal_free(prm_sms_result[sms_ndx]);
    }
  }
  return ui_ret;
}

#endif
#ifdef UI_FE600_SERDES
/********************************************************************
 *  Function handler: srd_rate_set (section serdes)
 */
int
  ui_fe600_api_serdes_srd_rate_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_lane_ndx_min, prm_lane_ndx_max, lane_i, nof_lanes;
  uint32
    prm_rate;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_serdes");
  soc_sand_proc_name = "fe600_srd_rate_set";

  unit = fe600_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_RATE_SET_SRD_RATE_SET_LANE_NDX_ID,1))
  {
     UI_MACROS_GET_NUM_SYM_VAL_WITH_INDEX(PARAM_FE600_SRD_RATE_SET_SRD_RATE_SET_LANE_NDX_ID, 1);
     UI_MACROS_CHECK_GET_VAL_OF_ERROR;
     UI_MACROS_LOAD_LONG_VAL(prm_lane_ndx_min);

     prm_lane_ndx_max = prm_lane_ndx_min;
     UI_MACROS_GET_NUM_SYM_VAL_WITH_INDEX(PARAM_FE600_SRD_RATE_SET_SRD_RATE_SET_LANE_NDX_ID, 2);
    
     if (ui_ret)
     {
       ui_ret = 0;
     }
     else
     {
       UI_MACROS_CHECK_GET_VAL_OF_ERROR;
       UI_MACROS_LOAD_LONG_VAL(prm_lane_ndx_max);
     }
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter lane_ndx after srd_rate_set***", TRUE);
    goto exit;
  }

  nof_lanes = (prm_lane_ndx_min==FE600_SRD_NOF_LANES)?((uint32)FE600_SRD_NOF_LANES):(prm_lane_ndx_max-prm_lane_ndx_min+1);
  for(lane_i=(prm_lane_ndx_min==FE600_SRD_NOF_LANES)?0:prm_lane_ndx_min;lane_i<((prm_lane_ndx_min==FE600_SRD_NOF_LANES)?FE600_SRD_NOF_LANES:(prm_lane_ndx_min+nof_lanes));lane_i++)
  {

    /* This is a set function, so call GET function first */
    ret = fe600_srd_rate_get(
            unit,
            lane_i,
            &prm_rate
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      fe600_disp_result(ret, "fe600_srd_rate_get");
      goto exit;
    }

    if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_RATE_SET_SRD_RATE_SET_RATE_ID,1))
    {
      UI_MACROS_GET_NUM_SYM_VAL(PARAM_FE600_SRD_RATE_SET_SRD_RATE_SET_RATE_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      UI_MACROS_LOAD_LONG_VAL(prm_rate);
    }

    /* Call function */
    ret = fe600_srd_rate_set(
            unit,
            lane_i,
            prm_rate
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      fe600_disp_result(ret, "fe600_srd_rate_set");
      goto exit;
    }
  }
  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: srd_rate_get (section serdes)
 */
int
  ui_fe600_api_serdes_srd_rate_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_lane_ndx_min, prm_lane_ndx_max, lane_i, nof_lanes;
  uint32
    prm_rate;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_serdes");
  soc_sand_proc_name = "fe600_srd_rate_get";

  unit = fe600_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_RATE_GET_SRD_RATE_GET_LANE_NDX_ID,1))
  {
     UI_MACROS_GET_NUM_SYM_VAL_WITH_INDEX(PARAM_FE600_SRD_RATE_GET_SRD_RATE_GET_LANE_NDX_ID, 1);
     UI_MACROS_CHECK_GET_VAL_OF_ERROR;
     UI_MACROS_LOAD_LONG_VAL(prm_lane_ndx_min);

     prm_lane_ndx_max = prm_lane_ndx_min;
     UI_MACROS_GET_NUM_SYM_VAL_WITH_INDEX(PARAM_FE600_SRD_RATE_GET_SRD_RATE_GET_LANE_NDX_ID, 2);
    
     if (ui_ret)
     {
       ui_ret = 0;
     }
     else
     {
       UI_MACROS_CHECK_GET_VAL_OF_ERROR;
       UI_MACROS_LOAD_LONG_VAL(prm_lane_ndx_max);
     }
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter lane_ndx after srd_rate_get***", TRUE);
    goto exit;
  }

  nof_lanes = (prm_lane_ndx_min==FE600_SRD_NOF_LANES)?((uint32)FE600_SRD_NOF_LANES):(prm_lane_ndx_max-prm_lane_ndx_min+1);
  for(lane_i=(prm_lane_ndx_min==FE600_SRD_NOF_LANES)?0:prm_lane_ndx_min;lane_i<((prm_lane_ndx_min==FE600_SRD_NOF_LANES)?FE600_SRD_NOF_LANES:(prm_lane_ndx_min+nof_lanes));lane_i++)
  {
    soc_sand_os_printf( "Lane %d:\n\r", lane_i);

    /* Call function */
    ret = fe600_srd_rate_get(
            unit,
            lane_i,
            &prm_rate
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      fe600_disp_result(ret, "fe600_srd_rate_get");
      goto exit;
    }

        if (FE600_SRD_IS_RATE_ENUM(prm_rate))
    {
      soc_sand_os_printf( 
        "rate: %s\n\r",
        fe600_FE600_SRD_DATA_RATE_to_string(prm_rate)
      );
    }
    else
    {
      soc_sand_os_printf( 
        "rate: %u.%02u Mbps\n\r",
        prm_rate/1000, prm_rate%1000
      );
    }

  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: srd_tx_phys_params_set_explicit (section serdes)
 */
int
  ui_fe600_api_serdes_srd_tx_phys_params_set_explicit(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_lane_ndx_min, prm_lane_ndx_max, lane_i, nof_lanes;
  FE600_SRD_TX_PHYS_CONF_MODE
    prm_conf_mode_ndx = 0;
  FE600_SRD_TX_PHYS_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_serdes");
  soc_sand_proc_name = "ui_fe600_api_serdes_srd_tx_phys_params_set_explicit";

  unit = fe600_get_default_unit();
  fe600_FE600_SRD_TX_PHYS_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_TX_PHYS_PARAMS_SET_EXPLICIT_SRD_TX_PHYS_PARAMS_SET_LANE_NDX_ID,1))
  {
     UI_MACROS_GET_NUM_SYM_VAL_WITH_INDEX(PARAM_FE600_SRD_TX_PHYS_PARAMS_SET_EXPLICIT_SRD_TX_PHYS_PARAMS_SET_LANE_NDX_ID, 1);
     UI_MACROS_CHECK_GET_VAL_OF_ERROR;
     UI_MACROS_LOAD_LONG_VAL(prm_lane_ndx_min);

     prm_lane_ndx_max = prm_lane_ndx_min;
     UI_MACROS_GET_NUM_SYM_VAL_WITH_INDEX(PARAM_FE600_SRD_TX_PHYS_PARAMS_SET_EXPLICIT_SRD_TX_PHYS_PARAMS_SET_LANE_NDX_ID, 2);
    
     if (ui_ret)
     {
       ui_ret = 0;
     }
     else
     {
       UI_MACROS_CHECK_GET_VAL_OF_ERROR;
       UI_MACROS_LOAD_LONG_VAL(prm_lane_ndx_max);
     }
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter lane_ndx after srd_tx_phys_params_set***", TRUE);
    goto exit;
  }

  prm_conf_mode_ndx = FE600_SRD_TX_PHYS_CONF_MODE_EXPLICIT;
  nof_lanes = (prm_lane_ndx_min==FE600_SRD_NOF_LANES)?((uint32)FE600_SRD_NOF_LANES):(prm_lane_ndx_max-prm_lane_ndx_min+1);
  for(lane_i=(prm_lane_ndx_min==FE600_SRD_NOF_LANES)?0:prm_lane_ndx_min;lane_i<((prm_lane_ndx_min==FE600_SRD_NOF_LANES)?FE600_SRD_NOF_LANES:(prm_lane_ndx_min+nof_lanes));lane_i++)
  {

    /* This is a set function, so call GET function first */
    ret = fe600_srd_tx_phys_params_get(
            unit,
            lane_i,
            prm_conf_mode_ndx,
            &prm_info
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      fe600_disp_result(ret, "fe600_srd_tx_phys_params_get");
      goto exit;
    }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_TX_PHYS_PARAMS_SET_SRD_TX_PHYS_PARAMS_SET_INFO_CONF_EXPLCT_SWING_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_TX_PHYS_PARAMS_SET_SRD_TX_PHYS_PARAMS_SET_INFO_CONF_EXPLCT_SWING_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.conf.explct.swing = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_TX_PHYS_PARAMS_SET_SRD_TX_PHYS_PARAMS_SET_INFO_CONF_EXPLCT_POST_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_TX_PHYS_PARAMS_SET_SRD_TX_PHYS_PARAMS_SET_INFO_CONF_EXPLCT_POST_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.conf.explct.post = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_TX_PHYS_PARAMS_SET_SRD_TX_PHYS_PARAMS_SET_INFO_CONF_EXPLCT_PRE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_TX_PHYS_PARAMS_SET_SRD_TX_PHYS_PARAMS_SET_INFO_CONF_EXPLCT_PRE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.conf.explct.pre = (uint32)param_val->value.ulong_value;
  }

    /* Call function */
    ret = fe600_srd_tx_phys_params_set(
            unit,
            lane_i,
            prm_conf_mode_ndx,
            &prm_info
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      fe600_disp_result(ret, "fe600_srd_tx_phys_params_set");
      goto exit;
    }
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: srd_tx_phys_params_set_internal (section serdes)
 */
int
  ui_fe600_api_serdes_srd_tx_phys_params_set_atten(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_lane_ndx_min, prm_lane_ndx_max, lane_i, nof_lanes;
  FE600_SRD_TX_PHYS_CONF_MODE
    prm_conf_mode_ndx = 0;
  FE600_SRD_TX_PHYS_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_serdes");
  soc_sand_proc_name = "ui_fe600_api_serdes_srd_tx_phys_params_set_atten";

  unit = fe600_get_default_unit();
  fe600_FE600_SRD_TX_PHYS_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_TX_PHYS_PARAMS_SET_ATTEN_SRD_TX_PHYS_PARAMS_SET_LANE_NDX_ID,1))
  {
     UI_MACROS_GET_NUM_SYM_VAL_WITH_INDEX(PARAM_FE600_SRD_TX_PHYS_PARAMS_SET_ATTEN_SRD_TX_PHYS_PARAMS_SET_LANE_NDX_ID, 1);
     UI_MACROS_CHECK_GET_VAL_OF_ERROR;
     UI_MACROS_LOAD_LONG_VAL(prm_lane_ndx_min);

     prm_lane_ndx_max = prm_lane_ndx_min;
     UI_MACROS_GET_NUM_SYM_VAL_WITH_INDEX(PARAM_FE600_SRD_TX_PHYS_PARAMS_SET_ATTEN_SRD_TX_PHYS_PARAMS_SET_LANE_NDX_ID, 2);
    
     if (ui_ret)
     {
       ui_ret = 0;
     }
     else
     {
       UI_MACROS_CHECK_GET_VAL_OF_ERROR;
       UI_MACROS_LOAD_LONG_VAL(prm_lane_ndx_max);
     }
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter lane_ndx after srd_tx_phys_params_set***", TRUE);
    goto exit;
  }

  prm_conf_mode_ndx = FE600_SRD_TX_PHYS_CONF_MODE_ATTEN;
  nof_lanes = (prm_lane_ndx_min==FE600_SRD_NOF_LANES)?((uint32)FE600_SRD_NOF_LANES):(prm_lane_ndx_max-prm_lane_ndx_min+1);
  for(lane_i=(prm_lane_ndx_min==FE600_SRD_NOF_LANES)?0:prm_lane_ndx_min;lane_i<((prm_lane_ndx_min==FE600_SRD_NOF_LANES)?FE600_SRD_NOF_LANES:(prm_lane_ndx_min+nof_lanes));lane_i++)
  {

    /* This is a set function, so call GET function first */
    ret = fe600_srd_tx_phys_params_get(
            unit,
            lane_i,
            prm_conf_mode_ndx,
            &prm_info
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      fe600_disp_result(ret, "fe600_srd_tx_phys_params_get");
      goto exit;
    }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_TX_PHYS_PARAMS_SET_SRD_TX_PHYS_PARAMS_SET_INFO_CONF_ATTEN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_TX_PHYS_PARAMS_SET_SRD_TX_PHYS_PARAMS_SET_INFO_CONF_ATTEN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.conf.atten = (uint32)param_val->value.ulong_value;
  }


    /* Call function */
    ret = fe600_srd_tx_phys_params_set(
            unit,
            lane_i,
            prm_conf_mode_ndx,
            &prm_info
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      fe600_disp_result(ret, "fe600_srd_tx_phys_params_set");
      goto exit;
    }

  }
  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: srd_tx_phys_params_set_internal (section serdes)
 */
int
  ui_fe600_api_serdes_srd_tx_phys_params_set_internal(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_lane_ndx_min, prm_lane_ndx_max, lane_i, nof_lanes;
  FE600_SRD_TX_PHYS_CONF_MODE
    prm_conf_mode_ndx = 0;
  FE600_SRD_TX_PHYS_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_serdes");
  soc_sand_proc_name = "ui_fe600_api_serdes_srd_tx_phys_params_set_internal";

  unit = fe600_get_default_unit();
  fe600_FE600_SRD_TX_PHYS_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_TX_PHYS_PARAMS_SET_INTERNAL_SRD_TX_PHYS_PARAMS_SET_LANE_NDX_ID,1))
  {
     UI_MACROS_GET_NUM_SYM_VAL_WITH_INDEX(PARAM_FE600_SRD_TX_PHYS_PARAMS_SET_INTERNAL_SRD_TX_PHYS_PARAMS_SET_LANE_NDX_ID, 1);
     UI_MACROS_CHECK_GET_VAL_OF_ERROR;
     UI_MACROS_LOAD_LONG_VAL(prm_lane_ndx_min);

     prm_lane_ndx_max = prm_lane_ndx_min;
     UI_MACROS_GET_NUM_SYM_VAL_WITH_INDEX(PARAM_FE600_SRD_TX_PHYS_PARAMS_SET_INTERNAL_SRD_TX_PHYS_PARAMS_SET_LANE_NDX_ID, 2);
    
     if (ui_ret)
     {
       ui_ret = 0;
     }
     else
     {
       UI_MACROS_CHECK_GET_VAL_OF_ERROR;
       UI_MACROS_LOAD_LONG_VAL(prm_lane_ndx_max);
     }
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter lane_ndx after srd_tx_phys_params_set***", TRUE);
    goto exit;
  }

  prm_conf_mode_ndx = FE600_SRD_TX_PHYS_CONF_MODE_INTERNAL;
  nof_lanes = (prm_lane_ndx_min==FE600_SRD_NOF_LANES)?((uint32)FE600_SRD_NOF_LANES):(prm_lane_ndx_max-prm_lane_ndx_min+1);
  for(lane_i=(prm_lane_ndx_min==FE600_SRD_NOF_LANES)?0:prm_lane_ndx_min;lane_i<((prm_lane_ndx_min==FE600_SRD_NOF_LANES)?FE600_SRD_NOF_LANES:(prm_lane_ndx_min+nof_lanes));lane_i++)
  {

    /* This is a set function, so call GET function first */
    ret = fe600_srd_tx_phys_params_get(
            unit,
            lane_i,
            prm_conf_mode_ndx,
            &prm_info
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      fe600_disp_result(ret, "fe600_srd_tx_phys_params_get");
      goto exit;
    }

    if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_TX_PHYS_PARAMS_SET_INTERNAL_SRD_TX_PHYS_PARAMS_SET_INFO_CONF_INTERN_POST_ID,1))
    {
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_TX_PHYS_PARAMS_SET_INTERNAL_SRD_TX_PHYS_PARAMS_SET_INFO_CONF_INTERN_POST_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      prm_info.conf.intern.post = (uint8)param_val->value.ulong_value;
    }
    if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_TX_PHYS_PARAMS_SET_INTERNAL_SRD_TX_PHYS_PARAMS_SET_INFO_CONF_INTERN_PRE_ID,1))
    {
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_TX_PHYS_PARAMS_SET_INTERNAL_SRD_TX_PHYS_PARAMS_SET_INFO_CONF_INTERN_PRE_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      prm_info.conf.intern.pre = (uint8)param_val->value.ulong_value;
    }
    if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_TX_PHYS_PARAMS_SET_INTERNAL_SRD_TX_PHYS_PARAMS_SET_INFO_CONF_INTERN_MAIN_ID,1))
    {
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_TX_PHYS_PARAMS_SET_INTERNAL_SRD_TX_PHYS_PARAMS_SET_INFO_CONF_INTERN_MAIN_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      prm_info.conf.intern.main = (uint8)param_val->value.ulong_value;
    }
    if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_TX_PHYS_PARAMS_SET_INTERNAL_SRD_TX_PHYS_PARAMS_SET_INFO_CONF_INTERN_AMP_ID,1))
    {
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_TX_PHYS_PARAMS_SET_INTERNAL_SRD_TX_PHYS_PARAMS_SET_INFO_CONF_INTERN_AMP_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      prm_info.conf.intern.amp = (uint8)param_val->value.ulong_value;
    }


    /* Call function */
    ret = fe600_srd_tx_phys_params_set(
            unit,
            lane_i,
            prm_conf_mode_ndx,
            &prm_info
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      fe600_disp_result(ret, "fe600_srd_tx_phys_params_set");
      goto exit;
    }

  }
  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: srd_tx_phys_params_set_media_type (section serdes)
 */
int
  ui_fe600_api_serdes_srd_tx_phys_params_set_media_type(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_lane_ndx_min, prm_lane_ndx_max, lane_i, nof_lanes;
  FE600_SRD_TX_PHYS_CONF_MODE
    prm_conf_mode_ndx = 0;
  FE600_SRD_TX_PHYS_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_serdes");
  soc_sand_proc_name = "ui_fe600_api_serdes_srd_tx_phys_params_set_media_type";

  unit = fe600_get_default_unit();
  fe600_FE600_SRD_TX_PHYS_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_TX_PHYS_PARAMS_SET_MEDIA_TYPE_SRD_TX_PHYS_PARAMS_SET_LANE_NDX_ID,1))
  {
     UI_MACROS_GET_NUM_SYM_VAL_WITH_INDEX(PARAM_FE600_SRD_TX_PHYS_PARAMS_SET_MEDIA_TYPE_SRD_TX_PHYS_PARAMS_SET_LANE_NDX_ID, 1);
     UI_MACROS_CHECK_GET_VAL_OF_ERROR;
     UI_MACROS_LOAD_LONG_VAL(prm_lane_ndx_min);

     prm_lane_ndx_max = prm_lane_ndx_min;
     UI_MACROS_GET_NUM_SYM_VAL_WITH_INDEX(PARAM_FE600_SRD_TX_PHYS_PARAMS_SET_MEDIA_TYPE_SRD_TX_PHYS_PARAMS_SET_LANE_NDX_ID, 2);
    
     if (ui_ret)
     {
       ui_ret = 0;
     }
     else
     {
       UI_MACROS_CHECK_GET_VAL_OF_ERROR;
       UI_MACROS_LOAD_LONG_VAL(prm_lane_ndx_max);
     }
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter lane_ndx after srd_tx_phys_params_set***", TRUE);
    goto exit;
  }

  prm_conf_mode_ndx = FE600_SRD_TX_PHYS_CONF_MODE_MEDIA_TYPE;
  nof_lanes = (prm_lane_ndx_min==FE600_SRD_NOF_LANES)?((uint32)FE600_SRD_NOF_LANES):(prm_lane_ndx_max-prm_lane_ndx_min+1);
  for(lane_i=(prm_lane_ndx_min==FE600_SRD_NOF_LANES)?0:prm_lane_ndx_min;lane_i<((prm_lane_ndx_min==FE600_SRD_NOF_LANES)?FE600_SRD_NOF_LANES:(prm_lane_ndx_min+nof_lanes));lane_i++)
  {

    /* This is a set function, so call GET function first */
    ret = fe600_srd_tx_phys_params_get(
            unit,
            lane_i,
            prm_conf_mode_ndx,
            &prm_info
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      fe600_disp_result(ret, "fe600_srd_tx_phys_params_get");
      goto exit;
    }

    if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_TX_PHYS_PARAMS_SET_MEDIA_TYPE_SRD_TX_PHYS_PARAMS_SET_INFO_CONF_MEDIA_TYPE_ID,1))
    {
      UI_MACROS_GET_SYMBOL_VAL(PARAM_FE600_SRD_TX_PHYS_PARAMS_SET_MEDIA_TYPE_SRD_TX_PHYS_PARAMS_SET_INFO_CONF_MEDIA_TYPE_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      prm_info.conf.media_type = param_val->numeric_equivalent;
    }


    /* Call function */
    ret = fe600_srd_tx_phys_params_set(
            unit,
            lane_i,
            prm_conf_mode_ndx,
            &prm_info
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      fe600_disp_result(ret, "fe600_srd_tx_phys_params_set");
      goto exit;
    }

  }
  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: srd_tx_phys_params_get (section serdes)
 */
int
  ui_fe600_api_serdes_srd_tx_phys_params_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_lane_ndx_min, prm_lane_ndx_max, lane_i, nof_lanes;
  FE600_SRD_TX_PHYS_CONF_MODE
    prm_conf_mode_ndx = 0;
  FE600_SRD_TX_PHYS_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_serdes");
  soc_sand_proc_name = "fe600_srd_tx_phys_params_get";

  unit = fe600_get_default_unit();
  fe600_FE600_SRD_TX_PHYS_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_TX_PHYS_PARAMS_GET_SRD_TX_PHYS_PARAMS_GET_LANE_NDX_ID,1))
  {
     UI_MACROS_GET_NUM_SYM_VAL_WITH_INDEX(PARAM_FE600_SRD_TX_PHYS_PARAMS_GET_SRD_TX_PHYS_PARAMS_GET_LANE_NDX_ID, 1);
     UI_MACROS_CHECK_GET_VAL_OF_ERROR;
     UI_MACROS_LOAD_LONG_VAL(prm_lane_ndx_min);

     prm_lane_ndx_max = prm_lane_ndx_min;
     UI_MACROS_GET_NUM_SYM_VAL_WITH_INDEX(PARAM_FE600_SRD_TX_PHYS_PARAMS_GET_SRD_TX_PHYS_PARAMS_GET_LANE_NDX_ID, 2);
    
     if (ui_ret)
     {
       ui_ret = 0;
     }
     else
     {
       UI_MACROS_CHECK_GET_VAL_OF_ERROR;
       UI_MACROS_LOAD_LONG_VAL(prm_lane_ndx_max);
     }
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter lane_ndx after srd_tx_phys_params_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_TX_PHYS_PARAMS_GET_SRD_TX_PHYS_PARAMS_GET_CONF_MODE_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FE600_SRD_TX_PHYS_PARAMS_GET_SRD_TX_PHYS_PARAMS_GET_CONF_MODE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_conf_mode_ndx = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter conf_mode_ndx after srd_tx_phys_params_get***", TRUE);
    goto exit;
  }
  nof_lanes = (prm_lane_ndx_min==FE600_SRD_NOF_LANES)?((uint32)FE600_SRD_NOF_LANES):(prm_lane_ndx_max-prm_lane_ndx_min+1);
  for(lane_i=(prm_lane_ndx_min==FE600_SRD_NOF_LANES)?0:prm_lane_ndx_min;lane_i<((prm_lane_ndx_min==FE600_SRD_NOF_LANES)?FE600_SRD_NOF_LANES:(prm_lane_ndx_min+nof_lanes));lane_i++)
  {
    soc_sand_os_printf( "Lane %d:\n\r", lane_i);

    /* Call function */
    ret = fe600_srd_tx_phys_params_get(
            unit,
            lane_i,
            prm_conf_mode_ndx,
            &prm_info
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      fe600_disp_result(ret, "fe600_srd_tx_phys_params_get");
      goto exit;
    }

    send_string_to_screen("--> info:", TRUE);
    fe600_FE600_SRD_TX_PHYS_INFO_print(prm_conf_mode_ndx,&prm_info);

  }
  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: srd_lane_polarity_set (section serdes)
 */
int
  ui_fe600_api_serdes_srd_lane_polarity_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_lane_ndx_min, prm_lane_ndx_max, lane_i, nof_lanes;
  FE600_CONNECTION_DIRECTION
    prm_direction_ndx = FE600_CONNECTION_DIRECTION_BOTH;
  uint8
    prm_rx_is_swap_polarity,
    prm_tx_is_swap_polarity;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_serdes");
  soc_sand_proc_name = "fe600_srd_lane_polarity_set";

  unit = fe600_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_LANE_POLARITY_SET_SRD_LANE_POLARITY_SET_LANE_NDX_ID,1))
  {
     UI_MACROS_GET_NUM_SYM_VAL_WITH_INDEX(PARAM_FE600_SRD_LANE_POLARITY_SET_SRD_LANE_POLARITY_SET_LANE_NDX_ID, 1);
     UI_MACROS_CHECK_GET_VAL_OF_ERROR;
     UI_MACROS_LOAD_LONG_VAL(prm_lane_ndx_min);

     prm_lane_ndx_max = prm_lane_ndx_min;
     UI_MACROS_GET_NUM_SYM_VAL_WITH_INDEX(PARAM_FE600_SRD_LANE_POLARITY_SET_SRD_LANE_POLARITY_SET_LANE_NDX_ID, 2);
    
     if (ui_ret)
     {
       ui_ret = 0;
     }
     else
     {
       UI_MACROS_CHECK_GET_VAL_OF_ERROR;
       UI_MACROS_LOAD_LONG_VAL(prm_lane_ndx_max);
     }
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter lane_ndx after srd_lane_polarity_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_LANE_POLARITY_SET_SRD_LANE_POLARITY_SET_DIRECTION_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FE600_SRD_LANE_POLARITY_SET_SRD_LANE_POLARITY_SET_DIRECTION_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_direction_ndx = param_val->numeric_equivalent;
  }

  nof_lanes = (prm_lane_ndx_min==FE600_SRD_NOF_LANES)?((uint32)FE600_SRD_NOF_LANES):(prm_lane_ndx_max-prm_lane_ndx_min+1);
  for(lane_i=(prm_lane_ndx_min==FE600_SRD_NOF_LANES)?0:prm_lane_ndx_min;lane_i<((prm_lane_ndx_min==FE600_SRD_NOF_LANES)?FE600_SRD_NOF_LANES:(prm_lane_ndx_min+nof_lanes));lane_i++)
  {

    /* This is a set function, so call GET function first */
    ret = fe600_srd_lane_polarity_get(
            unit,
            lane_i,
            &prm_rx_is_swap_polarity,
            &prm_tx_is_swap_polarity
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      fe600_disp_result(ret, "fe600_srd_lane_polarity_get");
      goto exit;
    }

    if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_LANE_POLARITY_SET_SRD_LANE_POLARITY_SET_IS_SWAP_POLARITY_ID,1))
    {
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_LANE_POLARITY_SET_SRD_LANE_POLARITY_SET_IS_SWAP_POLARITY_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      prm_rx_is_swap_polarity = (uint8)param_val->value.ulong_value;
      prm_tx_is_swap_polarity = (uint8)param_val->value.ulong_value;
    }


    /* Call function */
    ret = fe600_srd_lane_polarity_set(
            unit,
            lane_i,
            prm_direction_ndx,
            prm_rx_is_swap_polarity
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      fe600_disp_result(ret, "fe600_srd_lane_polarity_set");
      goto exit;
    }

  }
  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: srd_lane_polarity_get (section serdes)
 */
int
  ui_fe600_api_serdes_srd_lane_polarity_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_lane_ndx_min, prm_lane_ndx_max, lane_i, nof_lanes;
  uint8
    prm_rx_is_swap_polarity,
    prm_tx_is_swap_polarity;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_serdes");
  soc_sand_proc_name = "fe600_srd_lane_polarity_get";

  unit = fe600_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_LANE_POLARITY_GET_SRD_LANE_POLARITY_GET_LANE_NDX_ID,1))
  {
     UI_MACROS_GET_NUM_SYM_VAL_WITH_INDEX(PARAM_FE600_SRD_LANE_POLARITY_GET_SRD_LANE_POLARITY_GET_LANE_NDX_ID, 1);
     UI_MACROS_CHECK_GET_VAL_OF_ERROR;
     UI_MACROS_LOAD_LONG_VAL(prm_lane_ndx_min);

     prm_lane_ndx_max = prm_lane_ndx_min;
     UI_MACROS_GET_NUM_SYM_VAL_WITH_INDEX(PARAM_FE600_SRD_LANE_POLARITY_GET_SRD_LANE_POLARITY_GET_LANE_NDX_ID, 2);
    
     if (ui_ret)
     {
       ui_ret = 0;
     }
     else
     {
       UI_MACROS_CHECK_GET_VAL_OF_ERROR;
       UI_MACROS_LOAD_LONG_VAL(prm_lane_ndx_max);
     }
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter lane_ndx after srd_lane_polarity_get***", TRUE);
    goto exit;
  }

  nof_lanes = (prm_lane_ndx_min==FE600_SRD_NOF_LANES)?((uint32)FE600_SRD_NOF_LANES):(prm_lane_ndx_max-prm_lane_ndx_min+1);
  for(lane_i=(prm_lane_ndx_min==FE600_SRD_NOF_LANES)?0:prm_lane_ndx_min;lane_i<((prm_lane_ndx_min==FE600_SRD_NOF_LANES)?FE600_SRD_NOF_LANES:(prm_lane_ndx_min+nof_lanes));lane_i++)
  {
    soc_sand_os_printf( "Lane %d:\n\r", lane_i);

    /* Call function */
    ret = fe600_srd_lane_polarity_get(
            unit,
            lane_i,
            &prm_rx_is_swap_polarity,
            &prm_tx_is_swap_polarity
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      fe600_disp_result(ret, "fe600_srd_lane_polarity_get");
      goto exit;
    }

    send_string_to_screen("--> RX: \n\r", TRUE);
    soc_sand_os_printf( "is_swap_polarity: %u, ",prm_rx_is_swap_polarity);
    send_string_to_screen("\n\r", TRUE);
    send_string_to_screen("--> TX: \n\r", TRUE);
    soc_sand_os_printf( "is_swap_polarity: %u ",prm_tx_is_swap_polarity);
    send_string_to_screen("\n\r", TRUE);


  }
  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: srd_lane_power_state_set (section serdes)
 */
int
  ui_fe600_api_serdes_srd_lane_power_state_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_lane_ndx_min, prm_lane_ndx_max, lane_i, nof_lanes;
  FE600_CONNECTION_DIRECTION
    prm_direction_ndx = FE600_CONNECTION_DIRECTION_BOTH;
  FE600_SRD_POWER_STATE
    prm_rx_state,
    prm_tx_state;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_serdes");
  soc_sand_proc_name = "fe600_srd_lane_power_state_set";

  unit = fe600_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_LANE_POWER_STATE_SET_SRD_LANE_POWER_STATE_SET_LANE_NDX_ID,1))
  {
     UI_MACROS_GET_NUM_SYM_VAL_WITH_INDEX(PARAM_FE600_SRD_LANE_POWER_STATE_SET_SRD_LANE_POWER_STATE_SET_LANE_NDX_ID, 1);
     UI_MACROS_CHECK_GET_VAL_OF_ERROR;
     UI_MACROS_LOAD_LONG_VAL(prm_lane_ndx_min);

     prm_lane_ndx_max = prm_lane_ndx_min;
     UI_MACROS_GET_NUM_SYM_VAL_WITH_INDEX(PARAM_FE600_SRD_LANE_POWER_STATE_SET_SRD_LANE_POWER_STATE_SET_LANE_NDX_ID, 2);
    
     if (ui_ret)
     {
       ui_ret = 0;
     }
     else
     {
       UI_MACROS_CHECK_GET_VAL_OF_ERROR;
       UI_MACROS_LOAD_LONG_VAL(prm_lane_ndx_max);
     }
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter lane_ndx after srd_lane_power_state_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_LANE_POWER_STATE_SET_SRD_LANE_POWER_STATE_SET_DIRECTION_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FE600_SRD_LANE_POWER_STATE_SET_SRD_LANE_POWER_STATE_SET_DIRECTION_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_direction_ndx = param_val->numeric_equivalent;
  }

  nof_lanes = (prm_lane_ndx_min==FE600_SRD_NOF_LANES)?((uint32)FE600_SRD_NOF_LANES):(prm_lane_ndx_max-prm_lane_ndx_min+1);
  for(lane_i=(prm_lane_ndx_min==FE600_SRD_NOF_LANES)?0:prm_lane_ndx_min;lane_i<((prm_lane_ndx_min==FE600_SRD_NOF_LANES)?FE600_SRD_NOF_LANES:(prm_lane_ndx_min+nof_lanes));lane_i++)
  {

    /* This is a set function, so call GET function first */
    ret = fe600_srd_lane_power_state_get(
            unit,
            lane_i,
            &prm_rx_state,
            &prm_tx_state
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      fe600_disp_result(ret, "fe600_srd_lane_power_state_get");
      goto exit;
    }

    if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_LANE_POWER_STATE_SET_SRD_LANE_POWER_STATE_SET_STATE_ID,1))
    {
      UI_MACROS_GET_SYMBOL_VAL(PARAM_FE600_SRD_LANE_POWER_STATE_SET_SRD_LANE_POWER_STATE_SET_STATE_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      prm_rx_state = param_val->numeric_equivalent;
      prm_tx_state = param_val->numeric_equivalent;
    }


    /* Call function */
    ret = fe600_srd_lane_power_state_set(
            unit,
            lane_i,
            prm_direction_ndx,
            prm_rx_state
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      fe600_disp_result(ret, "fe600_srd_lane_power_state_set");
      goto exit;
    }
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: srd_lane_power_state_get (section serdes)
 */
int
  ui_fe600_api_serdes_srd_lane_power_state_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_lane_ndx_min, prm_lane_ndx_max, lane_i, nof_lanes;
  FE600_SRD_POWER_STATE
    prm_rx_state = 0,
    prm_tx_state;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_serdes");
  soc_sand_proc_name = "fe600_srd_lane_power_state_get";

  unit = fe600_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_LANE_POWER_STATE_GET_SRD_LANE_POWER_STATE_GET_LANE_NDX_ID,1))
  {
     UI_MACROS_GET_NUM_SYM_VAL_WITH_INDEX(PARAM_FE600_SRD_LANE_POWER_STATE_GET_SRD_LANE_POWER_STATE_GET_LANE_NDX_ID, 1);
     UI_MACROS_CHECK_GET_VAL_OF_ERROR;
     UI_MACROS_LOAD_LONG_VAL(prm_lane_ndx_min);

     prm_lane_ndx_max = prm_lane_ndx_min;
     UI_MACROS_GET_NUM_SYM_VAL_WITH_INDEX(PARAM_FE600_SRD_LANE_POWER_STATE_GET_SRD_LANE_POWER_STATE_GET_LANE_NDX_ID, 2);
    
     if (ui_ret)
     {
       ui_ret = 0;
     }
     else
     {
       UI_MACROS_CHECK_GET_VAL_OF_ERROR;
       UI_MACROS_LOAD_LONG_VAL(prm_lane_ndx_max);
     }
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter lane_ndx after srd_lane_power_state_get***", TRUE);
    goto exit;
  }

  nof_lanes = (prm_lane_ndx_min==FE600_SRD_NOF_LANES)?((uint32)FE600_SRD_NOF_LANES):(prm_lane_ndx_max-prm_lane_ndx_min+1);
  for(lane_i=(prm_lane_ndx_min==FE600_SRD_NOF_LANES)?0:prm_lane_ndx_min;lane_i<((prm_lane_ndx_min==FE600_SRD_NOF_LANES)?FE600_SRD_NOF_LANES:(prm_lane_ndx_min+nof_lanes));lane_i++)
  {
    soc_sand_os_printf( "Lane %d:\n\r", lane_i);

    /* Call function */
    ret = fe600_srd_lane_power_state_get(
            unit,
            lane_i,
            &prm_rx_state,
            &prm_tx_state
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      fe600_disp_result(ret, "fe600_srd_lane_power_state_get");
      goto exit;
    }

    send_string_to_screen("--> RX: \n\r", TRUE);
    soc_sand_os_printf( "state: %s, ",fe600_FE600_SRD_POWER_STATE_to_string(prm_rx_state));
    send_string_to_screen("\n\r", TRUE);
    send_string_to_screen("--> TX: \n\r", TRUE);
    soc_sand_os_printf( "state: %s\n\r",fe600_FE600_SRD_POWER_STATE_to_string(prm_tx_state));
    send_string_to_screen("\n\r", TRUE);

  }
  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: srd_star_set (section serdes)
 */
int
  ui_fe600_api_serdes_srd_star_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  FE600_SRD_STAR_ID
    prm_star_ndx = 0;
  uint32
    prm_qrtt_index = 0xFFFFFFFF;
  FE600_SRD_STAR_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_serdes");
  soc_sand_proc_name = "fe600_srd_star_set";

  unit = fe600_get_default_unit();
  fe600_FE600_SRD_STAR_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_STAR_SET_SRD_STAR_SET_STAR_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_STAR_SET_SRD_STAR_SET_STAR_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_star_ndx = param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter star_ndx after srd_star_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = fe600_srd_star_get(
          unit,
          prm_star_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    fe600_disp_result(ret, "fe600_srd_star_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_STAR_SET_SRD_STAR_SET_INFO_QRTT_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_STAR_SET_SRD_STAR_SET_INFO_QRTT_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_qrtt_index = (uint32)param_val->value.ulong_value;
  }

  if(prm_qrtt_index != 0xFFFFFFFF)
  {

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_STAR_SET_SRD_STAR_SET_INFO_QRTT_MAX_EXPECTED_LANE_RATE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FE600_SRD_STAR_SET_SRD_STAR_SET_INFO_QRTT_MAX_EXPECTED_LANE_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.qrtt[ prm_qrtt_index].max_expected_lane_rate = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_STAR_SET_SRD_STAR_SET_INFO_QRTT_IS_ACTIVE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_STAR_SET_SRD_STAR_SET_INFO_QRTT_IS_ACTIVE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.qrtt[ prm_qrtt_index].is_active = (uint8)param_val->value.ulong_value;
  }

  }


  /* Call function */
  ret = fe600_srd_star_set(
          unit,
          prm_star_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    fe600_disp_result(ret, "fe600_srd_star_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: srd_star_get (section serdes)
 */
int
  ui_fe600_api_serdes_srd_star_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  FE600_SRD_STAR_ID
    prm_star_ndx = 0;
  FE600_SRD_STAR_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_serdes");
  soc_sand_proc_name = "fe600_srd_star_get";

  unit = fe600_get_default_unit();
  fe600_FE600_SRD_STAR_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_STAR_GET_SRD_STAR_GET_STAR_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_STAR_GET_SRD_STAR_GET_STAR_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_star_ndx = param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter star_ndx after srd_star_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = fe600_srd_star_get(
          unit,
          prm_star_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    fe600_disp_result(ret, "fe600_srd_star_get");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  fe600_FE600_SRD_STAR_INFO_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}
/********************************************************************
 *  Function handler: srd_qrtt_set (section serdes)
 */
int
  ui_fe600_api_serdes_srd_qrtt_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_qrtt_ndx;
  FE600_SRD_QRTT_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_serdes");
  soc_sand_proc_name = "fe600_srd_qrtt_set";

  unit = fe600_get_default_unit();
  fe600_FE600_SRD_QRTT_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_QRTT_SET_SRD_QRTT_SET_QRTT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_QRTT_SET_SRD_QRTT_SET_QRTT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_qrtt_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter qrtt_ndx after srd_qrtt_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = fe600_srd_qrtt_get(
          unit,
          prm_qrtt_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_srd_qrtt_get - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_srd_qrtt_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_QRTT_SET_SRD_QRTT_SET_INFO_MAX_EXPECTED_LANE_RATE_ID,1))
  {
    UI_MACROS_GET_NUM_SYM_VAL(PARAM_FE600_SRD_QRTT_SET_SRD_QRTT_SET_INFO_MAX_EXPECTED_LANE_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    UI_MACROS_LOAD_LONG_VAL(prm_info.max_expected_lane_rate);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_QRTT_SET_SRD_QRTT_SET_INFO_IS_ACTIVE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_QRTT_SET_SRD_QRTT_SET_INFO_IS_ACTIVE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.is_active = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = fe600_srd_qrtt_set(
          unit,
          prm_qrtt_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_srd_qrtt_set - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_srd_qrtt_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: srd_qrtt_get (section serdes)
 */
int
  ui_fe600_api_serdes_srd_qrtt_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_qrtt_ndx;
  FE600_SRD_QRTT_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_serdes");
  soc_sand_proc_name = "fe600_srd_qrtt_get";

  unit = fe600_get_default_unit();
  fe600_FE600_SRD_QRTT_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_QRTT_GET_SRD_QRTT_GET_QRTT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_QRTT_GET_SRD_QRTT_GET_QRTT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_qrtt_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter qrtt_ndx after srd_qrtt_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = fe600_srd_qrtt_get(
          unit,
          prm_qrtt_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_srd_qrtt_get - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_srd_qrtt_get");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  fe600_FE600_SRD_QRTT_INFO_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: srd_all_set (section serdes)
 */
int
  ui_fe600_api_serdes_srd_all_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_lane_conf_index = 0xFFFFFFFF;
  uint32
    prm_star_conf_index = 0xFFFFFFFF;
  uint32
    prm_qrtt_index = 0xFFFFFFFF;
  FE600_SRD_ALL_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_serdes");
  soc_sand_proc_name = "fe600_srd_all_set";

  unit = fe600_get_default_unit();
  fe600_FE600_SRD_ALL_INFO_clear(&prm_info);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = fe600_srd_all_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_srd_all_get - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_srd_all_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_lane_conf_index = (uint32)param_val->value.ulong_value;
  }

  if(prm_lane_conf_index != 0xFFFFFFFF)
  {

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_IS_SWAP_POLARITY_RX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_IS_SWAP_POLARITY_RX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.lane_conf[ prm_lane_conf_index].is_swap_polarity_rx = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_IS_SWAP_POLARITY_TX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_IS_SWAP_POLARITY_TX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.lane_conf[ prm_lane_conf_index].is_swap_polarity_tx = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_POWER_STATE_CONF_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_POWER_STATE_CONF_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.lane_conf[ prm_lane_conf_index].power_state_conf = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_RX_PHYS_CONF_INTERN_G1CNT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_RX_PHYS_CONF_INTERN_G1CNT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.lane_conf[ prm_lane_conf_index].rx_phys_conf.intern.g1cnt = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_RX_PHYS_CONF_INTERN_TLTH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_RX_PHYS_CONF_INTERN_TLTH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.lane_conf[ prm_lane_conf_index].rx_phys_conf.intern.tlth = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_RX_PHYS_CONF_INTERN_DFELTH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_RX_PHYS_CONF_INTERN_DFELTH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.lane_conf[ prm_lane_conf_index].rx_phys_conf.intern.dfelth = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_RX_PHYS_CONF_INTERN_Z1CNT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_RX_PHYS_CONF_INTERN_Z1CNT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.lane_conf[ prm_lane_conf_index].rx_phys_conf.intern.z1cnt = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_RX_PHYS_CONF_INTERN_ZCNT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_RX_PHYS_CONF_INTERN_ZCNT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.lane_conf[ prm_lane_conf_index].rx_phys_conf.intern.zcnt = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_TX_PHYS_CONF_CONF_MEDIA_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_TX_PHYS_CONF_CONF_MEDIA_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.lane_conf[ prm_lane_conf_index].tx_phys_conf.conf.media_type = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_TX_PHYS_CONF_CONF_ATTEN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_TX_PHYS_CONF_CONF_ATTEN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.lane_conf[ prm_lane_conf_index].tx_phys_conf.conf.atten = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_TX_PHYS_CONF_CONF_EXPLCT_SWING_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_TX_PHYS_CONF_CONF_EXPLCT_SWING_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.lane_conf[ prm_lane_conf_index].tx_phys_conf.conf.explct.swing = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_TX_PHYS_CONF_CONF_EXPLCT_POST_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_TX_PHYS_CONF_CONF_EXPLCT_POST_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.lane_conf[ prm_lane_conf_index].tx_phys_conf.conf.explct.post = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_TX_PHYS_CONF_CONF_EXPLCT_PRE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_TX_PHYS_CONF_CONF_EXPLCT_PRE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.lane_conf[ prm_lane_conf_index].tx_phys_conf.conf.explct.pre = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_TX_PHYS_CONF_CONF_INTERN_POST_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_TX_PHYS_CONF_CONF_INTERN_POST_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.lane_conf[ prm_lane_conf_index].tx_phys_conf.conf.intern.post = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_TX_PHYS_CONF_CONF_INTERN_PRE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_TX_PHYS_CONF_CONF_INTERN_PRE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.lane_conf[ prm_lane_conf_index].tx_phys_conf.conf.intern.pre = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_TX_PHYS_CONF_CONF_INTERN_MAIN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_TX_PHYS_CONF_CONF_INTERN_MAIN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.lane_conf[ prm_lane_conf_index].tx_phys_conf.conf.intern.main = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_TX_PHYS_CONF_CONF_INTERN_AMP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_TX_PHYS_CONF_CONF_INTERN_AMP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.lane_conf[ prm_lane_conf_index].tx_phys_conf.conf.intern.amp = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_RATE_CONF_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_RATE_CONF_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.lane_conf[ prm_lane_conf_index].rate_conf = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.lane_conf[ prm_lane_conf_index].enable = (uint8)param_val->value.ulong_value;
  }

  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_STAR_CONF_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_STAR_CONF_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_star_conf_index = (uint32)param_val->value.ulong_value;
  }

  if(prm_star_conf_index != 0xFFFFFFFF)
  {

  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_STAR_CONF_CONF_QRTT_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_STAR_CONF_CONF_QRTT_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_qrtt_index = (uint32)param_val->value.ulong_value;
  }

  if(prm_qrtt_index != 0xFFFFFFFF)
  {

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_STAR_CONF_CONF_QRTT_MAX_EXPECTED_LANE_RATE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_STAR_CONF_CONF_QRTT_MAX_EXPECTED_LANE_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.star_conf[ prm_star_conf_index].conf.qrtt[ prm_qrtt_index].max_expected_lane_rate = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_STAR_CONF_CONF_QRTT_IS_ACTIVE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_STAR_CONF_CONF_QRTT_IS_ACTIVE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.star_conf[ prm_star_conf_index].conf.qrtt[ prm_qrtt_index].is_active = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_STAR_CONF_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_STAR_CONF_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.star_conf[ prm_star_conf_index].enable = (uint8)param_val->value.ulong_value;
  }

  }


  /* Call function */
  ret = fe600_srd_all_set(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    fe600_disp_result(ret, "fe600_srd_all_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: srd_all_get (section serdes)
 */
int
  ui_fe600_api_serdes_srd_all_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  FE600_SRD_ALL_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_serdes");
  soc_sand_proc_name = "fe600_srd_all_get";

  unit = fe600_get_default_unit();
  fe600_FE600_SRD_ALL_INFO_clear(&prm_info);

  /* Get parameters */

  /* Call function */
  ret = fe600_srd_all_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    fe600_disp_result(ret, "fe600_srd_all_get");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  fe600_FE600_SRD_ALL_INFO_print(FE600_SRD_TX_PHYS_CONF_MODE_INTERNAL,&prm_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: srd_auto_equalize (section serdes)
 */
int
  ui_fe600_api_serdes_srd_auto_equalize(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_lane_ndx_min, prm_lane_ndx_max, lane_i, nof_lanes;
  FE600_SRD_AEQ_MODE
    prm_mode=FE600_SRD_AEQ_MODE_BLIND;
  FE600_SRD_AEQ_STATUS
    prm_status;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_serdes");
  soc_sand_proc_name = "fe600_srd_auto_equalize";

  unit = fe600_get_default_unit();
  fe600_FE600_SRD_AEQ_STATUS_clear(&prm_status);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_AUTO_EQUALIZE_SRD_AUTO_EQUALIZE_LANE_NDX_ID,1))
  {
     UI_MACROS_GET_NUM_SYM_VAL_WITH_INDEX(PARAM_FE600_SRD_AUTO_EQUALIZE_SRD_AUTO_EQUALIZE_LANE_NDX_ID, 1);
     UI_MACROS_CHECK_GET_VAL_OF_ERROR;
     UI_MACROS_LOAD_LONG_VAL(prm_lane_ndx_min);

     prm_lane_ndx_max = prm_lane_ndx_min;
     UI_MACROS_GET_NUM_SYM_VAL_WITH_INDEX(PARAM_FE600_SRD_AUTO_EQUALIZE_SRD_AUTO_EQUALIZE_LANE_NDX_ID, 2);
    
     if (ui_ret)
     {
       ui_ret = 0;
     }
     else
     {
       UI_MACROS_CHECK_GET_VAL_OF_ERROR;
       UI_MACROS_LOAD_LONG_VAL(prm_lane_ndx_max);
     }
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter lane_ndx after srd_auto_equalize***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_AUTO_EQUALIZE_SRD_AUTO_EQUALIZE_MODE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FE600_SRD_AUTO_EQUALIZE_SRD_AUTO_EQUALIZE_MODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mode = param_val->numeric_equivalent;
  }

  nof_lanes = (prm_lane_ndx_min==FE600_SRD_NOF_LANES)?((uint32)FE600_SRD_NOF_LANES):(prm_lane_ndx_max-prm_lane_ndx_min+1);
  for(lane_i=(prm_lane_ndx_min==FE600_SRD_NOF_LANES)?0:prm_lane_ndx_min;lane_i<((prm_lane_ndx_min==FE600_SRD_NOF_LANES)?FE600_SRD_NOF_LANES:(prm_lane_ndx_min+nof_lanes));lane_i++)
  {
    soc_sand_os_printf( "Lane %d:\n\r", lane_i);

    /* Call function */
    ret = fe600_srd_auto_equalize(
            unit,
            lane_i,
            prm_mode,
            &prm_status
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      fe600_disp_result(ret, "fe600_srd_auto_equalize");
      goto exit;
    }

    send_string_to_screen("--> status:", TRUE);
    fe600_FE600_SRD_AEQ_STATUS_print(&prm_status);

  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: srd_rx_phys_params_set (section serdes)
 */
int
  ui_fe600_api_serdes_srd_rx_phys_params_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_lane_ndx_min, prm_lane_ndx_max, lane_i, nof_lanes;
  FE600_SRD_RX_PHYS_INTERNAL_PARAMS
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_serdes");
  soc_sand_proc_name = "fe600_srd_rx_phys_params_set";

  unit = fe600_get_default_unit();
  fe600_FE600_SRD_RX_PHYS_INTERNAL_PARAMS_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_RX_PHYS_PARAMS_SET_SRD_RX_PHYS_PARAMS_SET_LANE_NDX_ID,1))
  {
     UI_MACROS_GET_NUM_SYM_VAL_WITH_INDEX(PARAM_FE600_SRD_RX_PHYS_PARAMS_SET_SRD_RX_PHYS_PARAMS_SET_LANE_NDX_ID, 1);
     UI_MACROS_CHECK_GET_VAL_OF_ERROR;
     UI_MACROS_LOAD_LONG_VAL(prm_lane_ndx_min);

     prm_lane_ndx_max = prm_lane_ndx_min;
     UI_MACROS_GET_NUM_SYM_VAL_WITH_INDEX(PARAM_FE600_SRD_RX_PHYS_PARAMS_SET_SRD_RX_PHYS_PARAMS_SET_LANE_NDX_ID, 2);
    
     if (ui_ret)
     {
       ui_ret = 0;
     }
     else
     {
       UI_MACROS_CHECK_GET_VAL_OF_ERROR;
       UI_MACROS_LOAD_LONG_VAL(prm_lane_ndx_max);
     }
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter lane_ndx after srd_rx_phys_params_set***", TRUE);
    goto exit;
  }

  nof_lanes = (prm_lane_ndx_min==FE600_SRD_NOF_LANES)?((uint32)FE600_SRD_NOF_LANES):(prm_lane_ndx_max-prm_lane_ndx_min+1);
  for(lane_i=(prm_lane_ndx_min==FE600_SRD_NOF_LANES)?0:prm_lane_ndx_min;lane_i<((prm_lane_ndx_min==FE600_SRD_NOF_LANES)?FE600_SRD_NOF_LANES:(prm_lane_ndx_min+nof_lanes));lane_i++)
  {

    /* This is a set function, so call GET function first */
    ret = fe600_srd_rx_phys_params_get(
            unit,
            lane_i,
            &prm_info
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      fe600_disp_result(ret, "fe600_srd_rx_phys_params_get");
      goto exit;
    }

    if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_RX_PHYS_PARAMS_SET_SRD_RX_PHYS_PARAMS_SET_INFO_G1CNT_ID,1))
    {
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_RX_PHYS_PARAMS_SET_SRD_RX_PHYS_PARAMS_SET_INFO_G1CNT_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      prm_info.g1cnt = (uint8)param_val->value.ulong_value;
    }

    if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_RX_PHYS_PARAMS_SET_SRD_RX_PHYS_PARAMS_SET_INFO_TLTH_ID,1))
    {
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_RX_PHYS_PARAMS_SET_SRD_RX_PHYS_PARAMS_SET_INFO_TLTH_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      prm_info.tlth = (uint8)param_val->value.ulong_value;
    }

    if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_RX_PHYS_PARAMS_SET_SRD_RX_PHYS_PARAMS_SET_INFO_DFELTH_ID,1))
    {
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_RX_PHYS_PARAMS_SET_SRD_RX_PHYS_PARAMS_SET_INFO_DFELTH_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      prm_info.dfelth = (uint8)param_val->value.ulong_value;
    }

    if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_RX_PHYS_PARAMS_SET_SRD_RX_PHYS_PARAMS_SET_INFO_Z1CNT_ID,1))
    {
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_RX_PHYS_PARAMS_SET_SRD_RX_PHYS_PARAMS_SET_INFO_Z1CNT_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      prm_info.z1cnt = (uint8)param_val->value.ulong_value;
    }

    if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_RX_PHYS_PARAMS_SET_SRD_RX_PHYS_PARAMS_SET_INFO_ZCNT_ID,1))
    {
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_RX_PHYS_PARAMS_SET_SRD_RX_PHYS_PARAMS_SET_INFO_ZCNT_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      prm_info.zcnt = (uint8)param_val->value.ulong_value;
    }


    /* Call function */
    ret = fe600_srd_rx_phys_params_set(
            unit,
            lane_i,
            &prm_info
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      fe600_disp_result(ret, "fe600_srd_rx_phys_params_set");
      goto exit;
    }
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: srd_rx_phys_params_get (section serdes)
 */
int
  ui_fe600_api_serdes_srd_rx_phys_params_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_lane_ndx_min, prm_lane_ndx_max, lane_i, nof_lanes;
  FE600_SRD_RX_PHYS_INTERNAL_PARAMS
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_serdes");
  soc_sand_proc_name = "fe600_srd_rx_phys_params_get";

  unit = fe600_get_default_unit();
  fe600_FE600_SRD_RX_PHYS_INTERNAL_PARAMS_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_RX_PHYS_PARAMS_GET_SRD_RX_PHYS_PARAMS_GET_LANE_NDX_ID,1))
  {
     UI_MACROS_GET_NUM_SYM_VAL_WITH_INDEX(PARAM_FE600_SRD_RX_PHYS_PARAMS_GET_SRD_RX_PHYS_PARAMS_GET_LANE_NDX_ID, 1);
     UI_MACROS_CHECK_GET_VAL_OF_ERROR;
     UI_MACROS_LOAD_LONG_VAL(prm_lane_ndx_min);

     prm_lane_ndx_max = prm_lane_ndx_min;
     UI_MACROS_GET_NUM_SYM_VAL_WITH_INDEX(PARAM_FE600_SRD_RX_PHYS_PARAMS_GET_SRD_RX_PHYS_PARAMS_GET_LANE_NDX_ID, 2);
    
     if (ui_ret)
     {
       ui_ret = 0;
     }
     else
     {
       UI_MACROS_CHECK_GET_VAL_OF_ERROR;
       UI_MACROS_LOAD_LONG_VAL(prm_lane_ndx_max);
     }
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter lane_ndx after srd_rx_phys_params_get***", TRUE);
    goto exit;
  }

  nof_lanes = (prm_lane_ndx_min==FE600_SRD_NOF_LANES)?((uint32)FE600_SRD_NOF_LANES):(prm_lane_ndx_max-prm_lane_ndx_min+1);
  for(lane_i=(prm_lane_ndx_min==FE600_SRD_NOF_LANES)?0:prm_lane_ndx_min;lane_i<((prm_lane_ndx_min==FE600_SRD_NOF_LANES)?FE600_SRD_NOF_LANES:(prm_lane_ndx_min+nof_lanes));lane_i++)
  {
    soc_sand_os_printf( "Lane %d:\n\r", lane_i);

    /* Call function */
    ret = fe600_srd_rx_phys_params_get(
            unit,
            lane_i,
            &prm_info
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      fe600_disp_result(ret, "fe600_srd_rx_phys_params_get");
      goto exit;
    }

    send_string_to_screen("--> info:", TRUE);
    fe600_FE600_SRD_RX_PHYS_INTERNAL_PARAMS_print(&prm_info);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif
#ifdef UI_FE600_SERDES_UTILS
/********************************************************************
 *  Function handler: srd_reg_write (section serdes_utils)
 */
int
  ui_fe600_api_serdes_utils_srd_reg_write(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  FE600_SRD_ENTITY_TYPE
    prm_entity;
  uint32
    prm_entity_ndx, lane_i, nof_lanes;
  FE600_SRD_REGS_ADDR
    prm_reg;
  uint8
    prm_val=0;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_serdes_utils");
  soc_sand_proc_name = "fe600_srd_reg_write";

  unit = fe600_get_default_unit();
  fe600_FE600_SRD_REGS_ADDR_clear(&prm_reg);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_REG_WRITE_SRD_REG_WRITE_ENTITY_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_REG_WRITE_SRD_REG_WRITE_ENTITY_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entity_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entity_ndx after srd_reg_write***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_REG_WRITE_SRD_REG_WRITE_ENTITY_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FE600_SRD_REG_WRITE_SRD_REG_WRITE_ENTITY_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entity = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entity_ndx after srd_reg_write***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_REG_WRITE_SRD_REG_WRITE_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_REG_WRITE_SRD_REG_WRITE_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_reg.offset = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_REG_WRITE_SRD_REG_WRITE_REG_ELEMENT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_REG_WRITE_SRD_REG_WRITE_REG_ELEMENT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_reg.element = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_REG_WRITE_SRD_REG_WRITE_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_REG_WRITE_SRD_REG_WRITE_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_val = (uint8)param_val->value.ulong_value;
  }


  if((prm_entity == FE600_SRD_ENTITY_TYPE_LANE) && (prm_entity_ndx == FE600_NOF_LINKS))
  {
    uint32 prm_lane_ndx = prm_entity_ndx;
    nof_lanes = (prm_lane_ndx==FE600_NOF_LINKS)?((uint32)FE600_NOF_LINKS):(prm_lane_ndx+1);
    for(lane_i=(prm_lane_ndx==FE600_NOF_LINKS)?0:prm_lane_ndx;lane_i<nof_lanes;lane_i++)
    {
      /* Call function */
      ret = fe600_srd_reg_write(
        unit,
        prm_entity,
        lane_i,
        &prm_reg,
        prm_val
        );
      if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
      {
        send_string_to_screen(" *** fe600_srd_reg_write - FAIL", TRUE);
        fe600_disp_result(ret, "fe600_srd_reg_write");
        goto exit;
      }
    }

  }
  else
  {
    /* Call function */
    ret = fe600_srd_reg_write(
      unit,
      prm_entity,
      prm_entity_ndx,
      &prm_reg,
      prm_val
      );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
    fe600_disp_result(ret, "fe600_srd_reg_write");
    goto exit;
    }
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: srd_reg_read (section serdes_utils)
 */
int
  ui_fe600_api_serdes_utils_srd_reg_read(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  FE600_SRD_ENTITY_TYPE
    prm_entity=FE600_SRD_ENTITY_TYPE_LANE;
  uint32
    prm_entity_ndx, lane_i, nof_lanes;
  FE600_SRD_REGS_ADDR
    prm_reg;
  uint8
    prm_val=0;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_serdes_utils");
  soc_sand_proc_name = "fe600_srd_reg_read";

  unit = fe600_get_default_unit();
  fe600_FE600_SRD_REGS_ADDR_clear(&prm_reg);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_REG_READ_SRD_REG_READ_ENTITY_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_REG_READ_SRD_REG_READ_ENTITY_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entity_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entity_ndx after srd_reg_read***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_REG_READ_SRD_REG_READ_ENTITY_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FE600_SRD_REG_READ_SRD_REG_READ_ENTITY_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entity = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_REG_READ_SRD_REG_READ_REG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_REG_READ_SRD_REG_READ_REG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_reg.offset = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_REG_READ_SRD_REG_READ_REG_ELEMENT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_REG_READ_SRD_REG_READ_REG_ELEMENT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_reg.element = (uint32)param_val->value.ulong_value;
  }

  if((prm_entity == FE600_SRD_ENTITY_TYPE_LANE) && (prm_entity_ndx == FE600_NOF_LINKS))
  {
    uint32 prm_lane_ndx = prm_entity_ndx;
    nof_lanes = (prm_lane_ndx==FE600_NOF_LINKS)?((uint32)FE600_NOF_LINKS):(prm_lane_ndx+1);
    for(lane_i=(prm_lane_ndx==FE600_NOF_LINKS)?0:prm_lane_ndx;lane_i<nof_lanes;lane_i++)
    {
      /* Call function */
      ret = fe600_srd_reg_read(
        unit,
        prm_entity,
        lane_i,
        &prm_reg,
        &prm_val
        );
      if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
      {
        send_string_to_screen(" *** fe600_srd_reg_read - FAIL", TRUE);
        fe600_disp_result(ret, "fe600_srd_reg_read");
        goto exit;
      }
      send_string_to_screen(" ---> fe600_srd_reg_read - SUCCEEDED", TRUE);
    }

  }
  else
  {
    /* Call function */
    ret = fe600_srd_reg_read(
      unit,
      prm_entity,
      prm_entity_ndx,
      &prm_reg,
      &prm_val
      );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      fe600_disp_result(ret, "fe600_srd_reg_read");
      goto exit;
    }
  }

  soc_sand_os_printf( "val: 0x%.2lx\n\r",prm_val);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: srd_fld_write (section serdes_utils)
 */
int
  ui_fe600_api_serdes_utils_srd_fld_write(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  FE600_SRD_ENTITY_TYPE
    prm_entity=FE600_SRD_ENTITY_TYPE_LANE;
  uint32
    prm_entity_ndx;
  FE600_SRD_REGS_FIELD
    prm_fld;
  uint8
    prm_val=0;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_serdes_utils");
  soc_sand_proc_name = "fe600_srd_fld_write";

  unit = fe600_get_default_unit();
  fe600_FE600_SRD_REGS_FIELD_clear(&prm_fld);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_FLD_WRITE_SRD_FLD_WRITE_ENTITY_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_FLD_WRITE_SRD_FLD_WRITE_ENTITY_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entity_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entity_ndx after srd_fld_write***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_FLD_WRITE_SRD_FLD_WRITE_ENTITY_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FE600_SRD_FLD_WRITE_SRD_FLD_WRITE_ENTITY_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entity = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_FLD_WRITE_SRD_FLD_WRITE_FLD_LSB_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_FLD_WRITE_SRD_FLD_WRITE_FLD_LSB_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_fld.lsb = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_FLD_WRITE_SRD_FLD_WRITE_FLD_MSB_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_FLD_WRITE_SRD_FLD_WRITE_FLD_MSB_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_fld.msb = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_FLD_WRITE_SRD_FLD_WRITE_FLD_ADDR_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_FLD_WRITE_SRD_FLD_WRITE_FLD_ADDR_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_fld.addr.offset = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_FLD_WRITE_SRD_FLD_WRITE_FLD_ADDR_ELEMENT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_FLD_WRITE_SRD_FLD_WRITE_FLD_ADDR_ELEMENT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_fld.addr.element = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_FLD_WRITE_SRD_FLD_WRITE_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_FLD_WRITE_SRD_FLD_WRITE_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_val = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = fe600_srd_fld_write(
          unit,
          prm_entity,
          prm_entity_ndx,
          &prm_fld,
          prm_val
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    fe600_disp_result(ret, "fe600_srd_fld_write");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: srd_fld_read (section serdes_utils)
 */
int
  ui_fe600_api_serdes_utils_srd_fld_read(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  FE600_SRD_ENTITY_TYPE
    prm_entity=FE600_SRD_ENTITY_TYPE_LANE;
  uint32
    prm_entity_ndx = 0;
  FE600_SRD_REGS_FIELD
    prm_fld;
  uint8
    prm_val = 0;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_serdes_utils");
  soc_sand_proc_name = "fe600_srd_fld_read";

  unit = fe600_get_default_unit();
  fe600_FE600_SRD_REGS_FIELD_clear(&prm_fld);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_FLD_READ_SRD_FLD_READ_ENTITY_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_FLD_READ_SRD_FLD_READ_ENTITY_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entity_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entity_ndx after srd_fld_read***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_FLD_READ_SRD_FLD_READ_ENTITY_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FE600_SRD_FLD_READ_SRD_FLD_READ_ENTITY_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entity = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_FLD_READ_SRD_FLD_READ_FLD_LSB_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_FLD_READ_SRD_FLD_READ_FLD_LSB_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_fld.lsb = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_FLD_READ_SRD_FLD_READ_FLD_MSB_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_FLD_READ_SRD_FLD_READ_FLD_MSB_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_fld.msb = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_FLD_READ_SRD_FLD_READ_FLD_ADDR_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_FLD_READ_SRD_FLD_READ_FLD_ADDR_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_fld.addr.offset = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_FLD_READ_SRD_FLD_READ_FLD_ADDR_ELEMENT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_FLD_READ_SRD_FLD_READ_FLD_ADDR_ELEMENT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_fld.addr.element = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = fe600_srd_fld_read(
          unit,
          prm_entity,
          prm_entity_ndx,
          &prm_fld,
          &prm_val
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    fe600_disp_result(ret, "fe600_srd_fld_read");
    goto exit;
  }

  soc_sand_os_printf( "val: 0x%.2lx\n\r",prm_val);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: srd_link_rx_eye_monitor (section serdes_utils)
 */
int
  ui_fe600_api_serdes_utils_srd_link_rx_eye_monitor(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_lane_ndx_min, prm_lane_ndx_max, lane_i, nof_lanes;
  FE600_SRD_LANE_EQ_MODE
    prm_equalizer_mode;
  uint32
    prm_eye_sample;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_serdes_utils");
  soc_sand_proc_name = "fe600_srd_link_rx_eye_monitor";

  unit = fe600_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_LINK_RX_EYE_MONITOR_SRD_LINK_RX_EYE_MONITOR_LANE_NDX_ID,1))
  {
     UI_MACROS_GET_NUM_SYM_VAL_WITH_INDEX(PARAM_FE600_SRD_LINK_RX_EYE_MONITOR_SRD_LINK_RX_EYE_MONITOR_LANE_NDX_ID, 1);
     UI_MACROS_CHECK_GET_VAL_OF_ERROR;
     UI_MACROS_LOAD_LONG_VAL(prm_lane_ndx_min);

     prm_lane_ndx_max = prm_lane_ndx_min;
     UI_MACROS_GET_NUM_SYM_VAL_WITH_INDEX(PARAM_FE600_SRD_LINK_RX_EYE_MONITOR_SRD_LINK_RX_EYE_MONITOR_LANE_NDX_ID, 2);
    
     if (ui_ret)
     {
       ui_ret = 0;
     }
     else
     {
       UI_MACROS_CHECK_GET_VAL_OF_ERROR;
       UI_MACROS_LOAD_LONG_VAL(prm_lane_ndx_max);
     }
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter lane_ndx after srd_link_rx_eye_monitor***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_LINK_RX_EYE_MONITOR_SRD_LINK_RX_EYE_MONITOR_EQUALIZER_MODE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FE600_SRD_LINK_RX_EYE_MONITOR_SRD_LINK_RX_EYE_MONITOR_EQUALIZER_MODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_equalizer_mode = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter lane_ndx after srd_link_rx_eye_monitor***", TRUE);
    goto exit;
  }

  nof_lanes = (prm_lane_ndx_min==FE600_SRD_NOF_LANES)?((uint32)FE600_SRD_NOF_LANES):(prm_lane_ndx_max-prm_lane_ndx_min+1);
  for(lane_i=(prm_lane_ndx_min==FE600_SRD_NOF_LANES)?0:prm_lane_ndx_min;lane_i<((prm_lane_ndx_min==FE600_SRD_NOF_LANES)?FE600_SRD_NOF_LANES:(prm_lane_ndx_min+nof_lanes));lane_i++)
  {
    /* Call function */
    ret = fe600_srd_link_rx_eye_monitor(
            unit,
            lane_i,
            prm_equalizer_mode,
            &prm_eye_sample
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      fe600_disp_result(ret, "fe600_srd_link_rx_eye_monitor");
      goto exit;
    }

    soc_sand_os_printf( "eye_sample: %lu\n\r",prm_eye_sample);

  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: srd_lane_loopback_mode_set (section serdes_utils)
 */
int
  ui_fe600_api_serdes_utils_srd_lane_loopback_mode_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_lane_ndx_min, prm_lane_ndx_max,lane_i,nof_lanes;
  FE600_SRD_LANE_LOOPBACK_MODE
    prm_loopback_mode;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_serdes_utils");
  soc_sand_proc_name = "fe600_srd_lane_loopback_mode_set";

  unit = fe600_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_LANE_LOOPBACK_MODE_SET_SRD_LANE_LOOPBACK_MODE_SET_LANE_NDX_ID,1))
  {
     UI_MACROS_GET_NUM_SYM_VAL_WITH_INDEX(PARAM_FE600_SRD_LANE_LOOPBACK_MODE_SET_SRD_LANE_LOOPBACK_MODE_SET_LANE_NDX_ID, 1);
     UI_MACROS_CHECK_GET_VAL_OF_ERROR;
     UI_MACROS_LOAD_LONG_VAL(prm_lane_ndx_min);

     prm_lane_ndx_max = prm_lane_ndx_min;
     UI_MACROS_GET_NUM_SYM_VAL_WITH_INDEX(PARAM_FE600_SRD_LANE_LOOPBACK_MODE_SET_SRD_LANE_LOOPBACK_MODE_SET_LANE_NDX_ID, 2);
    
     if (ui_ret)
     {
       ui_ret = 0;
     }
     else
     {
       UI_MACROS_CHECK_GET_VAL_OF_ERROR;
       UI_MACROS_LOAD_LONG_VAL(prm_lane_ndx_max);
     }
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter lane_ndx after srd_lane_loopback_mode_set***", TRUE);
    goto exit;
  }


  nof_lanes = (prm_lane_ndx_min==FE600_SRD_NOF_LANES)?((uint32)FE600_SRD_NOF_LANES):(prm_lane_ndx_max-prm_lane_ndx_min+1);
  for(lane_i=(prm_lane_ndx_min==FE600_SRD_NOF_LANES)?0:prm_lane_ndx_min;lane_i<((prm_lane_ndx_min==FE600_SRD_NOF_LANES)?FE600_SRD_NOF_LANES:(prm_lane_ndx_min+nof_lanes));lane_i++)
  {
    /* This is a set function, so call GET function first */
    ret = fe600_srd_lane_loopback_mode_get(
            unit,
            lane_i,
            &prm_loopback_mode
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      fe600_disp_result(ret, "fe600_srd_lane_loopback_mode_get");
      goto exit;
    }

    if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_LANE_LOOPBACK_MODE_SET_SRD_LANE_LOOPBACK_MODE_SET_LOOPBACK_MODE_ID,1))
    {
      UI_MACROS_GET_SYMBOL_VAL(PARAM_FE600_SRD_LANE_LOOPBACK_MODE_SET_SRD_LANE_LOOPBACK_MODE_SET_LOOPBACK_MODE_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      prm_loopback_mode = param_val->numeric_equivalent;
    }


    /* Call function */
    ret = fe600_srd_lane_loopback_mode_set(
            unit,
            lane_i,
            prm_loopback_mode
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      fe600_disp_result(ret, "fe600_srd_lane_loopback_mode_set");
      goto exit;
    }
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: srd_lane_loopback_mode_get (section serdes_utils)
 */
int
  ui_fe600_api_serdes_utils_srd_lane_loopback_mode_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_lane_ndx_min, prm_lane_ndx_max, lane_i, nof_lanes;
  FE600_SRD_LANE_LOOPBACK_MODE
    prm_loopback_mode;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_serdes_utils");
  soc_sand_proc_name = "fe600_srd_lane_loopback_mode_get";

  unit = fe600_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_LANE_LOOPBACK_MODE_GET_SRD_LANE_LOOPBACK_MODE_GET_LANE_NDX_ID,1))
  {
     UI_MACROS_GET_NUM_SYM_VAL_WITH_INDEX(PARAM_FE600_SRD_LANE_LOOPBACK_MODE_GET_SRD_LANE_LOOPBACK_MODE_GET_LANE_NDX_ID, 1);
     UI_MACROS_CHECK_GET_VAL_OF_ERROR;
     UI_MACROS_LOAD_LONG_VAL(prm_lane_ndx_min);

     prm_lane_ndx_max = prm_lane_ndx_min;
     UI_MACROS_GET_NUM_SYM_VAL_WITH_INDEX(PARAM_FE600_SRD_LANE_LOOPBACK_MODE_GET_SRD_LANE_LOOPBACK_MODE_GET_LANE_NDX_ID, 2);
    
     if (ui_ret)
     {
       ui_ret = 0;
     }
     else
     {
       UI_MACROS_CHECK_GET_VAL_OF_ERROR;
       UI_MACROS_LOAD_LONG_VAL(prm_lane_ndx_max);
     }
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter lane_ndx after srd_lane_loopback_mode_get***", TRUE);
    goto exit;
  }


  nof_lanes = (prm_lane_ndx_min==FE600_SRD_NOF_LANES)?((uint32)FE600_SRD_NOF_LANES):(prm_lane_ndx_max-prm_lane_ndx_min+1);
  for(lane_i=(prm_lane_ndx_min==FE600_SRD_NOF_LANES)?0:prm_lane_ndx_min;lane_i<((prm_lane_ndx_min==FE600_SRD_NOF_LANES)?FE600_SRD_NOF_LANES:(prm_lane_ndx_min+nof_lanes));lane_i++)
  {
    soc_sand_os_printf( "Lane %d:\n\r", lane_i);

    /* Call function */
    ret = fe600_srd_lane_loopback_mode_get(
            unit,
            lane_i,
            &prm_loopback_mode
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      fe600_disp_result(ret, "fe600_srd_lane_loopback_mode_get");
      goto exit;
    }

    soc_sand_os_printf( "loopback_mode: %s\n\r",fe600_FE600_SRD_LANE_LOOPBACK_MODE_to_string(prm_loopback_mode));
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: srd_prbs_mode_set (section serdes_utils)
 */
int
  ui_fe600_api_serdes_utils_srd_prbs_mode_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_lane_ndx_min, prm_lane_ndx_max, lane_i, nof_lanes;
  FE600_CONNECTION_DIRECTION
    prm_direction_ndx = FE600_CONNECTION_DIRECTION_BOTH;
  FE600_SRD_PRBS_MODE
    prm_mode_rx,
    prm_mode_tx;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_serdes_utils");
  soc_sand_proc_name = "fe600_srd_prbs_mode_set";

  unit = fe600_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_PRBS_MODE_SET_SRD_PRBS_MODE_SET_LANE_NDX_ID,1))
  {
     UI_MACROS_GET_NUM_SYM_VAL_WITH_INDEX(PARAM_FE600_SRD_PRBS_MODE_SET_SRD_PRBS_MODE_SET_LANE_NDX_ID, 1);
     UI_MACROS_CHECK_GET_VAL_OF_ERROR;
     UI_MACROS_LOAD_LONG_VAL(prm_lane_ndx_min);

     prm_lane_ndx_max = prm_lane_ndx_min;
     UI_MACROS_GET_NUM_SYM_VAL_WITH_INDEX(PARAM_FE600_SRD_PRBS_MODE_SET_SRD_PRBS_MODE_SET_LANE_NDX_ID, 2);
    
     if (ui_ret)
     {
       ui_ret = 0;
     }
     else
     {
       UI_MACROS_CHECK_GET_VAL_OF_ERROR;
       UI_MACROS_LOAD_LONG_VAL(prm_lane_ndx_max);
     }
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter lane_ndx after srd_prbs_mode_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_PRBS_MODE_SET_SRD_PRBS_MODE_SET_DIRECTION_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FE600_SRD_PRBS_MODE_SET_SRD_PRBS_MODE_SET_DIRECTION_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_direction_ndx = param_val->numeric_equivalent;
  }

  nof_lanes = (prm_lane_ndx_min==FE600_SRD_NOF_LANES)?((uint32)FE600_SRD_NOF_LANES):(prm_lane_ndx_max-prm_lane_ndx_min+1);
  for(lane_i=(prm_lane_ndx_min==FE600_SRD_NOF_LANES)?0:prm_lane_ndx_min;lane_i<((prm_lane_ndx_min==FE600_SRD_NOF_LANES)?FE600_SRD_NOF_LANES:(prm_lane_ndx_min+nof_lanes));lane_i++)
  {

    /* This is a set function, so call GET function first */
    ret = fe600_srd_prbs_mode_get(
            unit,
            lane_i,
            &prm_mode_rx,
            &prm_mode_tx
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      fe600_disp_result(ret, "fe600_srd_prbs_mode_get");
      goto exit;
    }

    if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_PRBS_MODE_SET_SRD_PRBS_MODE_SET_MODE_ID,1))
    {
      UI_MACROS_GET_SYMBOL_VAL(PARAM_FE600_SRD_PRBS_MODE_SET_SRD_PRBS_MODE_SET_MODE_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      prm_mode_rx = param_val->numeric_equivalent;
      prm_mode_tx = param_val->numeric_equivalent;
    }


    /* Call function */
    ret = fe600_srd_prbs_mode_set(
            unit,
            lane_i,
            prm_direction_ndx,
            prm_mode_rx
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      fe600_disp_result(ret, "fe600_srd_prbs_mode_set");
      goto exit;
    }

  }
  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: srd_prbs_mode_get (section serdes_utils)
 */
int
  ui_fe600_api_serdes_utils_srd_prbs_mode_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_lane_ndx_min, prm_lane_ndx_max, lane_i, nof_lanes;
  FE600_SRD_PRBS_MODE
    prm_mode_rx,
    prm_mode_tx;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_serdes_utils");
  soc_sand_proc_name = "fe600_srd_prbs_mode_get";

  unit = fe600_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_PRBS_MODE_GET_SRD_PRBS_MODE_GET_LANE_NDX_ID,1))
  {
     UI_MACROS_GET_NUM_SYM_VAL_WITH_INDEX(PARAM_FE600_SRD_PRBS_MODE_GET_SRD_PRBS_MODE_GET_LANE_NDX_ID, 1);
     UI_MACROS_CHECK_GET_VAL_OF_ERROR;
     UI_MACROS_LOAD_LONG_VAL(prm_lane_ndx_min);

     prm_lane_ndx_max = prm_lane_ndx_min;
     UI_MACROS_GET_NUM_SYM_VAL_WITH_INDEX(PARAM_FE600_SRD_PRBS_MODE_GET_SRD_PRBS_MODE_GET_LANE_NDX_ID, 2);
    
     if (ui_ret)
     {
       ui_ret = 0;
     }
     else
     {
       UI_MACROS_CHECK_GET_VAL_OF_ERROR;
       UI_MACROS_LOAD_LONG_VAL(prm_lane_ndx_max);
     }
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter lane_ndx after srd_prbs_mode_get***", TRUE);
    goto exit;
  }


  nof_lanes = (prm_lane_ndx_min==FE600_SRD_NOF_LANES)?((uint32)FE600_SRD_NOF_LANES):(prm_lane_ndx_max-prm_lane_ndx_min+1);
  for(lane_i=(prm_lane_ndx_min==FE600_SRD_NOF_LANES)?0:prm_lane_ndx_min;lane_i<((prm_lane_ndx_min==FE600_SRD_NOF_LANES)?FE600_SRD_NOF_LANES:(prm_lane_ndx_min+nof_lanes));lane_i++)
  {
    soc_sand_os_printf( "Lane %d:\n\r", lane_i);

    /* Call function */
    ret = fe600_srd_prbs_mode_get(
            unit,
            lane_i,
            &prm_mode_rx,
            &prm_mode_tx
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      fe600_disp_result(ret, "fe600_srd_prbs_mode_get");
      goto exit;
    }

    send_string_to_screen("--> RX: \n\r", TRUE);
    soc_sand_os_printf( "mode: %s\n\r",fe600_FE600_SRD_PRBS_MODE_to_string(prm_mode_rx));
    send_string_to_screen("--> TX: \n\r", TRUE);
    soc_sand_os_printf( "mode: %s\n\r",fe600_FE600_SRD_PRBS_MODE_to_string(prm_mode_tx));
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: srd_prbs_start (section serdes_utils)
 */
int
  ui_fe600_api_serdes_utils_srd_prbs_start(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_lane_ndx_min, prm_lane_ndx_max, lane_i, nof_lanes;
  FE600_CONNECTION_DIRECTION
    prm_direction_ndx = FE600_CONNECTION_DIRECTION_BOTH;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_serdes_utils");
  soc_sand_proc_name = "fe600_srd_prbs_start";

  unit = fe600_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_PRBS_START_SRD_PRBS_START_LANE_NDX_ID,1))
  {
     UI_MACROS_GET_NUM_SYM_VAL_WITH_INDEX(PARAM_FE600_SRD_PRBS_START_SRD_PRBS_START_LANE_NDX_ID, 1);
     UI_MACROS_CHECK_GET_VAL_OF_ERROR;
     UI_MACROS_LOAD_LONG_VAL(prm_lane_ndx_min);

     prm_lane_ndx_max = prm_lane_ndx_min;
     UI_MACROS_GET_NUM_SYM_VAL_WITH_INDEX(PARAM_FE600_SRD_PRBS_START_SRD_PRBS_START_LANE_NDX_ID, 2);
    
     if (ui_ret)
     {
       ui_ret = 0;
     }
     else
     {
       UI_MACROS_CHECK_GET_VAL_OF_ERROR;
       UI_MACROS_LOAD_LONG_VAL(prm_lane_ndx_max);
     }
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter lane_ndx after srd_prbs_start***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_PRBS_START_SRD_PRBS_START_DIRECTION_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FE600_SRD_PRBS_START_SRD_PRBS_START_DIRECTION_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_direction_ndx = param_val->numeric_equivalent;
  }

  nof_lanes = (prm_lane_ndx_min==FE600_SRD_NOF_LANES)?((uint32)FE600_SRD_NOF_LANES):(prm_lane_ndx_max-prm_lane_ndx_min+1);
  for(lane_i=(prm_lane_ndx_min==FE600_SRD_NOF_LANES)?0:prm_lane_ndx_min;lane_i<((prm_lane_ndx_min==FE600_SRD_NOF_LANES)?FE600_SRD_NOF_LANES:(prm_lane_ndx_min+nof_lanes));lane_i++)
  {
    soc_sand_os_printf( "Lane %d:\n\r", lane_i);

    /* Call function */
    ret = fe600_srd_prbs_start(
            unit,
            lane_i,
            prm_direction_ndx
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      fe600_disp_result(ret, "fe600_srd_prbs_start");
      goto exit;
    }
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: srd_prbs_stop (section serdes_utils)
 */
int
  ui_fe600_api_serdes_utils_srd_prbs_stop(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_lane_ndx_min, prm_lane_ndx_max, lane_i, nof_lanes;
  FE600_CONNECTION_DIRECTION
    prm_direction_ndx = FE600_CONNECTION_DIRECTION_BOTH;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_serdes_utils");
  soc_sand_proc_name = "fe600_srd_prbs_stop";

  unit = fe600_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_PRBS_STOP_SRD_PRBS_STOP_LANE_NDX_ID,1))
  {
     UI_MACROS_GET_NUM_SYM_VAL_WITH_INDEX(PARAM_FE600_SRD_PRBS_STOP_SRD_PRBS_STOP_LANE_NDX_ID, 1);
     UI_MACROS_CHECK_GET_VAL_OF_ERROR;
     UI_MACROS_LOAD_LONG_VAL(prm_lane_ndx_min);

     prm_lane_ndx_max = prm_lane_ndx_min;
     UI_MACROS_GET_NUM_SYM_VAL_WITH_INDEX(PARAM_FE600_SRD_PRBS_STOP_SRD_PRBS_STOP_LANE_NDX_ID, 2);
    
     if (ui_ret)
     {
       ui_ret = 0;
     }
     else
     {
       UI_MACROS_CHECK_GET_VAL_OF_ERROR;
       UI_MACROS_LOAD_LONG_VAL(prm_lane_ndx_max);
     }
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter lane_ndx after srd_prbs_stop***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_PRBS_STOP_SRD_PRBS_STOP_DIRECTION_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FE600_SRD_PRBS_STOP_SRD_PRBS_STOP_DIRECTION_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_direction_ndx = param_val->numeric_equivalent;
  }

  /* Call function */
  nof_lanes = (prm_lane_ndx_min==FE600_SRD_NOF_LANES)?((uint32)FE600_SRD_NOF_LANES):(prm_lane_ndx_max-prm_lane_ndx_min+1);
  for(lane_i=(prm_lane_ndx_min==FE600_SRD_NOF_LANES)?0:prm_lane_ndx_min;lane_i<((prm_lane_ndx_min==FE600_SRD_NOF_LANES)?FE600_SRD_NOF_LANES:(prm_lane_ndx_min+nof_lanes));lane_i++)
  {
    ret = fe600_srd_prbs_stop(
            unit,
            lane_i,
            prm_direction_ndx
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      fe600_disp_result(ret, "fe600_srd_prbs_stop");
      goto exit;
    }
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: srd_prbs_get_and_clear_stat (section serdes_utils)
 */
int
  ui_fe600_api_serdes_utils_srd_prbs_get_and_clear_stat(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_lane_ndx_min, prm_lane_ndx_max, lane_i, nof_lanes;
  FE600_SRD_PRBS_RX_STATUS
    prm_status;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_serdes_utils");
  soc_sand_proc_name = "fe600_srd_prbs_get_and_clear_stat";

  unit = fe600_get_default_unit();
  fe600_FE600_SRD_PRBS_RX_STATUS_clear(&prm_status);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_PRBS_GET_AND_CLEAR_STAT_SRD_PRBS_GET_AND_CLEAR_STAT_LANE_NDX_ID,1))
  {
     UI_MACROS_GET_NUM_SYM_VAL_WITH_INDEX(PARAM_FE600_SRD_PRBS_GET_AND_CLEAR_STAT_SRD_PRBS_GET_AND_CLEAR_STAT_LANE_NDX_ID, 1);
     UI_MACROS_CHECK_GET_VAL_OF_ERROR;
     UI_MACROS_LOAD_LONG_VAL(prm_lane_ndx_min);

     prm_lane_ndx_max = prm_lane_ndx_min;
     UI_MACROS_GET_NUM_SYM_VAL_WITH_INDEX(PARAM_FE600_SRD_PRBS_GET_AND_CLEAR_STAT_SRD_PRBS_GET_AND_CLEAR_STAT_LANE_NDX_ID, 2);
    
     if (ui_ret)
     {
       ui_ret = 0;
     }
     else
     {
       UI_MACROS_CHECK_GET_VAL_OF_ERROR;
       UI_MACROS_LOAD_LONG_VAL(prm_lane_ndx_max);
     }
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter lane_ndx after srd_prbs_get_and_clear_stat***", TRUE);
    goto exit;
  }

  nof_lanes = (prm_lane_ndx_min==FE600_SRD_NOF_LANES)?((uint32)FE600_SRD_NOF_LANES):(prm_lane_ndx_max-prm_lane_ndx_min+1);
  for(lane_i=(prm_lane_ndx_min==FE600_SRD_NOF_LANES)?0:prm_lane_ndx_min;lane_i<((prm_lane_ndx_min==FE600_SRD_NOF_LANES)?FE600_SRD_NOF_LANES:(prm_lane_ndx_min+nof_lanes));lane_i++)
  {
    soc_sand_os_printf( "Lane %d:\n\r", lane_i);

    /* Call function */
    ret = fe600_srd_prbs_get_and_clear_stat(
            unit,
            lane_i,
            &prm_status
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      fe600_disp_result(ret, "fe600_srd_prbs_get_and_clear_stat");
      goto exit;
    }

    send_string_to_screen("--> status:", TRUE);
    fe600_FE600_SRD_PRBS_RX_STATUS_print(&prm_status);
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: srd_lane_status_get (section serdes_utils)
 */
int
  ui_fe600_api_serdes_utils_srd_lane_status_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  uint32
    prm_lane_ndx_min, prm_lane_ndx_max, lane_i, nof_lanes;
  FE600_SRD_LANE_STATUS_INFO
    prm_lane_stt_info;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_serdes_utils");
  soc_sand_proc_name = "fe600_srd_lane_status_get";

  unit = fe600_get_default_unit();
  fe600_FE600_SRD_LANE_STATUS_INFO_clear(&prm_lane_stt_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_LANE_STATUS_GET_SRD_LANE_STATUS_GET_LANE_NDX_ID,1))
  {
     UI_MACROS_GET_NUM_SYM_VAL_WITH_INDEX(PARAM_FE600_SRD_LANE_STATUS_GET_SRD_LANE_STATUS_GET_LANE_NDX_ID, 1);
     UI_MACROS_CHECK_GET_VAL_OF_ERROR;
     UI_MACROS_LOAD_LONG_VAL(prm_lane_ndx_min);

     prm_lane_ndx_max = prm_lane_ndx_min;
     UI_MACROS_GET_NUM_SYM_VAL_WITH_INDEX(PARAM_FE600_SRD_LANE_STATUS_GET_SRD_LANE_STATUS_GET_LANE_NDX_ID, 2);
    
     if (ui_ret)
     {
       ui_ret = 0;
     }
     else
     {
       UI_MACROS_CHECK_GET_VAL_OF_ERROR;
       UI_MACROS_LOAD_LONG_VAL(prm_lane_ndx_max);
     }
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter lane_ndx after srd_lane_status_get***", TRUE);
    goto exit;
  }

  nof_lanes = (prm_lane_ndx_min==FE600_SRD_NOF_LANES)?((uint32)FE600_SRD_NOF_LANES):(prm_lane_ndx_max-prm_lane_ndx_min+1);
  for(lane_i=(prm_lane_ndx_min==FE600_SRD_NOF_LANES)?0:prm_lane_ndx_min;lane_i<((prm_lane_ndx_min==FE600_SRD_NOF_LANES)?FE600_SRD_NOF_LANES:(prm_lane_ndx_min+nof_lanes));lane_i++)
  {
    soc_sand_os_printf( "Lane %d:\n\r", lane_i);

    /* Call function */
    ret = fe600_srd_lane_status_get(
            unit,
            lane_i,
            &prm_lane_stt_info
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      fe600_disp_result(ret, "fe600_srd_lane_status_get");
      goto exit;
    }

    send_string_to_screen("--> lane_stt_info:", TRUE);
    fe600_FE600_SRD_LANE_STATUS_INFO_print(&prm_lane_stt_info);
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: srd_qrtt_status_get (section serdes_utils)
 */
int
  ui_fe600_api_serdes_utils_srd_qrtt_status_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_qrtt_id;
  FE600_SRD_QRTT_STATUS_INFO
    prm_qrtt_status;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_serdes_utils");
  soc_sand_proc_name = "fe600_srd_qrtt_status_get";

  unit = fe600_get_default_unit();
  fe600_FE600_SRD_QRTT_STATUS_INFO_clear(&prm_qrtt_status);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_QRTT_STATUS_GET_SRD_QRTT_STATUS_GET_QRTT_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_QRTT_STATUS_GET_SRD_QRTT_STATUS_GET_QRTT_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_qrtt_id = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter lane_ndx after srd_link_rx_eye_monitor***", TRUE);
    goto exit;
  }



  /* Call function */
  ret = fe600_srd_qrtt_status_get(
          unit,
          prm_qrtt_id,
          &prm_qrtt_status
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    fe600_disp_result(ret, "fe600_srd_qrtt_status_get");
    goto exit;
  }

  send_string_to_screen("--> qrtt_status:", TRUE);
  fe600_FE600_SRD_QRTT_STATUS_INFO_print(&prm_qrtt_status);


  goto exit;
exit:
  return ui_ret;
}


/********************************************************************
 *  Function handler: srd_eye_scan_run (section serdes_utils)
 */
int
  ui_fe600_api_serdes_utils_srd_eye_scan_run(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    indx,
    prm_nof_lane_index=0;
  FE600_SRD_EYE_SCAN_INFO
    prm_info;
  uint32
    res_i;
  FE600_SRD_EYE_SCAN_RES
    *prm_res = NULL;
  FE600_SRD_CNT_SRC
    counting_src = FE600_SRD_NOF_CNT_SRCS;
  FE600_SRD_TRAFFIC_SRC 
    traffic_src = FE600_SRD_NOF_TRAFFIC_SRCS;


  UI_MACROS_INIT_FUNCTION("ui_fe600_api_serdes_utils");
  soc_sand_proc_name = "fe600_srd_eye_scan_run";

  unit = fe600_get_default_unit();
  fe600_FE600_SRD_EYE_SCAN_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_EYE_SCAN_RUN_SRD_EYE_SCAN_RUN_INFO_TRAFFIC_SRC_ID,1))
  {
    UI_MACROS_GET_NUM_SYM_VAL(PARAM_FE600_SRD_EYE_SCAN_RUN_SRD_EYE_SCAN_RUN_INFO_TRAFFIC_SRC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    UI_MACROS_LOAD_LONG_VAL(traffic_src);
  }
  prm_info.traffic_src = traffic_src;

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_EYE_SCAN_RUN_SRD_EYE_SCAN_RUN_INFO_RESOLUTION_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_EYE_SCAN_RUN_SRD_EYE_SCAN_RUN_INFO_RESOLUTION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.resolution = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_EYE_SCAN_RUN_SRD_EYE_SCAN_RUN_INFO_TX_PRBS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_EYE_SCAN_RUN_SRD_EYE_SCAN_RUN_INFO_TX_PRBS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.params.prbs.is_tx_prbs_enabled = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_EYE_SCAN_RUN_SRD_EYE_SCAN_RUN_INFO_PRBS_DURATION_MIN_SEC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_EYE_SCAN_RUN_SRD_EYE_SCAN_RUN_INFO_PRBS_DURATION_MIN_SEC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.duration_min_sec = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_EYE_SCAN_RUN_SRD_EYE_SCAN_RUN_INFO_LANE_NDX_ID,1))
  {
    for (prm_nof_lane_index = 0; prm_nof_lane_index < FE600_SRD_NOF_LANES; ++prm_nof_lane_index, ui_ret = 0)
    {
      UI_MACROS_GET_NUM_SYM_VAL_WITH_INDEX(PARAM_FE600_SRD_EYE_SCAN_RUN_SRD_EYE_SCAN_RUN_INFO_LANE_NDX_ID, prm_nof_lane_index + 1);
      if (ui_ret)
      {
        ui_ret = 0;
        break;
      }
      UI_MACROS_LOAD_LONG_VAL(prm_info.lane_ndx[prm_nof_lane_index]);

      /*
       *  If the option 'all' links has been chosen
       */
      if (prm_info.lane_ndx[prm_nof_lane_index] == FE600_SRD_NOF_LANES)
      {
        for (indx = 0; indx < FE600_SRD_NOF_LANES; ++indx)
        {
          prm_info.lane_ndx[indx] = indx;
          prm_nof_lane_index = FE600_SRD_NOF_LANES;
        }
        break;
      }

    }
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_EYE_SCAN_RUN_SRD_EYE_SCAN_RUN_INFO_CRC_ID,1))
  {
    UI_MACROS_GET_NUM_SYM_VAL(PARAM_FE600_SRD_EYE_SCAN_RUN_SRD_EYE_SCAN_RUN_INFO_CRC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    UI_MACROS_LOAD_LONG_VAL(counting_src);
  }
  if (traffic_src == FE600_SRD_TRAFFIC_SRC_PRBS) 
  {
    prm_info.params.prbs.cnt_src = counting_src;
  } 
  else 
  {
    prm_info.params.ext.cnt_src = counting_src;
  }

  prm_info.nof_lane_ndx_entries = prm_nof_lane_index;

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_EYE_SCAN_RUN_SRD_EYE_SCAN_RUN_INFO_MODE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FE600_SRD_EYE_SCAN_RUN_SRD_EYE_SCAN_RUN_INFO_MODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.params.prbs.mode = param_val->numeric_equivalent;
  }

  /* allocate result structures */
  prm_res = sal_alloc_any_size(sizeof(FE600_SRD_EYE_SCAN_RES) * prm_info.nof_lane_ndx_entries);
  if (!prm_res)
  {
    send_string_to_screen(" *** sal_alloc_any_size - FAIL", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_EYE_SCAN_RUN_SRD_EYE_SCAN_RUN_INFO_RANGE_DFELTH_MAX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_EYE_SCAN_RUN_SRD_EYE_SCAN_RUN_INFO_RANGE_DFELTH_MAX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.range.dfelth_max = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_EYE_SCAN_RUN_SRD_EYE_SCAN_RUN_INFO_RANGE_DFELTH_MIN_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_EYE_SCAN_RUN_SRD_EYE_SCAN_RUN_INFO_RANGE_DFELTH_MIN_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.range.dfelth_min = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_EYE_SCAN_RUN_SRD_EYE_SCAN_RUN_INFO_RANGE_TLTH_MAX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_EYE_SCAN_RUN_SRD_EYE_SCAN_RUN_INFO_RANGE_TLTH_MAX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.range.tlth_max = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_EYE_SCAN_RUN_SRD_EYE_SCAN_RUN_INFO_RANGE_TLTH_MIN_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_EYE_SCAN_RUN_SRD_EYE_SCAN_RUN_INFO_RANGE_TLTH_MIN_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.range.tlth_min = (uint8)param_val->value.ulong_value;
  } 


  for (res_i = 0; res_i < prm_info.nof_lane_ndx_entries; res_i++)
  {
    fe600_FE600_SRD_EYE_SCAN_RES_clear(&(prm_res[res_i]));
  }

  /* Call function */

  ret = fe600_srd_eye_scan_run(
          unit,
          &prm_info,
          FALSE,
          prm_res
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_srd_eye_scan_run - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_srd_eye_scan_run");
    goto exit;
  }

  send_string_to_screen("--> res:", TRUE);

  for (res_i = 0; res_i < prm_info.nof_lane_ndx_entries; res_i++)
  {
    FE600_SRD_RX_PHYS_INTERNAL_PARAMS
      rx_phys_internal_params;

    fe600_FE600_SRD_RX_PHYS_INTERNAL_PARAMS_clear(&rx_phys_internal_params);

    ret = fe600_srd_rx_phys_params_get(
            unit,
            prm_info.lane_ndx[res_i],
            &rx_phys_internal_params);
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** fe600_srd_rx_phys_params_get - FAIL", TRUE);
      fe600_disp_result(ret, "fe600_srd_rx_phys_params_get");
      goto exit;
    }

    soc_sand_os_printf( "Lane %d:\n\r", prm_info.lane_ndx[res_i]);
    fe600_FE600_SRD_EYE_SCAN_RES_print(&prm_res[res_i]);
    rx_phys_internal_params.tlth = prm_res[res_i].optimum.tlth;
    rx_phys_internal_params.dfelth = prm_res[res_i].optimum.dfelth;
    fe600_FE600_SRD_RX_PHYS_INTERNAL_PARAMS_print(&rx_phys_internal_params);
    soc_sand_os_printf( "\n\r");
  }

  goto exit;
exit:
  sal_free_any_size(prm_res);
  return ui_ret;
}
/********************************************************************
 *  Function handler: srd_scif_enable_set (section serdes_utils)
 */
int
  ui_fe600_api_serdes_utils_srd_scif_enable_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint8
    prm_is_enabled;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_serdes_utils");
  soc_sand_proc_name = "fe600_srd_scif_enable_set";

  unit = fe600_get_default_unit();

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = fe600_srd_scif_enable_get(
          unit,
          &prm_is_enabled
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_srd_scif_enable_get - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_srd_scif_enable_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_SCIF_ENABLE_SET_SRD_SCIF_ENABLE_SET_IS_ENABLED_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_SCIF_ENABLE_SET_SRD_SCIF_ENABLE_SET_IS_ENABLED_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_is_enabled = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = fe600_srd_scif_enable_set(
          unit,
          prm_is_enabled
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_srd_scif_enable_set - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_srd_scif_enable_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: srd_scif_enable_get (section serdes_utils)
 */
int
  ui_fe600_api_serdes_utils_srd_scif_enable_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint8
    prm_is_enabled;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_serdes_utils");
  soc_sand_proc_name = "fe600_srd_scif_enable_get";

  unit = fe600_get_default_unit();

  /* Get parameters */

  /* Call function */
  ret = fe600_srd_scif_enable_get(
          unit,
          &prm_is_enabled
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_srd_scif_enable_get - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_srd_scif_enable_get");
    goto exit;
  }

  send_string_to_screen("--> is_enabled:", TRUE);
  soc_sand_os_printf( "is_enabled: %u\n\r",prm_is_enabled);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: srd_relock (section serdes_utils)
 */
int
  ui_fe600_api_serdes_utils_srd_relock(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_lane_ndx;

  UI_MACROS_INIT_FUNCTION("ui_fe600_api_serdes_utils");
  soc_sand_proc_name = "fe600_srd_relock";

  unit = fe600_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_RELOCK_SRD_RELOCK_LANE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SRD_RELOCK_SRD_RELOCK_LANE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_lane_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter lane_ndx after srd_relock***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = fe600_srd_relock(
          unit,
          prm_lane_ndx
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** fe600_srd_relock - FAIL", TRUE);
    fe600_disp_result(ret, "fe600_srd_relock");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

#endif
#ifdef UI_FE600_GENERAL/* { general*/

int
  ui_fe600_iwrite(
    CURRENT_LINE *current_line
  )
{
  uint32
    addr,
    param_i,
    data[3],
    ret,
    vals[4],
    size=0,
    do_ndx,
    nof_occurs = 1,
    wait_time=1,
    sec[3],
    nano[3];
  PARAM_VAL
    *param_val_general_purpose;

  UI_MACROS_INIT_FUNCTION("ui_fe600_iwrite");
  soc_sand_proc_name = "ui_fe600_iwrite";

  unit = fe600_get_default_unit();


  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SIZE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SIZE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    size = (uint32)param_val->value.ulong_value;
  }

  for (param_i=1; param_i<=4; ++param_i)
  {
    ui_ret = get_val_of( current_line, (int *)&match_index,
      PARAM_FE600_IWRITE_ID, param_i,
      &param_val_general_purpose, VAL_NUMERIC, err_msg
      );
    if (0 != ui_ret)
    {
      ui_ret=0;
      break;
    }
    vals[param_i-1] = param_val_general_purpose->value.ulong_value;
  }
  addr = vals[0];
  if(size == 1)
  {
    data[0] = vals[1];
  }
  else if(size == 3)
  {
    data[2] = vals[1];
    data[1] = vals[2];
    data[0] = vals[3];
  }
  else
  {
    soc_sand_os_printf( "size %d is invalid\n\r", size);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_DO_TWICE_ID, 1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_DO_TWICE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    nof_occurs = 2;
    wait_time = (uint32)param_val->value.ulong_value;
  }

  for(do_ndx=0;do_ndx<nof_occurs;do_ndx++)
  {
    ret = soc_sand_tbl_write(
      unit,
      data,
      addr,
      size * sizeof(uint32),
      FE600_RTP_ID,
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
      fe600_disp_result(ret, "soc_sand_mem_iwrite");
      goto exit;
    }
  }

  send_string_to_screen(" ---> soc_sand_mem_iwrite - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

int
  ui_fe600_iread(
    CURRENT_LINE *current_line
  )
{
  uint32
    addr=0,
    data[3],
    ret,
    size=0,
    do_ndx,
    nof_occurs = 1,
    wait_time=1,
    sec[3],
    nano[3];

  UI_MACROS_INIT_FUNCTION("ui_fe600_iread");
  soc_sand_proc_name = "ui_fe600_iread";

  unit = fe600_get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_IREAD_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_IREAD_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    addr = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SIZE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_SIZE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    size = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_DO_TWICE_ID, 1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_DO_TWICE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    nof_occurs = 2;
    wait_time = (uint32)param_val->value.ulong_value;
  }

  for(do_ndx=0;do_ndx<nof_occurs;do_ndx++)
  {
    ret = soc_sand_tbl_read(
      unit,
      data,
      addr,
      size * sizeof(uint32),
      FE600_RTP_ID,
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
      fe600_disp_result(ret, "soc_sand_mem_iread");
      goto exit;
    }
    soc_sand_os_printf( "addr 0x%x data 0x%x 0x%x 0x%x", addr, data[2], data[1], data[0]);
  }
  send_string_to_screen(" ---> soc_sand_mem_iread - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}


int
  ui_fe600_write(
    CURRENT_LINE *current_line
  )
{
  uint32
    addr,
    param_i,
    data,
    ret,
    vals[2],
    do_ndx,
    nof_occurs = 1,
    wait_time=1,
    sec[3],
    nano[3];
  PARAM_VAL
    *param_val_general_purpose;

  UI_MACROS_INIT_FUNCTION("ui_fe600_write");
  soc_sand_proc_name = "ui_fe600_write";

  unit = fe600_get_default_unit();


  for (param_i=1; param_i<=2; ++param_i)
  {
    ui_ret = get_val_of( current_line, (int *)&match_index,
      PARAM_FE600_WRITE_ID, param_i,
      &param_val_general_purpose, VAL_NUMERIC, err_msg
      );
    if (0 != ui_ret)
    {
      ui_ret=0;
      break;
    }
    vals[param_i-1] = param_val_general_purpose->value.ulong_value;
  }
  addr = vals[0];
  data = vals[1];

  if(addr > 0x3500 * sizeof(uint32))
  {
    soc_sand_os_printf( "DBG!! offset %d is illegal\n\r", addr);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_DO_TWICE_ID, 1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_DO_TWICE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    nof_occurs = 2;
    wait_time = (uint32)param_val->value.ulong_value;
  }

  for(do_ndx=0;do_ndx<nof_occurs;do_ndx++)
  {
    ret = soc_sand_mem_write(
      unit,
      &data,
      addr * sizeof(uint32),
      sizeof(uint32),
      FALSE
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
      send_string_to_screen(" *** soc_sand_mem_write - FAIL", TRUE);
      fe600_disp_result(ret, "soc_sand_mem_write");
      goto exit;
    }
  }



  send_string_to_screen(" ---> soc_sand_mem_write - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

int
  ui_fe600_read(
    CURRENT_LINE *current_line
  )
{
  uint32
    addr,
    data,
    ret,
    do_ndx,
    nof_occurs = 1,
    wait_time=1,
    sec[3],
    nano[3];

  UI_MACROS_INIT_FUNCTION("ui_fe600_read");
  soc_sand_proc_name = "ui_fe600_read";

  unit = fe600_get_default_unit();


  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_READ_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_READ_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    addr = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter lane_ndx after srd_link_rx_eye_monitor***", TRUE);
    goto exit;
  }

  if(addr > 0x3500 * sizeof(uint32))
  {
    soc_sand_os_printf( "DBG!! offset %d is illegal\n\r", addr);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_DO_TWICE_ID, 1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FE600_DO_TWICE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    nof_occurs = 2;
    wait_time = (uint32)param_val->value.ulong_value;
  }

  for(do_ndx=0;do_ndx<nof_occurs;do_ndx++)
  {
    ret = soc_sand_mem_read(
      unit,
      &data,
      addr * sizeof(uint32),
      sizeof(uint32),
      FALSE
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
        soc_sand_os_printf( " Waited %d seconds and %d nanoseconds\n\r", sec[2],nano[2]);
      }
    }
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** soc_sand_mem_read - FAIL", TRUE);
      fe600_disp_result(ret, "soc_sand_mem_read");
      goto exit;
    }
    soc_sand_os_printf( "addr 0x%x data 0x%x", addr, data);
  }
  send_string_to_screen(" ---> soc_sand_mem_read - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}
/********************************************************************
 *  Section handler: general
 */
int
  ui_fe600_api_general(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fe600_api_general");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_WRITE_ID,1))
  {
    ui_ret = ui_fe600_write(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_READ_ID,1))
  {
    ui_ret = ui_fe600_read(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_IWRITE_ID,1))
  {
    ui_ret = ui_fe600_iwrite(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_IREAD_ID,1))
  {
    ui_ret = ui_fe600_iread(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after general***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif/* { general*/

#ifdef UI_FE600_MGMT/* { mgmt*/
/********************************************************************
 *  Section handler: mgmt
 */
int
  ui_fe600_api_mgmt(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fe600_api_mgmt");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_REGISTER_DEVICE_REGISTER_DEVICE_ID,1))
  {
    ui_ret = ui_fe600_api_mgmt_register_device(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_UNREGISTER_DEVICE_UNREGISTER_DEVICE_ID,1))
  {
    ui_ret = ui_fe600_api_mgmt_unregister_device(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_MGMT_OPERATION_MODE_SET_OPERATION_MODE_SET_ID,1))
  {
    ui_ret = ui_fe600_api_mgmt_operation_mode_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_MGMT_OPERATION_MODE_GET_OPERATION_MODE_GET_ID,1))
  {
    ui_ret = ui_fe600_api_mgmt_operation_mode_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_MGMT_INIT_SEQUENCE_PHASE2_INIT_SEQUENCE_PHASE2_ID,1))
  {
    ui_ret = ui_fe600_api_mgmt_init_sequence_phase2(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_MGMT_SYSTEM_FE_ID_SET_SYSTEM_FE_ID_SET_ID,1))
  {
    ui_ret = ui_fe600_api_mgmt_system_fe_id_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_MGMT_SYSTEM_FE_ID_GET_SYSTEM_FE_ID_GET_ID,1))
  {
    ui_ret = ui_fe600_api_mgmt_system_fe_id_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_MGMT_ALL_CTRL_CELLS_ENABLE_ALL_CTRL_CELLS_ENABLE_ID,1))
  {
    ui_ret = ui_fe600_api_mgmt_all_ctrl_cells_enable(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_MGMT_ENABLE_TRAFFIC_SET_ENABLE_TRAFFIC_SET_ID,1))
  {
    ui_ret = ui_fe600_api_mgmt_enable_traffic_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_MGMT_ENABLE_TRAFFIC_GET_ENABLE_TRAFFIC_GET_ID,1))
  {
    ui_ret = ui_fe600_api_mgmt_enable_traffic_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SSR_ID,1))
  {
    ui_ret = ui_fe600_api_ssr(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after mgmt***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif/* { mgmt*/

#ifdef UI_FE600_LINKS_TOPOLOGY/* { links_topology*/
/********************************************************************
 *  Section handler: links_topology
 */
int
  ui_fe600_api_links_topology(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fe600_api_links_topology");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_TOPOLOGY_CONST_CONNECTIVITY_SET_TOPOLOGY_CONST_CONNECTIVITY_SET_ID,1))
  {
    ui_ret = ui_fe600_api_links_topology_topology_const_connectivity_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_TOPOLOGY_CONST_CONNECTIVITY_GET_TOPOLOGY_CONST_CONNECTIVITY_GET_ID,1))
  {
    ui_ret = ui_fe600_api_links_topology_topology_const_connectivity_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_TOPOLOGY_CONST_CONNECTIVITY_UPDATE_TOPOLOGY_CONST_CONNECTIVITY_UPDATE_ID,1))
  {
    ui_ret = ui_fe600_api_links_topology_topology_const_connectivity_update(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_TOPOLOGY_SET_CONST_TOPOLOGY_AS_CURRENT_TOPOLOGY_SET_CONST_TOPOLOGY_AS_CURRENT_ID,1))
  {
    ui_ret = ui_fe600_api_links_topology_topology_set_const_topology_as_current(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_TOPOLOGY_REPEATER_SET_TOPOLOGY_REPEATER_SET_ID,1))
  {
    ui_ret = ui_fe600_api_links_topology_topology_repeater_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_TOPOLOGY_REPEATER_GET_TOPOLOGY_REPEATER_GET_ID,1))
  {
    ui_ret = ui_fe600_api_links_topology_topology_repeater_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_TOPOLOGY_GRACEFUL_SHUTDOWN_TOPOLOGY_GRACEFUL_SHUTDOWN_ID,1))
  {
    ui_ret = ui_fe600_api_links_topology_topology_graceful_shutdown(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_TOPOLOGY_STATUS_REACHABILITY_MAP_GET_TOPOLOGY_STATUS_REACHABILITY_MAP_GET_ID,1))
  {
    ui_ret = ui_fe600_api_links_topology_topology_status_reachability_map_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_TOPOLOGY_STATUS_REACHABILITY_MAP_PRINT_TOPOLOGY_STATUS_REACHABILITY_MAP_PRINT_ID,1))
  {
    ui_ret = ui_fe600_api_links_topology_topology_status_reachability_map_print(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_TOPOLOGY_STATUS_CMP_WITH_CURR_CONNECTIVITY_GET_TOPOLOGY_STATUS_CMP_WITH_CURR_CONNECTIVITY_GET_ID,1))
  {
    ui_ret = ui_fe600_api_links_topology_topology_status_cmp_with_curr_connectivity_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_TOPOLOGY_STATUS_CONNECTIVITY_GET_TOPOLOGY_STATUS_CONNECTIVITY_GET_ID,1))
  {
    ui_ret = ui_fe600_api_links_topology_topology_status_connectivity_get(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after links_topology***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif/* { links_topology*/

#ifdef UI_FE600_LINKS/* { links*/
/********************************************************************
 *  Section handler: links
 */
int
  ui_fe600_api_links(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fe600_api_links");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_FABRIC_SRD_QRTT_RESET_SRD_QRTT_RESET_ID,1))
  {
    ui_ret = ui_fe600_api_fabric_srd_qrtt_reset(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_LINKS_RESET_SET_RESET_SET_ID,1))
  {
    ui_ret = ui_fe600_api_link_on_off_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_LINKS_RESET_GET_RESET_GET_ID,1))
  {
    ui_ret = ui_fe600_api_link_on_off_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_LINKS_IS_TDM_ONLY_SET_IS_TDM_ONLY_SET_ID,1))
  {
    ui_ret = ui_fe600_api_links_is_tdm_only_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_LINKS_IS_TDM_ONLY_GET_IS_TDM_ONLY_GET_ID,1))
  {
    ui_ret = ui_fe600_api_links_is_tdm_only_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_LINKS_LINK_LEVEL_FC_ENABLE_SET_LINK_LEVEL_FC_ENABLE_SET_ID,1))
  {
    ui_ret = ui_fe600_api_links_link_level_fc_enable_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_LINKS_LINK_LEVEL_FC_ENABLE_GET_LINK_LEVEL_FC_ENABLE_GET_ID,1))
  {
    ui_ret = ui_fe600_api_links_link_level_fc_enable_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_LINKS_FEC_ENABLE_SET_FEC_ENABLE_SET_ID,1))
  {
    ui_ret = ui_fe600_api_links_fec_enable_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_LINKS_FEC_ENABLE_GET_FEC_ENABLE_GET_ID,1))
  {
    ui_ret = ui_fe600_api_links_fec_enable_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_LINKS_TYPE_SET_TYPE_SET_ID,1))
  {
    ui_ret = ui_fe600_api_links_type_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_LINKS_TYPE_GET_TYPE_GET_ID,1))
  {
    ui_ret = ui_fe600_api_links_type_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_LINKS_PER_TYPE_PARAMS_SET_PER_TYPE_PARAMS_SET_ID,1))
  {
    ui_ret = ui_fe600_api_links_per_type_params_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_LINKS_PER_TYPE_PARAMS_GET_PER_TYPE_PARAMS_GET_ID,1))
  {
    ui_ret = ui_fe600_api_links_per_type_params_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_LINKS_STATUS_GET_STATUS_GET_ID,1))
  {
    ui_ret = ui_fe600_api_links_status_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAN_FE600_LINKS_ACCEPT_CELLS_GET_ID,1))
  {
    ui_ret = ui_fe600_links_accept_cell_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_LINKS_Q_MAX_OCCUPANCY_GET_Q_MAX_OCCUPANCY_GET_ID,1)) 
  { 
    ui_ret = ui_fe600_api_links_q_max_occupancy_get(current_line); 
  } 
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after links***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif/* { links*/

#ifdef UI_FE600_MULTICAST/* { multicast*/
/********************************************************************
 *  Section handler: multicast
 */
int
  ui_fe600_api_multicast(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fe600_api_multicast");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_MULTICAST_CONGESTION_INFO_SET_CONGESTION_INFO_SET_ID,1))
  {
    ui_ret = ui_fe600_api_multicast_congestion_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_MULTICAST_CONGESTION_INFO_GET_CONGESTION_INFO_GET_ID,1))
  {
    ui_ret = ui_fe600_api_multicast_congestion_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_MULTICAST_LOW_PRIO_SET_LOW_PRIO_SET_ID,1))
  {
    ui_ret = ui_fe600_api_multicast_low_prio_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_MULTICAST_LOW_PRIO_GET_LOW_PRIO_GET_ID,1))
  {
    ui_ret = ui_fe600_api_multicast_low_prio_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_MULTICAST_GROUP_DIRECT_SET_GROUP_DIRECT_SET_ID,1))
  {
    ui_ret = ui_fe600_api_multicast_group_direct_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_MULTICAST_GROUP_DIRECT_GET_GROUP_DIRECT_GET_ID,1))
  {
    ui_ret = ui_fe600_api_multicast_group_direct_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_MULTICAST_GROUP_INDIRECT_SET_GROUP_INDIRECT_SET_ID,1))
  {
    ui_ret = ui_fe600_api_multicast_group_indirect_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_MULTICAST_GROUP_INDIRECT_GET_GROUP_INDIRECT_GET_ID,1))
  {
    ui_ret = ui_fe600_api_multicast_group_indirect_get(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after multicast***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif/* { multicast*/

#ifdef UI_FE600_STATUS/* { status*/
/********************************************************************
 *  Section handler: status
 */
int
  ui_fe600_api_status(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fe600_api_status");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_STATUS_COUNTERS_AND_INTERRUPTS_GET_COUNTERS_AND_INTERRUPTS_GET_ID,1))
  {
    ui_ret = ui_fe600_api_status_counters_and_interrupts_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_STAT_REGS_DUMP_STAT_REGS_DUMP_ID,1))
  {
    ui_ret = ui_fe600_api_status_stat_regs_dump(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_STAT_TBLS_DUMP_STAT_TBLS_DUMP_ID,1))
  {
    ui_ret = ui_fe600_api_status_stat_tbls_dump(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_STAT_ECI_ACCESS_TST_DBG_ECI_ACCESS_TST_ID,1)) 
  { 
    ui_ret = ui_fe600_api_status_eci_access_tst(current_line); 
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_DIAG_SOFT_ERROR_TEST_START_DIAG_SOFT_ERROR_TEST_START_ID,1))
  {
    ui_ret = ui_fe600_api_diagnostics_diag_soft_error_test_start(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_DIAG_SOFT_ERROR_TEST_RESULT_GET_DIAG_SOFT_ERROR_TEST_RESULT_GET_ID,1))
  {
    ui_ret = ui_fe600_api_diagnostics_diag_soft_error_test_result_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_DIAG_MBIST_RUN_ID,1))
  {
    ui_ret = ui_fe600_api_diagnostics_mbist_run(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after status***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif/* { status*/

#ifdef UI_FE600_SERDES/* { serdes*/
/********************************************************************
 *  Section handler: serdes
 */
int
  ui_fe600_api_serdes(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fe600_api_serdes");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_RATE_SET_SRD_RATE_SET_ID,1))
  {
    ui_ret = ui_fe600_api_serdes_srd_rate_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_RATE_GET_SRD_RATE_GET_ID,1))
  {
    ui_ret = ui_fe600_api_serdes_srd_rate_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_TX_PHYS_PARAMS_SET_EXPLICIT_SRD_TX_PHYS_PARAMS_SET_ID,1))
  {
    ui_ret = ui_fe600_api_serdes_srd_tx_phys_params_set_explicit(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_TX_PHYS_PARAMS_SET_INTERNAL_SRD_TX_PHYS_PARAMS_SET_ID,1))
  {
    ui_ret = ui_fe600_api_serdes_srd_tx_phys_params_set_internal(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_TX_PHYS_PARAMS_SET_MEDIA_TYPE_SRD_TX_PHYS_PARAMS_SET_ID,1))
  {
    ui_ret = ui_fe600_api_serdes_srd_tx_phys_params_set_media_type(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_TX_PHYS_PARAMS_SET_ATTEN_SRD_TX_PHYS_PARAMS_SET_ID,1))
  {
    ui_ret = ui_fe600_api_serdes_srd_tx_phys_params_set_atten(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_TX_PHYS_PARAMS_GET_SRD_TX_PHYS_PARAMS_GET_ID,1))
  {
    ui_ret = ui_fe600_api_serdes_srd_tx_phys_params_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_LANE_POLARITY_SET_SRD_LANE_POLARITY_SET_ID,1))
  {
    ui_ret = ui_fe600_api_serdes_srd_lane_polarity_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_LANE_POLARITY_GET_SRD_LANE_POLARITY_GET_ID,1))
  {
    ui_ret = ui_fe600_api_serdes_srd_lane_polarity_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_LANE_POWER_STATE_SET_SRD_LANE_POWER_STATE_SET_ID,1))
  {
    ui_ret = ui_fe600_api_serdes_srd_lane_power_state_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_LANE_POWER_STATE_GET_SRD_LANE_POWER_STATE_GET_ID,1))
  {
    ui_ret = ui_fe600_api_serdes_srd_lane_power_state_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_STAR_SET_SRD_STAR_SET_ID,1))
  {
    ui_ret = ui_fe600_api_serdes_srd_star_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_STAR_GET_SRD_STAR_GET_ID,1))
  {
    ui_ret = ui_fe600_api_serdes_srd_star_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_QRTT_SET_SRD_QRTT_SET_ID,1))
  {
    ui_ret = ui_fe600_api_serdes_srd_qrtt_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_QRTT_GET_SRD_QRTT_GET_ID,1))
  {
    ui_ret = ui_fe600_api_serdes_srd_qrtt_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_ID,1))
  {
    ui_ret = ui_fe600_api_serdes_srd_all_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_ALL_GET_SRD_ALL_GET_ID,1))
  {
    ui_ret = ui_fe600_api_serdes_srd_all_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_AUTO_EQUALIZE_SRD_AUTO_EQUALIZE_ID,1))
  {
    ui_ret = ui_fe600_api_serdes_srd_auto_equalize(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_RX_PHYS_PARAMS_SET_SRD_RX_PHYS_PARAMS_SET_ID,1))
  {
    ui_ret = ui_fe600_api_serdes_srd_rx_phys_params_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_RX_PHYS_PARAMS_GET_SRD_RX_PHYS_PARAMS_GET_ID,1))
  {
    ui_ret = ui_fe600_api_serdes_srd_rx_phys_params_get(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after serdes***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif/* { serdes*/

#ifdef UI_FE600_CELL/* { cell */
/********************************************************************
 *  Section handler: cell
 */
int
  ui_fe600_api_cell(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fe600_api_cell");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_CELL_READ_DIRECT_ID,1))
  {
    ui_ret = ui_fe600_api_cell_fe600_read_from_fe600(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_CELL_WRITE_DIRECT_ID,1))
  {
    ui_ret = ui_fe600_api_cell_fe600_write_to_fe600(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_CELL_READ_INDIRECT_ID,1))
  {
    ui_ret = ui_fe600_api_cell_fe600_indirect_read_from_fe600(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_CELL_WRITE_INDIRECT_ID,1))
  {
    ui_ret = ui_fe600_api_cell_fe600_indirect_write_to_fe600(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_CELL_CPU2CPU_ID,1))
  {
    ui_ret = ui_fe600_api_cell_fe600_cpu2cpu_write(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_CELL_CPU2CPU_READ_ID,1))
  {
    ui_ret = ui_fe600_api_cell_fe600_cpu2cpu_read(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_CELL_SNAKE_TEST_INIT_SNAKE_TEST_INIT_ID,1))
  {
    ui_ret = ui_fe600_api_cell_snake_test_init(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_CELL_SNAKE_TEST_RATE_GET_SNAKE_TEST_RATE_GET_ID,1))
  {
    ui_ret = ui_fe600_api_cell_snake_test_rate_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_CELL_SNAKE_TEST_STOP_SNAKE_TEST_STOP_ID,1))
  {
    ui_ret = ui_fe600_api_cell_snake_test_stop(current_line);
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
#ifdef UI_FE600_SERDES_UTILS/* { serdes_utils*/
/********************************************************************
 *  Section handler: serdes_utils
 */
int
  ui_fe600_api_serdes_utils(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fe600_api_serdes_utils");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_REG_WRITE_SRD_REG_WRITE_ID,1))
  {
    ui_ret = ui_fe600_api_serdes_utils_srd_reg_write(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_REG_READ_SRD_REG_READ_ID,1))
  {
    ui_ret = ui_fe600_api_serdes_utils_srd_reg_read(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_FLD_WRITE_SRD_FLD_WRITE_ID,1))
  {
    ui_ret = ui_fe600_api_serdes_utils_srd_fld_write(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_FLD_READ_SRD_FLD_READ_ID,1))
  {
    ui_ret = ui_fe600_api_serdes_utils_srd_fld_read(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_LINK_RX_EYE_MONITOR_SRD_LINK_RX_EYE_MONITOR_ID,1))
  {
    ui_ret = ui_fe600_api_serdes_utils_srd_link_rx_eye_monitor(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_LANE_LOOPBACK_MODE_SET_SRD_LANE_LOOPBACK_MODE_SET_ID,1))
  {
    ui_ret = ui_fe600_api_serdes_utils_srd_lane_loopback_mode_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_LANE_LOOPBACK_MODE_GET_SRD_LANE_LOOPBACK_MODE_GET_ID,1))
  {
    ui_ret = ui_fe600_api_serdes_utils_srd_lane_loopback_mode_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_PRBS_MODE_SET_SRD_PRBS_MODE_SET_ID,1))
  {
    ui_ret = ui_fe600_api_serdes_utils_srd_prbs_mode_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_PRBS_MODE_GET_SRD_PRBS_MODE_GET_ID,1))
  {
    ui_ret = ui_fe600_api_serdes_utils_srd_prbs_mode_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_PRBS_START_SRD_PRBS_START_ID,1))
  {
    ui_ret = ui_fe600_api_serdes_utils_srd_prbs_start(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_PRBS_STOP_SRD_PRBS_STOP_ID,1))
  {
    ui_ret = ui_fe600_api_serdes_utils_srd_prbs_stop(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_PRBS_GET_AND_CLEAR_STAT_SRD_PRBS_GET_AND_CLEAR_STAT_ID,1))
  {
    ui_ret = ui_fe600_api_serdes_utils_srd_prbs_get_and_clear_stat(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_PRBS_GET_AND_CLEAR_STAT_SRD_PRBS_GET_AND_CLEAR_STAT_ID,1))
  {
    ui_ret = ui_fe600_api_serdes_utils_srd_prbs_get_and_clear_stat(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_LANE_STATUS_GET_SRD_LANE_STATUS_GET_ID,1))
  {
    ui_ret = ui_fe600_api_serdes_utils_srd_lane_status_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_QRTT_STATUS_GET_SRD_QRTT_STATUS_GET_ID,1))
  {
    ui_ret = ui_fe600_api_serdes_utils_srd_qrtt_status_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_EYE_SCAN_RUN_SRD_EYE_SCAN_RUN_ID,1))
  {
    ui_ret = ui_fe600_api_serdes_utils_srd_eye_scan_run(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_SCIF_ENABLE_SET_SRD_SCIF_ENABLE_SET_ID,1))
  {
    ui_ret = ui_fe600_api_serdes_utils_srd_scif_enable_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_SCIF_ENABLE_GET_SRD_SCIF_ENABLE_GET_ID,1))
  {
    ui_ret = ui_fe600_api_serdes_utils_srd_scif_enable_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_SRD_RELOCK_SRD_RELOCK_ID,1))
  {
    ui_ret = ui_fe600_api_serdes_utils_srd_relock(current_line);
  }

  else
  {
    send_string_to_screen(" *** SW error - expecting function name after serdes_utils***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif/* { serdes_utils*/


#ifdef UI_FE600_UTILS/* { ui_utils*/
/********************************************************************
*  Section handler: ui_utils
 */
int
  ui_fe600_api_utils(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fe600_api_utils");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_UTILS_SET_DEFAULT_DEVICE_ID,1))
  {
    ui_ret = ui_fe600_api_set_default_unit(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_UTILS_GET_DEFAULT_DEVICE_ID,1))
  {
    ui_ret = ui_fe600_api_get_default_unit(current_line);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif/* { */ /* } mgmt*/

/*****************************************************
*NAME
*  subject_fe600_api
*TYPE: PROC
*DATE: 29/DEC/2002
*FUNCTION:
*  Process input line which has an 'subject_fe600_api' subject.
*  Input line is assumed to have been checked and
*  found to be of right format.
*CALLING SEQUENCE:
*  subject_fe600_api(current_line,current_line_ptr)
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
  subject_fe600_api(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  )
{
  unsigned int
    match_index;
  int
    ui_ret;
  unsigned int
    ;
  char
    *proc_name ;


  proc_name = "subject_fe600_api" ;
  ui_ret = FALSE ;
  unit = 0;


  /*
   * the rest of the line is empty
   */
  if (current_line->num_param_names == 0)
  {
    send_string_to_screen("\r\n",FALSE) ;
    send_string_to_screen("'subject_fe600_api()' function was called with no parameters.\r\n",FALSE) ;
    goto exit ;
  }
  /*
   * Else, there are parameters on the line (not just 'subject_fe600_api').
   */

  send_array_to_screen("\r\n",2) ;



  if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_API_GENERAL_ID,1))
  {
    ui_ret = ui_fe600_api_general(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_API_MGMT_ID,1))
  {
    ui_ret = ui_fe600_api_mgmt(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_API_LINKS_TOPOLOGY_ID,1))
  {
    ui_ret = ui_fe600_api_links_topology(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_API_LINKS_ID,1))
  {
    ui_ret = ui_fe600_api_links(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_API_MULTICAST_ID,1))
  {
    ui_ret = ui_fe600_api_multicast(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_API_STATUS_ID,1))
  {
    ui_ret = ui_fe600_api_status(current_line);
  }
#ifdef UI_FE600_CELL
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_API_CELLS_ID,1))
  {
    ui_ret = ui_fe600_api_cell(current_line);
  }
#endif
#ifdef UI_FE600_SERDES
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_API_SERDES_ID,1))
  {
    ui_ret = ui_fe600_api_serdes(current_line);
  }
#endif
#ifdef UI_FE600_SERDES_UTILS
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_API_SERDES_UTILS_ID,1))
  {
    ui_ret = ui_fe600_api_serdes_utils(current_line);
  }
#endif
#ifdef UI_FE600_UTILS
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FE600_API_UTILS_ID,1))
  {
  ui_ret = ui_fe600_api_utils(current_line);
  }
#endif
  else
  {
    /*
     * Enter if an unknown request.
     */
    send_string_to_screen(
      "\r\n"
      "*** fe600_api command with unknown parameters'.\r\n"
      "    Syntax error/sw error...\r\n",
      TRUE)  ;
    ui_ret = TRUE  ;
    goto exit  ;
  }

exit:
  return (ui_ret);
}

#else
int ui_fe600_api = 0;
#endif
