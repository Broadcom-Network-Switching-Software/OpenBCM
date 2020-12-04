/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __UI_ROM_DEFI_PB_PP_ACC2_INCLUDED__
/* { */
#define __UI_ROM_DEFI_PB_PP_ACC2_INCLUDED__

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <appl/dpp/UserInterface/ui_pure_defi_pb_pp_acc2.h>                        

 /* 
  * ENUM RULES 
  * {
  */
 /* 
  * } ENUM RULES 
  */
/********************************************************/        
/********************************************************/        
EXTERN CONST 
   PARAM_VAL_RULES 
     soc_pb_pp_acc2_free_vals[] 
#ifdef INIT 
   = 
{ 
  { 
    VAL_NUMERIC,SYMB_NAME_NUMERIC, 
    { 
      { 
        HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE, 
        0,0, 
          /* 
           * Number of times this value can be repeated (i.e., 
           * value is an array). Must be at least 1! 
           */ 
        1, 
        (VAL_PROC_PTR)NULL 
      } 
    } 
  }, 
/* 
 * Last element. Do not remove. 
 */ 
  { 
    VAL_END_OF_LIST 
  } 
} 
#endif 
; 
EXTERN CONST 
   PARAM_VAL_RULES 
     soc_pb_pp_acc2_empty_vals[]
#ifdef INIT 
   = 
{ 
/* 
 * Last element. Do not remove. 
 */ 
  { 
    VAL_END_OF_LIST 
  } 
} 
#endif 
; 
/********************************************************/        
/********************************************************/        
EXTERN CONST 
   PARAM 
     soc_pb_pp_acc2_params[] 
#ifdef INIT 
   = 
{ 
#if UI_PB_PP_IND_ACC2/* { soc_pb_pp_ind_acc*/
  { 
    PARAM_PB_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_SET_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_SET_ID, 
    "system_physical_port_lookup_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_system_physical_port_lookup_table_tbl_set\n\r"
    "  entry_offset_ndx 0 sys_phy_port 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_SET_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_SET_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_SET_TBL_DATA_SYS_PHY_PORT_ID, 
    "sys_phy_port", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.sys_phy_port:\n\r"
    "  uint32 sys_phy_port\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_system_physical_port_lookup_table_tbl_set\n\r"
    "  entry_offset_ndx 0 sys_phy_port 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_SET_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_GET_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_GET_ID, 
    "system_physical_port_lookup_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_system_physical_port_lookup_table_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_GET_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_SET_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_SET_ID, 
    "destination_device_and_port_lookup_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_destination_device_and_port_lookup_table_tbl_set\n\r"
    "  entry_offset_ndx 0 dest_dev 0 dest_port 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_SET_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_SET_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_SET_TBL_DATA_DEST_DEV_ID, 
    "dest_dev", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.dest_dev:\n\r"
    "  uint32 dest_dev\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_destination_device_and_port_lookup_table_tbl_set\n\r"
    "  entry_offset_ndx 0 dest_dev 0 dest_port 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_SET_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_SET_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_SET_TBL_DATA_DEST_PORT_ID, 
    "dest_port", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.dest_port:\n\r"
    "  uint32 dest_port\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_destination_device_and_port_lookup_table_tbl_set\n\r"
    "  entry_offset_ndx 0 dest_dev 0 dest_port 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_SET_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_GET_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_GET_ID, 
    "destination_device_and_port_lookup_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_destination_device_and_port_lookup_table_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_GET_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_FLOW_ID_LOOKUP_TABLE_TBL_SET_IPS_FLOW_ID_LOOKUP_TABLE_TBL_SET_ID, 
    "flow_id_lookup_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_flow_id_lookup_table_tbl_set entry_offset_ndx 0\n\r"
    "  sub_flow_mode 0 base_flow 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_FLOW_ID_LOOKUP_TABLE_TBL_SET_IPS_FLOW_ID_LOOKUP_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_FLOW_ID_LOOKUP_TABLE_TBL_SET_IPS_FLOW_ID_LOOKUP_TABLE_TBL_SET_TBL_DATA_SUB_FLOW_MODE_ID, 
    "sub_flow_mode", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.sub_flow_mode:\n\r"
    "  uint32 sub_flow_mode\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_flow_id_lookup_table_tbl_set entry_offset_ndx 0\n\r"
    "  sub_flow_mode 0 base_flow 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_FLOW_ID_LOOKUP_TABLE_TBL_SET_IPS_FLOW_ID_LOOKUP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_FLOW_ID_LOOKUP_TABLE_TBL_SET_IPS_FLOW_ID_LOOKUP_TABLE_TBL_SET_TBL_DATA_BASE_FLOW_ID, 
    "base_flow", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.base_flow:\n\r"
    "  uint32 base_flow\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_flow_id_lookup_table_tbl_set entry_offset_ndx 0\n\r"
    "  sub_flow_mode 0 base_flow 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_FLOW_ID_LOOKUP_TABLE_TBL_SET_IPS_FLOW_ID_LOOKUP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_FLOW_ID_LOOKUP_TABLE_TBL_GET_IPS_FLOW_ID_LOOKUP_TABLE_TBL_GET_ID, 
    "flow_id_lookup_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_flow_id_lookup_table_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_FLOW_ID_LOOKUP_TABLE_TBL_GET_IPS_FLOW_ID_LOOKUP_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_SET_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_SET_ID, 
    "queue_type_lookup_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_queue_type_lookup_table_tbl_set entry_offset_ndx 0\n\r"
    "  queue_type_lookup_table 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_SET_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_SET_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_SET_TBL_DATA_QUEUE_TYPE_LOOKUP_TABLE_ID, 
    "queue_type_lookup_table", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.queue_type_lookup_table:\n\r"
    "  uint32 queue_type_lookup_table\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_queue_type_lookup_table_tbl_set entry_offset_ndx 0\n\r"
    "  queue_type_lookup_table 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_SET_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_GET_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_GET_ID, 
    "queue_type_lookup_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_queue_type_lookup_table_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_GET_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_SET_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_SET_ID, 
    "queue_priority_map_select_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_queue_priority_map_select_tbl_set entry_offset_ndx 0\n\r"
    "  queue_priority_map_select 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_SET_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_SET_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_SET_TBL_DATA_QUEUE_PRIORITY_MAP_SELECT_ID, 
    "queue_priority_map_select", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.queue_priority_map_select:\n\r"
    "  uint32 queue_priority_map_select\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_queue_priority_map_select_tbl_set entry_offset_ndx 0\n\r"
    "  queue_priority_map_select 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_SET_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_GET_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_GET_ID, 
    "queue_priority_map_select_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_queue_priority_map_select_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_GET_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_SET_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_SET_ID, 
    "queue_priority_maps_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_queue_priority_maps_table_tbl_set entry_offset_ndx 0\n\r"
    "  queue_priority_maps_table_index 0 queue_priority_maps_table 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_SET_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_SET_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_SET_TBL_DATA_QUEUE_PRIORITY_MAPS_TABLE_INDEX_ID, 
    "queue_priority_maps_table_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  queue_priority_maps_table_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_queue_priority_maps_table_tbl_set entry_offset_ndx 0\n\r"
    "  queue_priority_maps_table_index 0 queue_priority_maps_table 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_SET_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_SET_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_SET_TBL_DATA_QUEUE_PRIORITY_MAPS_TABLE_ID, 
    "queue_priority_maps_table", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.queue_priority_maps_table[ prm_queue_priority_maps_table_index]:\n\r"
    "  uint32 queue_priority_maps_table[2]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_queue_priority_maps_table_tbl_set entry_offset_ndx 0\n\r"
    "  queue_priority_maps_table_index 0 queue_priority_maps_table 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_SET_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_GET_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_GET_ID, 
    "queue_priority_maps_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_queue_priority_maps_table_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_GET_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_SET_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_SET_ID, 
    "queue_size_based_thresholds_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_queue_size_based_thresholds_table_tbl_set\n\r"
    "  entry_offset_ndx 0 fsm_th_mul 0 norm_to_slow_msg_th 0 slow_to_norm_msg_th 0\n\r"
    "  off_to_norm_msg_th 0 off_to_slow_msg_th 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_SET_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_SET_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_SET_TBL_DATA_FSM_TH_MUL_ID, 
    "fsm_th_mul", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.fsm_th_mul:\n\r"
    "  uint32 fsm_th_mul\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_queue_size_based_thresholds_table_tbl_set\n\r"
    "  entry_offset_ndx 0 fsm_th_mul 0 norm_to_slow_msg_th 0 slow_to_norm_msg_th 0\n\r"
    "  off_to_norm_msg_th 0 off_to_slow_msg_th 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_SET_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_SET_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_SET_TBL_DATA_NORM_TO_SLOW_MSG_TH_ID, 
    "norm_to_slow_msg_th", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.norm_to_slow_msg_th:\n\r"
    "  uint32 norm_to_slow_msg_th\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_queue_size_based_thresholds_table_tbl_set\n\r"
    "  entry_offset_ndx 0 fsm_th_mul 0 norm_to_slow_msg_th 0 slow_to_norm_msg_th 0\n\r"
    "  off_to_norm_msg_th 0 off_to_slow_msg_th 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_SET_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_SET_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_SET_TBL_DATA_SLOW_TO_NORM_MSG_TH_ID, 
    "slow_to_norm_msg_th", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.slow_to_norm_msg_th:\n\r"
    "  uint32 slow_to_norm_msg_th\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_queue_size_based_thresholds_table_tbl_set\n\r"
    "  entry_offset_ndx 0 fsm_th_mul 0 norm_to_slow_msg_th 0 slow_to_norm_msg_th 0\n\r"
    "  off_to_norm_msg_th 0 off_to_slow_msg_th 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_SET_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_SET_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_SET_TBL_DATA_OFF_TO_NORM_MSG_TH_ID, 
    "off_to_norm_msg_th", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.off_to_norm_msg_th:\n\r"
    "  uint32 off_to_norm_msg_th\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_queue_size_based_thresholds_table_tbl_set\n\r"
    "  entry_offset_ndx 0 fsm_th_mul 0 norm_to_slow_msg_th 0 slow_to_norm_msg_th 0\n\r"
    "  off_to_norm_msg_th 0 off_to_slow_msg_th 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_SET_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_SET_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_SET_TBL_DATA_OFF_TO_SLOW_MSG_TH_ID, 
    "off_to_slow_msg_th", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.off_to_slow_msg_th:\n\r"
    "  uint32 off_to_slow_msg_th\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_queue_size_based_thresholds_table_tbl_set\n\r"
    "  entry_offset_ndx 0 fsm_th_mul 0 norm_to_slow_msg_th 0 slow_to_norm_msg_th 0\n\r"
    "  off_to_norm_msg_th 0 off_to_slow_msg_th 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_SET_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_GET_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_GET_ID, 
    "queue_size_based_thresholds_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_queue_size_based_thresholds_table_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_GET_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_SET_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_SET_ID, 
    "credit_balance_based_thresholds_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_credit_balance_based_thresholds_table_tbl_set\n\r"
    "  entry_offset_ndx 0 backlog_exit_qcr_bal_th 0 backlog_enter_qcr_bal_th 0\n\r"
    "  backoff_exit_qcr_bal_th 0 backoff_enter_qcr_bal_th 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_SET_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_SET_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_SET_TBL_DATA_BACKLOG_EXIT_QCR_BAL_TH_ID, 
    "backlog_exit_qcr_bal_th", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.backlog_exit_qcr_bal_th:\n\r"
    "  uint32 backlog_exit_qcr_bal_th\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_credit_balance_based_thresholds_table_tbl_set\n\r"
    "  entry_offset_ndx 0 backlog_exit_qcr_bal_th 0 backlog_enter_qcr_bal_th 0\n\r"
    "  backoff_exit_qcr_bal_th 0 backoff_enter_qcr_bal_th 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_SET_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_SET_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_SET_TBL_DATA_BACKLOG_ENTER_QCR_BAL_TH_ID, 
    "backlog_enter_qcr_bal_th", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.backlog_enter_qcr_bal_th:\n\r"
    "  uint32 backlog_enter_qcr_bal_th\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_credit_balance_based_thresholds_table_tbl_set\n\r"
    "  entry_offset_ndx 0 backlog_exit_qcr_bal_th 0 backlog_enter_qcr_bal_th 0\n\r"
    "  backoff_exit_qcr_bal_th 0 backoff_enter_qcr_bal_th 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_SET_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_SET_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_SET_TBL_DATA_BACKOFF_EXIT_QCR_BAL_TH_ID, 
    "backoff_exit_qcr_bal_th", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.backoff_exit_qcr_bal_th:\n\r"
    "  uint32 backoff_exit_qcr_bal_th\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_credit_balance_based_thresholds_table_tbl_set\n\r"
    "  entry_offset_ndx 0 backlog_exit_qcr_bal_th 0 backlog_enter_qcr_bal_th 0\n\r"
    "  backoff_exit_qcr_bal_th 0 backoff_enter_qcr_bal_th 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_SET_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_SET_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_SET_TBL_DATA_BACKOFF_ENTER_QCR_BAL_TH_ID, 
    "backoff_enter_qcr_bal_th", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.backoff_enter_qcr_bal_th:\n\r"
    "  uint32 backoff_enter_qcr_bal_th\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_credit_balance_based_thresholds_table_tbl_set\n\r"
    "  entry_offset_ndx 0 backlog_exit_qcr_bal_th 0 backlog_enter_qcr_bal_th 0\n\r"
    "  backoff_exit_qcr_bal_th 0 backoff_enter_qcr_bal_th 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_SET_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_GET_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_GET_ID, 
    "credit_balance_based_thresholds_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_credit_balance_based_thresholds_table_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_GET_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_SET_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_SET_ID, 
    "empty_queue_credit_balance_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_empty_queue_credit_balance_table_tbl_set\n\r"
    "  entry_offset_ndx 0 exceed_max_empty_qcr_bal 0 max_empty_qcr_bal 0\n\r"
    "  empty_qsatisfied_cr_bal 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_SET_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_SET_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_SET_TBL_DATA_EXCEED_MAX_EMPTY_QCR_BAL_ID, 
    "exceed_max_empty_qcr_bal", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.exceed_max_empty_qcr_bal:\n\r"
    "  uint32 exceed_max_empty_qcr_bal\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_empty_queue_credit_balance_table_tbl_set\n\r"
    "  entry_offset_ndx 0 exceed_max_empty_qcr_bal 0 max_empty_qcr_bal 0\n\r"
    "  empty_qsatisfied_cr_bal 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_SET_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_SET_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_SET_TBL_DATA_MAX_EMPTY_QCR_BAL_ID, 
    "max_empty_qcr_bal", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.max_empty_qcr_bal:\n\r"
    "  uint32 max_empty_qcr_bal\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_empty_queue_credit_balance_table_tbl_set\n\r"
    "  entry_offset_ndx 0 exceed_max_empty_qcr_bal 0 max_empty_qcr_bal 0\n\r"
    "  empty_qsatisfied_cr_bal 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_SET_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_SET_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_SET_TBL_DATA_EMPTY_QSATISFIED_CR_BAL_ID, 
    "empty_qsatisfied_cr_bal", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.empty_qsatisfied_cr_bal:\n\r"
    "  uint32 empty_qsatisfied_cr_bal\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_empty_queue_credit_balance_table_tbl_set\n\r"
    "  entry_offset_ndx 0 exceed_max_empty_qcr_bal 0 max_empty_qcr_bal 0\n\r"
    "  empty_qsatisfied_cr_bal 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_SET_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_GET_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_GET_ID, 
    "empty_queue_credit_balance_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_empty_queue_credit_balance_table_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_GET_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_SET_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_SET_ID, 
    "credit_watchdog_thresholds_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_credit_watchdog_thresholds_table_tbl_set\n\r"
    "  entry_offset_ndx 0 wd_delete_qth 0 wd_status_msg_gen_period 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_SET_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_SET_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_SET_TBL_DATA_WD_DELETE_QTH_ID, 
    "wd_delete_qth", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.wd_delete_qth:\n\r"
    "  uint32 wd_delete_qth\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_credit_watchdog_thresholds_table_tbl_set\n\r"
    "  entry_offset_ndx 0 wd_delete_qth 0 wd_status_msg_gen_period 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_SET_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_SET_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_SET_TBL_DATA_WD_STATUS_MSG_GEN_PERIOD_ID, 
    "wd_status_msg_gen_period", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.wd_status_msg_gen_period:\n\r"
    "  uint32 wd_status_msg_gen_period\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_credit_watchdog_thresholds_table_tbl_set\n\r"
    "  entry_offset_ndx 0 wd_delete_qth 0 wd_status_msg_gen_period 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_SET_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_GET_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_GET_ID, 
    "credit_watchdog_thresholds_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_credit_watchdog_thresholds_table_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_GET_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_SET_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_SET_ID, 
    "queue_descriptor_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_queue_descriptor_table_tbl_set entry_offset_ndx 0\n\r"
    "  fsmrq_ctrl 0 wd_delete 0 in_dqcq 0 wd_last_cr_time 0 one_pkt_deq 0 crs 0\n\r"
    "  cr_bal 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_SET_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_SET_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_SET_TBL_DATA_FSMRQ_CTRL_ID, 
    "fsmrq_ctrl", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.fsmrq_ctrl:\n\r"
    "  uint32 fsmrq_ctrl\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_queue_descriptor_table_tbl_set entry_offset_ndx 0\n\r"
    "  fsmrq_ctrl 0 wd_delete 0 in_dqcq 0 wd_last_cr_time 0 one_pkt_deq 0 crs 0\n\r"
    "  cr_bal 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_SET_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_SET_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_SET_TBL_DATA_WD_DELETE_ID, 
    "wd_delete", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.wd_delete:\n\r"
    "  uint32 wd_delete\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_queue_descriptor_table_tbl_set entry_offset_ndx 0\n\r"
    "  fsmrq_ctrl 0 wd_delete 0 in_dqcq 0 wd_last_cr_time 0 one_pkt_deq 0 crs 0\n\r"
    "  cr_bal 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_SET_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_SET_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_SET_TBL_DATA_IN_DQCQ_ID, 
    "in_dqcq", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.in_dqcq:\n\r"
    "  uint32 in_dqcq\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_queue_descriptor_table_tbl_set entry_offset_ndx 0\n\r"
    "  fsmrq_ctrl 0 wd_delete 0 in_dqcq 0 wd_last_cr_time 0 one_pkt_deq 0 crs 0\n\r"
    "  cr_bal 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_SET_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_SET_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_SET_TBL_DATA_WD_LAST_CR_TIME_ID, 
    "wd_last_cr_time", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.wd_last_cr_time:\n\r"
    "  uint32 wd_last_cr_time\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_queue_descriptor_table_tbl_set entry_offset_ndx 0\n\r"
    "  fsmrq_ctrl 0 wd_delete 0 in_dqcq 0 wd_last_cr_time 0 one_pkt_deq 0 crs 0\n\r"
    "  cr_bal 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_SET_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_SET_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_SET_TBL_DATA_ONE_PKT_DEQ_ID, 
    "one_pkt_deq", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.one_pkt_deq:\n\r"
    "  uint32 one_pkt_deq\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_queue_descriptor_table_tbl_set entry_offset_ndx 0\n\r"
    "  fsmrq_ctrl 0 wd_delete 0 in_dqcq 0 wd_last_cr_time 0 one_pkt_deq 0 crs 0\n\r"
    "  cr_bal 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_SET_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_SET_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_SET_TBL_DATA_CRS_ID, 
    "crs", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.crs:\n\r"
    "  uint32 crs\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_queue_descriptor_table_tbl_set entry_offset_ndx 0\n\r"
    "  fsmrq_ctrl 0 wd_delete 0 in_dqcq 0 wd_last_cr_time 0 one_pkt_deq 0 crs 0\n\r"
    "  cr_bal 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_SET_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_SET_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_SET_TBL_DATA_CR_BAL_ID, 
    "cr_bal", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.cr_bal:\n\r"
    "  uint32 cr_bal\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_queue_descriptor_table_tbl_set entry_offset_ndx 0\n\r"
    "  fsmrq_ctrl 0 wd_delete 0 in_dqcq 0 wd_last_cr_time 0 one_pkt_deq 0 crs 0\n\r"
    "  cr_bal 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_SET_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_GET_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_GET_ID, 
    "queue_descriptor_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_queue_descriptor_table_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_GET_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_QUEUE_SIZE_TABLE_TBL_SET_IPS_QUEUE_SIZE_TABLE_TBL_SET_ID, 
    "queue_size_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_queue_size_table_tbl_set entry_offset_ndx 0 qsize_4b\n\r"
    "  0 mantissa 0 exponent 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_QUEUE_SIZE_TABLE_TBL_SET_IPS_QUEUE_SIZE_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_QUEUE_SIZE_TABLE_TBL_SET_IPS_QUEUE_SIZE_TABLE_TBL_SET_TBL_DATA_QSIZE_4B_ID, 
    "qsize_4b", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.qsize_4b:\n\r"
    "  uint32 qsize_4b\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_queue_size_table_tbl_set entry_offset_ndx 0 qsize_4b\n\r"
    "  0 mantissa 0 exponent 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_QUEUE_SIZE_TABLE_TBL_SET_IPS_QUEUE_SIZE_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_QUEUE_SIZE_TABLE_TBL_SET_IPS_QUEUE_SIZE_TABLE_TBL_SET_TBL_DATA_MANTISSA_ID, 
    "mantissa", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.mantissa:\n\r"
    "  uint32 mantissa\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_queue_size_table_tbl_set entry_offset_ndx 0 qsize_4b\n\r"
    "  0 mantissa 0 exponent 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_QUEUE_SIZE_TABLE_TBL_SET_IPS_QUEUE_SIZE_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_QUEUE_SIZE_TABLE_TBL_SET_IPS_QUEUE_SIZE_TABLE_TBL_SET_TBL_DATA_EXPONENT_ID, 
    "exponent", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.exponent:\n\r"
    "  uint32 exponent\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_queue_size_table_tbl_set entry_offset_ndx 0 qsize_4b\n\r"
    "  0 mantissa 0 exponent 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_QUEUE_SIZE_TABLE_TBL_SET_IPS_QUEUE_SIZE_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_QUEUE_SIZE_TABLE_TBL_GET_IPS_QUEUE_SIZE_TABLE_TBL_GET_ID, 
    "queue_size_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_queue_size_table_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_QUEUE_SIZE_TABLE_TBL_GET_IPS_QUEUE_SIZE_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_SET_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_SET_ID, 
    "system_red_max_queue_size_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_system_red_max_queue_size_table_tbl_set\n\r"
    "  entry_offset_ndx 0 maxqsz_age 0 maxqsz 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_SET_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_SET_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_SET_TBL_DATA_MAXQSZ_AGE_ID, 
    "maxqsz_age", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.maxqsz_age:\n\r"
    "  uint32 maxqsz_age\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_system_red_max_queue_size_table_tbl_set\n\r"
    "  entry_offset_ndx 0 maxqsz_age 0 maxqsz 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_SET_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_SET_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_SET_TBL_DATA_MAXQSZ_ID, 
    "maxqsz", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.maxqsz:\n\r"
    "  uint32 maxqsz\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_system_red_max_queue_size_table_tbl_set\n\r"
    "  entry_offset_ndx 0 maxqsz_age 0 maxqsz 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_SET_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_GET_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_GET_ID, 
    "system_red_max_queue_size_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ips_system_red_max_queue_size_table_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_GET_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_BDQ_TBL_SET_IPT_BDQ_TBL_SET_ID, 
    "bdq_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_bdq_tbl_set entry_offset_ndx 0 bdq 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_BDQ_TBL_SET_IPT_BDQ_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_BDQ_TBL_SET_IPT_BDQ_TBL_SET_TBL_DATA_BDQ_ID, 
    "bdq", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.bdq:\n\r"
    "  uint32 bdq\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_bdq_tbl_set entry_offset_ndx 0 bdq 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_BDQ_TBL_SET_IPT_BDQ_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_BDQ_TBL_GET_IPT_BDQ_TBL_GET_ID, 
    "bdq_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_bdq_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_BDQ_TBL_GET_IPT_BDQ_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_PCQ_TBL_SET_IPT_PCQ_TBL_SET_ID, 
    "pcq_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_pcq_tbl_set entry_offset_ndx 0 pcq_index 0 pcq 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_PCQ_TBL_SET_IPT_PCQ_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_PCQ_TBL_SET_IPT_PCQ_TBL_SET_TBL_DATA_PCQ_INDEX_ID, 
    "pcq_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  pcq_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_pcq_tbl_set entry_offset_ndx 0 pcq_index 0 pcq 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_PCQ_TBL_SET_IPT_PCQ_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_PCQ_TBL_SET_IPT_PCQ_TBL_SET_TBL_DATA_PCQ_ID, 
    "pcq", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.pcq[ prm_pcq_index]:\n\r"
    "  uint32 pcq[2]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_pcq_tbl_set entry_offset_ndx 0 pcq_index 0 pcq 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_PCQ_TBL_SET_IPT_PCQ_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_PCQ_TBL_GET_IPT_PCQ_TBL_GET_ID, 
    "pcq_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_pcq_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_PCQ_TBL_GET_IPT_PCQ_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_SOP_MMU_TBL_SET_IPT_SOP_MMU_TBL_SET_ID, 
    "sop_mmu_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_sop_mmu_tbl_set entry_offset_ndx 0 sop_mmu_index 0\n\r"
    "  sop_mmu 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_SOP_MMU_TBL_SET_IPT_SOP_MMU_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_SOP_MMU_TBL_SET_IPT_SOP_MMU_TBL_SET_TBL_DATA_SOP_MMU_INDEX_ID, 
    "sop_mmu_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  sop_mmu_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_sop_mmu_tbl_set entry_offset_ndx 0 sop_mmu_index 0\n\r"
    "  sop_mmu 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_SOP_MMU_TBL_SET_IPT_SOP_MMU_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_SOP_MMU_TBL_SET_IPT_SOP_MMU_TBL_SET_TBL_DATA_SOP_MMU_ID, 
    "sop_mmu", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.sop_mmu[ prm_sop_mmu_index]:\n\r"
    "  uint32 sop_mmu[3]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_sop_mmu_tbl_set entry_offset_ndx 0 sop_mmu_index 0\n\r"
    "  sop_mmu 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_SOP_MMU_TBL_SET_IPT_SOP_MMU_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_SOP_MMU_TBL_GET_IPT_SOP_MMU_TBL_GET_ID, 
    "sop_mmu_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_sop_mmu_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_SOP_MMU_TBL_GET_IPT_SOP_MMU_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_MOP_MMU_TBL_SET_IPT_MOP_MMU_TBL_SET_ID, 
    "mop_mmu_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_mop_mmu_tbl_set entry_offset_ndx 0 mop_mmu 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_MOP_MMU_TBL_SET_IPT_MOP_MMU_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_MOP_MMU_TBL_SET_IPT_MOP_MMU_TBL_SET_TBL_DATA_MOP_MMU_ID, 
    "mop_mmu", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.mop_mmu:\n\r"
    "  uint32 mop_mmu\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_mop_mmu_tbl_set entry_offset_ndx 0 mop_mmu 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_MOP_MMU_TBL_SET_IPT_MOP_MMU_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_MOP_MMU_TBL_GET_IPT_MOP_MMU_TBL_GET_ID, 
    "mop_mmu_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_mop_mmu_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_MOP_MMU_TBL_GET_IPT_MOP_MMU_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_FDTCTL_TBL_SET_IPT_FDTCTL_TBL_SET_ID, 
    "fdtctl_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_fdtctl_tbl_set entry_offset_ndx 0 fdtctl 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_FDTCTL_TBL_SET_IPT_FDTCTL_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_FDTCTL_TBL_SET_IPT_FDTCTL_TBL_SET_TBL_DATA_FDTCTL_ID, 
    "fdtctl", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.fdtctl:\n\r"
    "  uint32 fdtctl\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_fdtctl_tbl_set entry_offset_ndx 0 fdtctl 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_FDTCTL_TBL_SET_IPT_FDTCTL_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_FDTCTL_TBL_GET_IPT_FDTCTL_TBL_GET_ID, 
    "fdtctl_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_fdtctl_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_FDTCTL_TBL_GET_IPT_FDTCTL_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_FDTDATA_TBL_SET_IPT_FDTDATA_TBL_SET_ID, 
    "fdtdata_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_fdtdata_tbl_set entry_offset_ndx 0 fdtdata_index 0\n\r"
    "  fdtdata 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_FDTDATA_TBL_SET_IPT_FDTDATA_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_FDTDATA_TBL_SET_IPT_FDTDATA_TBL_SET_TBL_DATA_FDTDATA_INDEX_ID, 
    "fdtdata_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  fdtdata_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_fdtdata_tbl_set entry_offset_ndx 0 fdtdata_index 0\n\r"
    "  fdtdata 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_FDTDATA_TBL_SET_IPT_FDTDATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_FDTDATA_TBL_SET_IPT_FDTDATA_TBL_SET_TBL_DATA_FDTDATA_ID, 
    "fdtdata", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.fdtdata[ prm_fdtdata_index]:\n\r"
    "  uint32 fdtdata[16]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_fdtdata_tbl_set entry_offset_ndx 0 fdtdata_index 0\n\r"
    "  fdtdata 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_FDTDATA_TBL_SET_IPT_FDTDATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_FDTDATA_TBL_GET_IPT_FDTDATA_TBL_GET_ID, 
    "fdtdata_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_fdtdata_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_FDTDATA_TBL_GET_IPT_FDTDATA_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_EGQCTL_TBL_SET_IPT_EGQCTL_TBL_SET_ID, 
    "egqctl_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_egqctl_tbl_set entry_offset_ndx 0 egqctl 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_EGQCTL_TBL_SET_IPT_EGQCTL_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_EGQCTL_TBL_SET_IPT_EGQCTL_TBL_SET_TBL_DATA_EGQCTL_ID, 
    "egqctl", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.egqctl:\n\r"
    "  uint32 egqctl\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_egqctl_tbl_set entry_offset_ndx 0 egqctl 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_EGQCTL_TBL_SET_IPT_EGQCTL_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_EGQCTL_TBL_GET_IPT_EGQCTL_TBL_GET_ID, 
    "egqctl_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_egqctl_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_EGQCTL_TBL_GET_IPT_EGQCTL_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_EGQDATA_TBL_SET_IPT_EGQDATA_TBL_SET_ID, 
    "egqdata_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_egqdata_tbl_set entry_offset_ndx 0 egqdata_index 0\n\r"
    "  egqdata 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_EGQDATA_TBL_SET_IPT_EGQDATA_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_EGQDATA_TBL_SET_IPT_EGQDATA_TBL_SET_TBL_DATA_EGQDATA_INDEX_ID, 
    "egqdata_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  egqdata_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_egqdata_tbl_set entry_offset_ndx 0 egqdata_index 0\n\r"
    "  egqdata 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_EGQDATA_TBL_SET_IPT_EGQDATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_EGQDATA_TBL_SET_IPT_EGQDATA_TBL_SET_TBL_DATA_EGQDATA_ID, 
    "egqdata", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.egqdata[ prm_egqdata_index]:\n\r"
    "  uint32 egqdata[16]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_egqdata_tbl_set entry_offset_ndx 0 egqdata_index 0\n\r"
    "  egqdata 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_EGQDATA_TBL_SET_IPT_EGQDATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_EGQDATA_TBL_GET_IPT_EGQDATA_TBL_GET_ID, 
    "egqdata_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_egqdata_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_EGQDATA_TBL_GET_IPT_EGQDATA_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_CFG_BYTE_CNT_TBL_SET_IPT_CFG_BYTE_CNT_TBL_SET_ID, 
    "cfg_byte_cnt_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_cfg_byte_cnt_tbl_set entry_offset_ndx 0\n\r"
    "  cfg_byte_cnt_index 0 cfg_byte_cnt 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_CFG_BYTE_CNT_TBL_SET_IPT_CFG_BYTE_CNT_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_CFG_BYTE_CNT_TBL_SET_IPT_CFG_BYTE_CNT_TBL_SET_TBL_DATA_CFG_BYTE_CNT_INDEX_ID, 
    "cfg_byte_cnt_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  cfg_byte_cnt_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_cfg_byte_cnt_tbl_set entry_offset_ndx 0\n\r"
    "  cfg_byte_cnt_index 0 cfg_byte_cnt 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_CFG_BYTE_CNT_TBL_SET_IPT_CFG_BYTE_CNT_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_CFG_BYTE_CNT_TBL_SET_IPT_CFG_BYTE_CNT_TBL_SET_TBL_DATA_CFG_BYTE_CNT_ID, 
    "cfg_byte_cnt", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.cfg_byte_cnt[ prm_cfg_byte_cnt_index]:\n\r"
    "  uint32 cfg_byte_cnt[2]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_cfg_byte_cnt_tbl_set entry_offset_ndx 0\n\r"
    "  cfg_byte_cnt_index 0 cfg_byte_cnt 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_CFG_BYTE_CNT_TBL_SET_IPT_CFG_BYTE_CNT_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_CFG_BYTE_CNT_TBL_GET_IPT_CFG_BYTE_CNT_TBL_GET_ID, 
    "cfg_byte_cnt_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_cfg_byte_cnt_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_CFG_BYTE_CNT_TBL_GET_IPT_CFG_BYTE_CNT_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_EGQ_TXQ_WR_ADDR_TBL_SET_IPT_EGQ_TXQ_WR_ADDR_TBL_SET_ID, 
    "txq_wr_addr_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_egq_txq_wr_addr_tbl_set entry_offset_ndx 0\n\r"
    "  egq_txq_wr_addr 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_EGQ_TXQ_WR_ADDR_TBL_SET_IPT_EGQ_TXQ_WR_ADDR_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_EGQ_TXQ_WR_ADDR_TBL_SET_IPT_EGQ_TXQ_WR_ADDR_TBL_SET_TBL_DATA_EGQ_TXQ_WR_ADDR_ID, 
    "txq_wr_addr", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.egq_txq_wr_addr:\n\r"
    "  uint32 egq_txq_wr_addr\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_egq_txq_wr_addr_tbl_set entry_offset_ndx 0\n\r"
    "  egq_txq_wr_addr 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_EGQ_TXQ_WR_ADDR_TBL_SET_IPT_EGQ_TXQ_WR_ADDR_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_EGQ_TXQ_WR_ADDR_TBL_GET_IPT_EGQ_TXQ_WR_ADDR_TBL_GET_ID, 
    "txq_wr_addr_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_egq_txq_wr_addr_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_EGQ_TXQ_WR_ADDR_TBL_GET_IPT_EGQ_TXQ_WR_ADDR_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_EGQ_TXQ_RD_ADDR_TBL_SET_IPT_EGQ_TXQ_RD_ADDR_TBL_SET_ID, 
    "txq_rd_addr_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_egq_txq_rd_addr_tbl_set entry_offset_ndx 0\n\r"
    "  egq_txq_rd_addr 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_EGQ_TXQ_RD_ADDR_TBL_SET_IPT_EGQ_TXQ_RD_ADDR_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_EGQ_TXQ_RD_ADDR_TBL_SET_IPT_EGQ_TXQ_RD_ADDR_TBL_SET_TBL_DATA_EGQ_TXQ_RD_ADDR_ID, 
    "txq_rd_addr", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.egq_txq_rd_addr:\n\r"
    "  uint32 egq_txq_rd_addr\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_egq_txq_rd_addr_tbl_set entry_offset_ndx 0\n\r"
    "  egq_txq_rd_addr 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_EGQ_TXQ_RD_ADDR_TBL_SET_IPT_EGQ_TXQ_RD_ADDR_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_EGQ_TXQ_RD_ADDR_TBL_GET_IPT_EGQ_TXQ_RD_ADDR_TBL_GET_ID, 
    "txq_rd_addr_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_egq_txq_rd_addr_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_EGQ_TXQ_RD_ADDR_TBL_GET_IPT_EGQ_TXQ_RD_ADDR_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_FDT_TXQ_WR_ADDR_TBL_SET_IPT_FDT_TXQ_WR_ADDR_TBL_SET_ID, 
    "txq_wr_addr_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_fdt_txq_wr_addr_tbl_set entry_offset_ndx 0\n\r"
    "  dtq6_wr_addr 0 dtq5_wr_addr 0 dtq4_wr_addr 0 dtq3_wr_addr 0 dtq2_wr_addr 0\n\r"
    "  dtq1_wr_addr 0 dtq0_wr_addr 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_FDT_TXQ_WR_ADDR_TBL_SET_IPT_FDT_TXQ_WR_ADDR_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_FDT_TXQ_WR_ADDR_TBL_SET_IPT_FDT_TXQ_WR_ADDR_TBL_SET_TBL_DATA_DTQ6_WR_ADDR_ID, 
    "dtq6_wr_addr", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.dtq6_wr_addr:\n\r"
    "  uint32 dtq6_wr_addr\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_fdt_txq_wr_addr_tbl_set entry_offset_ndx 0\n\r"
    "  dtq6_wr_addr 0 dtq5_wr_addr 0 dtq4_wr_addr 0 dtq3_wr_addr 0 dtq2_wr_addr 0\n\r"
    "  dtq1_wr_addr 0 dtq0_wr_addr 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_FDT_TXQ_WR_ADDR_TBL_SET_IPT_FDT_TXQ_WR_ADDR_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_FDT_TXQ_WR_ADDR_TBL_SET_IPT_FDT_TXQ_WR_ADDR_TBL_SET_TBL_DATA_DTQ5_WR_ADDR_ID, 
    "dtq5_wr_addr", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.dtq5_wr_addr:\n\r"
    "  uint32 dtq5_wr_addr\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_fdt_txq_wr_addr_tbl_set entry_offset_ndx 0\n\r"
    "  dtq6_wr_addr 0 dtq5_wr_addr 0 dtq4_wr_addr 0 dtq3_wr_addr 0 dtq2_wr_addr 0\n\r"
    "  dtq1_wr_addr 0 dtq0_wr_addr 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_FDT_TXQ_WR_ADDR_TBL_SET_IPT_FDT_TXQ_WR_ADDR_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_FDT_TXQ_WR_ADDR_TBL_SET_IPT_FDT_TXQ_WR_ADDR_TBL_SET_TBL_DATA_DTQ4_WR_ADDR_ID, 
    "dtq4_wr_addr", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.dtq4_wr_addr:\n\r"
    "  uint32 dtq4_wr_addr\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_fdt_txq_wr_addr_tbl_set entry_offset_ndx 0\n\r"
    "  dtq6_wr_addr 0 dtq5_wr_addr 0 dtq4_wr_addr 0 dtq3_wr_addr 0 dtq2_wr_addr 0\n\r"
    "  dtq1_wr_addr 0 dtq0_wr_addr 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_FDT_TXQ_WR_ADDR_TBL_SET_IPT_FDT_TXQ_WR_ADDR_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_FDT_TXQ_WR_ADDR_TBL_SET_IPT_FDT_TXQ_WR_ADDR_TBL_SET_TBL_DATA_DTQ3_WR_ADDR_ID, 
    "dtq3_wr_addr", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.dtq3_wr_addr:\n\r"
    "  uint32 dtq3_wr_addr\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_fdt_txq_wr_addr_tbl_set entry_offset_ndx 0\n\r"
    "  dtq6_wr_addr 0 dtq5_wr_addr 0 dtq4_wr_addr 0 dtq3_wr_addr 0 dtq2_wr_addr 0\n\r"
    "  dtq1_wr_addr 0 dtq0_wr_addr 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_FDT_TXQ_WR_ADDR_TBL_SET_IPT_FDT_TXQ_WR_ADDR_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_FDT_TXQ_WR_ADDR_TBL_SET_IPT_FDT_TXQ_WR_ADDR_TBL_SET_TBL_DATA_DTQ2_WR_ADDR_ID, 
    "dtq2_wr_addr", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.dtq2_wr_addr:\n\r"
    "  uint32 dtq2_wr_addr\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_fdt_txq_wr_addr_tbl_set entry_offset_ndx 0\n\r"
    "  dtq6_wr_addr 0 dtq5_wr_addr 0 dtq4_wr_addr 0 dtq3_wr_addr 0 dtq2_wr_addr 0\n\r"
    "  dtq1_wr_addr 0 dtq0_wr_addr 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_FDT_TXQ_WR_ADDR_TBL_SET_IPT_FDT_TXQ_WR_ADDR_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_FDT_TXQ_WR_ADDR_TBL_SET_IPT_FDT_TXQ_WR_ADDR_TBL_SET_TBL_DATA_DTQ1_WR_ADDR_ID, 
    "dtq1_wr_addr", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.dtq1_wr_addr:\n\r"
    "  uint32 dtq1_wr_addr\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_fdt_txq_wr_addr_tbl_set entry_offset_ndx 0\n\r"
    "  dtq6_wr_addr 0 dtq5_wr_addr 0 dtq4_wr_addr 0 dtq3_wr_addr 0 dtq2_wr_addr 0\n\r"
    "  dtq1_wr_addr 0 dtq0_wr_addr 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_FDT_TXQ_WR_ADDR_TBL_SET_IPT_FDT_TXQ_WR_ADDR_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_FDT_TXQ_WR_ADDR_TBL_SET_IPT_FDT_TXQ_WR_ADDR_TBL_SET_TBL_DATA_DTQ0_WR_ADDR_ID, 
    "dtq0_wr_addr", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.dtq0_wr_addr:\n\r"
    "  uint32 dtq0_wr_addr\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_fdt_txq_wr_addr_tbl_set entry_offset_ndx 0\n\r"
    "  dtq6_wr_addr 0 dtq5_wr_addr 0 dtq4_wr_addr 0 dtq3_wr_addr 0 dtq2_wr_addr 0\n\r"
    "  dtq1_wr_addr 0 dtq0_wr_addr 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_FDT_TXQ_WR_ADDR_TBL_SET_IPT_FDT_TXQ_WR_ADDR_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_FDT_TXQ_WR_ADDR_TBL_GET_IPT_FDT_TXQ_WR_ADDR_TBL_GET_ID, 
    "txq_wr_addr_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_fdt_txq_wr_addr_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_FDT_TXQ_WR_ADDR_TBL_GET_IPT_FDT_TXQ_WR_ADDR_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_FDT_TXQ_RD_ADDR_TBL_SET_IPT_FDT_TXQ_RD_ADDR_TBL_SET_ID, 
    "txq_rd_addr_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_fdt_txq_rd_addr_tbl_set entry_offset_ndx 0\n\r"
    "  dtq6_rd_addr 0 dtq5_rd_addr 0 dtq4_rd_addr 0 dtq3_rd_addr 0 dtq2_rd_addr 0\n\r"
    "  dtq1_rd_addr 0 dtq0_rd_addr 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_FDT_TXQ_RD_ADDR_TBL_SET_IPT_FDT_TXQ_RD_ADDR_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_FDT_TXQ_RD_ADDR_TBL_SET_IPT_FDT_TXQ_RD_ADDR_TBL_SET_TBL_DATA_DTQ6_RD_ADDR_ID, 
    "dtq6_rd_addr", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.dtq6_rd_addr:\n\r"
    "  uint32 dtq6_rd_addr\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_fdt_txq_rd_addr_tbl_set entry_offset_ndx 0\n\r"
    "  dtq6_rd_addr 0 dtq5_rd_addr 0 dtq4_rd_addr 0 dtq3_rd_addr 0 dtq2_rd_addr 0\n\r"
    "  dtq1_rd_addr 0 dtq0_rd_addr 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_FDT_TXQ_RD_ADDR_TBL_SET_IPT_FDT_TXQ_RD_ADDR_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_FDT_TXQ_RD_ADDR_TBL_SET_IPT_FDT_TXQ_RD_ADDR_TBL_SET_TBL_DATA_DTQ5_RD_ADDR_ID, 
    "dtq5_rd_addr", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.dtq5_rd_addr:\n\r"
    "  uint32 dtq5_rd_addr\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_fdt_txq_rd_addr_tbl_set entry_offset_ndx 0\n\r"
    "  dtq6_rd_addr 0 dtq5_rd_addr 0 dtq4_rd_addr 0 dtq3_rd_addr 0 dtq2_rd_addr 0\n\r"
    "  dtq1_rd_addr 0 dtq0_rd_addr 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_FDT_TXQ_RD_ADDR_TBL_SET_IPT_FDT_TXQ_RD_ADDR_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_FDT_TXQ_RD_ADDR_TBL_SET_IPT_FDT_TXQ_RD_ADDR_TBL_SET_TBL_DATA_DTQ4_RD_ADDR_ID, 
    "dtq4_rd_addr", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.dtq4_rd_addr:\n\r"
    "  uint32 dtq4_rd_addr\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_fdt_txq_rd_addr_tbl_set entry_offset_ndx 0\n\r"
    "  dtq6_rd_addr 0 dtq5_rd_addr 0 dtq4_rd_addr 0 dtq3_rd_addr 0 dtq2_rd_addr 0\n\r"
    "  dtq1_rd_addr 0 dtq0_rd_addr 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_FDT_TXQ_RD_ADDR_TBL_SET_IPT_FDT_TXQ_RD_ADDR_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_FDT_TXQ_RD_ADDR_TBL_SET_IPT_FDT_TXQ_RD_ADDR_TBL_SET_TBL_DATA_DTQ3_RD_ADDR_ID, 
    "dtq3_rd_addr", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.dtq3_rd_addr:\n\r"
    "  uint32 dtq3_rd_addr\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_fdt_txq_rd_addr_tbl_set entry_offset_ndx 0\n\r"
    "  dtq6_rd_addr 0 dtq5_rd_addr 0 dtq4_rd_addr 0 dtq3_rd_addr 0 dtq2_rd_addr 0\n\r"
    "  dtq1_rd_addr 0 dtq0_rd_addr 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_FDT_TXQ_RD_ADDR_TBL_SET_IPT_FDT_TXQ_RD_ADDR_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_FDT_TXQ_RD_ADDR_TBL_SET_IPT_FDT_TXQ_RD_ADDR_TBL_SET_TBL_DATA_DTQ2_RD_ADDR_ID, 
    "dtq2_rd_addr", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.dtq2_rd_addr:\n\r"
    "  uint32 dtq2_rd_addr\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_fdt_txq_rd_addr_tbl_set entry_offset_ndx 0\n\r"
    "  dtq6_rd_addr 0 dtq5_rd_addr 0 dtq4_rd_addr 0 dtq3_rd_addr 0 dtq2_rd_addr 0\n\r"
    "  dtq1_rd_addr 0 dtq0_rd_addr 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_FDT_TXQ_RD_ADDR_TBL_SET_IPT_FDT_TXQ_RD_ADDR_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_FDT_TXQ_RD_ADDR_TBL_SET_IPT_FDT_TXQ_RD_ADDR_TBL_SET_TBL_DATA_DTQ1_RD_ADDR_ID, 
    "dtq1_rd_addr", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.dtq1_rd_addr:\n\r"
    "  uint32 dtq1_rd_addr\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_fdt_txq_rd_addr_tbl_set entry_offset_ndx 0\n\r"
    "  dtq6_rd_addr 0 dtq5_rd_addr 0 dtq4_rd_addr 0 dtq3_rd_addr 0 dtq2_rd_addr 0\n\r"
    "  dtq1_rd_addr 0 dtq0_rd_addr 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_FDT_TXQ_RD_ADDR_TBL_SET_IPT_FDT_TXQ_RD_ADDR_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_FDT_TXQ_RD_ADDR_TBL_SET_IPT_FDT_TXQ_RD_ADDR_TBL_SET_TBL_DATA_DTQ0_RD_ADDR_ID, 
    "dtq0_rd_addr", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.dtq0_rd_addr:\n\r"
    "  uint32 dtq0_rd_addr\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_fdt_txq_rd_addr_tbl_set entry_offset_ndx 0\n\r"
    "  dtq6_rd_addr 0 dtq5_rd_addr 0 dtq4_rd_addr 0 dtq3_rd_addr 0 dtq2_rd_addr 0\n\r"
    "  dtq1_rd_addr 0 dtq0_rd_addr 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_FDT_TXQ_RD_ADDR_TBL_SET_IPT_FDT_TXQ_RD_ADDR_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_FDT_TXQ_RD_ADDR_TBL_GET_IPT_FDT_TXQ_RD_ADDR_TBL_GET_ID, 
    "txq_rd_addr_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_fdt_txq_rd_addr_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_FDT_TXQ_RD_ADDR_TBL_GET_IPT_FDT_TXQ_RD_ADDR_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_GCI_BACKOFF_MASK_TBL_SET_IPT_GCI_BACKOFF_MASK_TBL_SET_ID, 
    "gci_backoff_mask_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_gci_backoff_mask_tbl_set entry_offset_ndx 0 mask3 0\n\r"
    "  mask2 0 mask1 0 mask0 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_GCI_BACKOFF_MASK_TBL_SET_IPT_GCI_BACKOFF_MASK_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_GCI_BACKOFF_MASK_TBL_SET_IPT_GCI_BACKOFF_MASK_TBL_SET_TBL_DATA_MASK3_ID, 
    "mask3", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.mask3:\n\r"
    "  uint32 mask3\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_gci_backoff_mask_tbl_set entry_offset_ndx 0 mask3 0\n\r"
    "  mask2 0 mask1 0 mask0 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_GCI_BACKOFF_MASK_TBL_SET_IPT_GCI_BACKOFF_MASK_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_GCI_BACKOFF_MASK_TBL_SET_IPT_GCI_BACKOFF_MASK_TBL_SET_TBL_DATA_MASK2_ID, 
    "mask2", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.mask2:\n\r"
    "  uint32 mask2\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_gci_backoff_mask_tbl_set entry_offset_ndx 0 mask3 0\n\r"
    "  mask2 0 mask1 0 mask0 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_GCI_BACKOFF_MASK_TBL_SET_IPT_GCI_BACKOFF_MASK_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_GCI_BACKOFF_MASK_TBL_SET_IPT_GCI_BACKOFF_MASK_TBL_SET_TBL_DATA_MASK1_ID, 
    "mask1", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.mask1:\n\r"
    "  uint32 mask1\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_gci_backoff_mask_tbl_set entry_offset_ndx 0 mask3 0\n\r"
    "  mask2 0 mask1 0 mask0 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_GCI_BACKOFF_MASK_TBL_SET_IPT_GCI_BACKOFF_MASK_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_GCI_BACKOFF_MASK_TBL_SET_IPT_GCI_BACKOFF_MASK_TBL_SET_TBL_DATA_MASK0_ID, 
    "mask0", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.mask0:\n\r"
    "  uint32 mask0\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_gci_backoff_mask_tbl_set entry_offset_ndx 0 mask3 0\n\r"
    "  mask2 0 mask1 0 mask0 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_GCI_BACKOFF_MASK_TBL_SET_IPT_GCI_BACKOFF_MASK_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IPT_GCI_BACKOFF_MASK_TBL_GET_IPT_GCI_BACKOFF_MASK_TBL_GET_ID, 
    "gci_backoff_mask_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ipt_gci_backoff_mask_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IPT_GCI_BACKOFF_MASK_TBL_GET_IPT_GCI_BACKOFF_MASK_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_DPI_DLL_RAM_TBL_SET_DPI_DLL_RAM_TBL_SET_ID, 
    "dll_ram_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc dpi_dll_ram_tbl_set entry_offset_ndx 0 module_id 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_DPI_DLL_RAM_TBL_SET_DPI_DLL_RAM_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_DPI_DLL_RAM_TBL_SET_DPI_DLL_RAM_TBL_SET_MODULE_ID_ID, 
    "module_id", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  module_id:\n\r"
    "  SOC_SAND_IN\n\r"
    "  uint32\n\r"
    "  module_id\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc dpi_dll_ram_tbl_set entry_offset_ndx 0 module_id 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_DPI_DLL_RAM_TBL_SET_DPI_DLL_RAM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_DPI_DLL_RAM_TBL_SET_DPI_DLL_RAM_TBL_SET_TBL_DATA_DLL_RAM_INDEX_ID, 
    "dll_ram_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  dll_ram_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc dpi_dll_ram_tbl_set entry_offset_ndx 0 module_id 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_DPI_DLL_RAM_TBL_SET_DPI_DLL_RAM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_DPI_DLL_RAM_TBL_SET_DPI_DLL_RAM_TBL_SET_TBL_DATA_DLL_RAM_ID, 
    "dll_ram", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.dll_ram[ prm_dll_ram_index]:\n\r"
    "  uint32 dll_ram[2]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc dpi_dll_ram_tbl_set entry_offset_ndx 0 module_id 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_DPI_DLL_RAM_TBL_SET_DPI_DLL_RAM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_DPI_DLL_RAM_TBL_GET_DPI_DLL_RAM_TBL_GET_ID, 
    "dll_ram_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc dpi_dll_ram_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_DPI_DLL_RAM_TBL_GET_DPI_DLL_RAM_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },    
  { 
    PARAM_PB_FDT_IPT_CONTRO_L_FIFO_TBL_GET_FDT_IPT_CONTRO_L_FIFO_TBL_GET_ID, 
    "ipt_contro_l_fifo_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc fdt_ipt_contro_l_fifo_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_FDT_IPT_CONTRO_L_FIFO_TBL_GET_FDT_IPT_CONTRO_L_FIFO_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL_SET_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL_SET_ID, 
    "unicast_distribution_memory_for_data_cells_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc rtp_unicast_distribution_memory_for_data_cells_tbl_set\n\r"
    "  entry_offset_ndx 0 unicast_distribution_memory_for_data_cells_index 0\n\r"
    "  unicast_distribution_memory_for_data_cells 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL_SET_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL_SET_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL_SET_TBL_DATA_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_INDEX_ID, 
    "unicast_distribution_memory_for_data_cells_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  unicast_distribution_memory_for_data_cells_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc rtp_unicast_distribution_memory_for_data_cells_tbl_set\n\r"
    "  entry_offset_ndx 0 unicast_distribution_memory_for_data_cells_index 0\n\r"
    "  unicast_distribution_memory_for_data_cells 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL_SET_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL_SET_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL_SET_TBL_DATA_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_ID, 
    "unicast_distribution_memory_for_data_cells", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.unicast_distribution_memory_for_data_cells[\n\r"
    "  prm_unicast_distribution_memory_for_data_cells_index]:\n\r"
    "  uint32 unicast_distribution_memory_for_data_cells[3]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc rtp_unicast_distribution_memory_for_data_cells_tbl_set\n\r"
    "  entry_offset_ndx 0 unicast_distribution_memory_for_data_cells_index 0\n\r"
    "  unicast_distribution_memory_for_data_cells 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL_SET_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL_GET_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL_GET_ID, 
    "unicast_distribution_memory_for_data_cells_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc rtp_unicast_distribution_memory_for_data_cells_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL_GET_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_TBL_SET_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_TBL_SET_ID, 
    "unicast_distribution_memory_for_control_cells_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc rtp_unicast_distribution_memory_for_control_cells_tbl_set\n\r"
    "  entry_offset_ndx 0 unicast_distribution_memory_for_control_cells_index 0\n\r"
    "  unicast_distribution_memory_for_control_cells 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_TBL_SET_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_TBL_SET_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_TBL_SET_TBL_DATA_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_INDEX_ID, 
    "unicast_distribution_memory_for_control_cells_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  unicast_distribution_memory_for_control_cells_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc rtp_unicast_distribution_memory_for_control_cells_tbl_set\n\r"
    "  entry_offset_ndx 0 unicast_distribution_memory_for_control_cells_index 0\n\r"
    "  unicast_distribution_memory_for_control_cells 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_TBL_SET_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_TBL_SET_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_TBL_SET_TBL_DATA_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_ID, 
    "unicast_distribution_memory_for_control_cells", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.unicast_distribution_memory_for_control_cells[\n\r"
    "  prm_unicast_distribution_memory_for_control_cells_index]:\n\r"
    "  uint32 unicast_distribution_memory_for_control_cells[3]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc rtp_unicast_distribution_memory_for_control_cells_tbl_set\n\r"
    "  entry_offset_ndx 0 unicast_distribution_memory_for_control_cells_index 0\n\r"
    "  unicast_distribution_memory_for_control_cells 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_TBL_SET_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_TBL_GET_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_TBL_GET_ID, 
    "unicast_distribution_memory_for_control_cells_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc rtp_unicast_distribution_memory_for_control_cells_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_TBL_GET_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL_SCM_TBL_SET_EGQ_NIF_MAL_SCM_TBL_SET_ID, 
    "nif_mal_scm_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal_scm_tbl_set mal_ndx 0 entry_offset_ndx 0\n\r"
    "  ofp_index 0 port_cr_to_add 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL_SCM_TBL_SET_EGQ_NIF_MAL_SCM_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL_SCM_TBL_SET_EGQ_NIF_MAL_SCM_TBL_SET_MAL_NDX_ID, 
    "key_mal_ndx", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  mal_ndx:\n\r"
    "  SOC_SAND_IN\n\r"
    "  uint32\n\r"
    "  mal_ndx\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal_scm_tbl_set mal_ndx 0 entry_offset_ndx 0\n\r"
    "  ofp_index 0 port_cr_to_add 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL_SCM_TBL_SET_EGQ_NIF_MAL_SCM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL_SCM_TBL_SET_EGQ_NIF_MAL_SCM_TBL_SET_TBL_DATA_OFP_INDEX_ID, 
    "ofp_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.ofp_index:\n\r"
    "  uint32 ofp_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal_scm_tbl_set mal_ndx 0 entry_offset_ndx 0\n\r"
    "  ofp_index 0 port_cr_to_add 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL_SCM_TBL_SET_EGQ_NIF_MAL_SCM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL_SCM_TBL_SET_EGQ_NIF_MAL_SCM_TBL_SET_TBL_DATA_PORT_CR_TO_ADD_ID, 
    "port_cr_to_add", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.port_cr_to_add:\n\r"
    "  uint32 port_cr_to_add\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal_scm_tbl_set mal_ndx 0 entry_offset_ndx 0\n\r"
    "  ofp_index 0 port_cr_to_add 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL_SCM_TBL_SET_EGQ_NIF_MAL_SCM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL_SCM_TBL_GET_EGQ_NIF_MAL_SCM_TBL_GET_ID, 
    "nif_mal_scm_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal_scm_tbl_get mal_ndx 0 entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL_SCM_TBL_GET_EGQ_NIF_MAL_SCM_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL_SCM_TBL_GET_EGQ_NIF_MAL_SCM_TBL_GET_MAL_NDX_ID, 
    "key_mal_ndx", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  mal_ndx:\n\r"
    "  SOC_SAND_IN\n\r"
    "  uint32\n\r"
    "  mal_ndx\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal_scm_tbl_get mal_ndx 0 entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL_SCM_TBL_GET_EGQ_NIF_MAL_SCM_TBL_GET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL1_SCM_TBL_SET_EGQ_NIF_MAL1_SCM_TBL_SET_ID, 
    "nif_mal1_scm_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal1_scm_tbl_set entry_offset_ndx 0 ofp_index 0\n\r"
    "  port_cr_to_add 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL1_SCM_TBL_SET_EGQ_NIF_MAL1_SCM_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL1_SCM_TBL_SET_EGQ_NIF_MAL1_SCM_TBL_SET_TBL_DATA_OFP_INDEX_ID, 
    "ofp_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.ofp_index:\n\r"
    "  uint32 ofp_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal1_scm_tbl_set entry_offset_ndx 0 ofp_index 0\n\r"
    "  port_cr_to_add 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL1_SCM_TBL_SET_EGQ_NIF_MAL1_SCM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL1_SCM_TBL_SET_EGQ_NIF_MAL1_SCM_TBL_SET_TBL_DATA_PORT_CR_TO_ADD_ID, 
    "port_cr_to_add", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.port_cr_to_add:\n\r"
    "  uint32 port_cr_to_add\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal1_scm_tbl_set entry_offset_ndx 0 ofp_index 0\n\r"
    "  port_cr_to_add 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL1_SCM_TBL_SET_EGQ_NIF_MAL1_SCM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL1_SCM_TBL_GET_EGQ_NIF_MAL1_SCM_TBL_GET_ID, 
    "nif_mal1_scm_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal1_scm_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL1_SCM_TBL_GET_EGQ_NIF_MAL1_SCM_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL2_SCM_TBL_SET_EGQ_NIF_MAL2_SCM_TBL_SET_ID, 
    "nif_mal2_scm_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal2_scm_tbl_set entry_offset_ndx 0 ofp_index 0\n\r"
    "  port_cr_to_add 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL2_SCM_TBL_SET_EGQ_NIF_MAL2_SCM_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL2_SCM_TBL_SET_EGQ_NIF_MAL2_SCM_TBL_SET_TBL_DATA_OFP_INDEX_ID, 
    "ofp_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.ofp_index:\n\r"
    "  uint32 ofp_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal2_scm_tbl_set entry_offset_ndx 0 ofp_index 0\n\r"
    "  port_cr_to_add 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL2_SCM_TBL_SET_EGQ_NIF_MAL2_SCM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL2_SCM_TBL_SET_EGQ_NIF_MAL2_SCM_TBL_SET_TBL_DATA_PORT_CR_TO_ADD_ID, 
    "port_cr_to_add", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.port_cr_to_add:\n\r"
    "  uint32 port_cr_to_add\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal2_scm_tbl_set entry_offset_ndx 0 ofp_index 0\n\r"
    "  port_cr_to_add 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL2_SCM_TBL_SET_EGQ_NIF_MAL2_SCM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL2_SCM_TBL_GET_EGQ_NIF_MAL2_SCM_TBL_GET_ID, 
    "nif_mal2_scm_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal2_scm_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL2_SCM_TBL_GET_EGQ_NIF_MAL2_SCM_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL3_SCM_TBL_SET_EGQ_NIF_MAL3_SCM_TBL_SET_ID, 
    "nif_mal3_scm_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal3_scm_tbl_set entry_offset_ndx 0 ofp_index 0\n\r"
    "  port_cr_to_add 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL3_SCM_TBL_SET_EGQ_NIF_MAL3_SCM_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL3_SCM_TBL_SET_EGQ_NIF_MAL3_SCM_TBL_SET_TBL_DATA_OFP_INDEX_ID, 
    "ofp_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.ofp_index:\n\r"
    "  uint32 ofp_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal3_scm_tbl_set entry_offset_ndx 0 ofp_index 0\n\r"
    "  port_cr_to_add 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL3_SCM_TBL_SET_EGQ_NIF_MAL3_SCM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL3_SCM_TBL_SET_EGQ_NIF_MAL3_SCM_TBL_SET_TBL_DATA_PORT_CR_TO_ADD_ID, 
    "port_cr_to_add", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.port_cr_to_add:\n\r"
    "  uint32 port_cr_to_add\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal3_scm_tbl_set entry_offset_ndx 0 ofp_index 0\n\r"
    "  port_cr_to_add 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL3_SCM_TBL_SET_EGQ_NIF_MAL3_SCM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL3_SCM_TBL_GET_EGQ_NIF_MAL3_SCM_TBL_GET_ID, 
    "nif_mal3_scm_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal3_scm_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL3_SCM_TBL_GET_EGQ_NIF_MAL3_SCM_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL4_SCM_TBL_SET_EGQ_NIF_MAL4_SCM_TBL_SET_ID, 
    "nif_mal4_scm_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal4_scm_tbl_set entry_offset_ndx 0 ofp_index 0\n\r"
    "  port_cr_to_add 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL4_SCM_TBL_SET_EGQ_NIF_MAL4_SCM_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL4_SCM_TBL_SET_EGQ_NIF_MAL4_SCM_TBL_SET_TBL_DATA_OFP_INDEX_ID, 
    "ofp_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.ofp_index:\n\r"
    "  uint32 ofp_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal4_scm_tbl_set entry_offset_ndx 0 ofp_index 0\n\r"
    "  port_cr_to_add 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL4_SCM_TBL_SET_EGQ_NIF_MAL4_SCM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL4_SCM_TBL_SET_EGQ_NIF_MAL4_SCM_TBL_SET_TBL_DATA_PORT_CR_TO_ADD_ID, 
    "port_cr_to_add", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.port_cr_to_add:\n\r"
    "  uint32 port_cr_to_add\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal4_scm_tbl_set entry_offset_ndx 0 ofp_index 0\n\r"
    "  port_cr_to_add 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL4_SCM_TBL_SET_EGQ_NIF_MAL4_SCM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL4_SCM_TBL_GET_EGQ_NIF_MAL4_SCM_TBL_GET_ID, 
    "nif_mal4_scm_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal4_scm_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL4_SCM_TBL_GET_EGQ_NIF_MAL4_SCM_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL5_SCM_TBL_SET_EGQ_NIF_MAL5_SCM_TBL_SET_ID, 
    "nif_mal5_scm_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal5_scm_tbl_set entry_offset_ndx 0 ofp_index 0\n\r"
    "  port_cr_to_add 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL5_SCM_TBL_SET_EGQ_NIF_MAL5_SCM_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL5_SCM_TBL_SET_EGQ_NIF_MAL5_SCM_TBL_SET_TBL_DATA_OFP_INDEX_ID, 
    "ofp_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.ofp_index:\n\r"
    "  uint32 ofp_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal5_scm_tbl_set entry_offset_ndx 0 ofp_index 0\n\r"
    "  port_cr_to_add 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL5_SCM_TBL_SET_EGQ_NIF_MAL5_SCM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL5_SCM_TBL_SET_EGQ_NIF_MAL5_SCM_TBL_SET_TBL_DATA_PORT_CR_TO_ADD_ID, 
    "port_cr_to_add", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.port_cr_to_add:\n\r"
    "  uint32 port_cr_to_add\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal5_scm_tbl_set entry_offset_ndx 0 ofp_index 0\n\r"
    "  port_cr_to_add 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL5_SCM_TBL_SET_EGQ_NIF_MAL5_SCM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL5_SCM_TBL_GET_EGQ_NIF_MAL5_SCM_TBL_GET_ID, 
    "nif_mal5_scm_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal5_scm_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL5_SCM_TBL_GET_EGQ_NIF_MAL5_SCM_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL6_SCM_TBL_SET_EGQ_NIF_MAL6_SCM_TBL_SET_ID, 
    "nif_mal6_scm_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal6_scm_tbl_set entry_offset_ndx 0 ofp_index 0\n\r"
    "  port_cr_to_add 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL6_SCM_TBL_SET_EGQ_NIF_MAL6_SCM_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL6_SCM_TBL_SET_EGQ_NIF_MAL6_SCM_TBL_SET_TBL_DATA_OFP_INDEX_ID, 
    "ofp_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.ofp_index:\n\r"
    "  uint32 ofp_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal6_scm_tbl_set entry_offset_ndx 0 ofp_index 0\n\r"
    "  port_cr_to_add 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL6_SCM_TBL_SET_EGQ_NIF_MAL6_SCM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL6_SCM_TBL_SET_EGQ_NIF_MAL6_SCM_TBL_SET_TBL_DATA_PORT_CR_TO_ADD_ID, 
    "port_cr_to_add", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.port_cr_to_add:\n\r"
    "  uint32 port_cr_to_add\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal6_scm_tbl_set entry_offset_ndx 0 ofp_index 0\n\r"
    "  port_cr_to_add 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL6_SCM_TBL_SET_EGQ_NIF_MAL6_SCM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL6_SCM_TBL_GET_EGQ_NIF_MAL6_SCM_TBL_GET_ID, 
    "nif_mal6_scm_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal6_scm_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL6_SCM_TBL_GET_EGQ_NIF_MAL6_SCM_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL7_SCM_TBL_SET_EGQ_NIF_MAL7_SCM_TBL_SET_ID, 
    "nif_mal7_scm_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal7_scm_tbl_set entry_offset_ndx 0 ofp_index 0\n\r"
    "  port_cr_to_add 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL7_SCM_TBL_SET_EGQ_NIF_MAL7_SCM_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL7_SCM_TBL_SET_EGQ_NIF_MAL7_SCM_TBL_SET_TBL_DATA_OFP_INDEX_ID, 
    "ofp_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.ofp_index:\n\r"
    "  uint32 ofp_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal7_scm_tbl_set entry_offset_ndx 0 ofp_index 0\n\r"
    "  port_cr_to_add 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL7_SCM_TBL_SET_EGQ_NIF_MAL7_SCM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL7_SCM_TBL_SET_EGQ_NIF_MAL7_SCM_TBL_SET_TBL_DATA_PORT_CR_TO_ADD_ID, 
    "port_cr_to_add", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.port_cr_to_add:\n\r"
    "  uint32 port_cr_to_add\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal7_scm_tbl_set entry_offset_ndx 0 ofp_index 0\n\r"
    "  port_cr_to_add 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL7_SCM_TBL_SET_EGQ_NIF_MAL7_SCM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL7_SCM_TBL_GET_EGQ_NIF_MAL7_SCM_TBL_GET_ID, 
    "nif_mal7_scm_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal7_scm_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL7_SCM_TBL_GET_EGQ_NIF_MAL7_SCM_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL8_SCM_TBL_SET_EGQ_NIF_MAL8_SCM_TBL_SET_ID, 
    "nif_mal8_scm_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal8_scm_tbl_set entry_offset_ndx 0 ofp_index 0\n\r"
    "  port_cr_to_add 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL8_SCM_TBL_SET_EGQ_NIF_MAL8_SCM_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL8_SCM_TBL_SET_EGQ_NIF_MAL8_SCM_TBL_SET_TBL_DATA_OFP_INDEX_ID, 
    "ofp_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.ofp_index:\n\r"
    "  uint32 ofp_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal8_scm_tbl_set entry_offset_ndx 0 ofp_index 0\n\r"
    "  port_cr_to_add 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL8_SCM_TBL_SET_EGQ_NIF_MAL8_SCM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL8_SCM_TBL_SET_EGQ_NIF_MAL8_SCM_TBL_SET_TBL_DATA_PORT_CR_TO_ADD_ID, 
    "port_cr_to_add", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.port_cr_to_add:\n\r"
    "  uint32 port_cr_to_add\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal8_scm_tbl_set entry_offset_ndx 0 ofp_index 0\n\r"
    "  port_cr_to_add 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL8_SCM_TBL_SET_EGQ_NIF_MAL8_SCM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL8_SCM_TBL_GET_EGQ_NIF_MAL8_SCM_TBL_GET_ID, 
    "nif_mal8_scm_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal8_scm_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL8_SCM_TBL_GET_EGQ_NIF_MAL8_SCM_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL9_SCM_TBL_SET_EGQ_NIF_MAL9_SCM_TBL_SET_ID, 
    "nif_mal9_scm_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal9_scm_tbl_set entry_offset_ndx 0 ofp_index 0\n\r"
    "  port_cr_to_add 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL9_SCM_TBL_SET_EGQ_NIF_MAL9_SCM_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL9_SCM_TBL_SET_EGQ_NIF_MAL9_SCM_TBL_SET_TBL_DATA_OFP_INDEX_ID, 
    "ofp_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.ofp_index:\n\r"
    "  uint32 ofp_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal9_scm_tbl_set entry_offset_ndx 0 ofp_index 0\n\r"
    "  port_cr_to_add 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL9_SCM_TBL_SET_EGQ_NIF_MAL9_SCM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL9_SCM_TBL_SET_EGQ_NIF_MAL9_SCM_TBL_SET_TBL_DATA_PORT_CR_TO_ADD_ID, 
    "port_cr_to_add", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.port_cr_to_add:\n\r"
    "  uint32 port_cr_to_add\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal9_scm_tbl_set entry_offset_ndx 0 ofp_index 0\n\r"
    "  port_cr_to_add 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL9_SCM_TBL_SET_EGQ_NIF_MAL9_SCM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL9_SCM_TBL_GET_EGQ_NIF_MAL9_SCM_TBL_GET_ID, 
    "nif_mal9_scm_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal9_scm_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL9_SCM_TBL_GET_EGQ_NIF_MAL9_SCM_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL10_SCM_TBL_SET_EGQ_NIF_MAL10_SCM_TBL_SET_ID, 
    "nif_mal10_scm_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal10_scm_tbl_set entry_offset_ndx 0 ofp_index 0\n\r"
    "  port_cr_to_add 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL10_SCM_TBL_SET_EGQ_NIF_MAL10_SCM_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL10_SCM_TBL_SET_EGQ_NIF_MAL10_SCM_TBL_SET_TBL_DATA_OFP_INDEX_ID, 
    "ofp_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.ofp_index:\n\r"
    "  uint32 ofp_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal10_scm_tbl_set entry_offset_ndx 0 ofp_index 0\n\r"
    "  port_cr_to_add 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL10_SCM_TBL_SET_EGQ_NIF_MAL10_SCM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL10_SCM_TBL_SET_EGQ_NIF_MAL10_SCM_TBL_SET_TBL_DATA_PORT_CR_TO_ADD_ID, 
    "port_cr_to_add", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.port_cr_to_add:\n\r"
    "  uint32 port_cr_to_add\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal10_scm_tbl_set entry_offset_ndx 0 ofp_index 0\n\r"
    "  port_cr_to_add 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL10_SCM_TBL_SET_EGQ_NIF_MAL10_SCM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL10_SCM_TBL_GET_EGQ_NIF_MAL10_SCM_TBL_GET_ID, 
    "nif_mal10_scm_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal10_scm_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL10_SCM_TBL_GET_EGQ_NIF_MAL10_SCM_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL11_SCM_TBL_SET_EGQ_NIF_MAL11_SCM_TBL_SET_ID, 
    "nif_mal11_scm_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal11_scm_tbl_set entry_offset_ndx 0 ofp_index 0\n\r"
    "  port_cr_to_add 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL11_SCM_TBL_SET_EGQ_NIF_MAL11_SCM_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL11_SCM_TBL_SET_EGQ_NIF_MAL11_SCM_TBL_SET_TBL_DATA_OFP_INDEX_ID, 
    "ofp_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.ofp_index:\n\r"
    "  uint32 ofp_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal11_scm_tbl_set entry_offset_ndx 0 ofp_index 0\n\r"
    "  port_cr_to_add 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL11_SCM_TBL_SET_EGQ_NIF_MAL11_SCM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL11_SCM_TBL_SET_EGQ_NIF_MAL11_SCM_TBL_SET_TBL_DATA_PORT_CR_TO_ADD_ID, 
    "port_cr_to_add", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.port_cr_to_add:\n\r"
    "  uint32 port_cr_to_add\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal11_scm_tbl_set entry_offset_ndx 0 ofp_index 0\n\r"
    "  port_cr_to_add 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL11_SCM_TBL_SET_EGQ_NIF_MAL11_SCM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL11_SCM_TBL_GET_EGQ_NIF_MAL11_SCM_TBL_GET_ID, 
    "nif_mal11_scm_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal11_scm_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL11_SCM_TBL_GET_EGQ_NIF_MAL11_SCM_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL12_SCM_TBL_SET_EGQ_NIF_MAL12_SCM_TBL_SET_ID, 
    "nif_mal12_scm_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal12_scm_tbl_set entry_offset_ndx 0 ofp_index 0\n\r"
    "  port_cr_to_add 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL12_SCM_TBL_SET_EGQ_NIF_MAL12_SCM_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL12_SCM_TBL_SET_EGQ_NIF_MAL12_SCM_TBL_SET_TBL_DATA_OFP_INDEX_ID, 
    "ofp_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.ofp_index:\n\r"
    "  uint32 ofp_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal12_scm_tbl_set entry_offset_ndx 0 ofp_index 0\n\r"
    "  port_cr_to_add 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL12_SCM_TBL_SET_EGQ_NIF_MAL12_SCM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL12_SCM_TBL_SET_EGQ_NIF_MAL12_SCM_TBL_SET_TBL_DATA_PORT_CR_TO_ADD_ID, 
    "port_cr_to_add", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.port_cr_to_add:\n\r"
    "  uint32 port_cr_to_add\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal12_scm_tbl_set entry_offset_ndx 0 ofp_index 0\n\r"
    "  port_cr_to_add 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL12_SCM_TBL_SET_EGQ_NIF_MAL12_SCM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL12_SCM_TBL_GET_EGQ_NIF_MAL12_SCM_TBL_GET_ID, 
    "nif_mal12_scm_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal12_scm_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL12_SCM_TBL_GET_EGQ_NIF_MAL12_SCM_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL13_SCM_TBL_SET_EGQ_NIF_MAL13_SCM_TBL_SET_ID, 
    "nif_mal13_scm_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal13_scm_tbl_set entry_offset_ndx 0 ofp_index 0\n\r"
    "  port_cr_to_add 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL13_SCM_TBL_SET_EGQ_NIF_MAL13_SCM_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL13_SCM_TBL_SET_EGQ_NIF_MAL13_SCM_TBL_SET_TBL_DATA_OFP_INDEX_ID, 
    "ofp_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.ofp_index:\n\r"
    "  uint32 ofp_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal13_scm_tbl_set entry_offset_ndx 0 ofp_index 0\n\r"
    "  port_cr_to_add 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL13_SCM_TBL_SET_EGQ_NIF_MAL13_SCM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL13_SCM_TBL_SET_EGQ_NIF_MAL13_SCM_TBL_SET_TBL_DATA_PORT_CR_TO_ADD_ID, 
    "port_cr_to_add", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.port_cr_to_add:\n\r"
    "  uint32 port_cr_to_add\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal13_scm_tbl_set entry_offset_ndx 0 ofp_index 0\n\r"
    "  port_cr_to_add 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL13_SCM_TBL_SET_EGQ_NIF_MAL13_SCM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL13_SCM_TBL_GET_EGQ_NIF_MAL13_SCM_TBL_GET_ID, 
    "nif_mal13_scm_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal13_scm_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL13_SCM_TBL_GET_EGQ_NIF_MAL13_SCM_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL14_SCM_TBL_SET_EGQ_NIF_MAL14_SCM_TBL_SET_ID, 
    "nif_mal14_scm_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal14_scm_tbl_set entry_offset_ndx 0 ofp_index 0\n\r"
    "  port_cr_to_add 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL14_SCM_TBL_SET_EGQ_NIF_MAL14_SCM_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL14_SCM_TBL_SET_EGQ_NIF_MAL14_SCM_TBL_SET_TBL_DATA_OFP_INDEX_ID, 
    "ofp_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.ofp_index:\n\r"
    "  uint32 ofp_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal14_scm_tbl_set entry_offset_ndx 0 ofp_index 0\n\r"
    "  port_cr_to_add 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL14_SCM_TBL_SET_EGQ_NIF_MAL14_SCM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL14_SCM_TBL_SET_EGQ_NIF_MAL14_SCM_TBL_SET_TBL_DATA_PORT_CR_TO_ADD_ID, 
    "port_cr_to_add", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.port_cr_to_add:\n\r"
    "  uint32 port_cr_to_add\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal14_scm_tbl_set entry_offset_ndx 0 ofp_index 0\n\r"
    "  port_cr_to_add 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL14_SCM_TBL_SET_EGQ_NIF_MAL14_SCM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL14_SCM_TBL_GET_EGQ_NIF_MAL14_SCM_TBL_GET_ID, 
    "nif_mal14_scm_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal14_scm_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL14_SCM_TBL_GET_EGQ_NIF_MAL14_SCM_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL15_SCM_TBL_SET_EGQ_NIF_MAL15_SCM_TBL_SET_ID, 
    "nif_mal15_scm_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal15_scm_tbl_set entry_offset_ndx 0 ofp_index 0\n\r"
    "  port_cr_to_add 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL15_SCM_TBL_SET_EGQ_NIF_MAL15_SCM_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL15_SCM_TBL_SET_EGQ_NIF_MAL15_SCM_TBL_SET_TBL_DATA_OFP_INDEX_ID, 
    "ofp_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.ofp_index:\n\r"
    "  uint32 ofp_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal15_scm_tbl_set entry_offset_ndx 0 ofp_index 0\n\r"
    "  port_cr_to_add 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL15_SCM_TBL_SET_EGQ_NIF_MAL15_SCM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL15_SCM_TBL_SET_EGQ_NIF_MAL15_SCM_TBL_SET_TBL_DATA_PORT_CR_TO_ADD_ID, 
    "port_cr_to_add", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.port_cr_to_add:\n\r"
    "  uint32 port_cr_to_add\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal15_scm_tbl_set entry_offset_ndx 0 ofp_index 0\n\r"
    "  port_cr_to_add 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL15_SCM_TBL_SET_EGQ_NIF_MAL15_SCM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIF_MAL15_SCM_TBL_GET_EGQ_NIF_MAL15_SCM_TBL_GET_ID, 
    "nif_mal15_scm_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nif_mal15_scm_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIF_MAL15_SCM_TBL_GET_EGQ_NIF_MAL15_SCM_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_RCY_SCM_TBL_SET_EGQ_RCY_SCM_TBL_SET_ID, 
    "rcy_scm_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_rcy_scm_tbl_set entry_offset_ndx 0 ofp_index 0\n\r"
    "  port_cr_to_add 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_RCY_SCM_TBL_SET_EGQ_RCY_SCM_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_RCY_SCM_TBL_SET_EGQ_RCY_SCM_TBL_SET_TBL_DATA_OFP_INDEX_ID, 
    "ofp_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.ofp_index:\n\r"
    "  uint32 ofp_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_rcy_scm_tbl_set entry_offset_ndx 0 ofp_index 0\n\r"
    "  port_cr_to_add 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_RCY_SCM_TBL_SET_EGQ_RCY_SCM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_RCY_SCM_TBL_SET_EGQ_RCY_SCM_TBL_SET_TBL_DATA_PORT_CR_TO_ADD_ID, 
    "port_cr_to_add", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.port_cr_to_add:\n\r"
    "  uint32 port_cr_to_add\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_rcy_scm_tbl_set entry_offset_ndx 0 ofp_index 0\n\r"
    "  port_cr_to_add 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_RCY_SCM_TBL_SET_EGQ_RCY_SCM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_RCY_SCM_TBL_GET_EGQ_RCY_SCM_TBL_GET_ID, 
    "rcy_scm_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_rcy_scm_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_RCY_SCM_TBL_GET_EGQ_RCY_SCM_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_CPU_SCM_TBL_SET_EGQ_CPU_SCM_TBL_SET_ID, 
    "cpu_scm_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_cpu_scm_tbl_set entry_offset_ndx 0 ofp_index 0\n\r"
    "  port_cr_to_add 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_CPU_SCM_TBL_SET_EGQ_CPU_SCM_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_CPU_SCM_TBL_SET_EGQ_CPU_SCM_TBL_SET_TBL_DATA_OFP_INDEX_ID, 
    "ofp_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.ofp_index:\n\r"
    "  uint32 ofp_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_cpu_scm_tbl_set entry_offset_ndx 0 ofp_index 0\n\r"
    "  port_cr_to_add 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_CPU_SCM_TBL_SET_EGQ_CPU_SCM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_CPU_SCM_TBL_SET_EGQ_CPU_SCM_TBL_SET_TBL_DATA_PORT_CR_TO_ADD_ID, 
    "port_cr_to_add", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.port_cr_to_add:\n\r"
    "  uint32 port_cr_to_add\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_cpu_scm_tbl_set entry_offset_ndx 0 ofp_index 0\n\r"
    "  port_cr_to_add 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_CPU_SCM_TBL_SET_EGQ_CPU_SCM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_CPU_SCM_TBL_GET_EGQ_CPU_SCM_TBL_GET_ID, 
    "cpu_scm_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_cpu_scm_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_CPU_SCM_TBL_GET_EGQ_CPU_SCM_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_CCM_TBL_SET_EGQ_CCM_TBL_SET_ID, 
    "ccm_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_ccm_tbl_set entry_offset_ndx 0 interface_select 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_CCM_TBL_SET_EGQ_CCM_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_CCM_TBL_SET_EGQ_CCM_TBL_SET_TBL_DATA_INTERFACE_SELECT_ID, 
    "interface_select", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.interface_select:\n\r"
    "  uint32 interface_select\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_ccm_tbl_set entry_offset_ndx 0 interface_select 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_CCM_TBL_SET_EGQ_CCM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_CCM_TBL_GET_EGQ_CCM_TBL_GET_ID, 
    "ccm_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_ccm_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_CCM_TBL_GET_EGQ_CCM_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PMC_TBL_SET_EGQ_PMC_TBL_SET_ID, 
    "pmc_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_pmc_tbl_set entry_offset_ndx 0 port_max_credit 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PMC_TBL_SET_EGQ_PMC_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PMC_TBL_SET_EGQ_PMC_TBL_SET_TBL_DATA_PORT_MAX_CREDIT_ID, 
    "port_max_credit", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.port_max_credit:\n\r"
    "  uint32 port_max_credit\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_pmc_tbl_set entry_offset_ndx 0 port_max_credit 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PMC_TBL_SET_EGQ_PMC_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PMC_TBL_GET_EGQ_PMC_TBL_GET_ID, 
    "pmc_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_pmc_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PMC_TBL_GET_EGQ_PMC_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_CBM_TBL_SET_EGQ_CBM_TBL_SET_ID, 
    "cbm_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_cbm_tbl_set entry_offset_ndx 0\n\r"
    "  egress_shaper_per_port_credit_balance_memory_cbm 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_CBM_TBL_SET_EGQ_CBM_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_CBM_TBL_SET_EGQ_CBM_TBL_SET_TBL_DATA_EGRESS_SHAPER_PER_PORT_CREDIT_BALANCE_MEMORY_CBM_ID, 
    "egress_shaper_per_port_credit_balance_memory_cbm", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.egress_shaper_per_port_credit_balance_memory_cbm:\n\r"
    "  uint32 egress_shaper_per_port_credit_balance_memory_cbm\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_cbm_tbl_set entry_offset_ndx 0\n\r"
    "  egress_shaper_per_port_credit_balance_memory_cbm 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_CBM_TBL_SET_EGQ_CBM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_CBM_TBL_GET_EGQ_CBM_TBL_GET_ID, 
    "cbm_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_cbm_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_CBM_TBL_GET_EGQ_CBM_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_FBM_TBL_SET_EGQ_FBM_TBL_SET_ID, 
    "fbm_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_fbm_tbl_set entry_offset_ndx 0 free_buffer_memory 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_FBM_TBL_SET_EGQ_FBM_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_FBM_TBL_SET_EGQ_FBM_TBL_SET_TBL_DATA_FREE_BUFFER_MEMORY_ID, 
    "free_buffer_memory", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.free_buffer_memory:\n\r"
    "  uint32 free_buffer_memory\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_fbm_tbl_set entry_offset_ndx 0 free_buffer_memory 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_FBM_TBL_SET_EGQ_FBM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_FBM_TBL_GET_EGQ_FBM_TBL_GET_ID, 
    "fbm_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_fbm_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_FBM_TBL_GET_EGQ_FBM_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_FDM_TBL_SET_EGQ_FDM_TBL_SET_ID, 
    "fdm_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_fdm_tbl_set entry_offset_ndx 0\n\r"
    "  free_descriptor_memory_index 0 free_descriptor_memory 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_FDM_TBL_SET_EGQ_FDM_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_FDM_TBL_SET_EGQ_FDM_TBL_SET_TBL_DATA_FREE_DESCRIPTOR_MEMORY_INDEX_ID, 
    "free_descriptor_memory_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  free_descriptor_memory_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_fdm_tbl_set entry_offset_ndx 0\n\r"
    "  free_descriptor_memory_index 0 free_descriptor_memory 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_FDM_TBL_SET_EGQ_FDM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_FDM_TBL_SET_EGQ_FDM_TBL_SET_TBL_DATA_FREE_DESCRIPTOR_MEMORY_ID, 
    "free_descriptor_memory", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.free_descriptor_memory[ prm_free_descriptor_memory_index]:\n\r"
    "  uint32 free_descriptor_memory[2]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_fdm_tbl_set entry_offset_ndx 0\n\r"
    "  free_descriptor_memory_index 0 free_descriptor_memory 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_FDM_TBL_SET_EGQ_FDM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_FDM_TBL_GET_EGQ_FDM_TBL_GET_ID, 
    "fdm_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_fdm_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_FDM_TBL_GET_EGQ_FDM_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_QM_TBL_SET_EGQ_QM_TBL_SET_ID, 
    "qm_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_qm_tbl_set entry_offset_ndx 0 pqp_oc768_qnum 0\n\r"
    "  pqp_qnum 0 before_last_seg_size 0 last_seg_size 0 copy_data 0\n\r"
    "  counter_decreament 0 packet_size256to_eop 0 read_pointer 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_QM_TBL_SET_EGQ_QM_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_QM_TBL_SET_EGQ_QM_TBL_SET_TBL_DATA_PQP_OC768_QNUM_ID, 
    "pqp_oc768_qnum", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.pqp_oc768_qnum:\n\r"
    "  uint32 pqp_oc768_qnum\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_qm_tbl_set entry_offset_ndx 0 pqp_oc768_qnum 0\n\r"
    "  pqp_qnum 0 before_last_seg_size 0 last_seg_size 0 copy_data 0\n\r"
    "  counter_decreament 0 packet_size256to_eop 0 read_pointer 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_QM_TBL_SET_EGQ_QM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_QM_TBL_SET_EGQ_QM_TBL_SET_TBL_DATA_PQP_QNUM_ID, 
    "pqp_qnum", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.pqp_qnum:\n\r"
    "  uint32 pqp_qnum\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_qm_tbl_set entry_offset_ndx 0 pqp_oc768_qnum 0\n\r"
    "  pqp_qnum 0 before_last_seg_size 0 last_seg_size 0 copy_data 0\n\r"
    "  counter_decreament 0 packet_size256to_eop 0 read_pointer 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_QM_TBL_SET_EGQ_QM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_QM_TBL_SET_EGQ_QM_TBL_SET_TBL_DATA_BEFORE_LAST_SEG_SIZE_ID, 
    "before_last_seg_size", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.before_last_seg_size:\n\r"
    "  uint32 before_last_seg_size\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_qm_tbl_set entry_offset_ndx 0 pqp_oc768_qnum 0\n\r"
    "  pqp_qnum 0 before_last_seg_size 0 last_seg_size 0 copy_data 0\n\r"
    "  counter_decreament 0 packet_size256to_eop 0 read_pointer 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_QM_TBL_SET_EGQ_QM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_QM_TBL_SET_EGQ_QM_TBL_SET_TBL_DATA_LAST_SEG_SIZE_ID, 
    "last_seg_size", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.last_seg_size:\n\r"
    "  uint32 last_seg_size\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_qm_tbl_set entry_offset_ndx 0 pqp_oc768_qnum 0\n\r"
    "  pqp_qnum 0 before_last_seg_size 0 last_seg_size 0 copy_data 0\n\r"
    "  counter_decreament 0 packet_size256to_eop 0 read_pointer 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_QM_TBL_SET_EGQ_QM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_QM_TBL_SET_EGQ_QM_TBL_SET_TBL_DATA_COPY_DATA_ID, 
    "copy_data", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.copy_data:\n\r"
    "  uint32 copy_data\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_qm_tbl_set entry_offset_ndx 0 pqp_oc768_qnum 0\n\r"
    "  pqp_qnum 0 before_last_seg_size 0 last_seg_size 0 copy_data 0\n\r"
    "  counter_decreament 0 packet_size256to_eop 0 read_pointer 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_QM_TBL_SET_EGQ_QM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_QM_TBL_SET_EGQ_QM_TBL_SET_TBL_DATA_COUNTER_DECREAMENT_ID, 
    "counter_decreament", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.counter_decreament:\n\r"
    "  uint32 counter_decreament\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_qm_tbl_set entry_offset_ndx 0 pqp_oc768_qnum 0\n\r"
    "  pqp_qnum 0 before_last_seg_size 0 last_seg_size 0 copy_data 0\n\r"
    "  counter_decreament 0 packet_size256to_eop 0 read_pointer 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_QM_TBL_SET_EGQ_QM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_QM_TBL_SET_EGQ_QM_TBL_SET_TBL_DATA_PACKET_SIZE256TO_EOP_ID, 
    "packet_size256to_eop", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.packet_size256to_eop:\n\r"
    "  uint32 packet_size256to_eop\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_qm_tbl_set entry_offset_ndx 0 pqp_oc768_qnum 0\n\r"
    "  pqp_qnum 0 before_last_seg_size 0 last_seg_size 0 copy_data 0\n\r"
    "  counter_decreament 0 packet_size256to_eop 0 read_pointer 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_QM_TBL_SET_EGQ_QM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_QM_TBL_SET_EGQ_QM_TBL_SET_TBL_DATA_READ_POINTER_ID, 
    "read_pointer", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.read_pointer:\n\r"
    "  uint32 read_pointer\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_qm_tbl_set entry_offset_ndx 0 pqp_oc768_qnum 0\n\r"
    "  pqp_qnum 0 before_last_seg_size 0 last_seg_size 0 copy_data 0\n\r"
    "  counter_decreament 0 packet_size256to_eop 0 read_pointer 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_QM_TBL_SET_EGQ_QM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_QM_TBL_GET_EGQ_QM_TBL_GET_ID, 
    "qm_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_qm_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_QM_TBL_GET_EGQ_QM_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_QSM_TBL_SET_EGQ_QSM_TBL_SET_ID, 
    "qsm_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_qsm_tbl_set entry_offset_ndx 0 qsm 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_QSM_TBL_SET_EGQ_QSM_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_QSM_TBL_SET_EGQ_QSM_TBL_SET_TBL_DATA_QSM_ID, 
    "qsm", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.qsm:\n\r"
    "  uint32 qsm\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_qsm_tbl_set entry_offset_ndx 0 qsm 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_QSM_TBL_SET_EGQ_QSM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_QSM_TBL_GET_EGQ_QSM_TBL_GET_ID, 
    "qsm_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_qsm_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_QSM_TBL_GET_EGQ_QSM_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_DCM_TBL_SET_EGQ_DCM_TBL_SET_ID, 
    "dcm_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_dcm_tbl_set entry_offset_ndx 0 dcm 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_DCM_TBL_SET_EGQ_DCM_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_DCM_TBL_SET_EGQ_DCM_TBL_SET_TBL_DATA_DCM_ID, 
    "dcm", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.dcm:\n\r"
    "  uint32 dcm\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_dcm_tbl_set entry_offset_ndx 0 dcm 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_DCM_TBL_SET_EGQ_DCM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_DCM_TBL_GET_EGQ_DCM_TBL_GET_ID, 
    "dcm_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_dcm_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_DCM_TBL_GET_EGQ_DCM_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_DWM_TBL_SET_EGQ_DWM_TBL_SET_ID, 
    "dwm_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_dwm_tbl_set entry_offset_ndx 0\n\r"
    "  uc_or_uc_low_queue_weight 0 mc_or_mc_low_queue_weight 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_DWM_TBL_SET_EGQ_DWM_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_DWM_TBL_SET_EGQ_DWM_TBL_SET_TBL_DATA_UC_OR_UC_LOW_QUEUE_WEIGHT_ID, 
    "uc_or_uc_low_queue_weight", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.uc_or_uc_low_queue_weight:\n\r"
    "  uint32 uc_or_uc_low_queue_weight\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_dwm_tbl_set entry_offset_ndx 0\n\r"
    "  uc_or_uc_low_queue_weight 0 mc_or_mc_low_queue_weight 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_DWM_TBL_SET_EGQ_DWM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_DWM_TBL_SET_EGQ_DWM_TBL_SET_TBL_DATA_MC_OR_MC_LOW_QUEUE_WEIGHT_ID, 
    "mc_or_mc_low_queue_weight", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.mc_or_mc_low_queue_weight:\n\r"
    "  uint32 mc_or_mc_low_queue_weight\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_dwm_tbl_set entry_offset_ndx 0\n\r"
    "  uc_or_uc_low_queue_weight 0 mc_or_mc_low_queue_weight 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_DWM_TBL_SET_EGQ_DWM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_DWM_TBL_GET_EGQ_DWM_TBL_GET_ID, 
    "dwm_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_dwm_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_DWM_TBL_GET_EGQ_DWM_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_DWM_NEW_TBL_SET_EGQ_DWM_NEW_TBL_SET_ID, 
    "dwm_new_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_dwm_new_tbl_set entry_offset_ndx 0\n\r"
    "  uc_or_uc_low_queue_weight 0 mc_or_mc_low_queue_weight 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_DWM_NEW_TBL_SET_EGQ_DWM_NEW_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_DWM_NEW_TBL_SET_EGQ_DWM_NEW_TBL_SET_TBL_DATA_UC_OR_UC_LOW_QUEUE_WEIGHT_ID, 
    "uc_high_queue_weight", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.uc_or_uc_low_queue_weight:\n\r"
    "  uint32 uc_or_uc_low_queue_weight\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_dwm_new_tbl_set entry_offset_ndx 0\n\r"
    "  uc_or_uc_low_queue_weight 0 mc_or_mc_low_queue_weight 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_DWM_NEW_TBL_SET_EGQ_DWM_NEW_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_DWM_NEW_TBL_SET_EGQ_DWM_NEW_TBL_SET_TBL_DATA_MC_OR_MC_LOW_QUEUE_WEIGHT_ID, 
    "mc_high_queue_weight", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.mc_or_mc_low_queue_weight:\n\r"
    "  uint32 mc_or_mc_low_queue_weight\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_dwm_new_tbl_set entry_offset_ndx 0\n\r"
    "  uc_or_uc_low_queue_weight 0 mc_or_mc_low_queue_weight 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_DWM_NEW_TBL_SET_EGQ_DWM_NEW_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_DWM_NEW_TBL_GET_EGQ_DWM_NEW_TBL_GET_ID, 
    "dwm_new_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_dwm_new_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_DWM_NEW_TBL_GET_EGQ_DWM_NEW_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_RRDM_TBL_SET_EGQ_RRDM_TBL_SET_ID, 
    "rrdm_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_rrdm_tbl_set entry_offset_ndx 0 fix129 0\n\r"
    "  stored_seg_size 0 nxt_frag_number 0 eopfrag_num 0 reas_state 0 crcremainder 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_RRDM_TBL_SET_EGQ_RRDM_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_RRDM_TBL_SET_EGQ_RRDM_TBL_SET_TBL_DATA_FIX129_ID, 
    "fix129", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.fix129:\n\r"
    "  uint32 fix129\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_rrdm_tbl_set entry_offset_ndx 0 fix129 0\n\r"
    "  stored_seg_size 0 nxt_frag_number 0 eopfrag_num 0 reas_state 0 crcremainder 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_RRDM_TBL_SET_EGQ_RRDM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_RRDM_TBL_SET_EGQ_RRDM_TBL_SET_TBL_DATA_STORED_SEG_SIZE_ID, 
    "stored_seg_size", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.stored_seg_size:\n\r"
    "  uint32 stored_seg_size\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_rrdm_tbl_set entry_offset_ndx 0 fix129 0\n\r"
    "  stored_seg_size 0 nxt_frag_number 0 eopfrag_num 0 reas_state 0 crcremainder 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_RRDM_TBL_SET_EGQ_RRDM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_RRDM_TBL_SET_EGQ_RRDM_TBL_SET_TBL_DATA_NXT_FRAG_NUMBER_ID, 
    "nxt_frag_number", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.nxt_frag_number:\n\r"
    "  uint32 nxt_frag_number\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_rrdm_tbl_set entry_offset_ndx 0 fix129 0\n\r"
    "  stored_seg_size 0 nxt_frag_number 0 eopfrag_num 0 reas_state 0 crcremainder 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_RRDM_TBL_SET_EGQ_RRDM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_RRDM_TBL_SET_EGQ_RRDM_TBL_SET_TBL_DATA_EOPFRAG_NUM_ID, 
    "eopfrag_num", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.eopfrag_num:\n\r"
    "  uint32 eopfrag_num\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_rrdm_tbl_set entry_offset_ndx 0 fix129 0\n\r"
    "  stored_seg_size 0 nxt_frag_number 0 eopfrag_num 0 reas_state 0 crcremainder 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_RRDM_TBL_SET_EGQ_RRDM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_RRDM_TBL_SET_EGQ_RRDM_TBL_SET_TBL_DATA_REAS_STATE_ID, 
    "reas_state", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.reas_state:\n\r"
    "  uint32 reas_state\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_rrdm_tbl_set entry_offset_ndx 0 fix129 0\n\r"
    "  stored_seg_size 0 nxt_frag_number 0 eopfrag_num 0 reas_state 0 crcremainder 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_RRDM_TBL_SET_EGQ_RRDM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_RRDM_TBL_SET_EGQ_RRDM_TBL_SET_TBL_DATA_CRCREMAINDER_ID, 
    "crcremainder", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.crcremainder:\n\r"
    "  uint32 crcremainder\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_rrdm_tbl_set entry_offset_ndx 0 fix129 0\n\r"
    "  stored_seg_size 0 nxt_frag_number 0 eopfrag_num 0 reas_state 0 crcremainder 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_RRDM_TBL_SET_EGQ_RRDM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_RRDM_TBL_GET_EGQ_RRDM_TBL_GET_ID, 
    "rrdm_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_rrdm_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_RRDM_TBL_GET_EGQ_RRDM_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_RPDM_TBL_SET_EGQ_RPDM_TBL_SET_ID, 
    "rpdm_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_rpdm_tbl_set entry_offset_ndx 0 packet_frag_cnt 0\n\r"
    "  reas_state 0 packet_buffer_write_pointer 0 packet_start_buffer_pointer 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_RPDM_TBL_SET_EGQ_RPDM_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_RPDM_TBL_SET_EGQ_RPDM_TBL_SET_TBL_DATA_PACKET_FRAG_CNT_ID, 
    "packet_frag_cnt", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.packet_frag_cnt:\n\r"
    "  uint32 packet_frag_cnt\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_rpdm_tbl_set entry_offset_ndx 0 packet_frag_cnt 0\n\r"
    "  reas_state 0 packet_buffer_write_pointer 0 packet_start_buffer_pointer 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_RPDM_TBL_SET_EGQ_RPDM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_RPDM_TBL_SET_EGQ_RPDM_TBL_SET_TBL_DATA_REAS_STATE_ID, 
    "reas_state", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.reas_state:\n\r"
    "  uint32 reas_state\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_rpdm_tbl_set entry_offset_ndx 0 packet_frag_cnt 0\n\r"
    "  reas_state 0 packet_buffer_write_pointer 0 packet_start_buffer_pointer 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_RPDM_TBL_SET_EGQ_RPDM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_RPDM_TBL_SET_EGQ_RPDM_TBL_SET_TBL_DATA_PACKET_BUFFER_WRITE_POINTER_ID, 
    "packet_buffer_write_pointer", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.packet_buffer_write_pointer:\n\r"
    "  uint32 packet_buffer_write_pointer\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_rpdm_tbl_set entry_offset_ndx 0 packet_frag_cnt 0\n\r"
    "  reas_state 0 packet_buffer_write_pointer 0 packet_start_buffer_pointer 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_RPDM_TBL_SET_EGQ_RPDM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_RPDM_TBL_SET_EGQ_RPDM_TBL_SET_TBL_DATA_PACKET_START_BUFFER_POINTER_ID, 
    "packet_start_buffer_pointer", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.packet_start_buffer_pointer:\n\r"
    "  uint32 packet_start_buffer_pointer\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_rpdm_tbl_set entry_offset_ndx 0 packet_frag_cnt 0\n\r"
    "  reas_state 0 packet_buffer_write_pointer 0 packet_start_buffer_pointer 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_RPDM_TBL_SET_EGQ_RPDM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_RPDM_TBL_GET_EGQ_RPDM_TBL_GET_ID, 
    "rpdm_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_rpdm_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_RPDM_TBL_GET_EGQ_RPDM_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_SET_ID, 
    "pct_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_pct_tbl_set entry_offset_ndx 0 cr_adjust_type 0\n\r"
    "  port_type 0 port_ch_num 0 port_profile 0 dest_port 0 outbound_mirr 0\n\r"
    "  out_pp_port 0 prog_editor_value 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_SET_TBL_DATA_CR_ADJUST_TYPE_ID, 
    "cr_adjust_type", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.cr_adjust_type:\n\r"
    "  uint32 cr_adjust_type\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_pct_tbl_set entry_offset_ndx 0 cr_adjust_type 0\n\r"
    "  port_type 0 port_ch_num 0 port_profile 0 dest_port 0 outbound_mirr 0\n\r"
    "  out_pp_port 0 prog_editor_value 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_SET_TBL_DATA_PORT_TYPE_ID, 
    "port_type", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.port_type:\n\r"
    "  uint32 port_type\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_pct_tbl_set entry_offset_ndx 0 cr_adjust_type 0\n\r"
    "  port_type 0 port_ch_num 0 port_profile 0 dest_port 0 outbound_mirr 0\n\r"
    "  out_pp_port 0 prog_editor_value 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_SET_TBL_DATA_PORT_CH_NUM_ID, 
    "port_ch_num", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.port_ch_num:\n\r"
    "  uint32 port_ch_num\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_pct_tbl_set entry_offset_ndx 0 cr_adjust_type 0\n\r"
    "  port_type 0 port_ch_num 0 port_profile 0 dest_port 0 outbound_mirr 0\n\r"
    "  out_pp_port 0 prog_editor_value 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_SET_TBL_DATA_PORT_PROFILE_ID, 
    "port_profile", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.port_profile:\n\r"
    "  uint32 port_profile\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_pct_tbl_set entry_offset_ndx 0 cr_adjust_type 0\n\r"
    "  port_type 0 port_ch_num 0 port_profile 0 dest_port 0 outbound_mirr 0\n\r"
    "  out_pp_port 0 prog_editor_value 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_SET_TBL_DATA_DEST_PORT_ID, 
    "dest_port", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.dest_port:\n\r"
    "  uint32 dest_port\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_pct_tbl_set entry_offset_ndx 0 cr_adjust_type 0\n\r"
    "  port_type 0 port_ch_num 0 port_profile 0 dest_port 0 outbound_mirr 0\n\r"
    "  out_pp_port 0 prog_editor_value 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_SET_TBL_DATA_OUTBOUND_MIRR_ID, 
    "outbound_mirr", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.outbound_mirr:\n\r"
    "  uint32 outbound_mirr\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_pct_tbl_set entry_offset_ndx 0 cr_adjust_type 0\n\r"
    "  port_type 0 port_ch_num 0 port_profile 0 dest_port 0 outbound_mirr 0\n\r"
    "  out_pp_port 0 prog_editor_value 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_SET_TBL_DATA_OUT_PP_PORT_ID, 
    "out_pp_port", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.out_pp_port:\n\r"
    "  uint32 out_pp_port\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_pct_tbl_set entry_offset_ndx 0 cr_adjust_type 0\n\r"
    "  port_type 0 port_ch_num 0 port_profile 0 dest_port 0 outbound_mirr 0\n\r"
    "  out_pp_port 0 prog_editor_value 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_SET_TBL_DATA_PROG_EDITOR_VALUE_ID, 
    "prog_editor_value", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.prog_editor_value:\n\r"
    "  uint32 prog_editor_value\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_pct_tbl_set entry_offset_ndx 0 cr_adjust_type 0\n\r"
    "  port_type 0 port_ch_num 0 port_profile 0 dest_port 0 outbound_mirr 0\n\r"
    "  out_pp_port 0 prog_editor_value 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PCT_TBL_GET_EGQ_PCT_TBL_GET_ID, 
    "pct_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_pct_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PCT_TBL_GET_EGQ_PCT_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_VLAN_TABLE_TBL_SET_EGQ_VLAN_TABLE_TBL_SET_ID, 
    "vlan_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_vlan_table_tbl_set entry_offset_ndx 0\n\r"
    "  vlan_membership_index 0 vlan_membership 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_VLAN_TABLE_TBL_SET_EGQ_VLAN_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_VLAN_TABLE_TBL_SET_EGQ_VLAN_TABLE_TBL_SET_TBL_DATA_VLAN_MEMBERSHIP_INDEX_ID, 
    "vlan_membership_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  vlan_membership_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_vlan_table_tbl_set entry_offset_ndx 0\n\r"
    "  vlan_membership_index 0 vlan_membership 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_VLAN_TABLE_TBL_SET_EGQ_VLAN_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_VLAN_TABLE_TBL_SET_EGQ_VLAN_TABLE_TBL_SET_TBL_DATA_VLAN_MEMBERSHIP_ID, 
    "vlan_membership", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.vlan_membership[ prm_vlan_membership_index]:\n\r"
    "  uint32 vlan_membership[3]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_vlan_table_tbl_set entry_offset_ndx 0\n\r"
    "  vlan_membership_index 0 vlan_membership 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_VLAN_TABLE_TBL_SET_EGQ_VLAN_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_VLAN_TABLE_TBL_GET_EGQ_VLAN_TABLE_TBL_GET_ID, 
    "vlan_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_vlan_table_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_VLAN_TABLE_TBL_GET_EGQ_VLAN_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_TC_DP_MAP_TBL_SET_EGQ_TC_DP_MAP_TBL_SET_ID, 
    "tc_dp_map_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_tc_dp_map_tbl_set dp 0 tc 0 is_syst_mc 0 is_egr_mc 0\n\r"
    "  map_profile 0\n\r"
    "  0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_TC_DP_MAP_TBL_SET_EGQ_TC_DP_MAP_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_TC_DP_MAP_TBL_SET_EGQ_TC_DP_MAP_TBL_SET_ENTRY_DP_ID, 
    "dp", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  entry.dp:\n\r"
    "  /* 2b */\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_tc_dp_map_tbl_set dp 0 tc 0 is_syst_mc 0 is_egr_mc 0\n\r"
    "  map_profile 0\n\r"
    "  0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_TC_DP_MAP_TBL_SET_EGQ_TC_DP_MAP_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_TC_DP_MAP_TBL_SET_EGQ_TC_DP_MAP_TBL_SET_ENTRY_TC_ID, 
    "tc", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  entry.tc:\n\r"
    "  /* 3b */\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_tc_dp_map_tbl_set dp 0 tc 0 is_syst_mc 0 is_egr_mc 0\n\r"
    "  map_profile 0\n\r"
    "  0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_TC_DP_MAP_TBL_SET_EGQ_TC_DP_MAP_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_TC_DP_MAP_TBL_SET_EGQ_TC_DP_MAP_TBL_SET_ENTRY_IS_SYST_MC_ID, 
    "is_syst_mc", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  entry.is_syst_mc:\n\r"
    "  /* 1b */\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_tc_dp_map_tbl_set dp 0 tc 0 is_syst_mc 0 is_egr_mc 0\n\r"
    "  map_profile 0\n\r"
    "  0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_TC_DP_MAP_TBL_SET_EGQ_TC_DP_MAP_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_TC_DP_MAP_TBL_SET_EGQ_TC_DP_MAP_TBL_SET_ENTRY_IS_EGR_MC_ID, 
    "is_egr_mc", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  entry.is_egr_mc:\n\r"
    "  /* 1b */\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_tc_dp_map_tbl_set dp 0 tc 0 is_syst_mc 0 is_egr_mc 0\n\r"
    "  map_profile 0\n\r"
    "  0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_TC_DP_MAP_TBL_SET_EGQ_TC_DP_MAP_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_TC_DP_MAP_TBL_SET_EGQ_TC_DP_MAP_TBL_SET_ENTRY_MAP_PROFILE_ID, 
    "map_profile", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  entry.map_profile:\n\r"
    "  /* 2b */\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_tc_dp_map_tbl_set dp 0 tc 0 is_syst_mc 0 is_egr_mc 0\n\r"
    "  map_profile 0\n\r"
    "  0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_TC_DP_MAP_TBL_SET_EGQ_TC_DP_MAP_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_TC_DP_MAP_TBL_SET_EGQ_TC_DP_MAP_TBL_SET_ENTRY__ID, 
    "", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  entry.:\n\r"
    "  SOC_SAND_MAGIC_NUM_VAR\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_tc_dp_map_tbl_set dp 0 tc 0 is_syst_mc 0 is_egr_mc 0\n\r"
    "  map_profile 0\n\r"
    "  0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_TC_DP_MAP_TBL_SET_EGQ_TC_DP_MAP_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_TC_DP_MAP_TBL_SET_EGQ_TC_DP_MAP_TBL_SET_TBL_DATA_DP_ID, 
    "dp", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.dp:\n\r"
    "  uint32 dp\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_tc_dp_map_tbl_set dp 0 tc 0 is_syst_mc 0 is_egr_mc 0\n\r"
    "  map_profile 0\n\r"
    "  0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_TC_DP_MAP_TBL_SET_EGQ_TC_DP_MAP_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_TC_DP_MAP_TBL_SET_EGQ_TC_DP_MAP_TBL_SET_TBL_DATA_TC_ID, 
    "tc", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.tc:\n\r"
    "  uint32 tc\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_tc_dp_map_tbl_set dp 0 tc 0 is_syst_mc 0 is_egr_mc 0\n\r"
    "  map_profile 0\n\r"
    "  0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_TC_DP_MAP_TBL_SET_EGQ_TC_DP_MAP_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_TC_DP_MAP_TBL_GET_EGQ_TC_DP_MAP_TBL_GET_ID, 
    "tc_dp_map_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_tc_dp_map_tbl_get\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_TC_DP_MAP_TBL_GET_EGQ_TC_DP_MAP_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_CFC_FLOW_CONTROL_TBL_SET_EGQ_CFC_FLOW_CONTROL_TBL_SET_ID, 
    "flow_control_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_cfc_flow_control_tbl_set entry_offset_ndx 0\n\r"
    "  cfc_flow_control_index 0 cfc_flow_control 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_CFC_FLOW_CONTROL_TBL_SET_EGQ_CFC_FLOW_CONTROL_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_CFC_FLOW_CONTROL_TBL_SET_EGQ_CFC_FLOW_CONTROL_TBL_SET_TBL_DATA_CFC_FLOW_CONTROL_INDEX_ID, 
    "flow_control_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  cfc_flow_control_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_cfc_flow_control_tbl_set entry_offset_ndx 0\n\r"
    "  cfc_flow_control_index 0 cfc_flow_control 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_CFC_FLOW_CONTROL_TBL_SET_EGQ_CFC_FLOW_CONTROL_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_CFC_FLOW_CONTROL_TBL_SET_EGQ_CFC_FLOW_CONTROL_TBL_SET_TBL_DATA_CFC_FLOW_CONTROL_ID, 
    "flow_control", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.cfc_flow_control[ prm_cfc_flow_control_index]:\n\r"
    "  uint32 cfc_flow_control[3]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_cfc_flow_control_tbl_set entry_offset_ndx 0\n\r"
    "  cfc_flow_control_index 0 cfc_flow_control 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_CFC_FLOW_CONTROL_TBL_SET_EGQ_CFC_FLOW_CONTROL_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_CFC_FLOW_CONTROL_TBL_GET_EGQ_CFC_FLOW_CONTROL_TBL_GET_ID, 
    "flow_control_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_cfc_flow_control_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_CFC_FLOW_CONTROL_TBL_GET_EGQ_CFC_FLOW_CONTROL_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIFA_FLOW_CONTROL_TBL_SET_EGQ_NIFA_FLOW_CONTROL_TBL_SET_ID, 
    "nifa_flow_control_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nifa_flow_control_tbl_set entry_offset_ndx 0\n\r"
    "  nifa_flow_control 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIFA_FLOW_CONTROL_TBL_SET_EGQ_NIFA_FLOW_CONTROL_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIFA_FLOW_CONTROL_TBL_SET_EGQ_NIFA_FLOW_CONTROL_TBL_SET_TBL_DATA_NIFA_FLOW_CONTROL_ID, 
    "nifa_flow_control", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.nifa_flow_control:\n\r"
    "  uint32 nifa_flow_control\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nifa_flow_control_tbl_set entry_offset_ndx 0\n\r"
    "  nifa_flow_control 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIFA_FLOW_CONTROL_TBL_SET_EGQ_NIFA_FLOW_CONTROL_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIFA_FLOW_CONTROL_TBL_GET_EGQ_NIFA_FLOW_CONTROL_TBL_GET_ID, 
    "nifa_flow_control_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nifa_flow_control_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIFA_FLOW_CONTROL_TBL_GET_EGQ_NIFA_FLOW_CONTROL_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIFB_FLOW_CONTROL_TBL_SET_EGQ_NIFB_FLOW_CONTROL_TBL_SET_ID, 
    "nifb_flow_control_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nifb_flow_control_tbl_set entry_offset_ndx 0\n\r"
    "  nifb_flow_control 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIFB_FLOW_CONTROL_TBL_SET_EGQ_NIFB_FLOW_CONTROL_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIFB_FLOW_CONTROL_TBL_SET_EGQ_NIFB_FLOW_CONTROL_TBL_SET_TBL_DATA_NIFB_FLOW_CONTROL_ID, 
    "nifb_flow_control", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.nifb_flow_control:\n\r"
    "  uint32 nifb_flow_control\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nifb_flow_control_tbl_set entry_offset_ndx 0\n\r"
    "  nifb_flow_control 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIFB_FLOW_CONTROL_TBL_SET_EGQ_NIFB_FLOW_CONTROL_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_NIFB_FLOW_CONTROL_TBL_GET_EGQ_NIFB_FLOW_CONTROL_TBL_GET_ID, 
    "nifb_flow_control_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_nifb_flow_control_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_NIFB_FLOW_CONTROL_TBL_GET_EGQ_NIFB_FLOW_CONTROL_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_CPU_LAST_HEADER_TBL_SET_EGQ_CPU_LAST_HEADER_TBL_SET_ID, 
    "cpu_last_header_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_cpu_last_header_tbl_set entry_offset_ndx 0\n\r"
    "  cpu_last_header_index 0 cpu_last_header 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_CPU_LAST_HEADER_TBL_SET_EGQ_CPU_LAST_HEADER_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_CPU_LAST_HEADER_TBL_SET_EGQ_CPU_LAST_HEADER_TBL_SET_TBL_DATA_CPU_LAST_HEADER_INDEX_ID, 
    "cpu_last_header_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  cpu_last_header_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_cpu_last_header_tbl_set entry_offset_ndx 0\n\r"
    "  cpu_last_header_index 0 cpu_last_header 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_CPU_LAST_HEADER_TBL_SET_EGQ_CPU_LAST_HEADER_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_CPU_LAST_HEADER_TBL_SET_EGQ_CPU_LAST_HEADER_TBL_SET_TBL_DATA_CPU_LAST_HEADER_ID, 
    "cpu_last_header", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.cpu_last_header[ prm_cpu_last_header_index]:\n\r"
    "  uint32 cpu_last_header[2]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_cpu_last_header_tbl_set entry_offset_ndx 0\n\r"
    "  cpu_last_header_index 0 cpu_last_header 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_CPU_LAST_HEADER_TBL_SET_EGQ_CPU_LAST_HEADER_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_CPU_LAST_HEADER_TBL_GET_EGQ_CPU_LAST_HEADER_TBL_GET_ID, 
    "cpu_last_header_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_cpu_last_header_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_CPU_LAST_HEADER_TBL_GET_EGQ_CPU_LAST_HEADER_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_IPT_LAST_HEADER_TBL_SET_EGQ_IPT_LAST_HEADER_TBL_SET_ID, 
    "ipt_last_header_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_ipt_last_header_tbl_set entry_offset_ndx 0\n\r"
    "  ipt_last_header_index 0 ipt_last_header 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_IPT_LAST_HEADER_TBL_SET_EGQ_IPT_LAST_HEADER_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_IPT_LAST_HEADER_TBL_SET_EGQ_IPT_LAST_HEADER_TBL_SET_TBL_DATA_IPT_LAST_HEADER_INDEX_ID, 
    "last_header_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  ipt_last_header_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_ipt_last_header_tbl_set entry_offset_ndx 0\n\r"
    "  ipt_last_header_index 0 ipt_last_header 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_IPT_LAST_HEADER_TBL_SET_EGQ_IPT_LAST_HEADER_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_IPT_LAST_HEADER_TBL_SET_EGQ_IPT_LAST_HEADER_TBL_SET_TBL_DATA_IPT_LAST_HEADER_ID, 
    "last_header", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.ipt_last_header[ prm_ipt_last_header_index]:\n\r"
    "  uint32 ipt_last_header[2]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_ipt_last_header_tbl_set entry_offset_ndx 0\n\r"
    "  ipt_last_header_index 0 ipt_last_header 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_IPT_LAST_HEADER_TBL_SET_EGQ_IPT_LAST_HEADER_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_IPT_LAST_HEADER_TBL_GET_EGQ_IPT_LAST_HEADER_TBL_GET_ID, 
    "ipt_last_header_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_ipt_last_header_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_IPT_LAST_HEADER_TBL_GET_EGQ_IPT_LAST_HEADER_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_FDR_LAST_HEADER_TBL_SET_EGQ_FDR_LAST_HEADER_TBL_SET_ID, 
    "fdr_last_header_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_fdr_last_header_tbl_set entry_offset_ndx 0\n\r"
    "  fdr_last_header_index 0 fdr_last_header 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_FDR_LAST_HEADER_TBL_SET_EGQ_FDR_LAST_HEADER_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_FDR_LAST_HEADER_TBL_SET_EGQ_FDR_LAST_HEADER_TBL_SET_TBL_DATA_FDR_LAST_HEADER_INDEX_ID, 
    "fdr_last_header_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  fdr_last_header_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_fdr_last_header_tbl_set entry_offset_ndx 0\n\r"
    "  fdr_last_header_index 0 fdr_last_header 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_FDR_LAST_HEADER_TBL_SET_EGQ_FDR_LAST_HEADER_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_FDR_LAST_HEADER_TBL_SET_EGQ_FDR_LAST_HEADER_TBL_SET_TBL_DATA_FDR_LAST_HEADER_ID, 
    "fdr_last_header", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.fdr_last_header[ prm_fdr_last_header_index]:\n\r"
    "  uint32 fdr_last_header[2]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_fdr_last_header_tbl_set entry_offset_ndx 0\n\r"
    "  fdr_last_header_index 0 fdr_last_header 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_FDR_LAST_HEADER_TBL_SET_EGQ_FDR_LAST_HEADER_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_FDR_LAST_HEADER_TBL_GET_EGQ_FDR_LAST_HEADER_TBL_GET_ID, 
    "fdr_last_header_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_fdr_last_header_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_FDR_LAST_HEADER_TBL_GET_EGQ_FDR_LAST_HEADER_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_CPU_PACKET_COUNTER_TBL_SET_EGQ_CPU_PACKET_COUNTER_TBL_SET_ID, 
    "cpu_packet_counter_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_cpu_packet_counter_tbl_set entry_offset_ndx 0\n\r"
    "  cpu_packet_counter_index 0 cpu_packet_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_CPU_PACKET_COUNTER_TBL_SET_EGQ_CPU_PACKET_COUNTER_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_CPU_PACKET_COUNTER_TBL_SET_EGQ_CPU_PACKET_COUNTER_TBL_SET_TBL_DATA_CPU_PACKET_COUNTER_INDEX_ID, 
    "cpu_packet_counter_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  cpu_packet_counter_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_cpu_packet_counter_tbl_set entry_offset_ndx 0\n\r"
    "  cpu_packet_counter_index 0 cpu_packet_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_CPU_PACKET_COUNTER_TBL_SET_EGQ_CPU_PACKET_COUNTER_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_CPU_PACKET_COUNTER_TBL_SET_EGQ_CPU_PACKET_COUNTER_TBL_SET_TBL_DATA_CPU_PACKET_COUNTER_ID, 
    "cpu_packet_counter", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.cpu_packet_counter[ prm_cpu_packet_counter_index]:\n\r"
    "  uint32 cpu_packet_counter[2]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_cpu_packet_counter_tbl_set entry_offset_ndx 0\n\r"
    "  cpu_packet_counter_index 0 cpu_packet_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_CPU_PACKET_COUNTER_TBL_SET_EGQ_CPU_PACKET_COUNTER_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_CPU_PACKET_COUNTER_TBL_GET_EGQ_CPU_PACKET_COUNTER_TBL_GET_ID, 
    "cpu_packet_counter_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_cpu_packet_counter_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_CPU_PACKET_COUNTER_TBL_GET_EGQ_CPU_PACKET_COUNTER_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_IPT_PACKET_COUNTER_TBL_SET_EGQ_IPT_PACKET_COUNTER_TBL_SET_ID, 
    "ipt_packet_counter_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_ipt_packet_counter_tbl_set entry_offset_ndx 0\n\r"
    "  ipt_packet_counter_index 0 ipt_packet_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_IPT_PACKET_COUNTER_TBL_SET_EGQ_IPT_PACKET_COUNTER_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_IPT_PACKET_COUNTER_TBL_SET_EGQ_IPT_PACKET_COUNTER_TBL_SET_TBL_DATA_IPT_PACKET_COUNTER_INDEX_ID, 
    "packet_counter_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  ipt_packet_counter_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_ipt_packet_counter_tbl_set entry_offset_ndx 0\n\r"
    "  ipt_packet_counter_index 0 ipt_packet_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_IPT_PACKET_COUNTER_TBL_SET_EGQ_IPT_PACKET_COUNTER_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_IPT_PACKET_COUNTER_TBL_SET_EGQ_IPT_PACKET_COUNTER_TBL_SET_TBL_DATA_IPT_PACKET_COUNTER_ID, 
    "packet_counter", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.ipt_packet_counter[ prm_ipt_packet_counter_index]:\n\r"
    "  uint32 ipt_packet_counter[2]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_ipt_packet_counter_tbl_set entry_offset_ndx 0\n\r"
    "  ipt_packet_counter_index 0 ipt_packet_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_IPT_PACKET_COUNTER_TBL_SET_EGQ_IPT_PACKET_COUNTER_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_IPT_PACKET_COUNTER_TBL_GET_EGQ_IPT_PACKET_COUNTER_TBL_GET_ID, 
    "ipt_packet_counter_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_ipt_packet_counter_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_IPT_PACKET_COUNTER_TBL_GET_EGQ_IPT_PACKET_COUNTER_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_FDR_PACKET_COUNTER_TBL_SET_EGQ_FDR_PACKET_COUNTER_TBL_SET_ID, 
    "fdr_packet_counter_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_fdr_packet_counter_tbl_set entry_offset_ndx 0\n\r"
    "  fdr_packet_counter_index 0 fdr_packet_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_FDR_PACKET_COUNTER_TBL_SET_EGQ_FDR_PACKET_COUNTER_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_FDR_PACKET_COUNTER_TBL_SET_EGQ_FDR_PACKET_COUNTER_TBL_SET_TBL_DATA_FDR_PACKET_COUNTER_INDEX_ID, 
    "fdr_packet_counter_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  fdr_packet_counter_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_fdr_packet_counter_tbl_set entry_offset_ndx 0\n\r"
    "  fdr_packet_counter_index 0 fdr_packet_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_FDR_PACKET_COUNTER_TBL_SET_EGQ_FDR_PACKET_COUNTER_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_FDR_PACKET_COUNTER_TBL_SET_EGQ_FDR_PACKET_COUNTER_TBL_SET_TBL_DATA_FDR_PACKET_COUNTER_ID, 
    "fdr_packet_counter", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.fdr_packet_counter[ prm_fdr_packet_counter_index]:\n\r"
    "  uint32 fdr_packet_counter[2]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_fdr_packet_counter_tbl_set entry_offset_ndx 0\n\r"
    "  fdr_packet_counter_index 0 fdr_packet_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_FDR_PACKET_COUNTER_TBL_SET_EGQ_FDR_PACKET_COUNTER_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_FDR_PACKET_COUNTER_TBL_GET_EGQ_FDR_PACKET_COUNTER_TBL_GET_ID, 
    "fdr_packet_counter_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_fdr_packet_counter_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_FDR_PACKET_COUNTER_TBL_GET_EGQ_FDR_PACKET_COUNTER_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_RQP_PACKET_COUNTER_TBL_SET_EGQ_RQP_PACKET_COUNTER_TBL_SET_ID, 
    "rqp_packet_counter_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_rqp_packet_counter_tbl_set entry_offset_ndx 0\n\r"
    "  rqp_packet_counter_index 0 rqp_packet_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_RQP_PACKET_COUNTER_TBL_SET_EGQ_RQP_PACKET_COUNTER_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_RQP_PACKET_COUNTER_TBL_SET_EGQ_RQP_PACKET_COUNTER_TBL_SET_TBL_DATA_RQP_PACKET_COUNTER_INDEX_ID, 
    "rqp_packet_counter_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  rqp_packet_counter_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_rqp_packet_counter_tbl_set entry_offset_ndx 0\n\r"
    "  rqp_packet_counter_index 0 rqp_packet_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_RQP_PACKET_COUNTER_TBL_SET_EGQ_RQP_PACKET_COUNTER_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_RQP_PACKET_COUNTER_TBL_SET_EGQ_RQP_PACKET_COUNTER_TBL_SET_TBL_DATA_RQP_PACKET_COUNTER_ID, 
    "rqp_packet_counter", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.rqp_packet_counter[ prm_rqp_packet_counter_index]:\n\r"
    "  uint32 rqp_packet_counter[2]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_rqp_packet_counter_tbl_set entry_offset_ndx 0\n\r"
    "  rqp_packet_counter_index 0 rqp_packet_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_RQP_PACKET_COUNTER_TBL_SET_EGQ_RQP_PACKET_COUNTER_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_RQP_PACKET_COUNTER_TBL_GET_EGQ_RQP_PACKET_COUNTER_TBL_GET_ID, 
    "rqp_packet_counter_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_rqp_packet_counter_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_RQP_PACKET_COUNTER_TBL_GET_EGQ_RQP_PACKET_COUNTER_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_RQP_DISCARD_PACKET_COUNTER_TBL_SET_EGQ_RQP_DISCARD_PACKET_COUNTER_TBL_SET_ID, 
    "rqp_discard_packet_counter_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_rqp_discard_packet_counter_tbl_set entry_offset_ndx 0\n\r"
    "  rqp_discard_packet_counter_index 0 rqp_discard_packet_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_RQP_DISCARD_PACKET_COUNTER_TBL_SET_EGQ_RQP_DISCARD_PACKET_COUNTER_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_RQP_DISCARD_PACKET_COUNTER_TBL_SET_EGQ_RQP_DISCARD_PACKET_COUNTER_TBL_SET_TBL_DATA_RQP_DISCARD_PACKET_COUNTER_INDEX_ID, 
    "rqp_discard_packet_counter_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  rqp_discard_packet_counter_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_rqp_discard_packet_counter_tbl_set entry_offset_ndx 0\n\r"
    "  rqp_discard_packet_counter_index 0 rqp_discard_packet_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_RQP_DISCARD_PACKET_COUNTER_TBL_SET_EGQ_RQP_DISCARD_PACKET_COUNTER_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_RQP_DISCARD_PACKET_COUNTER_TBL_SET_EGQ_RQP_DISCARD_PACKET_COUNTER_TBL_SET_TBL_DATA_RQP_DISCARD_PACKET_COUNTER_ID, 
    "rqp_discard_packet_counter", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.rqp_discard_packet_counter[ prm_rqp_discard_packet_counter_index]:\n\r"
    "  uint32 rqp_discard_packet_counter[2]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_rqp_discard_packet_counter_tbl_set entry_offset_ndx 0\n\r"
    "  rqp_discard_packet_counter_index 0 rqp_discard_packet_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_RQP_DISCARD_PACKET_COUNTER_TBL_SET_EGQ_RQP_DISCARD_PACKET_COUNTER_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_RQP_DISCARD_PACKET_COUNTER_TBL_GET_EGQ_RQP_DISCARD_PACKET_COUNTER_TBL_GET_ID, 
    "rqp_discard_packet_counter_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_rqp_discard_packet_counter_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_RQP_DISCARD_PACKET_COUNTER_TBL_GET_EGQ_RQP_DISCARD_PACKET_COUNTER_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_EHP_UNICAST_PACKET_COUNTER_TBL_SET_EGQ_EHP_UNICAST_PACKET_COUNTER_TBL_SET_ID, 
    "ehp_unicast_packet_counter_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_ehp_unicast_packet_counter_tbl_set entry_offset_ndx 0\n\r"
    "  ehp_unicast_packet_counter_index 0 ehp_unicast_packet_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_EHP_UNICAST_PACKET_COUNTER_TBL_SET_EGQ_EHP_UNICAST_PACKET_COUNTER_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_EHP_UNICAST_PACKET_COUNTER_TBL_SET_EGQ_EHP_UNICAST_PACKET_COUNTER_TBL_SET_TBL_DATA_EHP_UNICAST_PACKET_COUNTER_INDEX_ID, 
    "ehp_unicast_packet_counter_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  ehp_unicast_packet_counter_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_ehp_unicast_packet_counter_tbl_set entry_offset_ndx 0\n\r"
    "  ehp_unicast_packet_counter_index 0 ehp_unicast_packet_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_EHP_UNICAST_PACKET_COUNTER_TBL_SET_EGQ_EHP_UNICAST_PACKET_COUNTER_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_EHP_UNICAST_PACKET_COUNTER_TBL_SET_EGQ_EHP_UNICAST_PACKET_COUNTER_TBL_SET_TBL_DATA_EHP_UNICAST_PACKET_COUNTER_ID, 
    "ehp_unicast_packet_counter", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.ehp_unicast_packet_counter[ prm_ehp_unicast_packet_counter_index]:\n\r"
    "  uint32 ehp_unicast_packet_counter[2]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_ehp_unicast_packet_counter_tbl_set entry_offset_ndx 0\n\r"
    "  ehp_unicast_packet_counter_index 0 ehp_unicast_packet_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_EHP_UNICAST_PACKET_COUNTER_TBL_SET_EGQ_EHP_UNICAST_PACKET_COUNTER_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_EHP_UNICAST_PACKET_COUNTER_TBL_GET_EGQ_EHP_UNICAST_PACKET_COUNTER_TBL_GET_ID, 
    "ehp_unicast_packet_counter_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_ehp_unicast_packet_counter_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_EHP_UNICAST_PACKET_COUNTER_TBL_GET_EGQ_EHP_UNICAST_PACKET_COUNTER_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER_TBL_SET_EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER_TBL_SET_ID, 
    "ehp_multicast_high_packet_counter_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_ehp_multicast_high_packet_counter_tbl_set\n\r"
    "  entry_offset_ndx 0 ehp_multicast_high_packet_counter_index 0\n\r"
    "  ehp_multicast_high_packet_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER_TBL_SET_EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER_TBL_SET_EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER_TBL_SET_TBL_DATA_EHP_MULTICAST_HIGH_PACKET_COUNTER_INDEX_ID, 
    "ehp_multicast_high_packet_counter_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  ehp_multicast_high_packet_counter_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_ehp_multicast_high_packet_counter_tbl_set\n\r"
    "  entry_offset_ndx 0 ehp_multicast_high_packet_counter_index 0\n\r"
    "  ehp_multicast_high_packet_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER_TBL_SET_EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER_TBL_SET_EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER_TBL_SET_TBL_DATA_EHP_MULTICAST_HIGH_PACKET_COUNTER_ID, 
    "ehp_multicast_high_packet_counter", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.ehp_multicast_high_packet_counter[\n\r"
    "  prm_ehp_multicast_high_packet_counter_index]:\n\r"
    "  uint32 ehp_multicast_high_packet_counter[2]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_ehp_multicast_high_packet_counter_tbl_set\n\r"
    "  entry_offset_ndx 0 ehp_multicast_high_packet_counter_index 0\n\r"
    "  ehp_multicast_high_packet_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER_TBL_SET_EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER_TBL_GET_EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER_TBL_GET_ID, 
    "ehp_multicast_high_packet_counter_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_ehp_multicast_high_packet_counter_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER_TBL_GET_EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER_TBL_SET_EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER_TBL_SET_ID, 
    "ehp_multicast_low_packet_counter_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_ehp_multicast_low_packet_counter_tbl_set\n\r"
    "  entry_offset_ndx 0 ehp_multicast_low_packet_counter_index 0\n\r"
    "  ehp_multicast_low_packet_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER_TBL_SET_EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER_TBL_SET_EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER_TBL_SET_TBL_DATA_EHP_MULTICAST_LOW_PACKET_COUNTER_INDEX_ID, 
    "ehp_multicast_low_packet_counter_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  ehp_multicast_low_packet_counter_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_ehp_multicast_low_packet_counter_tbl_set\n\r"
    "  entry_offset_ndx 0 ehp_multicast_low_packet_counter_index 0\n\r"
    "  ehp_multicast_low_packet_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER_TBL_SET_EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER_TBL_SET_EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER_TBL_SET_TBL_DATA_EHP_MULTICAST_LOW_PACKET_COUNTER_ID, 
    "ehp_multicast_low_packet_counter", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.ehp_multicast_low_packet_counter[\n\r"
    "  prm_ehp_multicast_low_packet_counter_index]:\n\r"
    "  uint32 ehp_multicast_low_packet_counter[2]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_ehp_multicast_low_packet_counter_tbl_set\n\r"
    "  entry_offset_ndx 0 ehp_multicast_low_packet_counter_index 0\n\r"
    "  ehp_multicast_low_packet_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER_TBL_SET_EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER_TBL_GET_EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER_TBL_GET_ID, 
    "ehp_multicast_low_packet_counter_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_ehp_multicast_low_packet_counter_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER_TBL_GET_EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_EHP_DISCARD_PACKET_COUNTER_TBL_SET_EGQ_EHP_DISCARD_PACKET_COUNTER_TBL_SET_ID, 
    "ehp_discard_packet_counter_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_ehp_discard_packet_counter_tbl_set entry_offset_ndx 0\n\r"
    "  ehp_discard_packet_counter_index 0 ehp_discard_packet_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_EHP_DISCARD_PACKET_COUNTER_TBL_SET_EGQ_EHP_DISCARD_PACKET_COUNTER_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_EHP_DISCARD_PACKET_COUNTER_TBL_SET_EGQ_EHP_DISCARD_PACKET_COUNTER_TBL_SET_TBL_DATA_EHP_DISCARD_PACKET_COUNTER_INDEX_ID, 
    "ehp_discard_packet_counter_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  ehp_discard_packet_counter_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_ehp_discard_packet_counter_tbl_set entry_offset_ndx 0\n\r"
    "  ehp_discard_packet_counter_index 0 ehp_discard_packet_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_EHP_DISCARD_PACKET_COUNTER_TBL_SET_EGQ_EHP_DISCARD_PACKET_COUNTER_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_EHP_DISCARD_PACKET_COUNTER_TBL_SET_EGQ_EHP_DISCARD_PACKET_COUNTER_TBL_SET_TBL_DATA_EHP_DISCARD_PACKET_COUNTER_ID, 
    "ehp_discard_packet_counter", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.ehp_discard_packet_counter[ prm_ehp_discard_packet_counter_index]:\n\r"
    "  uint32 ehp_discard_packet_counter[2]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_ehp_discard_packet_counter_tbl_set entry_offset_ndx 0\n\r"
    "  ehp_discard_packet_counter_index 0 ehp_discard_packet_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_EHP_DISCARD_PACKET_COUNTER_TBL_SET_EGQ_EHP_DISCARD_PACKET_COUNTER_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_EHP_DISCARD_PACKET_COUNTER_TBL_GET_EGQ_EHP_DISCARD_PACKET_COUNTER_TBL_GET_ID, 
    "ehp_discard_packet_counter_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_ehp_discard_packet_counter_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_EHP_DISCARD_PACKET_COUNTER_TBL_GET_EGQ_EHP_DISCARD_PACKET_COUNTER_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PQP_UNICAST_HIGH_PACKET_COUNTER_TBL_SET_EGQ_PQP_UNICAST_HIGH_PACKET_COUNTER_TBL_SET_ID, 
    "pqp_unicast_high_packet_counter_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_pqp_unicast_high_packet_counter_tbl_set\n\r"
    "  entry_offset_ndx 0 pqp_unicast_high_packet_counter_index 0\n\r"
    "  pqp_unicast_high_packet_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PQP_UNICAST_HIGH_PACKET_COUNTER_TBL_SET_EGQ_PQP_UNICAST_HIGH_PACKET_COUNTER_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PQP_UNICAST_HIGH_PACKET_COUNTER_TBL_SET_EGQ_PQP_UNICAST_HIGH_PACKET_COUNTER_TBL_SET_TBL_DATA_PQP_UNICAST_HIGH_PACKET_COUNTER_INDEX_ID, 
    "pqp_unicast_high_packet_counter_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  pqp_unicast_high_packet_counter_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_pqp_unicast_high_packet_counter_tbl_set\n\r"
    "  entry_offset_ndx 0 pqp_unicast_high_packet_counter_index 0\n\r"
    "  pqp_unicast_high_packet_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PQP_UNICAST_HIGH_PACKET_COUNTER_TBL_SET_EGQ_PQP_UNICAST_HIGH_PACKET_COUNTER_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PQP_UNICAST_HIGH_PACKET_COUNTER_TBL_SET_EGQ_PQP_UNICAST_HIGH_PACKET_COUNTER_TBL_SET_TBL_DATA_PQP_UNICAST_HIGH_PACKET_COUNTER_ID, 
    "pqp_unicast_high_packet_counter", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.pqp_unicast_high_packet_counter[\n\r"
    "  prm_pqp_unicast_high_packet_counter_index]:\n\r"
    "  uint32 pqp_unicast_high_packet_counter[2]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_pqp_unicast_high_packet_counter_tbl_set\n\r"
    "  entry_offset_ndx 0 pqp_unicast_high_packet_counter_index 0\n\r"
    "  pqp_unicast_high_packet_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PQP_UNICAST_HIGH_PACKET_COUNTER_TBL_SET_EGQ_PQP_UNICAST_HIGH_PACKET_COUNTER_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PQP_UNICAST_HIGH_PACKET_COUNTER_TBL_GET_EGQ_PQP_UNICAST_HIGH_PACKET_COUNTER_TBL_GET_ID, 
    "pqp_unicast_high_packet_counter_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_pqp_unicast_high_packet_counter_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PQP_UNICAST_HIGH_PACKET_COUNTER_TBL_GET_EGQ_PQP_UNICAST_HIGH_PACKET_COUNTER_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PQP_UNICAST_LOW_PACKET_COUNTER_TBL_SET_EGQ_PQP_UNICAST_LOW_PACKET_COUNTER_TBL_SET_ID, 
    "pqp_unicast_low_packet_counter_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_pqp_unicast_low_packet_counter_tbl_set\n\r"
    "  entry_offset_ndx 0 pqp_unicast_low_packet_counter_index 0\n\r"
    "  pqp_unicast_low_packet_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PQP_UNICAST_LOW_PACKET_COUNTER_TBL_SET_EGQ_PQP_UNICAST_LOW_PACKET_COUNTER_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PQP_UNICAST_LOW_PACKET_COUNTER_TBL_SET_EGQ_PQP_UNICAST_LOW_PACKET_COUNTER_TBL_SET_TBL_DATA_PQP_UNICAST_LOW_PACKET_COUNTER_INDEX_ID, 
    "pqp_unicast_low_packet_counter_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  pqp_unicast_low_packet_counter_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_pqp_unicast_low_packet_counter_tbl_set\n\r"
    "  entry_offset_ndx 0 pqp_unicast_low_packet_counter_index 0\n\r"
    "  pqp_unicast_low_packet_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PQP_UNICAST_LOW_PACKET_COUNTER_TBL_SET_EGQ_PQP_UNICAST_LOW_PACKET_COUNTER_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PQP_UNICAST_LOW_PACKET_COUNTER_TBL_SET_EGQ_PQP_UNICAST_LOW_PACKET_COUNTER_TBL_SET_TBL_DATA_PQP_UNICAST_LOW_PACKET_COUNTER_ID, 
    "pqp_unicast_low_packet_counter", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.pqp_unicast_low_packet_counter[\n\r"
    "  prm_pqp_unicast_low_packet_counter_index]:\n\r"
    "  uint32 pqp_unicast_low_packet_counter[2]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_pqp_unicast_low_packet_counter_tbl_set\n\r"
    "  entry_offset_ndx 0 pqp_unicast_low_packet_counter_index 0\n\r"
    "  pqp_unicast_low_packet_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PQP_UNICAST_LOW_PACKET_COUNTER_TBL_SET_EGQ_PQP_UNICAST_LOW_PACKET_COUNTER_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PQP_UNICAST_LOW_PACKET_COUNTER_TBL_GET_EGQ_PQP_UNICAST_LOW_PACKET_COUNTER_TBL_GET_ID, 
    "pqp_unicast_low_packet_counter_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_pqp_unicast_low_packet_counter_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PQP_UNICAST_LOW_PACKET_COUNTER_TBL_GET_EGQ_PQP_UNICAST_LOW_PACKET_COUNTER_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PQP_MULTICAST_HIGH_PACKET_COUNTER_TBL_SET_EGQ_PQP_MULTICAST_HIGH_PACKET_COUNTER_TBL_SET_ID, 
    "pqp_multicast_high_packet_counter_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_pqp_multicast_high_packet_counter_tbl_set\n\r"
    "  entry_offset_ndx 0 pqp_multicast_high_packet_counter_index 0\n\r"
    "  pqp_multicast_high_packet_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PQP_MULTICAST_HIGH_PACKET_COUNTER_TBL_SET_EGQ_PQP_MULTICAST_HIGH_PACKET_COUNTER_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PQP_MULTICAST_HIGH_PACKET_COUNTER_TBL_SET_EGQ_PQP_MULTICAST_HIGH_PACKET_COUNTER_TBL_SET_TBL_DATA_PQP_MULTICAST_HIGH_PACKET_COUNTER_INDEX_ID, 
    "pqp_multicast_high_packet_counter_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  pqp_multicast_high_packet_counter_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_pqp_multicast_high_packet_counter_tbl_set\n\r"
    "  entry_offset_ndx 0 pqp_multicast_high_packet_counter_index 0\n\r"
    "  pqp_multicast_high_packet_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PQP_MULTICAST_HIGH_PACKET_COUNTER_TBL_SET_EGQ_PQP_MULTICAST_HIGH_PACKET_COUNTER_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PQP_MULTICAST_HIGH_PACKET_COUNTER_TBL_SET_EGQ_PQP_MULTICAST_HIGH_PACKET_COUNTER_TBL_SET_TBL_DATA_PQP_MULTICAST_HIGH_PACKET_COUNTER_ID, 
    "pqp_multicast_high_packet_counter", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.pqp_multicast_high_packet_counter[\n\r"
    "  prm_pqp_multicast_high_packet_counter_index]:\n\r"
    "  uint32 pqp_multicast_high_packet_counter[2]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_pqp_multicast_high_packet_counter_tbl_set\n\r"
    "  entry_offset_ndx 0 pqp_multicast_high_packet_counter_index 0\n\r"
    "  pqp_multicast_high_packet_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PQP_MULTICAST_HIGH_PACKET_COUNTER_TBL_SET_EGQ_PQP_MULTICAST_HIGH_PACKET_COUNTER_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PQP_MULTICAST_HIGH_PACKET_COUNTER_TBL_GET_EGQ_PQP_MULTICAST_HIGH_PACKET_COUNTER_TBL_GET_ID, 
    "pqp_multicast_high_packet_counter_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_pqp_multicast_high_packet_counter_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PQP_MULTICAST_HIGH_PACKET_COUNTER_TBL_GET_EGQ_PQP_MULTICAST_HIGH_PACKET_COUNTER_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PQP_MULTICAST_LOW_PACKET_COUNTER_TBL_SET_EGQ_PQP_MULTICAST_LOW_PACKET_COUNTER_TBL_SET_ID, 
    "pqp_multicast_low_packet_counter_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_pqp_multicast_low_packet_counter_tbl_set\n\r"
    "  entry_offset_ndx 0 pqp_multicast_low_packet_counter_index 0\n\r"
    "  pqp_multicast_low_packet_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PQP_MULTICAST_LOW_PACKET_COUNTER_TBL_SET_EGQ_PQP_MULTICAST_LOW_PACKET_COUNTER_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PQP_MULTICAST_LOW_PACKET_COUNTER_TBL_SET_EGQ_PQP_MULTICAST_LOW_PACKET_COUNTER_TBL_SET_TBL_DATA_PQP_MULTICAST_LOW_PACKET_COUNTER_INDEX_ID, 
    "pqp_multicast_low_packet_counter_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  pqp_multicast_low_packet_counter_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_pqp_multicast_low_packet_counter_tbl_set\n\r"
    "  entry_offset_ndx 0 pqp_multicast_low_packet_counter_index 0\n\r"
    "  pqp_multicast_low_packet_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PQP_MULTICAST_LOW_PACKET_COUNTER_TBL_SET_EGQ_PQP_MULTICAST_LOW_PACKET_COUNTER_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PQP_MULTICAST_LOW_PACKET_COUNTER_TBL_SET_EGQ_PQP_MULTICAST_LOW_PACKET_COUNTER_TBL_SET_TBL_DATA_PQP_MULTICAST_LOW_PACKET_COUNTER_ID, 
    "pqp_multicast_low_packet_counter", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.pqp_multicast_low_packet_counter[\n\r"
    "  prm_pqp_multicast_low_packet_counter_index]:\n\r"
    "  uint32 pqp_multicast_low_packet_counter[2]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_pqp_multicast_low_packet_counter_tbl_set\n\r"
    "  entry_offset_ndx 0 pqp_multicast_low_packet_counter_index 0\n\r"
    "  pqp_multicast_low_packet_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PQP_MULTICAST_LOW_PACKET_COUNTER_TBL_SET_EGQ_PQP_MULTICAST_LOW_PACKET_COUNTER_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PQP_MULTICAST_LOW_PACKET_COUNTER_TBL_GET_EGQ_PQP_MULTICAST_LOW_PACKET_COUNTER_TBL_GET_ID, 
    "pqp_multicast_low_packet_counter_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_pqp_multicast_low_packet_counter_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PQP_MULTICAST_LOW_PACKET_COUNTER_TBL_GET_EGQ_PQP_MULTICAST_LOW_PACKET_COUNTER_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PQP_UNICAST_HIGH_BYTES_COUNTER_TBL_SET_EGQ_PQP_UNICAST_HIGH_BYTES_COUNTER_TBL_SET_ID, 
    "pqp_unicast_high_bytes_counter_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_pqp_unicast_high_bytes_counter_tbl_set\n\r"
    "  entry_offset_ndx 0 pqp_unicast_high_bytes_counter_index 0\n\r"
    "  pqp_unicast_high_bytes_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PQP_UNICAST_HIGH_BYTES_COUNTER_TBL_SET_EGQ_PQP_UNICAST_HIGH_BYTES_COUNTER_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PQP_UNICAST_HIGH_BYTES_COUNTER_TBL_SET_EGQ_PQP_UNICAST_HIGH_BYTES_COUNTER_TBL_SET_TBL_DATA_PQP_UNICAST_HIGH_BYTES_COUNTER_INDEX_ID, 
    "pqp_unicast_high_bytes_counter_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  pqp_unicast_high_bytes_counter_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_pqp_unicast_high_bytes_counter_tbl_set\n\r"
    "  entry_offset_ndx 0 pqp_unicast_high_bytes_counter_index 0\n\r"
    "  pqp_unicast_high_bytes_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PQP_UNICAST_HIGH_BYTES_COUNTER_TBL_SET_EGQ_PQP_UNICAST_HIGH_BYTES_COUNTER_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PQP_UNICAST_HIGH_BYTES_COUNTER_TBL_SET_EGQ_PQP_UNICAST_HIGH_BYTES_COUNTER_TBL_SET_TBL_DATA_PQP_UNICAST_HIGH_BYTES_COUNTER_ID, 
    "pqp_unicast_high_bytes_counter", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.pqp_unicast_high_bytes_counter[\n\r"
    "  prm_pqp_unicast_high_bytes_counter_index]:\n\r"
    "  uint32 pqp_unicast_high_bytes_counter[2]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_pqp_unicast_high_bytes_counter_tbl_set\n\r"
    "  entry_offset_ndx 0 pqp_unicast_high_bytes_counter_index 0\n\r"
    "  pqp_unicast_high_bytes_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PQP_UNICAST_HIGH_BYTES_COUNTER_TBL_SET_EGQ_PQP_UNICAST_HIGH_BYTES_COUNTER_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PQP_UNICAST_HIGH_BYTES_COUNTER_TBL_GET_EGQ_PQP_UNICAST_HIGH_BYTES_COUNTER_TBL_GET_ID, 
    "pqp_unicast_high_bytes_counter_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_pqp_unicast_high_bytes_counter_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PQP_UNICAST_HIGH_BYTES_COUNTER_TBL_GET_EGQ_PQP_UNICAST_HIGH_BYTES_COUNTER_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PQP_UNICAST_LOW_BYTES_COUNTER_TBL_SET_EGQ_PQP_UNICAST_LOW_BYTES_COUNTER_TBL_SET_ID, 
    "pqp_unicast_low_bytes_counter_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_pqp_unicast_low_bytes_counter_tbl_set\n\r"
    "  entry_offset_ndx 0 pqp_unicast_low_bytes_counter_index 0\n\r"
    "  pqp_unicast_low_bytes_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PQP_UNICAST_LOW_BYTES_COUNTER_TBL_SET_EGQ_PQP_UNICAST_LOW_BYTES_COUNTER_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PQP_UNICAST_LOW_BYTES_COUNTER_TBL_SET_EGQ_PQP_UNICAST_LOW_BYTES_COUNTER_TBL_SET_TBL_DATA_PQP_UNICAST_LOW_BYTES_COUNTER_INDEX_ID, 
    "pqp_unicast_low_bytes_counter_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  pqp_unicast_low_bytes_counter_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_pqp_unicast_low_bytes_counter_tbl_set\n\r"
    "  entry_offset_ndx 0 pqp_unicast_low_bytes_counter_index 0\n\r"
    "  pqp_unicast_low_bytes_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PQP_UNICAST_LOW_BYTES_COUNTER_TBL_SET_EGQ_PQP_UNICAST_LOW_BYTES_COUNTER_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PQP_UNICAST_LOW_BYTES_COUNTER_TBL_SET_EGQ_PQP_UNICAST_LOW_BYTES_COUNTER_TBL_SET_TBL_DATA_PQP_UNICAST_LOW_BYTES_COUNTER_ID, 
    "pqp_unicast_low_bytes_counter", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.pqp_unicast_low_bytes_counter[\n\r"
    "  prm_pqp_unicast_low_bytes_counter_index]:\n\r"
    "  uint32 pqp_unicast_low_bytes_counter[2]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_pqp_unicast_low_bytes_counter_tbl_set\n\r"
    "  entry_offset_ndx 0 pqp_unicast_low_bytes_counter_index 0\n\r"
    "  pqp_unicast_low_bytes_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PQP_UNICAST_LOW_BYTES_COUNTER_TBL_SET_EGQ_PQP_UNICAST_LOW_BYTES_COUNTER_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PQP_UNICAST_LOW_BYTES_COUNTER_TBL_GET_EGQ_PQP_UNICAST_LOW_BYTES_COUNTER_TBL_GET_ID, 
    "pqp_unicast_low_bytes_counter_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_pqp_unicast_low_bytes_counter_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PQP_UNICAST_LOW_BYTES_COUNTER_TBL_GET_EGQ_PQP_UNICAST_LOW_BYTES_COUNTER_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PQP_MULTICAST_HIGH_BYTES_COUNTER_TBL_SET_EGQ_PQP_MULTICAST_HIGH_BYTES_COUNTER_TBL_SET_ID, 
    "pqp_multicast_high_bytes_counter_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_pqp_multicast_high_bytes_counter_tbl_set\n\r"
    "  entry_offset_ndx 0 pqp_multicast_high_bytes_counter_index 0\n\r"
    "  pqp_multicast_high_bytes_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PQP_MULTICAST_HIGH_BYTES_COUNTER_TBL_SET_EGQ_PQP_MULTICAST_HIGH_BYTES_COUNTER_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PQP_MULTICAST_HIGH_BYTES_COUNTER_TBL_SET_EGQ_PQP_MULTICAST_HIGH_BYTES_COUNTER_TBL_SET_TBL_DATA_PQP_MULTICAST_HIGH_BYTES_COUNTER_INDEX_ID, 
    "pqp_multicast_high_bytes_counter_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  pqp_multicast_high_bytes_counter_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_pqp_multicast_high_bytes_counter_tbl_set\n\r"
    "  entry_offset_ndx 0 pqp_multicast_high_bytes_counter_index 0\n\r"
    "  pqp_multicast_high_bytes_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PQP_MULTICAST_HIGH_BYTES_COUNTER_TBL_SET_EGQ_PQP_MULTICAST_HIGH_BYTES_COUNTER_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PQP_MULTICAST_HIGH_BYTES_COUNTER_TBL_SET_EGQ_PQP_MULTICAST_HIGH_BYTES_COUNTER_TBL_SET_TBL_DATA_PQP_MULTICAST_HIGH_BYTES_COUNTER_ID, 
    "pqp_multicast_high_bytes_counter", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.pqp_multicast_high_bytes_counter[\n\r"
    "  prm_pqp_multicast_high_bytes_counter_index]:\n\r"
    "  uint32 pqp_multicast_high_bytes_counter[2]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_pqp_multicast_high_bytes_counter_tbl_set\n\r"
    "  entry_offset_ndx 0 pqp_multicast_high_bytes_counter_index 0\n\r"
    "  pqp_multicast_high_bytes_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PQP_MULTICAST_HIGH_BYTES_COUNTER_TBL_SET_EGQ_PQP_MULTICAST_HIGH_BYTES_COUNTER_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PQP_MULTICAST_HIGH_BYTES_COUNTER_TBL_GET_EGQ_PQP_MULTICAST_HIGH_BYTES_COUNTER_TBL_GET_ID, 
    "pqp_multicast_high_bytes_counter_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_pqp_multicast_high_bytes_counter_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PQP_MULTICAST_HIGH_BYTES_COUNTER_TBL_GET_EGQ_PQP_MULTICAST_HIGH_BYTES_COUNTER_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PQP_MULTICAST_LOW_BYTES_COUNTER_TBL_SET_EGQ_PQP_MULTICAST_LOW_BYTES_COUNTER_TBL_SET_ID, 
    "pqp_multicast_low_bytes_counter_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_pqp_multicast_low_bytes_counter_tbl_set\n\r"
    "  entry_offset_ndx 0 pqp_multicast_low_bytes_counter_index 0\n\r"
    "  pqp_multicast_low_bytes_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PQP_MULTICAST_LOW_BYTES_COUNTER_TBL_SET_EGQ_PQP_MULTICAST_LOW_BYTES_COUNTER_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PQP_MULTICAST_LOW_BYTES_COUNTER_TBL_SET_EGQ_PQP_MULTICAST_LOW_BYTES_COUNTER_TBL_SET_TBL_DATA_PQP_MULTICAST_LOW_BYTES_COUNTER_INDEX_ID, 
    "pqp_multicast_low_bytes_counter_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  pqp_multicast_low_bytes_counter_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_pqp_multicast_low_bytes_counter_tbl_set\n\r"
    "  entry_offset_ndx 0 pqp_multicast_low_bytes_counter_index 0\n\r"
    "  pqp_multicast_low_bytes_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PQP_MULTICAST_LOW_BYTES_COUNTER_TBL_SET_EGQ_PQP_MULTICAST_LOW_BYTES_COUNTER_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PQP_MULTICAST_LOW_BYTES_COUNTER_TBL_SET_EGQ_PQP_MULTICAST_LOW_BYTES_COUNTER_TBL_SET_TBL_DATA_PQP_MULTICAST_LOW_BYTES_COUNTER_ID, 
    "pqp_multicast_low_bytes_counter", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.pqp_multicast_low_bytes_counter[\n\r"
    "  prm_pqp_multicast_low_bytes_counter_index]:\n\r"
    "  uint32 pqp_multicast_low_bytes_counter[2]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_pqp_multicast_low_bytes_counter_tbl_set\n\r"
    "  entry_offset_ndx 0 pqp_multicast_low_bytes_counter_index 0\n\r"
    "  pqp_multicast_low_bytes_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PQP_MULTICAST_LOW_BYTES_COUNTER_TBL_SET_EGQ_PQP_MULTICAST_LOW_BYTES_COUNTER_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PQP_MULTICAST_LOW_BYTES_COUNTER_TBL_GET_EGQ_PQP_MULTICAST_LOW_BYTES_COUNTER_TBL_GET_ID, 
    "pqp_multicast_low_bytes_counter_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_pqp_multicast_low_bytes_counter_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PQP_MULTICAST_LOW_BYTES_COUNTER_TBL_GET_EGQ_PQP_MULTICAST_LOW_BYTES_COUNTER_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER_TBL_SET_EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER_TBL_SET_ID, 
    "pqp_discard_unicast_packet_counter_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_pqp_discard_unicast_packet_counter_tbl_set\n\r"
    "  entry_offset_ndx 0 pqp_discard_unicast_packet_counter_index 0\n\r"
    "  pqp_discard_unicast_packet_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER_TBL_SET_EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER_TBL_SET_EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER_TBL_SET_TBL_DATA_PQP_DISCARD_UNICAST_PACKET_COUNTER_INDEX_ID, 
    "pqp_discard_unicast_packet_counter_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  pqp_discard_unicast_packet_counter_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_pqp_discard_unicast_packet_counter_tbl_set\n\r"
    "  entry_offset_ndx 0 pqp_discard_unicast_packet_counter_index 0\n\r"
    "  pqp_discard_unicast_packet_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER_TBL_SET_EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER_TBL_SET_EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER_TBL_SET_TBL_DATA_PQP_DISCARD_UNICAST_PACKET_COUNTER_ID, 
    "pqp_discard_unicast_packet_counter", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.pqp_discard_unicast_packet_counter[\n\r"
    "  prm_pqp_discard_unicast_packet_counter_index]:\n\r"
    "  uint32 pqp_discard_unicast_packet_counter[2]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_pqp_discard_unicast_packet_counter_tbl_set\n\r"
    "  entry_offset_ndx 0 pqp_discard_unicast_packet_counter_index 0\n\r"
    "  pqp_discard_unicast_packet_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER_TBL_SET_EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER_TBL_GET_EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER_TBL_GET_ID, 
    "pqp_discard_unicast_packet_counter_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_pqp_discard_unicast_packet_counter_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER_TBL_GET_EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER_TBL_SET_EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER_TBL_SET_ID, 
    "pqp_discard_multicast_packet_counter_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_pqp_discard_multicast_packet_counter_tbl_set\n\r"
    "  entry_offset_ndx 0 pqp_discard_multicast_packet_counter_index 0\n\r"
    "  pqp_discard_multicast_packet_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER_TBL_SET_EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER_TBL_SET_EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER_TBL_SET_TBL_DATA_PQP_DISCARD_MULTICAST_PACKET_COUNTER_INDEX_ID, 
    "pqp_discard_multicast_packet_counter_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  pqp_discard_multicast_packet_counter_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_pqp_discard_multicast_packet_counter_tbl_set\n\r"
    "  entry_offset_ndx 0 pqp_discard_multicast_packet_counter_index 0\n\r"
    "  pqp_discard_multicast_packet_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER_TBL_SET_EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER_TBL_SET_EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER_TBL_SET_TBL_DATA_PQP_DISCARD_MULTICAST_PACKET_COUNTER_ID, 
    "pqp_discard_multicast_packet_counter", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.pqp_discard_multicast_packet_counter[\n\r"
    "  prm_pqp_discard_multicast_packet_counter_index]:\n\r"
    "  uint32 pqp_discard_multicast_packet_counter[2]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_pqp_discard_multicast_packet_counter_tbl_set\n\r"
    "  entry_offset_ndx 0 pqp_discard_multicast_packet_counter_index 0\n\r"
    "  pqp_discard_multicast_packet_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER_TBL_SET_EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER_TBL_GET_EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER_TBL_GET_ID, 
    "pqp_discard_multicast_packet_counter_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_pqp_discard_multicast_packet_counter_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER_TBL_GET_EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_FQP_PACKET_COUNTER_TBL_SET_EGQ_FQP_PACKET_COUNTER_TBL_SET_ID, 
    "fqp_packet_counter_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_fqp_packet_counter_tbl_set entry_offset_ndx 0\n\r"
    "  fqp_packet_counter_index 0 fqp_packet_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_FQP_PACKET_COUNTER_TBL_SET_EGQ_FQP_PACKET_COUNTER_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_FQP_PACKET_COUNTER_TBL_SET_EGQ_FQP_PACKET_COUNTER_TBL_SET_TBL_DATA_FQP_PACKET_COUNTER_INDEX_ID, 
    "fqp_packet_counter_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  fqp_packet_counter_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_fqp_packet_counter_tbl_set entry_offset_ndx 0\n\r"
    "  fqp_packet_counter_index 0 fqp_packet_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_FQP_PACKET_COUNTER_TBL_SET_EGQ_FQP_PACKET_COUNTER_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_FQP_PACKET_COUNTER_TBL_SET_EGQ_FQP_PACKET_COUNTER_TBL_SET_TBL_DATA_FQP_PACKET_COUNTER_ID, 
    "fqp_packet_counter", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.fqp_packet_counter[ prm_fqp_packet_counter_index]:\n\r"
    "  uint32 fqp_packet_counter[2]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_fqp_packet_counter_tbl_set entry_offset_ndx 0\n\r"
    "  fqp_packet_counter_index 0 fqp_packet_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_FQP_PACKET_COUNTER_TBL_SET_EGQ_FQP_PACKET_COUNTER_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_FQP_PACKET_COUNTER_TBL_GET_EGQ_FQP_PACKET_COUNTER_TBL_GET_ID, 
    "fqp_packet_counter_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_fqp_packet_counter_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_FQP_PACKET_COUNTER_TBL_GET_EGQ_FQP_PACKET_COUNTER_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_FQP_NIF_PORT_MUX_TBL_SET_EGQ_FQP_NIF_PORT_MUX_TBL_SET_ID, 
    "fqp_nif_port_mux_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_fqp_nif_port_mux_tbl_set entry_offset_ndx 0\n\r"
    "  fqp_nif_port_mux 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_FQP_NIF_PORT_MUX_TBL_SET_EGQ_FQP_NIF_PORT_MUX_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_FQP_NIF_PORT_MUX_TBL_SET_EGQ_FQP_NIF_PORT_MUX_TBL_SET_TBL_DATA_FQP_NIF_PORT_MUX_ID, 
    "fqp_nif_port_mux", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.fqp_nif_port_mux:\n\r"
    "  uint32 fqp_nif_port_mux\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_fqp_nif_port_mux_tbl_set entry_offset_ndx 0\n\r"
    "  fqp_nif_port_mux 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_FQP_NIF_PORT_MUX_TBL_SET_EGQ_FQP_NIF_PORT_MUX_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_FQP_NIF_PORT_MUX_TBL_GET_EGQ_FQP_NIF_PORT_MUX_TBL_GET_ID, 
    "fqp_nif_port_mux_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_fqp_nif_port_mux_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_FQP_NIF_PORT_MUX_TBL_GET_EGQ_FQP_NIF_PORT_MUX_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_KEY_PROFILE_MAP_INDEX_TBL_SET_EGQ_KEY_PROFILE_MAP_INDEX_TBL_SET_ID, 
    "key_profile_map_index_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_key_profile_map_index_tbl_set entry_offset_ndx 0\n\r"
    "  key_profile_map_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_KEY_PROFILE_MAP_INDEX_TBL_SET_EGQ_KEY_PROFILE_MAP_INDEX_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_KEY_PROFILE_MAP_INDEX_TBL_SET_EGQ_KEY_PROFILE_MAP_INDEX_TBL_SET_TBL_DATA_KEY_PROFILE_MAP_INDEX_ID, 
    "key_profile_map_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.key_profile_map_index:\n\r"
    "  uint32 key_profile_map_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_key_profile_map_index_tbl_set entry_offset_ndx 0\n\r"
    "  key_profile_map_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_KEY_PROFILE_MAP_INDEX_TBL_SET_EGQ_KEY_PROFILE_MAP_INDEX_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_KEY_PROFILE_MAP_INDEX_TBL_GET_EGQ_KEY_PROFILE_MAP_INDEX_TBL_GET_ID, 
    "key_profile_map_index_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_key_profile_map_index_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_KEY_PROFILE_MAP_INDEX_TBL_GET_EGQ_KEY_PROFILE_MAP_INDEX_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_TCAM_KEY_RESOLUTION_PROFILE_TBL_SET_EGQ_TCAM_KEY_RESOLUTION_PROFILE_TBL_SET_ID, 
    "tcam_key_resolution_profile_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_tcam_key_resolution_profile_tbl_set entry_offset_ndx\n\r"
    "  0 tcam_profile 0 key_or_value 0 key_and_value 0 key_select 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_TCAM_KEY_RESOLUTION_PROFILE_TBL_SET_EGQ_TCAM_KEY_RESOLUTION_PROFILE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_TCAM_KEY_RESOLUTION_PROFILE_TBL_SET_EGQ_TCAM_KEY_RESOLUTION_PROFILE_TBL_SET_TBL_DATA_TCAM_PROFILE_ID, 
    "tcam_profile", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.tcam_profile:\n\r"
    "  uint32 tcam_profile\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_tcam_key_resolution_profile_tbl_set entry_offset_ndx\n\r"
    "  0 tcam_profile 0 key_or_value 0 key_and_value 0 key_select 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_TCAM_KEY_RESOLUTION_PROFILE_TBL_SET_EGQ_TCAM_KEY_RESOLUTION_PROFILE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_TCAM_KEY_RESOLUTION_PROFILE_TBL_SET_EGQ_TCAM_KEY_RESOLUTION_PROFILE_TBL_SET_TBL_DATA_KEY_OR_VALUE_ID, 
    "key_or_value", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.key_or_value:\n\r"
    "  uint32 key_or_value\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_tcam_key_resolution_profile_tbl_set entry_offset_ndx\n\r"
    "  0 tcam_profile 0 key_or_value 0 key_and_value 0 key_select 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_TCAM_KEY_RESOLUTION_PROFILE_TBL_SET_EGQ_TCAM_KEY_RESOLUTION_PROFILE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_TCAM_KEY_RESOLUTION_PROFILE_TBL_SET_EGQ_TCAM_KEY_RESOLUTION_PROFILE_TBL_SET_TBL_DATA_KEY_AND_VALUE_ID, 
    "key_and_value", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.key_and_value:\n\r"
    "  uint32 key_and_value\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_tcam_key_resolution_profile_tbl_set entry_offset_ndx\n\r"
    "  0 tcam_profile 0 key_or_value 0 key_and_value 0 key_select 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_TCAM_KEY_RESOLUTION_PROFILE_TBL_SET_EGQ_TCAM_KEY_RESOLUTION_PROFILE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_TCAM_KEY_RESOLUTION_PROFILE_TBL_SET_EGQ_TCAM_KEY_RESOLUTION_PROFILE_TBL_SET_TBL_DATA_KEY_SELECT_ID, 
    "key_select", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.key_select:\n\r"
    "  uint32 key_select\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_tcam_key_resolution_profile_tbl_set entry_offset_ndx\n\r"
    "  0 tcam_profile 0 key_or_value 0 key_and_value 0 key_select 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_TCAM_KEY_RESOLUTION_PROFILE_TBL_SET_EGQ_TCAM_KEY_RESOLUTION_PROFILE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_TCAM_KEY_RESOLUTION_PROFILE_TBL_GET_EGQ_TCAM_KEY_RESOLUTION_PROFILE_TBL_GET_ID, 
    "tcam_key_resolution_profile_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_tcam_key_resolution_profile_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_TCAM_KEY_RESOLUTION_PROFILE_TBL_GET_EGQ_TCAM_KEY_RESOLUTION_PROFILE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PARSER_LAST_SYS_RECORD_TBL_SET_EGQ_PARSER_LAST_SYS_RECORD_TBL_SET_ID, 
    "parser_last_sys_record_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_parser_last_sys_record_tbl_set entry_offset_ndx 0\n\r"
    "  parser_last_sys_record_index 0 parser_last_sys_record 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PARSER_LAST_SYS_RECORD_TBL_SET_EGQ_PARSER_LAST_SYS_RECORD_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PARSER_LAST_SYS_RECORD_TBL_SET_EGQ_PARSER_LAST_SYS_RECORD_TBL_SET_TBL_DATA_PARSER_LAST_SYS_RECORD_INDEX_ID, 
    "parser_last_sys_record_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  parser_last_sys_record_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_parser_last_sys_record_tbl_set entry_offset_ndx 0\n\r"
    "  parser_last_sys_record_index 0 parser_last_sys_record 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PARSER_LAST_SYS_RECORD_TBL_SET_EGQ_PARSER_LAST_SYS_RECORD_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PARSER_LAST_SYS_RECORD_TBL_SET_EGQ_PARSER_LAST_SYS_RECORD_TBL_SET_TBL_DATA_PARSER_LAST_SYS_RECORD_ID, 
    "parser_last_sys_record", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.parser_last_sys_record[ prm_parser_last_sys_record_index]:\n\r"
    "  uint32 parser_last_sys_record[8]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_parser_last_sys_record_tbl_set entry_offset_ndx 0\n\r"
    "  parser_last_sys_record_index 0 parser_last_sys_record 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PARSER_LAST_SYS_RECORD_TBL_SET_EGQ_PARSER_LAST_SYS_RECORD_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PARSER_LAST_SYS_RECORD_TBL_GET_EGQ_PARSER_LAST_SYS_RECORD_TBL_GET_ID, 
    "parser_last_sys_record_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_parser_last_sys_record_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PARSER_LAST_SYS_RECORD_TBL_GET_EGQ_PARSER_LAST_SYS_RECORD_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PARSER_LAST_NWK_RECORD1_TBL_SET_EGQ_PARSER_LAST_NWK_RECORD1_TBL_SET_ID, 
    "parser_last_nwk_record1_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_parser_last_nwk_record1_tbl_set entry_offset_ndx 0\n\r"
    "  parser_last_nwk_record1_index 0 parser_last_nwk_record1 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PARSER_LAST_NWK_RECORD1_TBL_SET_EGQ_PARSER_LAST_NWK_RECORD1_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PARSER_LAST_NWK_RECORD1_TBL_SET_EGQ_PARSER_LAST_NWK_RECORD1_TBL_SET_TBL_DATA_PARSER_LAST_NWK_RECORD1_INDEX_ID, 
    "parser_last_nwk_record1_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  parser_last_nwk_record1_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_parser_last_nwk_record1_tbl_set entry_offset_ndx 0\n\r"
    "  parser_last_nwk_record1_index 0 parser_last_nwk_record1 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PARSER_LAST_NWK_RECORD1_TBL_SET_EGQ_PARSER_LAST_NWK_RECORD1_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PARSER_LAST_NWK_RECORD1_TBL_SET_EGQ_PARSER_LAST_NWK_RECORD1_TBL_SET_TBL_DATA_PARSER_LAST_NWK_RECORD1_ID, 
    "parser_last_nwk_record1", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.parser_last_nwk_record1[ prm_parser_last_nwk_record1_index]:\n\r"
    "  uint32 parser_last_nwk_record1[8]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_parser_last_nwk_record1_tbl_set entry_offset_ndx 0\n\r"
    "  parser_last_nwk_record1_index 0 parser_last_nwk_record1 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PARSER_LAST_NWK_RECORD1_TBL_SET_EGQ_PARSER_LAST_NWK_RECORD1_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PARSER_LAST_NWK_RECORD1_TBL_GET_EGQ_PARSER_LAST_NWK_RECORD1_TBL_GET_ID, 
    "parser_last_nwk_record1_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_parser_last_nwk_record1_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PARSER_LAST_NWK_RECORD1_TBL_GET_EGQ_PARSER_LAST_NWK_RECORD1_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PARSER_LAST_NWK_RECORD2_TBL_SET_EGQ_PARSER_LAST_NWK_RECORD2_TBL_SET_ID, 
    "parser_last_nwk_record2_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_parser_last_nwk_record2_tbl_set entry_offset_ndx 0\n\r"
    "  parser_last_nwk_record2_index 0 parser_last_nwk_record2 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PARSER_LAST_NWK_RECORD2_TBL_SET_EGQ_PARSER_LAST_NWK_RECORD2_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PARSER_LAST_NWK_RECORD2_TBL_SET_EGQ_PARSER_LAST_NWK_RECORD2_TBL_SET_TBL_DATA_PARSER_LAST_NWK_RECORD2_INDEX_ID, 
    "parser_last_nwk_record2_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  parser_last_nwk_record2_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_parser_last_nwk_record2_tbl_set entry_offset_ndx 0\n\r"
    "  parser_last_nwk_record2_index 0 parser_last_nwk_record2 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PARSER_LAST_NWK_RECORD2_TBL_SET_EGQ_PARSER_LAST_NWK_RECORD2_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PARSER_LAST_NWK_RECORD2_TBL_SET_EGQ_PARSER_LAST_NWK_RECORD2_TBL_SET_TBL_DATA_PARSER_LAST_NWK_RECORD2_ID, 
    "parser_last_nwk_record2", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.parser_last_nwk_record2[ prm_parser_last_nwk_record2_index]:\n\r"
    "  uint32 parser_last_nwk_record2[2]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_parser_last_nwk_record2_tbl_set entry_offset_ndx 0\n\r"
    "  parser_last_nwk_record2_index 0 parser_last_nwk_record2 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PARSER_LAST_NWK_RECORD2_TBL_SET_EGQ_PARSER_LAST_NWK_RECORD2_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_PARSER_LAST_NWK_RECORD2_TBL_GET_EGQ_PARSER_LAST_NWK_RECORD2_TBL_GET_ID, 
    "parser_last_nwk_record2_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_parser_last_nwk_record2_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_PARSER_LAST_NWK_RECORD2_TBL_GET_EGQ_PARSER_LAST_NWK_RECORD2_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_ERPP_DEBUG_TBL_SET_EGQ_ERPP_DEBUG_TBL_SET_ID, 
    "erpp_debug_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_erpp_debug_tbl_set entry_offset_ndx 0\n\r"
    "  erpp_debug_index 0 erpp_debug 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_ERPP_DEBUG_TBL_SET_EGQ_ERPP_DEBUG_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_ERPP_DEBUG_TBL_SET_EGQ_ERPP_DEBUG_TBL_SET_TBL_DATA_ERPP_DEBUG_INDEX_ID, 
    "erpp_debug_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  erpp_debug_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_erpp_debug_tbl_set entry_offset_ndx 0\n\r"
    "  erpp_debug_index 0 erpp_debug 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_ERPP_DEBUG_TBL_SET_EGQ_ERPP_DEBUG_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_ERPP_DEBUG_TBL_SET_EGQ_ERPP_DEBUG_TBL_SET_TBL_DATA_ERPP_DEBUG_ID, 
    "erpp_debug", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.erpp_debug[ prm_erpp_debug_index]:\n\r"
    "  uint32 erpp_debug[8]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_erpp_debug_tbl_set entry_offset_ndx 0\n\r"
    "  erpp_debug_index 0 erpp_debug 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_ERPP_DEBUG_TBL_SET_EGQ_ERPP_DEBUG_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EGQ_ERPP_DEBUG_TBL_GET_EGQ_ERPP_DEBUG_TBL_GET_ID, 
    "erpp_debug_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc egq_erpp_debug_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EGQ_ERPP_DEBUG_TBL_GET_EGQ_ERPP_DEBUG_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_PINFO_LBP_TBL_SET_IHB_PINFO_LBP_TBL_SET_ID, 
    "pinfo_lbp_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_pinfo_lbp_tbl_set entry_offset_ndx 0\n\r"
    "  counter_compension 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_PINFO_LBP_TBL_SET_IHB_PINFO_LBP_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_PINFO_LBP_TBL_SET_IHB_PINFO_LBP_TBL_SET_TBL_DATA_COUNTER_COMPENSION_ID, 
    "counter_compension", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.counter_compension:\n\r"
    "  uint32 counter_compension\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_pinfo_lbp_tbl_set entry_offset_ndx 0\n\r"
    "  counter_compension 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_PINFO_LBP_TBL_SET_IHB_PINFO_LBP_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_PINFO_LBP_TBL_GET_IHB_PINFO_LBP_TBL_GET_ID, 
    "pinfo_lbp_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_pinfo_lbp_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_PINFO_LBP_TBL_GET_IHB_PINFO_LBP_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_PINFO_PMF_TBL_SET_IHB_PINFO_PMF_TBL_SET_ID, 
    "pinfo_pmf_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_pinfo_pmf_tbl_set entry_offset_ndx 0 port_pmf_profile\n\r"
    "  0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_PINFO_PMF_TBL_SET_IHB_PINFO_PMF_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_PINFO_PMF_TBL_SET_IHB_PINFO_PMF_TBL_SET_TBL_DATA_PORT_PMF_PROFILE_ID, 
    "port_pmf_profile", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.port_pmf_profile:\n\r"
    "  uint32 port_pmf_profile\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_pinfo_pmf_tbl_set entry_offset_ndx 0 port_pmf_profile\n\r"
    "  0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_PINFO_PMF_TBL_SET_IHB_PINFO_PMF_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_PINFO_PMF_TBL_GET_IHB_PINFO_PMF_TBL_GET_ID, 
    "pinfo_pmf_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_pinfo_pmf_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_PINFO_PMF_TBL_GET_IHB_PINFO_PMF_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_PACKET_FORMAT_CODE_PROFILE_TBL_SET_IHB_PACKET_FORMAT_CODE_PROFILE_TBL_SET_ID, 
    "packet_format_code_profile_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_packet_format_code_profile_tbl_set entry_offset_ndx 0\n\r"
    "  packet_format_code_profile 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_PACKET_FORMAT_CODE_PROFILE_TBL_SET_IHB_PACKET_FORMAT_CODE_PROFILE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_PACKET_FORMAT_CODE_PROFILE_TBL_SET_IHB_PACKET_FORMAT_CODE_PROFILE_TBL_SET_TBL_DATA_PACKET_FORMAT_CODE_PROFILE_ID, 
    "packet_format_code_profile", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.packet_format_code_profile:\n\r"
    "  uint32 packet_format_code_profile\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_packet_format_code_profile_tbl_set entry_offset_ndx 0\n\r"
    "  packet_format_code_profile 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_PACKET_FORMAT_CODE_PROFILE_TBL_SET_IHB_PACKET_FORMAT_CODE_PROFILE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_PACKET_FORMAT_CODE_PROFILE_TBL_GET_IHB_PACKET_FORMAT_CODE_PROFILE_TBL_GET_ID, 
    "packet_format_code_profile_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_packet_format_code_profile_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_PACKET_FORMAT_CODE_PROFILE_TBL_GET_IHB_PACKET_FORMAT_CODE_PROFILE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_SRC_DEST_PORT_FOR_L3_ACL_KEY_TBL_SET_IHB_SRC_DEST_PORT_FOR_L3_ACL_KEY_TBL_SET_ID, 
    "src_dest_port_for_l3_acl_key_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_src_dest_port_for_l3_acl_key_tbl_set entry_offset_ndx\n\r"
    "  0 destination_port_max 0 destination_port_min 0 source_port_max 0\n\r"
    "  source_port_min 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_SRC_DEST_PORT_FOR_L3_ACL_KEY_TBL_SET_IHB_SRC_DEST_PORT_FOR_L3_ACL_KEY_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_SRC_DEST_PORT_FOR_L3_ACL_KEY_TBL_SET_IHB_SRC_DEST_PORT_FOR_L3_ACL_KEY_TBL_SET_TBL_DATA_DESTINATION_PORT_MAX_ID, 
    "destination_port_max", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.destination_port_max:\n\r"
    "  uint32 destination_port_max\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_src_dest_port_for_l3_acl_key_tbl_set entry_offset_ndx\n\r"
    "  0 destination_port_max 0 destination_port_min 0 source_port_max 0\n\r"
    "  source_port_min 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_SRC_DEST_PORT_FOR_L3_ACL_KEY_TBL_SET_IHB_SRC_DEST_PORT_FOR_L3_ACL_KEY_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_SRC_DEST_PORT_FOR_L3_ACL_KEY_TBL_SET_IHB_SRC_DEST_PORT_FOR_L3_ACL_KEY_TBL_SET_TBL_DATA_DESTINATION_PORT_MIN_ID, 
    "destination_port_min", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.destination_port_min:\n\r"
    "  uint32 destination_port_min\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_src_dest_port_for_l3_acl_key_tbl_set entry_offset_ndx\n\r"
    "  0 destination_port_max 0 destination_port_min 0 source_port_max 0\n\r"
    "  source_port_min 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_SRC_DEST_PORT_FOR_L3_ACL_KEY_TBL_SET_IHB_SRC_DEST_PORT_FOR_L3_ACL_KEY_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_SRC_DEST_PORT_FOR_L3_ACL_KEY_TBL_SET_IHB_SRC_DEST_PORT_FOR_L3_ACL_KEY_TBL_SET_TBL_DATA_SOURCE_PORT_MAX_ID, 
    "source_port_max", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.source_port_max:\n\r"
    "  uint32 source_port_max\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_src_dest_port_for_l3_acl_key_tbl_set entry_offset_ndx\n\r"
    "  0 destination_port_max 0 destination_port_min 0 source_port_max 0\n\r"
    "  source_port_min 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_SRC_DEST_PORT_FOR_L3_ACL_KEY_TBL_SET_IHB_SRC_DEST_PORT_FOR_L3_ACL_KEY_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_SRC_DEST_PORT_FOR_L3_ACL_KEY_TBL_SET_IHB_SRC_DEST_PORT_FOR_L3_ACL_KEY_TBL_SET_TBL_DATA_SOURCE_PORT_MIN_ID, 
    "source_port_min", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.source_port_min:\n\r"
    "  uint32 source_port_min\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_src_dest_port_for_l3_acl_key_tbl_set entry_offset_ndx\n\r"
    "  0 destination_port_max 0 destination_port_min 0 source_port_max 0\n\r"
    "  source_port_min 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_SRC_DEST_PORT_FOR_L3_ACL_KEY_TBL_SET_IHB_SRC_DEST_PORT_FOR_L3_ACL_KEY_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_SRC_DEST_PORT_FOR_L3_ACL_KEY_TBL_GET_IHB_SRC_DEST_PORT_FOR_L3_ACL_KEY_TBL_GET_ID, 
    "src_dest_port_for_l3_acl_key_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_src_dest_port_for_l3_acl_key_tbl_get entry_offset_ndx\n\r"
    "  0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_SRC_DEST_PORT_FOR_L3_ACL_KEY_TBL_GET_IHB_SRC_DEST_PORT_FOR_L3_ACL_KEY_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID, 
    "fem_key_profile_resolved_data_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem_key_profile_resolved_data_tbl_set fem_ndx 0\n\r"
    "  cycle_ndx 0 entry_offset_ndx 0 key_select 0 program 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_SET_FEM_NDX_ID, 
    "key_fem_ndx", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  fem_ndx:\n\r"
    "  SOC_SAND_IN\n\r"
    "  uint32\n\r"
    "  fem_ndx\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem_key_profile_resolved_data_tbl_set fem_ndx 0\n\r"
    "  cycle_ndx 0 entry_offset_ndx 0 key_select 0 program 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_SET_CYCLE_NDX_ID, 
    "key_cycle_ndx", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  cycle_ndx:\n\r"
    "  SOC_SAND_IN\n\r"
    "  uint32\n\r"
    "  cycle_ndx\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem_key_profile_resolved_data_tbl_set fem_ndx 0\n\r"
    "  cycle_ndx 0 entry_offset_ndx 0 key_select 0 program 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_SET_TBL_DATA_KEY_SELECT_ID, 
    "key_select", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.key_select:\n\r"
    "  uint32 key_select\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem_key_profile_resolved_data_tbl_set fem_ndx 0\n\r"
    "  cycle_ndx 0 entry_offset_ndx 0 key_select 0 program 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_SET_TBL_DATA_PROGRAM_ID, 
    "program", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.program:\n\r"
    "  uint32 program\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem_key_profile_resolved_data_tbl_set fem_ndx 0\n\r"
    "  cycle_ndx 0 entry_offset_ndx 0 key_select 0 program 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_GET_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_GET_ID, 
    "fem_key_profile_resolved_data_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem_key_profile_resolved_data_tbl_get fem_ndx 0\n\r"
    "  cycle_ndx 0 entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_GET_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_GET_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_GET_FEM_NDX_ID, 
    "key_fem_ndx", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  fem_ndx:\n\r"
    "  SOC_SAND_IN\n\r"
    "  uint32\n\r"
    "  fem_ndx\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem_key_profile_resolved_data_tbl_get fem_ndx 0\n\r"
    "  cycle_ndx 0 entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_GET_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_GET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_GET_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_GET_CYCLE_NDX_ID, 
    "key_cycle_ndx", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  cycle_ndx:\n\r"
    "  SOC_SAND_IN\n\r"
    "  uint32\n\r"
    "  cycle_ndx\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem_key_profile_resolved_data_tbl_get fem_ndx 0\n\r"
    "  cycle_ndx 0 entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_GET_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_GET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_SET_ID, 
    "fem_program_resolved_data_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem_program_resolved_data_tbl_set fem_ndx 0 cycle_ndx\n\r"
    "  0 entry_offset_ndx 0 bit_select 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_SET_FEM_NDX_ID, 
    "key_fem_ndx", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  fem_ndx:\n\r"
    "  SOC_SAND_IN\n\r"
    "  uint32\n\r"
    "  fem_ndx\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem_program_resolved_data_tbl_set fem_ndx 0 cycle_ndx\n\r"
    "  0 entry_offset_ndx 0 bit_select 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_SET_CYCLE_NDX_ID, 
    "key_cycle_ndx", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  cycle_ndx:\n\r"
    "  SOC_SAND_IN\n\r"
    "  uint32\n\r"
    "  cycle_ndx\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem_program_resolved_data_tbl_set fem_ndx 0 cycle_ndx\n\r"
    "  0 entry_offset_ndx 0 bit_select 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_SET_TBL_DATA_BIT_SELECT_ID, 
    "bit_select", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.bit_select:\n\r"
    "  uint32 bit_select\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem_program_resolved_data_tbl_set fem_ndx 0 cycle_ndx\n\r"
    "  0 entry_offset_ndx 0 bit_select 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_GET_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_GET_ID, 
    "fem_program_resolved_data_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem_program_resolved_data_tbl_get fem_ndx 0 cycle_ndx\n\r"
    "  0 entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_GET_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_GET_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_GET_FEM_NDX_ID, 
    "key_fem_ndx", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  fem_ndx:\n\r"
    "  SOC_SAND_IN\n\r"
    "  uint32\n\r"
    "  fem_ndx\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem_program_resolved_data_tbl_get fem_ndx 0 cycle_ndx\n\r"
    "  0 entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_GET_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_GET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_GET_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_GET_CYCLE_NDX_ID, 
    "key_cycle_ndx", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  cycle_ndx:\n\r"
    "  SOC_SAND_IN\n\r"
    "  uint32\n\r"
    "  cycle_ndx\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem_program_resolved_data_tbl_get fem_ndx 0 cycle_ndx\n\r"
    "  0 entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_GET_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_GET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_SET_IHB_FEM_MAP_INDEX_TABLE_TBL_SET_ID, 
    "fem_map_index_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem_map_index_table_tbl_set fem_ndx 0 cycle_ndx 0\n\r"
    "  entry_offset_ndx 0 map_data 0 map_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_SET_IHB_FEM_MAP_INDEX_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_SET_IHB_FEM_MAP_INDEX_TABLE_TBL_SET_FEM_NDX_ID, 
    "key_fem_ndx", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  fem_ndx:\n\r"
    "  SOC_SAND_IN\n\r"
    "  uint32\n\r"
    "  fem_ndx\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem_map_index_table_tbl_set fem_ndx 0 cycle_ndx 0\n\r"
    "  entry_offset_ndx 0 map_data 0 map_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_SET_IHB_FEM_MAP_INDEX_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_SET_IHB_FEM_MAP_INDEX_TABLE_TBL_SET_CYCLE_NDX_ID, 
    "key_cycle_ndx", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  cycle_ndx:\n\r"
    "  SOC_SAND_IN\n\r"
    "  uint32\n\r"
    "  cycle_ndx\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem_map_index_table_tbl_set fem_ndx 0 cycle_ndx 0\n\r"
    "  entry_offset_ndx 0 map_data 0 map_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_SET_IHB_FEM_MAP_INDEX_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_SET_IHB_FEM_MAP_INDEX_TABLE_TBL_SET_TBL_DATA_MAP_DATA_ID, 
    "map_data", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.map_data:\n\r"
    "  uint32 map_data\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem_map_index_table_tbl_set fem_ndx 0 cycle_ndx 0\n\r"
    "  entry_offset_ndx 0 map_data 0 map_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_SET_IHB_FEM_MAP_INDEX_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_SET_IHB_FEM_MAP_INDEX_TABLE_TBL_SET_TBL_DATA_MAP_INDEX_ID, 
    "map_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.map_index:\n\r"
    "  uint32 map_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem_map_index_table_tbl_set fem_ndx 0 cycle_ndx 0\n\r"
    "  entry_offset_ndx 0 map_data 0 map_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_SET_IHB_FEM_MAP_INDEX_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_GET_IHB_FEM_MAP_INDEX_TABLE_TBL_GET_ID, 
    "fem_map_index_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem_map_index_table_tbl_get fem_ndx 0 cycle_ndx 0\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_GET_IHB_FEM_MAP_INDEX_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_GET_IHB_FEM_MAP_INDEX_TABLE_TBL_GET_FEM_NDX_ID, 
    "key_fem_ndx", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  fem_ndx:\n\r"
    "  SOC_SAND_IN\n\r"
    "  uint32\n\r"
    "  fem_ndx\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem_map_index_table_tbl_get fem_ndx 0 cycle_ndx 0\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_GET_IHB_FEM_MAP_INDEX_TABLE_TBL_GET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_GET_IHB_FEM_MAP_INDEX_TABLE_TBL_GET_CYCLE_NDX_ID, 
    "key_cycle_ndx", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  cycle_ndx:\n\r"
    "  SOC_SAND_IN\n\r"
    "  uint32\n\r"
    "  cycle_ndx\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem_map_index_table_tbl_get fem_ndx 0 cycle_ndx 0\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_GET_IHB_FEM_MAP_INDEX_TABLE_TBL_GET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM_MAP_TABLE_TBL_SET_IHB_FEM_MAP_TABLE_TBL_SET_ID, 
    "fem_map_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem_map_table_tbl_set fem_ndx 0 cycle_ndx 0\n\r"
    "  entry_offset_ndx 0 offset_index 0 field_select_map_index 0 field_select_map 0\n\r"
    "  action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM_MAP_TABLE_TBL_SET_IHB_FEM_MAP_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM_MAP_TABLE_TBL_SET_IHB_FEM_MAP_TABLE_TBL_SET_FEM_NDX_ID, 
    "key_fem_ndx", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  fem_ndx:\n\r"
    "  SOC_SAND_IN\n\r"
    "  uint32\n\r"
    "  fem_ndx\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem_map_table_tbl_set fem_ndx 0 cycle_ndx 0\n\r"
    "  entry_offset_ndx 0 offset_index 0 field_select_map_index 0 field_select_map 0\n\r"
    "  action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM_MAP_TABLE_TBL_SET_IHB_FEM_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM_MAP_TABLE_TBL_SET_IHB_FEM_MAP_TABLE_TBL_SET_CYCLE_NDX_ID, 
    "key_cycle_ndx", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  cycle_ndx:\n\r"
    "  SOC_SAND_IN\n\r"
    "  uint32\n\r"
    "  cycle_ndx\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem_map_table_tbl_set fem_ndx 0 cycle_ndx 0\n\r"
    "  entry_offset_ndx 0 offset_index 0 field_select_map_index 0 field_select_map 0\n\r"
    "  action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM_MAP_TABLE_TBL_SET_IHB_FEM_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM_MAP_TABLE_TBL_SET_IHB_FEM_MAP_TABLE_TBL_SET_TBL_DATA_OFFSET_INDEX_ID, 
    "offset_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.offset_index:\n\r"
    "  uint32 offset_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem_map_table_tbl_set fem_ndx 0 cycle_ndx 0\n\r"
    "  entry_offset_ndx 0 offset_index 0 field_select_map_index 0 field_select_map 0\n\r"
    "  action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM_MAP_TABLE_TBL_SET_IHB_FEM_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM_MAP_TABLE_TBL_SET_IHB_FEM_MAP_TABLE_TBL_SET_TBL_DATA_FIELD_SELECT_MAP_INDEX_ID, 
    "field_select_map_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  field_select_map_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem_map_table_tbl_set fem_ndx 0 cycle_ndx 0\n\r"
    "  entry_offset_ndx 0 offset_index 0 field_select_map_index 0 field_select_map 0\n\r"
    "  action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM_MAP_TABLE_TBL_SET_IHB_FEM_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM_MAP_TABLE_TBL_SET_IHB_FEM_MAP_TABLE_TBL_SET_TBL_DATA_FIELD_SELECT_MAP_ID, 
    "field_select_map", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.field_select_map[ prm_field_select_map_index]:\n\r"
    "  uint32 field_select_map[SOC_PB_IHB_FEM_MAX_OUTPUT_SIZE]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem_map_table_tbl_set fem_ndx 0 cycle_ndx 0\n\r"
    "  entry_offset_ndx 0 offset_index 0 field_select_map_index 0 field_select_map 0\n\r"
    "  action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM_MAP_TABLE_TBL_SET_IHB_FEM_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM_MAP_TABLE_TBL_SET_IHB_FEM_MAP_TABLE_TBL_SET_TBL_DATA_ACTION_TYPE_ID, 
    "action_type", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.action_type:\n\r"
    "  uint32 action_type\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem_map_table_tbl_set fem_ndx 0 cycle_ndx 0\n\r"
    "  entry_offset_ndx 0 offset_index 0 field_select_map_index 0 field_select_map 0\n\r"
    "  action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM_MAP_TABLE_TBL_SET_IHB_FEM_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM_MAP_TABLE_TBL_GET_IHB_FEM_MAP_TABLE_TBL_GET_ID, 
    "fem_map_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem_map_table_tbl_get fem_ndx 0 cycle_ndx 0\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM_MAP_TABLE_TBL_GET_IHB_FEM_MAP_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM_MAP_TABLE_TBL_GET_IHB_FEM_MAP_TABLE_TBL_GET_FEM_NDX_ID, 
    "key_fem_ndx", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  fem_ndx:\n\r"
    "  SOC_SAND_IN\n\r"
    "  uint32\n\r"
    "  fem_ndx\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem_map_table_tbl_get fem_ndx 0 cycle_ndx 0\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM_MAP_TABLE_TBL_GET_IHB_FEM_MAP_TABLE_TBL_GET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM_MAP_TABLE_TBL_GET_IHB_FEM_MAP_TABLE_TBL_GET_CYCLE_NDX_ID, 
    "key_cycle_ndx", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  cycle_ndx:\n\r"
    "  SOC_SAND_IN\n\r"
    "  uint32\n\r"
    "  cycle_ndx\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem_map_table_tbl_get fem_ndx 0 cycle_ndx 0\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM_MAP_TABLE_TBL_GET_IHB_FEM_MAP_TABLE_TBL_GET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM_OFFSET_TABLE_TBL_SET_IHB_FEM_OFFSET_TABLE_TBL_SET_ID, 
    "fem_offset_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem_offset_table_tbl_set fem_ndx 0 cycle_ndx 0\n\r"
    "  entry_offset_ndx 0 offset 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM_OFFSET_TABLE_TBL_SET_IHB_FEM_OFFSET_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM_OFFSET_TABLE_TBL_SET_IHB_FEM_OFFSET_TABLE_TBL_SET_FEM_NDX_ID, 
    "key_fem_ndx", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  fem_ndx:\n\r"
    "  SOC_SAND_IN\n\r"
    "  uint32\n\r"
    "  fem_ndx\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem_offset_table_tbl_set fem_ndx 0 cycle_ndx 0\n\r"
    "  entry_offset_ndx 0 offset 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM_OFFSET_TABLE_TBL_SET_IHB_FEM_OFFSET_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM_OFFSET_TABLE_TBL_SET_IHB_FEM_OFFSET_TABLE_TBL_SET_CYCLE_NDX_ID, 
    "key_cycle_ndx", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  cycle_ndx:\n\r"
    "  SOC_SAND_IN\n\r"
    "  uint32\n\r"
    "  cycle_ndx\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem_offset_table_tbl_set fem_ndx 0 cycle_ndx 0\n\r"
    "  entry_offset_ndx 0 offset 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM_OFFSET_TABLE_TBL_SET_IHB_FEM_OFFSET_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM_OFFSET_TABLE_TBL_SET_IHB_FEM_OFFSET_TABLE_TBL_SET_TBL_DATA_OFFSET_ID, 
    "offset", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.offset:\n\r"
    "  uint32 offset\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem_offset_table_tbl_set fem_ndx 0 cycle_ndx 0\n\r"
    "  entry_offset_ndx 0 offset 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM_OFFSET_TABLE_TBL_SET_IHB_FEM_OFFSET_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM_OFFSET_TABLE_TBL_GET_IHB_FEM_OFFSET_TABLE_TBL_GET_ID, 
    "fem_offset_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem_offset_table_tbl_get fem_ndx 0 cycle_ndx 0\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM_OFFSET_TABLE_TBL_GET_IHB_FEM_OFFSET_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM_OFFSET_TABLE_TBL_GET_IHB_FEM_OFFSET_TABLE_TBL_GET_FEM_NDX_ID, 
    "key_fem_ndx", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  fem_ndx:\n\r"
    "  SOC_SAND_IN\n\r"
    "  uint32\n\r"
    "  fem_ndx\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem_offset_table_tbl_get fem_ndx 0 cycle_ndx 0\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM_OFFSET_TABLE_TBL_GET_IHB_FEM_OFFSET_TABLE_TBL_GET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM_OFFSET_TABLE_TBL_GET_IHB_FEM_OFFSET_TABLE_TBL_GET_CYCLE_NDX_ID, 
    "key_cycle_ndx", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  cycle_ndx:\n\r"
    "  SOC_SAND_IN\n\r"
    "  uint32\n\r"
    "  cycle_ndx\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem_offset_table_tbl_get fem_ndx 0 cycle_ndx 0\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM_OFFSET_TABLE_TBL_GET_IHB_FEM_OFFSET_TABLE_TBL_GET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM0_4B_1ST_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM0_4B_1ST_PASS_OFFSET_TABLE_TBL_SET_ID, 
    "fem0_4b_1st_pass_offset_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem0_4b_1st_pass_offset_table_tbl_set\n\r"
    "  entry_offset_ndx 0 offset 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM0_4B_1ST_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM0_4B_1ST_PASS_OFFSET_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM0_4B_1ST_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM0_4B_1ST_PASS_OFFSET_TABLE_TBL_SET_TBL_DATA_OFFSET_ID, 
    "offset", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.offset:\n\r"
    "  uint32 offset\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem0_4b_1st_pass_offset_table_tbl_set\n\r"
    "  entry_offset_ndx 0 offset 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM0_4B_1ST_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM0_4B_1ST_PASS_OFFSET_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM0_4B_1ST_PASS_OFFSET_TABLE_TBL_GET_IHB_FEM0_4B_1ST_PASS_OFFSET_TABLE_TBL_GET_ID, 
    "fem0_4b_1st_pass_offset_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem0_4b_1st_pass_offset_table_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM0_4B_1ST_PASS_OFFSET_TABLE_TBL_GET_IHB_FEM0_4B_1ST_PASS_OFFSET_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM1_4B_1ST_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM1_4B_1ST_PASS_OFFSET_TABLE_TBL_SET_ID, 
    "fem1_4b_1st_pass_offset_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem1_4b_1st_pass_offset_table_tbl_set\n\r"
    "  entry_offset_ndx 0 offset 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM1_4B_1ST_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM1_4B_1ST_PASS_OFFSET_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM1_4B_1ST_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM1_4B_1ST_PASS_OFFSET_TABLE_TBL_SET_TBL_DATA_OFFSET_ID, 
    "offset", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.offset:\n\r"
    "  uint32 offset\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem1_4b_1st_pass_offset_table_tbl_set\n\r"
    "  entry_offset_ndx 0 offset 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM1_4B_1ST_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM1_4B_1ST_PASS_OFFSET_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM1_4B_1ST_PASS_OFFSET_TABLE_TBL_GET_IHB_FEM1_4B_1ST_PASS_OFFSET_TABLE_TBL_GET_ID, 
    "fem1_4b_1st_pass_offset_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem1_4b_1st_pass_offset_table_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM1_4B_1ST_PASS_OFFSET_TABLE_TBL_GET_IHB_FEM1_4B_1ST_PASS_OFFSET_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM0_4B_2ND_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM0_4B_2ND_PASS_OFFSET_TABLE_TBL_SET_ID, 
    "fem0_4b_2nd_pass_offset_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem0_4b_2nd_pass_offset_table_tbl_set\n\r"
    "  entry_offset_ndx 0 offset 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM0_4B_2ND_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM0_4B_2ND_PASS_OFFSET_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM0_4B_2ND_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM0_4B_2ND_PASS_OFFSET_TABLE_TBL_SET_TBL_DATA_OFFSET_ID, 
    "offset", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.offset:\n\r"
    "  uint32 offset\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem0_4b_2nd_pass_offset_table_tbl_set\n\r"
    "  entry_offset_ndx 0 offset 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM0_4B_2ND_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM0_4B_2ND_PASS_OFFSET_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM0_4B_2ND_PASS_OFFSET_TABLE_TBL_GET_IHB_FEM0_4B_2ND_PASS_OFFSET_TABLE_TBL_GET_ID, 
    "fem0_4b_2nd_pass_offset_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem0_4b_2nd_pass_offset_table_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM0_4B_2ND_PASS_OFFSET_TABLE_TBL_GET_IHB_FEM0_4B_2ND_PASS_OFFSET_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM1_4B_2ND_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM1_4B_2ND_PASS_OFFSET_TABLE_TBL_SET_ID, 
    "fem1_4b_2nd_pass_offset_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem1_4b_2nd_pass_offset_table_tbl_set\n\r"
    "  entry_offset_ndx 0 offset 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM1_4B_2ND_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM1_4B_2ND_PASS_OFFSET_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM1_4B_2ND_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM1_4B_2ND_PASS_OFFSET_TABLE_TBL_SET_TBL_DATA_OFFSET_ID, 
    "offset", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.offset:\n\r"
    "  uint32 offset\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem1_4b_2nd_pass_offset_table_tbl_set\n\r"
    "  entry_offset_ndx 0 offset 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM1_4B_2ND_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM1_4B_2ND_PASS_OFFSET_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM1_4B_2ND_PASS_OFFSET_TABLE_TBL_GET_IHB_FEM1_4B_2ND_PASS_OFFSET_TABLE_TBL_GET_ID, 
    "fem1_4b_2nd_pass_offset_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem1_4b_2nd_pass_offset_table_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM1_4B_2ND_PASS_OFFSET_TABLE_TBL_GET_IHB_FEM1_4B_2ND_PASS_OFFSET_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_DIRECT_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_DIRECT_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID, 
    "direct_1st_pass_key_profile_resolved_data_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_direct_1st_pass_key_profile_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 direct_db_or_value 0 direct_db_and_value 0\n\r"
    "  direct_key_select 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_DIRECT_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_DIRECT_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_DIRECT_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_DIRECT_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_TBL_DATA_DIRECT_DB_OR_VALUE_ID, 
    "direct_db_or_value", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.direct_db_or_value:\n\r"
    "  uint32 direct_db_or_value\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_direct_1st_pass_key_profile_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 direct_db_or_value 0 direct_db_and_value 0\n\r"
    "  direct_key_select 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_DIRECT_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_DIRECT_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_DIRECT_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_DIRECT_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_TBL_DATA_DIRECT_DB_AND_VALUE_ID, 
    "direct_db_and_value", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.direct_db_and_value:\n\r"
    "  uint32 direct_db_and_value\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_direct_1st_pass_key_profile_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 direct_db_or_value 0 direct_db_and_value 0\n\r"
    "  direct_key_select 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_DIRECT_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_DIRECT_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_DIRECT_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_DIRECT_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_TBL_DATA_DIRECT_KEY_SELECT_ID, 
    "direct_key_select", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.direct_key_select:\n\r"
    "  uint32 direct_key_select\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_direct_1st_pass_key_profile_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 direct_db_or_value 0 direct_db_and_value 0\n\r"
    "  direct_key_select 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_DIRECT_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_DIRECT_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_DIRECT_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_IHB_DIRECT_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_ID, 
    "direct_1st_pass_key_profile_resolved_data_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_direct_1st_pass_key_profile_resolved_data_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_DIRECT_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_IHB_DIRECT_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_DIRECT_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_DIRECT_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID, 
    "direct_2nd_pass_key_profile_resolved_data_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_direct_2nd_pass_key_profile_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 direct_db_or_value 0 direct_db_and_value 0\n\r"
    "  direct_key_select 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_DIRECT_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_DIRECT_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_DIRECT_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_DIRECT_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_TBL_DATA_DIRECT_DB_OR_VALUE_ID, 
    "direct_db_or_value", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.direct_db_or_value:\n\r"
    "  uint32 direct_db_or_value\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_direct_2nd_pass_key_profile_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 direct_db_or_value 0 direct_db_and_value 0\n\r"
    "  direct_key_select 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_DIRECT_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_DIRECT_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_DIRECT_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_DIRECT_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_TBL_DATA_DIRECT_DB_AND_VALUE_ID, 
    "direct_db_and_value", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.direct_db_and_value:\n\r"
    "  uint32 direct_db_and_value\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_direct_2nd_pass_key_profile_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 direct_db_or_value 0 direct_db_and_value 0\n\r"
    "  direct_key_select 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_DIRECT_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_DIRECT_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_DIRECT_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_DIRECT_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_TBL_DATA_DIRECT_KEY_SELECT_ID, 
    "direct_key_select", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.direct_key_select:\n\r"
    "  uint32 direct_key_select\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_direct_2nd_pass_key_profile_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 direct_db_or_value 0 direct_db_and_value 0\n\r"
    "  direct_key_select 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_DIRECT_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_DIRECT_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_DIRECT_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_IHB_DIRECT_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_ID, 
    "direct_2nd_pass_key_profile_resolved_data_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_direct_2nd_pass_key_profile_resolved_data_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_DIRECT_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_IHB_DIRECT_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM0_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM0_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID, 
    "fem0_4b_1st_pass_key_profile_resolved_data_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem0_4b_1st_pass_key_profile_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 key_select 0 program 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM0_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM0_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM0_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM0_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_TBL_DATA_KEY_SELECT_ID, 
    "key_select", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.key_select:\n\r"
    "  uint32 key_select\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem0_4b_1st_pass_key_profile_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 key_select 0 program 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM0_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM0_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM0_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM0_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_TBL_DATA_PROGRAM_ID, 
    "program", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.program:\n\r"
    "  uint32 program\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem0_4b_1st_pass_key_profile_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 key_select 0 program 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM0_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM0_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM0_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_IHB_FEM0_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_ID, 
    "fem0_4b_1st_pass_key_profile_resolved_data_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem0_4b_1st_pass_key_profile_resolved_data_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM0_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_IHB_FEM0_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM0_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM0_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_ID, 
    "fem0_4b_1st_pass_program_resolved_data_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem0_4b_1st_pass_program_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 bit_select 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM0_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM0_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM0_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM0_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_TBL_DATA_BIT_SELECT_ID, 
    "bit_select", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.bit_select:\n\r"
    "  uint32 bit_select\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem0_4b_1st_pass_program_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 bit_select 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM0_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM0_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM0_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_IHB_FEM0_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_ID, 
    "fem0_4b_1st_pass_program_resolved_data_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem0_4b_1st_pass_program_resolved_data_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM0_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_IHB_FEM0_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM0_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM0_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_ID, 
    "fem0_4b_1st_pass_map_index_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem0_4b_1st_pass_map_index_table_tbl_set\n\r"
    "  entry_offset_ndx 0 map_data 0 map_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM0_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM0_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM0_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM0_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_TBL_DATA_MAP_DATA_ID, 
    "map_data", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.map_data:\n\r"
    "  uint32 map_data\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem0_4b_1st_pass_map_index_table_tbl_set\n\r"
    "  entry_offset_ndx 0 map_data 0 map_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM0_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM0_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM0_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM0_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_TBL_DATA_MAP_INDEX_ID, 
    "map_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.map_index:\n\r"
    "  uint32 map_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem0_4b_1st_pass_map_index_table_tbl_set\n\r"
    "  entry_offset_ndx 0 map_data 0 map_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM0_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM0_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM0_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_GET_IHB_FEM0_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_GET_ID, 
    "fem0_4b_1st_pass_map_index_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem0_4b_1st_pass_map_index_table_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM0_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_GET_IHB_FEM0_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM0_4B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM0_4B_1ST_PASS_MAP_TABLE_TBL_SET_ID, 
    "fem0_4b_1st_pass_map_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem0_4b_1st_pass_map_table_tbl_set entry_offset_ndx 0\n\r"
    "  offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM0_4B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM0_4B_1ST_PASS_MAP_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM0_4B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM0_4B_1ST_PASS_MAP_TABLE_TBL_SET_TBL_DATA_OFFSET_INDEX_ID, 
    "offset_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.offset_index:\n\r"
    "  uint32 offset_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem0_4b_1st_pass_map_table_tbl_set entry_offset_ndx 0\n\r"
    "  offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM0_4B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM0_4B_1ST_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM0_4B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM0_4B_1ST_PASS_MAP_TABLE_TBL_SET_TBL_DATA_FIELD_SELECT_MAP_INDEX_ID, 
    "field_select_map_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  field_select_map_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem0_4b_1st_pass_map_table_tbl_set entry_offset_ndx 0\n\r"
    "  offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM0_4B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM0_4B_1ST_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM0_4B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM0_4B_1ST_PASS_MAP_TABLE_TBL_SET_TBL_DATA_FIELD_SELECT_MAP_ID, 
    "field_select_map", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.field_select_map[ prm_field_select_map_index]:\n\r"
    "  uint32 field_select_map[SOC_PB_IHB_FEM_MAX_OUTPUT_SIZE]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem0_4b_1st_pass_map_table_tbl_set entry_offset_ndx 0\n\r"
    "  offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM0_4B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM0_4B_1ST_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM0_4B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM0_4B_1ST_PASS_MAP_TABLE_TBL_SET_TBL_DATA_ACTION_TYPE_ID, 
    "action_type", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.action_type:\n\r"
    "  uint32 action_type\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem0_4b_1st_pass_map_table_tbl_set entry_offset_ndx 0\n\r"
    "  offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM0_4B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM0_4B_1ST_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM0_4B_1ST_PASS_MAP_TABLE_TBL_GET_IHB_FEM0_4B_1ST_PASS_MAP_TABLE_TBL_GET_ID, 
    "fem0_4b_1st_pass_map_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem0_4b_1st_pass_map_table_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM0_4B_1ST_PASS_MAP_TABLE_TBL_GET_IHB_FEM0_4B_1ST_PASS_MAP_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM1_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM1_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID, 
    "fem1_4b_1st_pass_key_profile_resolved_data_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem1_4b_1st_pass_key_profile_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 key_select 0 program 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM1_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM1_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM1_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM1_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_TBL_DATA_KEY_SELECT_ID, 
    "key_select", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.key_select:\n\r"
    "  uint32 key_select\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem1_4b_1st_pass_key_profile_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 key_select 0 program 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM1_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM1_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM1_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM1_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_TBL_DATA_PROGRAM_ID, 
    "program", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.program:\n\r"
    "  uint32 program\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem1_4b_1st_pass_key_profile_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 key_select 0 program 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM1_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM1_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM1_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_IHB_FEM1_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_ID, 
    "fem1_4b_1st_pass_key_profile_resolved_data_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem1_4b_1st_pass_key_profile_resolved_data_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM1_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_IHB_FEM1_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM1_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM1_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_ID, 
    "fem1_4b_1st_pass_program_resolved_data_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem1_4b_1st_pass_program_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 bit_select 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM1_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM1_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM1_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM1_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_TBL_DATA_BIT_SELECT_ID, 
    "bit_select", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.bit_select:\n\r"
    "  uint32 bit_select\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem1_4b_1st_pass_program_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 bit_select 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM1_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM1_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM1_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_IHB_FEM1_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_ID, 
    "fem1_4b_1st_pass_program_resolved_data_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem1_4b_1st_pass_program_resolved_data_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM1_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_IHB_FEM1_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM1_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM1_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_ID, 
    "fem1_4b_1st_pass_map_index_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem1_4b_1st_pass_map_index_table_tbl_set\n\r"
    "  entry_offset_ndx 0 map_data 0 map_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM1_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM1_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM1_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM1_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_TBL_DATA_MAP_DATA_ID, 
    "map_data", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.map_data:\n\r"
    "  uint32 map_data\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem1_4b_1st_pass_map_index_table_tbl_set\n\r"
    "  entry_offset_ndx 0 map_data 0 map_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM1_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM1_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM1_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM1_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_TBL_DATA_MAP_INDEX_ID, 
    "map_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.map_index:\n\r"
    "  uint32 map_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem1_4b_1st_pass_map_index_table_tbl_set\n\r"
    "  entry_offset_ndx 0 map_data 0 map_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM1_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM1_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM1_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_GET_IHB_FEM1_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_GET_ID, 
    "fem1_4b_1st_pass_map_index_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem1_4b_1st_pass_map_index_table_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM1_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_GET_IHB_FEM1_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM1_4B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM1_4B_1ST_PASS_MAP_TABLE_TBL_SET_ID, 
    "fem1_4b_1st_pass_map_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem1_4b_1st_pass_map_table_tbl_set entry_offset_ndx 0\n\r"
    "  offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM1_4B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM1_4B_1ST_PASS_MAP_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM1_4B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM1_4B_1ST_PASS_MAP_TABLE_TBL_SET_TBL_DATA_OFFSET_INDEX_ID, 
    "offset_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.offset_index:\n\r"
    "  uint32 offset_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem1_4b_1st_pass_map_table_tbl_set entry_offset_ndx 0\n\r"
    "  offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM1_4B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM1_4B_1ST_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM1_4B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM1_4B_1ST_PASS_MAP_TABLE_TBL_SET_TBL_DATA_FIELD_SELECT_MAP_INDEX_ID, 
    "field_select_map_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  field_select_map_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem1_4b_1st_pass_map_table_tbl_set entry_offset_ndx 0\n\r"
    "  offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM1_4B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM1_4B_1ST_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM1_4B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM1_4B_1ST_PASS_MAP_TABLE_TBL_SET_TBL_DATA_FIELD_SELECT_MAP_ID, 
    "field_select_map", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.field_select_map[ prm_field_select_map_index]:\n\r"
    "  uint32 field_select_map[SOC_PB_IHB_FEM_MAX_OUTPUT_SIZE]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem1_4b_1st_pass_map_table_tbl_set entry_offset_ndx 0\n\r"
    "  offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM1_4B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM1_4B_1ST_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM1_4B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM1_4B_1ST_PASS_MAP_TABLE_TBL_SET_TBL_DATA_ACTION_TYPE_ID, 
    "action_type", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.action_type:\n\r"
    "  uint32 action_type\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem1_4b_1st_pass_map_table_tbl_set entry_offset_ndx 0\n\r"
    "  offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM1_4B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM1_4B_1ST_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM1_4B_1ST_PASS_MAP_TABLE_TBL_GET_IHB_FEM1_4B_1ST_PASS_MAP_TABLE_TBL_GET_ID, 
    "fem1_4b_1st_pass_map_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem1_4b_1st_pass_map_table_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM1_4B_1ST_PASS_MAP_TABLE_TBL_GET_IHB_FEM1_4B_1ST_PASS_MAP_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM2_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM2_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID, 
    "fem2_14b_1st_pass_key_profile_resolved_data_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem2_14b_1st_pass_key_profile_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 key_select 0 program 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM2_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM2_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM2_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM2_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_TBL_DATA_KEY_SELECT_ID, 
    "key_select", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.key_select:\n\r"
    "  uint32 key_select\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem2_14b_1st_pass_key_profile_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 key_select 0 program 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM2_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM2_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM2_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM2_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_TBL_DATA_PROGRAM_ID, 
    "program", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.program:\n\r"
    "  uint32 program\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem2_14b_1st_pass_key_profile_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 key_select 0 program 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM2_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM2_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM2_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_IHB_FEM2_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_ID, 
    "fem2_14b_1st_pass_key_profile_resolved_data_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem2_14b_1st_pass_key_profile_resolved_data_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM2_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_IHB_FEM2_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM2_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM2_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_ID, 
    "fem2_14b_1st_pass_program_resolved_data_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem2_14b_1st_pass_program_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 bit_select 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM2_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM2_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM2_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM2_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_TBL_DATA_BIT_SELECT_ID, 
    "bit_select", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.bit_select:\n\r"
    "  uint32 bit_select\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem2_14b_1st_pass_program_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 bit_select 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM2_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM2_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM2_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_IHB_FEM2_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_ID, 
    "fem2_14b_1st_pass_program_resolved_data_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem2_14b_1st_pass_program_resolved_data_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM2_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_IHB_FEM2_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM2_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM2_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_ID, 
    "fem2_14b_1st_pass_map_index_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem2_14b_1st_pass_map_index_table_tbl_set\n\r"
    "  entry_offset_ndx 0 map_data 0 map_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM2_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM2_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM2_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM2_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_TBL_DATA_MAP_DATA_ID, 
    "map_data", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.map_data:\n\r"
    "  uint32 map_data\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem2_14b_1st_pass_map_index_table_tbl_set\n\r"
    "  entry_offset_ndx 0 map_data 0 map_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM2_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM2_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM2_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM2_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_TBL_DATA_MAP_INDEX_ID, 
    "map_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.map_index:\n\r"
    "  uint32 map_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem2_14b_1st_pass_map_index_table_tbl_set\n\r"
    "  entry_offset_ndx 0 map_data 0 map_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM2_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM2_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM2_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_GET_IHB_FEM2_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_GET_ID, 
    "fem2_14b_1st_pass_map_index_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem2_14b_1st_pass_map_index_table_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM2_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_GET_IHB_FEM2_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM2_14B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM2_14B_1ST_PASS_MAP_TABLE_TBL_SET_ID, 
    "fem2_14b_1st_pass_map_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem2_14b_1st_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM2_14B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM2_14B_1ST_PASS_MAP_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM2_14B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM2_14B_1ST_PASS_MAP_TABLE_TBL_SET_TBL_DATA_OFFSET_INDEX_ID, 
    "offset_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.offset_index:\n\r"
    "  uint32 offset_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem2_14b_1st_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM2_14B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM2_14B_1ST_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM2_14B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM2_14B_1ST_PASS_MAP_TABLE_TBL_SET_TBL_DATA_FIELD_SELECT_MAP_INDEX_ID, 
    "field_select_map_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  field_select_map_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem2_14b_1st_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM2_14B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM2_14B_1ST_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM2_14B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM2_14B_1ST_PASS_MAP_TABLE_TBL_SET_TBL_DATA_FIELD_SELECT_MAP_ID, 
    "field_select_map", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.field_select_map[ prm_field_select_map_index]:\n\r"
    "  uint32 field_select_map[SOC_PB_IHB_FEM_MAX_OUTPUT_SIZE]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem2_14b_1st_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM2_14B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM2_14B_1ST_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM2_14B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM2_14B_1ST_PASS_MAP_TABLE_TBL_SET_TBL_DATA_ACTION_TYPE_ID, 
    "action_type", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.action_type:\n\r"
    "  uint32 action_type\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem2_14b_1st_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM2_14B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM2_14B_1ST_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM2_14B_1ST_PASS_MAP_TABLE_TBL_GET_IHB_FEM2_14B_1ST_PASS_MAP_TABLE_TBL_GET_ID, 
    "fem2_14b_1st_pass_map_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem2_14b_1st_pass_map_table_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM2_14B_1ST_PASS_MAP_TABLE_TBL_GET_IHB_FEM2_14B_1ST_PASS_MAP_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM2_14B_1ST_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM2_14B_1ST_PASS_OFFSET_TABLE_TBL_SET_ID, 
    "fem2_14b_1st_pass_offset_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem2_14b_1st_pass_offset_table_tbl_set\n\r"
    "  entry_offset_ndx 0 offset 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM2_14B_1ST_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM2_14B_1ST_PASS_OFFSET_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM2_14B_1ST_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM2_14B_1ST_PASS_OFFSET_TABLE_TBL_SET_TBL_DATA_OFFSET_ID, 
    "offset", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.offset:\n\r"
    "  uint32 offset\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem2_14b_1st_pass_offset_table_tbl_set\n\r"
    "  entry_offset_ndx 0 offset 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM2_14B_1ST_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM2_14B_1ST_PASS_OFFSET_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM2_14B_1ST_PASS_OFFSET_TABLE_TBL_GET_IHB_FEM2_14B_1ST_PASS_OFFSET_TABLE_TBL_GET_ID, 
    "fem2_14b_1st_pass_offset_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem2_14b_1st_pass_offset_table_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM2_14B_1ST_PASS_OFFSET_TABLE_TBL_GET_IHB_FEM2_14B_1ST_PASS_OFFSET_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM3_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM3_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID, 
    "fem3_14b_1st_pass_key_profile_resolved_data_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem3_14b_1st_pass_key_profile_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 key_select 0 program 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM3_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM3_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM3_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM3_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_TBL_DATA_KEY_SELECT_ID, 
    "key_select", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.key_select:\n\r"
    "  uint32 key_select\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem3_14b_1st_pass_key_profile_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 key_select 0 program 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM3_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM3_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM3_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM3_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_TBL_DATA_PROGRAM_ID, 
    "program", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.program:\n\r"
    "  uint32 program\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem3_14b_1st_pass_key_profile_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 key_select 0 program 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM3_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM3_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM3_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_IHB_FEM3_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_ID, 
    "fem3_14b_1st_pass_key_profile_resolved_data_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem3_14b_1st_pass_key_profile_resolved_data_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM3_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_IHB_FEM3_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM3_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM3_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_ID, 
    "fem3_14b_1st_pass_program_resolved_data_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem3_14b_1st_pass_program_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 bit_select 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM3_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM3_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM3_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM3_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_TBL_DATA_BIT_SELECT_ID, 
    "bit_select", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.bit_select:\n\r"
    "  uint32 bit_select\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem3_14b_1st_pass_program_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 bit_select 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM3_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM3_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM3_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_IHB_FEM3_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_ID, 
    "fem3_14b_1st_pass_program_resolved_data_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem3_14b_1st_pass_program_resolved_data_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM3_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_IHB_FEM3_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM3_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM3_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_ID, 
    "fem3_14b_1st_pass_map_index_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem3_14b_1st_pass_map_index_table_tbl_set\n\r"
    "  entry_offset_ndx 0 map_data 0 map_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM3_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM3_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM3_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM3_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_TBL_DATA_MAP_DATA_ID, 
    "map_data", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.map_data:\n\r"
    "  uint32 map_data\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem3_14b_1st_pass_map_index_table_tbl_set\n\r"
    "  entry_offset_ndx 0 map_data 0 map_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM3_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM3_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM3_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM3_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_TBL_DATA_MAP_INDEX_ID, 
    "map_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.map_index:\n\r"
    "  uint32 map_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem3_14b_1st_pass_map_index_table_tbl_set\n\r"
    "  entry_offset_ndx 0 map_data 0 map_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM3_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM3_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM3_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_GET_IHB_FEM3_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_GET_ID, 
    "fem3_14b_1st_pass_map_index_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem3_14b_1st_pass_map_index_table_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM3_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_GET_IHB_FEM3_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM3_14B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM3_14B_1ST_PASS_MAP_TABLE_TBL_SET_ID, 
    "fem3_14b_1st_pass_map_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem3_14b_1st_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM3_14B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM3_14B_1ST_PASS_MAP_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM3_14B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM3_14B_1ST_PASS_MAP_TABLE_TBL_SET_TBL_DATA_OFFSET_INDEX_ID, 
    "offset_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.offset_index:\n\r"
    "  uint32 offset_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem3_14b_1st_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM3_14B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM3_14B_1ST_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM3_14B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM3_14B_1ST_PASS_MAP_TABLE_TBL_SET_TBL_DATA_FIELD_SELECT_MAP_INDEX_ID, 
    "field_select_map_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  field_select_map_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem3_14b_1st_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM3_14B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM3_14B_1ST_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM3_14B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM3_14B_1ST_PASS_MAP_TABLE_TBL_SET_TBL_DATA_FIELD_SELECT_MAP_ID, 
    "field_select_map", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.field_select_map[ prm_field_select_map_index]:\n\r"
    "  uint32 field_select_map[SOC_PB_IHB_FEM_MAX_OUTPUT_SIZE]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem3_14b_1st_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM3_14B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM3_14B_1ST_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM3_14B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM3_14B_1ST_PASS_MAP_TABLE_TBL_SET_TBL_DATA_ACTION_TYPE_ID, 
    "action_type", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.action_type:\n\r"
    "  uint32 action_type\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem3_14b_1st_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM3_14B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM3_14B_1ST_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM3_14B_1ST_PASS_MAP_TABLE_TBL_GET_IHB_FEM3_14B_1ST_PASS_MAP_TABLE_TBL_GET_ID, 
    "fem3_14b_1st_pass_map_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem3_14b_1st_pass_map_table_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM3_14B_1ST_PASS_MAP_TABLE_TBL_GET_IHB_FEM3_14B_1ST_PASS_MAP_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM3_14B_1ST_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM3_14B_1ST_PASS_OFFSET_TABLE_TBL_SET_ID, 
    "fem3_14b_1st_pass_offset_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem3_14b_1st_pass_offset_table_tbl_set\n\r"
    "  entry_offset_ndx 0 offset 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM3_14B_1ST_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM3_14B_1ST_PASS_OFFSET_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM3_14B_1ST_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM3_14B_1ST_PASS_OFFSET_TABLE_TBL_SET_TBL_DATA_OFFSET_ID, 
    "offset", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.offset:\n\r"
    "  uint32 offset\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem3_14b_1st_pass_offset_table_tbl_set\n\r"
    "  entry_offset_ndx 0 offset 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM3_14B_1ST_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM3_14B_1ST_PASS_OFFSET_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM3_14B_1ST_PASS_OFFSET_TABLE_TBL_GET_IHB_FEM3_14B_1ST_PASS_OFFSET_TABLE_TBL_GET_ID, 
    "fem3_14b_1st_pass_offset_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem3_14b_1st_pass_offset_table_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM3_14B_1ST_PASS_OFFSET_TABLE_TBL_GET_IHB_FEM3_14B_1ST_PASS_OFFSET_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM4_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM4_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID, 
    "fem4_14b_1st_pass_key_profile_resolved_data_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem4_14b_1st_pass_key_profile_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 key_select 0 program 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM4_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM4_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM4_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM4_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_TBL_DATA_KEY_SELECT_ID, 
    "key_select", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.key_select:\n\r"
    "  uint32 key_select\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem4_14b_1st_pass_key_profile_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 key_select 0 program 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM4_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM4_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM4_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM4_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_TBL_DATA_PROGRAM_ID, 
    "program", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.program:\n\r"
    "  uint32 program\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem4_14b_1st_pass_key_profile_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 key_select 0 program 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM4_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM4_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM4_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_IHB_FEM4_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_ID, 
    "fem4_14b_1st_pass_key_profile_resolved_data_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem4_14b_1st_pass_key_profile_resolved_data_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM4_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_IHB_FEM4_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM4_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM4_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_ID, 
    "fem4_14b_1st_pass_program_resolved_data_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem4_14b_1st_pass_program_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 bit_select 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM4_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM4_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM4_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM4_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_TBL_DATA_BIT_SELECT_ID, 
    "bit_select", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.bit_select:\n\r"
    "  uint32 bit_select\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem4_14b_1st_pass_program_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 bit_select 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM4_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM4_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM4_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_IHB_FEM4_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_ID, 
    "fem4_14b_1st_pass_program_resolved_data_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem4_14b_1st_pass_program_resolved_data_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM4_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_IHB_FEM4_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM4_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM4_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_ID, 
    "fem4_14b_1st_pass_map_index_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem4_14b_1st_pass_map_index_table_tbl_set\n\r"
    "  entry_offset_ndx 0 map_data 0 map_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM4_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM4_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM4_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM4_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_TBL_DATA_MAP_DATA_ID, 
    "map_data", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.map_data:\n\r"
    "  uint32 map_data\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem4_14b_1st_pass_map_index_table_tbl_set\n\r"
    "  entry_offset_ndx 0 map_data 0 map_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM4_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM4_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM4_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM4_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_TBL_DATA_MAP_INDEX_ID, 
    "map_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.map_index:\n\r"
    "  uint32 map_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem4_14b_1st_pass_map_index_table_tbl_set\n\r"
    "  entry_offset_ndx 0 map_data 0 map_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM4_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM4_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM4_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_GET_IHB_FEM4_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_GET_ID, 
    "fem4_14b_1st_pass_map_index_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem4_14b_1st_pass_map_index_table_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM4_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_GET_IHB_FEM4_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM4_14B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM4_14B_1ST_PASS_MAP_TABLE_TBL_SET_ID, 
    "fem4_14b_1st_pass_map_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem4_14b_1st_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM4_14B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM4_14B_1ST_PASS_MAP_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM4_14B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM4_14B_1ST_PASS_MAP_TABLE_TBL_SET_TBL_DATA_OFFSET_INDEX_ID, 
    "offset_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.offset_index:\n\r"
    "  uint32 offset_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem4_14b_1st_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM4_14B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM4_14B_1ST_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM4_14B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM4_14B_1ST_PASS_MAP_TABLE_TBL_SET_TBL_DATA_FIELD_SELECT_MAP_INDEX_ID, 
    "field_select_map_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  field_select_map_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem4_14b_1st_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM4_14B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM4_14B_1ST_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM4_14B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM4_14B_1ST_PASS_MAP_TABLE_TBL_SET_TBL_DATA_FIELD_SELECT_MAP_ID, 
    "field_select_map", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.field_select_map[ prm_field_select_map_index]:\n\r"
    "  uint32 field_select_map[SOC_PB_IHB_FEM_MAX_OUTPUT_SIZE]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem4_14b_1st_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM4_14B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM4_14B_1ST_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM4_14B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM4_14B_1ST_PASS_MAP_TABLE_TBL_SET_TBL_DATA_ACTION_TYPE_ID, 
    "action_type", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.action_type:\n\r"
    "  uint32 action_type\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem4_14b_1st_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM4_14B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM4_14B_1ST_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM4_14B_1ST_PASS_MAP_TABLE_TBL_GET_IHB_FEM4_14B_1ST_PASS_MAP_TABLE_TBL_GET_ID, 
    "fem4_14b_1st_pass_map_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem4_14b_1st_pass_map_table_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM4_14B_1ST_PASS_MAP_TABLE_TBL_GET_IHB_FEM4_14B_1ST_PASS_MAP_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM4_14B_1ST_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM4_14B_1ST_PASS_OFFSET_TABLE_TBL_SET_ID, 
    "fem4_14b_1st_pass_offset_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem4_14b_1st_pass_offset_table_tbl_set\n\r"
    "  entry_offset_ndx 0 offset 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM4_14B_1ST_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM4_14B_1ST_PASS_OFFSET_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM4_14B_1ST_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM4_14B_1ST_PASS_OFFSET_TABLE_TBL_SET_TBL_DATA_OFFSET_ID, 
    "offset", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.offset:\n\r"
    "  uint32 offset\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem4_14b_1st_pass_offset_table_tbl_set\n\r"
    "  entry_offset_ndx 0 offset 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM4_14B_1ST_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM4_14B_1ST_PASS_OFFSET_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM4_14B_1ST_PASS_OFFSET_TABLE_TBL_GET_IHB_FEM4_14B_1ST_PASS_OFFSET_TABLE_TBL_GET_ID, 
    "fem4_14b_1st_pass_offset_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem4_14b_1st_pass_offset_table_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM4_14B_1ST_PASS_OFFSET_TABLE_TBL_GET_IHB_FEM4_14B_1ST_PASS_OFFSET_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM5_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM5_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID, 
    "fem5_17b_1st_pass_key_profile_resolved_data_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem5_17b_1st_pass_key_profile_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 key_select 0 program 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM5_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM5_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM5_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM5_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_TBL_DATA_KEY_SELECT_ID, 
    "key_select", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.key_select:\n\r"
    "  uint32 key_select\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem5_17b_1st_pass_key_profile_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 key_select 0 program 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM5_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM5_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM5_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM5_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_TBL_DATA_PROGRAM_ID, 
    "program", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.program:\n\r"
    "  uint32 program\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem5_17b_1st_pass_key_profile_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 key_select 0 program 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM5_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM5_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM5_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_IHB_FEM5_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_ID, 
    "fem5_17b_1st_pass_key_profile_resolved_data_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem5_17b_1st_pass_key_profile_resolved_data_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM5_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_IHB_FEM5_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM5_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM5_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_ID, 
    "fem5_17b_1st_pass_program_resolved_data_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem5_17b_1st_pass_program_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 bit_select 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM5_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM5_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM5_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM5_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_TBL_DATA_BIT_SELECT_ID, 
    "bit_select", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.bit_select:\n\r"
    "  uint32 bit_select\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem5_17b_1st_pass_program_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 bit_select 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM5_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM5_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM5_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_IHB_FEM5_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_ID, 
    "fem5_17b_1st_pass_program_resolved_data_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem5_17b_1st_pass_program_resolved_data_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM5_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_IHB_FEM5_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM5_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM5_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_ID, 
    "fem5_17b_1st_pass_map_index_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem5_17b_1st_pass_map_index_table_tbl_set\n\r"
    "  entry_offset_ndx 0 map_data 0 map_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM5_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM5_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM5_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM5_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_TBL_DATA_MAP_DATA_ID, 
    "map_data", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.map_data:\n\r"
    "  uint32 map_data\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem5_17b_1st_pass_map_index_table_tbl_set\n\r"
    "  entry_offset_ndx 0 map_data 0 map_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM5_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM5_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM5_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM5_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_TBL_DATA_MAP_INDEX_ID, 
    "map_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.map_index:\n\r"
    "  uint32 map_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem5_17b_1st_pass_map_index_table_tbl_set\n\r"
    "  entry_offset_ndx 0 map_data 0 map_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM5_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM5_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM5_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_GET_IHB_FEM5_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_GET_ID, 
    "fem5_17b_1st_pass_map_index_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem5_17b_1st_pass_map_index_table_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM5_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_GET_IHB_FEM5_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM5_17B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM5_17B_1ST_PASS_MAP_TABLE_TBL_SET_ID, 
    "fem5_17b_1st_pass_map_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem5_17b_1st_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM5_17B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM5_17B_1ST_PASS_MAP_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM5_17B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM5_17B_1ST_PASS_MAP_TABLE_TBL_SET_TBL_DATA_OFFSET_INDEX_ID, 
    "offset_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.offset_index:\n\r"
    "  uint32 offset_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem5_17b_1st_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM5_17B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM5_17B_1ST_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM5_17B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM5_17B_1ST_PASS_MAP_TABLE_TBL_SET_TBL_DATA_FIELD_SELECT_MAP_INDEX_ID, 
    "field_select_map_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  field_select_map_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem5_17b_1st_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM5_17B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM5_17B_1ST_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM5_17B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM5_17B_1ST_PASS_MAP_TABLE_TBL_SET_TBL_DATA_FIELD_SELECT_MAP_ID, 
    "field_select_map", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.field_select_map[ prm_field_select_map_index]:\n\r"
    "  uint32 field_select_map[SOC_PB_IHB_FEM_MAX_OUTPUT_SIZE]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem5_17b_1st_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM5_17B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM5_17B_1ST_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM5_17B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM5_17B_1ST_PASS_MAP_TABLE_TBL_SET_TBL_DATA_ACTION_TYPE_ID, 
    "action_type", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.action_type:\n\r"
    "  uint32 action_type\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem5_17b_1st_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM5_17B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM5_17B_1ST_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM5_17B_1ST_PASS_MAP_TABLE_TBL_GET_IHB_FEM5_17B_1ST_PASS_MAP_TABLE_TBL_GET_ID, 
    "fem5_17b_1st_pass_map_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem5_17b_1st_pass_map_table_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM5_17B_1ST_PASS_MAP_TABLE_TBL_GET_IHB_FEM5_17B_1ST_PASS_MAP_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM5_17B_1ST_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM5_17B_1ST_PASS_OFFSET_TABLE_TBL_SET_ID, 
    "fem5_17b_1st_pass_offset_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem5_17b_1st_pass_offset_table_tbl_set\n\r"
    "  entry_offset_ndx 0 offset 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM5_17B_1ST_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM5_17B_1ST_PASS_OFFSET_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM5_17B_1ST_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM5_17B_1ST_PASS_OFFSET_TABLE_TBL_SET_TBL_DATA_OFFSET_ID, 
    "offset", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.offset:\n\r"
    "  uint32 offset\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem5_17b_1st_pass_offset_table_tbl_set\n\r"
    "  entry_offset_ndx 0 offset 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM5_17B_1ST_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM5_17B_1ST_PASS_OFFSET_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM5_17B_1ST_PASS_OFFSET_TABLE_TBL_GET_IHB_FEM5_17B_1ST_PASS_OFFSET_TABLE_TBL_GET_ID, 
    "fem5_17b_1st_pass_offset_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem5_17b_1st_pass_offset_table_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM5_17B_1ST_PASS_OFFSET_TABLE_TBL_GET_IHB_FEM5_17B_1ST_PASS_OFFSET_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM6_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM6_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID, 
    "fem6_17b_1st_pass_key_profile_resolved_data_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem6_17b_1st_pass_key_profile_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 key_select 0 program 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM6_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM6_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM6_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM6_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_TBL_DATA_KEY_SELECT_ID, 
    "key_select", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.key_select:\n\r"
    "  uint32 key_select\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem6_17b_1st_pass_key_profile_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 key_select 0 program 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM6_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM6_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM6_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM6_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_TBL_DATA_PROGRAM_ID, 
    "program", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.program:\n\r"
    "  uint32 program\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem6_17b_1st_pass_key_profile_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 key_select 0 program 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM6_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM6_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM6_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_IHB_FEM6_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_ID, 
    "fem6_17b_1st_pass_key_profile_resolved_data_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem6_17b_1st_pass_key_profile_resolved_data_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM6_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_IHB_FEM6_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM6_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM6_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_ID, 
    "fem6_17b_1st_pass_program_resolved_data_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem6_17b_1st_pass_program_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 bit_select 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM6_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM6_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM6_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM6_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_TBL_DATA_BIT_SELECT_ID, 
    "bit_select", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.bit_select:\n\r"
    "  uint32 bit_select\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem6_17b_1st_pass_program_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 bit_select 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM6_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM6_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM6_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_IHB_FEM6_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_ID, 
    "fem6_17b_1st_pass_program_resolved_data_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem6_17b_1st_pass_program_resolved_data_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM6_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_IHB_FEM6_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM6_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM6_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_ID, 
    "fem6_17b_1st_pass_map_index_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem6_17b_1st_pass_map_index_table_tbl_set\n\r"
    "  entry_offset_ndx 0 map_data 0 map_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM6_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM6_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM6_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM6_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_TBL_DATA_MAP_DATA_ID, 
    "map_data", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.map_data:\n\r"
    "  uint32 map_data\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem6_17b_1st_pass_map_index_table_tbl_set\n\r"
    "  entry_offset_ndx 0 map_data 0 map_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM6_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM6_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM6_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM6_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_TBL_DATA_MAP_INDEX_ID, 
    "map_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.map_index:\n\r"
    "  uint32 map_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem6_17b_1st_pass_map_index_table_tbl_set\n\r"
    "  entry_offset_ndx 0 map_data 0 map_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM6_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM6_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM6_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_GET_IHB_FEM6_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_GET_ID, 
    "fem6_17b_1st_pass_map_index_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem6_17b_1st_pass_map_index_table_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM6_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_GET_IHB_FEM6_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM6_17B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM6_17B_1ST_PASS_MAP_TABLE_TBL_SET_ID, 
    "fem6_17b_1st_pass_map_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem6_17b_1st_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM6_17B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM6_17B_1ST_PASS_MAP_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM6_17B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM6_17B_1ST_PASS_MAP_TABLE_TBL_SET_TBL_DATA_OFFSET_INDEX_ID, 
    "offset_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.offset_index:\n\r"
    "  uint32 offset_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem6_17b_1st_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM6_17B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM6_17B_1ST_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM6_17B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM6_17B_1ST_PASS_MAP_TABLE_TBL_SET_TBL_DATA_FIELD_SELECT_MAP_INDEX_ID, 
    "field_select_map_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  field_select_map_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem6_17b_1st_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM6_17B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM6_17B_1ST_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM6_17B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM6_17B_1ST_PASS_MAP_TABLE_TBL_SET_TBL_DATA_FIELD_SELECT_MAP_ID, 
    "field_select_map", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.field_select_map[ prm_field_select_map_index]:\n\r"
    "  uint32 field_select_map[SOC_PB_IHB_FEM_MAX_OUTPUT_SIZE]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem6_17b_1st_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM6_17B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM6_17B_1ST_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM6_17B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM6_17B_1ST_PASS_MAP_TABLE_TBL_SET_TBL_DATA_ACTION_TYPE_ID, 
    "action_type", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.action_type:\n\r"
    "  uint32 action_type\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem6_17b_1st_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM6_17B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM6_17B_1ST_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM6_17B_1ST_PASS_MAP_TABLE_TBL_GET_IHB_FEM6_17B_1ST_PASS_MAP_TABLE_TBL_GET_ID, 
    "fem6_17b_1st_pass_map_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem6_17b_1st_pass_map_table_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM6_17B_1ST_PASS_MAP_TABLE_TBL_GET_IHB_FEM6_17B_1ST_PASS_MAP_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM6_17B_1ST_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM6_17B_1ST_PASS_OFFSET_TABLE_TBL_SET_ID, 
    "fem6_17b_1st_pass_offset_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem6_17b_1st_pass_offset_table_tbl_set\n\r"
    "  entry_offset_ndx 0 offset 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM6_17B_1ST_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM6_17B_1ST_PASS_OFFSET_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM6_17B_1ST_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM6_17B_1ST_PASS_OFFSET_TABLE_TBL_SET_TBL_DATA_OFFSET_ID, 
    "offset", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.offset:\n\r"
    "  uint32 offset\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem6_17b_1st_pass_offset_table_tbl_set\n\r"
    "  entry_offset_ndx 0 offset 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM6_17B_1ST_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM6_17B_1ST_PASS_OFFSET_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM6_17B_1ST_PASS_OFFSET_TABLE_TBL_GET_IHB_FEM6_17B_1ST_PASS_OFFSET_TABLE_TBL_GET_ID, 
    "fem6_17b_1st_pass_offset_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem6_17b_1st_pass_offset_table_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM6_17B_1ST_PASS_OFFSET_TABLE_TBL_GET_IHB_FEM6_17B_1ST_PASS_OFFSET_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM7_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM7_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID, 
    "fem7_17b_1st_pass_key_profile_resolved_data_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem7_17b_1st_pass_key_profile_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 key_select 0 program 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM7_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM7_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM7_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM7_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_TBL_DATA_KEY_SELECT_ID, 
    "key_select", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.key_select:\n\r"
    "  uint32 key_select\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem7_17b_1st_pass_key_profile_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 key_select 0 program 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM7_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM7_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM7_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM7_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_TBL_DATA_PROGRAM_ID, 
    "program", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.program:\n\r"
    "  uint32 program\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem7_17b_1st_pass_key_profile_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 key_select 0 program 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM7_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM7_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM7_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_IHB_FEM7_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_ID, 
    "fem7_17b_1st_pass_key_profile_resolved_data_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem7_17b_1st_pass_key_profile_resolved_data_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM7_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_IHB_FEM7_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM7_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM7_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_ID, 
    "fem7_17b_1st_pass_program_resolved_data_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem7_17b_1st_pass_program_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 bit_select 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM7_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM7_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM7_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM7_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_TBL_DATA_BIT_SELECT_ID, 
    "bit_select", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.bit_select:\n\r"
    "  uint32 bit_select\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem7_17b_1st_pass_program_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 bit_select 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM7_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM7_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM7_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_IHB_FEM7_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_ID, 
    "fem7_17b_1st_pass_program_resolved_data_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem7_17b_1st_pass_program_resolved_data_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM7_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_IHB_FEM7_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM7_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM7_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_ID, 
    "fem7_17b_1st_pass_map_index_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem7_17b_1st_pass_map_index_table_tbl_set\n\r"
    "  entry_offset_ndx 0 map_data 0 map_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM7_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM7_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM7_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM7_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_TBL_DATA_MAP_DATA_ID, 
    "map_data", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.map_data:\n\r"
    "  uint32 map_data\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem7_17b_1st_pass_map_index_table_tbl_set\n\r"
    "  entry_offset_ndx 0 map_data 0 map_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM7_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM7_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM7_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM7_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_TBL_DATA_MAP_INDEX_ID, 
    "map_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.map_index:\n\r"
    "  uint32 map_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem7_17b_1st_pass_map_index_table_tbl_set\n\r"
    "  entry_offset_ndx 0 map_data 0 map_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM7_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM7_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM7_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_GET_IHB_FEM7_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_GET_ID, 
    "fem7_17b_1st_pass_map_index_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem7_17b_1st_pass_map_index_table_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM7_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_GET_IHB_FEM7_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM7_17B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM7_17B_1ST_PASS_MAP_TABLE_TBL_SET_ID, 
    "fem7_17b_1st_pass_map_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem7_17b_1st_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM7_17B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM7_17B_1ST_PASS_MAP_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM7_17B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM7_17B_1ST_PASS_MAP_TABLE_TBL_SET_TBL_DATA_OFFSET_INDEX_ID, 
    "offset_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.offset_index:\n\r"
    "  uint32 offset_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem7_17b_1st_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM7_17B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM7_17B_1ST_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM7_17B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM7_17B_1ST_PASS_MAP_TABLE_TBL_SET_TBL_DATA_FIELD_SELECT_MAP_INDEX_ID, 
    "field_select_map_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  field_select_map_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem7_17b_1st_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM7_17B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM7_17B_1ST_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM7_17B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM7_17B_1ST_PASS_MAP_TABLE_TBL_SET_TBL_DATA_FIELD_SELECT_MAP_ID, 
    "field_select_map", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.field_select_map[ prm_field_select_map_index]:\n\r"
    "  uint32 field_select_map[SOC_PB_IHB_FEM_MAX_OUTPUT_SIZE]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem7_17b_1st_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM7_17B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM7_17B_1ST_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM7_17B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM7_17B_1ST_PASS_MAP_TABLE_TBL_SET_TBL_DATA_ACTION_TYPE_ID, 
    "action_type", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.action_type:\n\r"
    "  uint32 action_type\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem7_17b_1st_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM7_17B_1ST_PASS_MAP_TABLE_TBL_SET_IHB_FEM7_17B_1ST_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM7_17B_1ST_PASS_MAP_TABLE_TBL_GET_IHB_FEM7_17B_1ST_PASS_MAP_TABLE_TBL_GET_ID, 
    "fem7_17b_1st_pass_map_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem7_17b_1st_pass_map_table_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM7_17B_1ST_PASS_MAP_TABLE_TBL_GET_IHB_FEM7_17B_1ST_PASS_MAP_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM7_17B_1ST_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM7_17B_1ST_PASS_OFFSET_TABLE_TBL_SET_ID, 
    "fem7_17b_1st_pass_offset_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem7_17b_1st_pass_offset_table_tbl_set\n\r"
    "  entry_offset_ndx 0 offset 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM7_17B_1ST_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM7_17B_1ST_PASS_OFFSET_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM7_17B_1ST_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM7_17B_1ST_PASS_OFFSET_TABLE_TBL_SET_TBL_DATA_OFFSET_ID, 
    "offset", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.offset:\n\r"
    "  uint32 offset\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem7_17b_1st_pass_offset_table_tbl_set\n\r"
    "  entry_offset_ndx 0 offset 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM7_17B_1ST_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM7_17B_1ST_PASS_OFFSET_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM7_17B_1ST_PASS_OFFSET_TABLE_TBL_GET_IHB_FEM7_17B_1ST_PASS_OFFSET_TABLE_TBL_GET_ID, 
    "fem7_17b_1st_pass_offset_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem7_17b_1st_pass_offset_table_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM7_17B_1ST_PASS_OFFSET_TABLE_TBL_GET_IHB_FEM7_17B_1ST_PASS_OFFSET_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM0_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM0_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID, 
    "fem0_4b_2nd_pass_key_profile_resolved_data_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem0_4b_2nd_pass_key_profile_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 key_select 0 program 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM0_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM0_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM0_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM0_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_TBL_DATA_KEY_SELECT_ID, 
    "key_select", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.key_select:\n\r"
    "  uint32 key_select\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem0_4b_2nd_pass_key_profile_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 key_select 0 program 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM0_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM0_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM0_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM0_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_TBL_DATA_PROGRAM_ID, 
    "program", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.program:\n\r"
    "  uint32 program\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem0_4b_2nd_pass_key_profile_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 key_select 0 program 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM0_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM0_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM0_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_IHB_FEM0_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_ID, 
    "fem0_4b_2nd_pass_key_profile_resolved_data_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem0_4b_2nd_pass_key_profile_resolved_data_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM0_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_IHB_FEM0_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM0_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM0_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_ID, 
    "fem0_4b_2nd_pass_program_resolved_data_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem0_4b_2nd_pass_program_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 bit_select 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM0_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM0_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM0_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM0_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_TBL_DATA_BIT_SELECT_ID, 
    "bit_select", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.bit_select:\n\r"
    "  uint32 bit_select\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem0_4b_2nd_pass_program_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 bit_select 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM0_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM0_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM0_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_IHB_FEM0_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_ID, 
    "fem0_4b_2nd_pass_program_resolved_data_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem0_4b_2nd_pass_program_resolved_data_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM0_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_IHB_FEM0_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM0_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM0_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_ID, 
    "fem0_4b_2nd_pass_map_index_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem0_4b_2nd_pass_map_index_table_tbl_set\n\r"
    "  entry_offset_ndx 0 map_data 0 map_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM0_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM0_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM0_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM0_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_TBL_DATA_MAP_DATA_ID, 
    "map_data", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.map_data:\n\r"
    "  uint32 map_data\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem0_4b_2nd_pass_map_index_table_tbl_set\n\r"
    "  entry_offset_ndx 0 map_data 0 map_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM0_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM0_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM0_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM0_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_TBL_DATA_MAP_INDEX_ID, 
    "map_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.map_index:\n\r"
    "  uint32 map_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem0_4b_2nd_pass_map_index_table_tbl_set\n\r"
    "  entry_offset_ndx 0 map_data 0 map_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM0_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM0_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM0_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_GET_IHB_FEM0_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_GET_ID, 
    "fem0_4b_2nd_pass_map_index_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem0_4b_2nd_pass_map_index_table_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM0_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_GET_IHB_FEM0_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM0_4B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM0_4B_2ND_PASS_MAP_TABLE_TBL_SET_ID, 
    "fem0_4b_2nd_pass_map_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem0_4b_2nd_pass_map_table_tbl_set entry_offset_ndx 0\n\r"
    "  offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM0_4B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM0_4B_2ND_PASS_MAP_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM0_4B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM0_4B_2ND_PASS_MAP_TABLE_TBL_SET_TBL_DATA_OFFSET_INDEX_ID, 
    "offset_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.offset_index:\n\r"
    "  uint32 offset_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem0_4b_2nd_pass_map_table_tbl_set entry_offset_ndx 0\n\r"
    "  offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM0_4B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM0_4B_2ND_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM0_4B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM0_4B_2ND_PASS_MAP_TABLE_TBL_SET_TBL_DATA_FIELD_SELECT_MAP_INDEX_ID, 
    "field_select_map_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  field_select_map_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem0_4b_2nd_pass_map_table_tbl_set entry_offset_ndx 0\n\r"
    "  offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM0_4B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM0_4B_2ND_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM0_4B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM0_4B_2ND_PASS_MAP_TABLE_TBL_SET_TBL_DATA_FIELD_SELECT_MAP_ID, 
    "field_select_map", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.field_select_map[ prm_field_select_map_index]:\n\r"
    "  uint32 field_select_map[SOC_PB_IHB_FEM_MAX_OUTPUT_SIZE]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem0_4b_2nd_pass_map_table_tbl_set entry_offset_ndx 0\n\r"
    "  offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM0_4B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM0_4B_2ND_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM0_4B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM0_4B_2ND_PASS_MAP_TABLE_TBL_SET_TBL_DATA_ACTION_TYPE_ID, 
    "action_type", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.action_type:\n\r"
    "  uint32 action_type\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem0_4b_2nd_pass_map_table_tbl_set entry_offset_ndx 0\n\r"
    "  offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM0_4B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM0_4B_2ND_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM0_4B_2ND_PASS_MAP_TABLE_TBL_GET_IHB_FEM0_4B_2ND_PASS_MAP_TABLE_TBL_GET_ID, 
    "fem0_4b_2nd_pass_map_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem0_4b_2nd_pass_map_table_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM0_4B_2ND_PASS_MAP_TABLE_TBL_GET_IHB_FEM0_4B_2ND_PASS_MAP_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM1_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM1_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID, 
    "fem1_4b_2nd_pass_key_profile_resolved_data_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem1_4b_2nd_pass_key_profile_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 key_select 0 program 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM1_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM1_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM1_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM1_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_TBL_DATA_KEY_SELECT_ID, 
    "key_select", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.key_select:\n\r"
    "  uint32 key_select\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem1_4b_2nd_pass_key_profile_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 key_select 0 program 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM1_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM1_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM1_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM1_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_TBL_DATA_PROGRAM_ID, 
    "program", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.program:\n\r"
    "  uint32 program\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem1_4b_2nd_pass_key_profile_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 key_select 0 program 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM1_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM1_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM1_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_IHB_FEM1_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_ID, 
    "fem1_4b_2nd_pass_key_profile_resolved_data_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem1_4b_2nd_pass_key_profile_resolved_data_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM1_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_IHB_FEM1_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM1_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM1_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_ID, 
    "fem1_4b_2nd_pass_program_resolved_data_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem1_4b_2nd_pass_program_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 bit_select 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM1_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM1_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM1_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM1_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_TBL_DATA_BIT_SELECT_ID, 
    "bit_select", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.bit_select:\n\r"
    "  uint32 bit_select\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem1_4b_2nd_pass_program_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 bit_select 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM1_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM1_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM1_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_IHB_FEM1_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_ID, 
    "fem1_4b_2nd_pass_program_resolved_data_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem1_4b_2nd_pass_program_resolved_data_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM1_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_IHB_FEM1_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM1_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM1_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_ID, 
    "fem1_4b_2nd_pass_map_index_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem1_4b_2nd_pass_map_index_table_tbl_set\n\r"
    "  entry_offset_ndx 0 map_data 0 map_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM1_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM1_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM1_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM1_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_TBL_DATA_MAP_DATA_ID, 
    "map_data", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.map_data:\n\r"
    "  uint32 map_data\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem1_4b_2nd_pass_map_index_table_tbl_set\n\r"
    "  entry_offset_ndx 0 map_data 0 map_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM1_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM1_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM1_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM1_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_TBL_DATA_MAP_INDEX_ID, 
    "map_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.map_index:\n\r"
    "  uint32 map_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem1_4b_2nd_pass_map_index_table_tbl_set\n\r"
    "  entry_offset_ndx 0 map_data 0 map_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM1_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM1_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM1_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_GET_IHB_FEM1_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_GET_ID, 
    "fem1_4b_2nd_pass_map_index_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem1_4b_2nd_pass_map_index_table_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM1_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_GET_IHB_FEM1_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM1_4B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM1_4B_2ND_PASS_MAP_TABLE_TBL_SET_ID, 
    "fem1_4b_2nd_pass_map_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem1_4b_2nd_pass_map_table_tbl_set entry_offset_ndx 0\n\r"
    "  offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM1_4B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM1_4B_2ND_PASS_MAP_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM1_4B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM1_4B_2ND_PASS_MAP_TABLE_TBL_SET_TBL_DATA_OFFSET_INDEX_ID, 
    "offset_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.offset_index:\n\r"
    "  uint32 offset_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem1_4b_2nd_pass_map_table_tbl_set entry_offset_ndx 0\n\r"
    "  offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM1_4B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM1_4B_2ND_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM1_4B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM1_4B_2ND_PASS_MAP_TABLE_TBL_SET_TBL_DATA_FIELD_SELECT_MAP_INDEX_ID, 
    "field_select_map_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  field_select_map_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem1_4b_2nd_pass_map_table_tbl_set entry_offset_ndx 0\n\r"
    "  offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM1_4B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM1_4B_2ND_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM1_4B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM1_4B_2ND_PASS_MAP_TABLE_TBL_SET_TBL_DATA_FIELD_SELECT_MAP_ID, 
    "field_select_map", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.field_select_map[ prm_field_select_map_index]:\n\r"
    "  uint32 field_select_map[SOC_PB_IHB_FEM_MAX_OUTPUT_SIZE]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem1_4b_2nd_pass_map_table_tbl_set entry_offset_ndx 0\n\r"
    "  offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM1_4B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM1_4B_2ND_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM1_4B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM1_4B_2ND_PASS_MAP_TABLE_TBL_SET_TBL_DATA_ACTION_TYPE_ID, 
    "action_type", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.action_type:\n\r"
    "  uint32 action_type\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem1_4b_2nd_pass_map_table_tbl_set entry_offset_ndx 0\n\r"
    "  offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM1_4B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM1_4B_2ND_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM1_4B_2ND_PASS_MAP_TABLE_TBL_GET_IHB_FEM1_4B_2ND_PASS_MAP_TABLE_TBL_GET_ID, 
    "fem1_4b_2nd_pass_map_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem1_4b_2nd_pass_map_table_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM1_4B_2ND_PASS_MAP_TABLE_TBL_GET_IHB_FEM1_4B_2ND_PASS_MAP_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM2_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM2_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID, 
    "fem2_14b_2nd_pass_key_profile_resolved_data_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem2_14b_2nd_pass_key_profile_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 key_select 0 program 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM2_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM2_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM2_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM2_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_TBL_DATA_KEY_SELECT_ID, 
    "key_select", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.key_select:\n\r"
    "  uint32 key_select\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem2_14b_2nd_pass_key_profile_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 key_select 0 program 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM2_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM2_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM2_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM2_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_TBL_DATA_PROGRAM_ID, 
    "program", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.program:\n\r"
    "  uint32 program\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem2_14b_2nd_pass_key_profile_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 key_select 0 program 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM2_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM2_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM2_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_IHB_FEM2_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_ID, 
    "fem2_14b_2nd_pass_key_profile_resolved_data_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem2_14b_2nd_pass_key_profile_resolved_data_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM2_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_IHB_FEM2_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM2_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM2_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_ID, 
    "fem2_14b_2nd_pass_program_resolved_data_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem2_14b_2nd_pass_program_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 bit_select 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM2_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM2_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM2_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM2_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_TBL_DATA_BIT_SELECT_ID, 
    "bit_select", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.bit_select:\n\r"
    "  uint32 bit_select\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem2_14b_2nd_pass_program_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 bit_select 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM2_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM2_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM2_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_IHB_FEM2_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_ID, 
    "fem2_14b_2nd_pass_program_resolved_data_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem2_14b_2nd_pass_program_resolved_data_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM2_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_IHB_FEM2_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM2_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM2_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_ID, 
    "fem2_14b_2nd_pass_map_index_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem2_14b_2nd_pass_map_index_table_tbl_set\n\r"
    "  entry_offset_ndx 0 map_data 0 map_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM2_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM2_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM2_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM2_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_TBL_DATA_MAP_DATA_ID, 
    "map_data", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.map_data:\n\r"
    "  uint32 map_data\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem2_14b_2nd_pass_map_index_table_tbl_set\n\r"
    "  entry_offset_ndx 0 map_data 0 map_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM2_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM2_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM2_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM2_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_TBL_DATA_MAP_INDEX_ID, 
    "map_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.map_index:\n\r"
    "  uint32 map_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem2_14b_2nd_pass_map_index_table_tbl_set\n\r"
    "  entry_offset_ndx 0 map_data 0 map_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM2_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM2_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM2_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_GET_IHB_FEM2_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_GET_ID, 
    "fem2_14b_2nd_pass_map_index_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem2_14b_2nd_pass_map_index_table_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM2_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_GET_IHB_FEM2_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM2_14B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM2_14B_2ND_PASS_MAP_TABLE_TBL_SET_ID, 
    "fem2_14b_2nd_pass_map_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem2_14b_2nd_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM2_14B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM2_14B_2ND_PASS_MAP_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM2_14B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM2_14B_2ND_PASS_MAP_TABLE_TBL_SET_TBL_DATA_OFFSET_INDEX_ID, 
    "offset_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.offset_index:\n\r"
    "  uint32 offset_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem2_14b_2nd_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM2_14B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM2_14B_2ND_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM2_14B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM2_14B_2ND_PASS_MAP_TABLE_TBL_SET_TBL_DATA_FIELD_SELECT_MAP_INDEX_ID, 
    "field_select_map_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  field_select_map_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem2_14b_2nd_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM2_14B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM2_14B_2ND_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM2_14B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM2_14B_2ND_PASS_MAP_TABLE_TBL_SET_TBL_DATA_FIELD_SELECT_MAP_ID, 
    "field_select_map", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.field_select_map[ prm_field_select_map_index]:\n\r"
    "  uint32 field_select_map[SOC_PB_IHB_FEM_MAX_OUTPUT_SIZE]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem2_14b_2nd_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM2_14B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM2_14B_2ND_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM2_14B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM2_14B_2ND_PASS_MAP_TABLE_TBL_SET_TBL_DATA_ACTION_TYPE_ID, 
    "action_type", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.action_type:\n\r"
    "  uint32 action_type\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem2_14b_2nd_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM2_14B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM2_14B_2ND_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM2_14B_2ND_PASS_MAP_TABLE_TBL_GET_IHB_FEM2_14B_2ND_PASS_MAP_TABLE_TBL_GET_ID, 
    "fem2_14b_2nd_pass_map_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem2_14b_2nd_pass_map_table_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM2_14B_2ND_PASS_MAP_TABLE_TBL_GET_IHB_FEM2_14B_2ND_PASS_MAP_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM2_14B_2ND_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM2_14B_2ND_PASS_OFFSET_TABLE_TBL_SET_ID, 
    "fem2_14b_2nd_pass_offset_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem2_14b_2nd_pass_offset_table_tbl_set\n\r"
    "  entry_offset_ndx 0 offset 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM2_14B_2ND_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM2_14B_2ND_PASS_OFFSET_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM2_14B_2ND_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM2_14B_2ND_PASS_OFFSET_TABLE_TBL_SET_TBL_DATA_OFFSET_ID, 
    "offset", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.offset:\n\r"
    "  uint32 offset\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem2_14b_2nd_pass_offset_table_tbl_set\n\r"
    "  entry_offset_ndx 0 offset 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM2_14B_2ND_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM2_14B_2ND_PASS_OFFSET_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM2_14B_2ND_PASS_OFFSET_TABLE_TBL_GET_IHB_FEM2_14B_2ND_PASS_OFFSET_TABLE_TBL_GET_ID, 
    "fem2_14b_2nd_pass_offset_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem2_14b_2nd_pass_offset_table_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM2_14B_2ND_PASS_OFFSET_TABLE_TBL_GET_IHB_FEM2_14B_2ND_PASS_OFFSET_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM3_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM3_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID, 
    "fem3_14b_2nd_pass_key_profile_resolved_data_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem3_14b_2nd_pass_key_profile_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 key_select 0 program 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM3_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM3_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM3_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM3_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_TBL_DATA_KEY_SELECT_ID, 
    "key_select", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.key_select:\n\r"
    "  uint32 key_select\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem3_14b_2nd_pass_key_profile_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 key_select 0 program 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM3_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM3_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM3_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM3_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_TBL_DATA_PROGRAM_ID, 
    "program", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.program:\n\r"
    "  uint32 program\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem3_14b_2nd_pass_key_profile_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 key_select 0 program 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM3_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM3_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM3_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_IHB_FEM3_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_ID, 
    "fem3_14b_2nd_pass_key_profile_resolved_data_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem3_14b_2nd_pass_key_profile_resolved_data_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM3_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_IHB_FEM3_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM3_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM3_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_ID, 
    "fem3_14b_2nd_pass_program_resolved_data_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem3_14b_2nd_pass_program_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 bit_select 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM3_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM3_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM3_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM3_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_TBL_DATA_BIT_SELECT_ID, 
    "bit_select", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.bit_select:\n\r"
    "  uint32 bit_select\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem3_14b_2nd_pass_program_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 bit_select 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM3_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM3_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM3_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_IHB_FEM3_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_ID, 
    "fem3_14b_2nd_pass_program_resolved_data_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem3_14b_2nd_pass_program_resolved_data_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM3_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_IHB_FEM3_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM3_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM3_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_ID, 
    "fem3_14b_2nd_pass_map_index_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem3_14b_2nd_pass_map_index_table_tbl_set\n\r"
    "  entry_offset_ndx 0 map_data 0 map_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM3_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM3_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM3_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM3_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_TBL_DATA_MAP_DATA_ID, 
    "map_data", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.map_data:\n\r"
    "  uint32 map_data\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem3_14b_2nd_pass_map_index_table_tbl_set\n\r"
    "  entry_offset_ndx 0 map_data 0 map_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM3_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM3_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM3_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM3_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_TBL_DATA_MAP_INDEX_ID, 
    "map_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.map_index:\n\r"
    "  uint32 map_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem3_14b_2nd_pass_map_index_table_tbl_set\n\r"
    "  entry_offset_ndx 0 map_data 0 map_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM3_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM3_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM3_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_GET_IHB_FEM3_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_GET_ID, 
    "fem3_14b_2nd_pass_map_index_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem3_14b_2nd_pass_map_index_table_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM3_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_GET_IHB_FEM3_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM3_14B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM3_14B_2ND_PASS_MAP_TABLE_TBL_SET_ID, 
    "fem3_14b_2nd_pass_map_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem3_14b_2nd_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM3_14B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM3_14B_2ND_PASS_MAP_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM3_14B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM3_14B_2ND_PASS_MAP_TABLE_TBL_SET_TBL_DATA_OFFSET_INDEX_ID, 
    "offset_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.offset_index:\n\r"
    "  uint32 offset_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem3_14b_2nd_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM3_14B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM3_14B_2ND_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM3_14B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM3_14B_2ND_PASS_MAP_TABLE_TBL_SET_TBL_DATA_FIELD_SELECT_MAP_INDEX_ID, 
    "field_select_map_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  field_select_map_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem3_14b_2nd_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM3_14B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM3_14B_2ND_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM3_14B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM3_14B_2ND_PASS_MAP_TABLE_TBL_SET_TBL_DATA_FIELD_SELECT_MAP_ID, 
    "field_select_map", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.field_select_map[ prm_field_select_map_index]:\n\r"
    "  uint32 field_select_map[SOC_PB_IHB_FEM_MAX_OUTPUT_SIZE]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem3_14b_2nd_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM3_14B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM3_14B_2ND_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM3_14B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM3_14B_2ND_PASS_MAP_TABLE_TBL_SET_TBL_DATA_ACTION_TYPE_ID, 
    "action_type", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.action_type:\n\r"
    "  uint32 action_type\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem3_14b_2nd_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM3_14B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM3_14B_2ND_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM3_14B_2ND_PASS_MAP_TABLE_TBL_GET_IHB_FEM3_14B_2ND_PASS_MAP_TABLE_TBL_GET_ID, 
    "fem3_14b_2nd_pass_map_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem3_14b_2nd_pass_map_table_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM3_14B_2ND_PASS_MAP_TABLE_TBL_GET_IHB_FEM3_14B_2ND_PASS_MAP_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM3_14B_2ND_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM3_14B_2ND_PASS_OFFSET_TABLE_TBL_SET_ID, 
    "fem3_14b_2nd_pass_offset_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem3_14b_2nd_pass_offset_table_tbl_set\n\r"
    "  entry_offset_ndx 0 offset 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM3_14B_2ND_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM3_14B_2ND_PASS_OFFSET_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM3_14B_2ND_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM3_14B_2ND_PASS_OFFSET_TABLE_TBL_SET_TBL_DATA_OFFSET_ID, 
    "offset", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.offset:\n\r"
    "  uint32 offset\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem3_14b_2nd_pass_offset_table_tbl_set\n\r"
    "  entry_offset_ndx 0 offset 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM3_14B_2ND_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM3_14B_2ND_PASS_OFFSET_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM3_14B_2ND_PASS_OFFSET_TABLE_TBL_GET_IHB_FEM3_14B_2ND_PASS_OFFSET_TABLE_TBL_GET_ID, 
    "fem3_14b_2nd_pass_offset_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem3_14b_2nd_pass_offset_table_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM3_14B_2ND_PASS_OFFSET_TABLE_TBL_GET_IHB_FEM3_14B_2ND_PASS_OFFSET_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM4_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM4_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID, 
    "fem4_14b_2nd_pass_key_profile_resolved_data_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem4_14b_2nd_pass_key_profile_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 key_select 0 program 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM4_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM4_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM4_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM4_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_TBL_DATA_KEY_SELECT_ID, 
    "key_select", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.key_select:\n\r"
    "  uint32 key_select\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem4_14b_2nd_pass_key_profile_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 key_select 0 program 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM4_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM4_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM4_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM4_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_TBL_DATA_PROGRAM_ID, 
    "program", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.program:\n\r"
    "  uint32 program\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem4_14b_2nd_pass_key_profile_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 key_select 0 program 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM4_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM4_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM4_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_IHB_FEM4_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_ID, 
    "fem4_14b_2nd_pass_key_profile_resolved_data_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem4_14b_2nd_pass_key_profile_resolved_data_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM4_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_IHB_FEM4_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM4_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM4_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_ID, 
    "fem4_14b_2nd_pass_program_resolved_data_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem4_14b_2nd_pass_program_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 bit_select 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM4_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM4_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM4_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM4_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_TBL_DATA_BIT_SELECT_ID, 
    "bit_select", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.bit_select:\n\r"
    "  uint32 bit_select\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem4_14b_2nd_pass_program_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 bit_select 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM4_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM4_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM4_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_IHB_FEM4_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_ID, 
    "fem4_14b_2nd_pass_program_resolved_data_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem4_14b_2nd_pass_program_resolved_data_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM4_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_IHB_FEM4_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM4_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM4_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_ID, 
    "fem4_14b_2nd_pass_map_index_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem4_14b_2nd_pass_map_index_table_tbl_set\n\r"
    "  entry_offset_ndx 0 map_data 0 map_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM4_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM4_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM4_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM4_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_TBL_DATA_MAP_DATA_ID, 
    "map_data", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.map_data:\n\r"
    "  uint32 map_data\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem4_14b_2nd_pass_map_index_table_tbl_set\n\r"
    "  entry_offset_ndx 0 map_data 0 map_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM4_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM4_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM4_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM4_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_TBL_DATA_MAP_INDEX_ID, 
    "map_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.map_index:\n\r"
    "  uint32 map_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem4_14b_2nd_pass_map_index_table_tbl_set\n\r"
    "  entry_offset_ndx 0 map_data 0 map_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM4_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM4_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM4_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_GET_IHB_FEM4_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_GET_ID, 
    "fem4_14b_2nd_pass_map_index_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem4_14b_2nd_pass_map_index_table_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM4_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_GET_IHB_FEM4_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM4_14B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM4_14B_2ND_PASS_MAP_TABLE_TBL_SET_ID, 
    "fem4_14b_2nd_pass_map_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem4_14b_2nd_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM4_14B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM4_14B_2ND_PASS_MAP_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM4_14B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM4_14B_2ND_PASS_MAP_TABLE_TBL_SET_TBL_DATA_OFFSET_INDEX_ID, 
    "offset_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.offset_index:\n\r"
    "  uint32 offset_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem4_14b_2nd_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM4_14B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM4_14B_2ND_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM4_14B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM4_14B_2ND_PASS_MAP_TABLE_TBL_SET_TBL_DATA_FIELD_SELECT_MAP_INDEX_ID, 
    "field_select_map_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  field_select_map_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem4_14b_2nd_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM4_14B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM4_14B_2ND_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM4_14B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM4_14B_2ND_PASS_MAP_TABLE_TBL_SET_TBL_DATA_FIELD_SELECT_MAP_ID, 
    "field_select_map", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.field_select_map[ prm_field_select_map_index]:\n\r"
    "  uint32 field_select_map[SOC_PB_IHB_FEM_MAX_OUTPUT_SIZE]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem4_14b_2nd_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM4_14B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM4_14B_2ND_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM4_14B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM4_14B_2ND_PASS_MAP_TABLE_TBL_SET_TBL_DATA_ACTION_TYPE_ID, 
    "action_type", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.action_type:\n\r"
    "  uint32 action_type\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem4_14b_2nd_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM4_14B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM4_14B_2ND_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM4_14B_2ND_PASS_MAP_TABLE_TBL_GET_IHB_FEM4_14B_2ND_PASS_MAP_TABLE_TBL_GET_ID, 
    "fem4_14b_2nd_pass_map_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem4_14b_2nd_pass_map_table_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM4_14B_2ND_PASS_MAP_TABLE_TBL_GET_IHB_FEM4_14B_2ND_PASS_MAP_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM4_14B_2ND_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM4_14B_2ND_PASS_OFFSET_TABLE_TBL_SET_ID, 
    "fem4_14b_2nd_pass_offset_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem4_14b_2nd_pass_offset_table_tbl_set\n\r"
    "  entry_offset_ndx 0 offset 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM4_14B_2ND_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM4_14B_2ND_PASS_OFFSET_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM4_14B_2ND_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM4_14B_2ND_PASS_OFFSET_TABLE_TBL_SET_TBL_DATA_OFFSET_ID, 
    "offset", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.offset:\n\r"
    "  uint32 offset\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem4_14b_2nd_pass_offset_table_tbl_set\n\r"
    "  entry_offset_ndx 0 offset 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM4_14B_2ND_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM4_14B_2ND_PASS_OFFSET_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM4_14B_2ND_PASS_OFFSET_TABLE_TBL_GET_IHB_FEM4_14B_2ND_PASS_OFFSET_TABLE_TBL_GET_ID, 
    "fem4_14b_2nd_pass_offset_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem4_14b_2nd_pass_offset_table_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM4_14B_2ND_PASS_OFFSET_TABLE_TBL_GET_IHB_FEM4_14B_2ND_PASS_OFFSET_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM5_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM5_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID, 
    "fem5_17b_2nd_pass_key_profile_resolved_data_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem5_17b_2nd_pass_key_profile_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 key_select 0 program 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM5_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM5_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM5_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM5_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_TBL_DATA_KEY_SELECT_ID, 
    "key_select", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.key_select:\n\r"
    "  uint32 key_select\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem5_17b_2nd_pass_key_profile_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 key_select 0 program 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM5_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM5_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM5_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM5_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_TBL_DATA_PROGRAM_ID, 
    "program", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.program:\n\r"
    "  uint32 program\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem5_17b_2nd_pass_key_profile_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 key_select 0 program 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM5_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM5_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM5_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_IHB_FEM5_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_ID, 
    "fem5_17b_2nd_pass_key_profile_resolved_data_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem5_17b_2nd_pass_key_profile_resolved_data_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM5_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_IHB_FEM5_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM5_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM5_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_ID, 
    "fem5_17b_2nd_pass_program_resolved_data_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem5_17b_2nd_pass_program_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 bit_select 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM5_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM5_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM5_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM5_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_TBL_DATA_BIT_SELECT_ID, 
    "bit_select", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.bit_select:\n\r"
    "  uint32 bit_select\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem5_17b_2nd_pass_program_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 bit_select 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM5_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM5_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM5_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_IHB_FEM5_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_ID, 
    "fem5_17b_2nd_pass_program_resolved_data_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem5_17b_2nd_pass_program_resolved_data_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM5_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_IHB_FEM5_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM5_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM5_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_ID, 
    "fem5_17b_2nd_pass_map_index_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem5_17b_2nd_pass_map_index_table_tbl_set\n\r"
    "  entry_offset_ndx 0 map_data 0 map_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM5_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM5_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM5_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM5_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_TBL_DATA_MAP_DATA_ID, 
    "map_data", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.map_data:\n\r"
    "  uint32 map_data\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem5_17b_2nd_pass_map_index_table_tbl_set\n\r"
    "  entry_offset_ndx 0 map_data 0 map_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM5_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM5_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM5_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM5_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_TBL_DATA_MAP_INDEX_ID, 
    "map_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.map_index:\n\r"
    "  uint32 map_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem5_17b_2nd_pass_map_index_table_tbl_set\n\r"
    "  entry_offset_ndx 0 map_data 0 map_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM5_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM5_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM5_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_GET_IHB_FEM5_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_GET_ID, 
    "fem5_17b_2nd_pass_map_index_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem5_17b_2nd_pass_map_index_table_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM5_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_GET_IHB_FEM5_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM5_17B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM5_17B_2ND_PASS_MAP_TABLE_TBL_SET_ID, 
    "fem5_17b_2nd_pass_map_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem5_17b_2nd_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM5_17B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM5_17B_2ND_PASS_MAP_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM5_17B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM5_17B_2ND_PASS_MAP_TABLE_TBL_SET_TBL_DATA_OFFSET_INDEX_ID, 
    "offset_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.offset_index:\n\r"
    "  uint32 offset_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem5_17b_2nd_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM5_17B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM5_17B_2ND_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM5_17B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM5_17B_2ND_PASS_MAP_TABLE_TBL_SET_TBL_DATA_FIELD_SELECT_MAP_INDEX_ID, 
    "field_select_map_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  field_select_map_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem5_17b_2nd_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM5_17B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM5_17B_2ND_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM5_17B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM5_17B_2ND_PASS_MAP_TABLE_TBL_SET_TBL_DATA_FIELD_SELECT_MAP_ID, 
    "field_select_map", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.field_select_map[ prm_field_select_map_index]:\n\r"
    "  uint32 field_select_map[SOC_PB_IHB_FEM_MAX_OUTPUT_SIZE]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem5_17b_2nd_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM5_17B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM5_17B_2ND_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM5_17B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM5_17B_2ND_PASS_MAP_TABLE_TBL_SET_TBL_DATA_ACTION_TYPE_ID, 
    "action_type", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.action_type:\n\r"
    "  uint32 action_type\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem5_17b_2nd_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM5_17B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM5_17B_2ND_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM5_17B_2ND_PASS_MAP_TABLE_TBL_GET_IHB_FEM5_17B_2ND_PASS_MAP_TABLE_TBL_GET_ID, 
    "fem5_17b_2nd_pass_map_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem5_17b_2nd_pass_map_table_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM5_17B_2ND_PASS_MAP_TABLE_TBL_GET_IHB_FEM5_17B_2ND_PASS_MAP_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM5_17B_2ND_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM5_17B_2ND_PASS_OFFSET_TABLE_TBL_SET_ID, 
    "fem5_17b_2nd_pass_offset_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem5_17b_2nd_pass_offset_table_tbl_set\n\r"
    "  entry_offset_ndx 0 offset 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM5_17B_2ND_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM5_17B_2ND_PASS_OFFSET_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM5_17B_2ND_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM5_17B_2ND_PASS_OFFSET_TABLE_TBL_SET_TBL_DATA_OFFSET_ID, 
    "offset", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.offset:\n\r"
    "  uint32 offset\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem5_17b_2nd_pass_offset_table_tbl_set\n\r"
    "  entry_offset_ndx 0 offset 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM5_17B_2ND_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM5_17B_2ND_PASS_OFFSET_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM5_17B_2ND_PASS_OFFSET_TABLE_TBL_GET_IHB_FEM5_17B_2ND_PASS_OFFSET_TABLE_TBL_GET_ID, 
    "fem5_17b_2nd_pass_offset_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem5_17b_2nd_pass_offset_table_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM5_17B_2ND_PASS_OFFSET_TABLE_TBL_GET_IHB_FEM5_17B_2ND_PASS_OFFSET_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM6_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM6_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID, 
    "fem6_17b_2nd_pass_key_profile_resolved_data_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem6_17b_2nd_pass_key_profile_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 key_select 0 program 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM6_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM6_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM6_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM6_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_TBL_DATA_KEY_SELECT_ID, 
    "key_select", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.key_select:\n\r"
    "  uint32 key_select\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem6_17b_2nd_pass_key_profile_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 key_select 0 program 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM6_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM6_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM6_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM6_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_TBL_DATA_PROGRAM_ID, 
    "program", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.program:\n\r"
    "  uint32 program\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem6_17b_2nd_pass_key_profile_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 key_select 0 program 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM6_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM6_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM6_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_IHB_FEM6_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_ID, 
    "fem6_17b_2nd_pass_key_profile_resolved_data_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem6_17b_2nd_pass_key_profile_resolved_data_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM6_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_IHB_FEM6_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM6_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM6_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_ID, 
    "fem6_17b_2nd_pass_program_resolved_data_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem6_17b_2nd_pass_program_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 bit_select 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM6_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM6_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM6_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM6_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_TBL_DATA_BIT_SELECT_ID, 
    "bit_select", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.bit_select:\n\r"
    "  uint32 bit_select\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem6_17b_2nd_pass_program_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 bit_select 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM6_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM6_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM6_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_IHB_FEM6_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_ID, 
    "fem6_17b_2nd_pass_program_resolved_data_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem6_17b_2nd_pass_program_resolved_data_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM6_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_IHB_FEM6_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM6_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM6_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_ID, 
    "fem6_17b_2nd_pass_map_index_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem6_17b_2nd_pass_map_index_table_tbl_set\n\r"
    "  entry_offset_ndx 0 map_data 0 map_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM6_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM6_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM6_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM6_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_TBL_DATA_MAP_DATA_ID, 
    "map_data", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.map_data:\n\r"
    "  uint32 map_data\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem6_17b_2nd_pass_map_index_table_tbl_set\n\r"
    "  entry_offset_ndx 0 map_data 0 map_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM6_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM6_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM6_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM6_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_TBL_DATA_MAP_INDEX_ID, 
    "map_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.map_index:\n\r"
    "  uint32 map_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem6_17b_2nd_pass_map_index_table_tbl_set\n\r"
    "  entry_offset_ndx 0 map_data 0 map_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM6_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM6_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM6_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_GET_IHB_FEM6_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_GET_ID, 
    "fem6_17b_2nd_pass_map_index_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem6_17b_2nd_pass_map_index_table_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM6_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_GET_IHB_FEM6_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM6_17B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM6_17B_2ND_PASS_MAP_TABLE_TBL_SET_ID, 
    "fem6_17b_2nd_pass_map_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem6_17b_2nd_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM6_17B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM6_17B_2ND_PASS_MAP_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM6_17B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM6_17B_2ND_PASS_MAP_TABLE_TBL_SET_TBL_DATA_OFFSET_INDEX_ID, 
    "offset_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.offset_index:\n\r"
    "  uint32 offset_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem6_17b_2nd_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM6_17B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM6_17B_2ND_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM6_17B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM6_17B_2ND_PASS_MAP_TABLE_TBL_SET_TBL_DATA_FIELD_SELECT_MAP_INDEX_ID, 
    "field_select_map_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  field_select_map_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem6_17b_2nd_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM6_17B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM6_17B_2ND_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM6_17B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM6_17B_2ND_PASS_MAP_TABLE_TBL_SET_TBL_DATA_FIELD_SELECT_MAP_ID, 
    "field_select_map", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.field_select_map[ prm_field_select_map_index]:\n\r"
    "  uint32 field_select_map[SOC_PB_IHB_FEM_MAX_OUTPUT_SIZE]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem6_17b_2nd_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM6_17B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM6_17B_2ND_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM6_17B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM6_17B_2ND_PASS_MAP_TABLE_TBL_SET_TBL_DATA_ACTION_TYPE_ID, 
    "action_type", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.action_type:\n\r"
    "  uint32 action_type\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem6_17b_2nd_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM6_17B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM6_17B_2ND_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM6_17B_2ND_PASS_MAP_TABLE_TBL_GET_IHB_FEM6_17B_2ND_PASS_MAP_TABLE_TBL_GET_ID, 
    "fem6_17b_2nd_pass_map_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem6_17b_2nd_pass_map_table_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM6_17B_2ND_PASS_MAP_TABLE_TBL_GET_IHB_FEM6_17B_2ND_PASS_MAP_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM6_17B_2ND_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM6_17B_2ND_PASS_OFFSET_TABLE_TBL_SET_ID, 
    "fem6_17b_2nd_pass_offset_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem6_17b_2nd_pass_offset_table_tbl_set\n\r"
    "  entry_offset_ndx 0 offset 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM6_17B_2ND_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM6_17B_2ND_PASS_OFFSET_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM6_17B_2ND_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM6_17B_2ND_PASS_OFFSET_TABLE_TBL_SET_TBL_DATA_OFFSET_ID, 
    "offset", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.offset:\n\r"
    "  uint32 offset\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem6_17b_2nd_pass_offset_table_tbl_set\n\r"
    "  entry_offset_ndx 0 offset 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM6_17B_2ND_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM6_17B_2ND_PASS_OFFSET_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM6_17B_2ND_PASS_OFFSET_TABLE_TBL_GET_IHB_FEM6_17B_2ND_PASS_OFFSET_TABLE_TBL_GET_ID, 
    "fem6_17b_2nd_pass_offset_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem6_17b_2nd_pass_offset_table_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM6_17B_2ND_PASS_OFFSET_TABLE_TBL_GET_IHB_FEM6_17B_2ND_PASS_OFFSET_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM7_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM7_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID, 
    "fem7_17b_2nd_pass_key_profile_resolved_data_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem7_17b_2nd_pass_key_profile_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 key_select 0 program 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM7_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM7_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM7_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM7_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_TBL_DATA_KEY_SELECT_ID, 
    "key_select", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.key_select:\n\r"
    "  uint32 key_select\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem7_17b_2nd_pass_key_profile_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 key_select 0 program 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM7_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM7_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM7_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM7_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_TBL_DATA_PROGRAM_ID, 
    "program", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.program:\n\r"
    "  uint32 program\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem7_17b_2nd_pass_key_profile_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 key_select 0 program 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM7_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_IHB_FEM7_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM7_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_IHB_FEM7_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_ID, 
    "fem7_17b_2nd_pass_key_profile_resolved_data_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem7_17b_2nd_pass_key_profile_resolved_data_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM7_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_IHB_FEM7_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM7_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM7_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_ID, 
    "fem7_17b_2nd_pass_program_resolved_data_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem7_17b_2nd_pass_program_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 bit_select 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM7_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM7_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM7_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM7_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_TBL_DATA_BIT_SELECT_ID, 
    "bit_select", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.bit_select:\n\r"
    "  uint32 bit_select\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem7_17b_2nd_pass_program_resolved_data_tbl_set\n\r"
    "  entry_offset_ndx 0 bit_select 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM7_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_IHB_FEM7_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM7_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_IHB_FEM7_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_ID, 
    "fem7_17b_2nd_pass_program_resolved_data_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem7_17b_2nd_pass_program_resolved_data_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM7_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_IHB_FEM7_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM7_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM7_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_ID, 
    "fem7_17b_2nd_pass_map_index_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem7_17b_2nd_pass_map_index_table_tbl_set\n\r"
    "  entry_offset_ndx 0 map_data 0 map_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM7_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM7_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM7_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM7_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_TBL_DATA_MAP_DATA_ID, 
    "map_data", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.map_data:\n\r"
    "  uint32 map_data\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem7_17b_2nd_pass_map_index_table_tbl_set\n\r"
    "  entry_offset_ndx 0 map_data 0 map_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM7_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM7_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM7_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM7_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_TBL_DATA_MAP_INDEX_ID, 
    "map_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.map_index:\n\r"
    "  uint32 map_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem7_17b_2nd_pass_map_index_table_tbl_set\n\r"
    "  entry_offset_ndx 0 map_data 0 map_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM7_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_IHB_FEM7_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM7_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_GET_IHB_FEM7_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_GET_ID, 
    "fem7_17b_2nd_pass_map_index_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem7_17b_2nd_pass_map_index_table_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM7_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_GET_IHB_FEM7_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM7_17B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM7_17B_2ND_PASS_MAP_TABLE_TBL_SET_ID, 
    "fem7_17b_2nd_pass_map_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem7_17b_2nd_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM7_17B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM7_17B_2ND_PASS_MAP_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM7_17B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM7_17B_2ND_PASS_MAP_TABLE_TBL_SET_TBL_DATA_OFFSET_INDEX_ID, 
    "offset_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.offset_index:\n\r"
    "  uint32 offset_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem7_17b_2nd_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM7_17B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM7_17B_2ND_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM7_17B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM7_17B_2ND_PASS_MAP_TABLE_TBL_SET_TBL_DATA_FIELD_SELECT_MAP_INDEX_ID, 
    "field_select_map_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  field_select_map_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem7_17b_2nd_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM7_17B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM7_17B_2ND_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM7_17B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM7_17B_2ND_PASS_MAP_TABLE_TBL_SET_TBL_DATA_FIELD_SELECT_MAP_ID, 
    "field_select_map", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.field_select_map[ prm_field_select_map_index]:\n\r"
    "  uint32 field_select_map[SOC_PB_IHB_FEM_MAX_OUTPUT_SIZE]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem7_17b_2nd_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM7_17B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM7_17B_2ND_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM7_17B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM7_17B_2ND_PASS_MAP_TABLE_TBL_SET_TBL_DATA_ACTION_TYPE_ID, 
    "action_type", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.action_type:\n\r"
    "  uint32 action_type\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem7_17b_2nd_pass_map_table_tbl_set entry_offset_ndx\n\r"
    "  0 offset_index 0 field_select_map_index 0 field_select_map 0 action_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM7_17B_2ND_PASS_MAP_TABLE_TBL_SET_IHB_FEM7_17B_2ND_PASS_MAP_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM7_17B_2ND_PASS_MAP_TABLE_TBL_GET_IHB_FEM7_17B_2ND_PASS_MAP_TABLE_TBL_GET_ID, 
    "fem7_17b_2nd_pass_map_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem7_17b_2nd_pass_map_table_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM7_17B_2ND_PASS_MAP_TABLE_TBL_GET_IHB_FEM7_17B_2ND_PASS_MAP_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM7_17B_2ND_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM7_17B_2ND_PASS_OFFSET_TABLE_TBL_SET_ID, 
    "fem7_17b_2nd_pass_offset_table_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem7_17b_2nd_pass_offset_table_tbl_set\n\r"
    "  entry_offset_ndx 0 offset 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM7_17B_2ND_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM7_17B_2ND_PASS_OFFSET_TABLE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM7_17B_2ND_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM7_17B_2ND_PASS_OFFSET_TABLE_TBL_SET_TBL_DATA_OFFSET_ID, 
    "offset", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.offset:\n\r"
    "  uint32 offset\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem7_17b_2nd_pass_offset_table_tbl_set\n\r"
    "  entry_offset_ndx 0 offset 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM7_17B_2ND_PASS_OFFSET_TABLE_TBL_SET_IHB_FEM7_17B_2ND_PASS_OFFSET_TABLE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IHB_FEM7_17B_2ND_PASS_OFFSET_TABLE_TBL_GET_IHB_FEM7_17B_2ND_PASS_OFFSET_TABLE_TBL_GET_ID, 
    "fem7_17b_2nd_pass_offset_table_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc ihb_fem7_17b_2nd_pass_offset_table_tbl_get\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_IHB_FEM7_17B_2ND_PASS_OFFSET_TABLE_TBL_GET_IHB_FEM7_17B_2ND_PASS_OFFSET_TABLE_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EPNI_EPE_PACKET_COUNTER_TBL_SET_EPNI_EPE_PACKET_COUNTER_TBL_SET_ID, 
    "epe_packet_counter_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc epni_epe_packet_counter_tbl_set entry_offset_ndx 0\n\r"
    "  epe_packet_counter_index 0 epe_packet_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EPNI_EPE_PACKET_COUNTER_TBL_SET_EPNI_EPE_PACKET_COUNTER_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EPNI_EPE_PACKET_COUNTER_TBL_SET_EPNI_EPE_PACKET_COUNTER_TBL_SET_TBL_DATA_EPE_PACKET_COUNTER_INDEX_ID, 
    "epe_packet_counter_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  epe_packet_counter_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc epni_epe_packet_counter_tbl_set entry_offset_ndx 0\n\r"
    "  epe_packet_counter_index 0 epe_packet_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EPNI_EPE_PACKET_COUNTER_TBL_SET_EPNI_EPE_PACKET_COUNTER_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EPNI_EPE_PACKET_COUNTER_TBL_SET_EPNI_EPE_PACKET_COUNTER_TBL_SET_TBL_DATA_EPE_PACKET_COUNTER_ID, 
    "epe_packet_counter", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.epe_packet_counter[ prm_epe_packet_counter_index]:\n\r"
    "  uint32 epe_packet_counter[2]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc epni_epe_packet_counter_tbl_set entry_offset_ndx 0\n\r"
    "  epe_packet_counter_index 0 epe_packet_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EPNI_EPE_PACKET_COUNTER_TBL_SET_EPNI_EPE_PACKET_COUNTER_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EPNI_EPE_PACKET_COUNTER_TBL_GET_EPNI_EPE_PACKET_COUNTER_TBL_GET_ID, 
    "epe_packet_counter_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc epni_epe_packet_counter_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EPNI_EPE_PACKET_COUNTER_TBL_GET_EPNI_EPE_PACKET_COUNTER_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EPNI_EPE_BYTES_COUNTER_TBL_SET_EPNI_EPE_BYTES_COUNTER_TBL_SET_ID, 
    "epe_bytes_counter_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc epni_epe_bytes_counter_tbl_set entry_offset_ndx 0\n\r"
    "  epe_bytes_counter_index 0 epe_bytes_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EPNI_EPE_BYTES_COUNTER_TBL_SET_EPNI_EPE_BYTES_COUNTER_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EPNI_EPE_BYTES_COUNTER_TBL_SET_EPNI_EPE_BYTES_COUNTER_TBL_SET_TBL_DATA_EPE_BYTES_COUNTER_INDEX_ID, 
    "epe_bytes_counter_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  epe_bytes_counter_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc epni_epe_bytes_counter_tbl_set entry_offset_ndx 0\n\r"
    "  epe_bytes_counter_index 0 epe_bytes_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EPNI_EPE_BYTES_COUNTER_TBL_SET_EPNI_EPE_BYTES_COUNTER_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EPNI_EPE_BYTES_COUNTER_TBL_SET_EPNI_EPE_BYTES_COUNTER_TBL_SET_TBL_DATA_EPE_BYTES_COUNTER_ID, 
    "epe_bytes_counter", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.epe_bytes_counter[ prm_epe_bytes_counter_index]:\n\r"
    "  uint32 epe_bytes_counter[2]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc epni_epe_bytes_counter_tbl_set entry_offset_ndx 0\n\r"
    "  epe_bytes_counter_index 0 epe_bytes_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EPNI_EPE_BYTES_COUNTER_TBL_SET_EPNI_EPE_BYTES_COUNTER_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EPNI_EPE_BYTES_COUNTER_TBL_GET_EPNI_EPE_BYTES_COUNTER_TBL_GET_ID, 
    "epe_bytes_counter_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc epni_epe_bytes_counter_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EPNI_EPE_BYTES_COUNTER_TBL_GET_EPNI_EPE_BYTES_COUNTER_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EPNI_COPY_ENGINE_PROGRAM_TBL_SET_EPNI_COPY_ENGINE_PROGRAM_TBL_SET_ID, 
    "copy_engine_program_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc epni_copy_engine_program_tbl_set tbl_ndx 0\n\r"
    "  entry_offset_ndx 0 inst_lfem_program_index 0 inst_lfem_program 0\n\r"
    "  inst_bit_count_index 0 inst_bit_count 0 inst_niblle_field_offset_index 0\n\r"
    "  inst_niblle_field_offset 0 inst_header_offset_select_index 0\n\r"
    "  inst_header_offset_select 0 inst_source_select_index 0 inst_source_select 0\n\r"
    "  inst_valid_index 0 inst_valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EPNI_COPY_ENGINE_PROGRAM_TBL_SET_EPNI_COPY_ENGINE_PROGRAM_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EPNI_COPY_ENGINE_PROGRAM_TBL_SET_EPNI_COPY_ENGINE_PROGRAM_TBL_SET_TBL_NDX_ID, 
    "key_tbl_ndx", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_ndx:\n\r"
    "  SOC_SAND_IN\n\r"
    "  uint32\n\r"
    "  tbl_ndx\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc epni_copy_engine_program_tbl_set tbl_ndx 0\n\r"
    "  entry_offset_ndx 0 inst_lfem_program_index 0 inst_lfem_program 0\n\r"
    "  inst_bit_count_index 0 inst_bit_count 0 inst_niblle_field_offset_index 0\n\r"
    "  inst_niblle_field_offset 0 inst_header_offset_select_index 0\n\r"
    "  inst_header_offset_select 0 inst_source_select_index 0 inst_source_select 0\n\r"
    "  inst_valid_index 0 inst_valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EPNI_COPY_ENGINE_PROGRAM_TBL_SET_EPNI_COPY_ENGINE_PROGRAM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EPNI_COPY_ENGINE_PROGRAM_TBL_SET_EPNI_COPY_ENGINE_PROGRAM_TBL_SET_TBL_DATA_INST_LFEM_PROGRAM_INDEX_ID, 
    "inst_lfem_program_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  inst_lfem_program_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc epni_copy_engine_program_tbl_set tbl_ndx 0\n\r"
    "  entry_offset_ndx 0 inst_lfem_program_index 0 inst_lfem_program 0\n\r"
    "  inst_bit_count_index 0 inst_bit_count 0 inst_niblle_field_offset_index 0\n\r"
    "  inst_niblle_field_offset 0 inst_header_offset_select_index 0\n\r"
    "  inst_header_offset_select 0 inst_source_select_index 0 inst_source_select 0\n\r"
    "  inst_valid_index 0 inst_valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EPNI_COPY_ENGINE_PROGRAM_TBL_SET_EPNI_COPY_ENGINE_PROGRAM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EPNI_COPY_ENGINE_PROGRAM_TBL_SET_EPNI_COPY_ENGINE_PROGRAM_TBL_SET_TBL_DATA_INST_LFEM_PROGRAM_ID, 
    "inst_lfem_program", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.inst_lfem_program[ prm_inst_lfem_program_index]:\n\r"
    "  uint32 inst_lfem_program[2]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc epni_copy_engine_program_tbl_set tbl_ndx 0\n\r"
    "  entry_offset_ndx 0 inst_lfem_program_index 0 inst_lfem_program 0\n\r"
    "  inst_bit_count_index 0 inst_bit_count 0 inst_niblle_field_offset_index 0\n\r"
    "  inst_niblle_field_offset 0 inst_header_offset_select_index 0\n\r"
    "  inst_header_offset_select 0 inst_source_select_index 0 inst_source_select 0\n\r"
    "  inst_valid_index 0 inst_valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EPNI_COPY_ENGINE_PROGRAM_TBL_SET_EPNI_COPY_ENGINE_PROGRAM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EPNI_COPY_ENGINE_PROGRAM_TBL_SET_EPNI_COPY_ENGINE_PROGRAM_TBL_SET_TBL_DATA_INST_BIT_COUNT_INDEX_ID, 
    "inst_bit_count_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  inst_bit_count_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc epni_copy_engine_program_tbl_set tbl_ndx 0\n\r"
    "  entry_offset_ndx 0 inst_lfem_program_index 0 inst_lfem_program 0\n\r"
    "  inst_bit_count_index 0 inst_bit_count 0 inst_niblle_field_offset_index 0\n\r"
    "  inst_niblle_field_offset 0 inst_header_offset_select_index 0\n\r"
    "  inst_header_offset_select 0 inst_source_select_index 0 inst_source_select 0\n\r"
    "  inst_valid_index 0 inst_valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EPNI_COPY_ENGINE_PROGRAM_TBL_SET_EPNI_COPY_ENGINE_PROGRAM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EPNI_COPY_ENGINE_PROGRAM_TBL_SET_EPNI_COPY_ENGINE_PROGRAM_TBL_SET_TBL_DATA_INST_BIT_COUNT_ID, 
    "inst_bit_count", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.inst_bit_count[ prm_inst_bit_count_index]:\n\r"
    "  uint32 inst_bit_count[2]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc epni_copy_engine_program_tbl_set tbl_ndx 0\n\r"
    "  entry_offset_ndx 0 inst_lfem_program_index 0 inst_lfem_program 0\n\r"
    "  inst_bit_count_index 0 inst_bit_count 0 inst_niblle_field_offset_index 0\n\r"
    "  inst_niblle_field_offset 0 inst_header_offset_select_index 0\n\r"
    "  inst_header_offset_select 0 inst_source_select_index 0 inst_source_select 0\n\r"
    "  inst_valid_index 0 inst_valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EPNI_COPY_ENGINE_PROGRAM_TBL_SET_EPNI_COPY_ENGINE_PROGRAM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EPNI_COPY_ENGINE_PROGRAM_TBL_SET_EPNI_COPY_ENGINE_PROGRAM_TBL_SET_TBL_DATA_INST_NIBLLE_FIELD_OFFSET_INDEX_ID, 
    "inst_niblle_field_offset_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  inst_niblle_field_offset_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc epni_copy_engine_program_tbl_set tbl_ndx 0\n\r"
    "  entry_offset_ndx 0 inst_lfem_program_index 0 inst_lfem_program 0\n\r"
    "  inst_bit_count_index 0 inst_bit_count 0 inst_niblle_field_offset_index 0\n\r"
    "  inst_niblle_field_offset 0 inst_header_offset_select_index 0\n\r"
    "  inst_header_offset_select 0 inst_source_select_index 0 inst_source_select 0\n\r"
    "  inst_valid_index 0 inst_valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EPNI_COPY_ENGINE_PROGRAM_TBL_SET_EPNI_COPY_ENGINE_PROGRAM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EPNI_COPY_ENGINE_PROGRAM_TBL_SET_EPNI_COPY_ENGINE_PROGRAM_TBL_SET_TBL_DATA_INST_NIBLLE_FIELD_OFFSET_ID, 
    "inst_niblle_field_offset", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.inst_niblle_field_offset[ prm_inst_niblle_field_offset_index]:\n\r"
    "  uint32 inst_niblle_field_offset[2]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc epni_copy_engine_program_tbl_set tbl_ndx 0\n\r"
    "  entry_offset_ndx 0 inst_lfem_program_index 0 inst_lfem_program 0\n\r"
    "  inst_bit_count_index 0 inst_bit_count 0 inst_niblle_field_offset_index 0\n\r"
    "  inst_niblle_field_offset 0 inst_header_offset_select_index 0\n\r"
    "  inst_header_offset_select 0 inst_source_select_index 0 inst_source_select 0\n\r"
    "  inst_valid_index 0 inst_valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EPNI_COPY_ENGINE_PROGRAM_TBL_SET_EPNI_COPY_ENGINE_PROGRAM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EPNI_COPY_ENGINE_PROGRAM_TBL_SET_EPNI_COPY_ENGINE_PROGRAM_TBL_SET_TBL_DATA_INST_HEADER_OFFSET_SELECT_INDEX_ID, 
    "inst_header_offset_select_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  inst_header_offset_select_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc epni_copy_engine_program_tbl_set tbl_ndx 0\n\r"
    "  entry_offset_ndx 0 inst_lfem_program_index 0 inst_lfem_program 0\n\r"
    "  inst_bit_count_index 0 inst_bit_count 0 inst_niblle_field_offset_index 0\n\r"
    "  inst_niblle_field_offset 0 inst_header_offset_select_index 0\n\r"
    "  inst_header_offset_select 0 inst_source_select_index 0 inst_source_select 0\n\r"
    "  inst_valid_index 0 inst_valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EPNI_COPY_ENGINE_PROGRAM_TBL_SET_EPNI_COPY_ENGINE_PROGRAM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EPNI_COPY_ENGINE_PROGRAM_TBL_SET_EPNI_COPY_ENGINE_PROGRAM_TBL_SET_TBL_DATA_INST_HEADER_OFFSET_SELECT_ID, 
    "inst_header_offset_select", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.inst_header_offset_select[ prm_inst_header_offset_select_index]:\n\r"
    "  uint32 inst_header_offset_select[2]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc epni_copy_engine_program_tbl_set tbl_ndx 0\n\r"
    "  entry_offset_ndx 0 inst_lfem_program_index 0 inst_lfem_program 0\n\r"
    "  inst_bit_count_index 0 inst_bit_count 0 inst_niblle_field_offset_index 0\n\r"
    "  inst_niblle_field_offset 0 inst_header_offset_select_index 0\n\r"
    "  inst_header_offset_select 0 inst_source_select_index 0 inst_source_select 0\n\r"
    "  inst_valid_index 0 inst_valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EPNI_COPY_ENGINE_PROGRAM_TBL_SET_EPNI_COPY_ENGINE_PROGRAM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EPNI_COPY_ENGINE_PROGRAM_TBL_SET_EPNI_COPY_ENGINE_PROGRAM_TBL_SET_TBL_DATA_INST_SOURCE_SELECT_INDEX_ID, 
    "inst_source_select_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  inst_source_select_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc epni_copy_engine_program_tbl_set tbl_ndx 0\n\r"
    "  entry_offset_ndx 0 inst_lfem_program_index 0 inst_lfem_program 0\n\r"
    "  inst_bit_count_index 0 inst_bit_count 0 inst_niblle_field_offset_index 0\n\r"
    "  inst_niblle_field_offset 0 inst_header_offset_select_index 0\n\r"
    "  inst_header_offset_select 0 inst_source_select_index 0 inst_source_select 0\n\r"
    "  inst_valid_index 0 inst_valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EPNI_COPY_ENGINE_PROGRAM_TBL_SET_EPNI_COPY_ENGINE_PROGRAM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EPNI_COPY_ENGINE_PROGRAM_TBL_SET_EPNI_COPY_ENGINE_PROGRAM_TBL_SET_TBL_DATA_INST_SOURCE_SELECT_ID, 
    "inst_source_select", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.inst_source_select[ prm_inst_source_select_index]:\n\r"
    "  uint32 inst_source_select[2]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc epni_copy_engine_program_tbl_set tbl_ndx 0\n\r"
    "  entry_offset_ndx 0 inst_lfem_program_index 0 inst_lfem_program 0\n\r"
    "  inst_bit_count_index 0 inst_bit_count 0 inst_niblle_field_offset_index 0\n\r"
    "  inst_niblle_field_offset 0 inst_header_offset_select_index 0\n\r"
    "  inst_header_offset_select 0 inst_source_select_index 0 inst_source_select 0\n\r"
    "  inst_valid_index 0 inst_valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EPNI_COPY_ENGINE_PROGRAM_TBL_SET_EPNI_COPY_ENGINE_PROGRAM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EPNI_COPY_ENGINE_PROGRAM_TBL_SET_EPNI_COPY_ENGINE_PROGRAM_TBL_SET_TBL_DATA_INST_VALID_INDEX_ID, 
    "inst_valid_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  inst_valid_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc epni_copy_engine_program_tbl_set tbl_ndx 0\n\r"
    "  entry_offset_ndx 0 inst_lfem_program_index 0 inst_lfem_program 0\n\r"
    "  inst_bit_count_index 0 inst_bit_count 0 inst_niblle_field_offset_index 0\n\r"
    "  inst_niblle_field_offset 0 inst_header_offset_select_index 0\n\r"
    "  inst_header_offset_select 0 inst_source_select_index 0 inst_source_select 0\n\r"
    "  inst_valid_index 0 inst_valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EPNI_COPY_ENGINE_PROGRAM_TBL_SET_EPNI_COPY_ENGINE_PROGRAM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EPNI_COPY_ENGINE_PROGRAM_TBL_SET_EPNI_COPY_ENGINE_PROGRAM_TBL_SET_TBL_DATA_INST_VALID_ID, 
    "inst_valid", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.inst_valid[ prm_inst_valid_index]:\n\r"
    "  uint32 inst_valid[2]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc epni_copy_engine_program_tbl_set tbl_ndx 0\n\r"
    "  entry_offset_ndx 0 inst_lfem_program_index 0 inst_lfem_program 0\n\r"
    "  inst_bit_count_index 0 inst_bit_count 0 inst_niblle_field_offset_index 0\n\r"
    "  inst_niblle_field_offset 0 inst_header_offset_select_index 0\n\r"
    "  inst_header_offset_select 0 inst_source_select_index 0 inst_source_select 0\n\r"
    "  inst_valid_index 0 inst_valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EPNI_COPY_ENGINE_PROGRAM_TBL_SET_EPNI_COPY_ENGINE_PROGRAM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EPNI_COPY_ENGINE_PROGRAM_TBL_GET_EPNI_COPY_ENGINE_PROGRAM_TBL_GET_ID, 
    "copy_engine_program_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc epni_copy_engine_program_tbl_get tbl_ndx 0\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EPNI_COPY_ENGINE_PROGRAM_TBL_GET_EPNI_COPY_ENGINE_PROGRAM_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EPNI_COPY_ENGINE_PROGRAM_TBL_GET_EPNI_COPY_ENGINE_PROGRAM_TBL_GET_TBL_NDX_ID, 
    "key_tbl_ndx", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_ndx:\n\r"
    "  SOC_SAND_IN\n\r"
    "  uint32\n\r"
    "  tbl_ndx\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc epni_copy_engine_program_tbl_get tbl_ndx 0\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EPNI_COPY_ENGINE_PROGRAM_TBL_GET_EPNI_COPY_ENGINE_PROGRAM_TBL_GET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EPNI_LFEM_FIELD_SELECT_MAP_TBL_SET_EPNI_LFEM_FIELD_SELECT_MAP_TBL_SET_ID, 
    "lfem_field_select_map_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc epni_lfem_field_select_map_tbl_set tbl_ndx 0\n\r"
    "  entry_offset_ndx 0 lfem_field_select_map_index 0 lfem_field_select_map 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EPNI_LFEM_FIELD_SELECT_MAP_TBL_SET_EPNI_LFEM_FIELD_SELECT_MAP_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EPNI_LFEM_FIELD_SELECT_MAP_TBL_SET_EPNI_LFEM_FIELD_SELECT_MAP_TBL_SET_TBL_NDX_ID, 
    "key_tbl_ndx", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_ndx:\n\r"
    "  SOC_SAND_IN\n\r"
    "  uint32\n\r"
    "  tbl_ndx\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc epni_lfem_field_select_map_tbl_set tbl_ndx 0\n\r"
    "  entry_offset_ndx 0 lfem_field_select_map_index 0 lfem_field_select_map 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EPNI_LFEM_FIELD_SELECT_MAP_TBL_SET_EPNI_LFEM_FIELD_SELECT_MAP_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EPNI_LFEM_FIELD_SELECT_MAP_TBL_SET_EPNI_LFEM_FIELD_SELECT_MAP_TBL_SET_TBL_DATA_LFEM_FIELD_SELECT_MAP_INDEX_ID, 
    "lfem_field_select_map_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  lfem_field_select_map_index:\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc epni_lfem_field_select_map_tbl_set tbl_ndx 0\n\r"
    "  entry_offset_ndx 0 lfem_field_select_map_index 0 lfem_field_select_map 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EPNI_LFEM_FIELD_SELECT_MAP_TBL_SET_EPNI_LFEM_FIELD_SELECT_MAP_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EPNI_LFEM_FIELD_SELECT_MAP_TBL_SET_EPNI_LFEM_FIELD_SELECT_MAP_TBL_SET_TBL_DATA_LFEM_FIELD_SELECT_MAP_ID, 
    "lfem_field_select_map", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.lfem_field_select_map[ prm_lfem_field_select_map_index]:\n\r"
    "  uint32 lfem_field_select_map[16]\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc epni_lfem_field_select_map_tbl_set tbl_ndx 0\n\r"
    "  entry_offset_ndx 0 lfem_field_select_map_index 0 lfem_field_select_map 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EPNI_LFEM_FIELD_SELECT_MAP_TBL_SET_EPNI_LFEM_FIELD_SELECT_MAP_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EPNI_LFEM_FIELD_SELECT_MAP_TBL_GET_EPNI_LFEM_FIELD_SELECT_MAP_TBL_GET_ID, 
    "lfem_field_select_map_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc epni_lfem_field_select_map_tbl_get tbl_ndx 0\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EPNI_LFEM_FIELD_SELECT_MAP_TBL_GET_EPNI_LFEM_FIELD_SELECT_MAP_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_EPNI_LFEM_FIELD_SELECT_MAP_TBL_GET_EPNI_LFEM_FIELD_SELECT_MAP_TBL_GET_TBL_NDX_ID, 
    "key_tbl_ndx", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_ndx:\n\r"
    "  SOC_SAND_IN\n\r"
    "  uint32\n\r"
    "  tbl_ndx\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc epni_lfem_field_select_map_tbl_get tbl_ndx 0\n\r"
    "  entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_EPNI_LFEM_FIELD_SELECT_MAP_TBL_GET_EPNI_LFEM_FIELD_SELECT_MAP_TBL_GET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_RCL2_OFP_TBL_SET_CFC_RCL2_OFP_TBL_SET_ID, 
    "rcl2_ofp_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_rcl2_ofp_tbl_set entry_offset_ndx 0 egq_lp_valid 0\n\r"
    "  egq_hp_valid 0 sch_lp_valid 0 sch_hp_valid 0 ofp_num 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_RCL2_OFP_TBL_SET_CFC_RCL2_OFP_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_RCL2_OFP_TBL_SET_CFC_RCL2_OFP_TBL_SET_TBL_DATA_EGQ_LP_VALID_ID, 
    "lp_valid", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.egq_lp_valid:\n\r"
    "  uint32 egq_lp_valid\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_rcl2_ofp_tbl_set entry_offset_ndx 0 egq_lp_valid 0\n\r"
    "  egq_hp_valid 0 sch_lp_valid 0 sch_hp_valid 0 ofp_num 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_RCL2_OFP_TBL_SET_CFC_RCL2_OFP_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_RCL2_OFP_TBL_SET_CFC_RCL2_OFP_TBL_SET_TBL_DATA_EGQ_HP_VALID_ID, 
    "hp_valid", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.egq_hp_valid:\n\r"
    "  uint32 egq_hp_valid\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_rcl2_ofp_tbl_set entry_offset_ndx 0 egq_lp_valid 0\n\r"
    "  egq_hp_valid 0 sch_lp_valid 0 sch_hp_valid 0 ofp_num 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_RCL2_OFP_TBL_SET_CFC_RCL2_OFP_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_RCL2_OFP_TBL_SET_CFC_RCL2_OFP_TBL_SET_TBL_DATA_SCH_LP_VALID_ID, 
    "lp_valid", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.sch_lp_valid:\n\r"
    "  uint32 sch_lp_valid\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_rcl2_ofp_tbl_set entry_offset_ndx 0 egq_lp_valid 0\n\r"
    "  egq_hp_valid 0 sch_lp_valid 0 sch_hp_valid 0 ofp_num 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_RCL2_OFP_TBL_SET_CFC_RCL2_OFP_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_RCL2_OFP_TBL_SET_CFC_RCL2_OFP_TBL_SET_TBL_DATA_SCH_HP_VALID_ID, 
    "hp_valid", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.sch_hp_valid:\n\r"
    "  uint32 sch_hp_valid\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_rcl2_ofp_tbl_set entry_offset_ndx 0 egq_lp_valid 0\n\r"
    "  egq_hp_valid 0 sch_lp_valid 0 sch_hp_valid 0 ofp_num 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_RCL2_OFP_TBL_SET_CFC_RCL2_OFP_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_RCL2_OFP_TBL_SET_CFC_RCL2_OFP_TBL_SET_TBL_DATA_OFP_NUM_ID, 
    "ofp_num", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.ofp_num:\n\r"
    "  uint32 ofp_num\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_rcl2_ofp_tbl_set entry_offset_ndx 0 egq_lp_valid 0\n\r"
    "  egq_hp_valid 0 sch_lp_valid 0 sch_hp_valid 0 ofp_num 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_RCL2_OFP_TBL_SET_CFC_RCL2_OFP_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_RCL2_OFP_TBL_GET_CFC_RCL2_OFP_TBL_GET_ID, 
    "rcl2_ofp_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_rcl2_ofp_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_RCL2_OFP_TBL_GET_CFC_RCL2_OFP_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_NIFCLSB2_OFP_TBL_SET_CFC_NIFCLSB2_OFP_TBL_SET_ID, 
    "nifclsb2_ofp_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_nifclsb2_ofp_tbl_set entry_offset_ndx 0\n\r"
    "  egq_ofp_lp_valid3 0 egq_ofp_lp_valid2 0 egq_ofp_lp_valid1 0 egq_ofp_lp_valid0\n\r"
    "  0 egq_ofp_hp_valid3 0 egq_ofp_hp_valid2 0 egq_ofp_hp_valid1 0\n\r"
    "  egq_ofp_hp_valid0 0 sch_ofp_lp_valid3 0 sch_ofp_lp_valid2 0 sch_ofp_lp_valid1\n\r"
    "  0 sch_ofp_lp_valid0 0 sch_ofp_hp_valid3 0 sch_ofp_hp_valid2 0\n\r"
    "  sch_ofp_hp_valid1 0 sch_ofp_hp_valid0 0 ofp_num3 0 ofp_num2 0 ofp_num1 0\n\r"
    "  ofp_num0 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_NIFCLSB2_OFP_TBL_SET_CFC_NIFCLSB2_OFP_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_NIFCLSB2_OFP_TBL_SET_CFC_NIFCLSB2_OFP_TBL_SET_TBL_DATA_EGQ_OFP_LP_VALID3_ID, 
    "ofp_lp_valid3", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.egq_ofp_lp_valid3:\n\r"
    "  uint32 egq_ofp_lp_valid3\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_nifclsb2_ofp_tbl_set entry_offset_ndx 0\n\r"
    "  egq_ofp_lp_valid3 0 egq_ofp_lp_valid2 0 egq_ofp_lp_valid1 0 egq_ofp_lp_valid0\n\r"
    "  0 egq_ofp_hp_valid3 0 egq_ofp_hp_valid2 0 egq_ofp_hp_valid1 0\n\r"
    "  egq_ofp_hp_valid0 0 sch_ofp_lp_valid3 0 sch_ofp_lp_valid2 0 sch_ofp_lp_valid1\n\r"
    "  0 sch_ofp_lp_valid0 0 sch_ofp_hp_valid3 0 sch_ofp_hp_valid2 0\n\r"
    "  sch_ofp_hp_valid1 0 sch_ofp_hp_valid0 0 ofp_num3 0 ofp_num2 0 ofp_num1 0\n\r"
    "  ofp_num0 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_NIFCLSB2_OFP_TBL_SET_CFC_NIFCLSB2_OFP_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_NIFCLSB2_OFP_TBL_SET_CFC_NIFCLSB2_OFP_TBL_SET_TBL_DATA_EGQ_OFP_LP_VALID2_ID, 
    "ofp_lp_valid2", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.egq_ofp_lp_valid2:\n\r"
    "  uint32 egq_ofp_lp_valid2\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_nifclsb2_ofp_tbl_set entry_offset_ndx 0\n\r"
    "  egq_ofp_lp_valid3 0 egq_ofp_lp_valid2 0 egq_ofp_lp_valid1 0 egq_ofp_lp_valid0\n\r"
    "  0 egq_ofp_hp_valid3 0 egq_ofp_hp_valid2 0 egq_ofp_hp_valid1 0\n\r"
    "  egq_ofp_hp_valid0 0 sch_ofp_lp_valid3 0 sch_ofp_lp_valid2 0 sch_ofp_lp_valid1\n\r"
    "  0 sch_ofp_lp_valid0 0 sch_ofp_hp_valid3 0 sch_ofp_hp_valid2 0\n\r"
    "  sch_ofp_hp_valid1 0 sch_ofp_hp_valid0 0 ofp_num3 0 ofp_num2 0 ofp_num1 0\n\r"
    "  ofp_num0 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_NIFCLSB2_OFP_TBL_SET_CFC_NIFCLSB2_OFP_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_NIFCLSB2_OFP_TBL_SET_CFC_NIFCLSB2_OFP_TBL_SET_TBL_DATA_EGQ_OFP_LP_VALID1_ID, 
    "ofp_lp_valid1", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.egq_ofp_lp_valid1:\n\r"
    "  uint32 egq_ofp_lp_valid1\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_nifclsb2_ofp_tbl_set entry_offset_ndx 0\n\r"
    "  egq_ofp_lp_valid3 0 egq_ofp_lp_valid2 0 egq_ofp_lp_valid1 0 egq_ofp_lp_valid0\n\r"
    "  0 egq_ofp_hp_valid3 0 egq_ofp_hp_valid2 0 egq_ofp_hp_valid1 0\n\r"
    "  egq_ofp_hp_valid0 0 sch_ofp_lp_valid3 0 sch_ofp_lp_valid2 0 sch_ofp_lp_valid1\n\r"
    "  0 sch_ofp_lp_valid0 0 sch_ofp_hp_valid3 0 sch_ofp_hp_valid2 0\n\r"
    "  sch_ofp_hp_valid1 0 sch_ofp_hp_valid0 0 ofp_num3 0 ofp_num2 0 ofp_num1 0\n\r"
    "  ofp_num0 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_NIFCLSB2_OFP_TBL_SET_CFC_NIFCLSB2_OFP_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_NIFCLSB2_OFP_TBL_SET_CFC_NIFCLSB2_OFP_TBL_SET_TBL_DATA_EGQ_OFP_LP_VALID0_ID, 
    "ofp_lp_valid0", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.egq_ofp_lp_valid0:\n\r"
    "  uint32 egq_ofp_lp_valid0\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_nifclsb2_ofp_tbl_set entry_offset_ndx 0\n\r"
    "  egq_ofp_lp_valid3 0 egq_ofp_lp_valid2 0 egq_ofp_lp_valid1 0 egq_ofp_lp_valid0\n\r"
    "  0 egq_ofp_hp_valid3 0 egq_ofp_hp_valid2 0 egq_ofp_hp_valid1 0\n\r"
    "  egq_ofp_hp_valid0 0 sch_ofp_lp_valid3 0 sch_ofp_lp_valid2 0 sch_ofp_lp_valid1\n\r"
    "  0 sch_ofp_lp_valid0 0 sch_ofp_hp_valid3 0 sch_ofp_hp_valid2 0\n\r"
    "  sch_ofp_hp_valid1 0 sch_ofp_hp_valid0 0 ofp_num3 0 ofp_num2 0 ofp_num1 0\n\r"
    "  ofp_num0 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_NIFCLSB2_OFP_TBL_SET_CFC_NIFCLSB2_OFP_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_NIFCLSB2_OFP_TBL_SET_CFC_NIFCLSB2_OFP_TBL_SET_TBL_DATA_EGQ_OFP_HP_VALID3_ID, 
    "ofp_hp_valid3", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.egq_ofp_hp_valid3:\n\r"
    "  uint32 egq_ofp_hp_valid3\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_nifclsb2_ofp_tbl_set entry_offset_ndx 0\n\r"
    "  egq_ofp_lp_valid3 0 egq_ofp_lp_valid2 0 egq_ofp_lp_valid1 0 egq_ofp_lp_valid0\n\r"
    "  0 egq_ofp_hp_valid3 0 egq_ofp_hp_valid2 0 egq_ofp_hp_valid1 0\n\r"
    "  egq_ofp_hp_valid0 0 sch_ofp_lp_valid3 0 sch_ofp_lp_valid2 0 sch_ofp_lp_valid1\n\r"
    "  0 sch_ofp_lp_valid0 0 sch_ofp_hp_valid3 0 sch_ofp_hp_valid2 0\n\r"
    "  sch_ofp_hp_valid1 0 sch_ofp_hp_valid0 0 ofp_num3 0 ofp_num2 0 ofp_num1 0\n\r"
    "  ofp_num0 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_NIFCLSB2_OFP_TBL_SET_CFC_NIFCLSB2_OFP_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_NIFCLSB2_OFP_TBL_SET_CFC_NIFCLSB2_OFP_TBL_SET_TBL_DATA_EGQ_OFP_HP_VALID2_ID, 
    "ofp_hp_valid2", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.egq_ofp_hp_valid2:\n\r"
    "  uint32 egq_ofp_hp_valid2\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_nifclsb2_ofp_tbl_set entry_offset_ndx 0\n\r"
    "  egq_ofp_lp_valid3 0 egq_ofp_lp_valid2 0 egq_ofp_lp_valid1 0 egq_ofp_lp_valid0\n\r"
    "  0 egq_ofp_hp_valid3 0 egq_ofp_hp_valid2 0 egq_ofp_hp_valid1 0\n\r"
    "  egq_ofp_hp_valid0 0 sch_ofp_lp_valid3 0 sch_ofp_lp_valid2 0 sch_ofp_lp_valid1\n\r"
    "  0 sch_ofp_lp_valid0 0 sch_ofp_hp_valid3 0 sch_ofp_hp_valid2 0\n\r"
    "  sch_ofp_hp_valid1 0 sch_ofp_hp_valid0 0 ofp_num3 0 ofp_num2 0 ofp_num1 0\n\r"
    "  ofp_num0 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_NIFCLSB2_OFP_TBL_SET_CFC_NIFCLSB2_OFP_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_NIFCLSB2_OFP_TBL_SET_CFC_NIFCLSB2_OFP_TBL_SET_TBL_DATA_EGQ_OFP_HP_VALID1_ID, 
    "ofp_hp_valid1", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.egq_ofp_hp_valid1:\n\r"
    "  uint32 egq_ofp_hp_valid1\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_nifclsb2_ofp_tbl_set entry_offset_ndx 0\n\r"
    "  egq_ofp_lp_valid3 0 egq_ofp_lp_valid2 0 egq_ofp_lp_valid1 0 egq_ofp_lp_valid0\n\r"
    "  0 egq_ofp_hp_valid3 0 egq_ofp_hp_valid2 0 egq_ofp_hp_valid1 0\n\r"
    "  egq_ofp_hp_valid0 0 sch_ofp_lp_valid3 0 sch_ofp_lp_valid2 0 sch_ofp_lp_valid1\n\r"
    "  0 sch_ofp_lp_valid0 0 sch_ofp_hp_valid3 0 sch_ofp_hp_valid2 0\n\r"
    "  sch_ofp_hp_valid1 0 sch_ofp_hp_valid0 0 ofp_num3 0 ofp_num2 0 ofp_num1 0\n\r"
    "  ofp_num0 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_NIFCLSB2_OFP_TBL_SET_CFC_NIFCLSB2_OFP_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_NIFCLSB2_OFP_TBL_SET_CFC_NIFCLSB2_OFP_TBL_SET_TBL_DATA_EGQ_OFP_HP_VALID0_ID, 
    "ofp_hp_valid0", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.egq_ofp_hp_valid0:\n\r"
    "  uint32 egq_ofp_hp_valid0\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_nifclsb2_ofp_tbl_set entry_offset_ndx 0\n\r"
    "  egq_ofp_lp_valid3 0 egq_ofp_lp_valid2 0 egq_ofp_lp_valid1 0 egq_ofp_lp_valid0\n\r"
    "  0 egq_ofp_hp_valid3 0 egq_ofp_hp_valid2 0 egq_ofp_hp_valid1 0\n\r"
    "  egq_ofp_hp_valid0 0 sch_ofp_lp_valid3 0 sch_ofp_lp_valid2 0 sch_ofp_lp_valid1\n\r"
    "  0 sch_ofp_lp_valid0 0 sch_ofp_hp_valid3 0 sch_ofp_hp_valid2 0\n\r"
    "  sch_ofp_hp_valid1 0 sch_ofp_hp_valid0 0 ofp_num3 0 ofp_num2 0 ofp_num1 0\n\r"
    "  ofp_num0 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_NIFCLSB2_OFP_TBL_SET_CFC_NIFCLSB2_OFP_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_NIFCLSB2_OFP_TBL_SET_CFC_NIFCLSB2_OFP_TBL_SET_TBL_DATA_SCH_OFP_LP_VALID3_ID, 
    "ofp_lp_valid3", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.sch_ofp_lp_valid3:\n\r"
    "  uint32 sch_ofp_lp_valid3\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_nifclsb2_ofp_tbl_set entry_offset_ndx 0\n\r"
    "  egq_ofp_lp_valid3 0 egq_ofp_lp_valid2 0 egq_ofp_lp_valid1 0 egq_ofp_lp_valid0\n\r"
    "  0 egq_ofp_hp_valid3 0 egq_ofp_hp_valid2 0 egq_ofp_hp_valid1 0\n\r"
    "  egq_ofp_hp_valid0 0 sch_ofp_lp_valid3 0 sch_ofp_lp_valid2 0 sch_ofp_lp_valid1\n\r"
    "  0 sch_ofp_lp_valid0 0 sch_ofp_hp_valid3 0 sch_ofp_hp_valid2 0\n\r"
    "  sch_ofp_hp_valid1 0 sch_ofp_hp_valid0 0 ofp_num3 0 ofp_num2 0 ofp_num1 0\n\r"
    "  ofp_num0 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_NIFCLSB2_OFP_TBL_SET_CFC_NIFCLSB2_OFP_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_NIFCLSB2_OFP_TBL_SET_CFC_NIFCLSB2_OFP_TBL_SET_TBL_DATA_SCH_OFP_LP_VALID2_ID, 
    "ofp_lp_valid2", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.sch_ofp_lp_valid2:\n\r"
    "  uint32 sch_ofp_lp_valid2\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_nifclsb2_ofp_tbl_set entry_offset_ndx 0\n\r"
    "  egq_ofp_lp_valid3 0 egq_ofp_lp_valid2 0 egq_ofp_lp_valid1 0 egq_ofp_lp_valid0\n\r"
    "  0 egq_ofp_hp_valid3 0 egq_ofp_hp_valid2 0 egq_ofp_hp_valid1 0\n\r"
    "  egq_ofp_hp_valid0 0 sch_ofp_lp_valid3 0 sch_ofp_lp_valid2 0 sch_ofp_lp_valid1\n\r"
    "  0 sch_ofp_lp_valid0 0 sch_ofp_hp_valid3 0 sch_ofp_hp_valid2 0\n\r"
    "  sch_ofp_hp_valid1 0 sch_ofp_hp_valid0 0 ofp_num3 0 ofp_num2 0 ofp_num1 0\n\r"
    "  ofp_num0 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_NIFCLSB2_OFP_TBL_SET_CFC_NIFCLSB2_OFP_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_NIFCLSB2_OFP_TBL_SET_CFC_NIFCLSB2_OFP_TBL_SET_TBL_DATA_SCH_OFP_LP_VALID1_ID, 
    "ofp_lp_valid1", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.sch_ofp_lp_valid1:\n\r"
    "  uint32 sch_ofp_lp_valid1\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_nifclsb2_ofp_tbl_set entry_offset_ndx 0\n\r"
    "  egq_ofp_lp_valid3 0 egq_ofp_lp_valid2 0 egq_ofp_lp_valid1 0 egq_ofp_lp_valid0\n\r"
    "  0 egq_ofp_hp_valid3 0 egq_ofp_hp_valid2 0 egq_ofp_hp_valid1 0\n\r"
    "  egq_ofp_hp_valid0 0 sch_ofp_lp_valid3 0 sch_ofp_lp_valid2 0 sch_ofp_lp_valid1\n\r"
    "  0 sch_ofp_lp_valid0 0 sch_ofp_hp_valid3 0 sch_ofp_hp_valid2 0\n\r"
    "  sch_ofp_hp_valid1 0 sch_ofp_hp_valid0 0 ofp_num3 0 ofp_num2 0 ofp_num1 0\n\r"
    "  ofp_num0 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_NIFCLSB2_OFP_TBL_SET_CFC_NIFCLSB2_OFP_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_NIFCLSB2_OFP_TBL_SET_CFC_NIFCLSB2_OFP_TBL_SET_TBL_DATA_SCH_OFP_LP_VALID0_ID, 
    "ofp_lp_valid0", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.sch_ofp_lp_valid0:\n\r"
    "  uint32 sch_ofp_lp_valid0\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_nifclsb2_ofp_tbl_set entry_offset_ndx 0\n\r"
    "  egq_ofp_lp_valid3 0 egq_ofp_lp_valid2 0 egq_ofp_lp_valid1 0 egq_ofp_lp_valid0\n\r"
    "  0 egq_ofp_hp_valid3 0 egq_ofp_hp_valid2 0 egq_ofp_hp_valid1 0\n\r"
    "  egq_ofp_hp_valid0 0 sch_ofp_lp_valid3 0 sch_ofp_lp_valid2 0 sch_ofp_lp_valid1\n\r"
    "  0 sch_ofp_lp_valid0 0 sch_ofp_hp_valid3 0 sch_ofp_hp_valid2 0\n\r"
    "  sch_ofp_hp_valid1 0 sch_ofp_hp_valid0 0 ofp_num3 0 ofp_num2 0 ofp_num1 0\n\r"
    "  ofp_num0 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_NIFCLSB2_OFP_TBL_SET_CFC_NIFCLSB2_OFP_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_NIFCLSB2_OFP_TBL_SET_CFC_NIFCLSB2_OFP_TBL_SET_TBL_DATA_SCH_OFP_HP_VALID3_ID, 
    "ofp_hp_valid3", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.sch_ofp_hp_valid3:\n\r"
    "  uint32 sch_ofp_hp_valid3\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_nifclsb2_ofp_tbl_set entry_offset_ndx 0\n\r"
    "  egq_ofp_lp_valid3 0 egq_ofp_lp_valid2 0 egq_ofp_lp_valid1 0 egq_ofp_lp_valid0\n\r"
    "  0 egq_ofp_hp_valid3 0 egq_ofp_hp_valid2 0 egq_ofp_hp_valid1 0\n\r"
    "  egq_ofp_hp_valid0 0 sch_ofp_lp_valid3 0 sch_ofp_lp_valid2 0 sch_ofp_lp_valid1\n\r"
    "  0 sch_ofp_lp_valid0 0 sch_ofp_hp_valid3 0 sch_ofp_hp_valid2 0\n\r"
    "  sch_ofp_hp_valid1 0 sch_ofp_hp_valid0 0 ofp_num3 0 ofp_num2 0 ofp_num1 0\n\r"
    "  ofp_num0 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_NIFCLSB2_OFP_TBL_SET_CFC_NIFCLSB2_OFP_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_NIFCLSB2_OFP_TBL_SET_CFC_NIFCLSB2_OFP_TBL_SET_TBL_DATA_SCH_OFP_HP_VALID2_ID, 
    "ofp_hp_valid2", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.sch_ofp_hp_valid2:\n\r"
    "  uint32 sch_ofp_hp_valid2\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_nifclsb2_ofp_tbl_set entry_offset_ndx 0\n\r"
    "  egq_ofp_lp_valid3 0 egq_ofp_lp_valid2 0 egq_ofp_lp_valid1 0 egq_ofp_lp_valid0\n\r"
    "  0 egq_ofp_hp_valid3 0 egq_ofp_hp_valid2 0 egq_ofp_hp_valid1 0\n\r"
    "  egq_ofp_hp_valid0 0 sch_ofp_lp_valid3 0 sch_ofp_lp_valid2 0 sch_ofp_lp_valid1\n\r"
    "  0 sch_ofp_lp_valid0 0 sch_ofp_hp_valid3 0 sch_ofp_hp_valid2 0\n\r"
    "  sch_ofp_hp_valid1 0 sch_ofp_hp_valid0 0 ofp_num3 0 ofp_num2 0 ofp_num1 0\n\r"
    "  ofp_num0 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_NIFCLSB2_OFP_TBL_SET_CFC_NIFCLSB2_OFP_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_NIFCLSB2_OFP_TBL_SET_CFC_NIFCLSB2_OFP_TBL_SET_TBL_DATA_SCH_OFP_HP_VALID1_ID, 
    "ofp_hp_valid1", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.sch_ofp_hp_valid1:\n\r"
    "  uint32 sch_ofp_hp_valid1\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_nifclsb2_ofp_tbl_set entry_offset_ndx 0\n\r"
    "  egq_ofp_lp_valid3 0 egq_ofp_lp_valid2 0 egq_ofp_lp_valid1 0 egq_ofp_lp_valid0\n\r"
    "  0 egq_ofp_hp_valid3 0 egq_ofp_hp_valid2 0 egq_ofp_hp_valid1 0\n\r"
    "  egq_ofp_hp_valid0 0 sch_ofp_lp_valid3 0 sch_ofp_lp_valid2 0 sch_ofp_lp_valid1\n\r"
    "  0 sch_ofp_lp_valid0 0 sch_ofp_hp_valid3 0 sch_ofp_hp_valid2 0\n\r"
    "  sch_ofp_hp_valid1 0 sch_ofp_hp_valid0 0 ofp_num3 0 ofp_num2 0 ofp_num1 0\n\r"
    "  ofp_num0 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_NIFCLSB2_OFP_TBL_SET_CFC_NIFCLSB2_OFP_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_NIFCLSB2_OFP_TBL_SET_CFC_NIFCLSB2_OFP_TBL_SET_TBL_DATA_SCH_OFP_HP_VALID0_ID, 
    "ofp_hp_valid0", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.sch_ofp_hp_valid0:\n\r"
    "  uint32 sch_ofp_hp_valid0\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_nifclsb2_ofp_tbl_set entry_offset_ndx 0\n\r"
    "  egq_ofp_lp_valid3 0 egq_ofp_lp_valid2 0 egq_ofp_lp_valid1 0 egq_ofp_lp_valid0\n\r"
    "  0 egq_ofp_hp_valid3 0 egq_ofp_hp_valid2 0 egq_ofp_hp_valid1 0\n\r"
    "  egq_ofp_hp_valid0 0 sch_ofp_lp_valid3 0 sch_ofp_lp_valid2 0 sch_ofp_lp_valid1\n\r"
    "  0 sch_ofp_lp_valid0 0 sch_ofp_hp_valid3 0 sch_ofp_hp_valid2 0\n\r"
    "  sch_ofp_hp_valid1 0 sch_ofp_hp_valid0 0 ofp_num3 0 ofp_num2 0 ofp_num1 0\n\r"
    "  ofp_num0 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_NIFCLSB2_OFP_TBL_SET_CFC_NIFCLSB2_OFP_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_NIFCLSB2_OFP_TBL_SET_CFC_NIFCLSB2_OFP_TBL_SET_TBL_DATA_OFP_NUM3_ID, 
    "ofp_num3", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.ofp_num3:\n\r"
    "  uint32 ofp_num3\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_nifclsb2_ofp_tbl_set entry_offset_ndx 0\n\r"
    "  egq_ofp_lp_valid3 0 egq_ofp_lp_valid2 0 egq_ofp_lp_valid1 0 egq_ofp_lp_valid0\n\r"
    "  0 egq_ofp_hp_valid3 0 egq_ofp_hp_valid2 0 egq_ofp_hp_valid1 0\n\r"
    "  egq_ofp_hp_valid0 0 sch_ofp_lp_valid3 0 sch_ofp_lp_valid2 0 sch_ofp_lp_valid1\n\r"
    "  0 sch_ofp_lp_valid0 0 sch_ofp_hp_valid3 0 sch_ofp_hp_valid2 0\n\r"
    "  sch_ofp_hp_valid1 0 sch_ofp_hp_valid0 0 ofp_num3 0 ofp_num2 0 ofp_num1 0\n\r"
    "  ofp_num0 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_NIFCLSB2_OFP_TBL_SET_CFC_NIFCLSB2_OFP_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_NIFCLSB2_OFP_TBL_SET_CFC_NIFCLSB2_OFP_TBL_SET_TBL_DATA_OFP_NUM2_ID, 
    "ofp_num2", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.ofp_num2:\n\r"
    "  uint32 ofp_num2\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_nifclsb2_ofp_tbl_set entry_offset_ndx 0\n\r"
    "  egq_ofp_lp_valid3 0 egq_ofp_lp_valid2 0 egq_ofp_lp_valid1 0 egq_ofp_lp_valid0\n\r"
    "  0 egq_ofp_hp_valid3 0 egq_ofp_hp_valid2 0 egq_ofp_hp_valid1 0\n\r"
    "  egq_ofp_hp_valid0 0 sch_ofp_lp_valid3 0 sch_ofp_lp_valid2 0 sch_ofp_lp_valid1\n\r"
    "  0 sch_ofp_lp_valid0 0 sch_ofp_hp_valid3 0 sch_ofp_hp_valid2 0\n\r"
    "  sch_ofp_hp_valid1 0 sch_ofp_hp_valid0 0 ofp_num3 0 ofp_num2 0 ofp_num1 0\n\r"
    "  ofp_num0 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_NIFCLSB2_OFP_TBL_SET_CFC_NIFCLSB2_OFP_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_NIFCLSB2_OFP_TBL_SET_CFC_NIFCLSB2_OFP_TBL_SET_TBL_DATA_OFP_NUM1_ID, 
    "ofp_num1", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.ofp_num1:\n\r"
    "  uint32 ofp_num1\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_nifclsb2_ofp_tbl_set entry_offset_ndx 0\n\r"
    "  egq_ofp_lp_valid3 0 egq_ofp_lp_valid2 0 egq_ofp_lp_valid1 0 egq_ofp_lp_valid0\n\r"
    "  0 egq_ofp_hp_valid3 0 egq_ofp_hp_valid2 0 egq_ofp_hp_valid1 0\n\r"
    "  egq_ofp_hp_valid0 0 sch_ofp_lp_valid3 0 sch_ofp_lp_valid2 0 sch_ofp_lp_valid1\n\r"
    "  0 sch_ofp_lp_valid0 0 sch_ofp_hp_valid3 0 sch_ofp_hp_valid2 0\n\r"
    "  sch_ofp_hp_valid1 0 sch_ofp_hp_valid0 0 ofp_num3 0 ofp_num2 0 ofp_num1 0\n\r"
    "  ofp_num0 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_NIFCLSB2_OFP_TBL_SET_CFC_NIFCLSB2_OFP_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_NIFCLSB2_OFP_TBL_SET_CFC_NIFCLSB2_OFP_TBL_SET_TBL_DATA_OFP_NUM0_ID, 
    "ofp_num0", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.ofp_num0:\n\r"
    "  uint32 ofp_num0\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_nifclsb2_ofp_tbl_set entry_offset_ndx 0\n\r"
    "  egq_ofp_lp_valid3 0 egq_ofp_lp_valid2 0 egq_ofp_lp_valid1 0 egq_ofp_lp_valid0\n\r"
    "  0 egq_ofp_hp_valid3 0 egq_ofp_hp_valid2 0 egq_ofp_hp_valid1 0\n\r"
    "  egq_ofp_hp_valid0 0 sch_ofp_lp_valid3 0 sch_ofp_lp_valid2 0 sch_ofp_lp_valid1\n\r"
    "  0 sch_ofp_lp_valid0 0 sch_ofp_hp_valid3 0 sch_ofp_hp_valid2 0\n\r"
    "  sch_ofp_hp_valid1 0 sch_ofp_hp_valid0 0 ofp_num3 0 ofp_num2 0 ofp_num1 0\n\r"
    "  ofp_num0 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_NIFCLSB2_OFP_TBL_SET_CFC_NIFCLSB2_OFP_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_NIFCLSB2_OFP_TBL_GET_CFC_NIFCLSB2_OFP_TBL_GET_ID, 
    "nifclsb2_ofp_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_nifclsb2_ofp_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_NIFCLSB2_OFP_TBL_GET_CFC_NIFCLSB2_OFP_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_CALRXA_TBL_SET_CFC_CALRXA_TBL_SET_ID, 
    "calrxa_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_calrxa_tbl_set entry_offset_ndx 0 fc_dest_sel 0\n\r"
    "  fc_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_CALRXA_TBL_SET_CFC_CALRXA_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_CALRXA_TBL_SET_CFC_CALRXA_TBL_SET_TBL_DATA_FC_DEST_SEL_ID, 
    "fc_dest_sel", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.fc_dest_sel:\n\r"
    "  uint32 fc_dest_sel\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_calrxa_tbl_set entry_offset_ndx 0 fc_dest_sel 0\n\r"
    "  fc_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_CALRXA_TBL_SET_CFC_CALRXA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_CALRXA_TBL_SET_CFC_CALRXA_TBL_SET_TBL_DATA_FC_INDEX_ID, 
    "fc_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.fc_index:\n\r"
    "  uint32 fc_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_calrxa_tbl_set entry_offset_ndx 0 fc_dest_sel 0\n\r"
    "  fc_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_CALRXA_TBL_SET_CFC_CALRXA_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_CALRXA_TBL_GET_CFC_CALRXA_TBL_GET_ID, 
    "calrxa_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_calrxa_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_CALRXA_TBL_GET_CFC_CALRXA_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_CALRXB_TBL_SET_CFC_CALRXB_TBL_SET_ID, 
    "calrxb_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_calrxb_tbl_set entry_offset_ndx 0 fc_dest_sel 0\n\r"
    "  fc_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_CALRXB_TBL_SET_CFC_CALRXB_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_CALRXB_TBL_SET_CFC_CALRXB_TBL_SET_TBL_DATA_FC_DEST_SEL_ID, 
    "fc_dest_sel", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.fc_dest_sel:\n\r"
    "  uint32 fc_dest_sel\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_calrxb_tbl_set entry_offset_ndx 0 fc_dest_sel 0\n\r"
    "  fc_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_CALRXB_TBL_SET_CFC_CALRXB_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_CALRXB_TBL_SET_CFC_CALRXB_TBL_SET_TBL_DATA_FC_INDEX_ID, 
    "fc_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.fc_index:\n\r"
    "  uint32 fc_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_calrxb_tbl_set entry_offset_ndx 0 fc_dest_sel 0\n\r"
    "  fc_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_CALRXB_TBL_SET_CFC_CALRXB_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_CALRXB_TBL_GET_CFC_CALRXB_TBL_GET_ID, 
    "calrxb_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_calrxb_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_CALRXB_TBL_GET_CFC_CALRXB_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_OOB0_SCH_MAP_TBL_SET_CFC_OOB0_SCH_MAP_TBL_SET_ID, 
    "oob0_sch_map_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_oob0_sch_map_tbl_set entry_offset_ndx 0 hp_ofp_valid\n\r"
    "  0 lp_ofp_valid 0 ofp_hr 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_OOB0_SCH_MAP_TBL_SET_CFC_OOB0_SCH_MAP_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_OOB0_SCH_MAP_TBL_SET_CFC_OOB0_SCH_MAP_TBL_SET_TBL_DATA_HP_OFP_VALID_ID, 
    "hp_ofp_valid", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.hp_ofp_valid:\n\r"
    "  uint32 hp_ofp_valid\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_oob0_sch_map_tbl_set entry_offset_ndx 0 hp_ofp_valid\n\r"
    "  0 lp_ofp_valid 0 ofp_hr 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_OOB0_SCH_MAP_TBL_SET_CFC_OOB0_SCH_MAP_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_OOB0_SCH_MAP_TBL_SET_CFC_OOB0_SCH_MAP_TBL_SET_TBL_DATA_LP_OFP_VALID_ID, 
    "lp_ofp_valid", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.lp_ofp_valid:\n\r"
    "  uint32 lp_ofp_valid\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_oob0_sch_map_tbl_set entry_offset_ndx 0 hp_ofp_valid\n\r"
    "  0 lp_ofp_valid 0 ofp_hr 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_OOB0_SCH_MAP_TBL_SET_CFC_OOB0_SCH_MAP_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_OOB0_SCH_MAP_TBL_SET_CFC_OOB0_SCH_MAP_TBL_SET_TBL_DATA_OFP_HR_ID, 
    "ofp_hr", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.ofp_hr:\n\r"
    "  uint32 ofp_hr\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_oob0_sch_map_tbl_set entry_offset_ndx 0 hp_ofp_valid\n\r"
    "  0 lp_ofp_valid 0 ofp_hr 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_OOB0_SCH_MAP_TBL_SET_CFC_OOB0_SCH_MAP_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_OOB0_SCH_MAP_TBL_GET_CFC_OOB0_SCH_MAP_TBL_GET_ID, 
    "oob0_sch_map_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_oob0_sch_map_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_OOB0_SCH_MAP_TBL_GET_CFC_OOB0_SCH_MAP_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_OOB1_SCH_MAP_TBL_SET_CFC_OOB1_SCH_MAP_TBL_SET_ID, 
    "oob1_sch_map_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_oob1_sch_map_tbl_set entry_offset_ndx 0 hp_ofp_valid\n\r"
    "  0 lp_ofp_valid 0 ofp_hr 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_OOB1_SCH_MAP_TBL_SET_CFC_OOB1_SCH_MAP_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_OOB1_SCH_MAP_TBL_SET_CFC_OOB1_SCH_MAP_TBL_SET_TBL_DATA_HP_OFP_VALID_ID, 
    "hp_ofp_valid", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.hp_ofp_valid:\n\r"
    "  uint32 hp_ofp_valid\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_oob1_sch_map_tbl_set entry_offset_ndx 0 hp_ofp_valid\n\r"
    "  0 lp_ofp_valid 0 ofp_hr 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_OOB1_SCH_MAP_TBL_SET_CFC_OOB1_SCH_MAP_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_OOB1_SCH_MAP_TBL_SET_CFC_OOB1_SCH_MAP_TBL_SET_TBL_DATA_LP_OFP_VALID_ID, 
    "lp_ofp_valid", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.lp_ofp_valid:\n\r"
    "  uint32 lp_ofp_valid\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_oob1_sch_map_tbl_set entry_offset_ndx 0 hp_ofp_valid\n\r"
    "  0 lp_ofp_valid 0 ofp_hr 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_OOB1_SCH_MAP_TBL_SET_CFC_OOB1_SCH_MAP_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_OOB1_SCH_MAP_TBL_SET_CFC_OOB1_SCH_MAP_TBL_SET_TBL_DATA_OFP_HR_ID, 
    "ofp_hr", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.ofp_hr:\n\r"
    "  uint32 ofp_hr\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_oob1_sch_map_tbl_set entry_offset_ndx 0 hp_ofp_valid\n\r"
    "  0 lp_ofp_valid 0 ofp_hr 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_OOB1_SCH_MAP_TBL_SET_CFC_OOB1_SCH_MAP_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_OOB1_SCH_MAP_TBL_GET_CFC_OOB1_SCH_MAP_TBL_GET_ID, 
    "oob1_sch_map_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_oob1_sch_map_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_OOB1_SCH_MAP_TBL_GET_CFC_OOB1_SCH_MAP_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_CALTX_TBL_SET_CFC_CALTX_TBL_SET_ID, 
    "caltx_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_caltx_tbl_set entry_offset_ndx 0 fc_source_sel 0\n\r"
    "  fc_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_CALTX_TBL_SET_CFC_CALTX_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_CALTX_TBL_SET_CFC_CALTX_TBL_SET_TBL_DATA_FC_SOURCE_SEL_ID, 
    "fc_source_sel", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.fc_source_sel:\n\r"
    "  uint32 fc_source_sel\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_caltx_tbl_set entry_offset_ndx 0 fc_source_sel 0\n\r"
    "  fc_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_CALTX_TBL_SET_CFC_CALTX_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_CALTX_TBL_SET_CFC_CALTX_TBL_SET_TBL_DATA_FC_INDEX_ID, 
    "fc_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.fc_index:\n\r"
    "  uint32 fc_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_caltx_tbl_set entry_offset_ndx 0 fc_source_sel 0\n\r"
    "  fc_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_CALTX_TBL_SET_CFC_CALTX_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_CALTX_TBL_GET_CFC_CALTX_TBL_GET_ID, 
    "caltx_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_caltx_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_CALTX_TBL_GET_CFC_CALTX_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_ILKN0_CALRX_TBL_SET_CFC_ILKN0_CALRX_TBL_SET_ID, 
    "ilkn0_calrx_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_ilkn0_calrx_tbl_set entry_offset_ndx 0 fc_dest_sel 0\n\r"
    "  fc_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_ILKN0_CALRX_TBL_SET_CFC_ILKN0_CALRX_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_ILKN0_CALRX_TBL_SET_CFC_ILKN0_CALRX_TBL_SET_TBL_DATA_FC_DEST_SEL_ID, 
    "fc_dest_sel", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.fc_dest_sel:\n\r"
    "  uint32 fc_dest_sel\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_ilkn0_calrx_tbl_set entry_offset_ndx 0 fc_dest_sel 0\n\r"
    "  fc_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_ILKN0_CALRX_TBL_SET_CFC_ILKN0_CALRX_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_ILKN0_CALRX_TBL_SET_CFC_ILKN0_CALRX_TBL_SET_TBL_DATA_FC_INDEX_ID, 
    "fc_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.fc_index:\n\r"
    "  uint32 fc_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_ilkn0_calrx_tbl_set entry_offset_ndx 0 fc_dest_sel 0\n\r"
    "  fc_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_ILKN0_CALRX_TBL_SET_CFC_ILKN0_CALRX_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_ILKN0_CALRX_TBL_GET_CFC_ILKN0_CALRX_TBL_GET_ID, 
    "ilkn0_calrx_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_ilkn0_calrx_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_ILKN0_CALRX_TBL_GET_CFC_ILKN0_CALRX_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_ILKN1_CALRX_TBL_SET_CFC_ILKN1_CALRX_TBL_SET_ID, 
    "ilkn1_calrx_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_ilkn1_calrx_tbl_set entry_offset_ndx 0 fc_dest_sel 0\n\r"
    "  fc_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_ILKN1_CALRX_TBL_SET_CFC_ILKN1_CALRX_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_ILKN1_CALRX_TBL_SET_CFC_ILKN1_CALRX_TBL_SET_TBL_DATA_FC_DEST_SEL_ID, 
    "fc_dest_sel", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.fc_dest_sel:\n\r"
    "  uint32 fc_dest_sel\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_ilkn1_calrx_tbl_set entry_offset_ndx 0 fc_dest_sel 0\n\r"
    "  fc_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_ILKN1_CALRX_TBL_SET_CFC_ILKN1_CALRX_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_ILKN1_CALRX_TBL_SET_CFC_ILKN1_CALRX_TBL_SET_TBL_DATA_FC_INDEX_ID, 
    "fc_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.fc_index:\n\r"
    "  uint32 fc_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_ilkn1_calrx_tbl_set entry_offset_ndx 0 fc_dest_sel 0\n\r"
    "  fc_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_ILKN1_CALRX_TBL_SET_CFC_ILKN1_CALRX_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_ILKN1_CALRX_TBL_GET_CFC_ILKN1_CALRX_TBL_GET_ID, 
    "ilkn1_calrx_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_ilkn1_calrx_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_ILKN1_CALRX_TBL_GET_CFC_ILKN1_CALRX_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_ILKN0_SCH_MAP_TBL_SET_CFC_ILKN0_SCH_MAP_TBL_SET_ID, 
    "ilkn0_sch_map_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_ilkn0_sch_map_tbl_set entry_offset_ndx 0 hp_ofp_valid\n\r"
    "  0 lp_ofp_valid 0 ofp_hr 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_ILKN0_SCH_MAP_TBL_SET_CFC_ILKN0_SCH_MAP_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_ILKN0_SCH_MAP_TBL_SET_CFC_ILKN0_SCH_MAP_TBL_SET_TBL_DATA_HP_OFP_VALID_ID, 
    "hp_ofp_valid", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.hp_ofp_valid:\n\r"
    "  uint32 hp_ofp_valid\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_ilkn0_sch_map_tbl_set entry_offset_ndx 0 hp_ofp_valid\n\r"
    "  0 lp_ofp_valid 0 ofp_hr 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_ILKN0_SCH_MAP_TBL_SET_CFC_ILKN0_SCH_MAP_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_ILKN0_SCH_MAP_TBL_SET_CFC_ILKN0_SCH_MAP_TBL_SET_TBL_DATA_LP_OFP_VALID_ID, 
    "lp_ofp_valid", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.lp_ofp_valid:\n\r"
    "  uint32 lp_ofp_valid\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_ilkn0_sch_map_tbl_set entry_offset_ndx 0 hp_ofp_valid\n\r"
    "  0 lp_ofp_valid 0 ofp_hr 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_ILKN0_SCH_MAP_TBL_SET_CFC_ILKN0_SCH_MAP_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_ILKN0_SCH_MAP_TBL_SET_CFC_ILKN0_SCH_MAP_TBL_SET_TBL_DATA_OFP_HR_ID, 
    "ofp_hr", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.ofp_hr:\n\r"
    "  uint32 ofp_hr\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_ilkn0_sch_map_tbl_set entry_offset_ndx 0 hp_ofp_valid\n\r"
    "  0 lp_ofp_valid 0 ofp_hr 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_ILKN0_SCH_MAP_TBL_SET_CFC_ILKN0_SCH_MAP_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_ILKN0_SCH_MAP_TBL_GET_CFC_ILKN0_SCH_MAP_TBL_GET_ID, 
    "ilkn0_sch_map_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_ilkn0_sch_map_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_ILKN0_SCH_MAP_TBL_GET_CFC_ILKN0_SCH_MAP_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_ILKN1_SCH_MAP_TBL_SET_CFC_ILKN1_SCH_MAP_TBL_SET_ID, 
    "ilkn1_sch_map_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_ilkn1_sch_map_tbl_set entry_offset_ndx 0 hp_ofp_valid\n\r"
    "  0 lp_ofp_valid 0 ofp_hr 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_ILKN1_SCH_MAP_TBL_SET_CFC_ILKN1_SCH_MAP_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_ILKN1_SCH_MAP_TBL_SET_CFC_ILKN1_SCH_MAP_TBL_SET_TBL_DATA_HP_OFP_VALID_ID, 
    "hp_ofp_valid", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.hp_ofp_valid:\n\r"
    "  uint32 hp_ofp_valid\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_ilkn1_sch_map_tbl_set entry_offset_ndx 0 hp_ofp_valid\n\r"
    "  0 lp_ofp_valid 0 ofp_hr 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_ILKN1_SCH_MAP_TBL_SET_CFC_ILKN1_SCH_MAP_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_ILKN1_SCH_MAP_TBL_SET_CFC_ILKN1_SCH_MAP_TBL_SET_TBL_DATA_LP_OFP_VALID_ID, 
    "lp_ofp_valid", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.lp_ofp_valid:\n\r"
    "  uint32 lp_ofp_valid\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_ilkn1_sch_map_tbl_set entry_offset_ndx 0 hp_ofp_valid\n\r"
    "  0 lp_ofp_valid 0 ofp_hr 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_ILKN1_SCH_MAP_TBL_SET_CFC_ILKN1_SCH_MAP_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_ILKN1_SCH_MAP_TBL_SET_CFC_ILKN1_SCH_MAP_TBL_SET_TBL_DATA_OFP_HR_ID, 
    "ofp_hr", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.ofp_hr:\n\r"
    "  uint32 ofp_hr\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_ilkn1_sch_map_tbl_set entry_offset_ndx 0 hp_ofp_valid\n\r"
    "  0 lp_ofp_valid 0 ofp_hr 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_ILKN1_SCH_MAP_TBL_SET_CFC_ILKN1_SCH_MAP_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_ILKN1_SCH_MAP_TBL_GET_CFC_ILKN1_SCH_MAP_TBL_GET_ID, 
    "ilkn1_sch_map_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_ilkn1_sch_map_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_ILKN1_SCH_MAP_TBL_GET_CFC_ILKN1_SCH_MAP_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_ILKN0_CALTX_TBL_SET_CFC_ILKN0_CALTX_TBL_SET_ID, 
    "ilkn0_caltx_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_ilkn0_caltx_tbl_set entry_offset_ndx 0 fc_source_sel\n\r"
    "  0 fc_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_ILKN0_CALTX_TBL_SET_CFC_ILKN0_CALTX_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_ILKN0_CALTX_TBL_SET_CFC_ILKN0_CALTX_TBL_SET_TBL_DATA_FC_SOURCE_SEL_ID, 
    "fc_source_sel", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.fc_source_sel:\n\r"
    "  uint32 fc_source_sel\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_ilkn0_caltx_tbl_set entry_offset_ndx 0 fc_source_sel\n\r"
    "  0 fc_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_ILKN0_CALTX_TBL_SET_CFC_ILKN0_CALTX_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_ILKN0_CALTX_TBL_SET_CFC_ILKN0_CALTX_TBL_SET_TBL_DATA_FC_INDEX_ID, 
    "fc_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.fc_index:\n\r"
    "  uint32 fc_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_ilkn0_caltx_tbl_set entry_offset_ndx 0 fc_source_sel\n\r"
    "  0 fc_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_ILKN0_CALTX_TBL_SET_CFC_ILKN0_CALTX_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_ILKN0_CALTX_TBL_GET_CFC_ILKN0_CALTX_TBL_GET_ID, 
    "ilkn0_caltx_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_ilkn0_caltx_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_ILKN0_CALTX_TBL_GET_CFC_ILKN0_CALTX_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_ILKN1_CALTX_TBL_SET_CFC_ILKN1_CALTX_TBL_SET_ID, 
    "ilkn1_caltx_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_ilkn1_caltx_tbl_set entry_offset_ndx 0 fc_source_sel\n\r"
    "  0 fc_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_ILKN1_CALTX_TBL_SET_CFC_ILKN1_CALTX_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_ILKN1_CALTX_TBL_SET_CFC_ILKN1_CALTX_TBL_SET_TBL_DATA_FC_SOURCE_SEL_ID, 
    "fc_source_sel", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.fc_source_sel:\n\r"
    "  uint32 fc_source_sel\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_ilkn1_caltx_tbl_set entry_offset_ndx 0 fc_source_sel\n\r"
    "  0 fc_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_ILKN1_CALTX_TBL_SET_CFC_ILKN1_CALTX_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_ILKN1_CALTX_TBL_SET_CFC_ILKN1_CALTX_TBL_SET_TBL_DATA_FC_INDEX_ID, 
    "fc_index", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.fc_index:\n\r"
    "  uint32 fc_index\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_ilkn1_caltx_tbl_set entry_offset_ndx 0 fc_source_sel\n\r"
    "  0 fc_index 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_ILKN1_CALTX_TBL_SET_CFC_ILKN1_CALTX_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_CFC_ILKN1_CALTX_TBL_GET_CFC_ILKN1_CALTX_TBL_GET_ID, 
    "ilkn1_caltx_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc cfc_ilkn1_caltx_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_CFC_ILKN1_CALTX_TBL_GET_CFC_ILKN1_CALTX_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_CAL_TBL_SET_SCH_CAL_TBL_SET_ID, 
    "cal_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_cal_tbl_set entry_offset_ndx 0 hrsel 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_CAL_TBL_SET_SCH_CAL_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_CAL_TBL_SET_SCH_CAL_TBL_SET_TBL_DATA_HRSEL_ID, 
    "hrsel", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.hrsel:\n\r"
    "  uint32 hrsel\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_cal_tbl_set entry_offset_ndx 0 hrsel 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_CAL_TBL_SET_SCH_CAL_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_CAL_TBL_GET_SCH_CAL_TBL_GET_ID, 
    "cal_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_cal_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_CAL_TBL_GET_SCH_CAL_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_DRM_TBL_SET_SCH_DRM_TBL_SET_ID, 
    "drm_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_drm_tbl_set entry_offset_ndx 0 device_rate 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_DRM_TBL_SET_SCH_DRM_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_DRM_TBL_SET_SCH_DRM_TBL_SET_TBL_DATA_DEVICE_RATE_ID, 
    "device_rate", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.device_rate:\n\r"
    "  uint32 device_rate\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_drm_tbl_set entry_offset_ndx 0 device_rate 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_DRM_TBL_SET_SCH_DRM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_DRM_TBL_GET_SCH_DRM_TBL_GET_ID, 
    "drm_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_drm_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_DRM_TBL_GET_SCH_DRM_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_DSM_TBL_SET_SCH_DSM_TBL_SET_ID, 
    "dsm_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_dsm_tbl_set entry_offset_ndx 0 dual_shaper_ena 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_DSM_TBL_SET_SCH_DSM_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_DSM_TBL_SET_SCH_DSM_TBL_SET_TBL_DATA_DUAL_SHAPER_ENA_ID, 
    "dual_shaper_ena", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.dual_shaper_ena:\n\r"
    "  uint32 dual_shaper_ena\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_dsm_tbl_set entry_offset_ndx 0 dual_shaper_ena 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_DSM_TBL_SET_SCH_DSM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_DSM_TBL_GET_SCH_DSM_TBL_GET_ID, 
    "dsm_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_dsm_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_DSM_TBL_GET_SCH_DSM_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_FDMS_TBL_SET_SCH_FDMS_TBL_SET_ID, 
    "fdms_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_fdms_tbl_set entry_offset_ndx 0 hrsel_dual 0 cos 0\n\r"
    "  sch_number 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_FDMS_TBL_SET_SCH_FDMS_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_FDMS_TBL_SET_SCH_FDMS_TBL_SET_TBL_DATA_HRSEL_DUAL_ID, 
    "hrsel_dual", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.hrsel_dual:\n\r"
    "  uint32 hrsel_dual\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_fdms_tbl_set entry_offset_ndx 0 hrsel_dual 0 cos 0\n\r"
    "  sch_number 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_FDMS_TBL_SET_SCH_FDMS_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_FDMS_TBL_SET_SCH_FDMS_TBL_SET_TBL_DATA_COS_ID, 
    "cos", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.cos:\n\r"
    "  uint32 cos\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_fdms_tbl_set entry_offset_ndx 0 hrsel_dual 0 cos 0\n\r"
    "  sch_number 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_FDMS_TBL_SET_SCH_FDMS_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_FDMS_TBL_SET_SCH_FDMS_TBL_SET_TBL_DATA_SCH_NUMBER_ID, 
    "number", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.sch_number:\n\r"
    "  uint32 sch_number\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_fdms_tbl_set entry_offset_ndx 0 hrsel_dual 0 cos 0\n\r"
    "  sch_number 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_FDMS_TBL_SET_SCH_FDMS_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_FDMS_TBL_GET_SCH_FDMS_TBL_GET_ID, 
    "fdms_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_fdms_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_FDMS_TBL_GET_SCH_FDMS_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_SHDS_TBL_SET_SCH_SHDS_TBL_SET_ID, 
    "shds_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_shds_tbl_set entry_offset_ndx 0 max_burst_update_odd\n\r"
    "  0 max_burst_update_even 0 slow_rate2_sel_odd 0 max_burst_odd 0\n\r"
    "  peak_rate_exp_odd 0 peak_rate_man_odd 0 slow_rate2_sel_even 0 max_burst_even\n\r"
    "  0 peak_rate_exp_even 0 peak_rate_man_even 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_SHDS_TBL_SET_SCH_SHDS_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_SHDS_TBL_SET_SCH_SHDS_TBL_SET_TBL_DATA_MAX_BURST_UPDATE_ODD_ID, 
    "max_burst_update_odd", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.max_burst_update_odd:\n\r"
    "  uint32 max_burst_update_odd\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_shds_tbl_set entry_offset_ndx 0 max_burst_update_odd\n\r"
    "  0 max_burst_update_even 0 slow_rate2_sel_odd 0 max_burst_odd 0\n\r"
    "  peak_rate_exp_odd 0 peak_rate_man_odd 0 slow_rate2_sel_even 0 max_burst_even\n\r"
    "  0 peak_rate_exp_even 0 peak_rate_man_even 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_SHDS_TBL_SET_SCH_SHDS_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_SHDS_TBL_SET_SCH_SHDS_TBL_SET_TBL_DATA_MAX_BURST_UPDATE_EVEN_ID, 
    "max_burst_update_even", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.max_burst_update_even:\n\r"
    "  uint32 max_burst_update_even\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_shds_tbl_set entry_offset_ndx 0 max_burst_update_odd\n\r"
    "  0 max_burst_update_even 0 slow_rate2_sel_odd 0 max_burst_odd 0\n\r"
    "  peak_rate_exp_odd 0 peak_rate_man_odd 0 slow_rate2_sel_even 0 max_burst_even\n\r"
    "  0 peak_rate_exp_even 0 peak_rate_man_even 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_SHDS_TBL_SET_SCH_SHDS_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_SHDS_TBL_SET_SCH_SHDS_TBL_SET_TBL_DATA_SLOW_RATE2_SEL_ODD_ID, 
    "slow_rate2_sel_odd", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.slow_rate2_sel_odd:\n\r"
    "  uint32 slow_rate2_sel_odd\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_shds_tbl_set entry_offset_ndx 0 max_burst_update_odd\n\r"
    "  0 max_burst_update_even 0 slow_rate2_sel_odd 0 max_burst_odd 0\n\r"
    "  peak_rate_exp_odd 0 peak_rate_man_odd 0 slow_rate2_sel_even 0 max_burst_even\n\r"
    "  0 peak_rate_exp_even 0 peak_rate_man_even 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_SHDS_TBL_SET_SCH_SHDS_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_SHDS_TBL_SET_SCH_SHDS_TBL_SET_TBL_DATA_MAX_BURST_ODD_ID, 
    "max_burst_odd", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.max_burst_odd:\n\r"
    "  uint32 max_burst_odd\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_shds_tbl_set entry_offset_ndx 0 max_burst_update_odd\n\r"
    "  0 max_burst_update_even 0 slow_rate2_sel_odd 0 max_burst_odd 0\n\r"
    "  peak_rate_exp_odd 0 peak_rate_man_odd 0 slow_rate2_sel_even 0 max_burst_even\n\r"
    "  0 peak_rate_exp_even 0 peak_rate_man_even 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_SHDS_TBL_SET_SCH_SHDS_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_SHDS_TBL_SET_SCH_SHDS_TBL_SET_TBL_DATA_PEAK_RATE_EXP_ODD_ID, 
    "peak_rate_exp_odd", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.peak_rate_exp_odd:\n\r"
    "  uint32 peak_rate_exp_odd\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_shds_tbl_set entry_offset_ndx 0 max_burst_update_odd\n\r"
    "  0 max_burst_update_even 0 slow_rate2_sel_odd 0 max_burst_odd 0\n\r"
    "  peak_rate_exp_odd 0 peak_rate_man_odd 0 slow_rate2_sel_even 0 max_burst_even\n\r"
    "  0 peak_rate_exp_even 0 peak_rate_man_even 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_SHDS_TBL_SET_SCH_SHDS_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_SHDS_TBL_SET_SCH_SHDS_TBL_SET_TBL_DATA_PEAK_RATE_MAN_ODD_ID, 
    "peak_rate_man_odd", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.peak_rate_man_odd:\n\r"
    "  uint32 peak_rate_man_odd\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_shds_tbl_set entry_offset_ndx 0 max_burst_update_odd\n\r"
    "  0 max_burst_update_even 0 slow_rate2_sel_odd 0 max_burst_odd 0\n\r"
    "  peak_rate_exp_odd 0 peak_rate_man_odd 0 slow_rate2_sel_even 0 max_burst_even\n\r"
    "  0 peak_rate_exp_even 0 peak_rate_man_even 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_SHDS_TBL_SET_SCH_SHDS_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_SHDS_TBL_SET_SCH_SHDS_TBL_SET_TBL_DATA_SLOW_RATE2_SEL_EVEN_ID, 
    "slow_rate2_sel_even", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.slow_rate2_sel_even:\n\r"
    "  uint32 slow_rate2_sel_even\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_shds_tbl_set entry_offset_ndx 0 max_burst_update_odd\n\r"
    "  0 max_burst_update_even 0 slow_rate2_sel_odd 0 max_burst_odd 0\n\r"
    "  peak_rate_exp_odd 0 peak_rate_man_odd 0 slow_rate2_sel_even 0 max_burst_even\n\r"
    "  0 peak_rate_exp_even 0 peak_rate_man_even 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_SHDS_TBL_SET_SCH_SHDS_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_SHDS_TBL_SET_SCH_SHDS_TBL_SET_TBL_DATA_MAX_BURST_EVEN_ID, 
    "max_burst_even", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.max_burst_even:\n\r"
    "  uint32 max_burst_even\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_shds_tbl_set entry_offset_ndx 0 max_burst_update_odd\n\r"
    "  0 max_burst_update_even 0 slow_rate2_sel_odd 0 max_burst_odd 0\n\r"
    "  peak_rate_exp_odd 0 peak_rate_man_odd 0 slow_rate2_sel_even 0 max_burst_even\n\r"
    "  0 peak_rate_exp_even 0 peak_rate_man_even 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_SHDS_TBL_SET_SCH_SHDS_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_SHDS_TBL_SET_SCH_SHDS_TBL_SET_TBL_DATA_PEAK_RATE_EXP_EVEN_ID, 
    "peak_rate_exp_even", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.peak_rate_exp_even:\n\r"
    "  uint32 peak_rate_exp_even\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_shds_tbl_set entry_offset_ndx 0 max_burst_update_odd\n\r"
    "  0 max_burst_update_even 0 slow_rate2_sel_odd 0 max_burst_odd 0\n\r"
    "  peak_rate_exp_odd 0 peak_rate_man_odd 0 slow_rate2_sel_even 0 max_burst_even\n\r"
    "  0 peak_rate_exp_even 0 peak_rate_man_even 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_SHDS_TBL_SET_SCH_SHDS_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_SHDS_TBL_SET_SCH_SHDS_TBL_SET_TBL_DATA_PEAK_RATE_MAN_EVEN_ID, 
    "peak_rate_man_even", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.peak_rate_man_even:\n\r"
    "  uint32 peak_rate_man_even\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_shds_tbl_set entry_offset_ndx 0 max_burst_update_odd\n\r"
    "  0 max_burst_update_even 0 slow_rate2_sel_odd 0 max_burst_odd 0\n\r"
    "  peak_rate_exp_odd 0 peak_rate_man_odd 0 slow_rate2_sel_even 0 max_burst_even\n\r"
    "  0 peak_rate_exp_even 0 peak_rate_man_even 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_SHDS_TBL_SET_SCH_SHDS_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_SHDS_TBL_GET_SCH_SHDS_TBL_GET_ID, 
    "shds_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_shds_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_SHDS_TBL_GET_SCH_SHDS_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_SEM_TBL_SET_SCH_SEM_TBL_SET_ID, 
    "sem_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_sem_tbl_set entry_offset_ndx 0 sch_enable 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_SEM_TBL_SET_SCH_SEM_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_SEM_TBL_SET_SCH_SEM_TBL_SET_TBL_DATA_SCH_ENABLE_ID, 
    "enable", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.sch_enable:\n\r"
    "  uint32 sch_enable\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_sem_tbl_set entry_offset_ndx 0 sch_enable 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_SEM_TBL_SET_SCH_SEM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_SEM_TBL_GET_SCH_SEM_TBL_GET_ID, 
    "sem_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_sem_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_SEM_TBL_GET_SCH_SEM_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_FSF_TBL_SET_SCH_FSF_TBL_SET_ID, 
    "fsf_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_fsf_tbl_set entry_offset_ndx 0 sfenable 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_FSF_TBL_SET_SCH_FSF_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_FSF_TBL_SET_SCH_FSF_TBL_SET_TBL_DATA_SFENABLE_ID, 
    "sfenable", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.sfenable:\n\r"
    "  uint32 sfenable\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_fsf_tbl_set entry_offset_ndx 0 sfenable 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_FSF_TBL_SET_SCH_FSF_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_FSF_TBL_GET_SCH_FSF_TBL_GET_ID, 
    "fsf_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_fsf_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_FSF_TBL_GET_SCH_FSF_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_FGM_TBL_SET_SCH_FGM_TBL_SET_ID, 
    "fgm_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_fgm_tbl_set entry_offset_ndx 0 flow_group7 0\n\r"
    "  flow_group6 0 flow_group5 0 flow_group4 0 flow_group3 0 flow_group2 0\n\r"
    "  flow_group1 0 flow_group0 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_FGM_TBL_SET_SCH_FGM_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_FGM_TBL_SET_SCH_FGM_TBL_SET_TBL_DATA_FLOW_GROUP7_ID, 
    "flow_group7", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.flow_group7:\n\r"
    "  uint32 flow_group7\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_fgm_tbl_set entry_offset_ndx 0 flow_group7 0\n\r"
    "  flow_group6 0 flow_group5 0 flow_group4 0 flow_group3 0 flow_group2 0\n\r"
    "  flow_group1 0 flow_group0 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_FGM_TBL_SET_SCH_FGM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_FGM_TBL_SET_SCH_FGM_TBL_SET_TBL_DATA_FLOW_GROUP6_ID, 
    "flow_group6", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.flow_group6:\n\r"
    "  uint32 flow_group6\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_fgm_tbl_set entry_offset_ndx 0 flow_group7 0\n\r"
    "  flow_group6 0 flow_group5 0 flow_group4 0 flow_group3 0 flow_group2 0\n\r"
    "  flow_group1 0 flow_group0 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_FGM_TBL_SET_SCH_FGM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_FGM_TBL_SET_SCH_FGM_TBL_SET_TBL_DATA_FLOW_GROUP5_ID, 
    "flow_group5", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.flow_group5:\n\r"
    "  uint32 flow_group5\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_fgm_tbl_set entry_offset_ndx 0 flow_group7 0\n\r"
    "  flow_group6 0 flow_group5 0 flow_group4 0 flow_group3 0 flow_group2 0\n\r"
    "  flow_group1 0 flow_group0 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_FGM_TBL_SET_SCH_FGM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_FGM_TBL_SET_SCH_FGM_TBL_SET_TBL_DATA_FLOW_GROUP4_ID, 
    "flow_group4", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.flow_group4:\n\r"
    "  uint32 flow_group4\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_fgm_tbl_set entry_offset_ndx 0 flow_group7 0\n\r"
    "  flow_group6 0 flow_group5 0 flow_group4 0 flow_group3 0 flow_group2 0\n\r"
    "  flow_group1 0 flow_group0 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_FGM_TBL_SET_SCH_FGM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_FGM_TBL_SET_SCH_FGM_TBL_SET_TBL_DATA_FLOW_GROUP3_ID, 
    "flow_group3", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.flow_group3:\n\r"
    "  uint32 flow_group3\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_fgm_tbl_set entry_offset_ndx 0 flow_group7 0\n\r"
    "  flow_group6 0 flow_group5 0 flow_group4 0 flow_group3 0 flow_group2 0\n\r"
    "  flow_group1 0 flow_group0 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_FGM_TBL_SET_SCH_FGM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_FGM_TBL_SET_SCH_FGM_TBL_SET_TBL_DATA_FLOW_GROUP2_ID, 
    "flow_group2", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.flow_group2:\n\r"
    "  uint32 flow_group2\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_fgm_tbl_set entry_offset_ndx 0 flow_group7 0\n\r"
    "  flow_group6 0 flow_group5 0 flow_group4 0 flow_group3 0 flow_group2 0\n\r"
    "  flow_group1 0 flow_group0 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_FGM_TBL_SET_SCH_FGM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_FGM_TBL_SET_SCH_FGM_TBL_SET_TBL_DATA_FLOW_GROUP1_ID, 
    "flow_group1", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.flow_group1:\n\r"
    "  uint32 flow_group1\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_fgm_tbl_set entry_offset_ndx 0 flow_group7 0\n\r"
    "  flow_group6 0 flow_group5 0 flow_group4 0 flow_group3 0 flow_group2 0\n\r"
    "  flow_group1 0 flow_group0 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_FGM_TBL_SET_SCH_FGM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_FGM_TBL_SET_SCH_FGM_TBL_SET_TBL_DATA_FLOW_GROUP0_ID, 
    "flow_group0", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.flow_group0:\n\r"
    "  uint32 flow_group0\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_fgm_tbl_set entry_offset_ndx 0 flow_group7 0\n\r"
    "  flow_group6 0 flow_group5 0 flow_group4 0 flow_group3 0 flow_group2 0\n\r"
    "  flow_group1 0 flow_group0 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_FGM_TBL_SET_SCH_FGM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_FGM_TBL_GET_SCH_FGM_TBL_GET_ID, 
    "fgm_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_fgm_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_FGM_TBL_GET_SCH_FGM_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_SHC_TBL_SET_SCH_SHC_TBL_SET_ID, 
    "shc_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_shc_tbl_set entry_offset_ndx 0 hrmask_type 0 hrmode 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_SHC_TBL_SET_SCH_SHC_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_SHC_TBL_SET_SCH_SHC_TBL_SET_TBL_DATA_HRMASK_TYPE_ID, 
    "hrmask_type", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.hrmask_type:\n\r"
    "  uint32 hrmask_type\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_shc_tbl_set entry_offset_ndx 0 hrmask_type 0 hrmode 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_SHC_TBL_SET_SCH_SHC_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_SHC_TBL_SET_SCH_SHC_TBL_SET_TBL_DATA_HRMODE_ID, 
    "hrmode", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.hrmode:\n\r"
    "  uint32 hrmode\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_shc_tbl_set entry_offset_ndx 0 hrmask_type 0 hrmode 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_SHC_TBL_SET_SCH_SHC_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_SHC_TBL_GET_SCH_SHC_TBL_GET_ID, 
    "shc_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_shc_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_SHC_TBL_GET_SCH_SHC_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_SCC_TBL_SET_SCH_SCC_TBL_SET_ID, 
    "scc_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_scc_tbl_set entry_offset_ndx 0 clsch_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_SCC_TBL_SET_SCH_SCC_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_SCC_TBL_SET_SCH_SCC_TBL_SET_TBL_DATA_CLSCH_TYPE_ID, 
    "clsch_type", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.clsch_type:\n\r"
    "  uint32 clsch_type\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_scc_tbl_set entry_offset_ndx 0 clsch_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_SCC_TBL_SET_SCH_SCC_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_SCC_TBL_GET_SCH_SCC_TBL_GET_ID, 
    "scc_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_scc_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_SCC_TBL_GET_SCH_SCC_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_SCT_TBL_SET_SCH_SCT_TBL_SET_ID, 
    "sct_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_sct_tbl_set entry_offset_ndx 0 enh_clsphigh 0\n\r"
    "  enh_clen 0 af3_inv_weight 0 af2_inv_weight 0 af1_inv_weight 0 af0_inv_weight\n\r"
    "  0 clconfig 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_SCT_TBL_SET_SCH_SCT_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_SCT_TBL_SET_SCH_SCT_TBL_SET_TBL_DATA_ENH_CLSPHIGH_ID, 
    "enh_clsphigh", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.enh_clsphigh:\n\r"
    "  uint32 enh_clsphigh\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_sct_tbl_set entry_offset_ndx 0 enh_clsphigh 0\n\r"
    "  enh_clen 0 af3_inv_weight 0 af2_inv_weight 0 af1_inv_weight 0 af0_inv_weight\n\r"
    "  0 clconfig 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_SCT_TBL_SET_SCH_SCT_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_SCT_TBL_SET_SCH_SCT_TBL_SET_TBL_DATA_ENH_CLEN_ID, 
    "enh_clen", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.enh_clen:\n\r"
    "  uint32 enh_clen\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_sct_tbl_set entry_offset_ndx 0 enh_clsphigh 0\n\r"
    "  enh_clen 0 af3_inv_weight 0 af2_inv_weight 0 af1_inv_weight 0 af0_inv_weight\n\r"
    "  0 clconfig 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_SCT_TBL_SET_SCH_SCT_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_SCT_TBL_SET_SCH_SCT_TBL_SET_TBL_DATA_AF3_INV_WEIGHT_ID, 
    "af3_inv_weight", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.af3_inv_weight:\n\r"
    "  uint32 af3_inv_weight\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_sct_tbl_set entry_offset_ndx 0 enh_clsphigh 0\n\r"
    "  enh_clen 0 af3_inv_weight 0 af2_inv_weight 0 af1_inv_weight 0 af0_inv_weight\n\r"
    "  0 clconfig 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_SCT_TBL_SET_SCH_SCT_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_SCT_TBL_SET_SCH_SCT_TBL_SET_TBL_DATA_AF2_INV_WEIGHT_ID, 
    "af2_inv_weight", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.af2_inv_weight:\n\r"
    "  uint32 af2_inv_weight\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_sct_tbl_set entry_offset_ndx 0 enh_clsphigh 0\n\r"
    "  enh_clen 0 af3_inv_weight 0 af2_inv_weight 0 af1_inv_weight 0 af0_inv_weight\n\r"
    "  0 clconfig 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_SCT_TBL_SET_SCH_SCT_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_SCT_TBL_SET_SCH_SCT_TBL_SET_TBL_DATA_AF1_INV_WEIGHT_ID, 
    "af1_inv_weight", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.af1_inv_weight:\n\r"
    "  uint32 af1_inv_weight\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_sct_tbl_set entry_offset_ndx 0 enh_clsphigh 0\n\r"
    "  enh_clen 0 af3_inv_weight 0 af2_inv_weight 0 af1_inv_weight 0 af0_inv_weight\n\r"
    "  0 clconfig 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_SCT_TBL_SET_SCH_SCT_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_SCT_TBL_SET_SCH_SCT_TBL_SET_TBL_DATA_AF0_INV_WEIGHT_ID, 
    "af0_inv_weight", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.af0_inv_weight:\n\r"
    "  uint32 af0_inv_weight\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_sct_tbl_set entry_offset_ndx 0 enh_clsphigh 0\n\r"
    "  enh_clen 0 af3_inv_weight 0 af2_inv_weight 0 af1_inv_weight 0 af0_inv_weight\n\r"
    "  0 clconfig 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_SCT_TBL_SET_SCH_SCT_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_SCT_TBL_SET_SCH_SCT_TBL_SET_TBL_DATA_CLCONFIG_ID, 
    "clconfig", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.clconfig:\n\r"
    "  uint32 clconfig\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_sct_tbl_set entry_offset_ndx 0 enh_clsphigh 0\n\r"
    "  enh_clen 0 af3_inv_weight 0 af2_inv_weight 0 af1_inv_weight 0 af0_inv_weight\n\r"
    "  0 clconfig 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_SCT_TBL_SET_SCH_SCT_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_SCT_TBL_GET_SCH_SCT_TBL_GET_ID, 
    "sct_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_sct_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_SCT_TBL_GET_SCH_SCT_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_FQM_TBL_SET_SCH_FQM_TBL_SET_ID, 
    "fqm_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_fqm_tbl_set entry_offset_ndx 0 flow_slow_enable 0\n\r"
    "  sub_flow_mode 0 base_queue_num 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_FQM_TBL_SET_SCH_FQM_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_FQM_TBL_SET_SCH_FQM_TBL_SET_TBL_DATA_FLOW_SLOW_ENABLE_ID, 
    "flow_slow_enable", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.flow_slow_enable:\n\r"
    "  uint32 flow_slow_enable\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_fqm_tbl_set entry_offset_ndx 0 flow_slow_enable 0\n\r"
    "  sub_flow_mode 0 base_queue_num 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_FQM_TBL_SET_SCH_FQM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_FQM_TBL_SET_SCH_FQM_TBL_SET_TBL_DATA_SUB_FLOW_MODE_ID, 
    "sub_flow_mode", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.sub_flow_mode:\n\r"
    "  uint32 sub_flow_mode\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_fqm_tbl_set entry_offset_ndx 0 flow_slow_enable 0\n\r"
    "  sub_flow_mode 0 base_queue_num 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_FQM_TBL_SET_SCH_FQM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_FQM_TBL_SET_SCH_FQM_TBL_SET_TBL_DATA_BASE_QUEUE_NUM_ID, 
    "base_queue_num", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.base_queue_num:\n\r"
    "  uint32 base_queue_num\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_fqm_tbl_set entry_offset_ndx 0 flow_slow_enable 0\n\r"
    "  sub_flow_mode 0 base_queue_num 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_FQM_TBL_SET_SCH_FQM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_FQM_TBL_GET_SCH_FQM_TBL_GET_ID, 
    "fqm_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_fqm_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_FQM_TBL_GET_SCH_FQM_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_FFM_TBL_SET_SCH_FFM_TBL_SET_ID, 
    "ffm_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_ffm_tbl_set entry_offset_ndx 0 device_number 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_FFM_TBL_SET_SCH_FFM_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_FFM_TBL_SET_SCH_FFM_TBL_SET_TBL_DATA_DEVICE_NUMBER_ID, 
    "device_number", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.device_number:\n\r"
    "  uint32 device_number\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_ffm_tbl_set entry_offset_ndx 0 device_number 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_FFM_TBL_SET_SCH_FFM_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_FFM_TBL_GET_SCH_FFM_TBL_GET_ID, 
    "ffm_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_ffm_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_FFM_TBL_GET_SCH_FFM_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_TMC_TBL_SET_SCH_TMC_TBL_SET_ID, 
    "soc_tmctbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_tmc_tbl_set entry_offset_ndx 0 slow_status 0\n\r"
    "  token_count 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_TMC_TBL_SET_SCH_TMC_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_TMC_TBL_SET_SCH_TMC_TBL_SET_TBL_DATA_SLOW_STATUS_ID, 
    "slow_status", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.slow_status:\n\r"
    "  uint32 slow_status\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_tmc_tbl_set entry_offset_ndx 0 slow_status 0\n\r"
    "  token_count 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_TMC_TBL_SET_SCH_TMC_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_TMC_TBL_SET_SCH_TMC_TBL_SET_TBL_DATA_TOKEN_COUNT_ID, 
    "token_count", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.token_count:\n\r"
    "  uint32 token_count\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_tmc_tbl_set entry_offset_ndx 0 slow_status 0\n\r"
    "  token_count 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_TMC_TBL_SET_SCH_TMC_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_TMC_TBL_GET_SCH_TMC_TBL_GET_ID, 
    "soc_tmctbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_tmc_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_TMC_TBL_GET_SCH_TMC_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_PQS_TBL_SET_SCH_PQS_TBL_SET_ID, 
    "pqs_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_pqs_tbl_set entry_offset_ndx 0 aging_bit 0 flow_id 0\n\r"
    "  max_qsz 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_PQS_TBL_SET_SCH_PQS_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_PQS_TBL_SET_SCH_PQS_TBL_SET_TBL_DATA_AGING_BIT_ID, 
    "aging_bit", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.aging_bit:\n\r"
    "  uint32 aging_bit\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_pqs_tbl_set entry_offset_ndx 0 aging_bit 0 flow_id 0\n\r"
    "  max_qsz 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_PQS_TBL_SET_SCH_PQS_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_PQS_TBL_SET_SCH_PQS_TBL_SET_TBL_DATA_FLOW_ID_ID, 
    "flow_id", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.flow_id:\n\r"
    "  uint32 flow_id\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_pqs_tbl_set entry_offset_ndx 0 aging_bit 0 flow_id 0\n\r"
    "  max_qsz 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_PQS_TBL_SET_SCH_PQS_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_PQS_TBL_SET_SCH_PQS_TBL_SET_TBL_DATA_MAX_QSZ_ID, 
    "max_qsz", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.max_qsz:\n\r"
    "  uint32 max_qsz\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_pqs_tbl_set entry_offset_ndx 0 aging_bit 0 flow_id 0\n\r"
    "  max_qsz 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_PQS_TBL_SET_SCH_PQS_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_PQS_TBL_GET_SCH_PQS_TBL_GET_ID, 
    "pqs_tbl_get", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_pqs_tbl_get entry_offset_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_PQS_TBL_GET_SCH_PQS_TBL_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_SCHEDULER_INIT_TBL_SET_SCH_SCHEDULER_INIT_TBL_SET_ID, 
    "scheduler_init_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_scheduler_init_tbl_set entry_offset_ndx 0 schinit 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_SCHEDULER_INIT_TBL_SET_SCH_SCHEDULER_INIT_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_SCHEDULER_INIT_TBL_SET_SCH_SCHEDULER_INIT_TBL_SET_TBL_DATA_SCHINIT_ID, 
    "schinit", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.schinit:\n\r"
    "  uint32 schinit\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_scheduler_init_tbl_set entry_offset_ndx 0 schinit 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_SCHEDULER_INIT_TBL_SET_SCH_SCHEDULER_INIT_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  }, 
  { 
    PARAM_PB_SCH_FORCE_STATUS_MESSAGE_TBL_SET_SCH_FORCE_STATUS_MESSAGE_TBL_SET_ID, 
    "force_status_message_tbl_set", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_force_status_message_tbl_set entry_offset_ndx 0\n\r"
    "  message_flow_id 0 message_type_ 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_FORCE_STATUS_MESSAGE_TBL_SET_SCH_FORCE_STATUS_MESSAGE_TBL_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_FORCE_STATUS_MESSAGE_TBL_SET_SCH_FORCE_STATUS_MESSAGE_TBL_SET_TBL_DATA_MESSAGE_FLOW_ID_ID, 
    "message_flow_id", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.message_flow_id:\n\r"
    "  uint32 message_flow_id\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_force_status_message_tbl_set entry_offset_ndx 0\n\r"
    "  message_flow_id 0 message_type_ 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_FORCE_STATUS_MESSAGE_TBL_SET_SCH_FORCE_STATUS_MESSAGE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_SCH_FORCE_STATUS_MESSAGE_TBL_SET_SCH_FORCE_STATUS_MESSAGE_TBL_SET_TBL_DATA_MESSAGE_TYPE__ID, 
    "message_type_", 
    (PARAM_VAL_RULES *)&soc_pb_api_free_vals[0], 
    (sizeof(soc_pb_api_free_vals) / sizeof(soc_pb_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.message_type_:\n\r"
    "  uint32 message_type_\n\r"
    "",
#if UI_PB_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  soc_pb_api soc_pb_ind_acc sch_force_status_message_tbl_set entry_offset_ndx 0\n\r"
    "  message_flow_id 0 message_type_ 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PB_SCH_FORCE_STATUS_MESSAGE_TBL_SET_SCH_FORCE_STATUS_MESSAGE_TBL_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  }, 
  { 
    PARAM_PB_IND_ACC_ID_IPS, 
    "ips", 
    (PARAM_VAL_RULES *)&soc_pb_api_empty_vals[0], 
    (sizeof(soc_pb_api_empty_vals) / sizeof(soc_pb_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
    "", 
    "",
    BIT_UI_16_RNG(FUN_PB_IND_ACC_FIRST_ID_IPS,FUN_PB_IND_ACC_LAST_ID_IPS), 
    1, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IND_ACC_ID_IPT, 
    "ipt", 
    (PARAM_VAL_RULES *)&soc_pb_api_empty_vals[0], 
    (sizeof(soc_pb_api_empty_vals) / sizeof(soc_pb_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
    "", 
    "",
    BIT_UI_16_RNG(FUN_PB_IND_ACC_FIRST_ID_IPT,FUN_PB_IND_ACC_LAST_ID_IPT), 
    1, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IND_ACC_ID_DPI, 
    "dpi", 
    (PARAM_VAL_RULES *)&soc_pb_api_empty_vals[0], 
    (sizeof(soc_pb_api_empty_vals) / sizeof(soc_pb_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
    "", 
    "",
    BIT_UI_16_RNG(FUN_PB_IND_ACC_FIRST_ID_DPI,FUN_PB_IND_ACC_LAST_ID_DPI), 
    1, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IND_ACC_ID_FDT, 
    "fdt", 
    (PARAM_VAL_RULES *)&soc_pb_api_empty_vals[0], 
    (sizeof(soc_pb_api_empty_vals) / sizeof(soc_pb_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
    "", 
    "",
    BIT_UI_16_RNG(FUN_PB_IND_ACC_FIRST_ID_FDT,FUN_PB_IND_ACC_LAST_ID_FDT), 
    1, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IND_ACC_ID_RTP, 
    "rtp", 
    (PARAM_VAL_RULES *)&soc_pb_api_empty_vals[0], 
    (sizeof(soc_pb_api_empty_vals) / sizeof(soc_pb_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
    "", 
    "",
    BIT_UI_16_RNG(FUN_PB_IND_ACC_FIRST_ID_RTP,FUN_PB_IND_ACC_LAST_ID_RTP), 
    1, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IND_ACC_ID_EGQ, 
    "egq", 
    (PARAM_VAL_RULES *)&soc_pb_api_empty_vals[0], 
    (sizeof(soc_pb_api_empty_vals) / sizeof(soc_pb_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
    "", 
    "",
    BIT_UI_16_RNG(FUN_PB_IND_ACC_FIRST_ID_EGQ,FUN_PB_IND_ACC_LAST_ID_EGQ), 
    1, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IND_ACC_ID_IHB, 
    "ihb", 
    (PARAM_VAL_RULES *)&soc_pb_api_empty_vals[0], 
    (sizeof(soc_pb_api_empty_vals) / sizeof(soc_pb_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
    "", 
    "",
    BIT_UI_16_RNG(FUN_PB_IND_ACC_FIRST_ID_IHB,FUN_PB_IND_ACC_LAST_ID_IHB), 
    1, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IND_ACC_ID_EPNI, 
    "epni", 
    (PARAM_VAL_RULES *)&soc_pb_api_empty_vals[0], 
    (sizeof(soc_pb_api_empty_vals) / sizeof(soc_pb_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
    "", 
    "",
    BIT_UI_16_RNG(FUN_PB_IND_ACC_FIRST_ID_EPNI,FUN_PB_IND_ACC_LAST_ID_EPNI), 
    1, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IND_ACC_ID_CFC, 
    "cfc", 
    (PARAM_VAL_RULES *)&soc_pb_api_empty_vals[0], 
    (sizeof(soc_pb_api_empty_vals) / sizeof(soc_pb_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
    "", 
    "",
    BIT_UI_16_RNG(FUN_PB_IND_ACC_FIRST_ID_CFC,FUN_PB_IND_ACC_LAST_ID_CFC), 
    1, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PB_IND_ACC_ID_SCH, 
    "sch", 
    (PARAM_VAL_RULES *)&soc_pb_api_empty_vals[0], 
    (sizeof(soc_pb_api_empty_vals) / sizeof(soc_pb_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
    "", 
    "",
    BIT_UI_16_RNG(FUN_PB_IND_ACC_FIRST_ID_SCH,FUN_PB_IND_ACC_LAST_ID_SCH), 
    1, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
#endif /* } soc_pb_ind_acc*/

/* 
 * Last element. Do not remove.
 */ 
  { 
    PARAM_END_OF_LIST 
  } 
} 
#endif  
;  
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __UI_ROM_DEFI_PB_PP_ACC2_INCLUDED__*/
#endif

