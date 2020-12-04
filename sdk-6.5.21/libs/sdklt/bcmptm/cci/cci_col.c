/*! \file cci_col.c
 *
 * Functions for counter collection sub component
 *
 * This file contains implementation of counter collection for CCI
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <sal/sal_assert.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_types.h>
#include <sal/sal_thread.h>
#include "cci_col.h"


/*!
 * Initialize counter collection resources.
 */
int
bcmptm_cci_col_init(int unit,
                    const cci_context_t  *cci_con,
                    cci_handle_t *handle)
{
    bcmptm_cci_col_t *ctr_col = NULL;

    SHR_FUNC_ENTER(unit);
    /* Allocate the counter collection handle */
    SHR_ALLOC(ctr_col, sizeof(bcmptm_cci_col_t), "bcmptmCciCollect");
    SHR_NULL_CHECK(ctr_col, SHR_E_MEMORY);
    sal_memset(ctr_col, 0, sizeof(bcmptm_cci_col_t));
    ctr_col->parent = cci_con;
    SHR_IF_ERR_EXIT(
        bcmptm_cci_col_poll_init(unit, ctr_col, &ctr_col->hpol));
    *handle = ctr_col;

    SHR_FUNC_EXIT();
exit:
    SHR_FREE(ctr_col);
    SHR_FUNC_EXIT();

 }

/*!
 * Cleanup counter collection resources.
 */
int
bcmptm_cci_col_cleanup(int unit,
                       cci_handle_t handle)
{
    bcmptm_cci_col_t  *ctr_col = (bcmptm_cci_col_t *)handle;
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ctr_col, SHR_E_PARAM);
    bcmptm_cci_col_poll_cleanup(unit, ctr_col->hpol);
    ctr_col->hpol = NULL;
    SHR_FREE(ctr_col);

    exit:
    SHR_FUNC_EXIT();

}

/*!
 * Start counter collection.
 */
int
bcmptm_cci_col_run(int unit,
                   cci_handle_t handle)
{
    bcmptm_cci_col_t  *ctr_col = (bcmptm_cci_col_t *)handle;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ctr_col, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(
        bcmptm_cci_col_poll_run(unit, ctr_col->hpol));

exit:
    SHR_FUNC_EXIT();

}

/*!
 * Stop counter collection.
 */
int
bcmptm_cci_col_stop(int unit,
                    cci_handle_t handle)
{
    bcmptm_cci_col_t  *ctr_col = (bcmptm_cci_col_t *)handle;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ctr_col, SHR_E_PARAM);
    SHR_IF_ERR_EXIT(
        bcmptm_cci_col_poll_stop(unit,ctr_col->hpol));

exit:
    SHR_FUNC_EXIT();

}

/*!
 * Gett cci instance handle.
 */
int
bcmptm_cci_handle_get(int unit,
                      const bcmptm_cci_col_t *ctr_col,
                      cci_context_t **handle)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ctr_col, SHR_E_PARAM);

    *handle = (cci_context_t *)ctr_col->parent;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * Gett poll collection instance handle.
 */
int
bcmptm_cci_poll_handle_get(int unit,
                      cci_handle_t hcol,
                      cci_handle_t *hpol)
{
    bcmptm_cci_col_t  *ctr_col = (bcmptm_cci_col_t *)hcol;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ctr_col, SHR_E_PARAM);

    *hpol = ctr_col->hpol;
exit:
    SHR_FUNC_EXIT();
}

