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
#include <bcma/test/util/bcma_testutil_l2.h>
#include <bcma/test/util/bcma_testutil_drv.h>

#define BSL_LOG_MODULE BSL_LS_APPL_TEST

int
bcma_testutil_l2_unicast_add(int unit, int port, const shr_mac_t mac,
                             int vid, int copy2cpu)
{
    bcma_testutil_drv_t* drv;
    bcma_testutil_l2_op_t *l2_ops;

    SHR_FUNC_ENTER(unit);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);
    SHR_NULL_CHECK(drv->l2_op_get, SHR_E_UNAVAIL);

    l2_ops = drv->l2_op_get(unit);
    SHR_NULL_CHECK(l2_ops->unicast_add, SHR_E_UNAVAIL);
    SHR_ERR_EXIT
        (l2_ops->unicast_add(unit, port, mac, vid, copy2cpu));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_l2_unicast_update(int unit, int port, const shr_mac_t mac,
                                int vid, int copy2cpu)
{
    bcma_testutil_drv_t* drv;
    bcma_testutil_l2_op_t *l2_ops;

    SHR_FUNC_ENTER(unit);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);
    SHR_NULL_CHECK(drv->l2_op_get, SHR_E_UNAVAIL);

    l2_ops = drv->l2_op_get(unit);
    SHR_NULL_CHECK(l2_ops->unicast_update, SHR_E_UNAVAIL);
    SHR_ERR_EXIT
        (l2_ops->unicast_update(unit, port, mac, vid, copy2cpu));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_l2_unicast_delete(int unit, int port, const shr_mac_t mac,
                                int vid, int copy2cpu)
{
    bcma_testutil_drv_t* drv;
    bcma_testutil_l2_op_t *l2_ops;

    SHR_FUNC_ENTER(unit);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);
    SHR_NULL_CHECK(drv->l2_op_get, SHR_E_UNAVAIL);

    l2_ops = drv->l2_op_get(unit);
    SHR_NULL_CHECK(l2_ops->unicast_delete, SHR_E_UNAVAIL);
    SHR_ERR_EXIT
        (l2_ops->unicast_delete(unit, port, mac, vid, copy2cpu));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_l2_multicast_add(int unit, int group_id, bcmdrd_pbmp_t pbmp,
                               const shr_mac_t mac, int vid, int copy2cpu)
{
    bcma_testutil_drv_t* drv;
    bcma_testutil_l2_op_t *l2_ops;

    SHR_FUNC_ENTER(unit);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);
    SHR_NULL_CHECK(drv->l2_op_get, SHR_E_UNAVAIL);

    l2_ops = drv->l2_op_get(unit);
    SHR_NULL_CHECK(l2_ops->multicast_add, SHR_E_UNAVAIL);
    SHR_ERR_EXIT
        (l2_ops->multicast_add(unit, group_id, pbmp, mac, vid, copy2cpu));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_l2_multicast_delete(int unit, int group_id, bcmdrd_pbmp_t pbmp,
                                  const shr_mac_t mac, int vid, int copy2cpu)
{
    bcma_testutil_drv_t* drv;
    bcma_testutil_l2_op_t *l2_ops;

    SHR_FUNC_ENTER(unit);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);
    SHR_NULL_CHECK(drv->l2_op_get, SHR_E_UNAVAIL);

    l2_ops = drv->l2_op_get(unit);
    SHR_NULL_CHECK(l2_ops->multicast_delete, SHR_E_UNAVAIL);
    SHR_ERR_EXIT
        (l2_ops->multicast_delete(unit, group_id, pbmp,
                                      mac, vid, copy2cpu));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_l2_multicast_update(int unit, int group_id, bcmdrd_pbmp_t pbmp,
                                  const shr_mac_t mac, int vid, int copy2cpu)
{
    bcma_testutil_drv_t *drv;
    bcma_testutil_l2_op_t *l2_ops;

    SHR_FUNC_ENTER(unit);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);
    SHR_NULL_CHECK(drv->l2_op_get, SHR_E_UNAVAIL);

    l2_ops = drv->l2_op_get(unit);
    SHR_NULL_CHECK(l2_ops->multicast_update, SHR_E_UNAVAIL);
    SHR_ERR_EXIT
        (l2_ops->multicast_update(unit, group_id, pbmp,
                                      mac, vid, copy2cpu));

exit:
    SHR_FUNC_EXIT();
}
