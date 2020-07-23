/*! \file bcmcth_udf_attach.c
 *
 * BCMCTH UDF driver attach/detach APIs. This is the interface call
 * to system manager.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmcth/bcmcth_udf_drv.h>
#include <bcmcth/bcmcth_udf.h>
#include <bcmlrd/bcmlrd_conf.h>
#include <bcmcth/bcmcth_udf_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMCTH_UDF

/*******************************************************************************
 * Public Functions
 */
int
bcmcth_udf_attach(int unit, bool warm)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    SHR_IF_ERR_EXIT(bcmcth_udf_var_info_attach(unit));

    /* Initialize device-specific driver */
    SHR_IF_ERR_EXIT
        (bcmcth_udf_drv_init(unit, warm));

    /* Initialize device-specific driver */
    SHR_IF_ERR_EXIT
        (bcmcth_udf_policy_info_get(unit, warm));


exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_udf_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* De-initialize the UDF component. */
    SHR_IF_ERR_EXIT
        (bcmcth_udf_deinit(unit));


exit:
    SHR_FUNC_EXIT();
}
