/*! \file bcma_testutil_xgs_vlan.c
 *
 * VLAN operation utility.
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
#include <bcma/bcmlt/bcma_bcmlt.h>
#include <bcma/test/util/bcma_testutil_common.h>
#include <bcma/test/util/bcma_testutil_vlan.h>

#define BSL_LOG_MODULE BSL_LS_APPL_TEST

static shr_enum_map_t bcma_testutil_stp_state_name_id_map[] = {
    {"DISABLE", BCMA_TESTUTIL_STP_DISALBE},
    {"BLOCK", BCMA_TESTUTIL_STP_BLOCK},
    {"LEARN", BCMA_TESTUTIL_STP_LEARN},
    {"FORWARD", BCMA_TESTUTIL_STP_FORWARD}
};

static shr_enum_map_t bcma_testutil_tag_action_name_id_map[] = {
    {"DO_NOT_MODIFY", BCMA_TESTUTIL_VLAN_TAG_ACTION_DO_NOT_MODIFY},
    {"ADD", BCMA_TESTUTIL_VLAN_TAG_ACTION_ADD},
};

static int
table_entry_handle(int unit, const char *table, const char *key_name, uint64_t key_val, int opcode)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, table, &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, key_name, key_val));

    SHR_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl, opcode, BCMLT_PRIORITY_NORMAL));

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

static int
vlan_stg_get(int unit, int vid, uint64_t *stg_id)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int rv;
    const char *table = "VLAN";
    uint64_t field_data;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(stg_id, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, table, &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "VLAN_ID", vid));

    rv = bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL);
    if (SHR_FAILURE(rv)) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry_hdl, "VLAN_STG_ID", &field_data));

    *stg_id = field_data;

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

static int
vlan_stg_set(int unit, int vid, uint64_t stg_id, uint8_t profile_id)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int rv;
    const char *table = "VLAN";

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, table, &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "VLAN_ID", vid));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "VLAN_STG_ID", stg_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "VLAN_PROFILE_ID", profile_id));

    rv = bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_UPDATE,
                            BCMLT_PRIORITY_NORMAL);
    if (SHR_FAILURE(rv)) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

/*
 * In vlan table, we need to setup egress member ports in ingress pipeline.
 * This function :
 *   1) returns true if member ports is referenced by extra profile id
 *      ==> VLAN.VLAN_ING_EGR_MEMBER_PORTS_PROFILE_ID
 *          to VLAN_ING_EGR_MEMBER_PORTS_PROFILE.EGR_MEMBER_PORTS
 *   2) returns false if member ports is assigned in vlan table
 *      ==> VLAN.ING_EGR_MEMBER_PORTS
 */
static int
vlan_ing_egrmbr_profile_need(int unit, bool *ret_need)
{
    int rv;
    bcma_bcmlt_entry_info_t *ei = NULL;
    const char *table_name = "VLAN_ING_EGR_MEMBER_PORTS_PROFILE";

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ret_need, SHR_E_PARAM);

    rv = bcma_bcmlt_tables_search(unit, table_name, BCMA_BCMLT_MATCH_EXACT,
                                  BCMA_BCMLT_SCOPE_F_LTBL, NULL, &ei);
    if (rv <= 0) {
        *ret_need = false;
    } else {
        *ret_need = true;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
vlan_ing_egrmbr_profile_id_set(int unit, int vid, int pid)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "VLAN", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "VLAN_ID", vid));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add
            (entry_hdl, "VLAN_ING_EGR_MEMBER_PORTS_PROFILE_ID", pid));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_UPDATE,
                            BCMLT_PRIORITY_NORMAL));
exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
vlan_ing_egrmbr_profile_id_get(int unit, int vid, int* ret_pid)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t field_data;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ret_pid, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "VLAN", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "VLAN_ID", vid));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get
            (entry_hdl, "VLAN_ING_EGR_MEMBER_PORTS_PROFILE_ID", &field_data));

    *ret_pid = field_data;

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
vlan_ing_egrmbr_profile_ports_set(int unit, int vid,
                                  uint64_t *data, uint32_t num)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int pid;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(data, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (vlan_ing_egrmbr_profile_id_get(unit, vid, &pid));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate
            (unit, "VLAN_ING_EGR_MEMBER_PORTS_PROFILE", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add
            (entry_hdl, "VLAN_ING_EGR_MEMBER_PORTS_PROFILE_ID", pid));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add
            (entry_hdl, "EGR_MEMBER_PORTS", 0, data, num));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_UPDATE,
                            BCMLT_PRIORITY_NORMAL));
exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
vlan_ing_egrmbr_profile_ports_get(int unit, int vid,
                                  uint64_t *data, uint32_t num,
                                  uint32_t *actual_num)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int pid;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(data, SHR_E_PARAM);
    SHR_NULL_CHECK(actual_num, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (vlan_ing_egrmbr_profile_id_get(unit, vid, &pid));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate
            (unit, "VLAN_ING_EGR_MEMBER_PORTS_PROFILE", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add
            (entry_hdl, "VLAN_ING_EGR_MEMBER_PORTS_PROFILE_ID", pid));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_get(entry_hdl, "EGR_MEMBER_PORTS", 0,
                                     data, num, actual_num));

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
xgs_vlan_create(int unit, int vid, int ing_egrmbr_pid)
{
    bcmlt_entry_handle_t entry = BCMLT_INVALID_HDL;
    const char *stg_table = "VLAN_STG";
    const char *vlan_table = "VLAN";
    const char *vlan_profile_table = "VLAN_PROFILE";
    const int stg_id = 2;
    const uint8_t vlan_profile_id = 2;
    int rv;
    bool need_profile;

    SHR_FUNC_ENTER(unit);

    rv = table_entry_handle(unit, stg_table, "VLAN_STG_ID", stg_id, BCMLT_OPCODE_INSERT);
    if (rv != SHR_E_NONE && rv != SHR_E_EXISTS) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    rv = table_entry_handle(unit, vlan_table, "VLAN_ID", vid, BCMLT_OPCODE_INSERT);
    if (rv != SHR_E_NONE && rv != SHR_E_EXISTS) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    rv = table_entry_handle(unit, vlan_profile_table, "VLAN_PROFILE_ID",
                            vlan_profile_id, BCMLT_OPCODE_INSERT);
    if (rv != SHR_E_NONE && rv != SHR_E_EXISTS) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    SHR_IF_ERR_EXIT
        (vlan_stg_set(unit, vid, stg_id, vlan_profile_id));

    /* Default value is flood all MC */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, vlan_profile_table, &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "VLAN_PROFILE_ID", vlan_profile_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "L2_PFM", "FLOOD_NONE"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_EXIT
        (vlan_ing_egrmbr_profile_need(unit, &need_profile));

    if (need_profile == true) {
        const char *profile_table = "VLAN_ING_EGR_MEMBER_PORTS_PROFILE";
        const char *profile_table_id = "VLAN_ING_EGR_MEMBER_PORTS_PROFILE_ID";

        SHR_IF_ERR_EXIT_EXCEPT_IF
            (table_entry_handle
                (unit, profile_table, profile_table_id,
                 ing_egrmbr_pid, BCMLT_OPCODE_INSERT), SHR_E_EXISTS);

        SHR_IF_ERR_EXIT
            (vlan_ing_egrmbr_profile_id_set(unit, vid, ing_egrmbr_pid));
    }

exit:
    if (entry != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry);
    }
    SHR_FUNC_EXIT();
}

static int
xgs_vlan_destroy(int unit, int vid)
{
    const char *stg_table = "VLAN_STG";
    const char *vlan_table = "VLAN";
    uint64_t stg_id;
    bool need_profile;

    SHR_FUNC_ENTER(unit);

    /* delete stg */
    SHR_IF_ERR_EXIT
        (vlan_stg_get(unit, vid, &stg_id));

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (table_entry_handle(unit, stg_table, "VLAN_STG_ID",
                            stg_id, BCMLT_OPCODE_DELETE), SHR_E_NOT_FOUND);

    /* delete profile if need */
    SHR_IF_ERR_EXIT
        (vlan_ing_egrmbr_profile_need(unit, &need_profile));

    if (need_profile == true) {
        int pid;
        const char *profile_table = "VLAN_ING_EGR_MEMBER_PORTS_PROFILE";
        const char *profile_table_id = "VLAN_ING_EGR_MEMBER_PORTS_PROFILE_ID";

        SHR_IF_ERR_EXIT
            (vlan_ing_egrmbr_profile_id_get(unit, vid, &pid));

        SHR_IF_ERR_EXIT_EXCEPT_IF
            (table_entry_handle
                (unit, profile_table, profile_table_id,
                 pid, BCMLT_OPCODE_DELETE), SHR_E_NOT_FOUND);
    }

    /* delete vlan table finally */
    SHR_IF_ERR_EXIT
        (table_entry_handle(unit, vlan_table, "VLAN_ID", vid, BCMLT_OPCODE_DELETE));

exit:
    SHR_FUNC_EXIT();
}

