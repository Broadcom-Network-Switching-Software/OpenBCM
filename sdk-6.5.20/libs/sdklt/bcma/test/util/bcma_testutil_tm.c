/*! \file bcma_testutil_tm.c
 *
 * Chip-specific Check Health Utility.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#include <sal/sal_types.h>
#include <shr/shr_debug.h>
#include <sal/sal_assert.h>

#include <bcmdrd/bcmdrd_pt.h>
#include <bcmlt/bcmlt.h>
#include <bcma/test/util/bcma_testutil_drv.h>
#include <bcma/test/util/bcma_testutil_common.h>
#include <bcma/test/util/bcma_testutil_pt_common.h>
#include <bcma/test/util/bcma_testutil_cmicx.h>
#include <bcma/test/util/bcma_testutil_tm.h>
#include <bcma/test/util/bcma_testutil_ctr.h>
#include <bcma/test/util/bcma_testutil_port.h>
#include <bcmtm/bcmtm_utils.h>

#define BSL_LOG_MODULE BSL_LS_APPL_TEST
#define MAX_PHYS_PORTS 512

int
bcma_testustil_tm_comp_pt_field_value(int unit, bcmdrd_sid_t sid,
                                      bcmdrd_fid_t fid, int index, int inst,
                                      int wsize, uint32_t *exp_value)
{
    bcmdrd_sym_field_info_t finfo;
    uint32_t act_value[2] = {0};
    int i, j;
    int fail_cnt = 0;
    int min_index = 0, max_index = 0;
    int loop_idx;
    int min_inst = 0, max_inst = 0, loop_inst;
    uint32_t mask[2] = {0};

    SHR_FUNC_ENTER(unit);
    if (index == -1) {
        max_index = bcmdrd_pt_index_max(unit, sid);
        min_index = bcmdrd_pt_index_min(unit, sid);
    } else {
        min_index = max_index = index;
    }
    if (inst == -1) {
        max_inst = bcmdrd_pt_num_tbl_inst(unit, sid) - 1;
    } else {
        min_inst = max_inst = inst;
    }

    for (j = 0; j < wsize; j++) {
        mask[j] = 0xffffffff;
    }

    for (loop_inst = min_inst; loop_inst <= max_inst; loop_inst++) {
        for (loop_idx = min_index; loop_idx <= max_index; loop_idx++) {
            if (bcmdrd_pt_index_valid(unit, sid, loop_inst, loop_idx)) {
                SHR_IF_ERR_EXIT(
                    bcma_testutil_pt_field_get(unit, sid, fid, loop_idx, loop_inst,
                        act_value));
                SHR_IF_ERR_EXIT(bcmdrd_pt_field_info_get(unit, sid, fid, &finfo));
                for (i = 0; i < wsize; i++) {
                    if (((exp_value[i] ^ act_value[i]) & mask[i]) != 0) {
                            LOG_ERROR(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "FAIL: %s, %s, inst: %d, index: %d,"
                                        "data word: %d, Exp value: %d,"
                                        "Actual value: %d \n"),
                             bcmdrd_pt_sid_to_name(unit, sid), finfo.name,
                             loop_inst, loop_idx,
                             i, exp_value[i], act_value[i]));
                            fail_cnt++;
                    } else {
                        LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "PASS: %s, %s, inst: %d, index: %d,"
                                        "data word: %d, Exp value: %d,"
                                        "Actual value: %d \n"),
                             bcmdrd_pt_sid_to_name(unit, sid), finfo.name,
                             loop_inst, loop_idx,
                             i, exp_value[i], act_value[i]));
                    }
                }
            }
        }
    }
    SHR_ERR_EXIT(fail_cnt);
exit:
    SHR_FUNC_EXIT();

}


static int
tm_stat_ing_port_drop_table_stat_clear(int unit, int port)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int rv, i;
    const char *table[2] = {"CTR_ING_TM_PORT_UC_DROP", "CTR_TM_OBM_PORT_DROP"};

    SHR_FUNC_ENTER(unit);

    /* Delete any existing entry and insert new */
    for (i = 0; i < 2; i++) {
        SHR_IF_ERR_EXIT(bcmlt_entry_allocate(unit, table[i], &entry_hdl));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, "PORT_ID", port));

        if (i == 1) {
            /* Check if port has OBM */
            if (!bcmtm_obm_port_validation(unit, port)) {
                continue;
            }
        }

        rv = bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL);
        if (rv != SHR_E_NONE && rv != SHR_E_NOT_FOUND) {
            SHR_ERR_EXIT(SHR_E_FAIL);
        }

        SHR_IF_ERR_EXIT(bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_INSERT,
                    BCMLT_PRIORITY_NORMAL));

        if (entry_hdl != BCMLT_INVALID_HDL) {
            bcmlt_entry_free(entry_hdl);
            entry_hdl = BCMLT_INVALID_HDL;
        }
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();

}

