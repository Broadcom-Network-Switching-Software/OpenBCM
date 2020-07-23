/*! \file bcma_testutil_xfs_vlan.c
 *
 * XFS device VLAN operation utility.
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
#include <bcmltd/chip/bcmltd_str.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcma/test/util/bcma_testutil_vlan.h>
#include <bcma/test/util/bcma_testutil_common.h>

#define BSL_LOG_MODULE BSL_LS_APPL_TEST

#define ING_STG_PROFILE_ID 0
#define EGR_STG_PROFILE_ID 0
#define EGR_STRENGTH_PROFILE_ID 1

static shr_enum_map_t bcma_testutil_stp_state_name_id_map[] = {
    {"DISABLE", BCMA_TESTUTIL_STP_DISALBE},
    {"BLOCK", BCMA_TESTUTIL_STP_BLOCK},
    {"LEARN", BCMA_TESTUTIL_STP_LEARN},
    {"FORWARD", BCMA_TESTUTIL_STP_FORWARD}
};

static int
vlan_ing_egrmbr_profile_id_get(int unit, int vid, int* ret_pid)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t field_data;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ret_pid, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "ING_VFI_TABLE", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "VFI", vid));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get
            (entry_hdl, "MEMBERSHIP_PROFILE_PTR", &field_data));

    *ret_pid = field_data;

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
vlan_ing_member_profile_update_ports(int unit, int profile_id,
                                     const bcmdrd_pbmp_t pbmp,
                                     int add)
{
    bcmlt_entry_handle_t entry_hdl;
    uint64_t port_arr[160] = {0};
    uint32_t count = COUNTOF(port_arr), act_size, port;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "VLAN_ING_MEMBER_PROFILE", &entry_hdl));

    SHR_IF_ERR_EXIT
         (bcmlt_entry_field_add(entry_hdl, "VLAN_ING_MEMBER_PROFILE_ID",
                                profile_id));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_get(entry_hdl, "MEMBER_PORTS", 0, port_arr,
                                     count, &act_size));
    if (add) {
        BCMDRD_PBMP_ITER(pbmp, port) {
            if (port < count) {
                port_arr[port] = 1;
            }
        }
    } else {
        BCMDRD_PBMP_ITER(pbmp, port) {
            if (port < count) {
                port_arr[port] = 0;
            }
        }
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry_hdl, "MEMBER_PORTS", 0, port_arr,
                                     act_size));
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
vlan_ing_egr_member_ports_profile_update_ports(int unit, int profile_id,
                                               const bcmdrd_pbmp_t pbmp,
                                               int add)
{
    bcmlt_entry_handle_t entry_hdl;
    uint64_t port_arr[160] = {0};
    uint32_t count = COUNTOF(port_arr), act_size, port;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "VLAN_ING_EGR_MEMBER_PORTS_PROFILE",
                              &entry_hdl));

    SHR_IF_ERR_EXIT
         (bcmlt_entry_field_add(entry_hdl,
                                "VLAN_ING_EGR_MEMBER_PORTS_PROFILE_ID",
                                profile_id));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_get(entry_hdl, "EGR_MEMBER_PORTS", 0, port_arr,
                                     count, &act_size));

    if (add) {
        BCMDRD_PBMP_ITER(pbmp, port) {
            if (port < count) {
                port_arr[port] = 1;
            }
        }
    } else {
        BCMDRD_PBMP_ITER(pbmp, port) {
            if (port < count) {
                port_arr[port] = 0;
            }
        }
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry_hdl, "EGR_MEMBER_PORTS", 0, port_arr,
                                     act_size));

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
vlan_egr_member_profile_update_ports(int unit, int profile_id,
                                     const bcmdrd_pbmp_t pbmp,
                                     int add)
{
    bcmlt_entry_handle_t entry_hdl;
    uint64_t port_arr[160] = {0};
    uint32_t count = COUNTOF(port_arr), act_size, port;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "VLAN_EGR_MEMBER_PROFILE", &entry_hdl));

    SHR_IF_ERR_EXIT
         (bcmlt_entry_field_add(entry_hdl, "VLAN_EGR_MEMBER_PROFILE_ID",
                                profile_id));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_get(entry_hdl, "MEMBER_PORTS", 0, port_arr,
                                     count, &act_size));

    if (add) {
        BCMDRD_PBMP_ITER(pbmp, port) {
            if (port < count) {
                port_arr[port] = 1;
            }
        }
    } else {
        BCMDRD_PBMP_ITER(pbmp, port) {
            if (port < count) {
                port_arr[port] = 0;
            }
        }
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry_hdl, "MEMBER_PORTS", 0, port_arr,
                                     act_size));

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
xfs_vlan_profile_add(int unit)
{
    bcmlt_entry_handle_t entry_hdl_1 = BCMLT_INVALID_HDL;
    bcmlt_entry_handle_t entry_hdl_2 = BCMLT_INVALID_HDL;
    bcmlt_entry_handle_t entry_hdl_3 = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    /* Insert VLAN_ING_STG_PROFILE. (shared by all vlan id) */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "VLAN_ING_STG_PROFILE", &entry_hdl_1));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl_1, "VLAN_ING_STG_PROFILE_ID",
                               ING_STG_PROFILE_ID));

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmlt_entry_commit(entry_hdl_1, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL),
         SHR_E_EXISTS);

    /* Insert VLAN_ING_EGR_STG_MEMBER_PROFILEs (shared by all vlan id) */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "VLAN_ING_EGR_STG_MEMBER_PROFILE",
                              &entry_hdl_2));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl_2,
                               "VLAN_ING_EGR_STG_MEMBER_PROFILE_ID",
                               ING_STG_PROFILE_ID));

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmlt_entry_commit(entry_hdl_2, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL),
         SHR_E_EXISTS);

    /* Insert VLAN_EGR_STG_PROFILEs. (shared by all vlan id) */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "VLAN_EGR_STG_PROFILE", &entry_hdl_3));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl_3, "VLAN_EGR_STG_PROFILE_ID",
                               EGR_STG_PROFILE_ID));

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmlt_entry_commit(entry_hdl_3, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL),
         SHR_E_EXISTS);
