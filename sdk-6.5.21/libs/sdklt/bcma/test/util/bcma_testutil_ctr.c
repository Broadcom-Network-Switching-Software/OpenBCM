/*! \file bcma_testutil_ctr.c
 *
 * Counter operation utility.
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
#include <bcma/test/util/bcma_testutil_ctr.h>
#include <bcma/test/util/bcma_testutil_drv.h>

#define BSL_LOG_MODULE BSL_LS_APPL_TEST

int
bcma_testutil_ctr_evict_add(int unit, void *user_data)
{
    bcma_testutil_drv_t *drv;
    bcma_testutil_ctr_op_t *ctr_ops;

    SHR_FUNC_ENTER(unit);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);
    SHR_NULL_CHECK(drv->ctr_op_get, SHR_E_UNAVAIL);

    ctr_ops = drv->ctr_op_get(unit);
    SHR_NULL_CHECK(ctr_ops->evict_add, SHR_E_UNAVAIL);
    SHR_ERR_EXIT
        (ctr_ops->evict_add(unit, user_data));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_ctr_evict_delete(int unit, void *user_data)
{
    bcma_testutil_drv_t *drv;
    bcma_testutil_ctr_op_t *ctr_ops;

    SHR_FUNC_ENTER(unit);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);
    SHR_NULL_CHECK(drv->ctr_op_get, SHR_E_UNAVAIL);

    ctr_ops = drv->ctr_op_get(unit);
    SHR_NULL_CHECK(ctr_ops->evict_delete, SHR_E_UNAVAIL);
    SHR_ERR_EXIT
        (ctr_ops->evict_delete(unit, user_data));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_ctr_egr_perq_stat_create(int unit, bcmdrd_pbmp_t pbmp)
{
    bcma_testutil_drv_t *drv;
    bcma_testutil_ctr_op_t *ctr_ops;

    SHR_FUNC_ENTER(unit);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);
    SHR_NULL_CHECK(drv->ctr_op_get, SHR_E_UNAVAIL);

    ctr_ops = drv->ctr_op_get(unit);
    SHR_NULL_CHECK(ctr_ops->egr_perq_stat_create, SHR_E_UNAVAIL);
    SHR_ERR_EXIT
        (ctr_ops->egr_perq_stat_create(unit, pbmp));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_ctr_egr_perq_stat_destroy(int unit, bcmdrd_pbmp_t pbmp)
{
    bcma_testutil_drv_t *drv;
    bcma_testutil_ctr_op_t *ctr_ops;

    SHR_FUNC_ENTER(unit);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);
    SHR_NULL_CHECK(drv->ctr_op_get, SHR_E_UNAVAIL);

    ctr_ops = drv->ctr_op_get(unit);
    SHR_NULL_CHECK(ctr_ops->egr_perq_stat_destroy, SHR_E_UNAVAIL);
    SHR_ERR_EXIT
        (ctr_ops->egr_perq_stat_destroy(unit, pbmp));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_ctr_egr_perq_stat_get(int unit, int port,
                                    int q_id, traffic_queue_type_t q_type,
                                    bool pkt_counter, uint64_t *count)
{
    bcma_testutil_drv_t *drv;
    bcma_testutil_ctr_op_t *ctr_ops;

    SHR_FUNC_ENTER(unit);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);
    SHR_NULL_CHECK(drv->ctr_op_get, SHR_E_UNAVAIL);

    ctr_ops = drv->ctr_op_get(unit);
    SHR_NULL_CHECK(ctr_ops->egr_perq_stat_get, SHR_E_UNAVAIL);
    SHR_ERR_EXIT
        (ctr_ops->egr_perq_stat_get(unit, port, q_id, q_type, pkt_counter, count));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_ctr_egr_perq_stat_clear(int unit, int port,
                                      int q_id, traffic_queue_type_t q_type)
{
    bcma_testutil_drv_t *drv;
    bcma_testutil_ctr_op_t *ctr_ops;

    SHR_FUNC_ENTER(unit);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);
    SHR_NULL_CHECK(drv->ctr_op_get, SHR_E_UNAVAIL);

    ctr_ops = drv->ctr_op_get(unit);
    SHR_NULL_CHECK(ctr_ops->egr_perq_stat_clear, SHR_E_UNAVAIL);
    SHR_ERR_EXIT
        (ctr_ops->egr_perq_stat_clear(unit, port, q_id, q_type));

exit:
    SHR_FUNC_EXIT();
}