static int
tm_stat_egr_queue_drop_table_stat_clear(int unit, int port)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int rv, buf_pool;
    int num_mc_q, num_uc_q;
    int q_id;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT(bcmtm_lport_num_mcq_get(unit, port, &num_mc_q));
    SHR_IF_ERR_EXIT(bcmtm_lport_num_ucq_get(unit, port, &num_uc_q));

    /* Delete any existing entry and insert new */
    for (buf_pool = 0; buf_pool < 2; buf_pool++) {
        if (bcmtm_itm_valid_get(unit, buf_pool) == SHR_E_NONE) {
            for (q_id = 0; q_id < num_uc_q; q_id++) {
                SHR_IF_ERR_EXIT(bcmlt_entry_allocate(unit, "CTR_TM_UC_Q_DROP",
                            &entry_hdl));

                SHR_IF_ERR_EXIT
                    (bcmlt_entry_field_add(entry_hdl, "PORT_ID", port));
                SHR_IF_ERR_EXIT
                    (bcmlt_entry_field_add(entry_hdl, "BUFFER_POOL", buf_pool));
                SHR_IF_ERR_EXIT
                    (bcmlt_entry_field_add(entry_hdl, "TM_UC_Q_ID", q_id));

                rv = bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_DELETE,
                        BCMLT_PRIORITY_NORMAL);
                if (rv != SHR_E_NONE && rv != SHR_E_NOT_FOUND) {
                    SHR_ERR_EXIT(SHR_E_FAIL);
                }

                SHR_IF_ERR_EXIT(bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL));
                if (entry_hdl != BCMLT_INVALID_HDL) {
                    bcmlt_entry_free(entry_hdl);
                    entry_hdl = BCMLT_INVALID_HDL;
                }
            }
            for (q_id = 0; q_id < num_mc_q; q_id++) {
                SHR_IF_ERR_EXIT(bcmlt_entry_allocate(unit, "CTR_TM_MC_Q_DROP",
                            &entry_hdl));

                SHR_IF_ERR_EXIT
                    (bcmlt_entry_field_add(entry_hdl, "PORT_ID", port));
                SHR_IF_ERR_EXIT
                    (bcmlt_entry_field_add(entry_hdl, "BUFFER_POOL", buf_pool));
                SHR_IF_ERR_EXIT
                    (bcmlt_entry_field_add(entry_hdl, "TM_MC_Q_ID", q_id));

                rv = bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_DELETE,
                        BCMLT_PRIORITY_NORMAL);
                if (rv != SHR_E_NONE && rv != SHR_E_NOT_FOUND) {
                    SHR_ERR_EXIT(SHR_E_FAIL);
                }

                SHR_IF_ERR_EXIT(bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL));
                if (entry_hdl != BCMLT_INVALID_HDL) {
                    bcmlt_entry_free(entry_hdl);
                    entry_hdl = BCMLT_INVALID_HDL;
                }
            }
        }
    }


exit:

    SHR_FUNC_EXIT();

}

int
bcma_testutil_tm_stat_clear(int unit, int port)
{

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(tm_stat_ing_port_drop_table_stat_clear(unit, port));
    SHR_IF_ERR_EXIT(tm_stat_egr_queue_drop_table_stat_clear(unit, port));

exit:
    SHR_FUNC_EXIT();
}