static int
xgs_vlan_port_vlan_set(int unit, int port, int vid)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int rv;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "PORT", &entry_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "PORT_ID", port));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "ING_OVID", vid));
    rv = bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_UPDATE,
                            BCMLT_PRIORITY_NORMAL);
    if (SHR_FAILURE(rv)) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
xgs_vlan_port_add(int unit, int vid, const bcmdrd_pbmp_t pbmp,
                  const bcmdrd_pbmp_t ubmp, bool src_port_block)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    const char *policy_table = "PORT_POLICY";
    const char *flood_block_table = "PORT_FLOOD_BLOCK";
    const char *port_table = "PORT";
    const char *bridge_table = "PORT_BRIDGE";
    const char *vlan_table = "VLAN";
    int port = 0;
    uint64_t *port_id = NULL;
    uint64_t pass_on[4] = {1, 1, 1, 1};
    int field_depth;
    bool need_profile;

    SHR_FUNC_ENTER(unit);

    port_id = sal_alloc(sizeof(uint64_t) * BCMDRD_CONFIG_MAX_PORTS,
                        "bcmaTestVLANUtil");
    if (port_id == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    sal_memset(port_id, 0, sizeof(uint64_t) * BCMDRD_CONFIG_MAX_PORTS);
    BCMDRD_PBMP_ITER(pbmp, port) {
        port_id[port] = 1;
    }

    /* For each port, create policy, flood_block table */
    BCMDRD_PBMP_ITER(pbmp, port) {
        /* Policy table */
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (table_entry_handle(unit, policy_table, "PORT_ID",
                                port, BCMLT_OPCODE_INSERT), SHR_E_EXISTS);
        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(unit, policy_table, &entry_hdl));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, "PORT_ID", port));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_array_add(entry_hdl,
                                         "PASS_ON_OUTER_TPID_MATCH", 0,
                                         pass_on, COUNTOF(pass_on)));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_UPDATE,
                                BCMLT_PRIORITY_NORMAL));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_free(entry_hdl));
        entry_hdl = BCMLT_INVALID_HDL;

        /* Flood_block table */
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (table_entry_handle(unit, flood_block_table, "PORT_ID",
                                port, BCMLT_OPCODE_INSERT), SHR_E_EXISTS);

        SHR_IF_ERR_EXIT
            (bcma_testutil_lt_field_depth_get(unit, flood_block_table,
                                              "UNKNOWN_MC", &field_depth));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(unit, flood_block_table, &entry_hdl));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, "PORT_ID", port));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_array_add(entry_hdl, "UNKNOWN_MC", 0,
                                         port_id, field_depth));
        /* Block broadcast packet receiving on the ingress port */
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_array_add(entry_hdl, "BC", 0,
                                         port_id, field_depth));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_UPDATE,
                                BCMLT_PRIORITY_NORMAL));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_free(entry_hdl));
        entry_hdl = BCMLT_INVALID_HDL;
    }

    /* Update bridge table */
    BCMDRD_PBMP_ITER(pbmp, port) {
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (table_entry_handle(unit, bridge_table, "PORT_ID",
                                port, BCMLT_OPCODE_INSERT), SHR_E_EXISTS);
        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(unit, bridge_table, &entry_hdl));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, "PORT_ID", port));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, "BRIDGE",
                                   (src_port_block == FALSE)? 1: 0));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_UPDATE,
                                BCMLT_PRIORITY_NORMAL));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_free(entry_hdl));
        entry_hdl = BCMLT_INVALID_HDL;
    }

    /* Update port table */
    BCMDRD_PBMP_ITER(pbmp, port) {
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (table_entry_handle(unit, port_table, "PORT_ID",
                                port, BCMLT_OPCODE_INSERT), SHR_E_EXISTS);
        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(unit, port_table, &entry_hdl));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, "PORT_ID", port));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, "ING_OVID", vid));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_UPDATE,
                                BCMLT_PRIORITY_NORMAL));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_free(entry_hdl));

        entry_hdl = BCMLT_INVALID_HDL;
    }

    /* Add ports into vlan */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, vlan_table, &entry_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "VLAN_ID", vid));
    SHR_IF_ERR_EXIT
        (bcma_testutil_lt_field_depth_get(unit, vlan_table, "EGR_MEMBER_PORTS",
                                          &field_depth));

    /* (1) egr and ing members */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry_hdl, "EGR_MEMBER_PORTS", 0,
                                     port_id, field_depth));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry_hdl, "ING_MEMBER_PORTS", 0,
                                     port_id, field_depth));

    /* (2) ing_egr members */
    SHR_IF_ERR_EXIT
        (vlan_ing_egrmbr_profile_need(unit, &need_profile));

    if (need_profile == true) {
        SHR_IF_ERR_EXIT
            (vlan_ing_egrmbr_profile_ports_set(unit, vid, port_id, field_depth));
    } else {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_array_add(entry_hdl, "ING_EGR_MEMBER_PORTS", 0,
                                         port_id, field_depth));
    }

    /* (3) untag members */
    sal_memset(port_id, 0, sizeof(uint64_t) * BCMDRD_CONFIG_MAX_PORTS);
    BCMDRD_PBMP_ITER(ubmp, port) {
        port_id[port] = 1;
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry_hdl, "UNTAGGED_MEMBER_PORTS", 0,
                                     port_id, field_depth));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_UPDATE,
                            BCMLT_PRIORITY_NORMAL));

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }

    if (port_id != NULL) {
        sal_free(port_id);
    }

    SHR_FUNC_EXIT();
}

