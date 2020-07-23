
/*
 *                                                            $
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. $
 */

#ifndef ADAPTER_REG_ACCESS_H_INCLUDED

#define ADAPTER_REG_ACCESS_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <include/soc/register.h>
#include <shared/shrextend/shrextend_debug.h>

#define MAX_PACKET_SIZE_ADAPTER 16384
#define MAX_SIGNAL_SIZE 256

#define MAX_PACKETS_IN_PARALLEL 10

#define UPDATE_TO_SOC_REG_READ 0

#define UPDATE_TO_SOC_MEM_READ 1

#define UPDATE_TO_SOC_REG_WRITE 4

#define UPDATE_TO_SOC_MEM_WRITE 5

typedef enum
{

    ADAPTER_MS_ID_LOOPBACK = -3,
    ADAPTER_MS_ID_FIRST_MS = -1,

    ADAPTER_MS_ID_TERMINATE_SERVER = -5
} adapter_ms_id_e;

typedef enum
{
    ADAPTER_RX_TX,
    ADAPTER_REGS,
    ADAPTER_SIGNALS,
    ADAPTER_EVENTS
} socket_target_e;

extern int pipe_fds[2];

extern int adapter_sub_unit_id;

int adapter_reg32_get(
    int unit,
    soc_reg_t reg,
    int port,
    int index,
    uint32 *data);

int adapter_reg64_get(
    int unit,
    soc_reg_t reg,
    int port,
    int index,
    uint64 *data);

int adapter_reg_above64_get(
    int unit,
    soc_reg_t reg,
    int port,
    int index,
    soc_reg_above_64_val_t data);

int adapter_reg32_set(
    int unit,
    soc_reg_t reg,
    int port,
    int index,
    uint32 data);

int adapter_reg64_set(
    int unit,
    soc_reg_t reg,
    int port,
    int index,
    uint64 data);

int adapter_reg_above64_set(
    int unit,
    soc_reg_t reg,
    int port,
    int index,
    soc_reg_above_64_val_t data);

int adapter_mem_array_read(
    int unit,
    soc_mem_t mem,
    unsigned int array_index,
    int copyno,
    int index,
    void *entry_data);

int adapter_mem_array_write(
    int unit,
    soc_mem_t mem,
    unsigned int array_index,
    int copyno,
    int index,
    void *entry_data);

shr_error_e adapter_get_signal(
    int unit,
    uint32 ms_id,
    uint32 *signal_data_length,
    char **signal_data);

int adapter_write_buffer_to_socket(
    int unit,
    adapter_ms_id_e ms_id,
    socket_target_e target,
    int len,
    int num_of_signals,
    uint8 *buf);

int adapter_read_hit_bits(
    int unit,
    int read_and_delete,
    uint32 *record_length,
    char **recorded_data);

int control_get_table_data(
    int unit,
    char *tbl_name,
    char *blk_name,
    int entry_index,
    uint32 tbl_entry_data_allocated_size,
    uint32 *tbl_entry_size,
    char *tbl_entry_data);

int adapter_get_block_names(
    int unit,
    int *block_names_length,
    char **block_names);

int adapter_reg_access_init(
    int unit,
    int sub_unit_id);

int adapter_reg_access_deinit(
    int unit,
    int sub_unit_id);

int adapter_disable_collect_accesses(
    int unit,
    int sub_unit_id);

int adapter_dnx_init_done(
    int unit);

uint32 adapter_read_buffer(
    int unit,
    int sub_unit_id,
    adapter_ms_id_e * ms_id,
    uint32 *nof_signals,
    int *ifc_type,
    uint32 *port_channel,
    uint32 *port_first_lane,
    int *len,
    unsigned char *buf);

uint32 adapter_send_buffer(
    int unit,
    adapter_ms_id_e ms_id,
    uint32 src_port,
    uint32 src_channel,
    int len,
    unsigned char *buf,
    int nof_signals);

int adapter_memreg_access(
    int unit,
    int cmic_block,
    uint32 addr,
    uint32 data_length,
    int is_mem,
    int is_get,
    uint32 *data);

uint32 adapter_terminate_server(
    int unit);

#endif