exit:
    if (entry_hdl_1 != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl_1);
    }
    if (entry_hdl_2 != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl_2);
    }
    if (entry_hdl_3 != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl_3);
    }

    SHR_FUNC_EXIT();
}

static int
xfs_vlan_profile_del(int unit)
{
    bcmlt_entry_handle_t entry_hdl_1 = BCMLT_INVALID_HDL;
    bcmlt_entry_handle_t entry_hdl_2 = BCMLT_INVALID_HDL;
    bcmlt_entry_handle_t entry_hdl_3 = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    /* Delete VLAN_ING_STG_PROFILE. (shared by all vlan id) */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "VLAN_ING_STG_PROFILE", &entry_hdl_1));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl_1, "VLAN_ING_STG_PROFILE_ID",
                               ING_STG_PROFILE_ID));

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmlt_entry_commit(entry_hdl_1, BCMLT_OPCODE_DELETE,
                            BCMLT_PRIORITY_NORMAL),
         SHR_E_NOT_FOUND);

    /* Delete VLAN_ING_EGR_STG_MEMBER_PROFILEs (shared by all vlan id) */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "VLAN_ING_EGR_STG_MEMBER_PROFILE",
                              &entry_hdl_2));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl_2,
                               "VLAN_ING_EGR_STG_MEMBER_PROFILE_ID",
                               ING_STG_PROFILE_ID));

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmlt_entry_commit(entry_hdl_2, BCMLT_OPCODE_DELETE,
                            BCMLT_PRIORITY_NORMAL),
         SHR_E_NOT_FOUND);

    /* Delete VLAN_EGR_STG_PROFILEs. (shared by all vlan id) */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "VLAN_EGR_STG_PROFILE", &entry_hdl_3));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl_3, "VLAN_EGR_STG_PROFILE_ID",
                               EGR_STG_PROFILE_ID));

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmlt_entry_commit(entry_hdl_3, BCMLT_OPCODE_DELETE,
                            BCMLT_PRIORITY_NORMAL),
         SHR_E_NOT_FOUND);
exit:
    if (entry_hdl_1 != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl_1);
    }
    if (entry_hdl_2 != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl_2);
    }
    if (entry_hdl_3 != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl_3);
    }

    SHR_FUNC_EXIT();
}

