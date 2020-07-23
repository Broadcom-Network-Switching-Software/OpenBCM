/*! \file bcmbd_ccmdma.c
 *
 * CCMDMA driver APIs
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <sal_config.h>
#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <sal/sal_sem.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmbd/bcmbd_ccmdma.h>
#include <bcmbd/bcmbd_internal.h>

#define BSL_LOG_MODULE  BSL_LS_BCMBD_CCMDMA

static ccmdma_ctrl_t ccmdma_ctrl[CCMDMA_DEV_NUM_MAX];

int
bcmbd_ccmdma_attach(int unit)
{
    ccmdma_ctrl_t *ctrl = NULL;
    int i;

    SHR_FUNC_ENTER(unit);

    ctrl = bcmbd_ccmdma_ctrl_get(unit);
    if (!ctrl) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    sal_memset(ctrl, 0, sizeof(*ctrl));
    ctrl->unit = unit;

    for (i = 0; i < DEV_CHAN_NUM_MAX; i++) {
        ctrl->intr[i] = sal_sem_create("bcmbdCcmDmaIntrSem", SAL_SEM_BINARY, 0);
        if (!ctrl->intr[i]) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }
    }

    ctrl->active = 1;

exit:
    if (SHR_FUNC_ERR()) {
        if (ctrl) {
            for (i = 0; i < DEV_CHAN_NUM_MAX; i++) {
                if (ctrl->intr[i]) {
                    sal_sem_destroy(ctrl->intr[i]);
                    ctrl->intr[i] = NULL;
                }
            }
        }
    }

    SHR_FUNC_EXIT();
}

int
bcmbd_ccmdma_detach(int unit)
{
    ccmdma_ctrl_t *ctrl = &ccmdma_ctrl[unit];
    int i;

    if (!ctrl->active) {
        return SHR_E_NONE;
    }

    ctrl->active = 0;

    for (i = 0; i < DEV_CHAN_NUM_MAX; i++) {
        if (ctrl->intr[i]) {
            sal_sem_destroy(ctrl->intr[i]);
            ctrl->intr[i] = NULL;
        }
    }

    return SHR_E_NONE;
}

int
bcmbd_ccmdma_xfer(int unit, bcmbd_ccmdma_work_t *work)
{
    ccmdma_ctrl_t *ctrl = &ccmdma_ctrl[unit];

    if (!ctrl || !ctrl->active || !ctrl->ops) {
        return SHR_E_UNAVAIL;
    }

    if (!work) {
        return SHR_E_PARAM;
    }

    return ctrl->ops->work_execute(ctrl, work);
}

ccmdma_ctrl_t *
bcmbd_ccmdma_ctrl_get(int unit)
{
    if (!bcmdrd_dev_exists(unit)) {
        return NULL;
    }

    return &ccmdma_ctrl[unit];
}
