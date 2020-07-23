

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNXF_DATA_PROPERTY_H_

#define _DNXF_DATA_PROPERTY_H_

#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data.h>



#ifndef BCM_DNXF_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif




shr_error_e dnxf_data_property_port_general_fw_load_method_read(
    int unit,
    uint32 *fw_load_method);


shr_error_e dnxf_data_property_port_general_fw_crc_check_read(
    int unit,
    uint32 *fw_crc_check);


shr_error_e dnxf_data_property_port_general_fw_load_verify_read(
    int unit,
    uint32 *fw_load_verify);




shr_error_e dnxf_data_property_port_static_add_serdes_tx_taps_read(
    int unit,
    int port,
    dnxf_data_port_static_add_serdes_tx_taps_t *serdes_tx_taps);



shr_error_e dnxf_data_property_port_lane_map_info_read(
    int unit,
    int lane,
    dnxf_data_port_lane_map_info_t *info);




























shr_error_e dnxf_data_property_fabric_pipes_map_read(
    int unit,
    dnxf_data_fabric_pipes_map_t *map);




shr_error_e dnxf_data_property_fabric_fifos_rx_depth_per_pipe_read(
    int unit,
    int pipe_id,
    dnxf_data_fabric_fifos_rx_depth_per_pipe_t *rx_depth_per_pipe);


shr_error_e dnxf_data_property_fabric_fifos_mid_depth_per_pipe_read(
    int unit,
    int pipe_id,
    dnxf_data_fabric_fifos_mid_depth_per_pipe_t *mid_depth_per_pipe);


shr_error_e dnxf_data_property_fabric_fifos_tx_depth_per_pipe_read(
    int unit,
    int pipe_id,
    dnxf_data_fabric_fifos_tx_depth_per_pipe_t *tx_depth_per_pipe);























shr_error_e dnxf_data_property_module_testing_property_methods_custom_val_read(
    int unit,
    int link,
    int pipe,
    uint32 *val);




#endif 

