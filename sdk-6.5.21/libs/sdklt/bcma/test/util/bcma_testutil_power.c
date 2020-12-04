/*! \file bcma_testutil_l2.c
 *
 * L2 operation utility.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <sal/sal_libc.h>
#include <shr/shr_debug.h>
#include <bcmlt/bcmlt.h>
#include <bcma/test/util/bcma_testutil_power.h>
#include <bcma/test/util/bcma_testutil_drv.h>

#define BSL_LOG_MODULE BSL_LS_APPL_TEST


int
bcma_testutil_power_help(int unit)
{
    bcma_testutil_drv_t *drv;

    SHR_FUNC_ENTER(unit);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);
    SHR_NULL_CHECK(drv->power_op_get->power_init, SHR_E_UNAVAIL);

    SHR_ERR_EXIT
        (drv->power_op_get->power_help(unit));

exit:
    SHR_FUNC_EXIT();
}


int
bcma_testutil_power_init(int unit, void *bp)
{
    bcma_testutil_drv_t *drv;

    SHR_FUNC_ENTER(unit);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);
    SHR_NULL_CHECK(drv->power_op_get->power_init, SHR_E_UNAVAIL);

    SHR_ERR_EXIT
        (drv->power_op_get->power_init(unit, bp));

exit:
    SHR_FUNC_EXIT();
}


int
bcma_testutil_power_port_set(int unit, void *bp)
{
    bcma_testutil_drv_t *drv;

    SHR_FUNC_ENTER(unit);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);
    SHR_NULL_CHECK(drv->power_op_get->power_port_set, SHR_E_UNAVAIL);

    SHR_ERR_EXIT
        (drv->power_op_get->power_port_set(unit, bp));

exit:
    SHR_FUNC_EXIT();
}


int
bcma_testutil_power_config_set(int unit, void *bp)
{
    bcma_testutil_drv_t *drv;

    SHR_FUNC_ENTER(unit);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);
    SHR_NULL_CHECK(drv->power_op_get->power_config_set, SHR_E_UNAVAIL);

    SHR_ERR_EXIT
        (drv->power_op_get->power_config_set(unit, bp));

 exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_power_txrx(int unit, void *bp)
{
    bcma_testutil_drv_t *drv;

    SHR_FUNC_ENTER(unit);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);
    SHR_NULL_CHECK(drv->power_op_get->power_txrx, SHR_E_UNAVAIL);

    SHR_ERR_EXIT
        (drv->power_op_get->power_txrx(unit, bp));

exit:
    SHR_FUNC_EXIT();
}
