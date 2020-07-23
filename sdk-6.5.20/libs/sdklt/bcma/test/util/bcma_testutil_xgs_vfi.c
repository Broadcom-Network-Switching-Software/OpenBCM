/*! \file bcma_testutil_xgs_vfi.c
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
    int rv = SHR_E_NONE;
    const char *table = "VFI";
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t field_data = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(stg_id, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, table, &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "VFI_ID", vid));

    rv = bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL);
    if (SHR_FAILURE(rv)) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry_hdl, "VFI_STG_ID", &field_data));

    *stg_id = field_data;

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

static int
vlan_stg_set(int unit, int vid, uint64_t stg_id)
{
    int rv = SHR_E_NONE;
    const char *table = "VFI";
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, table, &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "VFI_ID", vid));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "VFI_STG_ID", stg_id));

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
vlan_table_profile_id_set(int unit, int vid, const char *field_name, int pid)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "VFI", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "VFI_ID", vid));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, field_name, pid));

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
vlan_table_profile_id_get(int unit, int vid, const char *field_name, int *pid)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t field_data;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(pid, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "VFI", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "VFI_ID", vid));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get
            (entry_hdl, field_name, &field_data));

    *pid = field_data;

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
vlan_ing_member_profile_add(int unit, int vid, int pid)
{
    const char *profile_table = "VFI_ING_MEMBER_PORTS_PROFILE";
    const char *profile_table_id = "VFI_ING_MEMBER_PORTS_PROFILE_ID";

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (table_entry_handle
            (unit, profile_table, profile_table_id,
                pid, BCMLT_OPCODE_INSERT), SHR_E_EXISTS);

    SHR_IF_ERR_EXIT
        (vlan_table_profile_id_set(unit, vid, profile_table_id, pid));

exit:
    SHR_FUNC_EXIT();
}

static int
vlan_egr_member_profile_add(int unit, int vid, int pid)
{
    const char *profile_table = "VFI_EGR_MEMBER_PORTS_PROFILE";
    const char *profile_table_id = "VFI_EGR_MEMBER_PORTS_PROFILE_ID";

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (table_entry_handle
            (unit, profile_table, profile_table_id,
                pid, BCMLT_OPCODE_INSERT), SHR_E_EXISTS);

    SHR_IF_ERR_EXIT
        (vlan_table_profile_id_set(unit, vid, profile_table_id, pid));

exit:
    SHR_FUNC_EXIT();
}

static int
vlan_ing_egr_member_profile_add(int unit, int vid, int pid)
{
    const char *profile_table = "VFI_ING_EGR_MEMBER_PORTS_PROFILE";
    const char *profile_table_id = "VFI_ING_EGR_MEMBER_PORTS_PROFILE_ID";

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (table_entry_handle
            (unit, profile_table, profile_table_id,
                pid, BCMLT_OPCODE_INSERT), SHR_E_EXISTS);

    SHR_IF_ERR_EXIT
        (vlan_table_profile_id_set(unit, vid, profile_table_id, pid));

exit:
    SHR_FUNC_EXIT();
}

static int
vlan_egr_untagged_member_profile_add(int unit, int vid, int pid)
{
    const char *profile_table = "VFI_EGR_UNTAGGED_MEMBER_PORTS_PROFILE";
    const char *profile_table_id = "VFI_EGR_MEMBER_PORTS_PROFILE_ID";

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (table_entry_handle
            (unit, profile_table, profile_table_id,
                pid, BCMLT_OPCODE_INSERT), SHR_E_EXISTS);

    SHR_IF_ERR_EXIT
        (vlan_table_profile_id_set(unit, vid, profile_table_id, pid));

exit:
    SHR_FUNC_EXIT();
}

static int
vlan_ing_member_profile_delete(int unit, int vid)
{
    int pid = 0;
    const char *profile_table = "VFI_ING_MEMBER_PORTS_PROFILE";
    const char *profile_table_id = "VFI_ING_MEMBER_PORTS_PROFILE_ID";

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (vlan_table_profile_id_get(unit, vid, profile_table_id, &pid));

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (table_entry_handle
            (unit, profile_table, profile_table_id,
                pid, BCMLT_OPCODE_DELETE), SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

static int
vlan_egr_member_profile_delete(int unit, int vid)
{
    int pid = 0;
    const char *profile_table = "VFI_EGR_MEMBER_PORTS_PROFILE";
    const char *profile_table_id = "VFI_EGR_MEMBER_PORTS_PROFILE_ID";

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (vlan_table_profile_id_get(unit, vid, profile_table_id, &pid));

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (table_entry_handle
            (unit, profile_table, profile_table_id,
                pid, BCMLT_OPCODE_DELETE), SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

static int
vlan_ing_egr_member_profile_delete(int unit, int vid)
{
    int pid = 0;
    const char *profile_table = "VFI_ING_EGR_MEMBER_PORTS_PROFILE";
    const char *profile_table_id = "VFI_ING_EGR_MEMBER_PORTS_PROFILE_ID";

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (vlan_table_profile_id_get(unit, vid, profile_table_id, &pid));

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (table_entry_handle
            (unit, profile_table, profile_table_id,
                pid, BCMLT_OPCODE_DELETE), SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

static int
vlan_egr_untagged_member_profile_delete(int unit, int vid)
{
    int pid = 0;
    const char *profile_table = "VFI_EGR_UNTAGGED_MEMBER_PORTS_PROFILE";
    const char *profile_table_id = "VFI_EGR_MEMBER_PORTS_PROFILE_ID";

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (vlan_table_profile_id_get(unit, vid, profile_table_id, &pid));

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (table_entry_handle
            (unit, profile_table, profile_table_id,
                pid, BCMLT_OPCODE_DELETE), SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

static int
vlan_ing_member_profile_ports_set(int unit, int vid, const bcmdrd_pbmp_t pbmp)
{
    int pid = 0;
    int port = 0;
    int field_depth = 0;
    uint64_t port_id[BCMDRD_CONFIG_MAX_PORTS] = {0};
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    const char *profile_table = "VFI_ING_MEMBER_PORTS_PROFILE";
    const char *profile_table_id = "VFI_ING_MEMBER_PORTS_PROFILE_ID";
    const char *field_name = "MEMBER_PORTS";

    SHR_FUNC_ENTER(unit);

    BCMDRD_PBMP_ITER(pbmp, port) {
        port_id[port] = 1;
    }

    SHR_IF_ERR_EXIT
        (bcma_testutil_lt_field_depth_get
            (unit, profile_table, field_name, &field_depth));

    SHR_IF_ERR_EXIT
        (vlan_table_profile_id_get(unit, vid, profile_table_id, &pid));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, profile_table, &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, profile_table_id, pid));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add
            (entry_hdl, field_name, 0, port_id, field_depth));

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
vlan_egr_member_profile_ports_set(int unit, int vid, const bcmdrd_pbmp_t pbmp)
{
    int pid = 0;
    int port = 0;
    int field_depth = 0;
    uint64_t port_id[BCMDRD_CONFIG_MAX_PORTS] = {0};
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    const char *profile_table = "VFI_EGR_MEMBER_PORTS_PROFILE";
    const char *profile_table_id = "VFI_EGR_MEMBER_PORTS_PROFILE_ID";
    const char *field_name = "MEMBER_PORTS";

    SHR_FUNC_ENTER(unit);

    BCMDRD_PBMP_ITER(pbmp, port) {
        port_id[port] = 1;
    }

    SHR_IF_ERR_EXIT
        (bcma_testutil_lt_field_depth_get
            (unit, profile_table, field_name, &field_depth));

    SHR_IF_ERR_EXIT
        (vlan_table_profile_id_get(unit, vid, profile_table_id, &pid));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, profile_table, &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, profile_table_id, pid));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add
            (entry_hdl, field_name, 0, port_id, field_depth));

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
vlan_ing_egr_member_profile_ports_set(int unit, int vid, const bcmdrd_pbmp_t pbmp)
{
    int pid = 0;
    int port = 0;
    int field_depth = 0;
    uint64_t port_id[BCMDRD_CONFIG_MAX_PORTS] = {0};
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    const char *profile_table = "VFI_ING_EGR_MEMBER_PORTS_PROFILE";
    const char *profile_table_id = "VFI_ING_EGR_MEMBER_PORTS_PROFILE_ID";
    const char *field_name = "EGR_MEMBER_PORTS";

    SHR_FUNC_ENTER(unit);

    BCMDRD_PBMP_ITER(pbmp, port) {
        port_id[port] = 1;
    }

    SHR_IF_ERR_EXIT
        (bcma_testutil_lt_field_depth_get
            (unit, profile_table, field_name, &field_depth));

    SHR_IF_ERR_EXIT
        (vlan_table_profile_id_get(unit, vid, profile_table_id, &pid));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, profile_table, &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, profile_table_id, pid));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add
            (entry_hdl, field_name, 0, port_id, field_depth));

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
vlan_egr_untagged_member_profile_ports_set(int unit, int vid, const bcmdrd_pbmp_t pbmp)
{
    int pid = 0;
    int port = 0;
    int field_depth = 0;
    uint64_t port_id[BCMDRD_CONFIG_MAX_PORTS] = {0};
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    const char *profile_table = "VFI_EGR_UNTAGGED_MEMBER_PORTS_PROFILE";
    const char *profile_table_id = "VFI_EGR_MEMBER_PORTS_PROFILE_ID";
    const char *field_name = "MEMBER_PORTS";

    SHR_FUNC_ENTER(unit);

    BCMDRD_PBMP_ITER(pbmp, port) {
        port_id[port] = 1;
    }

    SHR_IF_ERR_EXIT
        (bcma_testutil_lt_field_depth_get
            (unit, profile_table, field_name, &field_depth));

    SHR_IF_ERR_EXIT
        (vlan_table_profile_id_get(unit, vid, profile_table_id, &pid));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, profile_table, &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, profile_table_id, pid));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add
            (entry_hdl, field_name, 0, port_id, field_depth));

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
vlan_ing_member_profile_ports_remove(int unit, int vid, const bcmdrd_pbmp_t pbmp)
{
    int pid = 0;
    int port = 0;
    int field_depth = 0;
    uint32_t actual_num = 0;
    uint64_t port_id[BCMDRD_CONFIG_MAX_PORTS] = {0};
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    const char *profile_table = "VFI_ING_MEMBER_PORTS_PROFILE";
    const char *profile_table_id = "VFI_ING_MEMBER_PORTS_PROFILE_ID";
    const char *field_name = "MEMBER_PORTS";

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcma_testutil_lt_field_depth_get
            (unit, profile_table, field_name, &field_depth));

    SHR_IF_ERR_EXIT
        (vlan_table_profile_id_get(unit, vid, profile_table_id, &pid));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, profile_table, &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, profile_table_id, pid));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_get
            (entry_hdl, field_name, 0,
             port_id, field_depth, &actual_num));

    BCMDRD_PBMP_ITER(pbmp, port) {
        port_id[port] = 0;
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add
            (entry_hdl, field_name, 0, port_id, actual_num));

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
vlan_egr_member_profile_ports_remove(int unit, int vid, const bcmdrd_pbmp_t pbmp)
{
    int pid = 0;
    int port = 0;
    int field_depth = 0;
    uint32_t actual_num = 0;
    uint64_t port_id[BCMDRD_CONFIG_MAX_PORTS] = {0};
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    const char *profile_table = "VFI_EGR_MEMBER_PORTS_PROFILE";
    const char *profile_table_id = "VFI_EGR_MEMBER_PORTS_PROFILE_ID";
    const char *field_name = "MEMBER_PORTS";

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcma_testutil_lt_field_depth_get
            (unit, profile_table, field_name, &field_depth));

    SHR_IF_ERR_EXIT
        (vlan_table_profile_id_get(unit, vid, profile_table_id, &pid));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, profile_table, &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, profile_table_id, pid));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_get
            (entry_hdl, field_name, 0,
             port_id, field_depth, &actual_num));

    BCMDRD_PBMP_ITER(pbmp, port) {
        port_id[port] = 0;
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add
            (entry_hdl, field_name, 0, port_id, actual_num));

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
vlan_ing_egr_member_profile_ports_remove(int unit, int vid, const bcmdrd_pbmp_t pbmp)
{
    int pid = 0;
    int port = 0;
    int field_depth = 0;
    uint32_t actual_num = 0;
    uint64_t port_id[BCMDRD_CONFIG_MAX_PORTS] = {0};
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    const char *profile_table = "VFI_ING_EGR_MEMBER_PORTS_PROFILE";
    const char *profile_table_id = "VFI_ING_EGR_MEMBER_PORTS_PROFILE_ID";
    const char *field_name = "EGR_MEMBER_PORTS";

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcma_testutil_lt_field_depth_get
            (unit, profile_table, field_name, &field_depth));

    SHR_IF_ERR_EXIT
        (vlan_table_profile_id_get(unit, vid, profile_table_id, &pid));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, profile_table, &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, profile_table_id, pid));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_get
            (entry_hdl, field_name, 0,
             port_id, field_depth, &actual_num));

    BCMDRD_PBMP_ITER(pbmp, port) {
        port_id[port] = 0;
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add
            (entry_hdl, field_name, 0, port_id, actual_num));

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
vlan_egr_untagged_member_profile_ports_remove(int unit, int vid, const bcmdrd_pbmp_t pbmp)
{
    int pid = 0;
    int port = 0;
    int field_depth = 0;
    uint32_t actual_num = 0;
    uint64_t port_id[BCMDRD_CONFIG_MAX_PORTS] = {0};
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    const char *profile_table = "VFI_EGR_UNTAGGED_MEMBER_PORTS_PROFILE";
    const char *profile_table_id = "VFI_EGR_MEMBER_PORTS_PROFILE_ID";
    const char *field_name = "MEMBER_PORTS";

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcma_testutil_lt_field_depth_get
            (unit, profile_table, field_name, &field_depth));

    SHR_IF_ERR_EXIT
        (vlan_table_profile_id_get(unit, vid, profile_table_id, &pid));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, profile_table, &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, profile_table_id, pid));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_get
            (entry_hdl, field_name, 0,
             port_id, field_depth, &actual_num));

    BCMDRD_PBMP_ITER(pbmp, port) {
        port_id[port] = 0;
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add
            (entry_hdl, field_name, 0, port_id, actual_num));

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
vlan_l2_mc_flood_disable(int unit, int vid, int profile_id)
{
    const char *vlan_profile_table = "VFI_PROFILE";
    const char *vlan_table = "VFI";
    bcmlt_entry_handle_t entry = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    /* Disable MC Flood. */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, vlan_profile_table, &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "VFI_PROFILE_ID", profile_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "L2_PFM", "FLOOD_NONE"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_free(entry));

    /* Attach the profile to VLAN table. */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, vlan_table, &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "VFI_ID", vid));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "VFI_PROFILE_ID", profile_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));

