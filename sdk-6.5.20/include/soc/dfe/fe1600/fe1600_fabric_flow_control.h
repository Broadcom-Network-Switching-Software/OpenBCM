/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * FE1600 FABRIC FLOW CONTROL H
 */
 
#ifndef _SOC_FE1600_FABRIC_FLOW_CONTROL_H_
#define _SOC_FE1600_FABRIC_FLOW_CONTROL_H_

#include <bcm/fabric.h>
#include <soc/dfe/cmn/dfe_defs.h>
#include <soc/error.h>

soc_error_t soc_fe1600_fabric_flow_control_rci_gci_control_source_set(int unit, bcm_fabric_control_t type, soc_dcmn_fabric_pipe_t val);
soc_error_t soc_fe1600_fabric_flow_control_rci_gci_control_source_get(int unit, bcm_fabric_control_t type, soc_dcmn_fabric_pipe_t* val);
soc_error_t soc_fe1600_fabric_flow_control_rx_llfc_threshold_validate(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value);
soc_error_t soc_fe1600_fabric_flow_control_rx_llfc_threshold_set(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int value);
soc_error_t soc_fe1600_fabric_flow_control_rx_llfc_threshold_get(int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int* value);
soc_error_t soc_fe1600_fabric_flow_control_rx_gci_threshold_validate(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value, int is_fe1, int is_fe3);
soc_error_t soc_fe1600_fabric_flow_control_rx_gci_threshold_set(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int value);
soc_error_t soc_fe1600_fabric_flow_control_rx_gci_threshold_get(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int* value);
soc_error_t soc_fe1600_fabric_flow_control_rx_drop_threshold_validate(int unit, bcm_fabric_link_threshold_type_t type, soc_dcmn_fabric_pipe_t pipe, int value);
soc_error_t soc_fe1600_fabric_flow_control_rx_drop_threshold_set(int unit, bcm_fabric_link_threshold_type_t type, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int value);
soc_error_t soc_fe1600_fabric_flow_control_rx_drop_threshold_get(int unit, bcm_fabric_link_threshold_type_t type, soc_dcmn_fabric_control_source_t pipe, int fe1, int fe3, int* value);
soc_error_t soc_fe1600_fabric_flow_control_tx_rci_threshold_validate(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value);
soc_error_t soc_fe1600_fabric_flow_control_tx_rci_threshold_set(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int value);
soc_error_t soc_fe1600_fabric_flow_control_tx_rci_threshold_get(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int* value);
soc_error_t soc_fe1600_fabric_flow_control_tx_bypass_llfc_threshold_validate(int unit,soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value);
soc_error_t soc_fe1600_fabric_flow_control_tx_bypass_llfc_threshold_set(int unit,soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int value);
soc_error_t soc_fe1600_fabric_flow_control_tx_bypass_llfc_threshold_get(int unit,soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int* value);
soc_error_t soc_fe1600_fabric_flow_control_tx_gci_threshold_validate(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value);
soc_error_t soc_fe1600_fabric_flow_control_tx_gci_threshold_set(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int value);
soc_error_t soc_fe1600_fabric_flow_control_tx_gci_threshold_get(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int* value);
soc_error_t soc_fe1600_fabric_flow_control_tx_drop_threshold_validate(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value);
soc_error_t soc_fe1600_fabric_flow_control_tx_drop_threshold_set(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int value);
soc_error_t soc_fe1600_fabric_flow_control_tx_drop_threshold_get(int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int* value);
soc_error_t soc_fe1600_fabric_flow_control_thresholds_flags_validate(int unit,uint32 flags);
soc_error_t soc_fe1600_fabric_links_link_type_set(int unit, soc_dcmn_fabric_pipe_t pipe, soc_dfe_fabric_link_fifo_type_index_t type_index, int is_rx, int is_tx, int is_fe1, int is_fe3, uint32 links_count, soc_port_t* links);
soc_error_t soc_fe1600_fabric_links_link_type_get(int unit, soc_dcmn_fabric_pipe_t pipe, soc_dfe_fabric_link_fifo_type_index_t type_index, int is_rx, int is_tx, int is_fe1, int is_fe3, uint32 links_count_max, soc_port_t* links, uint32* links_count);



#endif 
