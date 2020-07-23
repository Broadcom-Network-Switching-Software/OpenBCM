/*! \file bcmbd_pvt_intr.c
 *
 * PVTMON interrupt driver API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>

#include <bcmdrd/bcmdrd_feature.h>
#include <bcmdrd/bcmdrd_chip.h>

#include <bcmbd/bcmbd_pvt_intr_internal.h>

/*******************************************************************************
* Local data
 */

static bcmbd_intr_drv_t *pvt_intr_drv[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
* Public functions
 */

int
bcmbd_pvt_intr_drv_init(int unit, bcmbd_intr_drv_t *intr_drv)
{
    bool emul;

    emul = bcmdrd_feature_enabled(unit, BCMDRD_FT_EMUL);
    if (emul) {
        return SHR_E_NONE;
    }

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    pvt_intr_drv[unit] = intr_drv;

    return SHR_E_NONE;
}

int
bcmbd_pvt_intr_func_set(int unit, unsigned int intr_num,
                        bcmbd_intr_f intr_func, uint32_t intr_param)
{
    bcmbd_intr_drv_t *idrv = pvt_intr_drv[unit];
    bool emul;

    emul = bcmdrd_feature_enabled(unit, BCMDRD_FT_EMUL);
    if (emul) {
        return SHR_E_NONE;
    }

    if (idrv == NULL) {
        return SHR_E_UNIT;
    }

    if (idrv->intr_func_set == NULL) {
        return SHR_E_UNAVAIL;
    }

    idrv->intr_func_set(unit, intr_num, intr_func, intr_param);

    return SHR_E_NONE;
}

int
bcmbd_pvt_intr_enable(int unit, unsigned int intr_num)
{
    bcmbd_intr_drv_t *idrv = pvt_intr_drv[unit];
    bool emul;

    emul = bcmdrd_feature_enabled(unit, BCMDRD_FT_EMUL);
    if (emul) {
        return SHR_E_NONE;
    }

    if (idrv == NULL) {
        return SHR_E_UNIT;
    }

    if (idrv->intr_enable == NULL) {
        return SHR_E_UNAVAIL;
    }

    idrv->intr_enable(unit, intr_num);

    return SHR_E_NONE;
}

int
bcmbd_pvt_intr_disable(int unit, unsigned int intr_num)
{
    bcmbd_intr_drv_t *idrv = pvt_intr_drv[unit];
    bool emul;

    emul = bcmdrd_feature_enabled(unit, BCMDRD_FT_EMUL);
    if (emul) {
        return SHR_E_NONE;
    }

    if (idrv == NULL) {
        return SHR_E_UNIT;
    }

    if (idrv->intr_disable == NULL) {
        return SHR_E_UNAVAIL;
    }

    idrv->intr_disable(unit, intr_num);

    return SHR_E_NONE;
}

int
bcmbd_pvt_intr_clear(int unit, unsigned int intr_num)
{
    bcmbd_intr_drv_t *idrv = pvt_intr_drv[unit];
    bool emul;

    emul = bcmdrd_feature_enabled(unit, BCMDRD_FT_EMUL);
    if (emul) {
        return SHR_E_NONE;
    }

    if (idrv == NULL) {
        return SHR_E_UNIT;
    }

    if (idrv->intr_clear == NULL) {
        /* Do not return error if not implemented/required */
        return SHR_E_NONE;
    }

    idrv->intr_clear(unit, intr_num);

    return SHR_E_NONE;
}

int
bcmbd_pvt_intr_start(int unit, unsigned int intr_num,
                     bcmbd_intr_f intr_func, uint32_t intr_param)
{
    bcmbd_intr_drv_t *idrv = pvt_intr_drv[unit];
    bool emul;

    emul = bcmdrd_feature_enabled(unit, BCMDRD_FT_EMUL);
    if (emul) {
        return SHR_E_NONE;
    }

    if (idrv == NULL) {
        return SHR_E_UNIT;
    }

    if (idrv->intr_func_set == NULL) {
        return SHR_E_UNAVAIL;
    }

    idrv->intr_func_set(unit, intr_num, intr_func, intr_param);
    idrv->intr_enable(unit, intr_num);

    return SHR_E_NONE;
}

int
bcmbd_pvt_intr_stop(int unit, unsigned int intr_num)
{
    bcmbd_intr_drv_t *idrv = pvt_intr_drv[unit];
    bool emul;

    emul = bcmdrd_feature_enabled(unit, BCMDRD_FT_EMUL);
    if (emul) {
        return SHR_E_NONE;
    }

    if (idrv == NULL) {
        return SHR_E_UNIT;
    }

    if (idrv->intr_func_set == NULL) {
        return SHR_E_UNAVAIL;
    }

    idrv->intr_disable(unit, intr_num);

    return SHR_E_NONE;
}