exit:
    if (entry != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry);
    }

    SHR_FUNC_EXIT()
}

static int
xgs_vlan_create(int unit, int vid, int ing_egrmbr_pid)
{
    int rv = SHR_E_NONE;
    const char *stg_table = "VFI_STG";
    const char *vlan_profile_table = "VFI_PROFILE";
    const char *vlan_table = "VFI";
    const int stg_id = 2;
    const uint8_t vlan_profile_id = 2;

    SHR_FUNC_ENTER(unit);

    rv = table_entry_handle(unit, stg_table, "VFI_STG_ID", stg_id,
                            BCMLT_OPCODE_INSERT);
    if (rv != SHR_E_NONE && rv != SHR_E_EXISTS) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    rv = table_entry_handle(unit, vlan_profile_table, "VFI_PROFILE_ID",
                            vlan_profile_id, BCMLT_OPCODE_INSERT);
    if (rv != SHR_E_NONE && rv != SHR_E_EXISTS) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    rv = table_entry_handle(unit, vlan_table, "VFI_ID", vid,
                            BCMLT_OPCODE_INSERT);
    if (rv != SHR_E_NONE && rv != SHR_E_EXISTS) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    SHR_IF_ERR_EXIT
        (vlan_stg_set(unit, vid, stg_id));

    SHR_IF_ERR_EXIT
        (vlan_l2_mc_flood_disable(unit, vid, vlan_profile_id));

    SHR_IF_ERR_EXIT
        (vlan_ing_member_profile_add(unit, vid, ing_egrmbr_pid));
    SHR_IF_ERR_EXIT
        (vlan_egr_member_profile_add(unit, vid, ing_egrmbr_pid));
    SHR_IF_ERR_EXIT
        (vlan_ing_egr_member_profile_add(unit, vid, ing_egrmbr_pid));
    SHR_IF_ERR_EXIT
        (vlan_egr_untagged_member_profile_add(unit, vid, ing_egrmbr_pid));

exit:
    SHR_FUNC_EXIT();
}

