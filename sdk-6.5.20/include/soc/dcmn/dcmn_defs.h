/*
* 
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
*
* This file contains structure and routine declarations for the
* Switch-on-a-Chip Driver.
*
* This file also includes the more common include files so the
* individual driver files don't have to include as much.
*/

#include <soc/sand/sand_defs.h>

#ifndef _SOC_DCMN_DEFS_H
#define _SOC_DCMN_DEFS_H

#define SOC_DCMN_NOF_LINKS_IN_MAC SOC_SAND_NOF_LINKS_IN_MAC


#define SOC_DCMN_RESET_ACTION_IN_RESET          SOC_SAND_RESET_ACTION_IN_RESET
#define SOC_DCMN_RESET_ACTION_OUT_RESET         SOC_SAND_RESET_ACTION_OUT_RESET
#define SOC_DCMN_RESET_ACTION_INOUT_RESET       SOC_SAND_RESET_ACTION_INOUT_RESET

#define SOC_DCMN_RESET_MODE_HARD_RESET                              SOC_SAND_RESET_MODE_HARD_RESET
#define SOC_DCMN_RESET_MODE_BLOCKS_RESET                            SOC_SAND_RESET_MODE_BLOCKS_RESET
#define SOC_DCMN_RESET_MODE_BLOCKS_SOFT_RESET                       SOC_SAND_RESET_MODE_BLOCKS_SOFT_RESET
#define SOC_DCMN_RESET_MODE_BLOCKS_SOFT_INGRESS_RESET               SOC_SAND_RESET_MODE_BLOCKS_SOFT_INGRESS_RESET
#define SOC_DCMN_RESET_MODE_BLOCKS_SOFT_EGRESS_RESET                SOC_SAND_RESET_MODE_BLOCKS_SOFT_EGRESS_RESET
#define SOC_DCMN_RESET_MODE_INIT_RESET                              SOC_SAND_RESET_MODE_INIT_RESET
#define SOC_DCMN_RESET_MODE_REG_ACCESS                              SOC_SAND_RESET_MODE_REG_ACCESS
#define SOC_DCMN_RESET_MODE_ENABLE_TRAFFIC                          SOC_SAND_RESET_MODE_ENABLE_TRAFFIC
#define SOC_DCMN_RESET_MODE_BLOCKS_AND_FABRIC_SOFT_RESET            SOC_SAND_RESET_MODE_BLOCKS_AND_FABRIC_SOFT_RESET
#define SOC_DCMN_RESET_MODE_BLOCKS_AND_FABRIC_SOFT_INGRESS_RESET    SOC_SAND_RESET_MODE_BLOCKS_AND_FABRIC_SOFT_INGRESS_RESET
#define SOC_DCMN_RESET_MODE_BLOCKS_AND_FABRIC_SOFT_EGRESS_RESET     SOC_SAND_RESET_MODE_BLOCKS_AND_FABRIC_SOFT_EGRESS_RESET
#define SOC_DCMN_RESET_MODE_BLOCKS_SOFT_RESET_DIRECT                SOC_SAND_RESET_MODE_BLOCKS_SOFT_RESET_DIRECT

#define SOC_DCMN_DIAG_NIF_NOF_REF_CLK_REMINDER_DIGITS                (2)

typedef enum soc_dcmn_loopback_mode_e{
    soc_dcmn_loopback_mode_none    = 0,
    soc_dcmn_loopback_mode_mac_outer,
    soc_dcmn_loopback_mode_mac_pcs, 
    soc_dcmn_loopback_mode_mac_async_fifo,  
    soc_dcmn_loopback_mode_phy_rloop,  
    soc_dcmn_loopback_mode_phy_gloop 
} soc_dcmn_loopback_mode_t;

typedef enum filter_type_e {
    soc_dcmn_filter_type_source_id = 0, 
    soc_dcmn_filter_type_multicast_id= 1, 
    soc_dcmn_filter_type_priority = 2,
    soc_dcmn_filter_type_dest_id = 3,
    soc_dcmn_filter_type_traffic_cast = 4
} soc_dcmn_filter_type_t;

typedef enum filter_control_type_e {
    soc_dcmn_filter_control_cell_type_source_device = 0,
    soc_dcmn_filter_control_cell_type_dest_device
} soc_dcmn_control_cell_filter_type_t;


typedef enum control_cell_types_e {
    soc_dcmn_flow_status_cell = 0,
    soc_dcmn_credit_cell,
    soc_dcmn_reachability_cell,
    soc_dcmn_fe2_filtered_cell = 6
} soc_dcmn_control_cell_types_t;


typedef struct soc_dcmn_captured_control_cells_s {
    int valid;
    soc_dcmn_control_cell_types_t control_type;
    int dest_device;
    int source_device;
    int dest_port;
    int src_queue_num;
    int dest_queue_num;
    int sub_flow_id;
    int flow_id;
    int reachability_bitmap;
    int base_index;
    int source_link_number;
} soc_dcmn_captured_control_cell_t;

typedef enum soc_dcmn_device_type_actual_value_e{
    soc_dcmn_device_type_actual_value_FAP_1 = 0,
    soc_dcmn_device_type_actual_value_FAP   = 5,
    soc_dcmn_device_type_actual_value_FIP   = 1,
    soc_dcmn_device_type_actual_value_FOP   = 4,
    soc_dcmn_device_type_actual_value_FE1   = 6,
    soc_dcmn_device_type_actual_value_FE2   = 3,
    soc_dcmn_device_type_actual_value_FE2_1 = 7,
    soc_dcmn_device_type_actual_value_FE3   = 2
} soc_dcmn_device_type_actual_value_t;

typedef enum soc_dcmn_init_serdes_ref_clock_s
{
    
    soc_dcmn_init_serdes_ref_clock_disable = -1,
    
    soc_dcmn_init_serdes_ref_clock_125 = 0,
    
    soc_dcmn_init_serdes_ref_clock_156_25 = 1,
    
    soc_dcmn_init_serdes_ref_clock_25 = 2,
    
    soc_dcmn_init_serdes_nof_ref_clocks = 3
}soc_dcmn_init_serdes_ref_clock_t;

typedef enum soc_dcmn_isolation_status_e
{
  soc_dcmn_isolation_status_active = 0,
  soc_dcmn_isolation_status_isolated = 1
} soc_dcmn_isolation_status_t;

typedef enum soc_dcmn_fabric_control_source_e
{
  soc_dcmn_fabric_control_source_none = 0,
  soc_dcmn_fabric_control_source_primary = 1,
  soc_dcmn_fabric_control_source_secondary = 2,
  soc_dcmn_fabric_control_source_both = 3
} soc_dcmn_fabric_control_source_t;

#endif 
