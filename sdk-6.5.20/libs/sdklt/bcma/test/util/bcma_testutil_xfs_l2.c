/*! \file bcma_testutil_xfs_l2.c
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
#include <bcmltd/chip/bcmltd_str.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcma/test/util/bcma_testutil_l2.h>

#define BSL_LOG_MODULE BSL_LS_APPL_TEST

#define L2_STRENGTH_PROFILE_ID            7
#define L3_NEXT_HOP_1_STRENGTH_PROFILE_ID 1
#define L3_NEXT_HOP_2_STRENGTH_PROFILE_ID 2
#define DVP_STRENGTH_PROFILE_ID           1

static int
xfs_l2_tile_mode_update(int unit)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    const char *l2_bank[2] = {"IFTA80_E2T_02_BANK0", "IFTA80_E2T_02_BANK1"};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DEVICE_EM_TILE", &entry_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, "DEVICE_EM_TILE_ID",
                                      "IFTA80_E2T_02"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, "MODE",
                                      "IFTA80_E2T_02_MODE0"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_UPDATE,
                            BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry_hdl);
    entry_hdl = BCMLT_INVALID_HDL;


    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DEVICE_EM_GROUP", &entry_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, "DEVICE_EM_GROUP_ID",
                                      "IFTA80_E2T_02_MODE0_GROUP0"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "NUM_BANKS", 2));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_symbol_add(entry_hdl, "DEVICE_EM_BANK_ID", 0,
                                            l2_bank, 2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_UPDATE,
                            BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry_hdl);
    entry_hdl = BCMLT_INVALID_HDL;


    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "TABLE_CONTROL", &entry_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, "TABLE_ID", "L2_HOST_TABLE"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "MAX_ENTRIES", 8192));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_UPDATE,
                            BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry_hdl);
    entry_hdl = BCMLT_INVALID_HDL;

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
xfs_l2_strength_profile_add(int unit)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    /* Insert L2_HOST_STRENGTH_PROFILE. (shared by all l2 entry) */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "L2_HOST_STRENGTH_PROFILE", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "L2_HOST_STRENGTH_PROFILE_INDEX",
                               L2_STRENGTH_PROFILE_ID));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "L2_DST_DESTINATION_STRENGTH", 7));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "L2_DST_DESTINATION_TYPE_STRENGTH",
                               7));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "L2_DST_MISC_CTRL_0_STRENGTH", 7));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "L2_DST_MISC_CTRL_1_STRENGTH", 1));

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL),
         SHR_E_EXISTS);

    bcmlt_entry_free(entry_hdl);
    entry_hdl = BCMLT_INVALID_HDL;

    /* Insert ING_L3_NEXT_HOP_[1/2]_STRENGTH_PROFILE. (shared by all l2 entry) */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "ING_L3_NEXT_HOP_1_TABLE", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl,
                               "STRENGTH_PROFILE_INDEX",
                               L3_NEXT_HOP_1_STRENGTH_PROFILE_ID));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "NHOP_INDEX_1", 0));

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL),
         SHR_E_EXISTS);

    bcmlt_entry_free(entry_hdl);
    entry_hdl = BCMLT_INVALID_HDL;

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "ING_L3_NEXT_HOP_1_STRENGTH_PROFILE",
                              &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl,
                               "ING_L3_NEXT_HOP_1_STRENGTH_PROFILE_INDEX",
                               L3_NEXT_HOP_1_STRENGTH_PROFILE_ID));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "DESTINATION_STRENGTH", 0));

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL),
         SHR_E_EXISTS);

    bcmlt_entry_free(entry_hdl);
    entry_hdl = BCMLT_INVALID_HDL;

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "ING_L3_NEXT_HOP_2_TABLE",  &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl,
                               "STRENGTH_PROFILE_INDEX",
                               L3_NEXT_HOP_2_STRENGTH_PROFILE_ID));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "NHOP_INDEX_2", 0));

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL),
         SHR_E_EXISTS);

    bcmlt_entry_free(entry_hdl);
    entry_hdl = BCMLT_INVALID_HDL;

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "ING_L3_NEXT_HOP_2_STRENGTH_PROFILE",
                              &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl,
                               "ING_L3_NEXT_HOP_2_STRENGTH_PROFILE_INDEX",
                               L3_NEXT_HOP_2_STRENGTH_PROFILE_ID));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "DESTINATION_STRENGTH", 0));

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL),
         SHR_E_EXISTS);

    bcmlt_entry_free(entry_hdl);
    entry_hdl = BCMLT_INVALID_HDL;

    /* ING_DVP_STRENGTH_PROFILE (shared by all l2 entry) */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "ING_DVP_TABLE", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl,
                               "STRENGTH_PROFILE_INDEX",
                               DVP_STRENGTH_PROFILE_ID));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "DVP", 0));

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL),
         SHR_E_EXISTS);

    bcmlt_entry_free(entry_hdl);
    entry_hdl = BCMLT_INVALID_HDL;

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "ING_DVP_STRENGTH_PROFILE",
                              &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl,
                               "ING_DVP_STRENGTH_PROFILE_INDEX",
                               DVP_STRENGTH_PROFILE_ID));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "DESTINATION_STRENGTH", 0));

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL),
         SHR_E_EXISTS);

    bcmlt_entry_free(entry_hdl);
    entry_hdl = BCMLT_INVALID_HDL;

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
xfs_l2_strength_profile_delete(int unit)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    /* Delete L2_HOST_STRENGTH_PROFILE. (shared by all l2 entry) */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "L2_HOST_STRENGTH_PROFILE", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "L2_HOST_STRENGTH_PROFILE_INDEX",
                               L2_STRENGTH_PROFILE_ID));

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_DELETE,
                            BCMLT_PRIORITY_NORMAL),
         SHR_E_NOT_FOUND);

    bcmlt_entry_free(entry_hdl);
    entry_hdl = BCMLT_INVALID_HDL;

    /* Delete ING_L3_NEXT_HOP_[1/2]_STRENGTH_PROFILE. (shared by all l2 entry) */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "ING_L3_NEXT_HOP_1_TABLE", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "NHOP_INDEX_1", 0));

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_DELETE,
                            BCMLT_PRIORITY_NORMAL),
         SHR_E_NOT_FOUND);

    bcmlt_entry_free(entry_hdl);
    entry_hdl = BCMLT_INVALID_HDL;

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "ING_L3_NEXT_HOP_1_STRENGTH_PROFILE",
                              &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl,
                               "ING_L3_NEXT_HOP_1_STRENGTH_PROFILE_INDEX",
                               L3_NEXT_HOP_1_STRENGTH_PROFILE_ID));

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_DELETE,
                            BCMLT_PRIORITY_NORMAL),
         SHR_E_NOT_FOUND);

    bcmlt_entry_free(entry_hdl);
    entry_hdl = BCMLT_INVALID_HDL;

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "ING_L3_NEXT_HOP_2_TABLE", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "NHOP_INDEX_2", 0));

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_DELETE,
                            BCMLT_PRIORITY_NORMAL),
         SHR_E_NOT_FOUND);

    bcmlt_entry_free(entry_hdl);
    entry_hdl = BCMLT_INVALID_HDL;

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "ING_L3_NEXT_HOP_2_STRENGTH_PROFILE",
                              &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl,
                               "ING_L3_NEXT_HOP_2_STRENGTH_PROFILE_INDEX",
                               L3_NEXT_HOP_2_STRENGTH_PROFILE_ID));

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_DELETE,
                            BCMLT_PRIORITY_NORMAL),
         SHR_E_NOT_FOUND);

    bcmlt_entry_free(entry_hdl);
    entry_hdl = BCMLT_INVALID_HDL;

    /* Delete ING_DVP_STRENGTH_PROFILE. (shared by all l2 entry) */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "ING_DVP_TABLE", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "DVP", 0));

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_DELETE,
                            BCMLT_PRIORITY_NORMAL),
         SHR_E_NOT_FOUND);

    bcmlt_entry_free(entry_hdl);
    entry_hdl = BCMLT_INVALID_HDL;

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "ING_DVP_STRENGTH_PROFILE", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl,
                               "ING_DVP_STRENGTH_PROFILE_INDEX",
                               DVP_STRENGTH_PROFILE_ID));

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_DELETE,
                            BCMLT_PRIORITY_NORMAL),
         SHR_E_NOT_FOUND);

    bcmlt_entry_free(entry_hdl);
    entry_hdl = BCMLT_INVALID_HDL;

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}


