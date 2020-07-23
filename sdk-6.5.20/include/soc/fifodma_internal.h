/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        fifodma_internal.h
 * Purpose:     Maps out structures used for FIFO DMA operations and
 *              internal routines.
 */

#ifndef _SOC_FIFODMA_INTERNAL_H
#define _SOC_FIFODMA_INTERNAL_H

#include <soc/defs.h>
#include <soc/types.h>
#include <sal/types.h>
#include <sal/core/time.h>
#include <sal/core/sync.h>
#include <sal/core/thread.h>
#include <soc/fifodma.h>

/* FIFO DMA function pointers */
typedef struct soc_fifodma_drv_s {
    int (*fifodma_stop)(int unit, int chan);
    int (*fifodma_start)(int unit, int chan, int is_mem,
                             soc_mem_t mem, soc_reg_t reg,
                             int copyno, int force_entry_size,
                             int host_entries, void *host_buf);
    int (*fifodma_intr_enable)(int unit, int ch);
    int (*fifodma_intr_disable)(int unit, int ch);
    int (*fifodma_get_read_ptr)(int unit, int chan, void **host_ptr, int *count);
    int (*fifodma_advance_read_ptr)(int unit, int chan, int count);
    int (*fifodma_num_entries_get)(int unit, int ch, int *count);
    int (*fifodma_set_entries_read)(int unit, int ch, uint32 num);
    int (*fifodma_status_clear)(int unit, int chan);
    int (*fifodma_status_get)(int unit, int chan, uint32 *status);
    int (*fifodma_masks_get)(int unit, uint32 *hostmem_timeout,
                             uint32 *hostmem_overflow, uint32 *error, uint32 *done);
    int (*l2mod_sbus_fifo_field_set)(int unit, soc_field_t field, int enable);
    int (*l2mod_sbus_fifo_field_get)(int unit, soc_field_t field, int *enable);
}soc_fifodma_drv_t;

/*******************************************
* @function cmicm_fifodma_init
* purpose API to Initialize cmicm FIFO DMA
*
* @param unit [in] unit
* @param drv [out] soc_fifodma_drv_t pointer
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
extern int cmicm_fifodma_init(int unit, soc_fifodma_drv_t *drv);

/*******************************************
* @function cmicm_fifodma_deinit
* purpose API to Deinitialize cmicm FIFO DMA
*
* @param unit [in] unit
* @param drv [in] soc_fifodma_drv_t pointer
*
* @returns SOC_E_NONE

*
* @end
 */
extern int cmicm_fifodma_deinit(int unit, soc_fifodma_drv_t *drv);

/*******************************************
* @function cmicx_fifodma_ch_endian_set
* purpose to set CMICX fifo dma channel endianness
*
* @param unit [in] unit #
* @param chan [in] channel number
* @param ch [in] big_endian
*
* @returns SOC_E_NONE
*
* @end
 */
extern int
cmicx_fifodma_ch_endian_set(int unit, int channel, int be);

/*******************************************
* @function cmicx_fifodma_init
* purpose API to Initialize cmicx FIFO DMA
*
* @param unit [in] unit
* @param drv [out] soc_fifodma_drv_t pointer
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
extern int cmicx_fifodma_init(int unit, soc_fifodma_drv_t *drv);

/*******************************************
* @function cmicx_fifodma_deinit
* purpose API to Deinitialize cmicx FIFO DMA
*
* @param unit [in] unit
* @param drv [in] soc_fifodma_drv_t pointer
*
* @returns SOC_E_NONE

*
* @end
 */
extern int cmicx_fifodma_deinit(int unit, soc_fifodma_drv_t *drv);

#endif /* !_SOC_FIFODMA_INTERNAL_H */
