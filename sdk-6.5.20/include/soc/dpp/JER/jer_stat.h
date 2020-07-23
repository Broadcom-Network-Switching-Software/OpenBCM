/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * JERICHO STAT H
 */
 
#ifndef _SOC_JER_MAC_STAT_H_
#define _SOC_JER_MAC_STAT_H_

#include <soc/error.h>
#include <soc/dcmn/error.h>
#include <bcm/stat.h>
#include <soc/dpp/ARAD/arad_stat.h>

#define SOC_JER_NIF_ILKN_COUNTER_PER_CHANNEL_CANNEL_SUPPORTED_MAX       (256)

typedef enum soc_jer_counters_e {
    soc_jer_counters_tx_control_cells_counter = 0,
    soc_jer_counters_tx_data_cell_counter = 1,
    soc_jer_counters_tx_data_byte_counter = 2,
    soc_jer_counters_rx_crc_errors_data_cells = 3, 
    soc_jer_counters_rx_crc_errors_control_cells_nonbypass = 4, 
    soc_jer_counters_rx_crc_errors_control_cells_bypass = 5,
    soc_jer_counters_rx_fec_correctable_error = 6,
    soc_jer_counters_rx_control_cells_counter = 7,
    soc_jer_counters_rx_data_cell_counter = 8,
    soc_jer_counters_rx_data_byte_counter = 9,
    soc_jer_counters_rx_dropped_retransmitted_control = 10,
    soc_jer_counters_dummy_val_1 = 11,
    soc_jer_counters_dummy_val_2 = 12,
    soc_jer_counters_tx_asyn_fifo_rate = 13,
    soc_jer_counters_rx_asyn_fifo_rate = 14,
    soc_jer_counters_rx_lfec_fec_uncorrrectable_errors = 15,
    soc_jer_counters_rx_llfc_primary_pipe = 16,
    soc_jer_counters_rx_llfc_second_pipe = 17,
    soc_jer_counters_rx_llfc_third_pipe = 18,
    soc_jer_counters_rx_kpcs_errors_counter = 19,
    soc_jer_counters_rx_kpcs_bypass_errors_counter = 20,
    soc_jer_counters_ilkn_rx_pkt_counter = 21,
    soc_jer_counters_ilkn_tx_pkt_counter = 22,
    soc_jer_counters_ilkn_rx_byte_counter = 23,
    soc_jer_counters_ilkn_tx_byte_counter = 24,
    soc_jer_counters_ilkn_rx_err_pkt_counter = 25,
    soc_jer_counters_ilkn_tx_err_pkt_counter = 26,
    soc_jer_counters_rx_eth_stats_drop_events = 27,
    soc_jer_counters_nif_rx_fec_correctable_error = 28,
    soc_jer_counters_nif_rx_fec_uncorrrectable_errors = 29,
    soc_jer_counters_nif_rx_bip_error = 30
}soc_jer_counters_t;

soc_error_t soc_jer_stat_nif_init(int unit);

soc_error_t soc_jer_fabric_stat_init(int unit);

soc_error_t soc_jer_mapping_stat_get(int unit,soc_port_t port, uint32 *counters,int *array_size ,bcm_stat_val_t type,int max_array_size);
soc_error_t soc_jer_stat_counter_length_get(int unit, int counter_id, int *length);

int soc_jer_nif_controlled_counter_clear(int unit, int port);

soc_error_t soc_jer_stat_controlled_counter_enable_get(int unit, soc_port_t port, int index, int *enable, int *printable);

#endif 
