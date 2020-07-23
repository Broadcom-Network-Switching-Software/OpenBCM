/*! \file bcma_testutil_ctr_port.c
 *
 * Per-port counter operation utility.
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
#include <bcmtm/bcmtm_utils.h>

#define BSL_LOG_MODULE  BSL_LS_APPL_TEST

#define BITS_QUEUE      4
#define BITS_PORT       9

static int
port_ctr_evict_add(int unit, void *user_data)
{
    bcmlt_entry_handle_t entry = BCMLT_INVALID_HDL;
    uint32_t egr_port;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(user_data, SHR_E_PARAM);

    egr_port = *((uint32_t *)user_data);

    /* Stop counter collection in CCI */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_CONTROL", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "COLLECTION_ENABLE", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    /* Add entry for pool control  */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_EGR_FLEX_POOL_CONTROL", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_EGR_FLEX_POOL_ID", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "EVICTION_MODE", "THRESHOLD"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    /* Add entry for action profile */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_EGR_EFLEX_ACTION_PROFILE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_EGR_EFLEX_ACTION_PROFILE_ID", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ACTION", 2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "POOL_ID", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "NUM_COUNTERS", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "INDEX_CTR_EGR_EFLEX_OPERAND_PROFILE_ID", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "VAL_A_CTR_EGR_EFLEX_OPERAND_PROFILE_ID", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "VAL_B_CTR_EGR_EFLEX_OPERAND_PROFILE_ID", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "MODE", "NORMAL"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_A", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_B", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "CTR_SRC_B", "USE_PKTLEN"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    /* Add entry for operand profile */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_EGR_EFLEX_OPERAND_PROFILE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_EGR_EFLEX_OPERAND_PROFILE_ID", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "PROFILE", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "OBJ_1", "USE_1"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "OBJ_2", "USE_1"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "MASK_SIZE_1", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "MASK_SIZE_2", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);


    /* Link counter action to egress port */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "PORT", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "PORT_ID", egr_port));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_EGR_EFLEX_ACTION", 2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));

exit:
    if (entry != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry);
    }

    SHR_FUNC_EXIT();
}


static int
port_ctr_evict_delete(int unit, void *user_data)
{
    bcmlt_entry_handle_t entry = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_EGR_FLEX_POOL_CONTROL", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_EGR_FLEX_POOL_ID", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_EGR_EFLEX_ACTION_PROFILE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_EGR_EFLEX_ACTION_PROFILE_ID", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_EGR_EFLEX_OPERAND_PROFILE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_EGR_EFLEX_OPERAND_PROFILE_ID", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

exit:
    if (entry != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry);
    }

    SHR_FUNC_EXIT();
}

static int
port_ctr_egr_perq_stat_create(int unit, bcmdrd_pbmp_t pbmp)
{
    bcmlt_entry_handle_t entry = BCMLT_INVALID_HDL;
    int action_profile = 1, oper_profile = 1;
    int action = 2, profile = 1;
    int port, ctr_num;
    const char *obj_name[2] = {"QUEUE_NUM", "EGR_PORT"};

    SHR_FUNC_ENTER(unit);

    ctr_num = (1 << BITS_QUEUE) * (1 << BITS_PORT);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_EGR_EFLEX_ACTION_PROFILE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_EGR_EFLEX_ACTION_PROFILE_ID", action_profile));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ACTION", action));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "POOL_ID", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "NUM_COUNTERS", ctr_num));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_symbol_add(entry, "OBJ", 0, obj_name, 2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "INDEX_CTR_EGR_EFLEX_OPERAND_PROFILE_ID", oper_profile));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "VAL_A_CTR_EGR_EFLEX_OPERAND_PROFILE_ID", oper_profile));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "VAL_B_CTR_EGR_EFLEX_OPERAND_PROFILE_ID", oper_profile));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "MODE", "NORMAL"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_A", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_B", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "CTR_SRC_B", "USE_PKTLEN"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_EGR_EFLEX_OPERAND_PROFILE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_EGR_EFLEX_OPERAND_PROFILE_ID", oper_profile));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "PROFILE", profile));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "OBJ_1", "USE_OBJ_1"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "OBJ_2", "USE_OBJ_2"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "MASK_SIZE_1", BITS_QUEUE - 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "MASK_SIZE_2", BITS_PORT - 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "SHIFT_1", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "SHIFT_2", BITS_QUEUE));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    BCMDRD_PBMP_ITER(pbmp, port) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(unit, "PORT", &entry));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry, "PORT_ID", port));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry, "CTR_EGR_EFLEX_ACTION", action));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(entry, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
        bcmlt_entry_free(entry);
    }

