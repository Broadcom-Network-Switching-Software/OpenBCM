/*! \file bcm56880_a0_bcma_pkttest_drv.c
 *
 * Chip specific functions for Packet I/O test.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <shr/shr_error.h>
#include <sal/sal_types.h>
#include <sal/sal_alloc.h>
#include <sal/sal_libc.h>
#include <sal/sal_sleep.h>
#include <bcmlt/bcmlt.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcma/bcmpkt/bcma_bcmpkt_test.h>
#include <bcmltd/chip/bcmltd_str.h>
#include <bcma/bcmpkt/bcma_bcmpkt_test_internal.h>

#define BSL_LOG_MODULE BSL_LS_APPL_TEST

static uint64_t pdd_profile_id = 1;
static uint64_t sbr_profile_id = 1;
static uint64_t presel_grp_id = 1;
static uint64_t presel_id = 1;

static uint64_t ifp_copy_policy_id = 0;
static uint64_t ifp_drop_policy_id = 0;

static uint64_t trace_event_entry_existed = 0;
static uint64_t trace_event_cpu_fld_set = 0;

static int
validate_lt_support(int unit, const char *table_name,
                    const char *field_name)
{
    bcmltd_sid_t sid = 0;
    bcmltd_fid_t fid = 0;
    int rv = SHR_E_NONE;

    rv = bcmlrd_table_field_name_to_idx(unit,
                                        table_name,
                                        field_name,
                                        &sid,
                                        &fid);
    if (SHR_SUCCESS(rv)) {
        /* Supported LT table and field. */
        return 1;
    } else {
        /* Un-supported LT table and field. */
        return 0;
    }
}

