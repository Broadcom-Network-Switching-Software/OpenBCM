/*! \file bcmcth_mirror_attach.c
 *
 * BCMCTH Mirror Driver attach/detach APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>

#include <bcmdrd/bcmdrd_dev.h>

#include <bcmcth/bcmcth_mirror.h>
#include <bcmcth/bcmcth_mirror_drv.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_MIRROR

/*******************************************************************************
* Public functions
 */

int
bcmcth_mirror_attach(int unit, bool warm)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    /* Initialize device-specific driver */
    SHR_IF_ERR_EXIT
        (bcmcth_mirror_drv_init(unit));

    /* Initialize the MIRROR component. */
    SHR_IF_ERR_EXIT
        (bcmcth_mirror_init(unit, warm));

    /* Initialize IMM driver */
    SHR_IF_ERR_EXIT
        (bcmcth_mirror_imm_init(unit, warm));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mirror_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* De-initialize IMM driver */
    SHR_IF_ERR_EXIT
        (bcmcth_mirror_imm_deinit(unit));

    /* De-initialize the MIRROR component. */
    SHR_IF_ERR_EXIT
        (bcmcth_mirror_deinit(unit));

exit:

    SHR_FUNC_EXIT();
}
