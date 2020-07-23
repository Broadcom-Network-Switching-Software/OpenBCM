/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        mbcm.h
 * Purpose:     Multiplexing of the bcm layer - For fabric element functions
 *
 * Different chip families require such different implementations
 * of some basic BCM layer functionality that the functions are
 * multiplexed to allow a fast runtime decision as to which function
 * to call.  This file contains the basic declarations for this
 * process.
 *
 * This code allows to use the same MBCM_DFE_DRIVER_CALL API independently of the chip type
 *
 * See internal/design/soft_arch/xgs_plan.txt for more info.
 *
 * Conventions:
 *    MBCM is the multiplexed bcm prefix
 *    _f is the function type declaration postfix
 */

#ifndef _SOC_DFE_CMN_MCBM_H
#define _SOC_DFE_CMN_MCBM_H

#include <soc/chip.h>
#include <soc/error.h>

#include <soc/dcmn/dcmn_defs.h>
#include <soc/dcmn/vsc256_fabric_cell.h>
#include <soc/dcmn/dcmn_fabric_cell.h>
#include <soc/dcmn/dcmn_port.h>

#include <soc/dfe/cmn/dfe_defs.h>
#include <soc/dfe/cmn/dfe_fabric.h>
#include <soc/dfe/cmn/dfe_fabric_cell.h>
#include <soc/dfe/cmn/dfe_fabric_cell_snake_test.h>
#include <soc/dfe/cmn/dfe_diag.h>
#include <soc/dfe/cmn/dfe_property.h>
#include <soc/dfe/cmn/dfe_fifo_dma.h>
#include <soc/dfe/cmn/dfe_port.h>
#include <soc/dfe/cmn/dfe_multicast.h>

#include <bcm/fabric.h>



typedef enum soc_dfe_chip_family_e {
    BCM_FAMILY_FE1600,
    BCM_FAMILY_FE3200
} soc_dfe_chip_family_t;

#define BCM_DFE_CHIP_FAMILY(unit)       (mbcm_dfe_family[unit])