static int
xgs_vlan_destroy(int unit, int vid)
{
    const char *stg_table = "VFI_STG";
    const char *vlan_profile_table = "VFI_PROFILE";
    const char *vlan_table = "VFI";
    uint64_t stg_id;
    int vlan_profile_id;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (vlan_stg_get(unit, vid, &stg_id));

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (table_entry_handle(unit, stg_table, "VFI_STG_ID",
                            stg_id, BCMLT_OPCODE_DELETE), SHR_E_NOT_FOUND);

    SHR_IF_ERR_EXIT
        (vlan_table_profile_id_get(unit, vid, "VFI_PROFILE_ID", &vlan_profile_id));

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (table_entry_handle(unit, vlan_profile_table, "VFI_PROFILE_ID",
                            vlan_profile_id, BCMLT_OPCODE_DELETE), SHR_E_NOT_FOUND);

    SHR_IF_ERR_EXIT
        (vlan_ing_member_profile_delete(unit, vid));
    SHR_IF_ERR_EXIT
        (vlan_egr_member_profile_delete(unit, vid));
    SHR_IF_ERR_EXIT
        (vlan_ing_egr_member_profile_delete(unit, vid));
    SHR_IF_ERR_EXIT
        (vlan_egr_untagged_member_profile_delete(unit, vid));

    SHR_IF_ERR_EXIT
        (table_entry_handle(unit, vlan_table, "VFI_ID", vid, BCMLT_OPCODE_DELETE));

exit:
    SHR_FUNC_EXIT();
}

