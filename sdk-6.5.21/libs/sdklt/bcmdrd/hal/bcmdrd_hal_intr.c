/*! \file bcmdrd_hal_intr.c
 *
 * Hardware abstraction APIs for interrupt handling.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <sal/sal_assert.h>

#include <shr/shr_error.h>

#include <bcmdrd/bcmdrd_feature.h>
#include <bcmdrd/bcmdrd_internal.h>
#include <bcmdrd/bcmdrd_dev.h>

int
bcmdrd_hal_intr_connect(int unit, int irq_num,
                        bcmdrd_hal_isr_func_f isr_func, void *isr_data)
{
    bcmdrd_dev_t *dev = bcmdrd_dev_get(unit);
    bcmdrd_hal_intr_t *intr;

    assert(dev);

    intr = &dev->intr;
    if (intr->intr_connect) {
        return intr->intr_connect(intr->devh, irq_num, isr_func, isr_data);
    }

    if (bcmdrd_feature_enabled(unit, BCMDRD_FT_PASSIVE_SIM)) {
        return SHR_E_NONE;
    }

    return SHR_E_INIT;
}

int
bcmdrd_hal_intr_disconnect(int unit, int irq_num)
{
    bcmdrd_dev_t *dev = bcmdrd_dev_get(unit);
    bcmdrd_hal_intr_t *intr;

    assert(dev);

    intr = &dev->intr;
    if (intr->intr_disconnect) {
        return intr->intr_disconnect(intr->devh, irq_num);
    }

    if (bcmdrd_feature_enabled(unit, BCMDRD_FT_PASSIVE_SIM)) {
        return SHR_E_NONE;
    }

    return SHR_E_INIT;
}

int
bcmdrd_hal_intr_sync(int unit, int irq_num,
                     bcmdrd_intr_sync_cb_f cb, void *data)
{
    bcmdrd_dev_t *dev = bcmdrd_dev_get(unit);
    bcmdrd_hal_intr_t *intr;

    assert(dev);
    assert(cb);

    intr = &dev->intr;
    if (intr->intr_sync) {
        return intr->intr_sync(intr->devh, irq_num, cb, data);
    }

    /* If no interrupt HAL is found, then we just call directly */
    return cb(data);
}

int
bcmdrd_hal_intr_configure(int unit, int irq_num,
                          bcmdrd_hal_intr_info_t *intr_info)
{
    bcmdrd_dev_t *dev = bcmdrd_dev_get(unit);
    bcmdrd_hal_intr_t *intr;

    assert(dev);

    intr = &dev->intr;
    if (intr->intr_configure) {
        return intr->intr_configure(intr->devh, irq_num, intr_info);
    }

    if (bcmdrd_feature_enabled(unit, BCMDRD_FT_PASSIVE_SIM)) {
        return SHR_E_NONE;
    }

    return SHR_E_INIT;
}

int
bcmdrd_hal_intr_mask_write(int unit, int irq_num,
                           uint32_t offs, uint32_t val)
{
    bcmdrd_dev_t *dev = bcmdrd_dev_get(unit);
    bcmdrd_hal_intr_t *intr;

    assert(dev);

    intr = &dev->intr;
    if (intr->intr_mask_write) {
        return intr->intr_mask_write(intr->devh, irq_num, offs, val);
    }

    if (bcmdrd_feature_enabled(unit, BCMDRD_FT_PASSIVE_SIM)) {
        return SHR_E_NONE;
    }

    return SHR_E_INIT;
}
