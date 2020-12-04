/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * RAMON STAT H
 */
 
#ifndef _SOC_RAMON_STAT_H_
#define _SOC_RAMON_STAT_H_

#ifndef BCM_DNXF_SUPPORT 
#error "This file is for use by DNXF (Ramon) family only!" 
#endif 

#include <bcm/stat.h>
#include <bcm/fabric.h>
#include <soc/dnxf/cmn/dnxf_defs.h>
#include <soc/error.h>
#include <shared/shrextend/shrextend_error.h>

#define _SOC_RAMON_FABRIC_STAT_PIPE_NO_VERIFY   (-1)

typedef enum soc_ramon_counters_e {
    soc_ramon_counters_tx_control_cells_counter = 0,
    soc_ramon_counters_tx_data_cell_counter = 1,
    soc_ramon_counters_tx_data_byte_counter = 2,
    soc_ramon_counters_rx_crc_errors_data_cells = 3, 
    soc_ramon_counters_rx_crc_errors_control_cells_nonbypass = 4, 
    soc_ramon_counters_rx_crc_errors_control_cells_bypass = 5,
    soc_ramon_counters_rx_fec_correctable_error = 6,
    soc_ramon_counters_rx_control_cells_counter = 7,
    soc_ramon_counters_rx_data_cell_counter = 8,
    soc_ramon_counters_rx_data_byte_counter = 9,
    soc_ramon_counters_rx_dropped_retransmitted_control = 10,
    soc_ramon_counters_dummy_val_1 = 11,
    soc_ramon_counters_dummy_val_2 = 12,
    soc_ramon_counters_tx_asyn_fifo_rate = 13,
    soc_ramon_counters_rx_asyn_fifo_rate = 14,
    soc_ramon_counters_rx_lfec_fec_uncorrrectable_errors = 15,
    soc_ramon_counters_rx_llfc_primary_pipe = 16,
    soc_ramon_counters_rx_llfc_second_pipe = 17,
    soc_ramon_counters_rx_llfc_third_pipe = 18,
    soc_ramon_counters_rx_kpcs_errors_counter = 19,
    soc_ramon_counters_rx_kpcs_bypass_errors_counter = 20,
    soc_ramon_counters_rx_rs_fec_bit_error_counter = 21,
    soc_ramon_counters_rx_rs_fec_symbol_error_counter = 22
}soc_ramon_counters_t;

shr_error_e soc_ramon_controlled_counter_set(int unit);
shr_error_e soc_ramon_stat_init(int unit);
shr_error_e soc_ramon_mapping_stat_get(int unit,bcm_port_t port, int *counters,int *array_size ,bcm_stat_val_t type,int max_array_size);
shr_error_e soc_ramon_stat_counter_length_get(int unit, int counter_id, int *length);
shr_error_e soc_ramon_stat_is_supported_type(int unit, bcm_port_t port, bcm_stat_val_t type);
shr_error_e soc_ramon_stat_get(int unit,bcm_port_t port,uint64 *value,int *counters,int arr_size);
shr_error_e soc_ramon_fabric_stat_get(int unit, bcm_fabric_stat_index_t index, bcm_fabric_stat_t stat, uint64 *value);
int soc_ramon_controlled_counter_get(int unit, int counter_id, int port, uint64* val, uint32* clear_on_read);

#endif

