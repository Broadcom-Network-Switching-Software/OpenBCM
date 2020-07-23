/*! \file bcmbd_m0ssq.c
 *
 * M0SSQ (ARM Cortex-M0 based Sub-System Quad) driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <shr/shr_error.h>
#include <shr/shr_debug.h>

#include <sal/sal_types.h>
#include <sal/sal_alloc.h>
#include <sal/sal_sleep.h>

#include <shr/shr_thread.h>

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_feature.h>

#include <bcmbd/bcmbd_cmicx_intr.h>
#include <bcmbd/bcmbd_m0ssq_internal.h>
#include <bcmbd/bcmbd_m0ssq_mbox_internal.h>

/*******************************************************************************
 * Local definition.
 */
#define DRV_CALL(_d, _f, _a) \
        ((_d) == NULL ? SHR_E_INIT: \
              ((_d)->_f == NULL ? SHR_E_UNAVAIL: (_d)->_f _a))

#define M0SSQ_INIT(_u) \
        DRV_CALL(bd_m0ssq_drv[_u], init, ((_u)))

#define M0SSQ_UC_START_SET(_u, _uc, _start) \
        DRV_CALL(bd_m0ssq_drv[_u], uc_start_set, \
                     ((_u), (_uc), (_start)))

#define M0SSQ_UC_START_GET(_u, _uc, _start) \
        DRV_CALL(bd_m0ssq_drv[_u], uc_start_get, \
                 ((_u), (_uc), (_start)))

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMBD_DEV

/* Macros for passing unit + uC numbers to the threads */
#define M0SSQ_THREAD_PRI               (50)
#define M0SSQ_ENCODE_UNIT_UC(unit, uc) ((void *) (uintptr_t) ((unit << 16) | uc))
#define M0SSQ_DECODE_UNIT(val)         ((int) (((uintptr_t) (val)) >> 16))
#define M0SSQ_DECODE_UC(val)           (((uintptr_t) (val)) & 0xffff)

/*******************************************************************************
 * Local variables.
 */
static const bcmbd_m0ssq_drv_t *bd_m0ssq_drv[BCMDRD_CONFIG_MAX_UNITS] = { NULL };
static bcmbd_m0ssq_dev_t *bd_m0ssq_dev[BCMDRD_CONFIG_MAX_UNITS] = { NULL };

/*******************************************************************************
 * Internal functions.
 */
static void
m0ssq_uc_swintr_thread(shr_thread_t tc, void *arg)
{
    int unit = M0SSQ_DECODE_UNIT(arg);
    uint32_t uc = M0SSQ_DECODE_UC(arg);
    bcmbd_m0ssq_dev_t *dev;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "M0SSQ uC %d interrupt thread start\n"), uc));

    if (!BCMDRD_UNIT_VALID(unit)) {
        return;
    }

    dev = bd_m0ssq_dev[unit];
    if (dev == NULL) {
        return;
    }

    while (1) {

        /* Wait for interrupt event/stop signal. */
        shr_thread_sleep(tc, SHR_THREAD_FOREVER);

        /* Check if system wants threads stop. */
        if (shr_thread_stopping(tc)) {
            return;
        }

        /* Check if uC software interrupt is disabled. */
        if (dev->uc_swintr_enable[uc] == 0) {
            dev->uc_swintr_pending[uc] = 1;
            continue;
        }

        /* Invoke interrupt handler. */
        dev->uc_swintr_handling[uc] = 1;
        if (dev->uc_swintr_handler[uc] != NULL) {
            dev->uc_swintr_handler[uc](unit, dev->uc_swintr_param[uc]);
        }
        dev->uc_swintr_handling[uc] = 0;
    }
}

static int
m0ssq_uc_swintr_init(int unit, uint32_t uc)
{
    shr_thread_t tc;
    bcmbd_m0ssq_dev_t *dev;
    char thread_name[64];

    SHR_FUNC_ENTER(unit);

    if (!BCMDRD_UNIT_VALID(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    dev = bd_m0ssq_dev[unit];
    SHR_NULL_CHECK(dev, SHR_E_INIT);

    /* Disable uC's software interrupt. */
    dev->uc_swintr_enable[uc] = 0;

    /* Clear "handling swintr" flag. */
    dev->uc_swintr_handling[uc] = 0;

    /*  Clear "swintr pending" flag. */
    dev->uc_swintr_pending[uc] = 0;

    sal_snprintf(thread_name, sizeof(thread_name),
                 "bcmbdM0ssqUc%dSwintr.%d", uc, unit);

    tc = shr_thread_start(thread_name,
                          M0SSQ_THREAD_PRI,
                          m0ssq_uc_swintr_thread,
                          M0SSQ_ENCODE_UNIT_UC(unit, uc));


    if (tc == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "M0SSQ thread %d creation failed \n"), uc));
        return SHR_E_MEMORY;
    }

    dev->uc_swintr_thread_ctrl[uc] = tc;

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */
void
bcmbd_m0ssq_uc_swintr_isr(int unit, uint32_t uc)
{
    bcmbd_m0ssq_dev_t *dev;

    dev = bd_m0ssq_dev[unit];
    if (dev == NULL) {
        return;
    }

    /* Increase software interrupt count as intr asserted. */
    if (dev->uc_swintr_enable[uc]) {
        shr_thread_wake(dev->uc_swintr_thread_ctrl[uc]);
    } else {
        dev->uc_swintr_pending[uc] = 1;
    }
}