exit:
    if (entry != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry);
    }

    SHR_FUNC_EXIT();
}

static int
port_ctr_egr_perq_stat_destroy(int unit, bcmdrd_pbmp_t pbmp)
{
    bcmlt_entry_handle_t entry = BCMLT_INVALID_HDL;
    int action_profile = 1, oper_profile = 1;
    int action = 0;
    int port;

    SHR_FUNC_ENTER(unit);

    BCMDRD_PBMP_ITER(pbmp, port) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(unit, "PORT", &entry));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry, "PORT_ID", port));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry, "CTR_EGR_EFLEX_ACTION", action));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(entry, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
        bcmlt_entry_free(entry);
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_EGR_EFLEX_ACTION_PROFILE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_EGR_EFLEX_ACTION_PROFILE_ID", action_profile));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_EGR_EFLEX_OPERAND_PROFILE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_EGR_EFLEX_OPERAND_PROFILE_ID", oper_profile));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

exit:
    if (entry != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry);
    }

    SHR_FUNC_EXIT();
}


static int
port_ctr_egr_perq_stat_get(int unit, int port,
                           int q_id, traffic_queue_type_t q_type,
                           bool pkt_counter, uint64_t *count)
{
    bcmlt_entry_handle_t entry = BCMLT_INVALID_HDL;
    int action_profile = 1;
    int num_mc_q, num_uc_q, ctr_idx;
    int is_mc;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(count, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmtm_lport_num_ucq_get(unit, port, &num_uc_q));
    SHR_IF_ERR_EXIT
        (bcmtm_lport_num_mcq_get(unit, port, &num_mc_q));

    if (q_type == TRAFFIC_Q_TYPE_UC) {
        is_mc = 0;
        if (q_id >= num_uc_q) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    } else if (q_type == TRAFFIC_Q_TYPE_MC) {
        is_mc = 1;
        if (q_id >= num_mc_q) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    ctr_idx = (port << BITS_QUEUE) + num_uc_q * is_mc + q_id;

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_EGR_EFLEX_STATS", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_EGR_EFLEX_ACTION_PROFILE_ID", action_profile));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_EFLEX_INDEX", ctr_idx));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_attrib_set(entry, BCMLT_ENT_ATTR_GET_FROM_HW));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));
    if (pkt_counter) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(entry, "CTR_A_LOWER", count));
    } else {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(entry, "CTR_B", count));
    }

exit:
    if (entry != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry);
    }

    SHR_FUNC_EXIT();
}

static int
port_ctr_egr_perq_stat_clear(int unit, int port,
                             int q_id, traffic_queue_type_t q_type)
{
    bcmlt_entry_handle_t entry = BCMLT_INVALID_HDL;
    int action_profile = 1;
    int num_mc_q, num_uc_q, ctr_idx;
    int is_mc;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtm_lport_num_ucq_get(unit, port, &num_uc_q));
    SHR_IF_ERR_EXIT
        (bcmtm_lport_num_mcq_get(unit, port, &num_mc_q));

    if (q_type == TRAFFIC_Q_TYPE_UC) {
        is_mc = 0;
        if (q_id >= num_uc_q) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    } else if (q_type == TRAFFIC_Q_TYPE_MC) {
        is_mc = 1;
        if (q_id >= num_mc_q) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    ctr_idx = (port << BITS_QUEUE) + num_uc_q * is_mc + q_id;

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_EGR_EFLEX_STATS", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_EGR_EFLEX_ACTION_PROFILE_ID", action_profile));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_EFLEX_INDEX", ctr_idx));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_A_LOWER", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_B", 0));
     SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));

exit:
    if (entry != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry);
    }

    SHR_FUNC_EXIT();
}


static bcma_testutil_ctr_op_t bcma_testutil_port_ctr_op = {
    .evict_add = port_ctr_evict_add,
    .evict_delete = port_ctr_evict_delete,
    .egr_perq_stat_create = port_ctr_egr_perq_stat_create,
    .egr_perq_stat_destroy = port_ctr_egr_perq_stat_destroy,
    .egr_perq_stat_get = port_ctr_egr_perq_stat_get,
    .egr_perq_stat_clear = port_ctr_egr_perq_stat_clear,
};

bcma_testutil_ctr_op_t *bcma_testutil_port_ctr_op_get(int unit)
{
    return &bcma_testutil_port_ctr_op;
}
