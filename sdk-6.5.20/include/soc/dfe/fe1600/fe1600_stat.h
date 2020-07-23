/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * FE1600 STAT H
 */
 
#ifndef _SOC_FE1600_STAT_H_
#define _SOC_FE1600_STAT_H_

#include <bcm/stat.h>
#include <soc/dfe/cmn/dfe_defs.h>
#include <soc/error.h>

typedef enum soc_fe1600_counters_e {
    soc_fe1600_counters_tx_control_cells_counter = 0,
    soc_fe1600_counters_tx_data_cell_counter = 1,
    soc_fe1600_counters_tx_data_byte_counter = 2,
    soc_fe1600_counters_rx_crc_errors_counter = 3, 
    soc_fe1600_counters_rx_lfec_fec_correctable_error = 4, 
    soc_fe1600_counters_rx_bec_crc_error = 4,
    soc_fe1600_counters_rx_8b_10b_disparity_errors = 4,
    soc_fe1600_counters_rx_control_cells_counter = 5,
    soc_fe1600_counters_rx_data_cell_counter = 6,
    soc_fe1600_counters_rx_data_byte_counter = 7,
    soc_fe1600_counters_rx_dropped_retransmitted_control = 8,
    soc_fe1600_counters_tx_bec_retransmit = 9,
    soc_fe1600_counters_rx_bec_retransmit = 10,
    soc_fe1600_counters_tx_asyn_fifo_rate_at_units_of_40_bits = 11,
    soc_fe1600_counters_rx_asyn_fifo_rate_at_units_of_40_bits = 12,
    soc_fe1600_counters_rx_lfec_fec_uncorrectable_erros = 13,
    soc_fe1600_counters_rx_bec_rx_fault = 13,
    soc_fe1600_counters_rx_8b_10b_code_errors = 13,
    soc_fe1600_nof_counters = 14
}soc_fe1600_counters_t;

soc_error_t soc_fe1600_stat_init(int unit);
soc_error_t soc_fe1600_stat_is_supported_type(int unit, bcm_port_t port, bcm_stat_val_t type);
soc_error_t soc_fe1600_stat_get(int unit,bcm_port_t port,uint64 *value,int *counters,int arr_size);
soc_error_t soc_fe1600_mapping_stat_get(int unit,bcm_port_t port, int *counters,int *array_size ,bcm_stat_val_t type,int max_array_size);

soc_error_t soc_fe1600_controlled_counter_set(int unit);
int soc_fe1600_controlled_counter_get(int unit, int counter_id, int port, uint64* val, uint32* clear_on_read);
soc_error_t soc_fe1600_stat_counter_length_get(int unit, int counter_id, int *length);
#endif 
