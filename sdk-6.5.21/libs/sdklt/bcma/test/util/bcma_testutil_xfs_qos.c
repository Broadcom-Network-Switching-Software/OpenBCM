/*! \file bcma_testutil_xfs_qos.c
 *
 * Qos operation utility.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <shr/shr_types.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmlt/bcmlt.h>
#include <bcma/test/util/bcma_testutil_qos.h>
#include <bcmltd/chip/bcmltd_str.h>
#include <bcmtm/bcmtm_utils.h>

#define BSL_LOG_MODULE BSL_LS_APPL_TEST

static int
xfs_qos_pri_cng_one_to_one_map_create(int unit, bcmdrd_pbmp_t pbmp)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int pri, cng, index;
    int ing_profile = 0;
    int port;
    int rv;
    int int_pri;
    int num_mc_q = 4, num_uc_q = 8;
    int map_id = 0;

    SHR_FUNC_ENTER(unit);

    /* Retrieve number of UC/MC queues using any port */
    BCMDRD_PBMP_ITER(pbmp, port) {
        SHR_IF_ERR_EXIT(bcmtm_lport_num_mcq_get(unit, port, &num_mc_q));
        SHR_IF_ERR_EXIT(bcmtm_lport_num_ucq_get(unit, port, &num_uc_q));
        break;
    }

    /* ingress internal priority to cos queue */
    for (int_pri = 0; int_pri < 16; int_pri++) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(unit, "PORT_COS_Q_MAP", &entry_hdl));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, "PORT_COS_Q_MAP_ID", map_id));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, "INT_PRI", int_pri));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, "UC_COS", int_pri % num_uc_q));
        if (num_mc_q != 0) {
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry_hdl, "MC_COS", int_pri % num_mc_q));
        }
        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
        bcmlt_entry_free(entry_hdl);
    }

    BCMDRD_PBMP_ITER(pbmp, port) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(unit, "PORT_POLICY", &entry_hdl));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, "PORT_ID", port));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, "PORT_COS_Q_MAP_ID", map_id));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
        bcmlt_entry_free(entry_hdl);
    }


    /* update PORT_COS_Q_STRENGTH_PROFILE table entry */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "PORT_COS_Q_STRENGTH_PROFILE", &entry_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "PORT_COS_Q_STRENGTH_PROFILE_ID", 0));
    rv = bcmlt_entry_commit(entry_hdl,
                            BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL);
    if (rv == SHR_E_NOT_FOUND) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(entry_hdl,
                                BCMLT_OPCODE_INSERT,
                                BCMLT_PRIORITY_NORMAL));
    }
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "UC_COS_STRENGTH", 7));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "MC_COS_STRENGTH", 7));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl,
                            BCMLT_OPCODE_UPDATE,
                            BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry_hdl);

    /* insert FLEX_QOS_ING_PHB_INT_PRI_STRENGTH_PROFILE table entry*/
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "FLEX_QOS_ING_PHB_INT_PRI_STRENGTH_PROFILE",
                              &entry_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl,
                               "FLEX_QOS_ING_PHB_INT_PRI_STRENGTH_PROFILE_ID",
                               0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "MAPPED_VALUE_STRENGTH", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl,
                            BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry_hdl);


    /* Ing entries to map pkt_pri, cng to int_pri */
    for (pri = 0; pri < 8; pri++) {
        for (cng = 0; cng < 2; cng++) {
            /* BASE_INDEX: (profile << 4) + (pri << 1) + cng */
            /* ING is configured to use profile 0 */
            index = (ing_profile << 4) + (pri << 1) + cng;
            SHR_IF_ERR_EXIT
                (bcmlt_entry_allocate(unit, "FLEX_QOS_ING_PHB", &entry_hdl));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry_hdl, "BASE_INDEX", index));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry_hdl, "INT_PRI", (cng << 3) + pri));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_commit(entry_hdl,
                                    BCMLT_OPCODE_INSERT,
                                    BCMLT_PRIORITY_NORMAL));
            bcmlt_entry_free(entry_hdl);
        }
    }
    BCMDRD_PBMP_ITER(pbmp, port) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(unit,
                                  "ING_L2_IIF_ATTRIBUTES_TABLE",
                                  &entry_hdl));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, "L2_IIF", port));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, "USE_OTAG_DOT1P_CFI_FOR_PHB", 1));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(entry_hdl,
                                BCMLT_OPCODE_UPDATE,
                                BCMLT_PRIORITY_NORMAL));
        bcmlt_entry_free(entry_hdl);
    }

 exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
xfs_qos_pri_cng_one_to_one_map_destroy(int unit, bcmdrd_pbmp_t pbmp)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int pri, cng, index;
    int ing_profile = 0;
    int port;
    int map_id = 0;

    SHR_FUNC_ENTER(unit);

    for (pri = 0; pri < 16; pri++) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(unit, "PORT_COS_Q_MAP", &entry_hdl));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, "PORT_COS_Q_MAP_ID", map_id));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, "INT_PRI", pri));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL));
        bcmlt_entry_free(entry_hdl);
    }

    BCMDRD_PBMP_ITER(pbmp, port) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(unit, "PORT_POLICY", &entry_hdl));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, "PORT_ID", port));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL));
        bcmlt_entry_free(entry_hdl);
    }


    /* Delete FLEX_QOS_ING_PHB_INT_PRI_STRENGTH_PROFILE table entry*/
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "FLEX_QOS_ING_PHB_INT_PRI_STRENGTH_PROFILE",
                              &entry_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl,
                               "FLEX_QOS_ING_PHB_INT_PRI_STRENGTH_PROFILE_ID",
                               0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl,
                            BCMLT_OPCODE_DELETE,
                            BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry_hdl);

    for (pri = 0; pri < 8; pri++) {
        for (cng = 0; cng < 2; cng++) {
            /* BASE_INDEX: (profile << 4) + (pri << 1) + cng */
            index = (ing_profile << 4) + (pri << 1) + cng;
            SHR_IF_ERR_EXIT
                (bcmlt_entry_allocate(unit, "FLEX_QOS_ING_PHB", &entry_hdl));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry_hdl, "BASE_INDEX", index));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_commit(entry_hdl,
                                    BCMLT_OPCODE_DELETE,
                                    BCMLT_PRIORITY_NORMAL));
            bcmlt_entry_free(entry_hdl);
        }
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}


static bcma_testutil_qos_lt_op_t bcma_testutil_xfs_qos_lt_op = {
    .qos_one_to_one_map_create = xfs_qos_pri_cng_one_to_one_map_create,
    .qos_one_to_one_map_destroy = xfs_qos_pri_cng_one_to_one_map_destroy
};

bcma_testutil_qos_lt_op_t *bcma_testutil_xfs_qos_lt_op_get(int unit)
{
    return &bcma_testutil_xfs_qos_lt_op;
}
