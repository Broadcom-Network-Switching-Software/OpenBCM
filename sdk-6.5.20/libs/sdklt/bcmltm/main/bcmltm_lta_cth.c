/*! \file bcmltm_lta_cth.c
 *
 * Custom table handler initialization and cleanup.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmltm/bcmltm_lta_cth_internal.h>
#include <bcmltm/bcmltm_lta_intf_internal.h>
#include <bsl/bsl.h>

#define BSL_LOG_MODULE BSL_LS_BCMLTM_ENTRY

/*!
 * \brief Call custom table handler initializer or cleanup functions.
 *
 * Call custom table handler initializer or cleanup functions for all
 * custom tables that have requested initialization service that are
 * mapped to the given unit. The value of 'cleanup' determines if the
 * cleanup function or init function is called.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  cleanup         Cleanup (true) or Initialize (false).
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
static int
bcmltm_lta_cth_init_cleanup(int unit, bool cleanup)
{
    size_t count;
    uint32_t init_id;
    int rv;
    const bcmltd_table_lifecycle_intf_t *lc_intf;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LS_BCMLTM_INIT,
                (BSL_META_U(unit,
                 "Logical Table Resource Manager CTH init or cleanup\n")));

    rv = bcmlrd_table_count_get(unit, &count);

    if (rv == SHR_E_UNAVAIL || count == 0) {
        /* No CTH initialization if no tables for this unit. */
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(rv);
    lc_intf = bcmltm_lta_intf_table_lifecycle_get(unit);

    if (lc_intf == NULL) {
        SHR_EXIT();
    }

    for (init_id = 0; init_id < lc_intf->count; init_id++) {
        const bcmltd_table_lifecycle_handler_t *handler;
        bcmltd_handler_lifecycle_op_f op;
        handler = lc_intf->handler[init_id];
        if (handler == NULL) {
            /* Something is seriously broken if handler is NULL */
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        /* Call cleanup or init depending on 'cleanup argument'. */
        op = cleanup ? handler->cleanup : handler->init;
        SHR_IF_ERR_EXIT(op(unit, handler->arg));
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_lta_cth_init(int unit)
{
    return bcmltm_lta_cth_init_cleanup(unit, false);
}


int
bcmltm_lta_cth_cleanup(int unit)
{
    return bcmltm_lta_cth_init_cleanup(unit, true);
}