static int
xfs_vlan_create(int unit, int vid, int ing_egrmbr_pid)
{
    bcmlt_transaction_hdl_t trans_hdl = BCMLT_INVALID_HDL;
    bcmlt_transaction_info_t trans_info;
    bcmlt_entry_handle_t entry_hdl;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_transaction_allocate(BCMLT_TRANS_TYPE_BATCH, &trans_hdl));

    /* Add vlan profile */
    SHR_IF_ERR_EXIT
        (xfs_vlan_profile_add(unit));

    /* Insert VLAN_ING_MEMBER_PROFILE. */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "VLAN_ING_MEMBER_PROFILE", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "VLAN_ING_MEMBER_PROFILE_ID",
                               ing_egrmbr_pid));

    SHR_IF_ERR_EXIT
            (bcmlt_transaction_entry_add(trans_hdl, BCMLT_OPCODE_INSERT,
                                         entry_hdl));

    /* Insert VLAN_ING_EGR_MEMBER_PORTS_PROFILE. */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "VLAN_ING_EGR_MEMBER_PORTS_PROFILE",
                              &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl,
                               "VLAN_ING_EGR_MEMBER_PORTS_PROFILE_ID",
                               ing_egrmbr_pid));

    SHR_IF_ERR_EXIT
            (bcmlt_transaction_entry_add(trans_hdl, BCMLT_OPCODE_INSERT,
                                         entry_hdl));

    /* Insert ING_VFI_TABLE. */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "ING_VFI_TABLE", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "VFI", vid));

    SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, "MAPPED_VFI", vid));

    SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, "SPANNING_TREE_GROUP",
                                   ING_STG_PROFILE_ID));

    SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, "MEMBERSHIP_PROFILE_PTR",
                                   ing_egrmbr_pid));

    SHR_IF_ERR_EXIT
            (bcmlt_transaction_entry_add(trans_hdl, BCMLT_OPCODE_INSERT,
                                         entry_hdl));

    /* Insert VLAN_EGR_MEMBER_PROFILE. */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "VLAN_EGR_MEMBER_PROFILE", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "VLAN_EGR_MEMBER_PROFILE_ID",
                               ing_egrmbr_pid));

    SHR_IF_ERR_EXIT
            (bcmlt_transaction_entry_add(trans_hdl, BCMLT_OPCODE_INSERT,
                                         entry_hdl));

    /* Insert EGR_VFI. */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "EGR_VFI", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "VFI", vid));

    /* Egress VLAN OTAG. */
    SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, "VLAN_0", vid));

    SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, "STG", EGR_STG_PROFILE_ID));

    SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, "MEMBERSHIP_PROFILE_IDX",
                                   ing_egrmbr_pid));

    SHR_IF_ERR_EXIT
            (bcmlt_transaction_entry_add(trans_hdl, BCMLT_OPCODE_INSERT,
                                         entry_hdl));

    /* Submit transaction. */
    SHR_IF_ERR_EXIT
        (bcmlt_transaction_commit(trans_hdl, BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_EXIT
        (bcmlt_transaction_info_get(trans_hdl, &trans_info));
    if (SHR_FAILURE(trans_info.status)) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

exit:
    if (trans_hdl != BCMLT_INVALID_HDL) {
        bcmlt_transaction_free(trans_hdl);
    }

    SHR_FUNC_EXIT();
}

static int
xfs_vlan_destroy(int unit, int vid)
{
    bcmlt_transaction_hdl_t trans_hdl = BCMLT_INVALID_HDL;
    bcmlt_transaction_info_t trans_info;
    bcmlt_entry_handle_t entry_hdl;
    int member_pid;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (vlan_ing_egrmbr_profile_id_get(unit, vid, &member_pid));

    SHR_IF_ERR_EXIT
        (bcmlt_transaction_allocate(BCMLT_TRANS_TYPE_BATCH, &trans_hdl));


    /* Delete VLAN_ING_MEMBER_PROFILE. */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "VLAN_ING_MEMBER_PROFILE", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "VLAN_ING_MEMBER_PROFILE_ID",
                               member_pid));

    SHR_IF_ERR_EXIT
            (bcmlt_transaction_entry_add(trans_hdl, BCMLT_OPCODE_DELETE,
                                         entry_hdl));

    /* Delete VLAN_ING_EGR_MEMBER_PORTS_PROFILE. */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "VLAN_ING_EGR_MEMBER_PORTS_PROFILE",
                              &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl,
                               "VLAN_ING_EGR_MEMBER_PORTS_PROFILE_ID",
                               member_pid));

    SHR_IF_ERR_EXIT
            (bcmlt_transaction_entry_add(trans_hdl, BCMLT_OPCODE_DELETE,
                                         entry_hdl));

    /* Delete ING_VFI_TABLE. */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "ING_VFI_TABLE", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "VFI", vid));

    SHR_IF_ERR_EXIT
            (bcmlt_transaction_entry_add(trans_hdl, BCMLT_OPCODE_DELETE,
                                         entry_hdl));

    /* Delete VLAN_EGR_MEMBER_PROFILE. */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "VLAN_EGR_MEMBER_PROFILE", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "VLAN_EGR_MEMBER_PROFILE_ID",
                               member_pid));

    SHR_IF_ERR_EXIT
            (bcmlt_transaction_entry_add(trans_hdl, BCMLT_OPCODE_DELETE,
                                         entry_hdl));

    /* Delete EGR_VFI. */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "EGR_VFI", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "VFI", vid));

    SHR_IF_ERR_EXIT
            (bcmlt_transaction_entry_add(trans_hdl, BCMLT_OPCODE_DELETE,
                                         entry_hdl));

    /* Submit transaction. */
    SHR_IF_ERR_EXIT
        (bcmlt_transaction_commit(trans_hdl, BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_EXIT
        (bcmlt_transaction_info_get(trans_hdl, &trans_info));
    if (SHR_FAILURE(trans_info.status)) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    /* Delete vlan profile */
    SHR_IF_ERR_EXIT
        (xfs_vlan_profile_del(unit));

exit:
    if (trans_hdl != BCMLT_INVALID_HDL) {
        bcmlt_transaction_free(trans_hdl);
    }

    SHR_FUNC_EXIT();
}

static int
xfs_vlan_port_interface_add(int unit, int port, int vid, bool src_port_block)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    /* Insert ING_SYSTEM_DESTINATION_TABLE. */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "ING_SYSTEM_DESTINATION_TABLE",
                              &entry_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "SYSTEM_DESTINATION_PORT",
                               port));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "L2_OIF", port));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "L2_OIF_STRENGTH", 1));

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL),
         SHR_E_EXISTS);

    SHR_IF_ERR_EXIT(bcmlt_entry_free(entry_hdl));
    entry_hdl = BCMLT_INVALID_HDL;

    /* Insert ING_SYSTEM_PORT_TABLE. */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "ING_SYSTEM_PORT_TABLE", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "SYSTEM_SOURCE", port));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "L2_IIF", port));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "INGRESS_PP_PORT", port));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "L2_IIF_STRENGTH", 1));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "OUTER_L2HDR_OTPID_ENABLE", 0xf));

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL),
         SHR_E_EXISTS);

    SHR_IF_ERR_EXIT(bcmlt_entry_free(entry_hdl));
    entry_hdl = BCMLT_INVALID_HDL;

    /* Insert PORT_SYSTEM_DESTINATION. */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "PORT_SYSTEM_DESTINATION", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "PORT_SYSTEM_ID", port));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "PORT_ID", port));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "IS_TRUNK_SYSTEM", 0));

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL),
         SHR_E_EXISTS);

    SHR_IF_ERR_EXIT(bcmlt_entry_free(entry_hdl));
    entry_hdl = BCMLT_INVALID_HDL;

    /* Insert L2_EIF_SYSTEM_DESTINATION. */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "L2_EIF_SYSTEM_DESTINATION", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "L2_EIF_ID", port));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "SYSTEM_PORT", port));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "IS_TRUNK", 0));

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL),
         SHR_E_EXISTS);

    SHR_IF_ERR_EXIT(bcmlt_entry_free(entry_hdl));
    entry_hdl = BCMLT_INVALID_HDL;

    /* Insert ING_L2_IIF_TABLE. */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "ING_L2_IIF_TABLE", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "L2_IIF", port));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "VFI", vid));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "VLAN_TO_VFI_MAPPING_ENABLE", 1));

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL),
         SHR_E_EXISTS);

    SHR_IF_ERR_EXIT(bcmlt_entry_free(entry_hdl));
    entry_hdl = BCMLT_INVALID_HDL;

    /* Insert ING_L2_IIF_ATTRIBUTES_TABLE */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "ING_L2_IIF_ATTRIBUTES_TABLE", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "L2_IIF", port));

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL),
         SHR_E_EXISTS);

    SHR_IF_ERR_EXIT(bcmlt_entry_free(entry_hdl));
    entry_hdl = BCMLT_INVALID_HDL;

    /* update src_port_block if need */
    if (src_port_block == false) {
        int i;
        int table_id = -1;
        const char *possible_table[2] = {"ING_L2_IIF_TABLE",
                                         "ING_L2_IIF_ATTRIBUTES_TABLE"};

        for (i = 0; i < COUNTOF(possible_table); i++) {
            const char *fld_desc = NULL;
            int rv = bcmlt_field_desc_get
                        (unit, possible_table[i],
                         "SRC_PORT_KNOCKOUT_DISABLE", &fld_desc);
            if (rv == SHR_E_NONE) {
                table_id = i;
                break;
            }
        }

        if (table_id == -1) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(unit, possible_table[table_id], &entry_hdl));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, "L2_IIF", port));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl,
                                   "SRC_PORT_KNOCKOUT_DISABLE", 1));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_UPDATE,
                                BCMLT_PRIORITY_NORMAL));

        SHR_IF_ERR_EXIT(bcmlt_entry_free(entry_hdl));
        entry_hdl = BCMLT_INVALID_HDL;
    }

    /* Insert EGR_L2_OIF. */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "EGR_L2_OIF", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "L2_OIF", port));

    /* Do not modify L2 switched packet. */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "EDIT_CTRL_ID", 1));

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL),
         SHR_E_EXISTS);

    SHR_IF_ERR_EXIT(bcmlt_entry_free(entry_hdl));
    entry_hdl = BCMLT_INVALID_HDL;

    /* Insert Port */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "PORT", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "PORT_ID", port));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "ING_SYSTEM_PORT_TABLE_ID", port));

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL),
         SHR_E_EXISTS);

    SHR_IF_ERR_EXIT(bcmlt_entry_free(entry_hdl));
    entry_hdl = BCMLT_INVALID_HDL;

    /* Insert PORT_PORPERTY */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              PORT_PROPERTYs, &entry_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl,
                               PORT_IDs, port)); /* port = 1 */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, PORT_PARSERs, 0x3));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, PORT_TYPEs, 0x1));

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL),
         SHR_E_EXISTS);

    SHR_IF_ERR_EXIT(bcmlt_entry_free(entry_hdl));
    entry_hdl = BCMLT_INVALID_HDL;

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
xfs_vlan_port_interface_delete(int unit, int port)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    /* Delete ING_SYSTEM_DESTINATION_TABLE. */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "ING_SYSTEM_DESTINATION_TABLE",
                              &entry_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "SYSTEM_DESTINATION_PORT",
                               port));
    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_DELETE,
                            BCMLT_PRIORITY_NORMAL),
         SHR_E_NOT_FOUND);

    SHR_IF_ERR_EXIT(bcmlt_entry_free(entry_hdl));
    entry_hdl = BCMLT_INVALID_HDL;

    /* Delete ING_SYSTEM_PORT_TABLE. */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "ING_SYSTEM_PORT_TABLE", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "SYSTEM_SOURCE", port));

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_DELETE,
                            BCMLT_PRIORITY_NORMAL),
         SHR_E_NOT_FOUND);

    SHR_IF_ERR_EXIT(bcmlt_entry_free(entry_hdl));
    entry_hdl = BCMLT_INVALID_HDL;

    /* Delete PORT_SYSTEM_DESTINATION. */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "PORT_SYSTEM_DESTINATION", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "PORT_SYSTEM_ID", port));

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_DELETE,
                            BCMLT_PRIORITY_NORMAL),
         SHR_E_NOT_FOUND);

    SHR_IF_ERR_EXIT(bcmlt_entry_free(entry_hdl));
    entry_hdl = BCMLT_INVALID_HDL;

    /* Delete L2_EIF_SYSTEM_DESTINATION. */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "L2_EIF_SYSTEM_DESTINATION", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "L2_EIF_ID", port));

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_DELETE,
                            BCMLT_PRIORITY_NORMAL),
         SHR_E_NOT_FOUND);

    SHR_IF_ERR_EXIT(bcmlt_entry_free(entry_hdl));
    entry_hdl = BCMLT_INVALID_HDL;

    /* Delete ING_L2_IIF_TABLE. */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "ING_L2_IIF_TABLE", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "L2_IIF", port));

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_DELETE,
                            BCMLT_PRIORITY_NORMAL),
         SHR_E_NOT_FOUND);

    SHR_IF_ERR_EXIT(bcmlt_entry_free(entry_hdl));
    entry_hdl = BCMLT_INVALID_HDL;

    /* Delete ING_L2_IIF_ATTRIBUTES_TABLE. */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "ING_L2_IIF_ATTRIBUTES_TABLE", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "L2_IIF", port));

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_DELETE,
                            BCMLT_PRIORITY_NORMAL),
         SHR_E_NOT_FOUND);

    SHR_IF_ERR_EXIT(bcmlt_entry_free(entry_hdl));
    entry_hdl = BCMLT_INVALID_HDL;

    /* Delete EGR_L2_OIF. */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "EGR_L2_OIF", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "L2_OIF", port));

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_DELETE,
                            BCMLT_PRIORITY_NORMAL),
         SHR_E_NOT_FOUND);

    SHR_IF_ERR_EXIT(bcmlt_entry_free(entry_hdl));
    entry_hdl = BCMLT_INVALID_HDL;

    /* Delete PORT. */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "PORT", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "PORT_ID", port));

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_DELETE,
                            BCMLT_PRIORITY_NORMAL),
         SHR_E_NOT_FOUND);

    SHR_IF_ERR_EXIT(bcmlt_entry_free(entry_hdl));
    entry_hdl = BCMLT_INVALID_HDL;

    /* Delete PORT_PROPERTY */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "PORT_PROPERTY", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "PORT_ID", port));

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_DELETE,
                            BCMLT_PRIORITY_NORMAL),
         SHR_E_NOT_FOUND);

    SHR_IF_ERR_EXIT(bcmlt_entry_free(entry_hdl));
    entry_hdl = BCMLT_INVALID_HDL;

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}


