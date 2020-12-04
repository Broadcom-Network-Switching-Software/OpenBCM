/*! \file bcmcth_ctr_evict_attach.c
 *
 * BCMCTH CTR_EVICT Driver attach/detach APIs
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmcth/bcmcth_ctr_evict.h>
#include <bcmcth/bcmcth_ctr_evict_drv.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_CTREVICT

/*******************************************************************************
 * Internal public functions
 */

int
bcmcth_ctr_evict_attach(int unit, bool warm)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    /* Initialize device-specific driver */
    SHR_IF_ERR_EXIT(bcmcth_ctr_evict_drv_init(unit));

    /* Initialize internal structures */
    SHR_IF_ERR_EXIT(bcmcth_ctr_evict_dev_init(unit, warm));

    /* Initialize IMM driver */
    SHR_IF_ERR_EXIT(bcmcth_ctr_evict_imm_init(unit));


exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ctr_evict_detach(int unit)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT(bcmcth_ctr_evict_drv_cleanup(unit));
exit:
    SHR_FUNC_EXIT();
}