static int
xgs_vlan_port_update(int unit, int vid, const bcmdrd_pbmp_t pbmp,
                     const bcmdrd_pbmp_t ubmp)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int rv;
    const char *vlan_table = "VLAN";
    uint64_t port_id[BCMDRD_CONFIG_MAX_PORTS] = {0};
    int field_depth, port;
    bool need_profile;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, vlan_table, &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "VLAN_ID", vid));

    SHR_IF_ERR_EXIT
        (bcma_testutil_lt_field_depth_get(unit, vlan_table, "EGR_MEMBER_PORTS",
                                          &field_depth));

    /* (1) egr and ing members */
    sal_memset(port_id, 0, sizeof(port_id));
    BCMDRD_PBMP_ITER(pbmp, port) {
        port_id[port] = 1;
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry_hdl, "EGR_MEMBER_PORTS", 0,
                                     port_id, field_depth));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry_hdl, "ING_MEMBER_PORTS", 0,
                                     port_id, field_depth));

    /* (2) ing_egr members */
    SHR_IF_ERR_EXIT
        (vlan_ing_egrmbr_profile_need(unit, &need_profile));

    if (need_profile == true) {
        SHR_IF_ERR_EXIT
            (vlan_ing_egrmbr_profile_ports_set(unit, vid, port_id, field_depth));
    } else {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_array_add(entry_hdl, "ING_EGR_MEMBER_PORTS", 0,
                                         port_id, field_depth));
    }

    /* (3) untag members */
    sal_memset(port_id, 0, sizeof(port_id));
    BCMDRD_PBMP_ITER(ubmp, port) {
        port_id[port] = 1;
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry_hdl, "UNTAGGED_MEMBER_PORTS", 0,
                                     port_id, field_depth));

    rv = bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_UPDATE,
                            BCMLT_PRIORITY_NORMAL);
    if (SHR_FAILURE(rv)) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

 exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

