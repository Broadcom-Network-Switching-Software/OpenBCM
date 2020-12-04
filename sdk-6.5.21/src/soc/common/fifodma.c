/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose: SOC FIFO DMA driver.
 */

#include <shared/bsl.h>

#include <sal/core/boot.h>
#include <sal/core/libc.h>
#include <shared/alloc.h>

#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/cm.h>
#include <soc/fifodma_internal.h>

STATIC soc_fifodma_drv_t _fifodma_drv[SOC_MAX_NUM_DEVICES];

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
soc_fifodma_stop(int unit, int chan)
{
    int rv = SOC_E_UNAVAIL;

    if (_fifodma_drv[unit].fifodma_stop != NULL) {
        rv = _fifodma_drv[unit].fifodma_stop(unit, chan);
    }

    return rv;
}

/*******************************************
* @function soc_fifodma_start
* purpose to start fifo dma channel
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
soc_fifodma_start(int unit, int chan, int is_mem,
                  soc_mem_t mem, soc_reg_t reg,
                  int copyno, int force_entry_size,
                  int host_entries, void *host_buf)
{
    int rv = SOC_E_UNAVAIL;

    if (_fifodma_drv[unit].fifodma_start != NULL) {
        rv = _fifodma_drv[unit].fifodma_start(unit, chan,
                                              is_mem, mem, reg,
                                              copyno, force_entry_size,
                                              host_entries, host_buf);
    }

    return rv;
}

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
soc_fifodma_intr_enable(int unit, int ch)
{
    int rv = SOC_E_UNAVAIL;

    if (_fifodma_drv[unit].fifodma_intr_enable != NULL) {
        rv = _fifodma_drv[unit].fifodma_intr_enable(unit, ch);
    }

    return rv;
}

/*******************************************
* @function soc_fifodma_intr_disable
* purpose to disable interrupt
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
soc_fifodma_intr_disable(int unit, int ch)
{
    int rv = SOC_E_UNAVAIL;

    if (_fifodma_drv[unit].fifodma_intr_disable != NULL) {
        rv = _fifodma_drv[unit].fifodma_intr_disable(unit, ch);
    }

    return rv;
}

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
soc_fifodma_get_read_ptr(int unit, int chan, void **host_ptr, int *count)
{
    int rv = SOC_E_UNAVAIL;

    if (_fifodma_drv[unit].fifodma_get_read_ptr != NULL) {
        rv = _fifodma_drv[unit].fifodma_get_read_ptr(unit, chan,
                                                     host_ptr, count);
    }

    return rv;
}

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
soc_fifodma_advance_read_ptr(int unit, int chan, int count)
{
    int rv = SOC_E_UNAVAIL;

    if (_fifodma_drv[unit].fifodma_advance_read_ptr != NULL) {
        rv = _fifodma_drv[unit].fifodma_advance_read_ptr(unit, chan,
                                                         count);
    }

    return rv;
}

/*******************************************
* @function soc_fifodma_set_entries_read
* purpose to read set entries
*
* @param unit [in] unit #
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
 */
extern int
soc_fifodma_set_entries_read(int unit, int chan, uint32 num)
{
    int rv = SOC_E_UNAVAIL;

    if (_fifodma_drv[unit].fifodma_set_entries_read != NULL) {
        rv = _fifodma_drv[unit].fifodma_set_entries_read(unit, chan, num);
    }

    return rv;
}

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
soc_fifodma_num_entries_get(int unit, int chan, int *count)
{
    int rv = SOC_E_UNAVAIL;

    if (_fifodma_drv[unit].fifodma_num_entries_get != NULL) {
        rv = _fifodma_drv[unit].fifodma_num_entries_get(unit, chan, count);
    }

    return rv;
}

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
soc_fifodma_status_clear(int unit, int chan)
{
    int rv = SOC_E_UNAVAIL;

    if (_fifodma_drv[unit].fifodma_status_clear != NULL) {
        rv = _fifodma_drv[unit].fifodma_status_clear(unit, chan);
    }

    return rv;
}

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
soc_fifodma_status_get(int unit, int chan, uint32 *status)
{
    int rv = SOC_E_UNAVAIL;

    if (_fifodma_drv[unit].fifodma_status_get != NULL) {
        rv = _fifodma_drv[unit].fifodma_status_get(unit, chan, status);
    }

    return rv;
}

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
int
soc_fifodma_ch_endian_set(int unit, int channel, int be)
{
#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx)) {
        return cmicx_fifodma_ch_endian_set(unit, channel, be);
    }
#endif
    return SOC_E_NONE;
}
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
soc_fifodma_masks_get(int unit, uint32 *hostmem_timeout, uint32 *hostmem_overflow, uint32 *error, uint32 *done)
{
    int rv = SOC_E_UNAVAIL;

    if (_fifodma_drv[unit].fifodma_masks_get != NULL) {
        rv = _fifodma_drv[unit].fifodma_masks_get(unit, hostmem_timeout, hostmem_overflow,
                                                  error, done);
    }

    return rv;
}

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
soc_l2mod_sbus_fifo_field_set(int unit, soc_field_t field, int enable)
{
    int rv = SOC_E_UNAVAIL;

    if (_fifodma_drv[unit].l2mod_sbus_fifo_field_set != NULL) {
        rv = _fifodma_drv[unit].l2mod_sbus_fifo_field_set(unit, field, enable);
    }

    return rv;
}

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
soc_l2mod_sbus_fifo_field_get(int unit, soc_field_t field, int *enable)
{
    int rv = SOC_E_UNAVAIL;

    if (_fifodma_drv[unit].l2mod_sbus_fifo_field_get != NULL) {
        rv = _fifodma_drv[unit].l2mod_sbus_fifo_field_get(unit, field, enable);
    }

    return rv;
}

/*******************************************
* @function soc_fifodma_init
* purpose API to Initialize CCM DMA
*
* @param unit [in] unit
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
int soc_fifodma_init(int unit)
{
    LOG_VERBOSE(BSL_LS_SOC_FIFODMA,
                      (BSL_META_U(unit,
                                  "fifo dma init\n")));
#ifdef BCM_CMICM_SUPPORT
    if (soc_feature(unit, soc_feature_cmicm)) {
        SOC_IF_ERROR_RETURN(cmicm_fifodma_init(unit,
                            &_fifodma_drv[unit]));
    }
#endif
#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx)) {
        SOC_IF_ERROR_RETURN(cmicx_fifodma_init(unit,
                            &_fifodma_drv[unit]));
    }
#endif
    return SOC_E_NONE;
}

/*******************************************
* @function soc_fifodma_deinit
* purpose API to Deinitialize CCM DMA
*
* @param unit [in] unit
*
* @returns SOC_E_NONE
*
* @end
 */
int soc_fifodma_deinit(int unit)
{
    LOG_VERBOSE(BSL_LS_SOC_FIFODMA,
                      (BSL_META_U(unit,
                                  "fifo dma deinit\n")));
#ifdef BCM_CMICM_SUPPORT
    if (soc_feature(unit, soc_feature_cmicm)) {

        SOC_IF_ERROR_RETURN(cmicm_fifodma_deinit(unit,
                            &_fifodma_drv[unit]));
    }
#endif
#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx)) {

        SOC_IF_ERROR_RETURN(cmicx_fifodma_deinit(unit,
                            &_fifodma_drv[unit]));
    }
#endif
    return SOC_E_NONE;
}
