/*! \file bcma_testutil_xfs_flex.c
 *
 * Flexport operation utility.
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
#include <bcma/test/util/bcma_testutil_flex.h>

#define BSL_LOG_MODULE BSL_LS_APPL_TEST

static int
xfs_flex_pc_port_add(int unit, uint32_t lport, uint32_t speed,
                     uint8_t num_lanes, uint32_t max_frame_size,
                     char *fec_mode, char *speed_vco)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(fec_mode, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(bcmlt_entry_allocate(unit, "PC_PORT", &entry_hdl));

    cli_out("flex port up: [PC_PORT] lport = %3d, speed = %0dG, \
             num_lanes = %0d,  fec_mode = %s, speed_vco = %s, \
             max_frame_size = %0d \n",
            lport, speed/1000, num_lanes, fec_mode, speed_vco,
            max_frame_size);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "ENABLE", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "PORT_ID", lport));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "SPEED", speed));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "NUM_LANES", num_lanes));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "MAX_FRAME_SIZE", max_frame_size));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, "FEC_MODE", fec_mode));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, "SPEED_VCO_FREQ", speed_vco));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit
            (entry_hdl, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
xfs_flex_pc_port_delete(int unit, uint32_t lport)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int lv;

    SHR_FUNC_ENTER(unit);

    cli_out("flex port down: [PC_PORT] lport = %3d\n", lport);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "LM_PORT_CONTROL", &entry_hdl));
     bcmlt_entry_clear(entry_hdl);
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "PORT_ID", lport));
    lv = (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));
    cli_out(" LM_PORT_CONTROL lookup val=%0d \n", lv);
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "PORT_ID", lport));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, "LINKSCAN_MODE","SOFTWARE"));
   if (lv == SHR_E_NOT_FOUND)
   SHR_IF_ERR_EXIT
       (bcmlt_entry_commit
           (entry_hdl, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));

     bcmlt_entry_free(entry_hdl);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "PC_PORT", &entry_hdl));
     bcmlt_entry_clear(entry_hdl);
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "PORT_ID", lport));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "ENABLE", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit
            (entry_hdl, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
     bcmlt_entry_free(entry_hdl);


    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "LM_PORT_CONTROL", &entry_hdl));
     bcmlt_entry_clear(entry_hdl);
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "PORT_ID", lport));
    lv = (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "PORT_ID", lport));
    cli_out(" LM_PORT_CONTROL lookup val=%0d\n", lv);
    if (lv != SHR_E_NOT_FOUND)
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit
            (entry_hdl, BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL));
     bcmlt_entry_free(entry_hdl);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "PC_PORT", &entry_hdl));
     bcmlt_entry_clear(entry_hdl);
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "PORT_ID", lport));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "ENABLE", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit
            (entry_hdl, BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL));

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
xfs_flex_pc_port_phys_map_add(int unit, uint32_t lport, uint32_t pport)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    cli_out("flex port up: [PC_PORT_PHYS_MAP] lport = %3d, pport = %3d\n",
            lport, pport);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "PC_PORT_PHYS_MAP", &entry_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "PORT_ID", lport));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "PC_PHYS_PORT_ID", pport));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit
            (entry_hdl, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
xfs_flex_pc_port_phys_map_delete(int unit, uint32_t lport)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    cli_out("flex port down: [PC_PORT_PHYS_MAP] lport = %3d\n", lport);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "PC_PORT_PHYS_MAP", &entry_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "PORT_ID", lport));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit
            (entry_hdl, BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL));

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
xfs_flex_l2p_map_get(int unit, bcmdrd_pbmp_t pbmp, uint32_t array_size,
                     uint32_t *l2p_map)
{
    bcmlt_entry_handle_t entry = BCMLT_INVALID_HDL;
    uint64_t fval;
    uint32_t pport, lport;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(l2p_map, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "PC_PORT_PHYS_MAP", &entry));

    BCMDRD_PBMP_ITER(pbmp, lport) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry, "PORT_ID", lport));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit
                (entry, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(entry, "PC_PHYS_PORT_ID", &fval));

        pport = (uint32_t) fval;

        if (lport < array_size) {
            l2p_map[lport] = pport;
        }
    }

exit:
    if (entry != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry);
    }
    SHR_FUNC_EXIT();
}

static bcma_testutil_flex_lt_op_t bcma_testutil_xfs_flex_lt_op = {
    .flex_pc_port_add = xfs_flex_pc_port_add,
    .flex_pc_port_delete = xfs_flex_pc_port_delete,
    .flex_pc_port_phys_map_add = xfs_flex_pc_port_phys_map_add,
    .flex_pc_port_phys_map_delete = xfs_flex_pc_port_phys_map_delete,
    .flex_l2p_map_get = xfs_flex_l2p_map_get
};

bcma_testutil_flex_lt_op_t *bcma_testutil_xfs_flex_lt_op_get(int unit)
{
    return &bcma_testutil_xfs_flex_lt_op;
}