static int
xgs_vlan_port_vlan_set(int unit, int port, int vid)
{
    int rv = SHR_E_NONE;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

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
    int rv = SHR_E_NONE;
    int port = 0;
    uint64_t *port_id = NULL;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    const char *policy_table = "PORT_POLICY";
    const char *flood_block_table = "PORT_FLOOD_BLOCK";
    const char *port_table = "PORT";
    const char *bridge_table = "PORT_BRIDGE";

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

    /* For each new port, create policy, flood_block table */
    BCMDRD_PBMP_ITER(pbmp, port) {

        /* policy table */
        rv = table_entry_handle(unit, policy_table, "PORT_ID",
                                port, BCMLT_OPCODE_INSERT);

        if (rv != SHR_E_NONE && rv != SHR_E_EXISTS) {
            SHR_ERR_EXIT(SHR_E_FAIL);
        } else if (rv == SHR_E_NONE) {
            uint64_t pass_on[4] = {1, 1, 1, 1};

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
        }

        /* flood_block table */
        rv = table_entry_handle(unit, flood_block_table, "PORT_ID",
                                port, BCMLT_OPCODE_INSERT);
        if (rv != SHR_E_NONE && rv != SHR_E_EXISTS) {
            SHR_ERR_EXIT(SHR_E_FAIL);
        } else if (rv == SHR_E_NONE) {
            int field_depth = 0;

            SHR_IF_ERR_EXIT
                (bcma_testutil_lt_field_depth_get(unit, flood_block_table,
                                                  "UNKNOWN_MC",
                                                  &field_depth));
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

    /* set member profile */
    SHR_IF_ERR_EXIT
        (vlan_ing_member_profile_ports_set(unit, vid, pbmp));
    SHR_IF_ERR_EXIT
        (vlan_egr_member_profile_ports_set(unit, vid, pbmp));
    SHR_IF_ERR_EXIT
        (vlan_ing_egr_member_profile_ports_set(unit, vid, pbmp));
    SHR_IF_ERR_EXIT
        (vlan_egr_untagged_member_profile_ports_set(unit, vid, ubmp));

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
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (vlan_ing_member_profile_ports_set(unit, vid, pbmp));
    SHR_IF_ERR_EXIT
        (vlan_egr_member_profile_ports_set(unit, vid, pbmp));
    SHR_IF_ERR_EXIT
        (vlan_ing_egr_member_profile_ports_set(unit, vid, pbmp));
    SHR_IF_ERR_EXIT
        (vlan_egr_untagged_member_profile_ports_set(unit, vid, ubmp));

 exit:
    SHR_FUNC_EXIT();
}

static int
xgs_vlan_port_remove(int unit, int vid, const bcmdrd_pbmp_t pbmp)
{
    int port = 0;
    const char *policy_table = "PORT_POLICY";
    const char *port_table = "PORT";
    const char *bridge_table = "PORT_BRIDGE";
    const char *flood_block_table = "PORT_FLOOD_BLOCK";

    SHR_FUNC_ENTER(unit);

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

    SHR_IF_ERR_EXIT
        (vlan_ing_member_profile_ports_remove(unit, vid, pbmp));
    SHR_IF_ERR_EXIT
        (vlan_egr_member_profile_ports_remove(unit, vid, pbmp));
    SHR_IF_ERR_EXIT
        (vlan_ing_egr_member_profile_ports_remove(unit, vid, pbmp));
    SHR_IF_ERR_EXIT
        (vlan_egr_untagged_member_profile_ports_remove(unit, vid, pbmp));

exit:
    SHR_FUNC_EXIT();
}

static int
xgs_vlan_stp_set(int unit, int vid, const bcmdrd_pbmp_t pbmp,
                 bcma_testutil_stp_state_t stp_state)
{
    int rv = SHR_E_NONE;
    int port = 0;
    uint64_t stg_id = 0;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    const char *table = "VFI_STG";
    const char *state[1];

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
        (bcmlt_entry_field_add(entry_hdl, "VFI_STG_ID", stg_id));

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
    int rv = SHR_E_NONE;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    const char *table = "VLAN_ING_TAG_ACTION_PROFILE";
    const char *entry_id = "VLAN_ING_TAG_ACTION_PROFILE_ID";
    const char *action_str = NULL;

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

static bcma_testutil_vlan_op_t bcma_testutil_xgs_vfi_op = {
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

bcma_testutil_vlan_op_t *bcma_testutil_xgs_vfi_op_get(int unit)
{
    return &bcma_testutil_xgs_vfi_op;
}
