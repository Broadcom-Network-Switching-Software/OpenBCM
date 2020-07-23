/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * FE1600 STAT H
 */
 
#ifndef _SOC_ARAD_MAC_STAT_H_
#define _SOC_ARAD_MAC_STAT_H_

#include <bcm/stat.h>
#include <soc/error.h>
#include <soc/dpp/dpp_defs.h>

#include <soc/dpp/ARAD/arad_api_nif.h>

extern soc_controlled_counter_t soc_arad_controlled_counter[];

#define SOC_ARAD_MAC_COUNTER_FIRST                                  ARAD_NIF_NOF_COUNTER_TYPES
#define SOC_ARAD_MAC_COUNTERS_TX_CONTROL_CELLS_COUNTER              (SOC_ARAD_MAC_COUNTER_FIRST + 0)
#define SOC_ARAD_MAC_COUNTERS_TX_DATA_CELL_COUNTER                  (SOC_ARAD_MAC_COUNTER_FIRST + 1)
#define SOC_ARAD_MAC_COUNTERS_TX_DATA_BYTE_COUNTER                  (SOC_ARAD_MAC_COUNTER_FIRST + 2)
#define SOC_ARAD_MAC_COUNTERS_RX_CRC_ERRORS_COUNTER                 (SOC_ARAD_MAC_COUNTER_FIRST + 3) 
#define SOC_ARAD_MAC_COUNTERS_RX_LFEC_FEC_CORRECTABLE_ERROR         (SOC_ARAD_MAC_COUNTER_FIRST + 4) 
#define SOC_ARAD_MAC_COUNTERS_RX_BEC_CRC_ERROR                      (SOC_ARAD_MAC_COUNTER_FIRST + 4)
#define SOC_ARAD_MAC_COUNTERS_RX_8B_10B_DISPARITY_ERRORS            (SOC_ARAD_MAC_COUNTER_FIRST + 4)
#define SOC_ARAD_MAC_COUNTERS_RX_CONTROL_CELLS_COUNTER              (SOC_ARAD_MAC_COUNTER_FIRST + 5)
#define SOC_ARAD_MAC_COUNTERS_RX_DATA_CELL_COUNTER                  (SOC_ARAD_MAC_COUNTER_FIRST + 6)
#define SOC_ARAD_MAC_COUNTERS_RX_DATA_BYTE_COUNTER                  (SOC_ARAD_MAC_COUNTER_FIRST + 7)
#define SOC_ARAD_MAC_COUNTERS_RX_DROPPED_RETRANSMITTED_CONTROL      (SOC_ARAD_MAC_COUNTER_FIRST + 8)
#define SOC_ARAD_MAC_COUNTERS_TX_BEC_RETRANSMIT                     (SOC_ARAD_MAC_COUNTER_FIRST + 9)
#define SOC_ARAD_MAC_COUNTERS_RX_BEC_RETRANSMIT                     (SOC_ARAD_MAC_COUNTER_FIRST + 10)
#define SOC_ARAD_MAC_COUNTERS_TX_ASYN_FIFO_RATE_AT_UNITS_OF_40_BITS (SOC_ARAD_MAC_COUNTER_FIRST + 11)
#define SOC_ARAD_MAC_COUNTERS_RX_ASYN_FIFO_RATE_AT_UNITS_OF_40_BITS (SOC_ARAD_MAC_COUNTER_FIRST + 12)
#define SOC_ARAD_MAC_COUNTERS_RX_LFEC_FEC_UNCORRECTABLE_ERRORS      (SOC_ARAD_MAC_COUNTER_FIRST + 13)
#define SOC_ARAD_MAC_COUNTERS_RX_BEC_RX_FAULT                       (SOC_ARAD_MAC_COUNTER_FIRST + 13)
#define SOC_ARAD_MAC_COUNTERS_RX_8B_10B_CODE_ERRORS                 (SOC_ARAD_MAC_COUNTER_FIRST + 13)
#define SOC_ARAD_MAC_COUNTERS_RX_LLFC_PRIMARY                       (SOC_ARAD_MAC_COUNTER_FIRST + 14)
#define SOC_ARAD_MAC_COUNTERS_RX_LLFC_SECONDARY                     (SOC_ARAD_MAC_COUNTER_FIRST + 15)

#define SOC_ARAD_NOF_MAC_COUNTERS                                   16
#define SOC_ARAD_NOF_COUNTERS                                       (ARAD_NIF_NOF_COUNTER_TYPES+SOC_ARAD_NOF_MAC_COUNTERS)


typedef enum soc_arad_stat_ilkn_counters_mode_e {
    soc_arad_stat_ilkn_counters_mode_physical,
    soc_arad_stat_ilkn_counters_mode_packets_per_channel
} soc_arad_stat_ilkn_counters_mode_t;

#define SOC_ARAD_NIF_ILKN_COUNTER_PER_CHANNEL_CANNEL_SUPPORTED_MAX  (112)

#define SOC_ARAD_STAT_COUNTER_MODE_PACKETS_PER_CHANNEL(unit, port)                                                                  \
                (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), port) &&                                                                        \
                SOC_DPP_CONFIG(unit)->arad->init.ports.ilkn_counters_mode == soc_arad_stat_ilkn_counters_mode_packets_per_channel)
#define SOC_ARAD_STAT_COUNTER_MODE_PHISYCAL(unit, port)                                                                             \
                (!SOC_ARAD_STAT_COUNTER_MODE_PACKETS_PER_CHANNEL(unit, port))

soc_error_t soc_arad_fabric_stat_init(int unit);
soc_error_t soc_arad_stat_nif_init(int unit);
soc_error_t soc_arad_mapping_stat_get(int unit, soc_port_t port, uint32 *cnt_type, int *num_cntrs_p, bcm_stat_val_t type, int num_cntrs_in);
soc_error_t soc_arad_stat_clear_on_read_set(int unit, int enable);
soc_error_t soc_arad_stat_controlled_counter_enable_get(int unit, soc_port_t port, int index, int *enable, int *printable);
soc_error_t soc_arad_stat_counter_length_get(int unit, int counter_id, int *length);
int soc_arad_mac_controlled_counter_get(int unit, int counter_id, int port, uint64* val, uint32* clear_on_read);
soc_error_t soc_arad_stat_path_info_get(int unit, soc_dpp_stat_path_info_t *info);



#endif 