int
bcma_testutil_tm_obm_pkt_drop_get(int unit, int port, uint64_t *value)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t drop_pkt = 0;

    SHR_FUNC_ENTER(unit);
    *value = 0;
    SHR_IF_ERR_EXIT(bcmlt_entry_allocate(unit, "CTR_TM_OBM_PORT_DROP",
                &entry_hdl));
    bcmlt_entry_attrib_set(entry_hdl, BCMLT_ENT_ATTR_GET_FROM_HW);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "PORT_ID", port));
    SHR_IF_ERR_EXIT(
            bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_LOOKUP,
                BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry_hdl, "LOSSLESS0_PKT", &drop_pkt));
    *value += drop_pkt;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry_hdl, "LOSSLESS1_PKT", &drop_pkt));
    *value += drop_pkt;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry_hdl, "LOSSY_HIGH_PKT", &drop_pkt));
    *value += drop_pkt;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry_hdl, "LOSSY_LOW_PKT", &drop_pkt));
    *value += drop_pkt;
    if (*value != 0) {
        cli_out("Port: %d, OBM packet drops: %"PRIu64" \n", port, *value);
    }
exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

int
bcma_testutil_tm_ing_pkt_drop_get(int unit, int port, uint64_t *value)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT(bcmlt_entry_allocate(unit, "CTR_ING_TM_PORT_UC_DROP",
                &entry_hdl));
    bcmlt_entry_attrib_set(entry_hdl, BCMLT_ENT_ATTR_GET_FROM_HW);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "PORT_ID", port));
    SHR_IF_ERR_EXIT(
            bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_LOOKUP,
                BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry_hdl, "PKT", value));
    if (*value != 0) {
        cli_out("Port: %d, MMU ingress packet drops: %"PRIu64" \n", port, *value);
    }


exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

int
bcma_testutil_tm_egr_uc_q_pkt_drop_get(int unit, int port, int queue,
                                       uint64_t *value)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t drop_pkt = 0;
    int buf_pool;

    SHR_FUNC_ENTER(unit);
    *value = 0;
    for (buf_pool = 0; buf_pool < 2; buf_pool ++) {
        if (bcmtm_itm_valid_get(unit, buf_pool) == SHR_E_NONE) {
            SHR_IF_ERR_EXIT(bcmlt_entry_allocate(unit, "CTR_TM_UC_Q_DROP",
                        &entry_hdl));
            bcmlt_entry_attrib_set(entry_hdl, BCMLT_ENT_ATTR_GET_FROM_HW);

            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry_hdl, "PORT_ID", port));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry_hdl, "BUFFER_POOL", buf_pool));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry_hdl, "TM_UC_Q_ID", queue));
            SHR_IF_ERR_EXIT(
                    bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_LOOKUP,
                        BCMLT_PRIORITY_NORMAL));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_get(entry_hdl, "PKT", &drop_pkt));
            *value += drop_pkt;
            if (drop_pkt != 0) {
                cli_out("Port: %d, Queue: %d, Buffer Pool: %d MMU UC queue drops: %"PRIu64" \n",
                        port, queue, buf_pool, drop_pkt);
            }
            if (entry_hdl != BCMLT_INVALID_HDL) {
                bcmlt_entry_free(entry_hdl);
            }
        }
    }


exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_tm_egr_mc_q_pkt_drop_get(int unit, int port, int queue,
                                       uint64_t *value)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t drop_pkt = 0;
    int buf_pool;

    SHR_FUNC_ENTER(unit);
    *value = 0;
    for (buf_pool = 0; buf_pool < 2; buf_pool ++) {
        if (bcmtm_itm_valid_get(unit, buf_pool) == SHR_E_NONE) {
            SHR_IF_ERR_EXIT(bcmlt_entry_allocate(unit, "CTR_TM_MC_Q_DROP",
                        &entry_hdl));
            bcmlt_entry_attrib_set(entry_hdl, BCMLT_ENT_ATTR_GET_FROM_HW);

            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry_hdl, "PORT_ID", port));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry_hdl, "BUFFER_POOL", buf_pool));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry_hdl, "TM_MC_Q_ID", queue));
            SHR_IF_ERR_EXIT(
                    bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_LOOKUP,
                        BCMLT_PRIORITY_NORMAL));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_get(entry_hdl, "PKT", &drop_pkt));
            *value += drop_pkt;
            if (drop_pkt != 0) {
                cli_out("Port: %d, Queue: %d, Buffer Pool: %d MMU MC queue drops: %"PRIu64" \n",
                        port, queue, buf_pool, drop_pkt);
            }
            if (entry_hdl != BCMLT_INVALID_HDL) {
                bcmlt_entry_free(entry_hdl);
            }
        }
    }


exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_tm_total_queue_pkt_drop_get(int unit, int port, uint64_t *value)
{
    uint64_t drop_pkt = 0;
    int q_id, num_mc_q, num_uc_q;

    SHR_FUNC_ENTER(unit);
    *value = 0;
    SHR_IF_ERR_EXIT(bcmtm_lport_num_mcq_get(unit, port, &num_mc_q));
    SHR_IF_ERR_EXIT(bcmtm_lport_num_ucq_get(unit, port, &num_uc_q));
    for (q_id = 0; q_id < num_uc_q; q_id++) {
        SHR_IF_ERR_EXIT(bcma_testutil_tm_egr_uc_q_pkt_drop_get(unit, port, q_id,
            &drop_pkt));
        *value += drop_pkt;
    }
    for (q_id = 0; q_id < num_mc_q; q_id++) {
        SHR_IF_ERR_EXIT(bcma_testutil_tm_egr_mc_q_pkt_drop_get(unit, port, q_id,
            &drop_pkt));
        *value += drop_pkt;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_tm_num_inject_pkst_line_rate_get(int unit, bcmdrd_pbmp_t pbmp,
                                               int length, uint32_t *cnt)
{
#define NUM_PORTS 160
    int latency_us = 2;
    int port;
    int speed;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(cnt, SHR_E_PARAM);

    BCMDRD_PBMP_ITER(pbmp, port) {
        SHR_IF_ERR_EXIT(bcmtm_lport_max_speed_get(unit, port, &speed));
        cnt[port] = (uint32_t)(((speed * latency_us) / (8 * length)) + 1);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
tm_stat_rqe_drop_stat_clear(int unit)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int buf_pool, rqe_q;
    int rv;

    SHR_FUNC_ENTER(unit);
    /* Delete any existing entry and insert new */
    for (buf_pool = 0; buf_pool < 2; buf_pool++) {
        if (bcmtm_itm_valid_get(unit, buf_pool) == SHR_E_NONE) {
            for (rqe_q = 0; rqe_q < 9; rqe_q++) {
                SHR_IF_ERR_EXIT(bcmlt_entry_allocate(unit, "CTR_TM_REPL_Q_DROP",
                            &entry_hdl));

                SHR_IF_ERR_EXIT
                    (bcmlt_entry_field_add(entry_hdl, "BUFFER_POOL", buf_pool));
                SHR_IF_ERR_EXIT
                    (bcmlt_entry_field_add(entry_hdl, "REPL_Q_NUM", rqe_q));

                rv = bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_DELETE,
                        BCMLT_PRIORITY_NORMAL);
                if (rv != SHR_E_NONE && rv != SHR_E_NOT_FOUND) {
                    SHR_ERR_EXIT(SHR_E_FAIL);
                }

                SHR_IF_ERR_EXIT(bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL));
                if (entry_hdl != BCMLT_INVALID_HDL) {
                    bcmlt_entry_free(entry_hdl);
                }
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
tm_stat_hdrm_pool_drop_stat_clear(int unit)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int buf_pool, hdrm_pool;
    int rv;

    SHR_FUNC_ENTER(unit);
    /* Delete any existing entry and insert new */
    for (buf_pool = 0; buf_pool < 2; buf_pool++) {
        if (bcmtm_itm_valid_get(unit, buf_pool) == SHR_E_NONE) {
            for (hdrm_pool = 0;hdrm_pool < 4; hdrm_pool++) {
                SHR_IF_ERR_EXIT(bcmlt_entry_allocate(unit,
                            "CTR_ING_TM_THD_HEADROOM_POOL_LOSSLESS_UC_DROP",
                            &entry_hdl));

                SHR_IF_ERR_EXIT
                    (bcmlt_entry_field_add(entry_hdl, "BUFFER_POOL", buf_pool));
                SHR_IF_ERR_EXIT
                    (bcmlt_entry_field_add(entry_hdl, "TM_HEADROOM_POOL_ID",
                                           hdrm_pool));

                rv = bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_DELETE,
                        BCMLT_PRIORITY_NORMAL);
                if (rv != SHR_E_NONE && rv != SHR_E_NOT_FOUND) {
                    SHR_ERR_EXIT(SHR_E_FAIL);
                }

                SHR_IF_ERR_EXIT(bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL));
                if (entry_hdl != BCMLT_INVALID_HDL) {
                    bcmlt_entry_free(entry_hdl);
                }
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_tm_global_drop_stat_clear(int unit)
{

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(tm_stat_rqe_drop_stat_clear(unit));
    SHR_IF_ERR_EXIT(tm_stat_hdrm_pool_drop_stat_clear(unit));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_tm_global_drop_count_check(int unit, bool *test_result)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int buf_pool, hdrm_pool, rqe_q;
    uint64_t drop_count;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(test_result, SHR_E_PARAM);
    *test_result = true;
    for (buf_pool = 0; buf_pool < 2; buf_pool++) {
        if (bcmtm_itm_valid_get(unit, buf_pool) == SHR_E_NONE) {
            /* Check RQE Queue drops */
            for (rqe_q = 0; rqe_q < 9; rqe_q++) {
                SHR_IF_ERR_EXIT(bcmlt_entry_allocate(unit, "CTR_TM_REPL_Q_DROP",
                            &entry_hdl));

                SHR_IF_ERR_EXIT
                    (bcmlt_entry_field_add(entry_hdl, "BUFFER_POOL", buf_pool));
                SHR_IF_ERR_EXIT
                    (bcmlt_entry_field_add(entry_hdl, "REPL_Q_NUM", rqe_q));

                SHR_IF_ERR_EXIT(bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_LOOKUP,
                        BCMLT_PRIORITY_NORMAL));
                SHR_IF_ERR_EXIT
                    (bcmlt_entry_field_get(entry_hdl, "PKT", &drop_count));
                if (drop_count != 0) {
                    cli_out("Buffer pool: %d Repl Queue: %d Total drops: %"PRIu64"\n",
                            buf_pool, rqe_q, drop_count);
                    *test_result = false;
                }
                SHR_IF_ERR_EXIT
                    (bcmlt_entry_field_get(entry_hdl, "RED_PKT", &drop_count));
                if (drop_count != 0) {
                    cli_out("Buffer pool: %d Repl Queue: %d Red drops: %"PRIu64"\n",
                            buf_pool, rqe_q, drop_count);
                    *test_result = false;
                }

                SHR_IF_ERR_EXIT
                    (bcmlt_entry_field_get(entry_hdl, "YELLOW_PKT", &drop_count));
                if (drop_count != 0) {
                    cli_out("Buffer pool: %d Repl Queue: %d Yellow drops: %"PRIu64"\n",
                            buf_pool, rqe_q, drop_count);
                    *test_result = false;
                }
                if (entry_hdl != BCMLT_INVALID_HDL) {
                    bcmlt_entry_free(entry_hdl);
                }
            }
            /* Headroom Pool drops */
            for (hdrm_pool = 0;hdrm_pool < 4; hdrm_pool++) {
                SHR_IF_ERR_EXIT(bcmlt_entry_allocate(unit,
                            "CTR_ING_TM_THD_HEADROOM_POOL_LOSSLESS_UC_DROP",
                            &entry_hdl));

                SHR_IF_ERR_EXIT
                    (bcmlt_entry_field_add(entry_hdl, "BUFFER_POOL", buf_pool));
                SHR_IF_ERR_EXIT
                    (bcmlt_entry_field_add(entry_hdl, "TM_HEADROOM_POOL_ID",
                                           hdrm_pool));
                SHR_IF_ERR_EXIT(bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_LOOKUP,
                        BCMLT_PRIORITY_NORMAL));
                SHR_IF_ERR_EXIT
                    (bcmlt_entry_field_get(entry_hdl, "PKT", &drop_count));
                if (drop_count != 0) {
                    cli_out("Buffer pool: %d Hdrm Pool: %d MMU drops: %"PRIu64"\n",
                            buf_pool, hdrm_pool, drop_count);
                    *test_result = false;
                }
                if (entry_hdl != BCMLT_INVALID_HDL) {
                    bcmlt_entry_free(entry_hdl);
                }
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_tm_port_meter(int unit, int port, int meter_perc)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int rv;
    int port_speed;
    uint64_t meter_rate_kbps;

    SHR_FUNC_ENTER(unit);
    /* Get port speed in Mbps */
    SHR_IF_ERR_EXIT(bcmtm_lport_max_speed_get(unit, port, &port_speed));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "TM_SHAPER_PORT",
                              &entry_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "PORT_ID", port));
    rv = bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL);
    if (rv == SHR_E_NOT_FOUND) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_INSERT,
                                BCMLT_PRIORITY_NORMAL));
    } else if (rv != SHR_E_NONE) {
        SHR_ERR_EXIT(rv);
    }
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "TM_SHAPER_PORT",
                              &entry_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "PORT_ID", port));
    meter_rate_kbps = ((uint64_t)port_speed * 10 * meter_perc);
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "BANDWIDTH_KBPS", meter_rate_kbps));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_UPDATE,
                            BCMLT_PRIORITY_NORMAL));

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

