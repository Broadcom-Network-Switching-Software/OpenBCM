/*! \file bcma_testutil_xgs_l2.c
 *
 * L2 operation utility.
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
#include <bcma/test/util/bcma_testutil_l2.h>

#define BSL_LOG_MODULE BSL_LS_APPL_TEST

#define ENET_MAC_SIZE 6

static int
l2_forward_table_entry_handle(int unit, int port, const shr_mac_t mac, int vid,
                              int copy2cpu, bcmlt_opcode_t opcode)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t mac64 = 0, tmp;
    int i;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < ENET_MAC_SIZE; i++) {
        tmp = mac[i];
        mac64 <<= 8;
        mac64 |= tmp;
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "L2_FDB_VLAN", &entry_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "VLAN_ID", vid));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "MAC_ADDR", mac64));
    if (opcode == BCMLT_OPCODE_INSERT || opcode == BCMLT_OPCODE_UPDATE) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, "MODPORT", port));
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmlt_entry_field_add(entry_hdl, "MODID", 0), SHR_E_NOT_FOUND);
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, "COPY_TO_CPU", copy2cpu));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, "STATIC", 1));
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl, opcode, BCMLT_PRIORITY_NORMAL));

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "l2 add port (%d) .\n"),
              (int)port));

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

static int
xgs_l2_unicast_add(int unit, int port, const shr_mac_t mac, int vid,
                   int copy2cpu)
{
    int  rv;

    SHR_FUNC_ENTER(unit);

    rv = l2_forward_table_entry_handle
            (unit, port, mac, vid, copy2cpu, BCMLT_OPCODE_INSERT);
    if (rv != SHR_E_NONE && rv != SHR_E_EXISTS) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
xgs_l2_unicast_update(int unit, int port, const shr_mac_t mac, int vid,
                      int copy2cpu)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (l2_forward_table_entry_handle
            (unit, port, mac, vid, copy2cpu, BCMLT_OPCODE_UPDATE));

exit:
    SHR_FUNC_EXIT();
}

static int
xgs_l2_unicast_delete(int unit, int port, const shr_mac_t mac, int vid,
                      int copy2cpu)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (l2_forward_table_entry_handle
            (unit, port, mac, vid, copy2cpu, BCMLT_OPCODE_DELETE));

exit:
    SHR_FUNC_EXIT();
}

static int
xgs_l2_multicast_add(int unit, int group_id, bcmdrd_pbmp_t pbmp,
                     const shr_mac_t mac, int vid, int copy2cpu)
{
    bcmlt_entry_handle_t entry_hdl_1 = BCMLT_INVALID_HDL;
    bcmlt_entry_handle_t entry_hdl_2 = BCMLT_INVALID_HDL;
    uint64_t mac_addr = 0, tmp;
    int idx;
    const char *l2mc_tbl_name = "L2_MC_GROUP";
    const char *l2mc_fields_portid_name = "PORT_ID";
    uint32_t l2mc_fields_num, l2mc_fields_actual_num, idx_u, port;
    bcmlt_field_def_t *l2mc_fields_array = NULL;
    bcmlt_field_def_t *l2mc_fields_portid = NULL;
    uint32_t portid_num;
    uint64_t *portid_data = NULL;

    SHR_FUNC_ENTER(unit);

    for (idx = 0; idx < 6; idx++) {
        tmp = mac[idx];
        mac_addr <<= 8;
        mac_addr |= tmp;
    }

    /* Get the information of L2_MC_GROUP's PORT_ID */
    SHR_IF_ERR_EXIT
        (bcmlt_table_field_defs_get
            (unit, l2mc_tbl_name, 0, NULL, &l2mc_fields_num));

    SHR_ALLOC(l2mc_fields_array, l2mc_fields_num * sizeof(bcmlt_field_def_t),
              "bcmaTestUtilL2McFieldsArray");
    SHR_NULL_CHECK(l2mc_fields_array, SHR_E_MEMORY);

    SHR_IF_ERR_EXIT
        (bcmlt_table_field_defs_get
            (unit, l2mc_tbl_name, l2mc_fields_num,
             l2mc_fields_array, &l2mc_fields_actual_num));

    if (l2mc_fields_num != l2mc_fields_actual_num) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    for (idx_u = 0; idx_u < l2mc_fields_num; idx_u++) {
        if (sal_strcmp(l2mc_fields_array[idx_u].name,
                       l2mc_fields_portid_name) == 0) {
            l2mc_fields_portid = &(l2mc_fields_array[idx_u]);
            portid_num = l2mc_fields_portid->elements;
            break;
        }
    }

    if (l2mc_fields_portid == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_ALLOC(portid_data, portid_num * sizeof(uint64_t),
              "bcmaTestUtilL2McPortIdData");
    SHR_NULL_CHECK(portid_data, SHR_E_MEMORY);
    sal_memset(portid_data, 0, portid_num * sizeof(uint64_t));

    /* Insert L2_MC_GROUP */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, l2mc_tbl_name, &entry_hdl_1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl_1, "L2_MC_GROUP_ID", group_id));

    BCMDRD_PBMP_ITER(pbmp, port) {
        if (port >= portid_num) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        portid_data[port] = 1;
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add
            (entry_hdl_1, l2mc_fields_portid_name, 0,
             portid_data, l2mc_fields_portid->elements));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl_1, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL));

    /* Insert L2_FDB_VLAN */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "L2_FDB_VLAN", &entry_hdl_2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl_2, "VLAN_ID", vid));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl_2, "MAC_ADDR", mac_addr));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl_2, "DEST_TYPE", "L2_MC_GRP"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl_2, "L2_MC_GRP_ID", group_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl_2, "STATIC", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl_2, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL));

