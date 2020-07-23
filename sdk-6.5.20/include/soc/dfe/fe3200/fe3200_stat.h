/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * FE3200 STAT H
 */
 
#ifndef _SOC_FE3200_STAT_H_
#define _SOC_FE3200_STAT_H_

#include <bcm/stat.h>
#include <soc/dfe/cmn/dfe_defs.h>
#include <soc/error.h>

typedef enum soc_fe3200_counters_e {
    soc_fe3200_counters_tx_control_cells_counter = 0,
    soc_fe3200_counters_tx_data_cell_counter = 1,
    soc_fe3200_counters_tx_data_byte_counter = 2,
    soc_fe3200_counters_rx_crc_errors_data_cells = 3, 
    soc_fe3200_counters_rx_crc_errors_control_cells_nonbypass = 4, 
    soc_fe3200_counters_rx_crc_errors_control_cells_bypass = 5,
    soc_fe3200_counters_rx_fec_correctable_error = 6,
    soc_fe3200_counters_rx_control_cells_counter = 7,
    soc_fe3200_counters_rx_data_cell_counter = 8,
    soc_fe3200_counters_rx_data_byte_counter = 9,
    soc_fe3200_counters_rx_dropped_retransmitted_control = 10,
    soc_fe3200_counters_dummy_val_1 = 11,
    soc_fe3200_counters_dummy_val_2 = 12,
    soc_fe3200_counters_tx_asyn_fifo_rate = 13,
    soc_fe3200_counters_rx_asyn_fifo_rate = 14,
    soc_fe3200_counters_rx_lfec_fec_uncorrrectable_errors = 15,
    soc_fe3200_counters_rx_llfc_primary_pipe = 16,
    soc_fe3200_counters_rx_llfc_second_pipe = 17,
    soc_fe3200_counters_rx_llfc_third_pipe = 18,
    soc_fe3200_counters_rx_kpcs_errors_counter = 19,
    soc_fe3200_counters_rx_kpcs_bypass_errors_counter = 20
}soc_fe3200_counters_t;

soc_error_t soc_fe3200_controlled_counter_set(int unit);
soc_error_t soc_fe3200_stat_init(int unit);
soc_error_t soc_fe3200_mapping_stat_get(int unit,bcm_port_t port, int *counters,int *array_size ,bcm_stat_val_t type,int max_array_size);
soc_error_t soc_fe3200_stat_counter_length_get(int unit, int counter_id, int *length);

#endif