static int
xfs_l2_unicast_add(int unit, int port, const shr_mac_t mac,
                   int vid, int copy2cpu)
{
    bcmlt_transaction_hdl_t trans_hdl = BCMLT_INVALID_HDL;
    bcmlt_transaction_info_t trans_info;
    bcmlt_entry_handle_t entry_hdl;
    uint64_t mac_addr = 0, tmp;
    int idx;

    SHR_FUNC_ENTER(unit);

    for (idx = 0; idx < 6; idx++) {
        tmp = mac[idx];
        mac_addr <<= 8;
        mac_addr |= tmp;
    }

    SHR_IF_ERR_EXIT
        (bcmlt_transaction_allocate(BCMLT_TRANS_TYPE_BATCH, &trans_hdl));

    /* Add L2 profile */
    SHR_IF_ERR_EXIT
        (xfs_l2_strength_profile_add(unit));

    /* update tile mode */
    SHR_IF_ERR_EXIT
        (xfs_l2_tile_mode_update(unit));

    /* Insert L2_HOST_TABLE. */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "L2_HOST_TABLE", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "VFI", vid));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "MAC_ADDR", mac_addr));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "STRENGTH_PROFILE_INDEX",
                               L2_STRENGTH_PROFILE_ID));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "DESTINATION", port));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, "DESTINATION_TYPE", "L2_OIF"));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "MISC_CTRL_0", 1));

    if (copy2cpu) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, "MISC_CTRL_1", 10));
    } else {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, "MISC_CTRL_1", 8));
    }

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
xfs_l2_unicast_udpate(int unit, int port, const shr_mac_t mac,
                      int vid, int copy2cpu)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t mac_addr = 0, tmp;
    int idx;

    SHR_FUNC_ENTER(unit);

    for (idx = 0; idx < 6; idx++) {
        tmp = mac[idx];
        mac_addr <<= 8;
        mac_addr |= tmp;
    }

    /* Insert L2_HOST_TABLE. */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "L2_HOST_TABLE", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "VFI", vid));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "MAC_ADDR", mac_addr));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "DESTINATION", port));

    if (copy2cpu) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, "MISC_CTRL_1", 10));
    } else {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, "MISC_CTRL_1", 8));
    }

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
xfs_l2_unicast_delete(int unit, int port, const shr_mac_t mac,
                      int vid, int copy2cpu)
{
    bcmlt_transaction_hdl_t trans_hdl = BCMLT_INVALID_HDL;
    bcmlt_transaction_info_t trans_info;
    bcmlt_entry_handle_t entry_hdl;
    uint64_t mac_addr = 0, tmp;
    int idx;

    SHR_FUNC_ENTER(unit);

    for (idx = 0; idx < 6; idx++) {
        tmp = mac[idx];
        mac_addr <<= 8;
        mac_addr |= tmp;
    }

    SHR_IF_ERR_EXIT
        (bcmlt_transaction_allocate(BCMLT_TRANS_TYPE_BATCH, &trans_hdl));

    /* Delete L2_HOST_TABLE. */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "L2_HOST_TABLE", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "VFI", vid));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "MAC_ADDR", mac_addr));

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

    /* Delete L2 profile */
    SHR_IF_ERR_EXIT
        (xfs_l2_strength_profile_delete(unit));

