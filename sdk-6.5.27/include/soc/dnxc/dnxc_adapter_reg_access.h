
/*
 *                                                            $
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved. $
 */

#ifndef DNXC_ADAPTER_REG_ACCESS_H_INCLUDED

#define DNXC_ADAPTER_REG_ACCESS_H_INCLUDED

typedef enum
{
    SPECIAL_SWAP_TYPE_NONE = 0,
    SPECIAL_SWAP_TYPE_ENTRY_OFFSET,
    SPECIAL_SWAP_TYPE_DATA_OFFSET,
    SPECIAL_SWAP_TYPE_ENTRY_DATA,
    SPECIAL_SWAP_TYPE_NBU_BLK_INSTANCE
} dnxc_adapter_special_swap_type_e;

typedef enum
{
    BLOCK_TYPE_SWAP_IND_NONE = 0,
    BLOCK_TYPE_SWAP_IND_IGNORE,
    BLOCK_TYPE_SWAP_IND_SPECIAL_REGMEM,
} dnxc_adapter_block_type_swap_indication_e;

#ifdef ADAPTER_SERVER_MODE

#include <include/soc/register.h>
#include <soc/sand/shrextend/shrextend_debug.h>

#define MAX_PACKET_SIZE_ADAPTER (16360*8 + 100)

#define MAX_SIGNAL_SIZE 256

#define MAX_PACKETS_IN_PARALLEL 10

#define UPDATE_TO_SOC_REG_READ 0

#define UPDATE_TO_SOC_MEM_READ 1

#define UPDATE_TO_SOC_REG_WRITE 4

#define UPDATE_TO_SOC_MEM_WRITE 5

#define ADAPTER_MS_RUN_UNIT_TEST_RESPONSE_CONSTANT_HEADER_SIZE (21)
#define ADAPTER_MS_RUN_UNIT_TEST_RESPONSE_MAX_MS_NAME_SIZE (65)
#define ADAPTER_MS_RUN_UNIT_TEST_RESPONSE_MAX_MESSAGE_SIZE (257)
#define ADAPTER_MS_RUN_UNIT_TEST_RESPONSE_MAX_HEADER_SIZE \
    (ADAPTER_MS_RUN_UNIT_TEST_RESPONSE_CONSTANT_HEADER_SIZE + ADAPTER_MS_RUN_UNIT_TEST_RESPONSE_MAX_MS_NAME_SIZE + ADAPTER_MS_RUN_UNIT_TEST_RESPONSE_MAX_MESSAGE_SIZE)

#define ADAPTER_MS_RUN_UNIT_TEST_REQUEST_CONSTANT_HEADER_SIZE (9)
#define ADAPTER_MS_RUN_UNIT_TEST_REQUEST_MAX_MS_NAME_SIZE (65)
#define ADAPTER_MS_RUN_UNIT_TEST_REQUEST_MAX_HEADER_SIZE (ADAPTER_MS_RUN_UNIT_TEST_REQUEST_CONSTANT_HEADER_SIZE + ADAPTER_MS_RUN_UNIT_TEST_REQUEST_MAX_MS_NAME_SIZE)

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
    int core,
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

int adapter_ms_run_unit_test(
    int unit,
    char *ms_name,
    uint32 unit_test_id,
    uint32 *unit_test_result,
    char message[ADAPTER_MS_RUN_UNIT_TEST_RESPONSE_MAX_MESSAGE_SIZE]);

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
    int len,
    unsigned char *buf,
    int nof_signals);

int adapter_memreg_access(
    int unit,
    int sub_unit_index,
    int cmic_block,
    uint32 addr,
    int nof_addresses,
    uint32 data_length,
    int is_mem,
    int is_get,
    int is_clear_on_read,
    uint32 *data);

uint32 adapter_terminate_server(
    int unit);

shr_error_e dnxc_adapter_init(
    int unit);

shr_error_e dnxc_init_adapter_reg_access_deinit(
    int unit);

soc_error_t dnxc_adapter_regmem_access_reset(
    int unit);

int adapter_nof_sub_units_get(
    int unit);
#endif

#endif
