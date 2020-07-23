/*! \file bcmcth_oam_attach.c
 *
 * BCMCTH OAM attach/detach APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>

#include <bcmdrd/bcmdrd_dev.h>

#include <bcmcth/bcmcth_oam.h>
#include <bcmcth/bcmcth_oam_imm.h>
#include <bcmcth/bcmcth_oam_drv.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_OAM

/*******************************************************************************
* Public functions
 */

int
bcmcth_oam_attach(int unit, bool warm)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    /* Initialize device-specific driver */
    SHR_IF_ERR_EXIT
        (bcmcth_oam_drv_init(unit));

    /* Initialize the OAM component. */
    SHR_IF_ERR_EXIT
        (bcmcth_oam_init(unit, warm));

    /* Initialize IMM driver */
    SHR_IF_ERR_EXIT
        (bcmcth_oam_imm_init(unit, warm));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_oam_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* De-initialize the OAM component. */
    SHR_IF_ERR_EXIT
        (bcmcth_oam_deinit(unit));
exit:
    SHR_FUNC_EXIT();
}

