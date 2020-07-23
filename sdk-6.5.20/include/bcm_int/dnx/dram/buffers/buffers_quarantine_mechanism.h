

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _BCMINT_DNX_DRAM_BUFFERS_BUFFERSQUARANTINEMECHANISM_H_INCLUDED

#define _BCMINT_DNX_DRAM_BUFFERS_BUFFERSQUARANTINEMECHANISM_H_INCLUDED

#include <shared/shrextend/shrextend_debug.h>






shr_error_e dnx_dram_buffers_quarantine_buffer_to_quarantine_schedule(
    int unit,
    bcm_core_t core,
    uint32 buffer);


shr_error_e dnx_dram_buffers_quarantine_nof_free_buffers_get(
    int unit,
    uint32 *nof_free_buffers);


shr_error_e dnx_dram_buffers_quarantine_buffer_to_quarantine_done(
    int unit,
    bcm_core_t core);


shr_error_e dnx_dram_buffers_quarantine_buffer_flush_all(
    int unit);


shr_error_e dnx_dram_buffers_quarantine_nof_deleted_buffers_get(
    int unit,
    uint32 *nof_deleted_buffers);


shr_error_e dnx_dram_buffers_quarantine_fifo_not_empty_interrupt_handler(
    int unit,
    bcm_core_t core);


shr_error_e dnx_dram_buffers_quarantine_ecc_error_interrupt_handler(
    int unit,
    uint32 hbc_block);


shr_error_e dnx_dram_buffers_quarantine_restore_deleted_buffers_after_soft_init(
    int unit);


#endif 