static int
find_first_unused_lt_entry(int unit, const char *table_name,
                           const char *field_name, uint64_t *entry_id)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    uint64_t data = 1;
    int status;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, table_name, &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl, field_name, data));

    while (1) {
        status = bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_LOOKUP,
                                    BCMLT_PRIORITY_NORMAL);
        if (status == SHR_E_NOT_FOUND) {
            break;
        }
        SHR_IF_ERR_EXIT
            (status);
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(ent_hdl, field_name, ++data));
    }
    *entry_id = data;

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
fwd_path_create(int unit, int port,
                bcma_bcmpkt_test_fp_cfg_t *ifp_cfg)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    uint64_t data[2];
    const char *symbols[2];

    SHR_FUNC_ENTER(unit);

    /*
     * Forward path setup.
     */
    /* Default L3 strength profile configuration */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "ING_DVP_TABLE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "DVP", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "STRENGTH_PROFILE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(ent_hdl);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "ING_DVP_STRENGTH_PROFILE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "ING_DVP_STRENGTH_PROFILE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "L2MC_L3MC_L2_OIF_SYS_DST_VALID_STRENGTH", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "DESTINATION_STRENGTH", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(ent_hdl);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "ING_L3_NEXT_HOP_1_TABLE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "NHOP_INDEX_1", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(ent_hdl, "VIEW_T",
                                      "L2_OIF"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "STRENGTH_PROFILE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(ent_hdl);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "ING_L3_NEXT_HOP_2_TABLE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "NHOP_INDEX_2", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "STRENGTH_PROFILE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(ent_hdl);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "ING_L3_NEXT_HOP_1_STRENGTH_PROFILE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "ING_L3_NEXT_HOP_1_STRENGTH_PROFILE_INDEX", 0x1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "L2MC_L3MC_L2_OIF_SYS_DST_VALID_STRENGTH", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "DESTINATION_STRENGTH", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(ent_hdl);

    /* PORT LT config */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "PORT", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "PORT_ID", port)); /* port = 1 */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "ING_SYSTEM_PORT_TABLE_ID", 0x1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(ent_hdl);

    /* Configure the port parser control */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "PORT_PROPERTY", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "PORT_ID", port)); /* port = 1 */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "PORT_PARSER", 0x3));
        SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "PORT_TYPE", 0x1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(ent_hdl);

    /* Ingress System Port Config */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "ING_SYSTEM_PORT_TABLE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "SYSTEM_SOURCE", 0x1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "OUTER_L2HDR_OTPID_ENABLE", 0xf));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "L2_IIF", 0x2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "L2_IIF_STRENGTH", 0x1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "INGRESS_PP_PORT", port));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(ent_hdl);

    /* Ingress VLAN tag assignment */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "ING_L2_IIF_TABLE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "L2_IIF", 0x2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "VLAN_TO_VFI_MAPPING_ENABLE", 0x1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "VFI", 0x2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(ent_hdl);

    /* Ingress VLAN table */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "ING_VFI_TABLE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "VFI", 0x2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "STRENGTH_PROFILE_INDEX", 0x1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "MAPPED_VFI", 0x2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "SPANNING_TREE_GROUP", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "MEMBERSHIP_PROFILE_PTR", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(ent_hdl);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "VLAN_ING_MEMBER_PROFILE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "VLAN_ING_MEMBER_PROFILE_ID", 0));
    data[0] = 1;
    data[1] = 1;
    SHR_IF_ERR_EXIT
         (bcmlt_entry_field_array_add(ent_hdl,
                                      "MEMBER_PORTS", 0, data, 2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(ent_hdl);

    /* Ingress VLAN STG 0 */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "VLAN_ING_STG_PROFILE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "VLAN_ING_STG_PROFILE_ID", 0));
    symbols[0] = "FORWARD";
    symbols[1] = "FORWARD";
    SHR_IF_ERR_EXIT
         (bcmlt_entry_field_array_symbol_add(ent_hdl,
                                             "STATE", 0, symbols, 2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(ent_hdl);

    /* VLAN port block mask for membership and STG */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "VLAN_ING_EGR_MEMBER_PORTS_PROFILE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "VLAN_ING_EGR_MEMBER_PORTS_PROFILE_ID", 0));
    data[0] = 1;
    data[1] = 1;
    SHR_IF_ERR_EXIT
         (bcmlt_entry_field_array_add(ent_hdl,
                                      "EGR_MEMBER_PORTS", 0, data, 2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(ent_hdl);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "VLAN_ING_EGR_STG_MEMBER_PROFILE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "VLAN_ING_EGR_STG_MEMBER_PROFILE_ID", 0));
    symbols[0] = "FORWARD";
    symbols[1] = "FORWARD";
    SHR_IF_ERR_EXIT
         (bcmlt_entry_field_array_symbol_add(ent_hdl,
                                             "STATE", 0, symbols, 2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(ent_hdl, "COMPARED_STATE", "FORWARD"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(ent_hdl);

    /* Tile mode config. */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "DEVICE_EM_TILE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(ent_hdl, "DEVICE_EM_TILE_ID", "IFTA80_E2T_02"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(ent_hdl, "MODE", "IFTA80_E2T_02_MODE0"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_UPDATE,
                            BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(ent_hdl);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "DEVICE_EM_GROUP", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(ent_hdl, "DEVICE_EM_GROUP_ID", "IFTA80_E2T_02_MODE0_GROUP0"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "NUM_BANKS", 2));
    symbols[0] = "IFTA80_E2T_02_BANK0";
    symbols[1] = "IFTA80_E2T_02_BANK1";
    SHR_IF_ERR_EXIT
         (bcmlt_entry_field_array_symbol_add(ent_hdl,
                                             "DEVICE_EM_BANK_ID", 0, symbols, 2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_UPDATE,
                            BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(ent_hdl);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "TABLE_CONTROL", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(ent_hdl, "TABLE_ID", "L2_HOST_TABLE"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "MAX_ENTRIES", 8192));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_UPDATE,
                            BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(ent_hdl);

    /* L2 hash table for DST lookup */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "L2_HOST_TABLE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "MAC_ADDR", DA_BYTE_5));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "VFI", 0x2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "STRENGTH_PROFILE_INDEX", 0x7));
    /* Destined to other ports than test port to avoid looping. */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "DESTINATION", 2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(ent_hdl, "DESTINATION_TYPE", "L2_OIF"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "MISC_CTRL_0", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "MISC_CTRL_1", 0x8));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(ent_hdl);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "L2_HOST_STRENGTH_PROFILE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "L2_HOST_STRENGTH_PROFILE_INDEX", 0x7));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "L2_DST_DESTINATION_STRENGTH", 7));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "L2_DST_DESTINATION_TYPE_STRENGTH", 7));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "L2_DST_MISC_CTRL_0_STRENGTH", 7));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "L2_DST_MISC_CTRL_1_STRENGTH", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(ent_hdl);

    /* Destination resolution (-> L2_OIF) */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "ING_SYSTEM_DESTINATION_TABLE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "SYSTEM_DESTINATION_PORT", 0x1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "L2_OIF", 0x1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "L2_OIF_STRENGTH", 0x1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(ent_hdl);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "L2_EIF_SYSTEM_DESTINATION", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "L2_EIF_ID", 0x1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "IS_TRUNK", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "SYSTEM_PORT", 0x1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(ent_hdl);

    /* Configure destination port resolution. */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "PORT_SYSTEM_DESTINATION", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "PORT_SYSTEM_ID", 0x1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "IS_TRUNK_SYSTEM", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "PORT_ID", 0x1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(ent_hdl);

    /* Egress VLAN configuration */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "EGR_VFI", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "VFI", 0x2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(ent_hdl, "VIEW_T", "VT_VFI_DEFAULT"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "VLAN_0", 0x2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "TAG_ACTION", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "STG", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "MEMBERSHIP_PROFILE_IDX", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(ent_hdl);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "VLAN_EGR_MEMBER_PROFILE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "VLAN_EGR_MEMBER_PROFILE_ID", 0));
    data[0] = 1;
    data[1] = 1;
    SHR_IF_ERR_EXIT
         (bcmlt_entry_field_array_add(ent_hdl,
                                      "MEMBER_PORTS", 0, data, 2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(ent_hdl);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "VLAN_EGR_STG_PROFILE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "VLAN_EGR_STG_PROFILE_ID", 0));
    symbols[0] = "FORWARD";
    symbols[1] = "FORWARD";
    SHR_IF_ERR_EXIT
         (bcmlt_entry_field_array_symbol_add(ent_hdl,
                                             "STATE", 0, symbols, 2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL));

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
trace_event_create(int unit)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    uint64_t copy_to_cpu_enable;
    int status;

    SHR_FUNC_ENTER(unit);

    /*
     * Enable copy to cpu action from the IFP event
     * (DT_IFP_PDD_TEMPLATE.IFP_SCRATCH_BUS_IFP_COPY_TO_CPU_ACTION) by
     * the following MON_ING_TRACE_EVENT LT configuration.
     */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "MON_ING_TRACE_EVENT", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(ent_hdl,
                                      "MON_ING_TRACE_EVENT_ID", "IFP"));
    /* Get current action state. */
    status = bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_LOOKUP,
                                BCMLT_PRIORITY_NORMAL);
    trace_event_entry_existed = 0;
    trace_event_cpu_fld_set = 0;
    if (status == SHR_E_NONE) {
        trace_event_entry_existed = 1;
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(ent_hdl, "CPU", &copy_to_cpu_enable));
        if (copy_to_cpu_enable == 1) {
            /*
             * The LT was inserted and CPU field was configured prior than the
             * command runs.
             */
            trace_event_cpu_fld_set = 1;
            SHR_EXIT();
        }
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "CPU", 1));
    if (!trace_event_entry_existed) {
        /* The LT was not inserted earlier. */
        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_INSERT,
                                BCMLT_PRIORITY_NORMAL));
    } else {
        /* The LT was inserted earlier but CPU field is not set. */
        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_UPDATE,
                                BCMLT_PRIORITY_NORMAL));
    }

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
trace_event_destroy(int unit)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    int status = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "MON_ING_TRACE_EVENT", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(ent_hdl, "MON_ING_TRACE_EVENT_ID",
                                      "IFP"));
    if (!trace_event_entry_existed) {
        /* The LT was not inserted prior than the command runs. Delete it. */
        status = bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_DELETE,
                                    BCMLT_PRIORITY_NORMAL);
    } else {
        if (!trace_event_cpu_fld_set) {
            /*
             * The LT was inserted prior than the command runs and CPU field
             * was not set. Recover the field.
             */
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(ent_hdl,
                                       "CPU", 0));
            status = bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_UPDATE,
                                        BCMLT_PRIORITY_NORMAL);
        }
    }
    if (status == SHR_E_NOT_FOUND) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("MON_ING_TRACE_EVENT IFP not exist\n")));
    }
    SHR_IF_ERR_EXIT(status);

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
ifp_presel_create(int unit)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    /* Presel group and entry configuration */
    /* find a unused entry */
    SHR_IF_ERR_EXIT
        (find_first_unused_lt_entry(unit, "DT_IFP_PRESEL_GRP_TEMPLATE",
                                    "DT_IFP_PRESEL_GRP_TEMPLATE_INDEX",
                                    &presel_grp_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "DT_IFP_PRESEL_GRP_TEMPLATE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "DT_IFP_PRESEL_GRP_TEMPLATE_INDEX", presel_grp_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(ent_hdl);

    /* find a unused entry */
    SHR_IF_ERR_EXIT
        (find_first_unused_lt_entry(unit, "DT_IFP_PRESEL_TEMPLATE",
                                    "IFP_PRESEL_TEMPLATE_INDEX", &presel_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "DT_IFP_PRESEL_TEMPLATE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "IFP_PRESEL_TEMPLATE_INDEX", presel_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL));

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
ifp_presel_destroy(int unit)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    int status;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "DT_IFP_PRESEL_TEMPLATE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "IFP_PRESEL_TEMPLATE_INDEX", presel_id));
    status = bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_DELETE,
                                BCMLT_PRIORITY_NORMAL);
    if (status == SHR_E_NOT_FOUND) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("DT_IFP_PRESEL_TEMPLATE %"PRId64" not exist\n"),
                  presel_id));
    }
    SHR_IF_ERR_EXIT(status);
    bcmlt_entry_free(ent_hdl);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "DT_IFP_PRESEL_GRP_TEMPLATE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "DT_IFP_PRESEL_GRP_TEMPLATE_INDEX", presel_grp_id));
    status = bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_DELETE,
                                BCMLT_PRIORITY_NORMAL);
    if (status == SHR_E_NOT_FOUND) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("DT_IFP_PRESEL_GRP_TEMPLATE %"PRId64" not exist\n"),
                  presel_grp_id));
    }
    SHR_IF_ERR_EXIT(status);

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}
static int
ifp_pdd_create(int unit)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    /* Create PDD profile for FP group */
    /* find a unused entry */
    SHR_IF_ERR_EXIT
        (find_first_unused_lt_entry(unit, "DT_IFP_PDD_TEMPLATE",
                                    "DT_IFP_PDD_TEMPLATE_INDEX", &pdd_profile_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "DT_IFP_PDD_TEMPLATE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "DT_IFP_PDD_TEMPLATE_INDEX", pdd_profile_id));
    /* For Copy-to-cpu action */
    if (validate_lt_support(unit, "DT_IFP_PDD_TEMPLATE",
                            "ING_CMD_BUS_IFP_COPY_TO_CPU_ACTION")) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(ent_hdl,
                                   "ING_CMD_BUS_IFP_COPY_TO_CPU_ACTION",
                                   1));
    } else if (validate_lt_support(unit, "DT_IFP_PDD_TEMPLATE",
                                   "IFP_SCRATCH_BUS_IFP_COPY_TO_CPU_ACTION")) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(ent_hdl,
                                   "IFP_SCRATCH_BUS_IFP_COPY_TO_CPU_ACTION",
                                   1));
    } else {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("Required LT field"
                           " does not exist on this device\n")));
        SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);
    }
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "ING_OBJ_BUS_IFP_DESTINATION",
                               1)); /* For Redirect action */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "ING_CMD_BUS_IFP_DESTINATION_TYPE",
                               1)); /* For Redirect action */
    if (validate_lt_support(unit, "DT_IFP_PDD_TEMPLATE",
                            "ING_CMD_BUS_IFP_DROP_ACTION")) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(ent_hdl,
                                   "ING_CMD_BUS_IFP_DROP_ACTION",
                                   1)); /* For Drop action */
    } else if (validate_lt_support(unit, "DT_IFP_PDD_TEMPLATE",
                                   "IFP_SCRATCH_BUS_IFP_DROP_ACTION")) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(ent_hdl,
                                   "IFP_SCRATCH_BUS_IFP_DROP_ACTION",
                                   1)); /* For Drop action */
    } else {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("Required LT field"
                           " does not exist on this device\n")));
        SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);
    }
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL));

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
ifp_pdd_destroy(int unit)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    int status;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "DT_IFP_PDD_TEMPLATE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "DT_IFP_PDD_TEMPLATE_INDEX", pdd_profile_id));
    status = bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_DELETE,
                                BCMLT_PRIORITY_NORMAL);
    if (status == SHR_E_NOT_FOUND) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("DT_IFP_PDD_TEMPLATE %"PRId64" not exist\n"),
                  pdd_profile_id));
    }
    SHR_IF_ERR_EXIT(status);

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
ifp_sbr_create(int unit)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    /* Create SBR profile for FP group */
    /* find a unused entry */
    SHR_IF_ERR_EXIT
        (find_first_unused_lt_entry(unit, "DT_IFP_SBR_TEMPLATE",
                                    "DT_IFP_SBR_TEMPLATE_INDEX", &sbr_profile_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "DT_IFP_SBR_TEMPLATE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "DT_IFP_SBR_TEMPLATE_INDEX", sbr_profile_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "ING_OBJ_BUS_IFP_DESTINATION", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "ING_CMD_BUS_IFP_DESTINATION_TYPE", 1));
    if (validate_lt_support(unit, "DT_IFP_SBR_TEMPLATE",
                            "ING_CMD_BUS_IFP_COPY_TO_CPU_ACTION")) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(ent_hdl,
                                   "ING_CMD_BUS_IFP_COPY_TO_CPU_ACTION", 1));
    }
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL));

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
ifp_sbr_destroy(int unit)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    int status;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "DT_IFP_SBR_TEMPLATE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "DT_IFP_SBR_TEMPLATE_INDEX", sbr_profile_id));
    status = bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_DELETE,
                                BCMLT_PRIORITY_NORMAL);
    if (status == SHR_E_NOT_FOUND) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("DT_IFP_SBR_TEMPLATE %"PRId64" not exist\n"),
                  sbr_profile_id));
    }
    SHR_IF_ERR_EXIT(status);

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
ifp_group_create(int unit, bcma_bcmpkt_test_fp_cfg_t *ifp_cfg)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    uint64_t data[2];

    SHR_FUNC_ENTER(unit);

    /* Create FP group */
    /* find a unused entry */
    SHR_IF_ERR_EXIT
        (find_first_unused_lt_entry(unit, "DT_IFP_GRP_TEMPLATE",
                                    "DT_IFP_GRP_TEMPLATE_INDEX", &ifp_cfg->grp_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "DT_IFP_GRP_TEMPLATE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "DT_IFP_GRP_TEMPLATE_INDEX", ifp_cfg->grp_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "DT_IFP_PDD_TEMPLATE_INDEX", pdd_profile_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "DT_IFP_SBR_TEMPLATE_INDEX", sbr_profile_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "MODE_AUTO", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "DT_IFP_PRESEL_GRP_TEMPLATE_INDEX", presel_grp_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "NUM_DT_IFP_PRESEL_TEMPLATE_INDEX", 1));
    data[0] = presel_id;
    SHR_IF_ERR_EXIT
         (bcmlt_entry_field_array_add(ent_hdl,
                                      "DT_IFP_PRESEL_TEMPLATE_INDEX", 0, data, 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "ING_OBJ_BUS_INGRESS_PP_PORT",
                               0xff));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "ING_CMD_BUS_DESTINATION_TYPE",
                               0xf));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL));

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
ifp_group_destroy(int unit, uint64_t group_id)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    int status;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "DT_IFP_GRP_TEMPLATE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "DT_IFP_GRP_TEMPLATE_INDEX", group_id));
    status = bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_DELETE,
                                BCMLT_PRIORITY_NORMAL);
    if (status == SHR_E_NOT_FOUND) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("DT_IFP_GRP_TEMPLATE %"PRId64" not exist\n"),
                  group_id));
    }
    SHR_IF_ERR_EXIT(status);

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
ifp_rule_create(int unit, bcma_bcmpkt_test_fp_cfg_t *ifp_cfg)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    /* Create FP rule */
    /* find a unused entry */
    SHR_IF_ERR_EXIT
        (find_first_unused_lt_entry(unit, "DT_IFP_RULE_TEMPLATE",
                                    "IFP_RULE_TEMPLATE_INDEX",
                                    &ifp_cfg->rule_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "DT_IFP_RULE_TEMPLATE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "IFP_RULE_TEMPLATE_INDEX", ifp_cfg->rule_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "ING_OBJ_BUS_INGRESS_PP_PORT", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "ING_OBJ_BUS_INGRESS_PP_PORT_MASK", 0xff));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL));

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
ifp_rule_destroy(int unit, uint64_t rule_id)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    int status;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "DT_IFP_RULE_TEMPLATE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "IFP_RULE_TEMPLATE_INDEX", rule_id));
    status = bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_DELETE,
                                BCMLT_PRIORITY_NORMAL);
    if (status == SHR_E_NOT_FOUND) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("DT_IFP_RULE_TEMPLATE %"PRId64" not exist\n"),
                  rule_id));
    }
    SHR_IF_ERR_EXIT(status);

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}
static int
ifp_policy_create(int unit, int port, bcma_bcmpkt_test_fp_cfg_t *ifp_cfg)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    /* Create FP policy. Drop. */
    /* find a unused entry */
    SHR_IF_ERR_EXIT
        (find_first_unused_lt_entry(unit, "DT_IFP_ACTION_TEMPLATE",
                                    "IFP_ACTION_TEMPLATE_INDEX",
                                    &ifp_cfg->drop_policy_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "DT_IFP_ACTION_TEMPLATE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "IFP_ACTION_TEMPLATE_INDEX",
                               ifp_cfg->drop_policy_id));
    /* For Drop action */
    if (validate_lt_support(unit, "DT_IFP_ACTION_TEMPLATE",
                            "ING_CMD_BUS_IFP_DROP_ACTION")) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(ent_hdl,
                                   "ING_CMD_BUS_IFP_DROP_ACTION",
                                   1));
    } else if (validate_lt_support(unit, "DT_IFP_ACTION_TEMPLATE",
                                   "IFP_SCRATCH_BUS_IFP_DROP_ACTION")) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(ent_hdl,
                                   "IFP_SCRATCH_BUS_IFP_DROP_ACTION",
                                   1));
    } else {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("Required LT field"
                           " does not exist on this device\n")));
        SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);
    }
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(ent_hdl);
    ifp_drop_policy_id = ifp_cfg->drop_policy_id;

    /* Create FP policy. Copy-to-cpu and redirect to port. */
    /* find a unused entry */
    SHR_IF_ERR_EXIT
        (find_first_unused_lt_entry(unit, "DT_IFP_ACTION_TEMPLATE",
                                    "IFP_ACTION_TEMPLATE_INDEX",
                                    &ifp_cfg->copy_policy_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "DT_IFP_ACTION_TEMPLATE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "IFP_ACTION_TEMPLATE_INDEX",
                               ifp_cfg->copy_policy_id));
    if (validate_lt_support(unit, "DT_IFP_ACTION_TEMPLATE",
                            "ING_CMD_BUS_IFP_COPY_TO_CPU_ACTION")) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(ent_hdl,
                                   "ING_CMD_BUS_IFP_COPY_TO_CPU_ACTION",
                                   1));
    } else if (validate_lt_support(unit, "DT_IFP_ACTION_TEMPLATE",
                                   "IFP_SCRATCH_BUS_IFP_COPY_TO_CPU_ACTION")) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(ent_hdl,
                                   "IFP_SCRATCH_BUS_IFP_COPY_TO_CPU_ACTION",
                                   1));
    } else {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("Required LT field"
                           " does not exist on this device\n")));
        SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);
    }
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "ING_OBJ_BUS_IFP_DESTINATION", port));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "ING_CMD_BUS_IFP_DESTINATION_TYPE", 0x1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL));
    ifp_copy_policy_id = ifp_cfg->copy_policy_id;

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
ifp_policy_destroy(int unit, uint64_t policy_id)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    int status;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "DT_IFP_ACTION_TEMPLATE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "IFP_ACTION_TEMPLATE_INDEX",
                               policy_id));
    status = bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_DELETE,
                                BCMLT_PRIORITY_NORMAL);
    if (status == SHR_E_NOT_FOUND) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("DT_IFP_ACTION_TEMPLATE %"PRId64" not exist\n"),
                  policy_id));
    }
    SHR_IF_ERR_EXIT(status);

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
ifp_entry_install(int unit, bcma_bcmpkt_test_fp_cfg_t *ifp_cfg)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    /* Create and install FP entry */
    /* find a unused entry */
    SHR_IF_ERR_EXIT
        (find_first_unused_lt_entry(unit, "DT_IFP_ENTRY",
                                    "DT_IFP_ENTRY_INDEX", &ifp_cfg->entry_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "DT_IFP_ENTRY", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "DT_IFP_ENTRY_INDEX", ifp_cfg->entry_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "DT_IFP_GRP_TEMPLATE_INDEX", ifp_cfg->grp_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "DT_IFP_RULE_TEMPLATE_INDEX", ifp_cfg->rule_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "DT_IFP_ACTION_TEMPLATE_INDEX",
                               ifp_cfg->copy_policy_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL));

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
ifp_entry_remove(int unit, uint64_t entry_id)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    int status;

    SHR_FUNC_ENTER(unit);

    /* FP entry */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "DT_IFP_ENTRY", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "DT_IFP_ENTRY_INDEX", entry_id));
    status = bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_DELETE,
                                BCMLT_PRIORITY_NORMAL);
    if (status == SHR_E_NOT_FOUND) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("DT_IFP_ENTRY %"PRId64" not exist\n"),
                  entry_id));
    }
    SHR_IF_ERR_EXIT(status);

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
ifp_create(int unit, int port, int redirect_port,
                     bcma_bcmpkt_test_fp_cfg_t *ifp_cfg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(trace_event_create(unit));

    SHR_IF_ERR_EXIT(ifp_presel_create(unit));

    SHR_IF_ERR_EXIT(ifp_pdd_create(unit));

    SHR_IF_ERR_EXIT(ifp_sbr_create(unit));

    SHR_IF_ERR_EXIT(ifp_group_create(unit, ifp_cfg));

    SHR_IF_ERR_EXIT(ifp_rule_create(unit, ifp_cfg));

    SHR_IF_ERR_EXIT(ifp_policy_create(unit, port, ifp_cfg));

    SHR_IF_ERR_EXIT(ifp_entry_install(unit, ifp_cfg));

exit:
    SHR_FUNC_EXIT();
}