static int
xgs_vlan_check(int unit, int vid)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "VLAN", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "VLAN_ID", vid));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL));
exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
xgs_vlan_port_remove(int unit, int vid, const bcmdrd_pbmp_t pbmp)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int rv;
    const char *vlan_table = "VLAN";
    const char *policy_table = "PORT_POLICY";
    const char *port_table = "PORT";
    const char *bridge_table = "PORT_BRIDGE";
    const char *flood_block_table = "PORT_FLOOD_BLOCK";
    uint64_t *ing_ports = NULL;
    uint64_t *egr_ports = NULL;
    uint64_t *untag_ports = NULL;
    uint32_t act_size;
    int port, field_depth;
    bool need_profile;

    SHR_FUNC_ENTER(unit);

    ing_ports = sal_alloc(sizeof(uint64_t) * BCMDRD_CONFIG_MAX_PORTS,
                          "bcmaTestVLANUtil");
    if (ing_ports == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    sal_memset(ing_ports, 0, sizeof(uint64_t) * BCMDRD_CONFIG_MAX_PORTS);

    egr_ports = sal_alloc(sizeof(uint64_t) * BCMDRD_CONFIG_MAX_PORTS,
                          "bcmaTestVLANUtil");
    if (egr_ports == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    sal_memset(egr_ports, 0, sizeof(uint64_t) * BCMDRD_CONFIG_MAX_PORTS);

    untag_ports = sal_alloc(sizeof(uint64_t) * BCMDRD_CONFIG_MAX_PORTS,
                          "bcmaTestVLANUtil");
    if (untag_ports == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    sal_memset(untag_ports, 0, sizeof(uint64_t) * BCMDRD_CONFIG_MAX_PORTS);

    SHR_IF_ERR_EXIT
        (bcma_testutil_lt_field_depth_get(unit, vlan_table, "EGR_MEMBER_PORTS",
                                          &field_depth));

    /* Delete Port, Port Policy, Port Bridge table */
    BCMDRD_PBMP_ITER(pbmp, port) {
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (table_entry_handle
                (unit, policy_table, "PORT_ID",
                 port, BCMLT_OPCODE_DELETE), SHR_E_NOT_FOUND);
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (table_entry_handle
                (unit, port_table, "PORT_ID",
                 port, BCMLT_OPCODE_DELETE), SHR_E_NOT_FOUND);
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (table_entry_handle
                (unit, bridge_table, "PORT_ID",
                 port, BCMLT_OPCODE_DELETE), SHR_E_NOT_FOUND);
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (table_entry_handle
                (unit, flood_block_table, "PORT_ID",
                 port, BCMLT_OPCODE_DELETE), SHR_E_NOT_FOUND);
    }

    /* Remove ports from ingress, egress, untag member of VLAN */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, vlan_table, &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "VLAN_ID", vid));

    rv = bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL);
    if (SHR_FAILURE(rv)) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    /* Set ingress member field */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_get(entry_hdl, "ING_MEMBER_PORTS", 0,
                                     ing_ports, field_depth, &act_size));

    BCMDRD_PBMP_ITER(pbmp, port) {
        ing_ports[port] = 0;
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry_hdl, "ING_MEMBER_PORTS", 0,
                                     ing_ports, act_size));

    /* Set egress member field */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_get(entry_hdl, "EGR_MEMBER_PORTS", 0,
                                     egr_ports, field_depth, &act_size));

    BCMDRD_PBMP_ITER(pbmp, port) {
        egr_ports[port] = 0;
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry_hdl, "EGR_MEMBER_PORTS", 0,
                                     egr_ports, act_size));

    /* Set egress member (in ingress pipeline) field */
    SHR_IF_ERR_EXIT
        (vlan_ing_egrmbr_profile_need(unit, &need_profile));

    sal_memset(egr_ports, 0, sizeof(uint64_t) * BCMDRD_CONFIG_MAX_PORTS);
    if (need_profile == true) {
        SHR_IF_ERR_EXIT
            (vlan_ing_egrmbr_profile_ports_get(unit, vid, egr_ports, field_depth,
                                               &act_size));
    } else {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_array_get(entry_hdl, "ING_EGR_MEMBER_PORTS", 0,
                                         egr_ports, field_depth, &act_size));
    }

    BCMDRD_PBMP_ITER(pbmp, port) {
        egr_ports[port] = 0;
    }

    if (need_profile == true) {
        SHR_IF_ERR_EXIT
            (vlan_ing_egrmbr_profile_ports_set(unit, vid, egr_ports, act_size));
    } else {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_array_add(entry_hdl, "ING_EGR_MEMBER_PORTS", 0,
                                         egr_ports, act_size));
    }

    /* Set untag member field */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_get(entry_hdl, "UNTAGGED_MEMBER_PORTS", 0,
                                     untag_ports, field_depth, &act_size));

    BCMDRD_PBMP_ITER(pbmp, port) {
        untag_ports[port] = 0;
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry_hdl, "UNTAGGED_MEMBER_PORTS", 0,
                                     untag_ports, act_size));

    rv = bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_UPDATE,
                            BCMLT_PRIORITY_NORMAL);
    if (SHR_FAILURE(rv)) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }

    if (ing_ports != NULL) {
        sal_free(ing_ports);
    }

    if (egr_ports != NULL) {
        sal_free(egr_ports);
    }

    if (untag_ports != NULL) {
        sal_free(untag_ports);
    }

    SHR_FUNC_EXIT();
}