typedef int (*mbcm_dfe_tbl_is_dynamic_f) (int unit, soc_mem_t mem);
typedef int (*mbcm_dfe_ser_init_f) (int unit);
typedef int (*mbcm_dfe_reset_device_f) (int unit);
typedef int (*mbcm_dfe_drv_soft_init_f) (int unit, uint32 soft_reset_mode_flags);
typedef int (*mbcm_dfe_drv_blocks_reset_f) (int unit, int force_blocks_reset_value, soc_reg_above_64_val_t *block_bitmap);
typedef int (*mbcm_dfe_drv_reg_access_only_reset_f) (int unit);
typedef int (*mbcm_dfe_TDM_fragment_validate_f) (int unit, uint32 tdm_frag);
typedef soc_error_t (*mbcm_dfe_sr_cell_send_f) (int unit, const vsc256_sr_cell_t* cell);
typedef soc_error_t (*mbcm_dfe_cell_filter_set_f) (int unit, uint32 flags, uint32 array_size, soc_dcmn_filter_type_t* filter_type_arr, uint32* filter_type_val);
typedef soc_error_t (*mbcm_dfe_cell_filter_clear_f) (int unit);
typedef soc_error_t (*mbcm_dfe_cell_filter_count_get_f) (int unit, int *count);
typedef soc_error_t (*mbcm_dfe_control_cell_filter_set_f) (int unit, uint32 flags, soc_dcmn_control_cell_types_t cell_type, uint32 array_size, soc_dcmn_control_cell_filter_type_t* control_cell_filter_type_arr, uint32* filter_type_val);
typedef soc_error_t (*mbcm_dfe_control_cell_filter_clear_f) (int unit);
typedef soc_error_t (*mbcm_dfe_control_cell_filter_receive_f) (int unit,  soc_dcmn_captured_control_cell_t* data_out);
typedef int (*mbcm_dfe_cell_snake_test_prepare_f) (int unit, uint32 flags);
typedef int (*mbcm_dfe_cell_snake_test_run_f) (int unit, uint32 flags, soc_fabric_cell_snake_test_results_t* results);
typedef soc_error_t (*mbcm_dfe_diag_fabric_cell_snake_test_interrupts_name_get_f) (int unit, const soc_dfe_diag_flag_str_t **interrupts_names);
typedef soc_error_t (*mbcm_dfe_diag_cell_pipe_counter_get_f) (int unit, int pipe, uint64 *counter);
typedef soc_error_t (*mbcm_dfe_diag_mesh_topology_get_f) (int unit, soc_dcmn_fabric_mesh_topology_diag_t *mesh_topology_diag);
typedef int (*mbcm_dfe_drv_graceful_shutdown_set_f) (int unit, soc_pbmp_t active_links, int shutdown, soc_pbmp_t unisolated_links, int isolate_device);
typedef int (*mbcm_dfe_drv_fe13_graceful_shutdown_set_f) (int unit, soc_pbmp_t active_links, soc_pbmp_t unisolated_links, int shutdown);
typedef int (*mbcm_dfe_drv_fe13_isolate_set_f) (int unit, soc_pbmp_t unisolated_links, int isolate);
typedef int (*mbcm_dfe_drv_soc_properties_validate_f) (int unit);
typedef int (*mbcm_dfe_drv_mbist_f) (int unit, int skip_errors);
typedef int (*mbcm_dfe_drv_link_to_block_mapping_f) (int unit, int link, int* block_id,int* inner_link, int block_type);
typedef int (*mbcm_dfe_drv_block_pbmp_get_f) (int unit, int block_type, int blk_instance, soc_pbmp_t *pbmp);
typedef soc_error_t (*mbcm_dfe_fabric_cell_get_f) (int unit, soc_dcmn_fabric_cell_entry_t entry);
typedef soc_error_t (*mbcm_dfe_fabric_cell_type_get_f) (int unit, soc_dcmn_fabric_cell_entry_t entry, soc_dcmn_fabric_cell_type_t *cell_type);
typedef soc_error_t (*mbcm_dfe_fabric_cell_parse_table_get_f) (int unit, soc_dcmn_fabric_cell_type_t cell_type, uint32 max_nof_lines, soc_dcmn_fabric_cell_parse_table_t *parse_table, uint32 *nof_lines, int is_two_parts);
typedef soc_error_t (*mbcm_dfe_fabric_cell_is_cell_in_two_parts_f) (int unit, soc_dcmn_fabric_cell_entry_t entry, int* is_two_parts);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_rci_gci_control_source_set_f) (int unit, bcm_fabric_control_t type, soc_dcmn_fabric_pipe_t val);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_rci_gci_control_source_get_f) (int unit, bcm_fabric_control_t type, soc_dcmn_fabric_pipe_t* val);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_thresholds_flags_validate_f) (int unit,uint32 flags);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_rx_llfc_threshold_validate_f) (int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_rx_llfc_threshold_set_f) (int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int value);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_rx_llfc_threshold_get_f) (int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int* value);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_rx_gci_threshold_validate_f) (int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value,int is_fe1,int is_fe3);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_rx_gci_threshold_set_f) (int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int value);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_rx_gci_threshold_get_f) (int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_control_source_t pipe, int fe1, int fe3, int* value);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_rx_rci_threshold_validate_f) (int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value,int is_fe1,int is_fe3);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_rx_rci_threshold_set_f) (int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int value);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_rx_rci_threshold_get_f) (int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_control_source_t pipe, int fe1, int fe3, int* value);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_rx_drop_threshold_validate_f) (int unit, bcm_fabric_link_threshold_type_t type, soc_dcmn_fabric_pipe_t pipe, int value);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_rx_drop_threshold_set_f) (int unit, bcm_fabric_link_threshold_type_t type, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int value);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_rx_drop_threshold_get_f) (int unit, bcm_fabric_link_threshold_type_t type, soc_dcmn_fabric_control_source_t pipe, int fe1, int fe3, int* value);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_rx_full_threshold_validate_f) (int unit,soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_rx_full_threshold_set_f) (int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int value);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_rx_full_threshold_get_f) (int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe, int *value);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_rx_fifo_size_threshold_validate_f) (int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_rx_fifo_size_threshold_set_f) (int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int value);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_rx_fifo_size_threshold_get_f) (int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int *value);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_rx_multicast_low_prio_drop_threshold_validate_f) (int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_rx_multicast_low_prio_drop_threshold_set_f) (int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int value);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_rx_multicast_low_prio_drop_threshold_get_f) (int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe, int *value);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_tx_rci_threshold_validate_f) (int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_tx_rci_threshold_set_f) (int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int value);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_tx_rci_threshold_get_f) (int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int* value);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_tx_bypass_llfc_threshold_validate_f) (int unit,soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_tx_bypass_llfc_threshold_set_f) (int unit,soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int value);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_tx_bypass_llfc_threshold_get_f) (int unit,soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int* value);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_tx_gci_threshold_validate_f) (int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_tx_gci_threshold_set_f) (int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int value);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_tx_gci_threshold_get_f) (int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_control_source_t pipe, int fe1, int fe3, int* value);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_tx_drop_threshold_validate_f) (int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_tx_drop_threshold_set_f) (int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int value);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_tx_drop_threshold_get_f) (int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int* value);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_tx_rci_threshold_fc_validate_f) (int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_tx_rci_threshold_fc_set_f) (int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int value);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_tx_rci_threshold_fc_get_f) (int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int fe1, int fe3, int* value);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_tx_almost_full_threshold_validate_f) (int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_tx_almost_full_threshold_set_f) (int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int is_fe3, int value, int to_check_counter_overflow);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_tx_almost_full_threshold_get_f) (int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int is_fe3, int* value);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_tx_fifo_size_threshold_validate_f) (int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_tx_fifo_size_threshold_set_f) (int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int is_fe3, int value, int to_check_counter_overflow);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_tx_fifo_size_threshold_get_f) (int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int is_fe3, int* value);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_mid_gci_threshold_validate_f) (int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_mid_gci_threshold_set_f) (int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int is_fe3, int value);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_mid_gci_threshold_get_f) (int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int* value);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_mid_rci_threshold_validate_f) (int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_mid_rci_threshold_set_f) (int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int is_fe3, int value);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_mid_rci_threshold_get_f) (int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int* value);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_mid_prio_drop_threshold_validate_f) (int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_mid_prio_drop_threshold_set_f) (int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int is_fe3, int value);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_mid_prio_drop_threshold_get_f) (int unit, bcm_fabric_link_threshold_type_t type, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int* value);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_mid_almost_full_threshold_validate_f) (int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_mid_almost_full_threshold_set_f) (int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int almost_full_index, int is_fe1, int is_fe3, int value);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_mid_almost_full_threshold_get_f) (int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int almost_full_index, int is_fe1, int* value);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_mid_fifo_size_threshold_validate_f) (int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_mid_fifo_size_threshold_set_f) (int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int is_fe3, int value, int to_check_counter_overflow);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_mid_fifo_size_threshold_get_f) (int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int* value);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_mid_full_threshold_validate_f) (int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int value);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_mid_full_threshold_set_f) (int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int is_fe3, int value);
typedef soc_error_t (*mbcm_dfe_fabric_flow_control_mid_full_threshold_get_f) (int unit, soc_dfe_fabric_link_fifo_type_index_t type_index, soc_dcmn_fabric_pipe_t pipe, int is_fe1, int *value);
typedef soc_error_t (*mbcm_dfe_fabric_links_link_type_set_f) (int unit, soc_dcmn_fabric_pipe_t pipe, soc_dfe_fabric_link_fifo_type_index_t type_index, int is_rx, int is_tx, int is_fe1, int is_fe3, uint32 links_count, soc_port_t* links);
typedef soc_error_t (*mbcm_dfe_fabric_links_link_type_get_f) (int unit, soc_dcmn_fabric_pipe_t pipe, soc_dfe_fabric_link_fifo_type_index_t type_index, int is_rx, int is_tx, int is_fe1, int is_fe3, uint32 links_count_max, soc_port_t* links, uint32* links_count);
typedef soc_error_t (*mbcm_dfe_fabric_links_nof_links_get_f) (int unit, int* nof_links);
typedef soc_error_t (*mbcm_dfe_fabric_links_validate_link_f) (int unit, soc_port_t link);
typedef soc_error_t (*mbcm_dfe_fabric_links_isolate_set_f) (int unit, soc_port_t link, soc_dcmn_isolation_status_t val);
typedef soc_error_t (*mbcm_dfe_fabric_links_bmp_isolate_set_f) (int unit, soc_pbmp_t bitmap, soc_dcmn_isolation_status_t val);
typedef soc_error_t (*mbcm_dfe_fabric_links_isolate_get_f) (int unit, soc_port_t link, soc_dcmn_isolation_status_t* val);
typedef soc_error_t (*mbcm_dfe_fabric_links_cell_format_verify_f) (int unit, soc_port_t link, soc_dfe_fabric_link_cell_size_t val);
typedef soc_error_t (*mbcm_dfe_fabric_links_cell_format_set_f) (int unit, soc_port_t link, soc_dfe_fabric_link_cell_size_t val);
typedef soc_error_t (*mbcm_dfe_fabric_links_cell_format_get_f) (int unit, soc_port_t link, soc_dfe_fabric_link_cell_size_t* val);
typedef soc_error_t (*mbcm_dfe_fabric_links_flow_status_control_cell_format_set_f) (int unit, soc_port_t link, soc_dfe_fabric_link_cell_size_t val);
typedef soc_error_t (*mbcm_dfe_fabric_links_flow_status_control_cell_format_get_f) (int unit, soc_port_t link, soc_dfe_fabric_link_cell_size_t* val);
typedef soc_error_t (*mbcm_dfe_fabric_links_cell_interleaving_set_f) (int unit, soc_port_t link, int val);
typedef soc_error_t (*mbcm_dfe_fabric_links_cell_interleaving_get_f) (int unit, soc_port_t link, int* val);
typedef soc_error_t (*mbcm_dfe_fabric_links_weight_validate_f) (int unit, int val);
typedef soc_error_t (*mbcm_dfe_fabric_links_weight_set_f) (int unit, soc_port_t link, int is_prim, int val);
typedef soc_error_t (*mbcm_dfe_fabric_links_weight_get_f) (int unit, soc_port_t link, int is_prim, int* val);
typedef soc_error_t (*mbcm_dfe_fabric_links_secondary_only_set_f) (int unit, soc_port_t link, int val);
typedef soc_error_t (*mbcm_dfe_fabric_links_secondary_only_get_f) (int unit, soc_port_t link, int* val);
typedef soc_error_t (*mbcm_dfe_fabric_links_llf_control_source_set_f) (int unit, soc_port_t link, soc_dcmn_fabric_pipe_t val);
typedef soc_error_t (*mbcm_dfe_fabric_links_llf_control_source_get_f) (int unit, soc_port_t link, soc_dcmn_fabric_pipe_t* val); 
typedef soc_error_t (*mbcm_dfe_fabric_links_aldwp_config_f) (int unit, soc_port_t port);
typedef soc_error_t (*mbcm_dfe_fabric_links_aldwp_init_f) (int unit);
typedef soc_error_t (*mbcm_dfe_fabric_links_pcp_enable_set_f) (int unit, soc_port_t link, int enable);
typedef soc_error_t (*mbcm_dfe_fabric_links_pcp_enable_get_f) (int unit, soc_port_t link, int *enable);
typedef soc_error_t (*mbcm_dfe_fabric_multicast_low_prio_drop_select_priority_set_f) (int unit, soc_dfe_fabric_priority_t prio);
typedef soc_error_t (*mbcm_dfe_fabric_multicast_low_prio_drop_select_priority_get_f) (int unit, soc_dfe_fabric_priority_t* prio);
typedef soc_error_t (*mbcm_dfe_fabric_multicast_low_prio_threshold_validate_f) (int unit, bcm_fabric_control_t type, int arg);
typedef soc_error_t (*mbcm_dfe_fabric_multicast_low_prio_threshold_set_f) (int unit, bcm_fabric_control_t type, int arg);
typedef soc_error_t (*mbcm_dfe_fabric_multicast_low_prio_threshold_get_f) (int unit, bcm_fabric_control_t type, int* arg);
typedef soc_error_t (*mbcm_dfe_fabric_multicast_low_priority_drop_enable_set_f) (int unit, int arg);
typedef soc_error_t (*mbcm_dfe_fabric_multicast_low_priority_drop_enable_get_f) (int unit, int* arg);
typedef soc_error_t (*mbcm_dfe_fabric_multicast_priority_range_validate_f) (int unit, bcm_fabric_control_t type, int arg);
typedef soc_error_t (*mbcm_dfe_fabric_multicast_priority_range_set_f) (int unit, bcm_fabric_control_t type, int arg);
typedef soc_error_t (*mbcm_dfe_fabric_multicast_priority_range_get_f) (int unit, bcm_fabric_control_t type, int* arg);
typedef soc_error_t (*mbcm_dfe_fabric_multicast_multi_set_f) (int unit, uint32 flags, uint32 ngroups, bcm_multicast_t *groups, bcm_fabric_module_vector_t *dest_array);
typedef soc_error_t (*mbcm_dfe_fabric_multicast_multi_get_f) (int unit, uint32 flags, uint32 ngroups, bcm_multicast_t *groups, bcm_fabric_module_vector_t *dest_array);
typedef soc_error_t (*mbcm_dfe_fabric_multicast_multi_write_range_f) (int unit, int mem_flags, soc_multicast_t group_min, soc_multicast_t group_max, uint32 *entry_array);
typedef soc_error_t (*mbcm_dfe_fabric_multicast_multi_read_info_get_f) (int unit, soc_dfe_multicast_read_range_info_t **info, int *info_size);
typedef soc_error_t (*mbcm_dfe_fabric_link_repeater_enable_set_f) (int unit, soc_port_t port, int enable, int empty_cell_size);
typedef soc_error_t (*mbcm_dfe_fabric_link_repeater_enable_get_f) (int unit, soc_port_t port, int *enable, int *empty_cell_size);
typedef soc_error_t (*mbcm_dfe_fabric_link_status_all_get_f) (int unit, int links_array_max_size, uint32* link_status, uint32* errored_token_count, int* links_array_count);
typedef soc_error_t (*mbcm_dfe_fabric_link_status_get_f) (int unit, soc_port_t link_id, uint32 *link_status, uint32 *errored_token_count);
typedef soc_error_t (*mbcm_dfe_fabric_reachability_status_get_f) (int unit, int moduleid, int links_max, uint32 *links_array, int *links_count);
typedef soc_error_t (*mbcm_dfe_fabric_link_connectivity_status_get_f) (int unit, soc_port_t link_id, bcm_fabric_link_connectivity_t *link_partner);
typedef soc_error_t (*mbcm_dfe_fabric_links_pipe_map_set_f) (int unit, soc_port_t port, soc_dfe_fabric_link_remote_pipe_mapping_t pipe_map);
typedef soc_error_t (*mbcm_dfe_fabric_links_pipe_map_get_f) (int unit, soc_port_t port, soc_dfe_fabric_link_remote_pipe_mapping_t *pipe_map);
typedef soc_error_t (*mbcm_dfe_fabric_links_repeater_nof_remote_pipe_set_f) (int unit, soc_port_t port, uint32 nof_remote_pipes);
typedef soc_error_t (*mbcm_dfe_fabric_links_repeater_nof_remote_pipe_get_f) (int unit, soc_port_t port, uint32 *nof_remote_pipes);
typedef soc_error_t (*mbcm_dfe_fabric_topology_isolate_set_f) (int unit, soc_dcmn_isolation_status_t val);
typedef soc_error_t (*mbcm_dfe_fabric_topology_isolate_get_f) (int unit, soc_dcmn_isolation_status_t* val);
typedef soc_error_t (*mbcm_dfe_fabric_topology_rmgr_set_f) (int unit, int enable);
typedef soc_error_t (*mbcm_dfe_fabric_link_topology_set_f) (int unit, bcm_module_t destination_local_id, int links_count, soc_port_t *links_array);
typedef soc_error_t (*mbcm_dfe_fabric_link_topology_get_f) (int unit, bcm_module_t destination_local_id, int links_count_max, int *links_count, soc_port_t *links_array);
typedef soc_error_t (*mbcm_dfe_fabric_topology_min_nof_links_set_f) (int unit, int min_nof_links);
typedef soc_error_t (*mbcm_dfe_fabric_topology_min_nof_links_get_f) (int unit, int *min_nof_links);
typedef soc_error_t (*mbcm_dfe_fabric_topology_nof_links_to_min_nof_links_default_f) (int unit, int nof_links, int *min_nof_links);
typedef soc_error_t (*mbcm_dfe_fabric_topology_repeater_destination_set_f) (int unit, soc_port_t source, soc_port_t destination);
typedef soc_error_t (*mbcm_dfe_fabric_topology_repeater_destination_get_f) (int unit, soc_port_t source, soc_port_t* destination);
typedef soc_error_t (*mbcm_dfe_fabric_topology_reachability_mask_set_f) (int unit, soc_pbmp_t active_links, soc_dcmn_isolation_status_t val);
typedef soc_error_t (*mbcm_dfe_fifo_dma_channel_init_f) (int unit, int channel, soc_dfe_fifo_dma_t *fifo_dma_info);
typedef soc_error_t (*mbcm_dfe_fifo_dma_channel_deinit_f) (int unit, int channel, soc_dfe_fifo_dma_t *fifo_dma_info);
typedef soc_error_t (*mbcm_dfe_fifo_dma_channel_clear_f) (int unit, int channel, soc_dfe_fifo_dma_t *fifo_dma_info);
typedef soc_error_t (*mbcm_dfe_fifo_dma_channel_read_entries_f) (int unit, int channel, soc_dfe_fifo_dma_t *fifo_dma_info);
typedef soc_error_t (*mbcm_dfe_fifo_dma_fabric_cell_validate_f) (int unit);
typedef int (*mbcm_dfe_nof_interrupts_f) (int unit, int* nof_interrupts);
typedef int (*mbcm_dfe_nof_block_instances_f) (int unit, soc_block_types_t block_types, int *nof_block_instances);
typedef soc_error_t (*mbcm_dfe_multicast_egress_add_f) (int unit, bcm_multicast_t group, soc_gport_t port);
typedef soc_error_t (*mbcm_dfe_multicast_egress_delete_f) (int unit, bcm_multicast_t group, soc_gport_t port);
typedef soc_error_t (*mbcm_dfe_multicast_egress_delete_all_f) (int unit, bcm_multicast_t group);
typedef soc_error_t (*mbcm_dfe_multicast_egress_get_f) (int unit, bcm_multicast_t group, int port_max, soc_gport_t *port_array, int *port_count);
typedef soc_error_t (*mbcm_dfe_multicast_egress_set_f) (int unit, bcm_multicast_t group, int port_count, soc_gport_t *port_array);
typedef soc_error_t (*mbcm_dfe_multicast_mode_get_f) (int unit, soc_dfe_multicast_table_mode_t* multicast_mode); 
typedef soc_error_t (*mbcm_dfe_multicast_table_size_get_f) (int unit, uint32* mc_table_size);
typedef soc_error_t (*mbcm_dfe_multicast_table_entry_size_get_f) (int unit, uint32* entry_size);
typedef soc_error_t (*mbcm_dfe_port_init_f) (int unit);
typedef soc_error_t (*mbcm_dfe_port_deinit_f) (int unit);
typedef soc_error_t (*mbcm_dfe_port_detach_f) (int unit, soc_port_t port);
typedef soc_error_t (*mbcm_dfe_port_soc_init_f) (int unit);
typedef soc_error_t (*mbcm_dfe_port_probe_f) (int unit, pbmp_t pbmp, pbmp_t *okay_pbmp, int is_init_sequence);
typedef soc_error_t (*mbcm_dfe_port_control_pcs_set_f) (int unit, soc_port_t port, soc_dcmn_port_pcs_t pcs);
typedef soc_error_t (*mbcm_dfe_port_control_pcs_get_f) (int unit, soc_port_t port, soc_dcmn_port_pcs_t* pcs);
typedef soc_error_t (*mbcm_dfe_port_control_power_set_f) (int unit, soc_port_t port, uint32 flags, soc_dcmn_port_power_t power);
typedef soc_error_t (*mbcm_dfe_port_control_power_get_f) (int unit, soc_port_t port, soc_dcmn_port_power_t* power);
typedef soc_error_t (*mbcm_dfe_port_control_strip_crc_set_f) (int unit, soc_port_t port, int strip_crc);
typedef soc_error_t (*mbcm_dfe_port_control_strip_crc_get_f) (int unit, soc_port_t port, int* strip_crc);
typedef soc_error_t (*mbcm_dfe_port_control_rx_enable_set_f) (int unit, soc_port_t port, uint32 flags, int enable);
typedef soc_error_t (*mbcm_dfe_port_control_tx_enable_set_f) (int unit, soc_port_t port, int enable);
typedef soc_error_t (*mbcm_dfe_port_control_rx_enable_get_f) (int unit, soc_port_t port, int* enable);
typedef soc_error_t (*mbcm_dfe_port_control_tx_enable_get_f) (int unit, soc_port_t port, int* enable);
typedef soc_error_t (*mbcm_dfe_port_control_low_latency_set_f) (int unit,soc_port_t port,int value);
typedef soc_error_t (*mbcm_dfe_port_control_low_latency_get_f) (int unit,soc_port_t port,int* value);
typedef soc_error_t (*mbcm_dfe_port_control_fec_error_detect_set_f) (int unit,soc_port_t port,int value);
typedef soc_error_t (*mbcm_dfe_port_control_fec_error_detect_get_f) (int unit,soc_port_t port,int* value);
typedef soc_error_t (*mbcm_dfe_port_phy_enable_set_f) (int unit, soc_port_t port, int enable);
typedef soc_error_t (*mbcm_dfe_port_phy_enable_get_f) (int unit, soc_port_t port, int *enable);
typedef soc_error_t (*mbcm_dfe_port_phy_cl72_set_f) (int unit, soc_port_t port, int enable);
typedef soc_error_t (*mbcm_dfe_port_phy_cl72_get_f) (int unit, soc_port_t port, int *enable);
typedef soc_error_t (*mbcm_dfe_port_phy_control_set_f) (int unit, soc_port_t port, int phyn, int lane, int is_sys_side, soc_phy_control_t type, uint32 value);
typedef soc_error_t (*mbcm_dfe_port_phy_control_get_f) (int unit, soc_port_t port, int phyn, int lane, int is_sys_side, soc_phy_control_t type, uint32 *value);
typedef soc_error_t (*mbcm_dfe_port_loopback_set_f) (int unit, soc_port_t port, soc_dcmn_loopback_mode_t loopback);
typedef soc_error_t (*mbcm_dfe_port_loopback_get_f) (int unit, soc_port_t port, soc_dcmn_loopback_mode_t* loopback);
typedef soc_error_t (*mbcm_dfe_port_fault_get_f) (int unit, soc_port_t port, uint32* flags);
typedef soc_error_t (*mbcm_dfe_port_speed_get_f) (int unit, soc_port_t port, int *speed);
typedef soc_error_t (*mbcm_dfe_port_speed_max_f) (int unit, soc_port_t port, int *speed);
typedef soc_error_t (*mbcm_dfe_port_speed_set_f) (int unit, soc_port_t port, int speed);
typedef soc_error_t (*mbcm_dfe_port_interface_set_f) (int unit, soc_port_t port, soc_port_if_t intf);
typedef soc_error_t (*mbcm_dfe_port_interface_get_f) (int unit, soc_port_t port, soc_port_if_t *intf);
typedef soc_error_t (*mbcm_dfe_port_serdes_power_disable_f) (int unit, soc_pbmp_t disable_pbmp);
typedef soc_error_t (*mbcm_dfe_port_link_status_get_f) (int unit, soc_port_t port, int *up);
typedef soc_error_t (*mbcm_dfe_port_bucket_fill_rate_validate_f) (int unit, uint32 bucket_fill_rate);
typedef soc_error_t (*mbcm_dfe_port_prbs_tx_enable_set_f) (int unit, soc_port_t port, soc_dcmn_port_prbs_mode_t mode, int value);
typedef soc_error_t (*mbcm_dfe_port_prbs_tx_enable_get_f) (int unit, soc_port_t port, soc_dcmn_port_prbs_mode_t mode, int* value);
typedef soc_error_t (*mbcm_dfe_port_prbs_rx_enable_set_f) (int unit, soc_port_t port, soc_dcmn_port_prbs_mode_t mode, int value);
typedef soc_error_t (*mbcm_dfe_port_prbs_rx_enable_get_f) (int unit, soc_port_t port, soc_dcmn_port_prbs_mode_t mode, int* value);
typedef soc_error_t (*mbcm_dfe_port_prbs_rx_status_get_f) (int unit, soc_port_t port, soc_dcmn_port_prbs_mode_t mode, int* value);
typedef soc_error_t (*mbcm_dfe_port_prbs_polynomial_set_f) (int unit, soc_port_t port, soc_dcmn_port_prbs_mode_t mode, int value);
typedef soc_error_t (*mbcm_dfe_port_prbs_polynomial_get_f) (int unit, soc_port_t port, soc_dcmn_port_prbs_mode_t mode, int* value);
typedef soc_error_t (*mbcm_dfe_port_prbs_tx_invert_data_set_f) (int unit, soc_port_t port, soc_dcmn_port_prbs_mode_t mode, int value);
typedef soc_error_t (*mbcm_dfe_port_prbs_tx_invert_data_get_f) (int unit, soc_port_t port, soc_dcmn_port_prbs_mode_t mode, int *value);
typedef soc_error_t (*mbcm_dfe_port_pump_enable_set_f) (int unit, soc_port_t port, int enable);
typedef soc_error_t (*mbcm_dfe_port_rate_egress_ppt_set_f) (int unit, soc_port_t port, uint32 burst, uint32 nof_tiks);
typedef soc_error_t (*mbcm_dfe_port_rate_egress_ppt_get_f) (int unit, soc_port_t port, uint32 *burst, uint32 *nof_tiks);
typedef soc_error_t (*mbcm_dfe_port_burst_control_set_f)(int unit, soc_port_t port, soc_dcmn_loopback_mode_t loopback);
typedef soc_error_t (*mbcm_dfe_port_extract_cig_from_llfc_enable_set_f) (int unit, soc_port_t port, int value);
typedef soc_error_t (*mbcm_dfe_port_extract_cig_from_llfc_enable_get_f) (int unit, soc_port_t port, int *value);
typedef soc_error_t (*mbcm_dfe_port_phy_get_f) (int unit, soc_port_t port, uint32 flags, uint32 phy_reg_addr, uint32 *phy_data);
typedef soc_error_t (*mbcm_dfe_port_phy_set_f) (int unit, soc_port_t port, uint32 flags, uint32 phy_reg_addr, uint32 phy_data);
typedef soc_error_t (*mbcm_dfe_port_phy_modify_f) (int unit, soc_port_t port, uint32 flags, uint32 phy_reg_addr, uint32 phy_data, uint32 phy_mask);
typedef soc_error_t (*mbcm_dfe_port_dynamic_port_update_f) (int unit, int port, int enable, soc_dfe_port_update_type_t type);
typedef soc_error_t (*mbcm_dfe_port_enable_set_f) (int unit, int port, int enable);
typedef soc_error_t (*mbcm_dfe_port_enable_get_f) (int unit, int port, int *enable);
typedef soc_error_t (*mbcm_dfe_port_sync_e_link_set_f) (int unit, int is_master, int arg);
typedef soc_error_t (*mbcm_dfe_port_sync_e_divider_set_f) (int unit, int divider);
typedef soc_error_t (*mbcm_dfe_port_sync_e_link_get_f) (int unit, int is_master, int *arg);
typedef soc_error_t (*mbcm_dfe_port_sync_e_divider_get_f) (int unit, int *arg);
typedef soc_error_t (*mbcm_dfe_set_mesh_topology_config_f) (int unit);
typedef soc_error_t (*mbcm_dfe_stk_modid_set_f) (int unit, uint32 fe_id);
typedef soc_error_t (*mbcm_dfe_stk_modid_get_f) (int unit, uint32* fe_id);
typedef soc_error_t (*mbcm_dfe_stk_module_max_all_reachable_verify_f) (int unit, bcm_module_t max_module);
typedef soc_error_t (*mbcm_dfe_stk_module_max_all_reachable_set_f) (int unit, bcm_module_t max_module);
typedef soc_error_t (*mbcm_dfe_stk_module_max_all_reachable_get_f) (int unit, bcm_module_t *max_module);
typedef soc_error_t (*mbcm_dfe_stk_module_max_fap_verify_f) (int unit, bcm_module_t max_module);
typedef soc_error_t (*mbcm_dfe_stk_module_max_fap_set_f) (int unit, bcm_module_t max_module);
typedef soc_error_t (*mbcm_dfe_stk_module_max_fap_get_f) (int unit, bcm_module_t *max_module);
typedef soc_error_t (*mbcm_dfe_stk_module_all_reachable_ignore_id_set_f) (int unit, bcm_module_t module, int arg);
typedef soc_error_t (*mbcm_dfe_stk_module_all_reachable_ignore_id_get_f) (int unit, bcm_module_t module, int *arg);
typedef soc_error_t (*mbcm_dfe_stk_valid_module_id_verify_f) (int unit, bcm_module_t module, int verify_divisble_by_32);
typedef soc_error_t (*mbcm_dfe_stat_init_f) (int unit);
typedef soc_error_t (*mbcm_dfe_stat_is_supported_type_f) (int unit, soc_port_t port, bcm_stat_val_t type);
typedef soc_error_t (*mbcm_dfe_stat_counter_length_get_f) (int unit, int counter_id, int *length);
typedef soc_error_t (*mbcm_dfe_stat_get_f) (int unit,bcm_port_t port,uint64 *value,int *counters,int arr_size);
typedef soc_error_t (*mbcm_dfe_mapping_stat_get_f) (int unit,soc_port_t port, int *counters,int *array_size ,bcm_stat_val_t type,int max_array_size);
typedef soc_error_t (*mbcm_dfe_counters_get_info_f) (int unit,soc_dfe_counters_info_t* fe_counters_info);
typedef soc_error_t (*mbcm_dfe_queues_get_info_f) (int unit,soc_dfe_queues_info_t* fe_queues_info);
typedef soc_error_t (*mbcm_dfe_fabric_link_device_mode_get_f) (int unit,soc_port_t port, int is_rx, soc_dfe_fabric_link_device_mode_t *device_mode);
typedef soc_error_t (*mbcm_dfe_controlled_counter_set_f) (int unit);
typedef void (*mbcm_dfe_get_soc_properties_array_f) (int unit, soc_dfe_property_info_t** soc_dfe_property_info_array);
typedef int (*mbcm_dfe_avs_value_get_f) (int unit, uint32 *avs_val);
typedef int (*mbcm_dfe_linkctrl_init_f) (int unit);
typedef int (*mbcm_dfe_interrupts_init_f) (int unit);
typedef int (*mbcm_dfe_interrupts_deinit_f) (int unit);
typedef int (*mbcm_dfe_interrupts_control_data_init_f) (int unit);
typedef void (*mbcm_dfe_interrupts_control_data_deinit_f) (int unit);
typedef soc_error_t (*mbcm_dfe_interrupt_all_enable_set_f) (int unit, int enable);
typedef soc_error_t (*mbcm_dfe_interrupt_all_enable_get_f) (int unit, int *enable);
typedef int (*mbcm_dfe_drv_sw_ver_set_f) (int unit);
typedef int (*mbcm_dfe_drv_temperature_monitor_get_f) (int unit, int temperature_max, soc_switch_temperature_monitor_t *temperature_array, int *temperature_count);
typedef int (*mbcm_dfe_drv_test_reg_filter_f) (int unit, soc_reg_t reg, int *is_filtered);
typedef int (*mbcm_dfe_drv_test_reg_default_val_filter_f) (int unit, soc_reg_t reg, int *is_filtered);
typedef int (*mbcm_dfe_drv_test_mem_filter_f) (int unit, soc_mem_t mem, int *is_filtered);
typedef int (*mbcm_dfe_drv_test_brdc_blk_filter_f) (int unit, soc_reg_t reg, int *is_filtered);
typedef int (*mbcm_dfe_drv_test_brdc_blk_info_get_f) (int unit, int max_size, soc_reg_brdc_block_info_t *brdc_info, int *actual_size);
typedef int (*mbcm_dfe_drv_asymmetrical_quad_get_f) (int unit, int link, int *asymmetrical_quad);
typedef soc_error_t (*mbcm_dfe_warm_boot_buffer_id_supported_get_f) (int unit, int buffer_id, int *is_supported);
typedef soc_error_t (*mbcm_dfe_cosq_pipe_rx_weight_set_f) (int unit, int pipe, int port, soc_dfe_cosq_weight_mode_t mode, int weight);
typedef soc_error_t (*mbcm_dfe_cosq_pipe_rx_weight_get_f) (int unit, int pipe, int port, soc_dfe_cosq_weight_mode_t mode, int *weight);
typedef soc_error_t (*mbcm_dfe_cosq_pipe_mid_weight_set_f) (int unit, int pipe, int port, soc_dfe_cosq_weight_mode_t mode, int weight);
typedef soc_error_t (*mbcm_dfe_cosq_pipe_mid_weight_get_f) (int unit, int pipe, int port, soc_dfe_cosq_weight_mode_t mode, int *weight);
typedef soc_error_t (*mbcm_dfe_cosq_pipe_tx_weight_set_f) (int unit, int pipe, int port, soc_dfe_cosq_weight_mode_t mode, int weight);
typedef soc_error_t (*mbcm_dfe_cosq_pipe_tx_weight_get_f) (int unit, int pipe, int port, soc_dfe_cosq_weight_mode_t mode, int *weight);
typedef soc_error_t (*mbcm_dfe_cosq_pipe_rx_threshold_set_f) (int unit, int pipe, int port, int threshold);
typedef soc_error_t (*mbcm_dfe_cosq_pipe_rx_threshold_get_f) (int unit, int pipe, int port, int *threshold);
typedef soc_error_t (*mbcm_dfe_cosq_pipe_mid_threshold_set_f) (int unit, int pipe, int port, int threshold);
typedef soc_error_t (*mbcm_dfe_cosq_pipe_mid_threshold_get_f) (int unit, int pipe, int port, int *threshold);
typedef soc_error_t (*mbcm_dfe_cosq_pipe_tx_threshold_set_f) (int unit, int pipe, int port, int threshold);
typedef soc_error_t (*mbcm_dfe_cosq_pipe_tx_threshold_get_f) (int unit, int pipe, int port, int *threshold);
typedef soc_error_t (*mbcm_dfe_cosq_pipe_rx_rate_set_f) (int unit, int pipe, int port, soc_dfe_cosq_shaper_mode_t shaper_mode, uint32 rate);
typedef soc_error_t (*mbcm_dfe_cosq_pipe_rx_rate_get_f) (int unit, int pipe, int port, soc_dfe_cosq_shaper_mode_t *shaper_mode, uint32 *rate);
typedef soc_error_t (*mbcm_dfe_cosq_pipe_tx_rate_set_f) (int unit, int port, soc_dfe_cosq_shaper_mode_t shaper_mode, uint32 rate);
typedef soc_error_t (*mbcm_dfe_cosq_pipe_tx_rate_get_f) (int unit, int port, soc_dfe_cosq_shaper_mode_t *shaper_mode, uint32 *rate);
typedef soc_error_t (*mbcm_dfe_cosq_pipe_tx_rate_enable_set_f) (int unit, int pipe, int port, int enable);
typedef soc_error_t (*mbcm_dfe_cosq_pipe_tx_rate_enable_get_f) (int unit, int pipe, int port, int *enable);
typedef soc_error_t (*mbcm_dfe_rx_cpu_address_modid_set_f) (int unit, int num_of_cpu_addresses, int *cpu_addresses);
typedef soc_error_t (*mbcm_dfe_rx_cpu_address_modid_init_f) (int unit);
typedef soc_error_t (*mbcm_dfe_port_quad_disabled_f) (int unit, int quad, int *disabled);
typedef soc_error_t (*mbcm_dfe_drv_block_valid_get_f) (int unit, int blktype, int id, char *valid);


