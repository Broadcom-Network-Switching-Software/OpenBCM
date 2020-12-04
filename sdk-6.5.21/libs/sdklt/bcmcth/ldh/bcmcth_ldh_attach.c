/*! \file bcmcth_ldh_attach.c
 *
 * BCMCTH Latency Distribution Histogram Driver attach/detach
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>

#include <bcmdrd/bcmdrd_dev.h>

#include <bcmcth/bcmcth_ldh.h>
#include <bcmcth/bcmcth_ldh_drv.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_LDH

/*******************************************************************************
* Public functions
 */

int
bcmcth_ldh_attach(int unit, bool warm)
{
    int rv = SHR_E_NONE;
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    /* Initialize device-specific driver */
    rv = bcmcth_ldh_drv_init(unit);
    if (rv == SHR_E_UNAVAIL) {
        SHR_EXIT();
    }

    /* Device-specific initialization */
    SHR_IF_ERR_EXIT
        (bcmcth_ldh_init(unit));

    /* Initialize IMM driver */
    SHR_IF_ERR_EXIT
        (bcmcth_ldh_imm_init(unit));

    /* Initialize device-specific info handler */
    SHR_IF_ERR_EXIT
        (bcmcth_ldh_device_info_init(unit));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ldh_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    bcmcth_ldh_imm_cleanup(unit);

    SHR_IF_ERR_CONT
        (bcmcth_ldh_cleanup(unit));

    SHR_FUNC_EXIT();
}