static int
xgs_vlan_stp_set(int unit, int vid, const bcmdrd_pbmp_t pbmp,
                 bcma_testutil_stp_state_t stp_state)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int rv;
    const char *table = "VLAN_STG";
    const char *state[1];
    uint64_t stg_id;
    int port;

    SHR_FUNC_ENTER(unit);

    state[0] = bcma_testutil_id2name(bcma_testutil_stp_state_name_id_map,
                                     BCMA_TESTUTIL_STP_COUNT, stp_state);
    if (state[0] == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (SHR_FAILURE(vlan_stg_get(unit, vid, &stg_id))) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    /* Set stp */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, table, &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "VLAN_STG_ID", stg_id));

    BCMDRD_PBMP_ITER(pbmp, port) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_array_symbol_add(entry_hdl, "STATE", port,
                                                state, COUNTOF(state)));
    }

    rv = bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_UPDATE,
                            BCMLT_PRIORITY_NORMAL);
    if (SHR_FAILURE(rv)) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

static int
xgs_vlan_untag_action_add(int unit,
                          bcma_testutil_vlan_tag_action_t tag_action)
{
    const char *table = "VLAN_ING_TAG_ACTION_PROFILE";
    const char *entry_id = "VLAN_ING_TAG_ACTION_PROFILE_ID";
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int rv;
    const char* action_str = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (table_entry_handle
            (unit, table, entry_id, 0, BCMLT_OPCODE_INSERT), SHR_E_EXISTS);

    action_str = bcma_testutil_id2name(bcma_testutil_tag_action_name_id_map,
                                       BCMA_TESTUTIL_VLAN_TAG_ACTION_COUNT,
                                       tag_action);
    if (action_str == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, table, &entry_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, entry_id, 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, "UT_OTAG", action_str));
    rv = bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_UPDATE,
                            BCMLT_PRIORITY_NORMAL);
    if (SHR_FAILURE(rv)) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
xgs_vlan_untag_action_remove(int unit)
{
    const char *table = "VLAN_ING_TAG_ACTION_PROFILE";
    const char *entry_id = "VLAN_ING_TAG_ACTION_PROFILE_ID";

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (table_entry_handle
            (unit, table, entry_id, 0, BCMLT_OPCODE_DELETE), SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

static bcma_testutil_vlan_op_t bcma_testutil_xgs_vlan_op = {
    .check = xgs_vlan_check,
    .create = xgs_vlan_create,
    .destroy = xgs_vlan_destroy,
    .port_add = xgs_vlan_port_add,
    .port_update = xgs_vlan_port_update,
    .port_remove = xgs_vlan_port_remove,
    .port_vlan_set = xgs_vlan_port_vlan_set,
    .stp_set = xgs_vlan_stp_set,
    .untag_action_add = xgs_vlan_untag_action_add,
    .untag_action_remove = xgs_vlan_untag_action_remove
};

bcma_testutil_vlan_op_t *bcma_testutil_xgs_vlan_op_get(int unit)
{
    return &bcma_testutil_xgs_vlan_op;
}