static int
pkttest_dpath_create(int unit, int port, int redirect_port,
                     bcma_bcmpkt_test_fp_cfg_t *ifp_cfg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (fwd_path_create(unit, port, ifp_cfg));

    SHR_IF_ERR_EXIT
        (ifp_create(unit, port, redirect_port, ifp_cfg));
exit:
    SHR_FUNC_EXIT();
}

static int
fwd_path_destroy(int unit, int port, bcma_bcmpkt_test_fp_cfg_t *ifp_cfg)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    int status;

    SHR_FUNC_ENTER(unit);

    /*
     * Forward path.
     */
    /* Default L3 strength profile configuration */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "ING_DVP_TABLE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "DVP", 0));
    status = bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_DELETE,
                                BCMLT_PRIORITY_NORMAL);
    if (status == SHR_E_NOT_FOUND) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("ING_DVP_TABLE 0 not exist\n")));
    }
    SHR_IF_ERR_EXIT(status);
    bcmlt_entry_free(ent_hdl);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "ING_DVP_STRENGTH_PROFILE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "ING_DVP_STRENGTH_PROFILE_INDEX", 1));
    status = bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_DELETE,
                                BCMLT_PRIORITY_NORMAL);
    if (status == SHR_E_NOT_FOUND) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("ING_DVP_STRENGTH_PROFILE 1 not exist\n")));
    }
    SHR_IF_ERR_EXIT(status);
    bcmlt_entry_free(ent_hdl);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "ING_L3_NEXT_HOP_1_TABLE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "NHOP_INDEX_1", 0));
    status = bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_DELETE,
                                BCMLT_PRIORITY_NORMAL);
    if (status == SHR_E_NOT_FOUND) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("ING_L3_NEXT_HOP_1_TABLE 0 not exist\n")));
    }
    SHR_IF_ERR_EXIT(status);
    bcmlt_entry_free(ent_hdl);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "ING_L3_NEXT_HOP_2_TABLE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "NHOP_INDEX_2", 0));
    status = bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_DELETE,
                                BCMLT_PRIORITY_NORMAL);
    if (status == SHR_E_NOT_FOUND) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("ING_L3_NEXT_HOP_2_TABLE 0 not exist\n")));
    }
    SHR_IF_ERR_EXIT(status);
    bcmlt_entry_free(ent_hdl);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "ING_L3_NEXT_HOP_1_STRENGTH_PROFILE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "ING_L3_NEXT_HOP_1_STRENGTH_PROFILE_INDEX", 0x1));
    status = bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_DELETE,
                                BCMLT_PRIORITY_NORMAL);
    if (status == SHR_E_NOT_FOUND) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("ING_L3_NEXT_HOP_1_STRENGTH_PROFILE 1 not exist\n")));
    }
    SHR_IF_ERR_EXIT(status);
    bcmlt_entry_free(ent_hdl);

    /* PORT LT config */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "PORT", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "PORT_ID", port)); /* port = 1 */
    status = bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_DELETE,
                                BCMLT_PRIORITY_NORMAL);
    if (status == SHR_E_NOT_FOUND) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("PORT %d not exist\n"),
                  port));
    }
    SHR_IF_ERR_EXIT(status);
    bcmlt_entry_free(ent_hdl);

    /* Configure the port praser control */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "PORT_PROPERTY", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "PORT_ID", port)); /* port = 1 */
    status = bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_DELETE,
                                BCMLT_PRIORITY_NORMAL);
    if (status == SHR_E_NOT_FOUND) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("PORT_PARSER_CONTROL %d not exist\n"),
                  port));
    }
    SHR_IF_ERR_EXIT(status);
    bcmlt_entry_free(ent_hdl);

    /* Ingress System Port Config */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "ING_SYSTEM_PORT_TABLE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "SYSTEM_SOURCE", 0x1));
    status = bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_DELETE,
                                BCMLT_PRIORITY_NORMAL);
    if (status == SHR_E_NOT_FOUND) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("ING_SYSTEM_PORT_TABLE SYSTEM_SOURCE=0x1 not exist\n")));
    }
    SHR_IF_ERR_EXIT(status);
    bcmlt_entry_free(ent_hdl);

    /* Ingress VLAN tag assignment */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "ING_L2_IIF_TABLE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "L2_IIF", 0x2));
    status = bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_DELETE,
                                BCMLT_PRIORITY_NORMAL);
    if (status == SHR_E_NOT_FOUND) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("ING_L2_IIF_TABLE L2_IIF=0x2 not exist\n")));
    }
    SHR_IF_ERR_EXIT(status);
    bcmlt_entry_free(ent_hdl);

    /* Ingress VLAN table */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "ING_VFI_TABLE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "VFI", 0x2));
    status = bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_DELETE,
                                BCMLT_PRIORITY_NORMAL);
    if (status == SHR_E_NOT_FOUND) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("ING_VFI_TABLE VFI=0x2 not exist\n")));
    }
    SHR_IF_ERR_EXIT(status);
    bcmlt_entry_free(ent_hdl);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "VLAN_ING_MEMBER_PROFILE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "VLAN_ING_MEMBER_PROFILE_ID", 0));
    status = bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_DELETE,
                                BCMLT_PRIORITY_NORMAL);
    if (status == SHR_E_NOT_FOUND) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("VLAN_ING_MEMBER_PROFILE 0 not exist\n")));
    }
    SHR_IF_ERR_EXIT(status);
    bcmlt_entry_free(ent_hdl);

    /* Ingress VLAN STG 0 */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "VLAN_ING_STG_PROFILE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "VLAN_ING_STG_PROFILE_ID", 0));
    status = bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_DELETE,
                                BCMLT_PRIORITY_NORMAL);
    if (status == SHR_E_NOT_FOUND) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("VLAN_ING_STG_PROFILE 0 not exist\n")));
    }
    SHR_IF_ERR_EXIT(status);
    bcmlt_entry_free(ent_hdl);

    /* VLAN port block mask for membership and STG */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "VLAN_ING_EGR_MEMBER_PORTS_PROFILE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "VLAN_ING_EGR_MEMBER_PORTS_PROFILE_ID", 0));
    status = bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_DELETE,
                                BCMLT_PRIORITY_NORMAL);
    if (status == SHR_E_NOT_FOUND) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("VLAN_ING_EGR_MEMBER_PORTS_PROFILE 0 not exist\n")));
    }
    SHR_IF_ERR_EXIT(status);
    bcmlt_entry_free(ent_hdl);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "VLAN_ING_EGR_STG_MEMBER_PROFILE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "VLAN_ING_EGR_STG_MEMBER_PROFILE_ID", 0));
    status = bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_DELETE,
                                BCMLT_PRIORITY_NORMAL);
    if (status == SHR_E_NOT_FOUND) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("VLAN_ING_EGR_STG_MEMBER_PROFILE 0 not exist\n")));
    }
    SHR_IF_ERR_EXIT(status);
    bcmlt_entry_free(ent_hdl);

    /* L2 hash table for DST lookup */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "L2_HOST_TABLE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "MAC_ADDR", DA_BYTE_5));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "VFI", 0x2));
    status = bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_DELETE,
                                BCMLT_PRIORITY_NORMAL);

    if (status == SHR_E_NOT_FOUND) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("L2_HOST_TABLE MAC_ADDR=0x11 not exist\n")));
    }
    SHR_IF_ERR_EXIT(status);
    bcmlt_entry_free(ent_hdl);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "L2_HOST_STRENGTH_PROFILE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "L2_HOST_STRENGTH_PROFILE_INDEX", 0x7));
    status = bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_DELETE,
                                BCMLT_PRIORITY_NORMAL);
    if (status == SHR_E_NOT_FOUND) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("L2_HOST_STRENGTH_PROFILE 0x7 not exist\n")));
    }
    SHR_IF_ERR_EXIT(status);
    bcmlt_entry_free(ent_hdl);

    /* Destination resolution (-> L2_OIF) */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "ING_SYSTEM_DESTINATION_TABLE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "SYSTEM_DESTINATION_PORT", 0x1));
    status = bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_DELETE,
                                BCMLT_PRIORITY_NORMAL);
    if (status == SHR_E_NOT_FOUND) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("ING_SYSTEM_DESTINATION_TABLE 0x1 not exist\n")));
    }
    SHR_IF_ERR_EXIT(status);
    bcmlt_entry_free(ent_hdl);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "L2_EIF_SYSTEM_DESTINATION", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "L2_EIF_ID", 0x1));
    status = bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_DELETE,
                                BCMLT_PRIORITY_NORMAL);
    if (status == SHR_E_NOT_FOUND) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("L2_EIF_SYSTEM_DESTINATION 0x1 not exist\n")));
    }
    SHR_IF_ERR_EXIT(status);
    bcmlt_entry_free(ent_hdl);

    /* Configure destination port resolution. */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "PORT_SYSTEM_DESTINATION", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "PORT_SYSTEM_ID", 0x1));
    status = bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_DELETE,
                                BCMLT_PRIORITY_NORMAL);
    if (status == SHR_E_NOT_FOUND) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("PORT_SYSTEM_DESTINATION 0x1 not exist\n")));
    }
    SHR_IF_ERR_EXIT(status);
    bcmlt_entry_free(ent_hdl);

    /* Egress VLAN configuration */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "EGR_VFI", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "VFI", 0x2));
    status = bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_DELETE,
                                BCMLT_PRIORITY_NORMAL);
    if (status == SHR_E_NOT_FOUND) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("EGR_VFI 0x2 not exist\n")));
    }
    SHR_IF_ERR_EXIT(status);
    bcmlt_entry_free(ent_hdl);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "VLAN_EGR_MEMBER_PROFILE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "VLAN_EGR_MEMBER_PROFILE_ID", 0));
    status = bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_DELETE,
                                BCMLT_PRIORITY_NORMAL);
    if (status == SHR_E_NOT_FOUND) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("VLAN_EGR_MEMBER_PROFILE 0 not exist\n")));
    }
    SHR_IF_ERR_EXIT(status);
    bcmlt_entry_free(ent_hdl);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit,
                              "VLAN_EGR_STG_PROFILE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               "VLAN_EGR_STG_PROFILE_ID", 0));
    status = bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_DELETE,
                                BCMLT_PRIORITY_NORMAL);
    if (status == SHR_E_NOT_FOUND) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("VLAN_EGR_STG_PROFILE 0 not exist\n")));
    }
    SHR_IF_ERR_EXIT(status);

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(ent_hdl);
    }

    SHR_FUNC_EXIT();
}

