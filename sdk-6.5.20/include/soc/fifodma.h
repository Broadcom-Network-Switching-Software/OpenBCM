/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        fifodma.h
 * Purpose:     Maps out structures used for FIFO DMA operations and
 *              exports routines.
 */

#ifndef _SOC_FIFODMA_H
#define _SOC_FIFODMA_H

#include <soc/defs.h>
#include <soc/types.h>
#include <sal/types.h>
#include <sal/core/time.h>
#include <sal/core/sync.h>
#include <sal/core/thread.h>

/*******************************************
* @function soc_l2mod_sbus_fifo_field_set
* obtain l2 mod sbus field set
*
* @param unit   [in] unit #
* @param field  [in] field
* @param enable [in] enable
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
 */

extern int
soc_l2mod_sbus_fifo_field_set(int unit, soc_field_t field, int enable);

/*******************************************
* @function soc_l2mod_sbus_fifo_field_get
* obtain l2 mod sbus field get
*
* @param unit   [in] unit #
* @param field  [in] field
* @param enable [in] enable
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
 */

extern int
soc_l2mod_sbus_fifo_field_get(int unit, soc_field_t field, int *enable);

/*******************************************
* @function soc_fifodma_stop
* purpose to stop fifo dma channel
*
* @param unit [in] unit #
* @param chan [in] channel number
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
 */
extern int
soc_fifodma_stop(int unit, int chan);

/*******************************************
* @function soc_fifodma_start
* purpose to start fifo dma channel
*
* @param unit [in] unit #
* @param ch [in] channel number
*
* @returns SOC_E_PARAM
* @returns SOC_E_NONE
*
* @end
 */
extern int
soc_fifodma_start(int unit, int chan, int is_mem,
                  soc_mem_t mem, soc_reg_t reg,
                  int copyno, int force_entry_size,
                  int host_entries, void *host_buf);

/*******************************************
* @function soc_fifodma_intr_enable
* purpose to enable interrupt
*
* @param unit [in] unit #
* @param ch [in] channel
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
 */
extern int
soc_fifodma_intr_enable(int unit, int ch);

/*******************************************
* @function soc_fifodma_intr_disable
* purpose to enable interrupt
*
* @param unit [in] unit #
* @param ch [in] channel
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
 */
extern int
soc_fifodma_intr_disable(int unit, int ch);

/*******************************************
* @function soc_fifodma_get_read_ptr
* purpose to get read ptr
*
* @param unit [in] unit #
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
 */
extern int
soc_fifodma_get_read_ptr(int unit, int chan, void **host_ptr, int *count);

/*******************************************
* @function soc_fifodma_advance_read_ptr
* purpose to advance read ptr
*
* @param unit [in] unit #
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
 */
extern int
soc_fifodma_advance_read_ptr(int unit, int chan, int count);

/*******************************************
* @function soc_fifodma_set_entries_read
* purpose to set entries read
*
* @param unit [in] unit #
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
 */
extern int
soc_fifodma_set_entries_read(int unit, int chan, uint32 num);

/*******************************************
* @function soc_fifodma_num_entries_get
* purpose to get number of entries
*
* @param unit [in] unit #
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
 */
extern int
soc_fifodma_num_entries_get(int unit, int chan, int *count);

/*******************************************
* @function soc_fifodma_status_get
* purpose to obtain fifo dma channel status
*
* @param unit [in] unit #
* @param chan [in] channel number
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
 */
extern int
soc_fifodma_status_get(int unit, int chan, uint32 *status);

/*******************************************
* @function soc_fifodma_ch_endian_set
* purpose to set fifo dma channel endianness
*
* @param unit [in] unit #
* @param chan [in] channel number
* @param ch [in] big_endian
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
 */
extern int
soc_fifodma_ch_endian_set(int unit, int channel, int be);

/*******************************************
* @function soc_fifodma_status_clear
* purpose to clear fifo dma channel status
*
* @param unit [in] unit #
* @param chan [in] channel number
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
 */
extern int
soc_fifodma_status_clear(int unit, int chan);

/*******************************************
* @function soc_fifodma_masks_get
* purpose to obtain fifodma masks
*
* @param unit [in] unit #
* @param chan [in] channel number
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
 */
extern int
soc_fifodma_masks_get(int unit, uint32 *hostmem_timeout, uint32 *hostmem_overflow, uint32 *error, uint32 *done);

extern int
soc_fifodma_init(int unit);

extern int
soc_fifodma_deinit(int unit);

#endif /* !_SOC_FIFODMA_H */