static int
xfs_vlan_port_add(int unit, int vid, const bcmdrd_pbmp_t pbmp,
                  const bcmdrd_pbmp_t ubmp, bool src_port_block)
{
    int port, member_pid;

    SHR_FUNC_ENTER(unit);

    /* Add port interface (iif/oif) */
    BCMDRD_PBMP_ITER(pbmp, port) {
        SHR_IF_ERR_EXIT
            (xfs_vlan_port_interface_add(unit, port, vid, src_port_block));
    }

    /* update member profile */
    SHR_IF_ERR_EXIT
        (vlan_ing_egrmbr_profile_id_get(unit, vid, &member_pid));

    SHR_IF_ERR_EXIT
        (vlan_ing_member_profile_update_ports(unit, member_pid, pbmp, 1));

    SHR_IF_ERR_EXIT
        (vlan_ing_egr_member_ports_profile_update_ports(unit, member_pid,
                                                        pbmp, 1));
    SHR_IF_ERR_EXIT
        (vlan_egr_member_profile_update_ports(unit, member_pid, pbmp, 1));

exit:
    SHR_FUNC_EXIT();
}

static int
xfs_vlan_port_vlan_set(int unit, int port, int vid)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "ING_L2_IIF_TABLE", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "L2_IIF", port));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "VFI", vid));

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
xfs_vlan_check(int unit, int vid)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "ING_VFI_TABLE", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "VFI", vid));

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
xfs_vlan_port_remove(int unit, int vid, const bcmdrd_pbmp_t pbmp)
{
    int port, member_pid;

    SHR_FUNC_ENTER(unit);

    /* Delete port interface (iif/oif) */
    BCMDRD_PBMP_ITER(pbmp, port) {
        SHR_IF_ERR_EXIT
            (xfs_vlan_port_interface_delete(unit, port));
    }

    /* Update member profile */
    SHR_IF_ERR_EXIT
        (vlan_ing_egrmbr_profile_id_get(unit, vid, &member_pid));

    SHR_IF_ERR_EXIT
        (vlan_ing_member_profile_update_ports(unit, member_pid, pbmp, 0));

    SHR_IF_ERR_EXIT
        (vlan_ing_egr_member_ports_profile_update_ports(unit, member_pid,
                                                        pbmp, 0));
    SHR_IF_ERR_EXIT
        (vlan_egr_member_profile_update_ports(unit, member_pid, pbmp, 0));

exit:
    SHR_FUNC_EXIT();
}

