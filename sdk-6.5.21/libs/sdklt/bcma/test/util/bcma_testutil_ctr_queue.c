/*! \file bcma_testutil_ctr_queue.c
 *
 * Per-queue counter operation utility.
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
#include <bcma/test/util/bcma_testutil_tm.h>
#include <bcmtm/bcmtm_utils.h>

#define BSL_LOG_MODULE BSL_LS_APPL_TEST

static int
queue_ctr_evict_add(int unit, void *user_data)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    /* Stop counter collection in CCI */
    SHR_IF_ERR_EXIT(bcmlt_entry_allocate(unit, "CTR_CONTROL", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "COLLECTION_ENABLE", 0));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));

    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }

    /* Enable EGR_PERQ counter eviction */
    SHR_IF_ERR_EXIT(bcmlt_entry_allocate(unit, "CTR_EGR_Q_CONTROL", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, "EVICTION_MODE", "THRESHOLD"));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

static int
queue_ctr_evict_delete(int unit, void *user_data)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmlt_entry_allocate(unit, "CTR_EGR_Q_CONTROL", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL));

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

static int
queue_ctr_egr_perq_stat_create(int unit, bcmdrd_pbmp_t pbmp)
{
    SHR_FUNC_ENTER(unit);
    /* h/w support per queue counter, do nothing */
    SHR_FUNC_EXIT();
}

static int
queue_ctr_egr_perq_stat_destroy(int unit, bcmdrd_pbmp_t pbmp)
{
    SHR_FUNC_ENTER(unit);
    /* h/w support per queue counter, do nothing */
    SHR_FUNC_EXIT();
}

static int
queue_ctr_egr_perq_stat_get(int unit, int port,
                            int q_id, traffic_queue_type_t q_type,
                            bool pkt_counter, uint64_t *count)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int num_mc_q, num_uc_q;

    SHR_FUNC_ENTER(unit);

    if (q_type == TRAFFIC_Q_TYPE_UC) {
        SHR_IF_ERR_EXIT
            (bcmtm_lport_num_ucq_get(unit, port, &num_uc_q));
        if (q_id >= num_uc_q) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(unit, "CTR_EGR_UC_Q", &entry_hdl));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, "UC_Q", q_id));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, "PORT_ID", port));
    } else if (q_type == TRAFFIC_Q_TYPE_MC) {
        SHR_IF_ERR_EXIT
            (bcmtm_lport_num_mcq_get(unit, port, &num_mc_q));
        if (q_id >= num_mc_q) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(unit, "CTR_EGR_MC_Q", &entry_hdl));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, "MC_Q", q_id));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, "PORT_ID", port));
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    bcmlt_entry_attrib_set(entry_hdl, BCMLT_ENT_ATTR_GET_FROM_HW);
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));
    if (pkt_counter) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(entry_hdl, "PKT", count));
    } else {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(entry_hdl, "BYTES", count));
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();

}

static int
queue_ctr_egr_perq_stat_clear(int unit, int port,
                              int q_id, traffic_queue_type_t q_type)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int rv;
    int num_mc_q, num_uc_q;

    SHR_FUNC_ENTER(unit);

    if (q_type == TRAFFIC_Q_TYPE_UC) {
        SHR_IF_ERR_EXIT
            (bcmtm_lport_num_ucq_get(unit, port, &num_uc_q));
        if (q_id >= num_uc_q) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(unit, "CTR_EGR_UC_Q", &entry_hdl));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, "UC_Q", q_id));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, "PORT_ID", port));
    } else if (q_type == TRAFFIC_Q_TYPE_MC) {
        SHR_IF_ERR_EXIT
            (bcmtm_lport_num_mcq_get(unit, port, &num_mc_q));
        if (q_id >= num_mc_q) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(unit, "CTR_EGR_MC_Q", &entry_hdl));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, "MC_Q", q_id));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, "PORT_ID", port));
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    rv = bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL);
    if (rv != SHR_E_NONE && rv != SHR_E_NOT_FOUND) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}


static bcma_testutil_ctr_op_t bcma_testutil_queue_ctr_op = {
    .evict_add = queue_ctr_evict_add,
    .evict_delete = queue_ctr_evict_delete,
    .egr_perq_stat_create = queue_ctr_egr_perq_stat_create,
    .egr_perq_stat_destroy = queue_ctr_egr_perq_stat_destroy,
    .egr_perq_stat_get = queue_ctr_egr_perq_stat_get,
    .egr_perq_stat_clear = queue_ctr_egr_perq_stat_clear,
};

bcma_testutil_ctr_op_t *bcma_testutil_queue_ctr_op_get(int unit)
{
    return &bcma_testutil_queue_ctr_op;
}