typedef struct mbcm_dfe_functions_s {
     
    mbcm_dfe_tbl_is_dynamic_f                                           mbcm_dfe_tbl_is_dynamic;
    mbcm_dfe_ser_init_f                                                 mbcm_dfe_ser_init;
    mbcm_dfe_reset_device_f                                             mbcm_dfe_reset_device;
    mbcm_dfe_drv_soft_init_f                                            mbcm_dfe_drv_soft_init;
    mbcm_dfe_drv_blocks_reset_f                                         mbcm_dfe_drv_blocks_reset;
    mbcm_dfe_drv_reg_access_only_reset_f                                mbcm_dfe_drv_reg_access_only_reset;
    mbcm_dfe_TDM_fragment_validate_f                                    mbcm_dfe_TDM_fragment_validate;
    mbcm_dfe_sr_cell_send_f                                             mbcm_dfe_sr_cell_send;
    mbcm_dfe_cell_filter_set_f                                          mbcm_dfe_cell_filter_set;
    mbcm_dfe_cell_filter_clear_f                                        mbcm_dfe_cell_filter_clear;
    mbcm_dfe_cell_filter_count_get_f                                    mbcm_dfe_cell_filter_count_get;
    mbcm_dfe_control_cell_filter_set_f                                  mbcm_dfe_control_cell_filter_set;
    mbcm_dfe_control_cell_filter_clear_f                                mbcm_dfe_control_cell_filter_clear;
    mbcm_dfe_control_cell_filter_receive_f                              mbcm_dfe_control_cell_filter_receive;
    mbcm_dfe_cell_snake_test_prepare_f                                  mbcm_dfe_cell_snake_test_prepare;
    mbcm_dfe_cell_snake_test_run_f                                      mbcm_dfe_cell_snake_test_run;
    mbcm_dfe_diag_fabric_cell_snake_test_interrupts_name_get_f          mbcm_dfe_diag_fabric_cell_snake_test_interrupts_name_get;
    mbcm_dfe_diag_cell_pipe_counter_get_f                               mbcm_dfe_diag_cell_pipe_counter_get;
    mbcm_dfe_diag_mesh_topology_get_f                                   mbcm_dfe_diag_mesh_topology_get;
    mbcm_dfe_drv_graceful_shutdown_set_f                                mbcm_dfe_drv_graceful_shutdown_set;
    mbcm_dfe_drv_fe13_graceful_shutdown_set_f                           mbcm_dfe_drv_fe13_graceful_shutdown_set;
    mbcm_dfe_drv_fe13_isolate_set_f                                     mbcm_dfe_drv_fe13_isolate_set;
    mbcm_dfe_drv_soc_properties_validate_f                              mbcm_dfe_drv_soc_properties_validate;
    mbcm_dfe_drv_mbist_f                                                mbcm_dfe_drv_mbist;
    mbcm_dfe_drv_link_to_block_mapping_f                                mbcm_dfe_drv_link_to_block_mapping;
    mbcm_dfe_drv_block_pbmp_get_f                                       mbcm_dfe_drv_block_pbmp_get;
    mbcm_dfe_fabric_cell_get_f                                          mbcm_dfe_fabric_cell_get;
    mbcm_dfe_fabric_cell_type_get_f                                     mbcm_dfe_fabric_cell_type_get;
    mbcm_dfe_fabric_cell_parse_table_get_f                              mbcm_dfe_fabric_cell_parse_table_get;
    mbcm_dfe_fabric_cell_is_cell_in_two_parts_f                         mbcm_dfe_fabric_cell_is_cell_in_two_parts;
    mbcm_dfe_fabric_flow_control_rci_gci_control_source_set_f           mbcm_dfe_fabric_flow_control_rci_gci_control_source_set;
    mbcm_dfe_fabric_flow_control_rci_gci_control_source_get_f           mbcm_dfe_fabric_flow_control_rci_gci_control_source_get;
    mbcm_dfe_fabric_flow_control_thresholds_flags_validate_f            mbcm_dfe_fabric_flow_control_thresholds_flags_validate;
    mbcm_dfe_fabric_flow_control_rx_llfc_threshold_validate_f           mbcm_dfe_fabric_flow_control_rx_llfc_threshold_validate;
    mbcm_dfe_fabric_flow_control_rx_llfc_threshold_set_f                mbcm_dfe_fabric_flow_control_rx_llfc_threshold_set;
    mbcm_dfe_fabric_flow_control_rx_llfc_threshold_get_f                mbcm_dfe_fabric_flow_control_rx_llfc_threshold_get;
    mbcm_dfe_fabric_flow_control_rx_gci_threshold_validate_f            mbcm_dfe_fabric_flow_control_rx_gci_threshold_validate;
    mbcm_dfe_fabric_flow_control_rx_gci_threshold_set_f                 mbcm_dfe_fabric_flow_control_rx_gci_threshold_set;
    mbcm_dfe_fabric_flow_control_rx_gci_threshold_get_f                 mbcm_dfe_fabric_flow_control_rx_gci_threshold_get;
    mbcm_dfe_fabric_flow_control_rx_rci_threshold_validate_f            mbcm_dfe_fabric_flow_control_rx_rci_threshold_validate;
    mbcm_dfe_fabric_flow_control_rx_rci_threshold_set_f                 mbcm_dfe_fabric_flow_control_rx_rci_threshold_set;
    mbcm_dfe_fabric_flow_control_rx_rci_threshold_get_f                 mbcm_dfe_fabric_flow_control_rx_rci_threshold_get;
    mbcm_dfe_fabric_flow_control_rx_drop_threshold_validate_f           mbcm_dfe_fabric_flow_control_rx_drop_threshold_validate;
    mbcm_dfe_fabric_flow_control_rx_drop_threshold_set_f                mbcm_dfe_fabric_flow_control_rx_drop_threshold_set;
    mbcm_dfe_fabric_flow_control_rx_drop_threshold_get_f                mbcm_dfe_fabric_flow_control_rx_drop_threshold_get;
    mbcm_dfe_fabric_flow_control_rx_full_threshold_validate_f           mbcm_dfe_fabric_flow_control_rx_full_threshold_validate;
    mbcm_dfe_fabric_flow_control_rx_full_threshold_set_f                mbcm_dfe_fabric_flow_control_rx_full_threshold_set;
    mbcm_dfe_fabric_flow_control_rx_full_threshold_get_f                mbcm_dfe_fabric_flow_control_rx_full_threshold_get;
    mbcm_dfe_fabric_flow_control_rx_fifo_size_threshold_validate_f      mbcm_dfe_fabric_flow_control_rx_fifo_size_threshold_validate;
    mbcm_dfe_fabric_flow_control_rx_fifo_size_threshold_set_f           mbcm_dfe_fabric_flow_control_rx_fifo_size_threshold_set;
    mbcm_dfe_fabric_flow_control_rx_fifo_size_threshold_get_f           mbcm_dfe_fabric_flow_control_rx_fifo_size_threshold_get;
    mbcm_dfe_fabric_flow_control_rx_multicast_low_prio_drop_threshold_validate_f  mbcm_dfe_fabric_flow_control_rx_multicast_low_prio_drop_threshold_validate;
    mbcm_dfe_fabric_flow_control_rx_multicast_low_prio_drop_threshold_set_f  mbcm_dfe_fabric_flow_control_rx_multicast_low_prio_drop_threshold_set;
    mbcm_dfe_fabric_flow_control_rx_multicast_low_prio_drop_threshold_get_f  mbcm_dfe_fabric_flow_control_rx_multicast_low_prio_drop_threshold_get;
    mbcm_dfe_fabric_flow_control_tx_rci_threshold_validate_f            mbcm_dfe_fabric_flow_control_tx_rci_threshold_validate;
    mbcm_dfe_fabric_flow_control_tx_rci_threshold_set_f                 mbcm_dfe_fabric_flow_control_tx_rci_threshold_set;
    mbcm_dfe_fabric_flow_control_tx_rci_threshold_get_f                 mbcm_dfe_fabric_flow_control_tx_rci_threshold_get;
    mbcm_dfe_fabric_flow_control_tx_bypass_llfc_threshold_validate_f    mbcm_dfe_fabric_flow_control_tx_bypass_llfc_threshold_validate;
    mbcm_dfe_fabric_flow_control_tx_bypass_llfc_threshold_set_f         mbcm_dfe_fabric_flow_control_tx_bypass_llfc_threshold_set;
    mbcm_dfe_fabric_flow_control_tx_bypass_llfc_threshold_get_f         mbcm_dfe_fabric_flow_control_tx_bypass_llfc_threshold_get;
    mbcm_dfe_fabric_flow_control_tx_gci_threshold_validate_f            mbcm_dfe_fabric_flow_control_tx_gci_threshold_validate;
    mbcm_dfe_fabric_flow_control_tx_gci_threshold_set_f                 mbcm_dfe_fabric_flow_control_tx_gci_threshold_set;
    mbcm_dfe_fabric_flow_control_tx_gci_threshold_get_f                 mbcm_dfe_fabric_flow_control_tx_gci_threshold_get;
    mbcm_dfe_fabric_flow_control_tx_drop_threshold_validate_f           mbcm_dfe_fabric_flow_control_tx_drop_threshold_validate;
    mbcm_dfe_fabric_flow_control_tx_drop_threshold_set_f                mbcm_dfe_fabric_flow_control_tx_drop_threshold_set;
    mbcm_dfe_fabric_flow_control_tx_drop_threshold_get_f                mbcm_dfe_fabric_flow_control_tx_drop_threshold_get;
    mbcm_dfe_fabric_flow_control_tx_rci_threshold_fc_validate_f         mbcm_dfe_fabric_flow_control_tx_rci_threshold_fc_validate;        
    mbcm_dfe_fabric_flow_control_tx_rci_threshold_fc_set_f              mbcm_dfe_fabric_flow_control_tx_rci_threshold_fc_set;
    mbcm_dfe_fabric_flow_control_tx_rci_threshold_fc_get_f              mbcm_dfe_fabric_flow_control_tx_rci_threshold_fc_get;
    mbcm_dfe_fabric_flow_control_tx_almost_full_threshold_validate_f    mbcm_dfe_fabric_flow_control_tx_almost_full_threshold_validate;
    mbcm_dfe_fabric_flow_control_tx_almost_full_threshold_set_f         mbcm_dfe_fabric_flow_control_tx_almost_full_threshold_set;
    mbcm_dfe_fabric_flow_control_tx_almost_full_threshold_get_f         mbcm_dfe_fabric_flow_control_tx_almost_full_threshold_get;
    mbcm_dfe_fabric_flow_control_tx_fifo_size_threshold_validate_f      mbcm_dfe_fabric_flow_control_tx_fifo_size_threshold_validate;
    mbcm_dfe_fabric_flow_control_tx_fifo_size_threshold_set_f           mbcm_dfe_fabric_flow_control_tx_fifo_size_threshold_set;
    mbcm_dfe_fabric_flow_control_tx_fifo_size_threshold_get_f           mbcm_dfe_fabric_flow_control_tx_fifo_size_threshold_get;
    mbcm_dfe_fabric_flow_control_mid_gci_threshold_validate_f           mbcm_dfe_fabric_flow_control_mid_gci_threshold_validate;
    mbcm_dfe_fabric_flow_control_mid_gci_threshold_set_f                mbcm_dfe_fabric_flow_control_mid_gci_threshold_set;
    mbcm_dfe_fabric_flow_control_mid_gci_threshold_get_f                mbcm_dfe_fabric_flow_control_mid_gci_threshold_get;
    mbcm_dfe_fabric_flow_control_mid_rci_threshold_validate_f           mbcm_dfe_fabric_flow_control_mid_rci_threshold_validate;
    mbcm_dfe_fabric_flow_control_mid_rci_threshold_set_f                mbcm_dfe_fabric_flow_control_mid_rci_threshold_set;
    mbcm_dfe_fabric_flow_control_mid_rci_threshold_get_f                mbcm_dfe_fabric_flow_control_mid_rci_threshold_get;
    mbcm_dfe_fabric_flow_control_mid_prio_drop_threshold_validate_f     mbcm_dfe_fabric_flow_control_mid_prio_drop_threshold_validate;
    mbcm_dfe_fabric_flow_control_mid_prio_drop_threshold_set_f          mbcm_dfe_fabric_flow_control_mid_prio_drop_threshold_set;
    mbcm_dfe_fabric_flow_control_mid_prio_drop_threshold_get_f          mbcm_dfe_fabric_flow_control_mid_prio_drop_threshold_get;
    mbcm_dfe_fabric_flow_control_mid_almost_full_threshold_validate_f   mbcm_dfe_fabric_flow_control_mid_almost_full_threshold_validate;
    mbcm_dfe_fabric_flow_control_mid_almost_full_threshold_set_f        mbcm_dfe_fabric_flow_control_mid_almost_full_threshold_set;
    mbcm_dfe_fabric_flow_control_mid_almost_full_threshold_get_f        mbcm_dfe_fabric_flow_control_mid_almost_full_threshold_get;
    mbcm_dfe_fabric_flow_control_mid_fifo_size_threshold_validate_f     mbcm_dfe_fabric_flow_control_mid_fifo_size_threshold_validate;
    mbcm_dfe_fabric_flow_control_mid_fifo_size_threshold_set_f          mbcm_dfe_fabric_flow_control_mid_fifo_size_threshold_set;
    mbcm_dfe_fabric_flow_control_mid_fifo_size_threshold_get_f          mbcm_dfe_fabric_flow_control_mid_fifo_size_threshold_get;
    mbcm_dfe_fabric_flow_control_mid_full_threshold_validate_f          mbcm_dfe_fabric_flow_control_mid_full_threshold_validate;
    mbcm_dfe_fabric_flow_control_mid_full_threshold_set_f               mbcm_dfe_fabric_flow_control_mid_full_threshold_set;
    mbcm_dfe_fabric_flow_control_mid_full_threshold_get_f               mbcm_dfe_fabric_flow_control_mid_full_threshold_get;
    mbcm_dfe_fabric_links_link_type_set_f                               mbcm_dfe_fabric_links_link_type_set;
    mbcm_dfe_fabric_links_link_type_get_f                               mbcm_dfe_fabric_links_link_type_get;
    mbcm_dfe_fabric_links_nof_links_get_f                               mbcm_dfe_fabric_links_nof_links_get;
    mbcm_dfe_fabric_links_validate_link_f                               mbcm_dfe_fabric_links_validate_link;
    mbcm_dfe_fabric_links_isolate_set_f                                 mbcm_dfe_fabric_links_isolate_set;
    mbcm_dfe_fabric_links_bmp_isolate_set_f                             mbcm_dfe_fabric_links_bmp_isolate_set;
    mbcm_dfe_fabric_links_isolate_get_f                                 mbcm_dfe_fabric_links_isolate_get;
    mbcm_dfe_fabric_links_cell_format_verify_f                          mbcm_dfe_fabric_links_cell_format_verify;
    mbcm_dfe_fabric_links_cell_format_set_f                             mbcm_dfe_fabric_links_cell_format_set;
    mbcm_dfe_fabric_links_cell_format_get_f                             mbcm_dfe_fabric_links_cell_format_get;
    mbcm_dfe_fabric_links_flow_status_control_cell_format_set_f         mbcm_dfe_fabric_links_flow_status_control_cell_format_set;
    mbcm_dfe_fabric_links_flow_status_control_cell_format_get_f         mbcm_dfe_fabric_links_flow_status_control_cell_format_get;
    mbcm_dfe_fabric_links_cell_interleaving_set_f                       mbcm_dfe_fabric_links_cell_interleaving_set;
    mbcm_dfe_fabric_links_cell_interleaving_get_f                       mbcm_dfe_fabric_links_cell_interleaving_get;
    mbcm_dfe_fabric_links_weight_validate_f                             mbcm_dfe_fabric_links_weight_validate;
    mbcm_dfe_fabric_links_weight_set_f                                  mbcm_dfe_fabric_links_weight_set;
    mbcm_dfe_fabric_links_weight_get_f                                  mbcm_dfe_fabric_links_weight_get;
    mbcm_dfe_fabric_links_secondary_only_set_f                          mbcm_dfe_fabric_links_secondary_only_set;
    mbcm_dfe_fabric_links_secondary_only_get_f                          mbcm_dfe_fabric_links_secondary_only_get;
    mbcm_dfe_fabric_links_llf_control_source_set_f                      mbcm_dfe_fabric_links_llf_control_source_set;
    mbcm_dfe_fabric_links_llf_control_source_get_f                      mbcm_dfe_fabric_links_llf_control_source_get;
    mbcm_dfe_fabric_links_aldwp_config_f                                mbcm_dfe_fabric_links_aldwp_config;
    mbcm_dfe_fabric_links_aldwp_init_f                                  mbcm_dfe_fabric_links_aldwp_init;
    mbcm_dfe_fabric_links_pcp_enable_set_f                              mbcm_dfe_fabric_links_pcp_enable_set;
    mbcm_dfe_fabric_links_pcp_enable_get_f                              mbcm_dfe_fabric_links_pcp_enable_get;
    mbcm_dfe_fabric_multicast_low_prio_drop_select_priority_set_f       mbcm_dfe_fabric_multicast_low_prio_drop_select_priority_set;
    mbcm_dfe_fabric_multicast_low_prio_drop_select_priority_get_f       mbcm_dfe_fabric_multicast_low_prio_drop_select_priority_get;
    mbcm_dfe_fabric_multicast_low_prio_threshold_validate_f             mbcm_dfe_fabric_multicast_low_prio_threshold_validate;
    mbcm_dfe_fabric_multicast_low_prio_threshold_set_f                  mbcm_dfe_fabric_multicast_low_prio_threshold_set;
    mbcm_dfe_fabric_multicast_low_prio_threshold_get_f                  mbcm_dfe_fabric_multicast_low_prio_threshold_get;
    mbcm_dfe_fabric_multicast_low_priority_drop_enable_set_f            mbcm_dfe_fabric_multicast_low_priority_drop_enable_set;
    mbcm_dfe_fabric_multicast_low_priority_drop_enable_get_f            mbcm_dfe_fabric_multicast_low_priority_drop_enable_get;
    mbcm_dfe_fabric_multicast_priority_range_validate_f                 mbcm_dfe_fabric_multicast_priority_range_validate;
    mbcm_dfe_fabric_multicast_priority_range_set_f                      mbcm_dfe_fabric_multicast_priority_range_set;
    mbcm_dfe_fabric_multicast_priority_range_get_f                      mbcm_dfe_fabric_multicast_priority_range_get;
    mbcm_dfe_fabric_multicast_multi_set_f                               mbcm_dfe_fabric_multicast_multi_set;
    mbcm_dfe_fabric_multicast_multi_get_f                               mbcm_dfe_fabric_multicast_multi_get;
    mbcm_dfe_fabric_multicast_multi_write_range_f                       mbcm_dfe_fabric_multicast_multi_write_range;
    mbcm_dfe_fabric_multicast_multi_read_info_get_f                     mbcm_dfe_fabric_multicast_multi_read_info_get;
    mbcm_dfe_fabric_link_repeater_enable_set_f                          mbcm_dfe_fabric_link_repeater_enable_set;
    mbcm_dfe_fabric_link_repeater_enable_get_f                          mbcm_dfe_fabric_link_repeater_enable_get;
    mbcm_dfe_fabric_link_status_all_get_f                               mbcm_dfe_fabric_link_status_all_get;
    mbcm_dfe_fabric_link_status_get_f                                   mbcm_dfe_fabric_link_status_get;
    mbcm_dfe_fabric_reachability_status_get_f                           mbcm_dfe_fabric_reachability_status_get;
    mbcm_dfe_fabric_link_connectivity_status_get_f                      mbcm_dfe_fabric_link_connectivity_status_get;
    mbcm_dfe_fabric_links_pipe_map_set_f                                mbcm_dfe_fabric_links_pipe_map_set;
    mbcm_dfe_fabric_links_pipe_map_get_f                                mbcm_dfe_fabric_links_pipe_map_get;
    mbcm_dfe_fabric_links_repeater_nof_remote_pipe_set_f                mbcm_dfe_fabric_links_repeater_nof_remote_pipe_set;
    mbcm_dfe_fabric_links_repeater_nof_remote_pipe_get_f                mbcm_dfe_fabric_links_repeater_nof_remote_pipe_get;
    mbcm_dfe_fabric_topology_isolate_set_f                              mbcm_dfe_fabric_topology_isolate_set;
    mbcm_dfe_fabric_topology_isolate_get_f                              mbcm_dfe_fabric_topology_isolate_get;
    mbcm_dfe_fabric_topology_rmgr_set_f                                 mbcm_dfe_fabric_topology_rmgr_set;
    mbcm_dfe_fabric_link_topology_set_f                                 mbcm_dfe_fabric_link_topology_set;
    mbcm_dfe_fabric_link_topology_get_f                                 mbcm_dfe_fabric_link_topology_get;
    mbcm_dfe_fabric_topology_min_nof_links_set_f                        mbcm_dfe_fabric_topology_min_nof_links_set;
    mbcm_dfe_fabric_topology_min_nof_links_get_f                        mbcm_dfe_fabric_topology_min_nof_links_get;
    mbcm_dfe_fabric_topology_nof_links_to_min_nof_links_default_f       mbcm_dfe_fabric_topology_nof_links_to_min_nof_links_default;
    mbcm_dfe_fabric_topology_repeater_destination_set_f                 mbcm_dfe_fabric_topology_repeater_destination_set;
    mbcm_dfe_fabric_topology_repeater_destination_get_f                 mbcm_dfe_fabric_topology_repeater_destination_get;
    mbcm_dfe_fabric_topology_reachability_mask_set_f                    mbcm_dfe_fabric_topology_reachability_mask_set;
    mbcm_dfe_fifo_dma_channel_init_f                                    mbcm_dfe_fifo_dma_channel_init;
    mbcm_dfe_fifo_dma_channel_deinit_f                                  mbcm_dfe_fifo_dma_channel_deinit;
    mbcm_dfe_fifo_dma_channel_clear_f                                   mbcm_dfe_fifo_dma_channel_clear;
    mbcm_dfe_fifo_dma_channel_read_entries_f                            mbcm_dfe_fifo_dma_channel_read_entries;
    mbcm_dfe_fifo_dma_fabric_cell_validate_f                            mbcm_dfe_fifo_dma_fabric_cell_validate;
    mbcm_dfe_nof_interrupts_f                                           mbcm_dfe_nof_interrupts;
    mbcm_dfe_nof_block_instances_f                                      mbcm_dfe_nof_block_instance;
    mbcm_dfe_multicast_egress_add_f                                     mbcm_dfe_multicast_egress_add;
    mbcm_dfe_multicast_egress_delete_f                                  mbcm_dfe_multicast_egress_delete;
    mbcm_dfe_multicast_egress_delete_all_f                              mbcm_dfe_multicast_egress_delete_all;
    mbcm_dfe_multicast_egress_get_f                                     mbcm_dfe_multicast_egress_get;
    mbcm_dfe_multicast_egress_set_f                                     mbcm_dfe_multicast_egress_set;
    mbcm_dfe_multicast_mode_get_f                                       mbcm_dfe_multicast_mode_get;
    mbcm_dfe_multicast_table_size_get_f                                 mbcm_dfe_multicast_table_size_get;
    mbcm_dfe_multicast_table_entry_size_get_f                           mbcm_dfe_multicast_table_entry_size_get;
    mbcm_dfe_port_soc_init_f                                            mbcm_dfe_port_soc_init;
    mbcm_dfe_port_init_f                                                mbcm_dfe_port_init;
    mbcm_dfe_port_deinit_f                                              mbcm_dfe_port_deinit;
    mbcm_dfe_port_detach_f                                              mbcm_dfe_port_detach;  
    mbcm_dfe_port_probe_f                                               mbcm_dfe_port_probe;
    mbcm_dfe_port_control_pcs_set_f                                     mbcm_dfe_port_control_pcs_set;
    mbcm_dfe_port_control_pcs_get_f                                     mbcm_dfe_port_control_pcs_get;
    mbcm_dfe_port_control_power_set_f                                   mbcm_dfe_port_control_power_set;
    mbcm_dfe_port_control_power_get_f                                   mbcm_dfe_port_control_power_get;
    mbcm_dfe_port_control_strip_crc_set_f                               mbcm_dfe_port_control_strip_crc_set;
    mbcm_dfe_port_control_strip_crc_get_f                               mbcm_dfe_port_control_strip_crc_get;
    mbcm_dfe_port_control_rx_enable_set_f                               mbcm_dfe_port_control_rx_enable_set;
    mbcm_dfe_port_control_tx_enable_set_f                               mbcm_dfe_port_control_tx_enable_set;
    mbcm_dfe_port_control_rx_enable_get_f                               mbcm_dfe_port_control_rx_enable_get;
    mbcm_dfe_port_control_tx_enable_get_f                               mbcm_dfe_port_control_tx_enable_get;
    mbcm_dfe_port_control_low_latency_set_f                             mbcm_dfe_port_control_low_latency_set;
    mbcm_dfe_port_control_low_latency_get_f                             mbcm_dfe_port_control_low_latency_get;
    mbcm_dfe_port_control_fec_error_detect_set_f                        mbcm_dfe_port_control_fec_error_detect_set;
    mbcm_dfe_port_control_fec_error_detect_get_f                        mbcm_dfe_port_control_fec_error_detect_get;
    mbcm_dfe_port_phy_enable_set_f                                      mbcm_dfe_port_phy_enable_set;
    mbcm_dfe_port_phy_enable_get_f                                      mbcm_dfe_port_phy_enable_get;
    mbcm_dfe_port_phy_cl72_set_f                                        mbcm_dfe_port_phy_cl72_set;
    mbcm_dfe_port_phy_cl72_get_f                                        mbcm_dfe_port_phy_cl72_get;
    mbcm_dfe_port_phy_control_set_f                                     mbcm_dfe_port_phy_control_set;
    mbcm_dfe_port_phy_control_get_f                                     mbcm_dfe_port_phy_control_get;
    mbcm_dfe_port_loopback_set_f                                        mbcm_dfe_port_loopback_set;
    mbcm_dfe_port_loopback_get_f                                        mbcm_dfe_port_loopback_get;
    mbcm_dfe_port_fault_get_f                                           mbcm_dfe_port_fault_get;
    mbcm_dfe_port_speed_get_f                                           mbcm_dfe_port_speed_get;
    mbcm_dfe_port_speed_max_f                                           mbcm_dfe_port_speed_max;
    mbcm_dfe_port_speed_set_f                                           mbcm_dfe_port_speed_set;
    mbcm_dfe_port_interface_set_f                                       mbcm_dfe_port_interface_set;
    mbcm_dfe_port_interface_get_f                                       mbcm_dfe_port_interface_get;
    mbcm_dfe_port_serdes_power_disable_f                                mbcm_dfe_port_serdes_power_disable;
    mbcm_dfe_port_link_status_get_f                                     mbcm_dfe_port_link_status_get;
    mbcm_dfe_port_bucket_fill_rate_validate_f                           mbcm_dfe_bucket_fill_rate_validate;
    mbcm_dfe_port_prbs_tx_enable_set_f                                  mbcm_dfe_port_prbs_tx_enable_set;
    mbcm_dfe_port_prbs_tx_enable_get_f                                  mbcm_dfe_port_prbs_tx_enable_get;
    mbcm_dfe_port_prbs_rx_enable_set_f                                  mbcm_dfe_port_prbs_rx_enable_set;
    mbcm_dfe_port_prbs_rx_enable_get_f                                  mbcm_dfe_port_prbs_rx_enable_get;
    mbcm_dfe_port_prbs_rx_status_get_f                                  mbcm_dfe_port_prbs_rx_status_get;
    mbcm_dfe_port_prbs_polynomial_set_f                                 mbcm_dfe_port_prbs_polynomial_set;   
    mbcm_dfe_port_prbs_polynomial_get_f                                 mbcm_dfe_port_prbs_polynomial_get;
    mbcm_dfe_port_prbs_tx_invert_data_set_f                             mbcm_dfe_port_prbs_tx_invert_data_set;
    mbcm_dfe_port_prbs_tx_invert_data_get_f                             mbcm_dfe_port_prbs_tx_invert_data_get;
    mbcm_dfe_port_pump_enable_set_f                                     mbcm_dfe_port_pump_enable_set;
    mbcm_dfe_port_rate_egress_ppt_set_f                                 mbcm_dfe_port_rate_egress_ppt_set;
    mbcm_dfe_port_rate_egress_ppt_get_f                                 mbcm_dfe_port_rate_egress_ppt_get;
    mbcm_dfe_port_burst_control_set_f                                   mbcm_dfe_port_burst_control_set;
    mbcm_dfe_port_extract_cig_from_llfc_enable_set_f                    mbcm_dfe_port_extract_cig_from_llfc_enable_set; 
    mbcm_dfe_port_extract_cig_from_llfc_enable_get_f                    mbcm_dfe_port_extract_cig_from_llfc_enable_get;
    mbcm_dfe_port_phy_get_f                                             mbcm_dfe_port_phy_get;
    mbcm_dfe_port_phy_set_f                                             mbcm_dfe_port_phy_set;
    mbcm_dfe_port_phy_modify_f                                          mbcm_dfe_port_phy_modify;
    mbcm_dfe_port_dynamic_port_update_f                                 mbcm_dfe_port_dynamic_port_update;
    mbcm_dfe_port_enable_set_f                                          mbcm_dfe_port_enable_set;
    mbcm_dfe_port_enable_get_f                                          mbcm_dfe_port_enable_get;
    mbcm_dfe_port_sync_e_link_set_f                                     mbcm_dfe_port_sync_e_link_set;
    mbcm_dfe_port_sync_e_divider_set_f                                  mbcm_dfe_port_sync_e_divider_set;
    mbcm_dfe_port_sync_e_link_get_f                                     mbcm_dfe_port_sync_e_link_get;
    mbcm_dfe_port_sync_e_divider_get_f                                  mbcm_dfe_port_sync_e_divider_get;
    mbcm_dfe_set_mesh_topology_config_f                                 mbcm_dfe_set_mesh_topology_config;
    mbcm_dfe_stk_modid_set_f                                            mbcm_dfe_stk_modid_set;
    mbcm_dfe_stk_modid_get_f                                            mbcm_dfe_stk_modid_get;
    mbcm_dfe_stk_module_max_all_reachable_verify_f                      mbcm_dfe_stk_module_max_all_reachable_verify;
    mbcm_dfe_stk_module_max_all_reachable_set_f                         mbcm_dfe_stk_module_max_all_reachable_set;
    mbcm_dfe_stk_module_max_all_reachable_get_f                         mbcm_dfe_stk_module_max_all_reachable_get;
    mbcm_dfe_stk_module_max_fap_verify_f                                mbcm_dfe_stk_module_max_fap_verify;
    mbcm_dfe_stk_module_max_fap_set_f                                   mbcm_dfe_stk_module_max_fap_set;
    mbcm_dfe_stk_module_max_fap_get_f                                   mbcm_dfe_stk_module_max_fap_get;
    mbcm_dfe_stk_module_all_reachable_ignore_id_set_f                   mbcm_dfe_stk_module_all_reachable_ignore_id_set;
    mbcm_dfe_stk_module_all_reachable_ignore_id_get_f                   mbcm_dfe_stk_module_all_reachable_ignore_id_get;
    mbcm_dfe_stk_valid_module_id_verify_f                               mbcm_dfe_stk_valid_module_id_verify;
    mbcm_dfe_stat_init_f                                                mbcm_dfe_stat_init;
    mbcm_dfe_stat_is_supported_type_f                                   mbcm_dfe_stat_is_supported_type;
    mbcm_dfe_stat_counter_length_get_f                                  mbcm_dfe_stat_counter_length_get;
    mbcm_dfe_stat_get_f                                                 mbcm_dfe_stat_get;
    mbcm_dfe_mapping_stat_get_f                                         mbcm_dfe_mapping_stat_get;
    mbcm_dfe_counters_get_info_f                                        mbcm_dfe_counters_get_info;
    mbcm_dfe_queues_get_info_f                                          mbcm_dfe_queues_get_info;
    mbcm_dfe_fabric_link_device_mode_get_f                              mbcm_dfe_fabric_link_device_mode_get;
    mbcm_dfe_controlled_counter_set_f                                   mbcm_dfe_controlled_counter_set;
    mbcm_dfe_get_soc_properties_array_f                                 mbcm_dfe_get_soc_properties_array;
    mbcm_dfe_avs_value_get_f                                            mbcm_dfe_avs_value_get;
    mbcm_dfe_linkctrl_init_f                                            mbcm_dfe_linkctrl_init;
    mbcm_dfe_interrupts_init_f                                          mbcm_dfe_interrupts_init;
    mbcm_dfe_interrupts_deinit_f                                        mbcm_dfe_interrupts_deinit;
    mbcm_dfe_interrupts_control_data_init_f                             mbcm_dfe_interrupts_control_data_init;
    mbcm_dfe_interrupts_control_data_deinit_f                           mbcm_dfe_interrupts_control_data_deinit;
    mbcm_dfe_interrupt_all_enable_set_f                                 mbcm_dfe_interrupt_all_enable_set;
    mbcm_dfe_interrupt_all_enable_get_f                                 mbcm_dfe_interrupt_all_enable_get;
    mbcm_dfe_drv_sw_ver_set_f                                           mbcm_dfe_drv_sw_ver_set;
    mbcm_dfe_drv_temperature_monitor_get_f                              mbcm_dfe_drv_temperature_monitor_get;
    mbcm_dfe_drv_test_reg_filter_f                                      mbcm_dfe_drv_test_reg_filter;
    mbcm_dfe_drv_test_reg_default_val_filter_f                          mbcm_dfe_drv_test_reg_default_val_filter;
    mbcm_dfe_drv_test_mem_filter_f                                      mbcm_dfe_drv_test_mem_filter;
    mbcm_dfe_drv_test_brdc_blk_filter_f                                 mbcm_dfe_drv_test_brdc_blk_filter;
    mbcm_dfe_drv_test_brdc_blk_info_get_f                               mbcm_dfe_drv_test_brdc_blk_info_get;
    mbcm_dfe_drv_asymmetrical_quad_get_f                                mbcm_dfe_drv_asymmetrical_quad_get;
    mbcm_dfe_warm_boot_buffer_id_supported_get_f                        mbcm_dfe_warm_boot_buffer_id_supported_get;
    mbcm_dfe_cosq_pipe_rx_weight_set_f                                  mbcm_dfe_cosq_pipe_rx_weight_set;
    mbcm_dfe_cosq_pipe_rx_weight_get_f                                  mbcm_dfe_cosq_pipe_rx_weight_get;
    mbcm_dfe_cosq_pipe_mid_weight_set_f                                 mbcm_dfe_cosq_pipe_mid_weight_set;
    mbcm_dfe_cosq_pipe_mid_weight_get_f                                 mbcm_dfe_cosq_pipe_mid_weight_get;
    mbcm_dfe_cosq_pipe_tx_weight_set_f                                  mbcm_dfe_cosq_pipe_tx_weight_set;
    mbcm_dfe_cosq_pipe_tx_weight_get_f                                  mbcm_dfe_cosq_pipe_tx_weight_get;
    mbcm_dfe_cosq_pipe_rx_threshold_set_f                               mbcm_dfe_cosq_pipe_rx_threshold_set;
    mbcm_dfe_cosq_pipe_rx_threshold_get_f                               mbcm_dfe_cosq_pipe_rx_threshold_get;
    mbcm_dfe_cosq_pipe_mid_threshold_set_f                              mbcm_dfe_cosq_pipe_mid_threshold_set;
    mbcm_dfe_cosq_pipe_mid_threshold_get_f                              mbcm_dfe_cosq_pipe_mid_threshold_get;
    mbcm_dfe_cosq_pipe_tx_threshold_set_f                               mbcm_dfe_cosq_pipe_tx_threshold_set;
    mbcm_dfe_cosq_pipe_tx_threshold_get_f                               mbcm_dfe_cosq_pipe_tx_threshold_get;
    mbcm_dfe_cosq_pipe_rx_rate_set_f                                    mbcm_dfe_cosq_pipe_rx_rate_set;
    mbcm_dfe_cosq_pipe_rx_rate_get_f                                    mbcm_dfe_cosq_pipe_rx_rate_get;
    mbcm_dfe_cosq_pipe_tx_rate_set_f                                    mbcm_dfe_cosq_pipe_tx_rate_set;
    mbcm_dfe_cosq_pipe_tx_rate_get_f                                    mbcm_dfe_cosq_pipe_tx_rate_get;
    mbcm_dfe_cosq_pipe_tx_rate_enable_set_f                             mbcm_dfe_cosq_pipe_tx_rate_enable_set;
    mbcm_dfe_cosq_pipe_tx_rate_enable_get_f                             mbcm_dfe_cosq_pipe_tx_rate_enable_get;
    mbcm_dfe_rx_cpu_address_modid_set_f                                 mbcm_dfe_rx_cpu_address_modid_set;
    mbcm_dfe_rx_cpu_address_modid_init_f                                mbcm_dfe_rx_cpu_address_modid_init;
    mbcm_dfe_port_quad_disabled_f                                       mbcm_dfe_port_quad_disabled;
    mbcm_dfe_drv_block_valid_get_f                                      mbcm_dfe_drv_block_valid_get;
} mbcm_dfe_functions_t; 