static int
ifp_destroy(int unit, int port, bcma_bcmpkt_test_fp_cfg_t *ifp_cfg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(ifp_entry_remove(unit, ifp_cfg->entry_id));

    SHR_IF_ERR_EXIT(ifp_policy_destroy(unit, ifp_cfg->copy_policy_id));

    SHR_IF_ERR_EXIT(ifp_policy_destroy(unit, ifp_cfg->drop_policy_id));

    SHR_IF_ERR_EXIT(ifp_rule_destroy(unit, ifp_cfg->rule_id));

    SHR_IF_ERR_EXIT(ifp_group_destroy(unit, ifp_cfg->grp_id));

    SHR_IF_ERR_EXIT(ifp_sbr_destroy(unit));

    SHR_IF_ERR_EXIT(ifp_pdd_destroy(unit));

    SHR_IF_ERR_EXIT(ifp_presel_destroy(unit));

    SHR_IF_ERR_EXIT(trace_event_destroy(unit));

exit:
    SHR_FUNC_EXIT();
}

static int
pkttest_dpath_destroy(int unit, int port, bcma_bcmpkt_test_fp_cfg_t *ifp_cfg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (ifp_destroy(unit, port, ifp_cfg));

    SHR_IF_ERR_EXIT
        (fwd_path_destroy(unit, port, ifp_cfg));

exit:
    SHR_FUNC_EXIT();
}

static int
pkttest_lt_policy_update(int unit, uint64_t entry_id, uint64_t policy_id)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    /*
     * Disable packet foward on the test port by update policy of IFP entry.
     * - policy drop, no forwarding.
     * - policy copy-to-cpu and redirect-to-port, forwarding packets.
     */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DT_IFP_ENTRY", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl, "DT_IFP_ENTRY_INDEX", entry_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl, "DT_IFP_ACTION_TEMPLATE_INDEX",
                               policy_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_UPDATE,
                            BCMLT_PRIORITY_NORMAL));

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

static bcma_bcmpkt_test_drv_t bcm56880_a0_pkttest_drv = {
    .dpath_create = pkttest_dpath_create,
    .dpath_destroy = pkttest_dpath_destroy,
    .lt_policy_update = pkttest_lt_policy_update,
};

int
bcma_bcmpkt_test_bcm56880_a0_drv_attach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcma_bcmpkt_test_drv_set(unit, &bcm56880_a0_pkttest_drv));

exit:
    SHR_FUNC_EXIT();
}
