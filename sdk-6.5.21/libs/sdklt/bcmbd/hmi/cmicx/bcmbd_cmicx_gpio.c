/*! \file bcmbd_cmicx_gpio.c
 *
 * CMICx GPIO operations
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <shr/shr_timeout.h>

#include <sal/sal_mutex.h>

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmbd/bcmbd_cmicx_acc.h>

#include <bcmbd/bcmbd_cmicx_gpio.h>

/*******************************************************************************
 * Local definitions
 */

#define GPIO_LOCK(_u)   sal_mutex_take(gpio_lock[_u], SAL_MUTEX_FOREVER)
#define GPIO_UNLOCK(_u) sal_mutex_give(gpio_lock[_u])


/*******************************************************************************
 * Local data
 */

static sal_mutex_t gpio_lock[BCMDRD_CONFIG_MAX_UNITS];


/*******************************************************************************
 * Private functions
 */

static inline void
cmicx_bit_set(uint32_t *val, int offset, bool set)
{
    if (set) {
        *val |= 1 << offset;
    } else {
        *val &= ~(1 << offset);
    }
}

static inline bool
cmicx_bit_get(uint32_t val, int offset)
{
    return (val & (1 << offset)) > 0;
}

static int
cmicx_gpio_init(int unit)
{
    gpio_lock[unit] = sal_mutex_create("bcmbdCmicxGpioLock");
    if (!gpio_lock[unit]) {
        return SHR_E_MEMORY;
    }

    return SHR_E_NONE;
}

static int
cmicx_gpio_cleanup(int unit)
{
    if (gpio_lock[unit]) {
        sal_mutex_destroy(gpio_lock[unit]);
        gpio_lock[unit] = NULL;
    }

    return SHR_E_NONE;
}

static int
cmicx_gpio_mode_set(int unit, uint32_t gpio, bcmbd_gpio_mode_t mode)
{
    int ioerr = 0;
    uint32_t en;
    GPIO_OUT_ENr_t out_en;

    /* Sanity checks */
    if (gpio >= BCMBD_CMICX_NUM_GPIO) {
        return SHR_E_PARAM;
    }

    GPIO_LOCK(unit);

    ioerr += READ_GPIO_OUT_ENr(unit, &out_en);
    en = GPIO_OUT_ENr_OUT_ENABLEf_GET(out_en);
    if (mode == BCMBD_GPIO_MODE_OUT) {
        cmicx_bit_set(&en, gpio, true);
    } else {
        cmicx_bit_set(&en, gpio, false);
    }
    GPIO_OUT_ENr_OUT_ENABLEf_SET(out_en, en);
    ioerr += WRITE_GPIO_OUT_ENr(unit, out_en);

    GPIO_UNLOCK(unit);

    return (ioerr == 0) ? SHR_E_NONE : SHR_E_IO;
}

static int
cmicx_gpio_mode_get(int unit, uint32_t gpio, bcmbd_gpio_mode_t *mode)
{
    int ioerr = 0;
    uint32_t en;
    GPIO_OUT_ENr_t out_en;

    /* Sanity checks */
    if (gpio >= BCMBD_CMICX_NUM_GPIO) {
        return SHR_E_PARAM;
    }

    ioerr += READ_GPIO_OUT_ENr(unit, &out_en);
    en = GPIO_OUT_ENr_OUT_ENABLEf_GET(out_en);
    if (cmicx_bit_get(en, gpio)) {
        *mode = BCMBD_GPIO_MODE_OUT;
    } else {
        *mode = BCMBD_GPIO_MODE_IN;
    }

    return (ioerr == 0) ? SHR_E_NONE : SHR_E_IO;
}

static int
cmicx_gpio_set(int unit, uint32_t gpio, bool val)
{
    int ioerr = 0;
    uint32_t data;
    GPIO_DATA_OUTr_t data_out;

    /* Sanity checks */
    if (gpio >= BCMBD_CMICX_NUM_GPIO) {
        return SHR_E_PARAM;
    }

    GPIO_LOCK(unit);

    ioerr += READ_GPIO_DATA_OUTr(unit, &data_out);
    data = GPIO_DATA_OUTr_DATA_OUTf_GET(data_out);
    cmicx_bit_set(&data, gpio, val);
    GPIO_DATA_OUTr_DATA_OUTf_SET(data_out, data);
    ioerr += WRITE_GPIO_DATA_OUTr(unit, data_out);

    GPIO_UNLOCK(unit);

    return (ioerr == 0) ? SHR_E_NONE : SHR_E_IO;
}

static int
cmicx_gpio_get(int unit, uint32_t gpio, bool *val)
{
    int ioerr = 0;
    uint32_t data;
    GPIO_DATA_INr_t data_in;

    /* Sanity checks */
    if (gpio >= BCMBD_CMICX_NUM_GPIO) {
        return SHR_E_PARAM;
    }

    ioerr += READ_GPIO_DATA_INr(unit, &data_in);
    data = GPIO_DATA_INr_DATA_INf_GET(data_in);
    *val = cmicx_bit_get(data, gpio);

    return (ioerr == 0) ? SHR_E_NONE : SHR_E_IO;
}


/*******************************************************************************
 * Driver object
 */

static bcmbd_gpio_drv_t cmicx_gpio_drv = {
    .init = cmicx_gpio_init,
    .cleanup = cmicx_gpio_cleanup,
    .mode_set = cmicx_gpio_mode_set,
    .mode_get = cmicx_gpio_mode_get,
    .set = cmicx_gpio_set,
    .get = cmicx_gpio_get
};


/*******************************************************************************
 * Public functions
 */

int
bcmbd_cmicx_gpio_drv_init(int unit)
{
    return bcmbd_gpio_drv_init(unit, &cmicx_gpio_drv);
}

int
bcmbd_cmicx_gpio_drv_cleanup(int unit)
{
    return bcmbd_gpio_drv_init(unit, NULL);
}
