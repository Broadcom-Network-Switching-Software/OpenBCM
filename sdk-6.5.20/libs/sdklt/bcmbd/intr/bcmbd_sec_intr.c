/*! \file bcmbd_sec_intr.c
 *
 * SEC interrupt driver API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_chip.h>
#include <bcmbd/bcmbd_sec_intr_internal.h>

/*******************************************************************************
* Local data
 */

static bcmbd_intr_drv_t *sec_intr_drv[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
* Public functions
 */

int
bcmbd_sec_intr_drv_init(int unit, bcmbd_intr_drv_t *intr_drv)
{
    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    sec_intr_drv[unit] = intr_drv;

    return SHR_E_NONE;
}

int
bcmbd_sec_intr_func_set(int unit, unsigned int intr_num,
                        bcmbd_intr_f intr_func, uint32_t intr_param)
{
    bcmbd_intr_drv_t *md = sec_intr_drv[unit];

    if (md == NULL) {
        return SHR_E_UNIT;
    }

    if (intr_param != SEC_INTR_PARAM_GET(intr_param)) {
        /* Make sure value can be encoded for callback */
        return SHR_E_PARAM;
    }

    if (md->intr_func_set == NULL) {
        return SHR_E_UNAVAIL;
    }

    md->intr_func_set(unit, intr_num, intr_func, intr_param);

    return SHR_E_NONE;
}

int
bcmbd_sec_intr_enable(int unit, unsigned int intr_num)
{
    bcmbd_intr_drv_t *md = sec_intr_drv[unit];

    if (md == NULL) {
        return SHR_E_UNIT;
    }

    if (md->intr_enable == NULL) {
        return SHR_E_UNAVAIL;
    }

    md->intr_enable(unit, intr_num);

    return SHR_E_NONE;
}

int
bcmbd_sec_intr_disable(int unit, unsigned int intr_num)
{
    bcmbd_intr_drv_t *md = sec_intr_drv[unit];

    if (md == NULL) {
        return SHR_E_UNIT;
    }

    if (md->intr_disable == NULL) {
        return SHR_E_UNAVAIL;
    }

    md->intr_disable(unit, intr_num);

    return SHR_E_NONE;
}

int
bcmbd_sec_intr_clear(int unit, unsigned int intr_num)
{
    bcmbd_intr_drv_t *md = sec_intr_drv[unit];

    if (md == NULL) {
        return SHR_E_UNIT;
    }

    if (md->intr_clear == NULL) {
        /* Do not return error if not implemented/required */
        return SHR_E_NONE;
    }

    md->intr_clear(unit, intr_num);

    return SHR_E_NONE;
}
