/*! \file bcmcth_lb_attach.c
 *
 * BCMCTH Load Balance Driver attach/detach APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>

#include <bcmdrd/bcmdrd_dev.h>

#include <bcmcth/bcmcth_lb.h>
#include <bcmcth/bcmcth_lb_drv.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_LB

/*******************************************************************************
* Public functions
 */

int
bcmcth_lb_attach(int unit, bool warm)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    /* Initialize device-specific driver */
    SHR_IF_ERR_EXIT
        (bcmcth_lb_drv_init(unit));

    /* Initialize hash_info handler */
    SHR_IF_ERR_EXIT
        (bcmcth_lb_hash_info_init(unit));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_lb_detach(int unit)
{
    return SHR_E_NONE;
}
