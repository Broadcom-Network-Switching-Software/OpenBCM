/*! \file bcma_testutil_qos.c
 *
 * QOS operation utility.
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
#include <bcma/test/util/bcma_testutil_qos.h>
#include <bcma/test/util/bcma_testutil_drv.h>

#define BSL_LOG_MODULE BSL_LS_APPL_TEST

int
bcma_testutil_drv_qos_one_to_one_map_create(int unit, bcmdrd_pbmp_t pbmp)
{
    bcma_testutil_drv_t *drv;
    bcma_testutil_qos_lt_op_t *qos_ops;

    SHR_FUNC_ENTER(unit);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);
    SHR_NULL_CHECK(drv->qos_lt_op_get, SHR_E_UNAVAIL);

    qos_ops = drv->qos_lt_op_get(unit);
    SHR_NULL_CHECK(qos_ops->qos_one_to_one_map_create, SHR_E_UNAVAIL);
    SHR_ERR_EXIT
        (qos_ops->qos_one_to_one_map_create(unit, pbmp));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_drv_qos_one_to_one_map_destroy(int unit, bcmdrd_pbmp_t pbmp)
{
    bcma_testutil_drv_t *drv;
    bcma_testutil_qos_lt_op_t *qos_ops;

    SHR_FUNC_ENTER(unit);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);
    SHR_NULL_CHECK(drv->qos_lt_op_get, SHR_E_UNAVAIL);

    qos_ops = drv->qos_lt_op_get(unit);
    SHR_NULL_CHECK(qos_ops->qos_one_to_one_map_destroy, SHR_E_UNAVAIL);
    SHR_ERR_EXIT
        (qos_ops->qos_one_to_one_map_destroy(unit, pbmp));

exit:
    SHR_FUNC_EXIT();
}