exit:
    if (entry_hdl_1 != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl_1);
    }
    if (entry_hdl_2 != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl_2);
    }
    SHR_FREE(l2mc_fields_array);
    SHR_FREE(portid_data);

    SHR_FUNC_EXIT();
}

static int
xgs_l2_multicast_delete(int unit, int group_id, bcmdrd_pbmp_t pbmp,
                        const shr_mac_t mac, int vid, int copy2cpu)
{
    bcmlt_entry_handle_t entry_hdl_1 = BCMLT_INVALID_HDL;
    bcmlt_entry_handle_t entry_hdl_2 = BCMLT_INVALID_HDL;
    uint64_t mac_addr = 0, tmp;
    int idx;

    SHR_FUNC_ENTER(unit);

    for (idx = 0; idx < 6; idx++) {
        tmp = mac[idx];
        mac_addr <<= 8;
        mac_addr |= tmp;
    }

    /* Delete L2_MC_GROUP */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "L2_MC_GROUP", &entry_hdl_1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl_1, "L2_MC_GROUP_ID", group_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl_1, BCMLT_OPCODE_DELETE,
                            BCMLT_PRIORITY_NORMAL));

    /* Delete L2_FDB_VLAN */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "L2_FDB_VLAN", &entry_hdl_2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl_2, "VLAN_ID", vid));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl_2, "MAC_ADDR", mac_addr));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl_2, BCMLT_OPCODE_DELETE,
                            BCMLT_PRIORITY_NORMAL));

exit:
    if (entry_hdl_1 != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl_1);
    }
    if (entry_hdl_2 != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl_2);
    }

    SHR_FUNC_EXIT();
}

static int
xgs_l2_multicast_update(int unit, int group_id, bcmdrd_pbmp_t pbmp,
                        const shr_mac_t mac, int vid, int copy2cpu)
{
    bcmlt_entry_handle_t entry = BCMLT_INVALID_HDL;
    uint32_t l2mc_fields_num, l2mc_fields_actual_num, idx_u, port;
    bcmlt_field_def_t *l2mc_fields_array = NULL;
    bcmlt_field_def_t *l2mc_fields_portid = NULL;
    uint32_t portid_num;
    uint64_t *portid_data = NULL;

    SHR_FUNC_ENTER(unit);

    /* Get the information of L2_MC_GROUP's PORT_ID */
    SHR_IF_ERR_EXIT
        (bcmlt_table_field_defs_get
            (unit, "L2_MC_GROUP", 0, NULL, &l2mc_fields_num));

    SHR_ALLOC(l2mc_fields_array, l2mc_fields_num * sizeof(bcmlt_field_def_t),
              "bcmaTestUtilL2McFieldsArray");
    SHR_NULL_CHECK(l2mc_fields_array, SHR_E_MEMORY);

    SHR_IF_ERR_EXIT
        (bcmlt_table_field_defs_get
            (unit, "L2_MC_GROUP", l2mc_fields_num,
             l2mc_fields_array, &l2mc_fields_actual_num));

    if (l2mc_fields_num != l2mc_fields_actual_num) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    for (idx_u = 0; idx_u < l2mc_fields_num; idx_u++) {
        if (sal_strcmp(l2mc_fields_array[idx_u].name, "PORT_ID") == 0) {
            l2mc_fields_portid = &(l2mc_fields_array[idx_u]);
            portid_num = l2mc_fields_portid->elements;
            break;
        }
    }

    if (l2mc_fields_portid == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_ALLOC(portid_data, portid_num * sizeof(uint64_t),
              "bcmaTestUtilL2McPortIdData");
    SHR_NULL_CHECK(portid_data, SHR_E_MEMORY);
    sal_memset(portid_data, 0, portid_num * sizeof(uint64_t));

    /* Insert L2_MC_GROUP */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "L2_MC_GROUP", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "L2_MC_GROUP_ID", group_id));

    BCMDRD_PBMP_ITER(pbmp, port) {
        if (port >= portid_num) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        portid_data[port] = 1;
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add
            (entry, "PORT_ID", 0,
             portid_data, l2mc_fields_portid->elements));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));

exit:
    if (entry != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry);
    }
    SHR_FREE(l2mc_fields_array);
    SHR_FREE(portid_data);

    SHR_FUNC_EXIT();
}

static bcma_testutil_l2_op_t bcma_testutil_xgs_l2_op = {
    .unicast_add = xgs_l2_unicast_add,
    .unicast_update = xgs_l2_unicast_update,
    .unicast_delete = xgs_l2_unicast_delete,
    .multicast_add = xgs_l2_multicast_add,
    .multicast_delete = xgs_l2_multicast_delete,
    .multicast_update = xgs_l2_multicast_update
};

bcma_testutil_l2_op_t *bcma_testutil_xgs_l2_op_get(int unit)
{
    return &bcma_testutil_xgs_l2_op;
}