static int
xfs_vlan_stp_set(int unit, int vid, const bcmdrd_pbmp_t pbmp,
                 bcma_testutil_stp_state_t stp_state)
{
    bcmlt_transaction_hdl_t trans_hdl = BCMLT_INVALID_HDL;
    bcmlt_transaction_info_t trans_info;
    bcmlt_entry_handle_t entry_hdl0, entry_hdl1, entry_hdl2;
    const char *state[1];
    int port;

    SHR_FUNC_ENTER(unit);

    state[0] = bcma_testutil_id2name(bcma_testutil_stp_state_name_id_map,
                                     BCMA_TESTUTIL_STP_COUNT, stp_state);

    SHR_IF_ERR_EXIT
        (bcmlt_transaction_allocate(BCMLT_TRANS_TYPE_BATCH, &trans_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "VLAN_ING_STG_PROFILE", &entry_hdl0));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl0, "VLAN_ING_STG_PROFILE_ID",
                               ING_STG_PROFILE_ID));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "VLAN_ING_EGR_STG_MEMBER_PROFILE",
                              &entry_hdl1));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl1, "VLAN_ING_EGR_STG_MEMBER_PROFILE_ID",
                               ING_STG_PROFILE_ID));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "VLAN_EGR_STG_PROFILE", &entry_hdl2));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl2, "VLAN_EGR_STG_PROFILE_ID",
                               EGR_STG_PROFILE_ID));

    BCMDRD_PBMP_ITER(pbmp, port) {
        /* Update VLAN_ING_STG_PROFILE. */
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_array_symbol_add(entry_hdl0, "STATE", port,
                                                state, 1));

        /* Update VLAN_ING_EGR_STG_MEMBER_PROFILE. */
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_array_symbol_add(entry_hdl1, "STATE", port,
                                                state, 1));

        /* Update VLAN_EGR_STG_PROFILE. */
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_array_symbol_add(entry_hdl2, "STATE", port,
                                                state, 1));
    }
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl1, "COMPARED_STATE", state[0]));

    SHR_IF_ERR_EXIT
        (bcmlt_transaction_entry_add(trans_hdl, BCMLT_OPCODE_UPDATE,
                                     entry_hdl0));

    SHR_IF_ERR_EXIT
        (bcmlt_transaction_entry_add(trans_hdl, BCMLT_OPCODE_UPDATE,
                                     entry_hdl1));

    SHR_IF_ERR_EXIT
        (bcmlt_transaction_entry_add(trans_hdl, BCMLT_OPCODE_UPDATE,
                                     entry_hdl2));

    /* Submit transaction. */
    SHR_IF_ERR_EXIT
        (bcmlt_transaction_commit(trans_hdl, BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_EXIT
        (bcmlt_transaction_info_get(trans_hdl, &trans_info));
    if (SHR_FAILURE(trans_info.status)) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

exit:
    if (trans_hdl != BCMLT_INVALID_HDL) {
        bcmlt_transaction_free(trans_hdl);
    }

    SHR_FUNC_EXIT();
}

static bcma_testutil_vlan_op_t bcma_testutil_xfs_vlan_op = {
    .check = xfs_vlan_check,
    .create = xfs_vlan_create,
    .destroy = xfs_vlan_destroy,
    .port_add = xfs_vlan_port_add,
    .port_update = NULL,
    .port_remove = xfs_vlan_port_remove,
    .port_vlan_set = xfs_vlan_port_vlan_set,
    .stp_set = xfs_vlan_stp_set,
    .untag_action_add = NULL,
    .untag_action_remove = NULL
};

bcma_testutil_vlan_op_t *bcma_testutil_xfs_vlan_op_get(int unit)
{
    return &bcma_testutil_xfs_vlan_op;
}
