/*! \file bcmcth_agm_attach.c
 *
 * BCMCTH Aggregation Group Monitor Driver attach/detach APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>

#include <bcmdrd/bcmdrd_dev.h>

#include <bcmcth/bcmcth_agm.h>
#include <bcmcth/bcmcth_agm_drv.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_AGM

/*******************************************************************************
 * Internal public functions
 */

int
bcmcth_agm_attach(int unit, bool warm)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    /* Initialize device-specific driver */
    SHR_IF_ERR_EXIT
        (bcmcth_agm_drv_init(unit));

    /* Device-specific initialization */
    SHR_IF_ERR_EXIT
        (bcmcth_agm_init(unit));

    /* Initialize IMM driver */
    SHR_IF_ERR_EXIT
        (bcmcth_agm_imm_init(unit));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_agm_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_CONT
        (bcmcth_agm_cleanup(unit));

    SHR_FUNC_EXIT();
}