exit:
    if (trans_hdl != BCMLT_INVALID_HDL) {
        bcmlt_transaction_free(trans_hdl);
    }

    SHR_FUNC_EXIT();
}

static int
xfs_l2_multicast_add(int unit, int group_id, bcmdrd_pbmp_t pbmp,
                     const shr_mac_t mac, int vid,
                     int copy2cpu)
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

    /* Add L2 profile */
    SHR_IF_ERR_EXIT
        (xfs_l2_strength_profile_add(unit));

    /* update tile mode */
    SHR_IF_ERR_EXIT
        (xfs_l2_tile_mode_update(unit));

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

    /* Insert L2_HOST_TABLE */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "L2_HOST_TABLE", &entry_hdl_2));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl_2, "VFI", vid));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl_2, "MAC_ADDR", mac_addr));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl_2, "STRENGTH_PROFILE_INDEX",
                               L2_STRENGTH_PROFILE_ID));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl_2, "DESTINATION", group_id));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl_2, "DESTINATION_TYPE",
                                      "L2MC_GROUP"));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl_2, "MISC_CTRL_0", 1));

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
xfs_l2_multicast_delete(int unit, int group_id, bcmdrd_pbmp_t pbmp,
                        const shr_mac_t mac, int vid,
                        int copy2cpu)
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

    /* Delete L2_HOST_TABLE */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "L2_HOST_TABLE", &entry_hdl_2));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl_2, "VFI", vid));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl_2, "MAC_ADDR", mac_addr));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl_2, BCMLT_OPCODE_DELETE,
                            BCMLT_PRIORITY_NORMAL));

    /* Delete L2 profile */
    SHR_IF_ERR_EXIT
        (xfs_l2_strength_profile_delete(unit));

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
xfs_l2_multicast_update(int unit, int group_id, bcmdrd_pbmp_t pbmp,
                     const shr_mac_t mac, int vid,
                     int copy2cpu)
{
    bcmlt_entry_handle_t entry_hdl_1 = BCMLT_INVALID_HDL;
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
        (bcmlt_entry_commit(entry_hdl_1, BCMLT_OPCODE_UPDATE,
                            BCMLT_PRIORITY_NORMAL));

exit:
    if (entry_hdl_1 != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl_1);
    }
    SHR_FREE(l2mc_fields_array);
    SHR_FREE(portid_data);

    SHR_FUNC_EXIT();
}

static bcma_testutil_l2_op_t bcma_testutil_xfs_l2_op = {
    .unicast_add = xfs_l2_unicast_add,
    .unicast_update = xfs_l2_unicast_udpate,
    .unicast_delete = xfs_l2_unicast_delete,
    .multicast_add = xfs_l2_multicast_add,
    .multicast_delete = xfs_l2_multicast_delete,
    .multicast_update = xfs_l2_multicast_update
};

bcma_testutil_l2_op_t *bcma_testutil_xfs_l2_op_get(int unit)
{
    return &bcma_testutil_xfs_l2_op;
}
