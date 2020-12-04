/*! \file bcma_testutil_xgs_qos.c
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
xgs_qos_pri_cng_one_to_one_map_create(int unit, bcmdrd_pbmp_t pbmp)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int remap_id = 0, phb_ing_l2_id = 1, map_id = 2;
    int pri, cfi;
    int port;
    int int_pri;
    int num_mc_q = 4, num_uc_q = 8;

    SHR_FUNC_ENTER(unit);

    /* Retrieve number of UC/MC queues using any port */
    BCMDRD_PBMP_ITER(pbmp, port) {
        SHR_IF_ERR_EXIT
            (bcmtm_lport_num_mcq_get(unit, port, &num_mc_q));
        SHR_IF_ERR_EXIT
            (bcmtm_lport_num_ucq_get(unit, port, &num_uc_q));
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

    /* XGS VLAN setup should insert entry already */
    BCMDRD_PBMP_ITER(pbmp, port) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(unit, "PORT_POLICY", &entry_hdl));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, "PORT_ID", port));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, "PORT_COS_Q_MAP_ID", map_id));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
        bcmlt_entry_free(entry_hdl);
    }

    /* ingress map PCP(3 bits) + DEI(1 bit) to internal priority */
    for (pri = 0; pri < 8; pri++) {
        for (cfi = 0; cfi < 2; cfi++) {
            SHR_IF_ERR_EXIT
                (bcmlt_entry_allocate(unit, "PHB_ING_L2_OTAG_REMAP", &entry_hdl));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry_hdl, "PHB_ING_L2_OTAG_REMAP_ID", remap_id));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry_hdl, "CFI", cfi));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry_hdl, "PRI", pri));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry_hdl, "NEW_CFI", cfi));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry_hdl, "NEW_PRI", pri));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
            bcmlt_entry_free(entry_hdl);

            SHR_IF_ERR_EXIT
                (bcmlt_entry_allocate(unit, "PHB_ING_L2_TAGGED_TO_INT_PRI", &entry_hdl));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry_hdl, "PHB_ING_L2_TAGGED_TO_INT_PRI_ID", phb_ing_l2_id));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry_hdl, "PRI", pri));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry_hdl, "CFI", cfi));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry_hdl, "INT_PRI", (cfi << 3) + pri));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry_hdl, "CNG", 1));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
            bcmlt_entry_free(entry_hdl);
        }
    }

    BCMDRD_PBMP_ITER(pbmp, port) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(unit, "PORT_PHB", &entry_hdl));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, "PORT_ID", port));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, "PHB_ING_L2_ID", phb_ing_l2_id));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, "PHB_ING_L2_OTAG_REMAP_ID", remap_id));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
        bcmlt_entry_free(entry_hdl);
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
xgs_qos_pri_cng_one_to_one_map_destroy(int unit, bcmdrd_pbmp_t pbmp)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int remap_id = 0, phb_ing_l2_id = 1, map_id = 2;
    int pri, cfi;
    int port;

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

    /* XGS VLAN destory should remove the entry */
    BCMDRD_PBMP_ITER(pbmp, port) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(unit, "PORT_POLICY", &entry_hdl));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, "PORT_ID", port));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, "PORT_COS_Q_MAP_ID", 0));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
        bcmlt_entry_free(entry_hdl);
    }


    /* ingress map PCP(3 bits) + DEI(1 bit) to internal priority */
    for (pri = 0; pri < 8; pri++) {
        for (cfi = 0; cfi < 2; cfi++) {
            SHR_IF_ERR_EXIT
                (bcmlt_entry_allocate(unit, "PHB_ING_L2_OTAG_REMAP", &entry_hdl));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry_hdl, "PHB_ING_L2_OTAG_REMAP_ID", remap_id));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry_hdl, "CFI", cfi));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry_hdl, "PRI", pri));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL));
            bcmlt_entry_free(entry_hdl);

            SHR_IF_ERR_EXIT
                (bcmlt_entry_allocate(unit, "PHB_ING_L2_TAGGED_TO_INT_PRI", &entry_hdl));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry_hdl, "PHB_ING_L2_TAGGED_TO_INT_PRI_ID", phb_ing_l2_id));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry_hdl, "PRI", pri));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry_hdl, "CFI", cfi));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL));
            bcmlt_entry_free(entry_hdl);
        }
    }

    BCMDRD_PBMP_ITER(pbmp, port) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(unit, "PORT_PHB", &entry_hdl));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, "PORT_ID", port));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL));
        bcmlt_entry_free(entry_hdl);
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}


static bcma_testutil_qos_lt_op_t bcma_testutil_xgs_qos_lt_op = {
    .qos_one_to_one_map_create = xgs_qos_pri_cng_one_to_one_map_create,
    .qos_one_to_one_map_destroy = xgs_qos_pri_cng_one_to_one_map_destroy
};

bcma_testutil_qos_lt_op_t *bcma_testutil_xgs_qos_lt_op_get(int unit)
{
    return &bcma_testutil_xgs_qos_lt_op;
}
