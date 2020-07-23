/*! \file bcma_bcmlt_transaction.c
 *
 * Functions based on bcmlt library.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <shr/shr_error.h>

#include <sal/sal_assert.h>
#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>

#include <bcma/bcmlt/bcma_bcmlt.h>

int
bcma_bcmlt_transaction_entry_add(bcma_bcmlt_transaction_info_t *ti,
                                 bcma_bcmlt_entry_info_t *ei)
{
    int rv;

    if (ti == NULL || ei == NULL) {
        return -1;
    }

    if (ei->pt) {
        rv = bcmlt_transaction_pt_entry_add(ti->th, ei->pt_opcode, ei->eh);
    } else {
        rv = bcmlt_transaction_entry_add(ti->th, ei->opcode, ei->eh);
    }
    if (SHR_FAILURE(rv)) {
        cli_out("%s%s: bcmlt_transaction_%sentry_add failed: "
                BCMA_BCMLT_ERRMSG_FMT".\n",
                BCMA_CLI_CONFIG_ERROR_STR, ei->name, ei->pt ? "pt_" : "",
                BCMA_BCMLT_ERRMSG(rv));
        return -1;
    }

    assert(ei->next == NULL);

    if (ti->ei_head == NULL) {
        assert(ti->ei_tail == NULL);
        ti->ei_head = ei;
        ti->ei_tail = ei;
    } else {
        ti->ei_tail->next = ei;
        ti->ei_tail = ei;
    }

    return 0;
}

bcma_bcmlt_transaction_info_t *
bcma_bcmlt_transaction_info_create(int unit, bcmlt_trans_type_t type,
                                   bcma_bcmlt_commit_params_t *params)
{
    int rv;
    bcmlt_transaction_hdl_t th;
    bcma_bcmlt_transaction_info_t *ti;
    bcma_bcmlt_commit_params_t *commit_params;

    ti = sal_alloc(sizeof(*ti), "bcmaLtTransactionInfo");
    if (ti == NULL) {
        return NULL;
    }
    sal_memset(ti, 0, sizeof(*ti));

    rv = bcmlt_transaction_allocate(type, &th);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_BCMLT,
                  (BSL_META("Failed to allocate transaction handle: "
                            "%s (%d)\n"), shr_errmsg(rv), rv));
        bcma_bcmlt_transaction_info_destroy(ti);
        return NULL;
    }
    ti->unit = unit;
    ti->th = th;
    ti->type = type;
    commit_params = &ti->commit_params;
    if (params) {
        sal_memcpy(commit_params, params, sizeof(*commit_params));
    } else {
        /* Use default parameters */
        commit_params->async = false;
        commit_params->notify = BCMLT_NOTIF_OPTION_HW;
        commit_params->priority = BCMLT_PRIORITY_NORMAL;
    }

    return ti;
}

int
bcma_bcmlt_transaction_info_destroy(bcma_bcmlt_transaction_info_t *ti)
{
    bcma_bcmlt_entry_info_t *ei, *ei_head;

    if (!ti) {
        return -1;
    }

    if (ti->th) {
        bcmlt_transaction_free(ti->th);
    }

    /* Free the entries added to the transaction */
    ei_head = ti->ei_head;
    while (ei_head) {
        ei = ei_head;
        ei_head = ei_head->next;
        bcma_bcmlt_entry_info_destroy(ei);
    }

    sal_free(ti);

    return 0;
}
