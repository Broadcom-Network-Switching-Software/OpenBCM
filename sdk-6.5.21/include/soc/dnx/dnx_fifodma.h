
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *  All Rights Reserved.
 */

#ifndef DNX_FIFODMA_H_INCLUDED

#define DNX_FIFODMA_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#ifdef DNX_DATA_INTERNAL
#undef DNX_DATA_INTERNAL
#endif
#include <shared/shrextend/shrextend_debug.h>

typedef void (
    *soc_dnx_fifodma_interrupt_handler) (
    int unit,
    void *entry,
    int entry_size,
    int entry_number,
    int total_amount_of_entries,
    void **user_data);

typedef enum soc_dnx_fifodma_src_e
{
    soc_dnx_fifodma_src_oam_event,
    soc_dnx_fifodma_src_oam_status,
    soc_dnx_fifodma_src_olp,
    soc_dnx_fifodma_src_crps0_dma0,
    soc_dnx_fifodma_src_crps0_dma1,
    soc_dnx_fifodma_src_crps1_dma0,
    soc_dnx_fifodma_src_crps1_dma1,
    soc_dnx_fifodma_src_event_fifo,
    soc_dnx_fifodma_src_8_reserved,
    soc_dnx_fifodma_src_9_reserved,
    soc_dnx_fifodma_src_10_reserved,
    soc_dnx_fifodma_src_11_reserved,
    soc_dnx_fifodma_src_reserved = 0xf
} soc_dnx_fifodma_src_t;

typedef struct
{

    uint32 max_entries;

    int is_mem;

    int mem;

    int reg;

    int array_index;

    int copyno;

    int force_entry_size;

    int threshold;

    int timeout;

    int is_poll;

    uint32 entry_size;

    int is_alloced;

    int is_started;

    int is_paused;

    int last_entry_id;

    uint8 *host_buff;
} soc_dnx_fifodma_config_t;

shr_error_e soc_dnx_fifodma_config_t_init(
    int unit,
    soc_dnx_fifodma_config_t * fifodma_info);

shr_error_e soc_dnx_fifodma_channel_alloc(
    int unit,
    const soc_dnx_fifodma_src_t source,
    int *chan);

shr_error_e soc_dnx_fifodma_channel_get(
    int unit,
    const soc_dnx_fifodma_src_t source,
    int *chan);

shr_error_e soc_dnx_fifodma_channel_is_started(
    int unit,
    soc_dnx_fifodma_src_t source,
    int *is_started);

shr_error_e soc_dnx_fifodma_channel_start(
    int unit,
    int chan,
    const soc_dnx_fifodma_config_t * fifodma_info,
    char *callback_name);

shr_error_e soc_dnx_fifodma_channel_stop(
    int unit,
    int chan);

shr_error_e soc_dnx_fifodma_channel_free(
    int unit,
    int chan);

shr_error_e soc_dnx_fifodma_channel_info_get(
    int unit,
    int chan,
    soc_dnx_fifodma_config_t * fifodma_info);

shr_error_e soc_dnx_fifodma_num_entries_get(
    int unit,
    int chan,
    int *count,
    uint8 **entries,
    uint32 *entry_size);

shr_error_e soc_dnx_fifodma_set_entries_read(
    int unit,
    int chan,
    int count);

shr_error_e soc_dnx_fifodma_channel_resume(
    int unit,
    int chan);

shr_error_e soc_dnx_fifodma_channel_pause(
    int unit,
    int chan);

void dnx_fifodma_intr_handler(
    void *unit_ptr,
    void *chan_ptr,
    void *unused_p3,
    void *unused_p4,
    void *unused_p5);

void dnx_fifodma_interrupt_handler_example(
    int unit,
    void *entry,
    int entry_size,
    int entry_number,
    int total_amount_of_entries,
    void **user_data);

shr_error_e soc_dnx_fifodma_init(
    int unit);

shr_error_e soc_dnx_fifodma_deinit(
    int unit);

#endif
