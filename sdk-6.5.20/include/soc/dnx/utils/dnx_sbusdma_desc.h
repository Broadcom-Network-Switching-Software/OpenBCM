
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef DNX_H_SBUSDMA_DESC_INCLUDED

#define DNX_H_SBUSDMA_DESC_INCLUDED

#include <shared/shrextend/shrextend_debug.h>

typedef enum
{
    SBUSDMA_DESC_MODULE_ENABLE_MDB_LPM = 0,
    SBUSDMA_DESC_MODULE_ENABLE_MDB_EEDB = 1,
    SBUSDMA_DESC_MODULE_ENABLE_MDB_INLIF = 2,
    SBUSDMA_DESC_MODULE_ENABLE_MDB_VSI = 3,
    SBUSDMA_DESC_MODULE_ENABLE_MDB_FEC = 4,
    SBUSDMA_DESC_MODULE_ENABLE_MDB_MAP = 5,
    SBUSDMA_DESC_MODULE_ENABLE_MDB_ISEM = 6,
    SBUSDMA_DESC_MODULE_ENABLE_MDB_LEM = 7,
    SBUSDMA_DESC_MODULE_ENABLE_MDB_OEM = 8,
    SBUSDMA_DESC_MODULE_ENABLE_MDB_PPMC = 9,
    SBUSDMA_DESC_MODULE_ENABLE_MDB_GLEM = 10,
    SBUSDMA_DESC_MODULE_ENABLE_MDB_ESEM = 11,
    SBUSDMA_DESC_MODULE_ENABLE_MDB_EXEM = 12,
    SBUSDMA_DESC_MODULE_ENABLE_MDB_RMEP_EM = 13,
    SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES
} sbusdma_desc_module_enable_e;

typedef struct dnx_sbusdma_desc_stats_s
{

    uint32 buff_size_kb;
    uint32 chain_length_max;
    uint32 timeout_usec;
    uint32 enabled;

    uint32 commit_counter;
    uint32 chain_length_counter_max;
    uint32 chain_length_counter_total;
    uint32 buff_size_kb_max;
    uint32 buff_size_kb_total;
} dnx_sbusdma_desc_stats_t;

shr_error_e dnx_sbusdma_desc_init_with_params(
    int unit,
    uint32 buff_size_kb,
    uint32 chain_length_max,
    uint32 timeout_usec);

shr_error_e dnx_sbusdma_desc_get_stats(
    int unit,
    int clear,
    dnx_sbusdma_desc_stats_t * desc_stats);

shr_error_e dnx_sbusdma_desc_init(
    int unit);

shr_error_e dnx_sbusdma_desc_deinit(
    int unit);

shr_error_e dnx_sbusdma_desc_wait_done(
    int unit);

shr_error_e dnx_sbusdma_desc_add_mem(
    int unit,
    uint32 mem,
    int array_index,
    int blk,
    uint32 offset,
    void *entry_data);

shr_error_e dnx_sbusdma_desc_add_mem_table(
    int unit,
    uint32 mem,
    int array_index,
    int blk,
    uint32 offset,
    uint32 count,
    void *entry_data);

shr_error_e dnx_sbusdma_desc_add_mem_clear(
    int unit,
    uint32 mem,
    int array_index,
    int blk,
    void *entry_data);

shr_error_e dnx_sbusdma_desc_add_reg(
    int unit,
    uint32 reg,
    int instance,
    uint32 array_index,
    void *entry_data);

uint32 dnx_sbusdma_desc_is_enabled(
    int unit,
    sbusdma_desc_module_enable_e module_enum);

#endif