int
bcma_testutil_tm_port_meter_cleanup(int unit, int port)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int rv;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "TM_SHAPER_PORT",
                              &entry_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "PORT_ID", port));
    rv = bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL);
    if (rv == SHR_E_NOT_FOUND) {
        SHR_EXIT();
    } else if (rv != SHR_E_NONE) {
        SHR_ERR_EXIT(rv);
    }
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_DELETE,
                            BCMLT_PRIORITY_NORMAL));

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

int
bcma_testutil_tm_mmu_thd_mode_is_lossless(int unit, bool *lossless_mode) {
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t thd_mode;
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lossless_mode, SHR_E_PARAM);
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "TM_THD_CONFIG",
                              &entry_hdl));
    SHR_IF_ERR_EXIT(bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_LOOKUP,
                                       BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry_hdl, "THRESHOLD_MODE", &thd_mode));
    if (thd_mode == 0) {
        /* Lossy mode. */
        *lossless_mode = false;
    } else {
        *lossless_mode = true;
    }
exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}


int
bcmtm_util_check_port_ep_mmu_credit_loop(int unit, int *err_count)
{
    int lport, pport;
    uint64_t link = 0;
    int rv;

    SHR_FUNC_ENTER(unit);
    for (pport = 0; pport < MAX_PHYS_PORTS; pport++) {
        rv = bcmtm_pport_lport_get(unit, pport, &lport);
        if (rv != SHR_E_NONE && rv != SHR_E_NOT_FOUND) {
            SHR_ERR_EXIT(rv);
        }
        if (rv == SHR_E_NONE) {
            /* Check link for valid ports */
            if (bcmtm_lport_is_fp(unit, lport) || bcmtm_lport_is_mgmt(unit, lport)) {
                SHR_IF_ERR_EXIT(bcma_testutil_port_link_status_get(unit, lport,
                    &link));
                if (link) {
                    *err_count += bcmtm_check_ep_mmu_credit(unit, lport);
                    *err_count += bcmtm_check_port_ep_credit(unit, lport);
                }
            } else {
                /* CPU and internal LB ports */
                *err_count += bcmtm_check_ep_mmu_credit(unit, lport);
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}