extern mbcm_dfe_functions_t *mbcm_dfe_driver[BCM_MAX_NUM_UNITS]; 


extern soc_dfe_chip_family_t mbcm_dfe_family[BCM_MAX_NUM_UNITS];


extern mbcm_dfe_functions_t mbcm_fe1600_driver;
#ifdef BCM_88950_A0
extern mbcm_dfe_functions_t mbcm_fe3200_driver;
#endif 

extern int mbcm_dfe_init(int unit);



#define MBCM_DFE_DRIVER_CALL(unit, function, args) ( (mbcm_dfe_driver[unit] && mbcm_dfe_driver[unit]->function) ? \
  mbcm_dfe_driver[unit]->function args : SOC_E_UNAVAIL )
#define MBCM_DFE_DRIVER_CALL_NO_ARGS(unit, function) ( (mbcm_dfe_driver[unit] && mbcm_dfe_driver[unit]->function) ? \
  mbcm_dfe_driver[unit]->function(unit) : SOC_E_UNAVAIL )
#define MBCM_DFE_DRIVER_CALL_NO_ARGS_VOID(unit, function) ( (mbcm_dfe_driver[unit] && mbcm_dfe_driver[unit]->function) ? \
  mbcm_dfe_driver[unit]->function(unit), SOC_E_NONE : SOC_E_UNAVAIL )
#define MBCM_DFE_DRIVER_CALL_VOID(unit, function, args) ( (mbcm_dfe_driver[unit] && mbcm_dfe_driver[unit]->function) ? \
  mbcm_dfe_driver[unit]->function args, SOC_E_NONE : SOC_E_UNAVAIL )

#if 0
#define MBCM_DFE_DRIVER_CALL_WITHOUT_DEV_ID(unit, function, args...) ( (mbcm_dfe_driver[unit] && mbcm_dfe_driver[unit]->function) ? \
  mbcm_dfe_driver[unit]->function(args) : SOC_E_UNAVAIL )
#endif






#endif  