int
bcmbd_m0ssq_drv_init(int unit, const bcmbd_m0ssq_drv_t *drv)
{
    SHR_FUNC_ENTER(unit);

    if (!BCMDRD_UNIT_VALID(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    if (!bcmdrd_feature_is_real_hw(unit)) {
        SHR_EXIT();
    }

    bd_m0ssq_drv[unit] = drv;

exit:
    SHR_FUNC_EXIT();
}

int
bcmbd_m0ssq_dev_get(int unit, bcmbd_m0ssq_dev_t **dev)
{
    SHR_FUNC_ENTER(unit);

    if (!BCMDRD_UNIT_VALID(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    *dev = bd_m0ssq_dev[unit];
    SHR_NULL_CHECK(*dev, SHR_E_UNAVAIL);

exit:
    SHR_FUNC_EXIT();
}

int
bcmbd_m0ssq_uc_num_get(int unit)
{
    bcmbd_m0ssq_dev_t *dev;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return 0;
    }

    dev = bd_m0ssq_dev[unit];
    if (dev) {
        return dev->num_of_uc;
    }

    return 0;
}

int
bcmbd_m0ssq_uc_swintr_handler_set(int unit, uint32_t uc,
                                  bcmbd_intr_f handler,
                                  uint32_t param)
{
    bcmbd_m0ssq_dev_t *dev;

    SHR_FUNC_ENTER(unit);

    if (!BCMDRD_UNIT_VALID(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    dev = bd_m0ssq_dev[unit];
    SHR_NULL_CHECK(dev, SHR_E_INIT);

    if (uc >= dev->num_of_uc) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    dev->uc_swintr_handler[uc] = handler;
    dev->uc_swintr_param[uc] = param;

exit:
    SHR_FUNC_EXIT();
}

int
bcmbd_m0ssq_uc_swintr_enable_set(int unit, uint32_t uc,
                                 bool enable)
{
    bcmbd_m0ssq_dev_t *dev;
    int timeout = 5;       /* 500ms. */
    int valid_cnt = 2;

    SHR_FUNC_ENTER(unit);

    if (!BCMDRD_UNIT_VALID(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    dev = bd_m0ssq_dev[unit];
    SHR_NULL_CHECK(dev, SHR_E_INIT);

    if (uc >= dev->num_of_uc) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    if (enable == 0 && dev->uc_swintr_enable[uc] == 1) {

        /* Stop swintr thread being wakeup */
        dev->uc_swintr_enable[uc] = 0;

        /* Wait for last swintr be handled */
        for (;timeout > 0; timeout--) {

             if (dev->uc_swintr_handling[uc] == 0) {
                 valid_cnt --;
             }

             if (valid_cnt <= 0) {
                 break;
             }

             /* Wait 100 ms. */
             sal_usleep(100000);
        }
    } else if ((enable == 1) && (dev->uc_swintr_enable[uc] == 0)) {

        /* If there are some pending swintrs, wakeup thread to
         * handle them.
         */
        if (dev->uc_swintr_pending[uc]) {

            /* Clear software interrupt pending Flag. */
            dev->uc_swintr_pending[uc] = 0;

            /* Wakeup thread to handled pending swintr. */
            shr_thread_wake(dev->uc_swintr_thread_ctrl[uc]);
        }
    }

    dev->uc_swintr_enable[uc] = enable;

exit:
    SHR_FUNC_EXIT();
}

int
bcmbd_m0ssq_init(int unit)
{
    uint32_t uc;
    bcmbd_m0ssq_dev_t *dev = NULL;

    SHR_FUNC_ENTER(unit);

    if (!BCMDRD_UNIT_VALID(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    if (bd_m0ssq_drv[unit] == NULL) {
        SHR_EXIT();
    }

    /* Allocate device-specific data. */
    dev = sal_alloc(sizeof(bcmbd_m0ssq_dev_t), "bcmbdM0ssqDev");
    SHR_NULL_CHECK(dev, SHR_E_MEMORY);

    /* Install device-specific data. */
    bd_m0ssq_dev[unit] = dev;

    /* Initialize chip specific part. */
    SHR_IF_ERR_EXIT(M0SSQ_INIT(unit));

    /* Initialize uC software interrupt. */
    for (uc = 0; uc < dev->num_of_uc; uc++) {
        SHR_IF_ERR_EXIT(m0ssq_uc_swintr_init(unit, uc));
    }

    /*
     * Initialize M0SSQ MBOX driver for 2 applications,
     * led and linkscan.
     */
    SHR_IF_ERR_EXIT(bcmbd_m0ssq_mbox_init(unit, &(dev->sram), 2));

exit:
    if (SHR_FUNC_ERR()) {

        /* Uninstall device-specific data. */
        if (BCMDRD_UNIT_VALID(unit)) {
            bd_m0ssq_dev[unit] = NULL;
        }
        SHR_FREE(dev);
    }
    SHR_FUNC_EXIT();
}

int
bcmbd_m0ssq_uc_fw_load(int unit, uint32_t uc, uint32_t offset,
                       const uint8_t *buf, uint32_t len)
{
    uint32_t wlen;
    uint32_t *ptr = NULL;
    uint32_t i;
    bcmbd_m0ssq_mem_t *mem;
    bcmbd_m0ssq_dev_t *dev = NULL;

    SHR_FUNC_ENTER(unit);

    if (!BCMDRD_UNIT_VALID(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    dev = bd_m0ssq_dev[unit];
    SHR_NULL_CHECK(dev, SHR_E_INIT);

    /* Make wlen as multiple of a 32 bits word. */
    wlen = (len + 3) & ~(0x3);

    /*
     * Allocate a temporary buffer to convert uint8_t input buffer
     * to uint32_t buffer for bottom driver use.
     */
    ptr = sal_alloc(wlen, "bcmbdM0ssqUcFwLoad");
    SHR_NULL_CHECK(ptr, SHR_E_MEMORY);

    /*
     * M0 is litte endian processor.
     * Assume that input buffer byte order is little endian.
     */
    for (i = 0; i < wlen; i += 4) {
         ptr[i/4] = ((uint32_t) buf[i]) |
                    ((uint32_t) buf[i + 1] << 8) |
                    ((uint32_t) buf[i + 2] << 16) |
                    ((uint32_t) buf[i + 3] << 24);
    }

    /* Get uC program memory object. */
    mem = &dev->m0_tcm[uc];

    /* Write firmware binary into program memory. */
    SHR_IF_ERR_EXIT
        (mem->write32(mem, offset, ptr, wlen));

exit:
    SHR_FREE(ptr);
    SHR_FUNC_EXIT();
}

int
bcmbd_m0ssq_uc_start(int unit, uint32_t uc)
{
    bcmbd_m0ssq_mem_t *mem;
    bcmbd_m0ssq_dev_t *dev;
    uint32_t chksum;

    SHR_FUNC_ENTER(unit);

    if (!BCMDRD_UNIT_VALID(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    dev = bd_m0ssq_dev[unit];
    SHR_NULL_CHECK(dev, SHR_E_INIT);

    if (uc >= dev->num_of_uc) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    /* Calculate checksum of firmware config. */
    SHR_IF_ERR_EXIT
        (bcmbd_m0ssq_fwconfig_chksum(dev->fwconfig[uc], &chksum));
    dev->fwconfig[uc].chksum = chksum;

    /* Fill out the content of firmware config memory. */
    mem = &dev->fwconfig_mem[uc];
    SHR_IF_ERR_EXIT
        (mem->write32(mem, 0, (uint32_t *)&dev->fwconfig[uc],
                      sizeof(bcmbd_fwconfig_t)));

    /* Let uc start running. */
    SHR_IF_ERR_EXIT
        (M0SSQ_UC_START_SET(unit, uc, 1));

exit:
    SHR_FUNC_EXIT();
}

int
bcmbd_m0ssq_uc_stop(int unit, uint32_t uc)
{
    bcmbd_m0ssq_dev_t *dev;

    SHR_FUNC_ENTER(unit);

    if (!BCMDRD_UNIT_VALID(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    dev = bd_m0ssq_dev[unit];
    SHR_NULL_CHECK(dev, SHR_E_INIT);

    if (uc >= dev->num_of_uc) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    /* Let uc stop running. */
    SHR_IF_ERR_EXIT(M0SSQ_UC_START_SET(unit, uc, 0));

exit:
    SHR_FUNC_EXIT();
}

int
bcmbd_m0ssq_uc_start_get(int unit, uint32_t uc, bool *start)
{
    SHR_FUNC_ENTER(unit);

    if (!BCMDRD_UNIT_VALID(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    SHR_IF_ERR_EXIT(M0SSQ_UC_START_GET(unit, uc, start));

exit:
    SHR_FUNC_EXIT();
}

int
bcmbd_m0ssq_cleanup(int unit)
{
    bcmbd_m0ssq_dev_t *dev = NULL;
    uint32_t uc;

    SHR_FUNC_ENTER(unit);

    if (!BCMDRD_UNIT_VALID(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    if (bd_m0ssq_drv[unit] == NULL) {
        SHR_EXIT();
    }

    dev = bd_m0ssq_dev[unit];
    SHR_NULL_CHECK(dev, SHR_E_NONE);

    /* Cleanup MBOX driver. */
    SHR_IF_ERR_CONT
        (bcmbd_m0ssq_mbox_cleanup(unit));

    /* Stop all swintr threads. */
    for (uc = 0; uc < dev->num_of_uc; uc++) {
        SHR_IF_ERR_CONT
            (shr_thread_stop(dev->uc_swintr_thread_ctrl[uc], 50000));
    }

    /* Cleanup M0 driver. */
    bd_m0ssq_dev[unit] = NULL;

exit:
    /* Free device-specific data. */
    SHR_FREE(dev);
    SHR_FUNC_EXIT();
}

int
bcmbd_m0ssq_iproc_mem_read32(bcmbd_m0ssq_mem_t *mem,
                             uint32_t offset,
                             uint32_t *data, uint32_t len)
{
    uint32_t addr;
    uint32_t i;
    int unit, rv;

    /* Parameter check. */
    if (mem == NULL) {
        return SHR_E_PARAM;
    }

    unit = mem->unit;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    if (((offset + len) > mem->size) ||
        (offset % 4) ||
        (len % 4))
    {
        return SHR_E_PARAM;
    }

    /* Read data from memory block. */
    addr = mem->base + offset;

    for (i = 0; i < len;  i += 4) {
        rv = bcmdrd_hal_iproc_read(unit, addr, &data[i/4]);
        if (SHR_FAILURE(rv)) {
             return rv;
        }
        addr += 4;
    }

    return SHR_E_NONE;
}

int
bcmbd_m0ssq_iproc_mem_write32(bcmbd_m0ssq_mem_t *mem,
                              uint32_t offset,
                              const uint32_t *data, uint32_t len)
{
    uint32_t addr;
    uint32_t i;
    int unit, rv;

    /* Parameter check. */
    if (mem == NULL) {
        return SHR_E_PARAM;
    }

    unit = mem->unit;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    if (((offset + len) > mem->size) ||
        (offset % 4) ||
        (len % 4))
    {
        return SHR_E_PARAM;
    }

    /* Write data into memory block. */
    addr = mem->base + offset;

    for (i = 0; i < len;  i += 4) {
         rv = bcmdrd_hal_iproc_write(unit, addr, data[i/4]);
         if (SHR_FAILURE(rv)) {
             return rv;
         }
         addr += 4;
    }

    return SHR_E_NONE;
}

int
bcmbd_m0ssq_mem_clear(bcmbd_m0ssq_mem_t *mem)
{
    uint32_t zero32 = 0;
    uint32_t offset;
    int rv;

    /* Parameter check. */
    if (mem == NULL) {
        return SHR_E_PARAM;
    }

    if (!BCMDRD_UNIT_VALID(mem->unit)) {
        return SHR_E_UNIT;
    }

    if (mem->write32 == NULL) {
        return SHR_E_INIT;
    }

    for (offset = 0; offset < mem->size; offset += 4) {
         rv = mem->write32(mem, offset, &zero32, 4);
         if (SHR_FAILURE(rv)) {
             return rv;
         }
    }

    return SHR_E_NONE;
}

int
bcmbd_m0ssq_fwconfig_chksum(bcmbd_fwconfig_t fwcfg, uint32_t *pchksum)
{
    int chksum = 0;
    uint32_t *ptr = (uint32_t *) &fwcfg;
    uint32_t i;

    if (pchksum == NULL) {
        return SHR_E_PARAM;
    }

    for (i = 0;
         i < offsetof(bcmbd_fwconfig_t, chksum); i += 4)
    {
        chksum += ptr[i / 4];
    }

    *pchksum = chksum;

    return SHR_E_NONE;
}
