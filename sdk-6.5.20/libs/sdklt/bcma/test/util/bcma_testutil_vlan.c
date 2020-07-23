/*! \file bcma_testutil_vlan.c
 *
 * VLAN operation utility.
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
#include <bcma/bcmlt/bcma_bcmlt.h>
#include <bcma/test/util/bcma_testutil_drv.h>
#include <bcma/test/util/bcma_testutil_common.h>
#include <bcma/test/util/bcma_testutil_vlan.h>

#define BSL_LOG_MODULE BSL_LS_APPL_TEST

int
bcma_testutil_vlan_check(int unit, int vid)
{
    bcma_testutil_drv_t *drv;
    bcma_testutil_vlan_op_t *vlan_ops;

    SHR_FUNC_ENTER(unit);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);
    SHR_NULL_CHECK(drv->vlan_op_get, SHR_E_UNAVAIL);

    vlan_ops = drv->vlan_op_get(unit);
    SHR_NULL_CHECK(vlan_ops->check, SHR_E_UNAVAIL);
    SHR_ERR_EXIT
        (vlan_ops->check(unit, vid));

exit:
    SHR_FUNC_EXIT();
}


int
bcma_testutil_vlan_create(int unit, int vid, int ing_egrmbr_pid)
{
    bcma_testutil_drv_t* drv;
    bcma_testutil_vlan_op_t *vlan_ops;

    SHR_FUNC_ENTER(unit);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);
    SHR_NULL_CHECK(drv->vlan_op_get, SHR_E_UNAVAIL);

    vlan_ops = drv->vlan_op_get(unit);
    SHR_NULL_CHECK(vlan_ops->create, SHR_E_UNAVAIL);
    SHR_ERR_EXIT
        (vlan_ops->create(unit, vid, ing_egrmbr_pid));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_vlan_destroy(int unit, int vid)
{
    bcma_testutil_drv_t* drv;
    bcma_testutil_vlan_op_t *vlan_ops;

    SHR_FUNC_ENTER(unit);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);
    SHR_NULL_CHECK(drv->vlan_op_get, SHR_E_UNAVAIL);

    vlan_ops = drv->vlan_op_get(unit);
    SHR_NULL_CHECK(vlan_ops->destroy, SHR_E_UNAVAIL);
    SHR_ERR_EXIT
        (vlan_ops->destroy(unit, vid));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_vlan_port_vlan_set(int unit, int port, int vid)
{
    bcma_testutil_drv_t* drv;
    bcma_testutil_vlan_op_t *vlan_ops;

    SHR_FUNC_ENTER(unit);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);
    SHR_NULL_CHECK(drv->vlan_op_get, SHR_E_UNAVAIL);

    vlan_ops = drv->vlan_op_get(unit);
    SHR_NULL_CHECK(vlan_ops->port_vlan_set, SHR_E_UNAVAIL);
    SHR_ERR_EXIT
        (vlan_ops->port_vlan_set(unit, port, vid));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_vlan_port_add(int unit, int vid, const bcmdrd_pbmp_t pbmp,
                            const bcmdrd_pbmp_t ubmp, bool src_port_block)
{
    bcma_testutil_drv_t* drv;
    bcma_testutil_vlan_op_t *vlan_ops;

    SHR_FUNC_ENTER(unit);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);
    SHR_NULL_CHECK(drv->vlan_op_get, SHR_E_UNAVAIL);

    vlan_ops = drv->vlan_op_get(unit);
    SHR_NULL_CHECK(vlan_ops->port_add, SHR_E_UNAVAIL);
    SHR_ERR_EXIT
        (vlan_ops->port_add(unit, vid, pbmp, ubmp, src_port_block));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_vlan_port_update(int unit, int vid, const bcmdrd_pbmp_t pbmp,
                               const bcmdrd_pbmp_t ubmp)
{
    bcma_testutil_drv_t* drv;
    bcma_testutil_vlan_op_t *vlan_ops;

    SHR_FUNC_ENTER(unit);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);
    SHR_NULL_CHECK(drv->vlan_op_get, SHR_E_UNAVAIL);

    vlan_ops = drv->vlan_op_get(unit);
    SHR_NULL_CHECK(vlan_ops->port_update, SHR_E_UNAVAIL);
    SHR_ERR_EXIT
        (vlan_ops->port_update(unit, vid, pbmp, ubmp));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_vlan_port_remove(int unit, int vid, const bcmdrd_pbmp_t pbmp)
{
    bcma_testutil_drv_t* drv;
    bcma_testutil_vlan_op_t *vlan_ops;

    SHR_FUNC_ENTER(unit);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);
    SHR_NULL_CHECK(drv->vlan_op_get, SHR_E_UNAVAIL);

    vlan_ops = drv->vlan_op_get(unit);
    SHR_NULL_CHECK(vlan_ops->port_remove, SHR_E_UNAVAIL);
    SHR_ERR_EXIT
        (vlan_ops->port_remove(unit, vid, pbmp));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_vlan_stp_set(int unit, int vid, const bcmdrd_pbmp_t pbmp,
                           bcma_testutil_stp_state_t stp_state)
{
    bcma_testutil_drv_t* drv;
    bcma_testutil_vlan_op_t *vlan_ops;

    SHR_FUNC_ENTER(unit);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);
    SHR_NULL_CHECK(drv->vlan_op_get, SHR_E_UNAVAIL);

    vlan_ops = drv->vlan_op_get(unit);
    SHR_NULL_CHECK(vlan_ops->stp_set, SHR_E_UNAVAIL);
    SHR_ERR_EXIT
        (vlan_ops->stp_set(unit, vid, pbmp, stp_state));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_vlan_untag_action_add(int unit,
                                    bcma_testutil_vlan_tag_action_t tag_action)
{
    bcma_testutil_drv_t* drv;
    bcma_testutil_vlan_op_t *vlan_ops;

    SHR_FUNC_ENTER(unit);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);
    SHR_NULL_CHECK(drv->vlan_op_get, SHR_E_UNAVAIL);

    vlan_ops = drv->vlan_op_get(unit);
    SHR_NULL_CHECK(vlan_ops->untag_action_add, SHR_E_UNAVAIL);
    SHR_ERR_EXIT
        (vlan_ops->untag_action_add(unit, tag_action));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_vlan_untag_action_remove(int unit)
{
    bcma_testutil_drv_t* drv;
    bcma_testutil_vlan_op_t *vlan_ops;

    SHR_FUNC_ENTER(unit);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);
    SHR_NULL_CHECK(drv->vlan_op_get, SHR_E_UNAVAIL);

    vlan_ops = drv->vlan_op_get(unit);
    SHR_NULL_CHECK(vlan_ops->untag_action_remove, SHR_E_UNAVAIL);
    SHR_ERR_EXIT
        (vlan_ops->untag_action_remove(unit));

exit:
    SHR_FUNC_EXIT();
}
