/*! \file bcm56880_a0_testutil_max_power.c
 *
 * Chip-specific TRAFFIC data.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <shr/shr_types.h>
#include <shr/shr_util.h>
#include <sal/sal_sleep.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_pt_ser.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmdrd/chip/bcm56880_a0_defs.h>
#include <bcmdrd/chip/bcm56880_a0_enum.h>

#include <bcmpc/bcmpc_pmgr.h>
#include <bcma/test/util/bcma_testutil_drv.h>
#include <bcma/test/util/bcma_testutil_common.h>
#include <bcma/test/util/bcma_testutil_packet.h>
#include <bcma/test/testcase/bcma_testcase_traffic_max_power.h>
#include <bcmlt/bcmlt.h>
#include <bcmptm/bcmptm.h>

#include <bcmpkt/bcmpkt_net.h>
#include <bcmpkt/bcmpkt_buf.h>
#include <bcma/test/util/bcma_testutil_packet.h>
#include <bcma/test/util/bcma_testutil_stat.h>
#include <bcma/test/util/bcma_testutil_power.h>
#include <bcma/test/util/bcma_testutil_port.h>
#include <bcma/test/util/bcma_testutil_traffic.h>

#include <bcma/test/chip/bcm56880_a0_testutil_traffic.h>

static int seed=0;

#define TRAFFIC_MAX_POWER_RMAC_ADDR {0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc}
#define TRAFFIC_MAX_POWER_MAC_BASE {0x12, 0x34, 0x56, 0x78, 0x90, 0x00}
#define TRAFFIC_MAX_POWER_INVERT_MAC_BASE {0xed, 0xcb, 0xa9, 0x87, 0x60, 0x00}
#define TRAFFIC_MAX_POWER_NEXT_HOP_BASE     (0x200)
#define TRAFFIC_MAX_POWER_INTF_NUM_BASE     (0x300)
#define TRAFFIC_MAX_POWER_IP_ADDR    {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0x00}
#define TRAFFIC_MAX_POWER_INVERT_IP_ADDR    {0x00, 0xff, 0xee, 0xdd, 0xcc, 0xbb, 0xaa, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x00}
#define TRAFFIC_MAX_POWER_VLAN_ID_BASE     (0x100)
#define TRAFFIC_MAX_POWER_ECMP_GRP_IDX_BASE     (0xff)
#define TRAFFIC_MAX_POWER_MAX_ECMP_GRP_NUM     (128)
#define TRAFFIC_MAX_POWER_ECMP_GRP_CNT     (4)
#define TRAFFIC_MAX_POWER_CPU_PORT                   (0)
#define TRAFFIC_MAX_POWER_VLAN_PROFILE_ID_BASE       (1)
#define TRAFFIC_MAX_POWER_DEFAULT_PKTLEN_MODE        (POWER_PKTLEN_WORSTCASE)
#define TRAFFIC_MAX_POWER_DEFAULT_PACKET_ETHERTYPE   (0xffff)
#define TRAFFIC_MAX_POWER_DEFAULT_PACKET_PATTERN     (0xa5a4a3a2)
#define TRAFFIC_MAX_POWER_DEFAULT_PACKET_PATTERN_INC (0)

#define L3_STRENGTH_PROFILE_ID 2
#define L3_DEFIP_STRENGTH_PROFILE_ID 2
#define ECMP_MEMBER_STRENGTH_PROFILE_ID 1
#define BSL_LOG_MODULE BSL_LS_APPL_TEST

static int
bcm56880_a0_l3_strength_profile_add(int unit)
{
    bcmlt_entry_handle_t entry = BCMLT_INVALID_HDL;
    bcmlt_entry_handle_t entry1 = BCMLT_INVALID_HDL;
    int rv, found;
    uint64_t entry_maximum;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "TABLE_INFO", &entry1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry1, "TABLE_ID",
                                       "L3_IPV6_UNICAST_STRENGTH_PROFILE"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry1, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry1, "ENTRY_MAXIMUM", &entry_maximum));
    bcmlt_entry_free(entry1);


    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "L3_IPV6_UNICAST_STRENGTH_PROFILE", &entry));
    if (entry_maximum > 1) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry, "L3_IPV6_UNICAST_STRENGTH_PROFILE_INDEX",
                                       L3_STRENGTH_PROFILE_ID));
    }
    rv = bcmlt_entry_commit(entry, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL);
    found = SHR_SUCCESS(rv);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "L3_DST_DESTINATION_STRENGTH",7));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "L3_DST_DESTINATION_TYPE_STRENGTH", 7));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "L3_DST_MISC_CTRL_0_STRENGTH", 7));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "L3_DST_MISC_CTRL_1_STRENGTH", 7));
    if (found) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(entry, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
    } else {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    }
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "TABLE_INFO", &entry1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry1, "TABLE_ID",
                                       "L3_IPV6_UNICAST_DEFIP_128_STRENGTH_PROFILE"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry1, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry1, "ENTRY_MAXIMUM", &entry_maximum));
    bcmlt_entry_free(entry1);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "L3_IPV6_UNICAST_DEFIP_128_STRENGTH_PROFILE", &entry));
    if (entry_maximum > 1) {
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry, "L3_IPV6_UNICAST_DEFIP_128_STRENGTH_PROFILE_INDEX",
                                 L3_DEFIP_STRENGTH_PROFILE_ID));
    }
    rv = bcmlt_entry_commit(entry, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL);
    found = SHR_SUCCESS(rv);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "L3_DST_DESTINATION_STRENGTH", 6));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "L3_DST_DESTINATION_TYPE_STRENGTH", 6));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "L3_DST_MISC_CTRL_0_STRENGTH", 6));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "L3_DST_MISC_CTRL_1_STRENGTH", 6));
    if (found) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(entry, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
    } else {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    }
    bcmlt_entry_free(entry);



exit:
    if (entry != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry);
    }
    SHR_FUNC_EXIT();
}

static int
bcm56880_a0_update_l3_ing_system_port_table(int unit, int port_id)
{
    bcmlt_entry_handle_t entry = BCMLT_INVALID_HDL;
    int  rv, found;
    SHR_FUNC_ENTER(unit);


    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "ING_SYSTEM_PORT_TABLE", &entry));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "SYSTEM_SOURCE", port_id));

    rv = bcmlt_entry_commit(entry, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL);

    found = SHR_SUCCESS(rv);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "OUTER_L2HDR_OTPID_ENABLE", 0xf));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "L2_IIF", port_id));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "L2_IIF_STRENGTH", 1));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "INGRESS_PP_PORT", port_id));

    if (found) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(entry, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
    } else {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    }
    bcmlt_entry_free(entry);

exit:
    if (entry != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry);
    }
    SHR_FUNC_EXIT();
}

static int
bcm56880_a0_update_l3_ing_system_dest_port_table(int unit, int port_id, int vlan_id)
{
    bcmlt_entry_handle_t entry = BCMLT_INVALID_HDL;
    int rv, found;
    SHR_FUNC_ENTER(unit);


    SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(unit, "ING_SYSTEM_DESTINATION_TABLE", &entry));
    SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry, "SYSTEM_DESTINATION_PORT", (port_id) & 0xfff));
    rv = bcmlt_entry_commit(entry, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL);
    found = SHR_SUCCESS(rv);

    SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry, "L2_OIF", vlan_id & 0x7ff));
    SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry, "L2_OIF_STRENGTH", 1));

    if (found) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(entry, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
    } else {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    }
    bcmlt_entry_free(entry);

exit:
    if (entry != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry);
    }
    SHR_FUNC_EXIT();
}

static int
bcm56880_a0_update_l3_if_table(int unit, int port_id, int vlan_id, int intf_num)
{
    bcmlt_entry_handle_t entry = BCMLT_INVALID_HDL;
    int rv, found;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "ING_L2_IIF_TABLE", &entry));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "L2_IIF", port_id));

    rv = bcmlt_entry_commit(entry, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL);

    found = SHR_SUCCESS(rv);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "VFI", vlan_id));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "L3_IIF", intf_num));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "VLAN_TO_VFI_MAPPING_ENABLE", 1));

    if (found) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(entry, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
    } else {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    }

    bcmlt_entry_free(entry);


    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "ING_L3_IIF_TABLE", &entry));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "L3_IIF", intf_num));

    rv = bcmlt_entry_commit(entry, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL);

    found = SHR_SUCCESS(rv);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "VRF", 15));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "IPV4UC_ENABLE", 1));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "IPV6UC_ENABLE", 1));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "IPV4MC_ENABLE", 1));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "IPV6MC_ENABLE", 1));

    if (found) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(entry, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
    } else {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    }

    bcmlt_entry_free(entry);

exit:
    if (entry != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry);
    }
    SHR_FUNC_EXIT();
}

static int
bcm56880_update_l3_ing_vfi_table(int unit, int vlan_id, int intf_num)
{
    bcmlt_entry_handle_t entry = BCMLT_INVALID_HDL;
    int  rv, found;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "ING_DVP_TABLE", &entry));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DVP", 0));

    rv = bcmlt_entry_commit(entry, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL);

    found = SHR_SUCCESS(rv);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "STRENGTH_PROFILE_INDEX", 1));

    if (found) {
        SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
    } else {
        SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    }

    bcmlt_entry_free(entry);


    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "ING_DVP_STRENGTH_PROFILE", &entry));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ING_DVP_STRENGTH_PROFILE_INDEX", 1));

    rv = bcmlt_entry_commit(entry, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL);

    found = SHR_SUCCESS(rv);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "L2MC_L3MC_L2_OIF_SYS_DST_VALID_STRENGTH", 0));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DESTINATION_STRENGTH", 0));

    if (found) {
        SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
    } else {
        SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    }

    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "ING_VFI_TABLE", &entry));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "VFI", vlan_id));

    rv = bcmlt_entry_commit(entry, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL);

    found = SHR_SUCCESS(rv);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DESTINATION", intf_num));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "DESTINATION_TYPE", "L2MC_GROUP"));


    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "L3_IIF", intf_num));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "SPANNING_TREE_GROUP", vlan_id & 0xff));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "MEMBERSHIP_PROFILE_PTR", vlan_id & 0xff));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "STRENGTH_PROFILE_INDEX", 0xb));

    if (found) {
        SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
    } else {
        SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    }

    bcmlt_entry_free(entry);


    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "VFI_STRENGTH_PROFILE", &entry));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "VFI_STRENGTH_PROFILE_INDEX", 0xb));

    rv = bcmlt_entry_commit(entry, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL);

    found = SHR_SUCCESS(rv);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "L3_IIF_STRENGTH", 0x4));

    if (found) {
        SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
    } else {
        SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    }

    bcmlt_entry_free(entry);

exit:
    if (entry != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry);
    }
    SHR_FUNC_EXIT();
}

static int
bcm56880_update_l3_ecmp_grp(int unit, int ecmp_grp_idx, int ecmp_base_ptr, int ecmp_grp_cnt)
{
    bcmlt_entry_handle_t entry = BCMLT_INVALID_HDL;
    int rv, found;
    int num_of_elem = 1;
    uint64_t data[num_of_elem];

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "ECMP_GROUP_LEVEL0_GROUP_TABLEm", &entry));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "BCMLT_PT_INDEX", ecmp_grp_idx & 0xfff));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "COUNT", 0));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "BASE_PTR", ecmp_grp_idx));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "LB_MODE", 1));

    SHR_IF_ERR_EXIT
        (bcmlt_pt_entry_commit(entry, BCMLT_PT_OPCODE_SET, BCMLT_PRIORITY_NORMAL));

    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "ECMP_LEVEL0", &entry));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ECMP_ID", ecmp_grp_idx));

    rv = bcmlt_entry_commit(entry, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL);

    found = SHR_SUCCESS(rv);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "NUM_PATHS", 0x1));

    data[0] = ecmp_grp_idx;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry, "NHOP_2_OR_ECMP_GROUP_INDEX_1", 0, data, num_of_elem));
    data[0] = 0x4;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry, "ECMP_AND_PROT_SWT_SFC_PROFILE_INDEX", 0, data, num_of_elem));

    if (found) {
        SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
    } else {
        SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    }
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "ECMP_GROUP_LEVEL1_GROUP_TABLEm", &entry));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "BCMLT_PT_INDEX", ecmp_grp_idx & 0xfff));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "COUNT", ecmp_grp_cnt));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "BASE_PTR", ecmp_base_ptr));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "LB_MODE", 1));

    SHR_IF_ERR_EXIT
        (bcmlt_pt_entry_commit(entry, BCMLT_PT_OPCODE_SET, BCMLT_PRIORITY_NORMAL));

    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "ECMP_GROUP_LEVEL1_RANDOM_SEEDr", &entry));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "BCMLT_PT_INDEX", 0));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "VALUE", 0x1234567));

    SHR_IF_ERR_EXIT
        (bcmlt_pt_entry_commit(entry, BCMLT_PT_OPCODE_SET, BCMLT_PRIORITY_NORMAL));

    bcmlt_entry_free(entry);

exit:
    if (entry != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry);
    }
    SHR_FUNC_EXIT();
}

static int
bcm56880_update_membership_check(int unit, int index)
{
    bcmlt_entry_handle_t entry = BCMLT_INVALID_HDL;
    int  i;
    int bmp_size = 160;
    int num_of_elem = (bmp_size + 63) >> 6;
    uint64_t data[num_of_elem];

    SHR_FUNC_ENTER(unit);


    sal_memset(data, 0, sizeof(uint64_t) * num_of_elem);
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "MEMBERSHIP_CHECK_ING0_BITMAPm", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "BCMLT_PT_INDEX", index & 0xff));

    for (i = 0; i < bmp_size; i++) {
        data[i / 64] |= (uint64_t) 1 << (i % 64);
    }
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry, "BMP", 0, data, num_of_elem));
    SHR_IF_ERR_EXIT
        (bcmlt_pt_entry_commit(entry, BCMLT_PT_OPCODE_SET, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    sal_memset(data, 0, sizeof(uint64_t) * num_of_elem);
    for (i = 0; i < (bmp_size >> 1); i++) {
        data[i / 32] |= (uint64_t) 3 << ((i % 32) << 1);
    }
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "MEMBERSHIP_CHECK_ING0_STATE_PROFILE_LOWERm", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "BCMLT_PT_INDEX", index & 0xff));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry, "BMP", 0, data, num_of_elem));
    SHR_IF_ERR_EXIT
        (bcmlt_pt_entry_commit(entry, BCMLT_PT_OPCODE_SET, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    sal_memset(data, 0, sizeof(uint64_t) * num_of_elem);
    for (i = 0; i < (bmp_size >> 1); i++) {
        data[i / 32] |= (uint64_t) 3 << ((i % 32) << 1);
    }
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "MEMBERSHIP_CHECK_ING0_STATE_PROFILE_UPPERm", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "BCMLT_PT_INDEX", index & 0xff));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry, "BMP", 0, data, num_of_elem));
    SHR_IF_ERR_EXIT
        (bcmlt_pt_entry_commit(entry, BCMLT_PT_OPCODE_SET, BCMLT_PRIORITY_NORMAL));


    bcmlt_entry_free(entry);

exit:
    if (entry != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry);
    }
    SHR_FUNC_EXIT();
}

static int
bcm56880_a0_l3_ipv6_unicast_ecmp_add(int unit, int port_id, const shr_mac_t mac_addr,
                                     const shr_mac_t rmac_addr, int vlan_id, int next_hop_index,
                                     shr_ip6_t ip_addr, int intf_num, int ecmp_grp_idx,
                                     int ecmp_base_ptr, int ecmp_grp_cnt, bool defip_enabled)
{
    bcmlt_entry_handle_t entry = BCMLT_INVALID_HDL;
    bcmlt_entry_handle_t entry1 = BCMLT_INVALID_HDL;
    uint64_t mac_da = 0, tmp, entry_maximum;
    uint64_t dst_ip_addr[2]= {0x0, 0x0}, ip_addr_mask[2] = {0xffffffffffffffff, 0xffffffffffffffff};
    const char *tcam_banks_str[4] = {"IFTA80_T2T_00_BANK0", "IFTA80_T2T_00_BANK1", "IFTA80_T2T_00_BANK2",
                                      "IFTA80_T2T_00_BANK3"};
    const char *tcam_banks_str1[4] = {"IFTA80_T2T_00_BANK4", "IFTA80_T2T_00_BANK5", "IFTA80_T2T_00_BANK6",
                                      "IFTA80_T2T_00_BANK7"};
    const char *hash_banks_str[2] = {"IFTA80_E2T_00_BANK0", "IFTA80_E2T_00_BANK1"};
    const char *my_station_tcam_banks_str[2] = {"IFTA40_T4T_00_BANK0","IFTA40_T4T_00_BANK1"};
    int idx, rv, found, i;
    SHR_FUNC_ENTER(unit);

    for (idx = 0; idx < 6; idx++) {
        if (idx >= 4) {
            tmp = 0;
        } else {
            tmp = mac_addr[idx];
        }
        mac_da <<= 8;
        mac_da |= tmp;
    }

    for (idx = 0; idx < SHR_IPV6_ADDR_LEN; idx++) {
        i = 1 - idx/8;
        dst_ip_addr[i] <<= 8;
        dst_ip_addr[i] |= ip_addr[idx];
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "IDEV_CONFIG_TABLEm", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "BCMLT_PT_INDEX", port_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DEVICE_PORT", port_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ING_PP_PORT", port_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "SYSTEM_PORT", port_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "PARSER0_CTRL_ID", 3));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CONFIG_DEVICE_PORT_TYPE", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_pt_entry_commit(entry, BCMLT_PT_OPCODE_SET, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcm56880_a0_update_l3_ing_system_port_table(unit, port_id));

    SHR_IF_ERR_EXIT
        (bcm56880_a0_update_l3_ing_system_dest_port_table(unit, port_id, vlan_id));

    SHR_IF_ERR_EXIT
        (bcm56880_a0_update_l3_if_table(unit, port_id, vlan_id, intf_num));

    SHR_IF_ERR_EXIT
        (bcm56880_update_l3_ing_vfi_table(unit, vlan_id, intf_num));

    SHR_IF_ERR_EXIT
        (bcm56880_update_membership_check(unit, vlan_id));

    SHR_IF_ERR_EXIT
        (bcm56880_update_l3_ecmp_grp(unit, ecmp_grp_idx, ecmp_base_ptr, ecmp_grp_cnt));

    /* L3_UC_CONTROL */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "L3_UC_CONTROL", &entry));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "URPF_DROP", 0));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));

    bcmlt_entry_free(entry);
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "TABLE_INFO", &entry1));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry1, "TABLE_ID",
                                   "MY_STATION_TABLE"));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry1, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry1, "ENTRY_LIMIT", &entry_maximum));

    bcmlt_entry_free(entry1);

    if (entry_maximum <= 0 ) {

        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(unit, "DEVICE_EM_GROUP", &entry));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_symbol_add(entry, "DEVICE_EM_GROUP_ID",
                                       "IFTA40_T4T_00_MODE0_GROUP0"));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry, "NUM_BANKS", 2));

        SHR_IF_ERR_EXIT
                (bcmlt_entry_field_array_symbol_add(entry, "DEVICE_EM_BANK_ID", 0,
                                              my_station_tcam_banks_str, 2));

        SHR_IF_ERR_EXIT
                (bcmlt_entry_commit(entry, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(unit, "TABLE_CONTROL", &entry));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_symbol_add(entry, "TABLE_ID",
                                       "MY_STATION_TABLE"));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry, "MAX_ENTRIES", 768));

        SHR_IF_ERR_EXIT
                (bcmlt_entry_commit(entry, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
    }

    /* Insert MY_STATION_TABLE. */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "MY_STATION_TABLE", &entry));


    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "MACDA", mac_da));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "MACDA_MASK", 0xffffffff0000));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "OVID", 0));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "OVID_MASK", 0));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "IVID", 0));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "IVID_MASK", 0));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "L2_IIF_VFI", 0));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "L2_IIF_VFI_MASK", 0));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "L2_IIF", 0));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "L2_IIF_MASK", 0x0));

    rv = bcmlt_entry_commit(entry, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL);

    found = SHR_SUCCESS(rv);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "VALID", 1));

    if (found) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(entry, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
    } else {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    }

    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "TABLE_INFO", &entry1));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry1, "TABLE_ID",
                                   "L3_IPV6_UNICAST_TABLE"));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry1, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry1, "ENTRY_LIMIT", &entry_maximum));

    bcmlt_entry_free(entry1);

    if (entry_maximum <= 0 ) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(unit, "DEVICE_EM_TILE", &entry));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_symbol_add(entry, "DEVICE_EM_TILE_ID",
                                       "IFTA80_E2T_00"));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_symbol_add(entry, "MODE",
                                       "IFTA80_E2T_00_MODE2"));

        SHR_IF_ERR_EXIT
                (bcmlt_entry_commit(entry, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(unit, "DEVICE_EM_GROUP", &entry));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_symbol_add(entry, "DEVICE_EM_GROUP_ID",
                                       "IFTA80_E2T_00_MODE2_GROUP0"));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry, "NUM_BANKS", 2));

        SHR_IF_ERR_EXIT
                (bcmlt_entry_field_array_symbol_add(entry, "DEVICE_EM_BANK_ID", 0,
                                              hash_banks_str, 2));

        SHR_IF_ERR_EXIT
                (bcmlt_entry_commit(entry, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(unit, "TABLE_CONTROL", &entry));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_symbol_add(entry, "TABLE_ID",
                                       "L3_IPV6_UNICAST_TABLE"));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry, "MAX_ENTRIES", 8192));

        SHR_IF_ERR_EXIT
                (bcmlt_entry_commit(entry, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
    }

    if ( !defip_enabled || (port_id%2)) {
        /* Insert L3_IPV6_UNICAST_TABLE. */
        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(unit, "TABLE_INFO", &entry1));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_symbol_add(entry1, "TABLE_ID",
                                       "L3_IPV6_UNICAST_STRENGTH_PROFILE"));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(entry1, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(entry1, "ENTRY_MAXIMUM", &entry_maximum));
        bcmlt_entry_free(entry1);

        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(unit, "L3_IPV6_UNICAST_TABLE", &entry));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_array_add(entry, "IPV6_ADDR", 0, dst_ip_addr, 2));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry, "VRF", 15));

        rv = bcmlt_entry_commit(entry, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL);

        found = SHR_SUCCESS(rv);

        if (entry_maximum > 1) {
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry, "STRENGTH_PROFILE_INDEX",
                                   L3_STRENGTH_PROFILE_ID));
        }

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry, "DESTINATION", ecmp_grp_idx));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_symbol_add(entry, "DESTINATION_TYPE", "ECMP"));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry, "MISC_CTRL_0", 1));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry, "MISC_CTRL_1", 8));

        if (found) {
            SHR_IF_ERR_EXIT
                (bcmlt_entry_commit(entry, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
        } else {
            SHR_IF_ERR_EXIT
                (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
        }
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "TABLE_INFO", &entry1));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry1, "TABLE_ID",
                                   "L3_IPV6_UNICAST_DEFIP_128_TABLE"));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry1, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry1, "ENTRY_LIMIT", &entry_maximum));

    bcmlt_entry_free(entry1);

    if (entry_maximum <=0 ) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(unit, "DEVICE_EM_TILE", &entry1));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_symbol_add(entry1, "DEVICE_EM_TILE_ID",
                                       "IFTA80_T2T_00"));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_symbol_add(entry1, "MODE",
                                       "IFTA80_T2T_00_MODE0"));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(entry1, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));

        bcmlt_entry_free(entry1);

        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(unit, "DEVICE_EM_GROUP", &entry1));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_symbol_add(entry1, "DEVICE_EM_GROUP_ID",
                                       "IFTA80_T2T_00_MODE0_GROUP0"));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry1, "NUM_BANKS", 4));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_array_symbol_add(entry1, "DEVICE_EM_BANK_ID", 0,
                                          tcam_banks_str, 4));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(entry1, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));

        bcmlt_entry_free(entry1);

        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(unit, "DEVICE_EM_GROUP", &entry1));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_symbol_add(entry1, "DEVICE_EM_GROUP_ID",
                                       "IFTA80_T2T_00_MODE0_GROUP1"));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry1, "NUM_BANKS", 4));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_array_symbol_add(entry1, "DEVICE_EM_BANK_ID", 0,
                                          tcam_banks_str1, 4));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(entry1, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));

        bcmlt_entry_free(entry1);

        /* Insert L3_IPV6_UNICAST_DEFIP_TABLE. */
        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(unit, "TABLE_CONTROL", &entry1));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_symbol_add(entry1, "TABLE_ID",
                                       "L3_IPV6_UNICAST_DEFIP_128_TABLE"));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(entry1, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry1, "MAX_ENTRIES", 0x3ff));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(entry1, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));

        bcmlt_entry_free(entry1);
    }



    if (defip_enabled && (port_id % 2 == 0)) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(unit, "L3_IPV6_UNICAST_DEFIP_128_TABLE", &entry1));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_array_add(entry1, "IPV6_ADDR", 0, dst_ip_addr, 2));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_array_add(entry1, "IPV6_ADDR_MASK", 0, ip_addr_mask,2));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry1, "VRF", 15));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry1, "VRF_MASK", 15));

        rv = bcmlt_entry_commit(entry1, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL);

        found = SHR_SUCCESS(rv);

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry1, "STRENGTH_PROFILE_INDEX",
                                L3_DEFIP_STRENGTH_PROFILE_ID));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry1, "DESTINATION", ecmp_grp_idx));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_symbol_add(entry1, "DESTINATION_TYPE", "ECMP"));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry1, "MISC_CTRL_0", 1));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry1, "MISC_CTRL_1", 8));

        if (found) {
            SHR_IF_ERR_EXIT
                (bcmlt_entry_commit(entry1, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
        } else {
            SHR_IF_ERR_EXIT
                (bcmlt_entry_commit(entry1, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
        }
    }

    /* Add L3 profile */
    SHR_IF_ERR_EXIT
        (bcm56880_a0_l3_strength_profile_add(unit));

exit:
    if (entry != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry);
    }
    if (entry1 != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry1);
    }

    SHR_FUNC_EXIT();
}


static int
bcm56880_a0_update_ipost_membership_check(int unit, int index)
{
    bcmlt_entry_handle_t entry = BCMLT_INVALID_HDL;
    int  i;
    int bmp_size = 160;
    int num_of_elem = (bmp_size + 63) >> 6;
    uint64_t data[(160 + 63) >> 6] = {0};
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "IPOST_MEMBERSHIP_CHECK_BLOCK_MASK_MEMBERSHIP_CHECK_BITMAPm",
                                                        &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "BCMLT_PT_INDEX", index & 0xff));

    for (i = 0; i < bmp_size; i++) {
        data[i / 64] |= (uint64_t) 1 << (i % 64);
    }
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry, "BMP", 0, data, num_of_elem));
    SHR_IF_ERR_EXIT
        (bcmlt_pt_entry_commit(entry, BCMLT_PT_OPCODE_SET, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "IPOST_MEMBERSHIP_CHECK_BLOCK_MASK_MEMBERSHIP_CHECK_BITMAPm",
                                                        &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "BCMLT_PT_INDEX", 0));

    for (i = 0; i < bmp_size; i++) {
        data[i / 64] |= (uint64_t) 1 << (i % 64);
    }
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry, "BMP", 0, data, num_of_elem));
    SHR_IF_ERR_EXIT
        (bcmlt_pt_entry_commit(entry, BCMLT_PT_OPCODE_SET, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    sal_memset(data, 0, sizeof(uint64_t) * num_of_elem);
    for (i = 0; i < (bmp_size >> 1); i++) {
        data[i / 32] |= (uint64_t) 3 << ((i % 32) << 1);
    }
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "IPOST_MEMBERSHIP_CHECK_BLOCK_MASK_MEMBERSHIP_STATE_LOWERm",
                                                        &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "BCMLT_PT_INDEX", index & 0xff));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry, "MEMBER_STATE", 0, data, num_of_elem));
    SHR_IF_ERR_EXIT
        (bcmlt_pt_entry_commit(entry, BCMLT_PT_OPCODE_SET, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    sal_memset(data, 0, sizeof(uint64_t) * num_of_elem);
    for (i = 0; i < (bmp_size >> 1); i++) {
        data[i / 32] |= (uint64_t) 3 << ((i % 32) << 1);
    }
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "IPOST_MEMBERSHIP_CHECK_BLOCK_MASK_MEMBERSHIP_STATE_UPPERm", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "BCMLT_PT_INDEX", index & 0xff));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry, "MEMBER_STATE", 0, data, num_of_elem));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "STATE_COMPARE_VALUE", 3));
    SHR_IF_ERR_EXIT
        (bcmlt_pt_entry_commit(entry, BCMLT_PT_OPCODE_SET, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

exit:
    if (entry != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry);
    }
    SHR_FUNC_EXIT();
}

static int
bcm56880_a0_update_egr_membership_check(int unit, int index)
{
    bcmlt_entry_handle_t entry = BCMLT_INVALID_HDL;
    int i;
    int bmp_size = 160;
    int num_of_elem = (bmp_size + 63) >> 6;
    uint64_t data[num_of_elem];
    SHR_FUNC_ENTER(unit);

    sal_memset(data, 0, sizeof(uint64_t) * num_of_elem);
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "EGR_MEMBERSHIP_MEMBERSHIP_PROFILEm", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "BCMLT_PT_INDEX", index & 0xff));

    for (i = 0; i < bmp_size; i++) {
        data[i / 64] |= (uint64_t) 1 << (i % 64);
    }
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry, "BMP", 0, data, num_of_elem));
    SHR_IF_ERR_EXIT
        (bcmlt_pt_entry_commit(entry, BCMLT_PT_OPCODE_SET, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    sal_memset(data, 0, sizeof(uint64_t) * num_of_elem);
    for (i = 0; i < (bmp_size >> 1); i++) {
        data[i / 32] |= (uint64_t) 3 << ((i % 32) << 1);
    }
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "EGR_MEMBERSHIP_STATE_PROFILE_LOWERm", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "BCMLT_PT_INDEX", index & 0xff));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry, "BMP", 0, data, num_of_elem));
    SHR_IF_ERR_EXIT
        (bcmlt_pt_entry_commit(entry, BCMLT_PT_OPCODE_SET, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    sal_memset(data, 0, sizeof(uint64_t) * num_of_elem);
    for (i = 0; i < (bmp_size >> 1); i++) {
        data[i / 32] |= (uint64_t) 3 << ((i % 32) << 1);
    }
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "EGR_MEMBERSHIP_STATE_PROFILE_UPPERm", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "BCMLT_PT_INDEX", index & 0xff));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry, "BMP", 0, data, num_of_elem));
    SHR_IF_ERR_EXIT
        (bcmlt_pt_entry_commit(entry, BCMLT_PT_OPCODE_SET, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

exit:
    if (entry != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry);
    }
    SHR_FUNC_EXIT();
}


static int
bcm56880_a0_update_l3_ecmp_nhi(int unit, int ecmp_member_index, int port_id,
                            int vlan_id, int next_hop_index, int intf_num,
                            int mtu_index, shr_mac_t mac_da, shr_mac_t mac_sa){
    int rv, found, idx;
    bcmlt_entry_handle_t entry;
    int l3_if_strength_profile;
    uint64_t l3_oif_process_ctrl, ipmc_intf_check_ctrl, fwd_domain_type;
    uint64_t check_ttl, ttl_threshold_value, ttl_decr_value;
    uint64_t mac_addr = 0, tmp;
#define EDIT_CTRL_ID_L3_NHOP_ROUTE   3
#define EDIT_CTRL_ID_L3_OIF_UC_ROUTE 4
    SHR_FUNC_ENTER(unit);


    l3_if_strength_profile=1;

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "EGR_L2_OIF", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "L2_OIF",  port_id & 0x3ff));
    rv = bcmlt_entry_commit(entry, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL);
    found = SHR_SUCCESS(rv);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "TPID_0", 0x8100));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "TPID_1", 0x9100));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "STRENGTH_PRFL_IDX", l3_if_strength_profile));

    if (found) {
        SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
    } else {
        SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    }
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "EGR_L3_OIF_1", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "L3_OIF_1", intf_num));
    rv = bcmlt_entry_commit(entry, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL);
    found = SHR_SUCCESS(rv);

    for (idx = 0; idx < 6; idx++) {
        tmp = mac_sa[idx];
        mac_addr <<= 8;
        mac_addr |= tmp;
    }
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "SRC_MAC_ADDRESS", mac_addr));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "EDIT_CTRL_ID", EDIT_CTRL_ID_L3_OIF_UC_ROUTE));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "VFI", vlan_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "OBJ_TABLE_SEL_1", 5));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "STRENGTH_PRFL_IDX", l3_if_strength_profile));

    /*
     *  b0-1    : ipmc_intf_check_ctrl; b0: check_en, b1 : allow_l2_copy
     *  b2-3    : fwd_domain_type; 0=VFI - 1=VRF
     *  b4-4    : check_ttl;
     *  b5-7    : ttl_decr;
     *  b8-15   : ttl_threshold;
     */
    ipmc_intf_check_ctrl = 0;
    fwd_domain_type = 0;
    check_ttl = 1;
    ttl_threshold_value = 2;
    ttl_decr_value = 0;
    l3_oif_process_ctrl = (ttl_threshold_value << 8) | (ttl_decr_value << 5) | (check_ttl << 4)
                        | (fwd_domain_type << 2) | ipmc_intf_check_ctrl;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "L3_OIF_PROCESS_CTRL", l3_oif_process_ctrl));
    if (found) {
        SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
    } else {
        SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    }
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "EGR_L3_OIF_1_STR_PROFILE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "EGR_L3_OIF_1_STR_PROFILE_INDEX", l3_if_strength_profile));
    rv = bcmlt_entry_commit(entry, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL);
    found = SHR_SUCCESS(rv);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "OBJ_TABLE_SEL_1", 2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "VFI_STR", 2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "MAC_SA_STR", 2));
    if (found) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(entry, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
    } else {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    }
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "EGR_L2_OIF_STR_PROFILE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "EGR_L2_OIF_STR_PROFILE_INDEX", l3_if_strength_profile));
    rv = bcmlt_entry_commit(entry, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL);
    found = SHR_SUCCESS(rv);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "TOS_PTR_STR", 5));
    if (found) {
        SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
    } else {
        SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    }
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "ING_L3_NEXT_HOP_1_TABLE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "NHOP_INDEX_1", next_hop_index));
    rv = bcmlt_entry_commit(entry, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL);
    found = SHR_SUCCESS(rv);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "MTU_PROFILE_PTR", mtu_index));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "L3_OIF_1", intf_num));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DESTINATION", (port_id) & 0x3ff));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "L2MC_L3MC_L2_OIF_SYS_DST_VALID", 4));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "STRENGTH_PROFILE_INDEX", 2));
    if (found) {
        SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
    } else {
        SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    }
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "ING_L3_NEXT_HOP_1_STRENGTH_PROFILE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ING_L3_NEXT_HOP_1_STRENGTH_PROFILE_INDEX", 2));
    rv = bcmlt_entry_commit(entry, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL);
    found = SHR_SUCCESS(rv);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DESTINATION_STRENGTH", 4));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "L2MC_L3MC_L2_OIF_SYS_DST_VALID_STRENGTH", 4));
    if (found) {
        SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
    } else {
        SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    }
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "IPOST_SEQ_NUM_MTU_CHECK_L3_MTU_PROFILEm", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "BCMLT_PT_INDEX", mtu_index));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "VALID", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "L3_MTU_LEN", 9416));
    SHR_IF_ERR_EXIT
        (bcmlt_pt_entry_commit(entry, BCMLT_PT_OPCODE_SET, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "EGR_L3_NEXT_HOP_1", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "L3_NEXT_HOP_1", next_hop_index));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "STRENGTH_PRFL_IDX", 2));
    rv = bcmlt_entry_commit(entry, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL);
    found = SHR_SUCCESS(rv);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "EDIT_CTRL_ID", EDIT_CTRL_ID_L3_NHOP_ROUTE));

    mac_addr = 0;
    for (idx = 0; idx < 6; idx++) {
        tmp = mac_da[idx];
        mac_addr <<= 8;
        mac_addr |= tmp;
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DST_MAC_ADDRESS", mac_addr));
    if (found) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(entry, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
    } else {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    }

    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "EGR_L3_NEXT_HOP_1_STR_PROFILE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "EGR_L3_NEXT_HOP_1_STR_PROFILE_INDEX", 2));
    rv = bcmlt_entry_commit(entry, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL);
    found = SHR_SUCCESS(rv);
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CLASSID_STR", 0));

    if (found) {
        SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
    } else {
        SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    }
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "IPOST_LAG_L2OIFm", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "BCMLT_PT_INDEX", (port_id) & 0x3ff));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "SYSTEM_DEST_TYPE", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "SYSTEM_DESTINATION", (port_id) & 0x7ff));
    SHR_IF_ERR_EXIT
        (bcmlt_pt_entry_commit(entry, BCMLT_PT_OPCODE_SET, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "IPOST_LAG_SYSTEM_PORTm", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "BCMLT_PT_INDEX", (port_id) & 0x7ff));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "LOCAL_DEST_TYPE", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "LOCAL_DESTINATION", port_id));
    SHR_IF_ERR_EXIT
        (bcmlt_pt_entry_commit(entry, BCMLT_PT_OPCODE_SET, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);


    SHR_IF_ERR_EXIT
        (bcm56880_a0_update_ipost_membership_check(unit, vlan_id));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "EGR_VFI", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "VFI", vlan_id));
    rv = bcmlt_entry_commit(entry, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL);
    found = SHR_SUCCESS(rv);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "STG", vlan_id & 0xff));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "VLAN_0", vlan_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "MEMBERSHIP_PROFILE_IDX", vlan_id & 0xff));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "TAG_ACTION", 1));
    if (found) {
        SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
    } else {
        SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    }
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcm56880_a0_update_egr_membership_check(unit, vlan_id));

exit:
    if (entry != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry);
    }


    SHR_FUNC_EXIT();

}

static int
bcm56880_a0_ecmp_member_add(int unit, int ecmp_member_index, int port_id,
                            int vlan_id, int next_hop_index, int intf_num,
                            int mtu_index, shr_mac_t mac_da, shr_mac_t mac_sa){
    int rv, found;
    bcmlt_entry_handle_t entry;
    int num_of_elem = 1;
    uint64_t num_paths;
    uint64_t data[num_of_elem];

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "ECMP_LEVEL1", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ECMP_ID", ecmp_member_index));
    rv = bcmlt_entry_commit(entry, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL);
    found = SHR_SUCCESS(rv);

    if (found) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(entry, "NUM_PATHS", &num_paths));
    } else {
        num_paths = 0;
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "NUM_PATHS", num_paths + 1));

    data[0] = next_hop_index;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry, "NHOP_INDEX_1", num_paths, data, num_of_elem));
    data[0] = 0x1;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry, "STRENGTH_PROFILE_INDEX", num_paths, data, num_of_elem));

    if (found) {
        SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
    } else {
        SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    }
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "ING_ECMP_MEMBER_1_STRENGTH_PROFILE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ING_ECMP_MEMBER_1_STRENGTH_PROFILE_INDEX", ECMP_MEMBER_STRENGTH_PROFILE_ID));
    rv = bcmlt_entry_commit(entry, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL);
    found = SHR_SUCCESS(rv);
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "NHOP_INDEX_1_STRENGTH", 7));
    if (found) {
        SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
    } else {
        SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    }

    SHR_IF_ERR_EXIT
        (bcm56880_a0_update_l3_ecmp_nhi(unit, ecmp_member_index, port_id,
                            vlan_id, next_hop_index, intf_num,
                            mtu_index, mac_da, mac_sa));

exit:
    if (entry != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry);
    }


    SHR_FUNC_EXIT();

}


static int
bcm56880_a0_update_pkt_integrity_check(int unit, bool enable){
    int rv;
    bcmlt_entry_handle_t entry;
    int found, i;
    int bmp_size = 256;
    int num_of_elem = 4;
    uint64_t data[4] = {0};

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "IPOST_TRACE_DROP_EVENT_DROP_EVENT_COPYTOCPU_MASKm", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "BCMLT_PT_INDEX", 0x0));
    for (i = 0; i < bmp_size; i++) {
        data[i/64] |= (uint64_t) 1 << (i%64);
    }
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry, "MASK", 0, data, num_of_elem));
    SHR_IF_ERR_EXIT
        (bcmlt_pt_entry_commit(entry, BCMLT_PT_OPCODE_SET, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "IPOST_TRACE_DROP_EVENT_TRACE_EVENT_COPYTOCPU_MASKm", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "BCMLT_PT_INDEX", 0x0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "MASK", 0xfffffffffffe));
    SHR_IF_ERR_EXIT
        (bcmlt_pt_entry_commit(entry, BCMLT_PT_OPCODE_SET, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    rv = 0 ;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "PKT_INTEGRITY_CHECK_TABLE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "TUNNEL_PROCESSING_RESULTS_1", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "MPLS_FLOW_TYPE", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "MY_STATION_1_HIT", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "L4_VALID", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ICMP_TYPE", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "IP_LAST_PROTOCOL", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "TCP_HDR_LEN_AND_FLAGS", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "FIXED_HVE_RESULT_0", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "FIXED_HVE_RESULT_1", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "FIXED_HVE_RESULT_2", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "FIXED_HVE_RESULT_3", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "FIXED_HVE_RESULT_4", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "FIXED_HVE_RESULT_5", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "FLEX_HVE_RESULT_0", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "FLEX_HVE_RESULT_1", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "FLEX_HVE_RESULT_2", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "TUNNEL_PROCESSING_RESULTS_1_MASK", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "MPLS_FLOW_TYPE_MASK", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "MY_STATION_1_HIT_MASK", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "L4_VALID_MASK", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ICMP_TYPE_MASK", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "IP_LAST_PROTOCOL_MASK", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "TCP_HDR_LEN_AND_FLAGS_MASK", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "FIXED_HVE_RESULT_0_MASK", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "FIXED_HVE_RESULT_1_MASK", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "FIXED_HVE_RESULT_2_MASK", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "FIXED_HVE_RESULT_3_MASK", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "FIXED_HVE_RESULT_4_MASK", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "FIXED_HVE_RESULT_5_MASK", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "FLEX_HVE_RESULT_0_MASK", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "FLEX_HVE_RESULT_1_MASK", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "FLEX_HVE_RESULT_2_MASK", 0));
    rv = bcmlt_entry_commit(entry, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL);
    found = SHR_SUCCESS(rv);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "COPY_TO_CPU", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ENTRY_PRIORITY", 10));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DROP", 1));

    if (found) {
        if (enable){
            SHR_IF_ERR_EXIT
                (bcmlt_entry_commit(entry, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
        } else {
            SHR_IF_ERR_EXIT
                (bcmlt_entry_commit(entry, BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL));
        }
    } else {
        if (enable) {
            SHR_IF_ERR_EXIT
                (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
        }
    }
    bcmlt_entry_free(entry);

exit:
    if (entry != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry);
    }


    SHR_FUNC_EXIT();

}

static int
bcm56880_a0_mirror_add(int unit, int mirror_port[8], int mirror_enable, int imirror,
                            int mirror_index[8]){
    bcmlt_entry_handle_t entry;
    int rv, found, i,j;
    int bmp_size = 160;
    int num_of_elem = (bmp_size + 63) >> 6;
    uint64_t bmp[3] = {0};
    uint64_t data[3] = {0};
    uint64_t lv;
    uint64_t session_type = 0;
    char *mirror_session_str[8] = {"IPOST_MIRROR_SAMPLER_MIRROR_SESSION_0m",
                                   "IPOST_MIRROR_SAMPLER_MIRROR_SESSION_1m",
                                   "IPOST_MIRROR_SAMPLER_MIRROR_SESSION_2m",
                                   "IPOST_MIRROR_SAMPLER_MIRROR_SESSION_3m",
                                   "IPOST_MIRROR_SAMPLER_MIRROR_SESSION_4m",
                                   "IPOST_MIRROR_SAMPLER_MIRROR_SESSION_5m",
                                   "IPOST_MIRROR_SAMPLER_MIRROR_SESSION_6m",
                                   "IPOST_MIRROR_SAMPLER_MIRROR_SESSION_7m"};

    char *emirror_ports_str[8] =  {"IPOST_MIRROR_SAMPLER_EMIRROR_PORTS_0m",
                                   "IPOST_MIRROR_SAMPLER_EMIRROR_PORTS_1m",
                                   "IPOST_MIRROR_SAMPLER_EMIRROR_PORTS_2m",
                                   "IPOST_MIRROR_SAMPLER_EMIRROR_PORTS_3m",
                                   "IPOST_MIRROR_SAMPLER_EMIRROR_PORTS_4m",
                                   "IPOST_MIRROR_SAMPLER_EMIRROR_PORTS_5m",
                                   "IPOST_MIRROR_SAMPLER_EMIRROR_PORTS_6m",
                                   "IPOST_MIRROR_SAMPLER_EMIRROR_PORTS_7m"};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(unit, "IPOST_MPB_CCBI_FIXED_ING_DEST_PORT_ENABLEm",
                                  &entry));
    SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry, "BCMLT_PT_INDEX", 0x0));
    for (i = 0; i < bmp_size; i++) {
        data[i / 64] |= (uint64_t) 1 << (i % 64);
    }
    SHR_IF_ERR_EXIT
            (bcmlt_entry_field_array_add(entry, "PORT_BITMAP", 0, data, num_of_elem));
    SHR_IF_ERR_EXIT
            (bcmlt_pt_entry_commit(entry, BCMLT_PT_OPCODE_SET, BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(unit, "IPOST_MIRROR_SAMPLER_CPU_BMPm", &entry));
    SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry, "BCMLT_PT_INDEX", 0x0));
    data[0]=0x0000000000000001;
    data[1]=0x0000000000000000;
    data[2]=0x0000000000000000;
    SHR_IF_ERR_EXIT
            (bcmlt_entry_field_array_add(entry, "BMP", 0, data, num_of_elem));
    SHR_IF_ERR_EXIT
            (bcmlt_pt_entry_commit(entry, BCMLT_PT_OPCODE_SET, BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(unit, "IPOST_MEMBERSHIP_CHECK_BLOCK_MASK_CPU_BMPm",
                                  &entry));
    SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry, "BCMLT_PT_INDEX", 0x0));
    data[0]=0x0000000000000001;
    data[1]=0x0000000000000000;
    data[2]=0x0000000000000000;
    SHR_IF_ERR_EXIT
            (bcmlt_entry_field_array_add(entry, "BMP", 0, data, num_of_elem));
    SHR_IF_ERR_EXIT
            (bcmlt_pt_entry_commit(entry, BCMLT_PT_OPCODE_SET, BCMLT_PRIORITY_NORMAL));

    for (i = 0; i < 160; i++) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(unit, "IPOST_MIRROR_SAMPLER_EMIRROR_CONTROL_0m",
                                  &entry));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry, "BCMLT_PT_INDEX", i));
        SHR_IF_ERR_EXIT
            (bcmlt_pt_entry_commit(entry, BCMLT_PT_OPCODE_GET, BCMLT_PRIORITY_NORMAL));
        if (mirror_enable & 1) {
            SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry, "ENABLE_0", 1));
            SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry, "VALUE_0", mirror_index[0]));
            SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry, "CONTAINER_0", 0));
        }

        if (mirror_enable & (1 << 1)) {
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry, "ENABLE_1", 1));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry, "VALUE_1", mirror_index[1]));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry, "CONTAINER_1", 1));
        }

        if (mirror_enable & (1 << 2)) {
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry, "ENABLE_2", 1));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry, "VALUE_2", mirror_index[2]));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry, "CONTAINER_2", 2));
        }

        if (mirror_enable & (1 << 3)) {
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry, "ENABLE_3", 1));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry, "VALUE_3", mirror_index[3]));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry, "CONTAINER_3", 3));
        }

        if (mirror_enable & (1 << 4)) {
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry, "ENABLE_4", 1));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry, "VALUE_4", mirror_index[4]));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry, "CONTAINER_4", 4));
        }

        if (mirror_enable & (1 << 5)) {
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry, "ENABLE_5", 1));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry, "VALUE_5", mirror_index[5]));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry, "CONTAINER_5", 5));
        }

        if (mirror_enable & (1 << 6)) {
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry, "ENABLE_6", 1));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry, "VALUE_6", mirror_index[6]));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry, "CONTAINER_6", 6));
        }

        if (mirror_enable & (1 << 7)) {
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry, "ENABLE_7", 1));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry, "VALUE_7", mirror_index[7]));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry, "CONTAINER_7", 7));
        }
        SHR_IF_ERR_EXIT
            (bcmlt_pt_entry_commit(entry, BCMLT_PT_OPCODE_SET, BCMLT_PRIORITY_NORMAL));
        bcmlt_entry_free(entry);


        lv = 0xffffffffffffffff;

        bmp[0] |= lv;
        bmp[1] |= lv;
        bmp[2] |= lv & ( ((uint64_t)1 << 32) - 1);


        for (j = 0; j < 8; j++) {
            SHR_IF_ERR_EXIT
                (bcmlt_entry_allocate(unit, emirror_ports_str[j], &entry));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry, "BCMLT_PT_INDEX", i));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_array_add(entry, "BITMAP", 0, bmp, num_of_elem));
            SHR_IF_ERR_EXIT
                (bcmlt_pt_entry_commit(entry, BCMLT_PT_OPCODE_SET, BCMLT_PRIORITY_NORMAL));
            bcmlt_entry_free(entry);
        }


    }



    for (i = 0; i < 8; i++) {
        if (mirror_enable & (1 << i)) {
            SHR_IF_ERR_EXIT
                (bcmlt_entry_allocate(unit, mirror_session_str[i], &entry));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry, "BCMLT_PT_INDEX", mirror_index[i]));
            SHR_IF_ERR_EXIT
                (bcmlt_pt_entry_commit(entry, BCMLT_PT_OPCODE_GET, BCMLT_PRIORITY_NORMAL));

            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry, "STRENGTH", 7));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry, "MIRR_SYSTEM_DEST_PORT", mirror_port[i]));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry, "MTP", mirror_index[i]));
            if (!(imirror & (1<<i))) {
                SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry, "EMIRROR", 1));
            }
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry, "MIRR_LOCAL_MTP", mirror_port[i]));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry, "ENABLE", 1));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry, "MC_COS", 1));
            SHR_IF_ERR_EXIT
                (bcmlt_pt_entry_commit(entry, BCMLT_PT_OPCODE_SET, BCMLT_PRIORITY_NORMAL));
            bcmlt_entry_free(entry);


            SHR_IF_ERR_EXIT
                (bcmlt_entry_allocate(unit, "EPRE_EDEV_CONFIG_DEVICE_PORT_TABLEm", &entry));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry, "BCMLT_PT_INDEX", mirror_port[i] & 0x7ff));
            SHR_IF_ERR_EXIT
                (bcmlt_pt_entry_commit(entry, BCMLT_PT_OPCODE_GET, BCMLT_PRIORITY_NORMAL));

            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry, "L2_OIF", (mirror_port[i]) & 0x7ff));
            SHR_IF_ERR_EXIT
                (bcmlt_pt_entry_commit(entry, BCMLT_PT_OPCODE_SET, BCMLT_PRIORITY_NORMAL));
            bcmlt_entry_free(entry);

            SHR_IF_ERR_EXIT
                (bcmlt_entry_allocate(unit, "EPRE_EDEV_CONFIG_MIRROR_ATTRIBUTES_TABLEm", &entry));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry, "BCMLT_PT_INDEX", mirror_index[i]));
            SHR_IF_ERR_EXIT
                (bcmlt_pt_entry_commit(entry, BCMLT_PT_OPCODE_GET, BCMLT_PRIORITY_NORMAL));

            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry, "SYSTEM_DEST_PORT", mirror_port[i]));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry, "TRUNCATE_EN", 0));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry, "MIRROR_EDIT_CTRL_ID", 0));
            SHR_IF_ERR_EXIT
                (bcmlt_pt_entry_commit(entry, BCMLT_PT_OPCODE_SET, BCMLT_PRIORITY_NORMAL));
            bcmlt_entry_free(entry);
        }

    }


    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "EGR_L2_OIF", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "L2_OIF", 0x3ff));
    rv = bcmlt_entry_commit(entry, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL);
    found = SHR_SUCCESS(rv);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "TPID_0", 0x8100));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "TPID_1", 0x9100));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "STRENGTH_PRFL_IDX", 1));
    if (found) {
         SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
    } else {
         SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    }
    bcmlt_entry_free(entry);




    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "EPRE_EDEV_CONFIG_MIRROR_TYPEr", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "BCMLT_PT_INDEX", 0));
    session_type=0;
    for (j = 0; j < 8; j++) {
        if (!(imirror & (1 << j))) {
            session_type|=1 << (mirror_index[j]);
        }
    }
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "MIRROR_SESSION_TYPE", session_type));
    SHR_IF_ERR_EXIT
        (bcmlt_pt_entry_commit(entry, BCMLT_PT_OPCODE_SET, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);




exit:
    if (entry != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry);
    }


    SHR_FUNC_EXIT();


}


static int
bcm56880_a0_dlb_ecmp_add(int unit){
    bcmlt_entry_handle_t entry;
    uint64_t value_array[10];
    int i=0;
    int j=0;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "ECMP_GROUP_LEVEL0_SHUFFLE_DEBUGr", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry,"BCMLT_PT_INDEX", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DEBUG_VALUE", 0xf123));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "EN", 0x1));
    SHR_IF_ERR_EXIT
        (bcmlt_pt_entry_commit(entry, BCMLT_PT_OPCODE_SET, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DLB_ECMP_DLB_ID_OFFSETr", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry,"BCMLT_PT_INDEX", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DLB_ID_OFFSET", 0x100));
    SHR_IF_ERR_EXIT
        (bcmlt_pt_entry_commit(entry, BCMLT_PT_OPCODE_SET, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DLB_ECMP_DLB_ID_0_TO_63_ENABLEr", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry,"BCMLT_PT_INDEX", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "BITMAP", 0xffffffffffffffff));
    SHR_IF_ERR_EXIT
        (bcmlt_pt_entry_commit(entry, BCMLT_PT_OPCODE_SET, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DLB_ECMP_DLB_ID_64_TO_127_ENABLEr", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry,"BCMLT_PT_INDEX", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "BITMAP", 0xffffffffffffffff));
    SHR_IF_ERR_EXIT
        (bcmlt_pt_entry_commit(entry, BCMLT_PT_OPCODE_SET, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DLB_ECMP_LINK_CONTROLm", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry,"BCMLT_PT_INDEX", 0));
    sal_memset(value_array, 0, sizeof(uint64_t) * 10);
    value_array[0]= 0xffffffffffffffff;
    value_array[1]= 0xffffffffffffffff;
    value_array[2]= 0xffffffff;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry, "SW_PORT_STATE",  0, value_array , 3));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry, "SW_OVERRIDE_PORT_MAP",  0, value_array , 3));
    SHR_IF_ERR_EXIT
        (bcmlt_pt_entry_commit(entry, BCMLT_PT_OPCODE_SET, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);


    for (j = 0; j < 128; j++) {
        SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DLB_ECMP_GROUP_CONTROLm", &entry));
        SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry,"BCMLT_PT_INDEX", j));
        SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "PRIMARY_GROUP_SIZE", 0x1));
        SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "PORT_ASSIGNMENT_MODE", 0x2));
        SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ENABLE_OPTIMAL_CANDIDATE_UPDATE", 0x1));
        SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "FLOW_SET_SIZE", 0x7));
        SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "FLOW_SET_BASE", 0x0));
        SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "GROUP_MEMBERSHIP_POINTER", j));
        SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ALTERNATE_PATH_BIAS", 0x7));
        SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "PRIMARY_PATH_THRESHOLD", 0x7));
        SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "INACTIVITY_DURATION", 0x64));
        SHR_IF_ERR_EXIT
        (bcmlt_pt_entry_commit(entry, BCMLT_PT_OPCODE_SET, BCMLT_PRIORITY_NORMAL));
        bcmlt_entry_free(entry);

        SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DLB_ECMP_GROUP_MEMBERSHIPm", &entry));
        SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry,"BCMLT_PT_INDEX",j));
        sal_memset(value_array, 0, sizeof(uint64_t) * 10);
        value_array[0]= 0x0000000000000006;
        value_array[1]= 0x0000000000000000;
        value_array[2]= 0x00000000;
        SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry, "PORT_MAP_ALTERNATE",  0, value_array , 3));
        SHR_IF_ERR_EXIT
        (bcmlt_pt_entry_commit(entry, BCMLT_PT_OPCODE_SET, BCMLT_PRIORITY_NORMAL));
        bcmlt_entry_free(entry);

        SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DLB_ECMP_GROUP_ALT_PORT_MEMBERS_0_TO_15m", &entry));
        SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry,"BCMLT_PT_INDEX", j));
        SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "MEMBER_0_PORT", 0x1));
        SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "MEMBER_0_PORT_VALID", 0x1));
        SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "MEMBER_1_PORT", 0x2));
        SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "MEMBER_1_PORT_VALID", 0x1));
        SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "MEMBER_2_PORT_VALID", 0x1));
        SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "MEMBER_2_PORT", 0x1));
        SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "MEMBER_3_PORT_VALID", 0x0));
        SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "MEMBER_3_PORT", 0x0));
        SHR_IF_ERR_EXIT
        (bcmlt_pt_entry_commit(entry, BCMLT_PT_OPCODE_SET, BCMLT_PRIORITY_NORMAL));
        bcmlt_entry_free(entry);
    }

    for (i = 0; i < 160; i++) {
        SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DLB_ECMP_PORT_QUALITY_UPDATE_MEASURE_CONTROLm", &entry));
        SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry,"BCMLT_PT_INDEX", i));
        SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ENABLE_MEASURE_AVERAGE_CALCULATION", 0x1));
        SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ENABLE_CREDIT_COLLECTION", 0x1));
        SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ENABLE_PORT_QUALITY_UPDATE", 0x1));
        SHR_IF_ERR_EXIT
        (bcmlt_pt_entry_commit(entry, BCMLT_PT_OPCODE_SET, BCMLT_PRIORITY_NORMAL));
        bcmlt_entry_free(entry);
    }


    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DLB_ECMP_PORT_QUALITY_MAPPINGm", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry,"BCMLT_PT_INDEX", 0x0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ASSIGNED_QUALITY", 0x7));
    SHR_IF_ERR_EXIT
        (bcmlt_pt_entry_commit(entry, BCMLT_PT_OPCODE_SET, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DLB_ECMP_QUALITY_MEASURE_CONTROLr", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry,"BCMLT_PT_INDEX", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "SAMPLING_PERIOD", 0x1));
    SHR_IF_ERR_EXIT
        (bcmlt_pt_entry_commit(entry, BCMLT_PT_OPCODE_SET, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "ING_DII_AUX_ARB_CONTROLr", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry,"BCMLT_PT_INDEX", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DLB_HGT_256NS_REFRESH_ENABLE", 0x1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "SBUS_SPACING", 0x20));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "REFRESH_TO_SBUS_RATIO", 0x3));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CLK_GRAN", 0x3));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "NULL_SLOT_PORT_INDEX", 0x14));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "SBUS_ARB_BLOCK_CNT", 0xff));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "FP_REFRESH_ENABLE", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_pt_entry_commit(entry, BCMLT_PT_OPCODE_SET, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    for (i = 0; i < 4; i++) {
            SHR_IF_ERR_EXIT
                (bcmlt_entry_allocate(unit, "ING_DII_SER_SCAN_CONFIGr", &entry));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry,"BCMLT_PT_INDEX", 0));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry,"BCMLT_PT_INSTANCE", i));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry, "ENABLE", 0x1));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry, "INTERVAL", 0x0));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry, "START_IDX", 0x0));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry, "MAX_IDX", 512));
            SHR_IF_ERR_EXIT
                (bcmlt_pt_entry_commit(entry, BCMLT_PT_OPCODE_SET, BCMLT_PRIORITY_NORMAL));
            bcmlt_entry_free(entry);

            SHR_IF_ERR_EXIT
                (bcmlt_entry_allocate(unit, "EGR_DII_SER_SCAN_CONFIGr", &entry));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry,"BCMLT_PT_INDEX", 0));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry,"BCMLT_PT_INSTANCE", i));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry, "ENABLE", 0x1));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry, "INTERVAL", 0x0));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry, "START_IDX", 0x0));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry, "MAX_IDX", 512));
            SHR_IF_ERR_EXIT
                (bcmlt_pt_entry_commit(entry, BCMLT_PT_OPCODE_SET, BCMLT_PRIORITY_NORMAL));
            bcmlt_entry_free(entry);
    }

exit:
    if (entry != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry);
    }


    SHR_FUNC_EXIT();

}


static int
bcm56880_a0_ifp_add(int unit){
    bcmlt_entry_handle_t entry;
    uint64_t i = 0;
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "EGR_TIMESTAMP_EGR_TS_PROFILEm", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "BCMLT_PT_INDEX", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "NTP_ENABLE", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_pt_entry_commit(entry, BCMLT_PT_OPCODE_SET, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DT_IFP_PDD_TEMPLATE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_PDD_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ING_OBJ_BUS_METER_TABLE_0_ACTION_SET", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "FLEX_CTR_VALID", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DT_IFP_PRESEL_GRP_TEMPLATE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_PRESEL_GRP_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DT_IFP_PRESEL_TEMPLATE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "IFP_PRESEL_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DT_IFP_RULE_TEMPLATE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "IFP_RULE_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "PKT_FWD_FIELD_BUS_MACDA", 0x0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "PKT_FWD_FIELD_BUS_MACDA_MASK", 0x1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DT_IFP_RULE_TEMPLATE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "IFP_RULE_TEMPLATE_INDEX", 2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "PKT_FWD_FIELD_BUS_MACDA", 0x1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "PKT_FWD_FIELD_BUS_MACDA_MASK", 0x1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DT_IFP_ACTION_TEMPLATE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "IFP_ACTION_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ING_OBJ_BUS_METER_TABLE_0_ACTION_SET", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DT_IFP_SBR_TEMPLATE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_SBR_TEMPLATE_INDEX", 1));
    /*
     * SHR_IF_ERR_EXIT
     *   (bcmlt_entry_field_add(entry, "ING_OBJ_BUS_METER_TABLE_0_ACTION_SET", 1));
     */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    /*
     * Create one entry in group 1 with priority 1(This entry will occupy slice 0)
     */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DT_IFP_GRP_TEMPLATE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_GRP_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_PDD_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_SBR_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "MODE_AUTO", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_PRESEL_GRP_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "PKT_FWD_FIELD_BUS_MACDA", 0xFFFFFFFFFFFF));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ENTRY_PRIORITY", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DT_IFP_ENTRY", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_ENTRY_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_GRP_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_RULE_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_ACTION_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "FLEX_CTR_ACTION", 14));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    /*
     * Create one entry in group 2 with priority 2(This entry will occupy slice 1)
     */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DT_IFP_GRP_TEMPLATE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_GRP_TEMPLATE_INDEX", 2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_PDD_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_SBR_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "MODE_AUTO", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_PRESEL_GRP_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "PKT_FWD_FIELD_BUS_MACDA", 0xFFFFFFFFFFFF));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ENTRY_PRIORITY", 2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DT_IFP_ENTRY", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_ENTRY_INDEX", 2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_GRP_TEMPLATE_INDEX", 2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_RULE_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_ACTION_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "FLEX_CTR_ACTION", 14));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    /*
     * Create one entry in group 3 with priority 3(This entry will occupy slice 2)
     */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DT_IFP_GRP_TEMPLATE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_GRP_TEMPLATE_INDEX", 3));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_PDD_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_SBR_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "MODE_AUTO", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_PRESEL_GRP_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "PKT_FWD_FIELD_BUS_MACDA", 0xFFFFFFFFFFFF));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ENTRY_PRIORITY", 3));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DT_IFP_ENTRY", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_ENTRY_INDEX", 3));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_GRP_TEMPLATE_INDEX", 3));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_RULE_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_ACTION_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "FLEX_CTR_ACTION", 14));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    /*
     * Create one entry in group 4 with priority 4(This entry will occupy slice 3)
     */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DT_IFP_GRP_TEMPLATE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_GRP_TEMPLATE_INDEX", 4));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_PDD_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_SBR_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "MODE_AUTO", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_PRESEL_GRP_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "PKT_FWD_FIELD_BUS_MACDA", 0xFFFFFFFFFFFF));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ENTRY_PRIORITY", 4));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DT_IFP_ENTRY", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_ENTRY_INDEX", 4));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_GRP_TEMPLATE_INDEX", 4));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_RULE_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_ACTION_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "FLEX_CTR_ACTION", 14));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    /*
     * Create one entry in group 5 with priority 5(This entry will occupy slice 4)
     */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DT_IFP_GRP_TEMPLATE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_GRP_TEMPLATE_INDEX", 5));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_PDD_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_SBR_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "MODE_AUTO", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_PRESEL_GRP_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "PKT_FWD_FIELD_BUS_MACDA", 0xFFFFFFFFFFFF));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ENTRY_PRIORITY", 5));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DT_IFP_ENTRY", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_ENTRY_INDEX", 5));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_GRP_TEMPLATE_INDEX", 5));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_RULE_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_ACTION_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "FLEX_CTR_ACTION", 14));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    /*
     * Create one entry in group 6 with priority 6(This entry will occupy slice 5)
     */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DT_IFP_GRP_TEMPLATE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_GRP_TEMPLATE_INDEX", 6));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_PDD_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_SBR_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "MODE_AUTO", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_PRESEL_GRP_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "PKT_FWD_FIELD_BUS_MACDA", 0xFFFFFFFFFFFF));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ENTRY_PRIORITY", 6));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DT_IFP_ENTRY", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_ENTRY_INDEX", 6));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_GRP_TEMPLATE_INDEX", 6));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_RULE_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_ACTION_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "FLEX_CTR_ACTION", 14));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    /*
     * Create one entry in group 7 with priority 7(This entry will occupy slice 6)
     */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DT_IFP_GRP_TEMPLATE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_GRP_TEMPLATE_INDEX", 7));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_PDD_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_SBR_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "MODE_AUTO", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_PRESEL_GRP_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "PKT_FWD_FIELD_BUS_MACDA", 0xFFFFFFFFFFFF));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ENTRY_PRIORITY", 7));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DT_IFP_ENTRY", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_ENTRY_INDEX", 7));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_GRP_TEMPLATE_INDEX", 7));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_RULE_TEMPLATE_INDEX", 2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_ACTION_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "FLEX_CTR_ACTION", 2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    /*
     * Create one entry in group 8 with priority 8(This entry will occupy slice 7)
     */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DT_IFP_GRP_TEMPLATE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_GRP_TEMPLATE_INDEX", 8));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_PDD_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_SBR_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "MODE_AUTO", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_PRESEL_GRP_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "PKT_FWD_FIELD_BUS_MACDA", 0xFFFFFFFFFFFF));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ENTRY_PRIORITY", 8));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DT_IFP_ENTRY", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_ENTRY_INDEX", 8));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_GRP_TEMPLATE_INDEX", 8));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_RULE_TEMPLATE_INDEX", 2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_ACTION_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "FLEX_CTR_ACTION", 2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    /*
     * Create one entry in group 9 with priority 9(This entry will occupy slice 8)
     */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DT_IFP_GRP_TEMPLATE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_GRP_TEMPLATE_INDEX", 9));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_PDD_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_SBR_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "MODE_AUTO", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_PRESEL_GRP_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "PKT_FWD_FIELD_BUS_MACDA", 0xFFFFFFFFFFFF));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ENTRY_PRIORITY", 9));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DT_IFP_ENTRY", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_ENTRY_INDEX", 9));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_GRP_TEMPLATE_INDEX", 9));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_RULE_TEMPLATE_INDEX", 2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_ACTION_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "FLEX_CTR_ACTION", 2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    /*
     * Create one entry in group 10 with priority 10(This entry will occupy slice 9)
     */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DT_IFP_GRP_TEMPLATE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_GRP_TEMPLATE_INDEX", 10));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_PDD_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_SBR_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "MODE_AUTO", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_PRESEL_GRP_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "PKT_FWD_FIELD_BUS_MACDA", 0xFFFFFFFFFFFF));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ENTRY_PRIORITY", 10));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DT_IFP_ENTRY", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_ENTRY_INDEX", 10));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_GRP_TEMPLATE_INDEX", 10));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_RULE_TEMPLATE_INDEX", 2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_ACTION_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "FLEX_CTR_ACTION", 2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    /*
     * Create one entry in group 11 with priority 11(This entry will occupy slice 10)
     */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DT_IFP_GRP_TEMPLATE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_GRP_TEMPLATE_INDEX", 11));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_PDD_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_SBR_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "MODE_AUTO", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_PRESEL_GRP_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "PKT_FWD_FIELD_BUS_MACDA", 0xFFFFFFFFFFFF));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ENTRY_PRIORITY", 5));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DT_IFP_ENTRY", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_ENTRY_INDEX", 11));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_GRP_TEMPLATE_INDEX", 11));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_RULE_TEMPLATE_INDEX", 2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_ACTION_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "FLEX_CTR_ACTION", 2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    /*
     * Create one entry in group 12 with priority 12(This entry will occupy slice 11)
     */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DT_IFP_GRP_TEMPLATE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_GRP_TEMPLATE_INDEX", 12));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_PDD_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_SBR_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "MODE_AUTO", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_PRESEL_GRP_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "PKT_FWD_FIELD_BUS_MACDA", 0xFFFFFFFFFFFF));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ENTRY_PRIORITY", 12));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DT_IFP_ENTRY", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_ENTRY_INDEX", 12));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_GRP_TEMPLATE_INDEX", 12));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_RULE_TEMPLATE_INDEX", 2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_IFP_ACTION_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "FLEX_CTR_ACTION", 2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    for (i = 0; i < 512; i++) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(unit, "IFP_METER_METER_TABLEm", &entry));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry,"BCMLT_PT_INDEX", i));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry, "BUCKETSIZE", 0xfff));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry, "REFRESHCOUNT", 0x3));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry, "REFRESH_MODE", 0x0));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry, "METER_GRAN", 0));
        SHR_IF_ERR_EXIT
            (bcmlt_pt_entry_commit(entry, BCMLT_PT_OPCODE_SET, BCMLT_PRIORITY_NORMAL));
        bcmlt_entry_free(entry);
    }

exit:
    if (entry != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry);
    }

    SHR_FUNC_EXIT();

}

static int
bcm56880_a0_efp_add(int unit){
    bcmlt_entry_handle_t entry;
    uint64_t i = 0;
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DT_EFP_PDD_TEMPLATE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_EFP_PDD_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "EGR_METER_TRIGGERS_METER_ACTION_SET", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "FLEX_CTR_VALID", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DT_EFP_PRESEL_GRP_TEMPLATE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_EFP_PRESEL_GRP_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DT_EFP_PRESEL_TEMPLATE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "EFP_PRESEL_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DT_EFP_RULE_TEMPLATE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "EFP_RULE_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "EGR_FIELD_BUS_FWD_MAC_DA", 0x0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "EGR_FIELD_BUS_FWD_MAC_DA_MASK", 0x1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DT_EFP_RULE_TEMPLATE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "EFP_RULE_TEMPLATE_INDEX", 2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "EGR_FIELD_BUS_FWD_MAC_DA", 0x1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "EGR_FIELD_BUS_FWD_MAC_DA_MASK", 0x1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DT_EFP_ACTION_TEMPLATE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "EFP_ACTION_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "EGR_METER_TRIGGERS_METER_ACTION_SET", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);


    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DT_EFP_SBR_TEMPLATE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_EFP_SBR_TEMPLATE_INDEX", 1));
    /*
     * SHR_IF_ERR_EXIT
     *    (bcmlt_entry_field_add(entry, "EGR_METER_TRIGGERS_METER_ACTION_SET", 1));
     */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);


    /*
     * Create one entry in group 1 with priority 1(This entry will occupy slice 0)
     */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DT_EFP_GRP_TEMPLATE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_EFP_GRP_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_EFP_PDD_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_EFP_SBR_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "MODE_AUTO", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_EFP_PRESEL_GRP_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "EGR_FIELD_BUS_FWD_MAC_DA", 0xFFFFFFFFFFFF));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ENTRY_PRIORITY", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DT_EFP_ENTRY", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_EFP_ENTRY_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_EFP_GRP_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_EFP_RULE_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_EFP_ACTION_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "FLEX_CTR_ACTION", 10));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    /*
     * Create one entry in group 2 with priority 2(This entry will occupy slice 1)
     */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DT_EFP_GRP_TEMPLATE",  &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_EFP_GRP_TEMPLATE_INDEX", 2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_EFP_PDD_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_EFP_SBR_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "MODE_AUTO", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_EFP_PRESEL_GRP_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "EGR_FIELD_BUS_FWD_MAC_DA", 0xFFFFFFFFFFFF));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ENTRY_PRIORITY", 2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DT_EFP_ENTRY", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_EFP_ENTRY_INDEX", 2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_EFP_GRP_TEMPLATE_INDEX", 2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_EFP_RULE_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_EFP_ACTION_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "FLEX_CTR_ACTION", 10));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    /*
     * Create one entry in group 3 with priority 3(This entry will occupy slice 2)
     */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DT_EFP_GRP_TEMPLATE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_EFP_GRP_TEMPLATE_INDEX", 3));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_EFP_PDD_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_EFP_SBR_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "MODE_AUTO", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_EFP_PRESEL_GRP_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "EGR_FIELD_BUS_FWD_MAC_DA", 0xFFFFFFFFFFFF));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ENTRY_PRIORITY", 3));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DT_EFP_ENTRY", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_EFP_ENTRY_INDEX", 3));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_EFP_GRP_TEMPLATE_INDEX", 3));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_EFP_RULE_TEMPLATE_INDEX", 2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_EFP_ACTION_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "FLEX_CTR_ACTION", 2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    /*
     * Create one entry in group 4 with priority 4(This entry will occupy slice 3)
     */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DT_EFP_GRP_TEMPLATE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_EFP_GRP_TEMPLATE_INDEX", 4));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_EFP_PDD_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_EFP_SBR_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "MODE_AUTO", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_EFP_PRESEL_GRP_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "EGR_FIELD_BUS_FWD_MAC_DA", 0xFFFFFFFFFFFF));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ENTRY_PRIORITY", 4));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DT_EFP_ENTRY", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_EFP_ENTRY_INDEX", 4));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_EFP_GRP_TEMPLATE_INDEX", 4));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_EFP_RULE_TEMPLATE_INDEX", 2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "DT_EFP_ACTION_TEMPLATE_INDEX", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "FLEX_CTR_ACTION", 2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    for (i = 0; i < 128; i++) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(unit, "EFP_METER_METER_TABLEm", &entry));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry,"BCMLT_PT_INDEX",  i));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry, "BUCKETSIZE", 0xfff));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry, "REFRESHCOUNT", 0x3fffff));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry, "REFRESH_MODE", 0x0));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry, "METER_GRAN", 0));
        SHR_IF_ERR_EXIT
            (bcmlt_pt_entry_commit(entry, BCMLT_PT_OPCODE_SET, BCMLT_PRIORITY_NORMAL));
        bcmlt_entry_free(entry);
    }

exit:
    if (entry != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry);
    }

    SHR_FUNC_EXIT();
}

static int
bcm56880_a0_flex_ctr_add(int unit){
    bcmlt_entry_handle_t entry;
    uint64_t fval = 0;
    SHR_FUNC_ENTER(unit);
    /*
     * ingress flex ctr
     */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_ING_EFLEX_ACTION_PROFILE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_ING_EFLEX_ACTION_PROFILE_ID", 2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ACTION", 2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "POOL_ID", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "NUM_COUNTERS", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "MODE", "WIDE"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_A", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "CTR_SRC_A", "USE_PKTLEN"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "COUNT_ON_HW_EXCP_DROP", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "COUNT_ON_RULE_DROP", "ALWAYS_COUNT"));
    SHR_IF_ERR_EXIT
    (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_ING_EFLEX_ACTION_PROFILE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_ING_EFLEX_ACTION_PROFILE_ID", 3));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ACTION", 3));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "POOL_ID", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "NUM_COUNTERS", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "MODE", "WIDE"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_A", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "CTR_SRC_A", "USE_PKTLEN"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "COUNT_ON_HW_EXCP_DROP", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "COUNT_ON_RULE_DROP", "ALWAYS_COUNT"));
    SHR_IF_ERR_EXIT
    (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_ING_EFLEX_ACTION_PROFILE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_ING_EFLEX_ACTION_PROFILE_ID", 4));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ACTION", 4));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "POOL_ID", 2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "NUM_COUNTERS", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "MODE", "WIDE"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_A", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "CTR_SRC_A", "USE_PKTLEN"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "COUNT_ON_HW_EXCP_DROP", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "COUNT_ON_RULE_DROP", "ALWAYS_COUNT"));
    SHR_IF_ERR_EXIT
    (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_ING_EFLEX_ACTION_PROFILE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_ING_EFLEX_ACTION_PROFILE_ID", 5));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ACTION", 5));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "POOL_ID", 3));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "NUM_COUNTERS", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "MODE", "WIDE"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_A", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "CTR_SRC_A", "USE_PKTLEN"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "COUNT_ON_HW_EXCP_DROP", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "COUNT_ON_RULE_DROP", "ALWAYS_COUNT"));
    SHR_IF_ERR_EXIT
    (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_ING_EFLEX_ACTION_PROFILE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_ING_EFLEX_ACTION_PROFILE_ID", 6));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ACTION", 6));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "POOL_ID", 4));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "NUM_COUNTERS", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "MODE", "WIDE"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_A", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "CTR_SRC_A", "USE_PKTLEN"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "COUNT_ON_HW_EXCP_DROP", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "COUNT_ON_RULE_DROP", "ALWAYS_COUNT"));
    SHR_IF_ERR_EXIT
    (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_ING_EFLEX_ACTION_PROFILE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_ING_EFLEX_ACTION_PROFILE_ID", 7));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ACTION", 7));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "POOL_ID", 5));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "NUM_COUNTERS", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "MODE", "WIDE"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_A", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "CTR_SRC_A", "USE_PKTLEN"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "COUNT_ON_HW_EXCP_DROP", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "COUNT_ON_RULE_DROP", "ALWAYS_COUNT"));
    SHR_IF_ERR_EXIT
    (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_ING_EFLEX_ACTION_PROFILE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_ING_EFLEX_ACTION_PROFILE_ID", 8));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ACTION", 8));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "POOL_ID", 6));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "NUM_COUNTERS", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "MODE", "WIDE"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_A", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "CTR_SRC_A", "USE_PKTLEN"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "COUNT_ON_HW_EXCP_DROP", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "COUNT_ON_RULE_DROP", "ALWAYS_COUNT"));
    SHR_IF_ERR_EXIT
    (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_ING_EFLEX_ACTION_PROFILE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_ING_EFLEX_ACTION_PROFILE_ID", 9));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ACTION", 9));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "POOL_ID", 7));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "NUM_COUNTERS", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "MODE", "WIDE"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_A", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "CTR_SRC_A", "USE_PKTLEN"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "COUNT_ON_HW_EXCP_DROP", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "COUNT_ON_RULE_DROP", "ALWAYS_COUNT"));
    SHR_IF_ERR_EXIT
    (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_ING_EFLEX_ACTION_PROFILE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_ING_EFLEX_ACTION_PROFILE_ID", 10));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ACTION", 10));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "POOL_ID", 8));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "NUM_COUNTERS", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "MODE", "WIDE"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_A", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "CTR_SRC_A", "USE_PKTLEN"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "COUNT_ON_HW_EXCP_DROP", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "COUNT_ON_RULE_DROP", "ALWAYS_COUNT"));
    SHR_IF_ERR_EXIT
    (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_ING_EFLEX_ACTION_PROFILE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_ING_EFLEX_ACTION_PROFILE_ID", 11));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ACTION", 11));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "POOL_ID", 9));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "NUM_COUNTERS", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "MODE", "WIDE"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_A", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "CTR_SRC_A", "USE_PKTLEN"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "COUNT_ON_HW_EXCP_DROP", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "COUNT_ON_RULE_DROP", "ALWAYS_COUNT"));
    SHR_IF_ERR_EXIT
    (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_ING_EFLEX_ACTION_PROFILE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_ING_EFLEX_ACTION_PROFILE_ID", 12));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ACTION", 12));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "POOL_ID", 10));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "NUM_COUNTERS", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "MODE", "WIDE"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_A", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "CTR_SRC_A", "USE_PKTLEN"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "COUNT_ON_HW_EXCP_DROP", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "COUNT_ON_RULE_DROP", "ALWAYS_COUNT"));
    SHR_IF_ERR_EXIT
    (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_ING_EFLEX_ACTION_PROFILE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_ING_EFLEX_ACTION_PROFILE_ID", 13));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ACTION", 13));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "POOL_ID", 11));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "NUM_COUNTERS", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "MODE", "WIDE"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_A", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "CTR_SRC_A", "USE_PKTLEN"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "COUNT_ON_HW_EXCP_DROP", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "COUNT_ON_RULE_DROP", "ALWAYS_COUNT"));
    SHR_IF_ERR_EXIT
    (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_ING_EFLEX_GROUP_ACTION_PROFILE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_ING_EFLEX_GROUP_ACTION_PROFILE_ID", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_ING_EFLEX_ACTION_PROFILE_ID", 2));
    fval = 3;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry, "GROUP_MAP", 0, &fval, 1));
    fval = 4;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry, "GROUP_MAP", 1, &fval, 1));
    fval = 5;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry, "GROUP_MAP", 2, &fval, 1));
    fval = 6;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry, "GROUP_MAP", 3, &fval, 1));
    fval = 7;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry, "GROUP_MAP", 4, &fval, 1));
    fval = 8;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry, "GROUP_MAP", 5, &fval, 1));
    fval = 9;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry, "GROUP_MAP", 6, &fval, 1));
    fval = 10;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry, "GROUP_MAP", 7, &fval, 1));
    fval = 11;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry, "GROUP_MAP", 8, &fval, 1));
    fval = 12;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry, "GROUP_MAP", 9, &fval, 1));
    fval = 13;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry, "GROUP_MAP", 10, &fval, 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    /*
     * ingress flex ctr second profile
     * Create ingress operand profile 1 with absolute value 1 to count number of packets
     */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_ING_EFLEX_OPERAND_PROFILE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_ING_EFLEX_OPERAND_PROFILE_ID", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "PROFILE", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "OBJ_1", "USE_1"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_ING_EFLEX_ACTION_PROFILE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_ING_EFLEX_ACTION_PROFILE_ID", 14));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ACTION", 14));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "POOL_ID", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "NUM_COUNTERS", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "MODE", "NORMAL"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_A", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_B", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "VAL_A_CTR_ING_EFLEX_OPERAND_PROFILE_ID", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "CTR_SRC_B", "USE_PKTLEN"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "COUNT_ON_HW_EXCP_DROP", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "COUNT_ON_RULE_DROP", "ALWAYS_COUNT"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_ING_EFLEX_ACTION_PROFILE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_ING_EFLEX_ACTION_PROFILE_ID", 15));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ACTION", 15));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "POOL_ID", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "NUM_COUNTERS", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "MODE", "NORMAL"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_A", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_B", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "VAL_A_CTR_ING_EFLEX_OPERAND_PROFILE_ID", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "CTR_SRC_B", "USE_PKTLEN"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "COUNT_ON_HW_EXCP_DROP", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "COUNT_ON_RULE_DROP", "ALWAYS_COUNT"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_ING_EFLEX_ACTION_PROFILE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_ING_EFLEX_ACTION_PROFILE_ID", 16));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ACTION", 16));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "POOL_ID", 2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "NUM_COUNTERS", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "MODE", "NORMAL"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_A", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_B", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "VAL_A_CTR_ING_EFLEX_OPERAND_PROFILE_ID", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "CTR_SRC_B", "USE_PKTLEN"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "COUNT_ON_HW_EXCP_DROP", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "COUNT_ON_RULE_DROP", "ALWAYS_COUNT"));
    SHR_IF_ERR_EXIT
    (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_ING_EFLEX_ACTION_PROFILE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_ING_EFLEX_ACTION_PROFILE_ID", 17));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ACTION", 17));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "POOL_ID", 3));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "NUM_COUNTERS", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "MODE", "NORMAL"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_A", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_B", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "VAL_A_CTR_ING_EFLEX_OPERAND_PROFILE_ID", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "CTR_SRC_B", "USE_PKTLEN"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "COUNT_ON_HW_EXCP_DROP", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "COUNT_ON_RULE_DROP", "ALWAYS_COUNT"));
    SHR_IF_ERR_EXIT
    (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_ING_EFLEX_ACTION_PROFILE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_ING_EFLEX_ACTION_PROFILE_ID", 18));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ACTION", 18));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "POOL_ID", 4));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "NUM_COUNTERS", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "MODE", "NORMAL"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_A", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_B", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "VAL_A_CTR_ING_EFLEX_OPERAND_PROFILE_ID", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "CTR_SRC_B", "USE_PKTLEN"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "COUNT_ON_HW_EXCP_DROP", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "COUNT_ON_RULE_DROP", "ALWAYS_COUNT"));
    SHR_IF_ERR_EXIT
    (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_ING_EFLEX_ACTION_PROFILE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_ING_EFLEX_ACTION_PROFILE_ID", 19));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ACTION", 19));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "POOL_ID", 5));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "NUM_COUNTERS", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "MODE", "NORMAL"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_A", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_B", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "VAL_A_CTR_ING_EFLEX_OPERAND_PROFILE_ID", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "CTR_SRC_B", "USE_PKTLEN"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "COUNT_ON_HW_EXCP_DROP", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "COUNT_ON_RULE_DROP", "ALWAYS_COUNT"));
    SHR_IF_ERR_EXIT
    (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_ING_EFLEX_ACTION_PROFILE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_ING_EFLEX_ACTION_PROFILE_ID", 20));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ACTION", 20));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "POOL_ID", 6));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "NUM_COUNTERS", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "MODE", "NORMAL"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_A", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_B", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "VAL_A_CTR_ING_EFLEX_OPERAND_PROFILE_ID", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "CTR_SRC_B", "USE_PKTLEN"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "COUNT_ON_HW_EXCP_DROP", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "COUNT_ON_RULE_DROP", "ALWAYS_COUNT"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_ING_EFLEX_ACTION_PROFILE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_ING_EFLEX_ACTION_PROFILE_ID", 21));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ACTION", 21));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "POOL_ID", 7));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "NUM_COUNTERS", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "MODE", "NORMAL"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_A", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_B", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "VAL_A_CTR_ING_EFLEX_OPERAND_PROFILE_ID", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "CTR_SRC_B", "USE_PKTLEN"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "COUNT_ON_HW_EXCP_DROP", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "COUNT_ON_RULE_DROP", "ALWAYS_COUNT"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_ING_EFLEX_ACTION_PROFILE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_ING_EFLEX_ACTION_PROFILE_ID", 22));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ACTION", 22));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "POOL_ID", 8));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "NUM_COUNTERS", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "MODE", "NORMAL"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_A", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_B", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "VAL_A_CTR_ING_EFLEX_OPERAND_PROFILE_ID", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "CTR_SRC_B", "USE_PKTLEN"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "COUNT_ON_HW_EXCP_DROP", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "COUNT_ON_RULE_DROP", "ALWAYS_COUNT"));
    SHR_IF_ERR_EXIT
    (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_ING_EFLEX_ACTION_PROFILE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_ING_EFLEX_ACTION_PROFILE_ID", 23));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ACTION", 23));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "POOL_ID", 9));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "NUM_COUNTERS", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "MODE", "NORMAL"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_A", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_B", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "VAL_A_CTR_ING_EFLEX_OPERAND_PROFILE_ID", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "CTR_SRC_B", "USE_PKTLEN"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "COUNT_ON_HW_EXCP_DROP", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "COUNT_ON_RULE_DROP", "ALWAYS_COUNT"));
    SHR_IF_ERR_EXIT
    (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_ING_EFLEX_ACTION_PROFILE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_ING_EFLEX_ACTION_PROFILE_ID", 24));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ACTION", 24));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "POOL_ID", 10));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "NUM_COUNTERS", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "MODE", "NORMAL"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_A", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_B", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "VAL_A_CTR_ING_EFLEX_OPERAND_PROFILE_ID", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "CTR_SRC_B", "USE_PKTLEN"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "COUNT_ON_HW_EXCP_DROP", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "COUNT_ON_RULE_DROP", "ALWAYS_COUNT"));
    SHR_IF_ERR_EXIT
    (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_ING_EFLEX_ACTION_PROFILE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_ING_EFLEX_ACTION_PROFILE_ID", 25));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ACTION", 25));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "POOL_ID", 11));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "NUM_COUNTERS", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "MODE", "NORMAL"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_A", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_B", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "VAL_A_CTR_ING_EFLEX_OPERAND_PROFILE_ID", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "CTR_SRC_B", "USE_PKTLEN"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "COUNT_ON_HW_EXCP_DROP", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "COUNT_ON_RULE_DROP", "ALWAYS_COUNT"));
    SHR_IF_ERR_EXIT
    (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_ING_EFLEX_GROUP_ACTION_PROFILE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_ING_EFLEX_GROUP_ACTION_PROFILE_ID", 2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_ING_EFLEX_ACTION_PROFILE_ID", 14));
    fval = 15;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry, "GROUP_MAP", 0, &fval, 1));
    fval = 16;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry, "GROUP_MAP", 1, &fval, 1));
    fval = 17;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry, "GROUP_MAP", 2, &fval, 1));
    fval = 18;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry, "GROUP_MAP", 3, &fval, 1));
    fval = 19;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry, "GROUP_MAP", 4, &fval, 1));
    fval = 20;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry, "GROUP_MAP", 5, &fval, 1));
    fval = 21;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry, "GROUP_MAP", 6, &fval, 1));
    fval = 22;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry, "GROUP_MAP", 7, &fval, 1));
    fval = 23;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry, "GROUP_MAP", 8, &fval, 1));
    fval = 24;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry, "GROUP_MAP", 9, &fval, 1));
    fval = 25;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry, "GROUP_MAP", 10, &fval, 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "GROUP", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_ING_EFLEX_STATS", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_ING_EFLEX_ACTION_PROFILE_ID", 2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_EFLEX_INDEX", 0));
    SHR_IF_ERR_EXIT
    (bcmlt_entry_commit(entry, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    /*
     * egress flex ctr
     */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_EGR_EFLEX_ACTION_PROFILE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_EGR_EFLEX_ACTION_PROFILE_ID", 2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ACTION", 2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "POOL_ID", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "NUM_COUNTERS", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "MODE", "WIDE"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_A", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "CTR_SRC_A", "USE_PKTLEN"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "COUNT_ON_HW_EXCP_DROP", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "COUNT_ON_RULE_DROP", "ALWAYS_COUNT"));
    SHR_IF_ERR_EXIT
    (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_EGR_EFLEX_ACTION_PROFILE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_EGR_EFLEX_ACTION_PROFILE_ID", 3));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ACTION", 3));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "POOL_ID", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "NUM_COUNTERS", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "MODE", "WIDE"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_A", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "CTR_SRC_A", "USE_PKTLEN"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "COUNT_ON_HW_EXCP_DROP", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "COUNT_ON_RULE_DROP", "ALWAYS_COUNT"));
    SHR_IF_ERR_EXIT
    (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_EGR_EFLEX_ACTION_PROFILE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_EGR_EFLEX_ACTION_PROFILE_ID", 4));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ACTION", 4));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "POOL_ID", 2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "NUM_COUNTERS", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "MODE", "WIDE"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_A", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "CTR_SRC_A", "USE_PKTLEN"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "COUNT_ON_HW_EXCP_DROP", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "COUNT_ON_RULE_DROP", "ALWAYS_COUNT"));
    SHR_IF_ERR_EXIT
    (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_EGR_EFLEX_ACTION_PROFILE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_EGR_EFLEX_ACTION_PROFILE_ID", 5));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ACTION", 5));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "POOL_ID", 3));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "NUM_COUNTERS", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "MODE", "WIDE"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_A", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "CTR_SRC_A", "USE_PKTLEN"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "COUNT_ON_HW_EXCP_DROP", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "COUNT_ON_RULE_DROP", "ALWAYS_COUNT"));
    SHR_IF_ERR_EXIT
    (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_EGR_EFLEX_ACTION_PROFILE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_EGR_EFLEX_ACTION_PROFILE_ID", 6));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ACTION", 6));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "POOL_ID", 4));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "NUM_COUNTERS", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "MODE", "WIDE"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_A", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "CTR_SRC_A", "USE_PKTLEN"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "COUNT_ON_HW_EXCP_DROP", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "COUNT_ON_RULE_DROP", "ALWAYS_COUNT"));
    SHR_IF_ERR_EXIT
    (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_EGR_EFLEX_ACTION_PROFILE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_EGR_EFLEX_ACTION_PROFILE_ID", 7));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ACTION", 7));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "POOL_ID", 5));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "NUM_COUNTERS", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "MODE", "WIDE"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_A", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "CTR_SRC_A", "USE_PKTLEN"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "COUNT_ON_HW_EXCP_DROP", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "COUNT_ON_RULE_DROP", "ALWAYS_COUNT"));
    SHR_IF_ERR_EXIT
    (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_EGR_EFLEX_ACTION_PROFILE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_EGR_EFLEX_ACTION_PROFILE_ID", 8));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ACTION", 8));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "POOL_ID", 6));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "NUM_COUNTERS", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "MODE", "WIDE"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_A", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "CTR_SRC_A", "USE_PKTLEN"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "COUNT_ON_HW_EXCP_DROP", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "COUNT_ON_RULE_DROP", "ALWAYS_COUNT"));
    SHR_IF_ERR_EXIT
    (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_EGR_EFLEX_ACTION_PROFILE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_EGR_EFLEX_ACTION_PROFILE_ID", 9));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ACTION", 9));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "POOL_ID", 7));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "NUM_COUNTERS", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "MODE", "WIDE"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_A", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "CTR_SRC_A", "USE_PKTLEN"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "COUNT_ON_HW_EXCP_DROP", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "COUNT_ON_RULE_DROP", "ALWAYS_COUNT"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_EGR_EFLEX_GROUP_ACTION_PROFILE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_EGR_EFLEX_GROUP_ACTION_PROFILE_ID", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_EGR_EFLEX_ACTION_PROFILE_ID", 2));
    fval = 3;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry, "GROUP_MAP", 0, &fval, 1));
    fval = 4;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry, "GROUP_MAP", 1, &fval, 1));
    fval = 5;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry, "GROUP_MAP", 2, &fval, 1));
    fval = 6;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry, "GROUP_MAP", 3, &fval, 1));
    fval = 7;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry, "GROUP_MAP", 4, &fval, 1));
    fval = 8;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry, "GROUP_MAP", 5, &fval, 1));
    fval = 9;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry, "GROUP_MAP", 6, &fval, 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    /*
     * egress flex ctr
     * Create ingress operand profile 1 with absolute value 1 to count number of packets
     */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_EGR_EFLEX_OPERAND_PROFILE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_EGR_EFLEX_OPERAND_PROFILE_ID", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "PROFILE", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "OBJ_1", "USE_1"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_EGR_EFLEX_ACTION_PROFILE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_EGR_EFLEX_ACTION_PROFILE_ID", 10));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ACTION", 10));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "POOL_ID", 0));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "NUM_COUNTERS", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "MODE", "NORMAL"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_A", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_B", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "VAL_A_CTR_EGR_EFLEX_OPERAND_PROFILE_ID", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "CTR_SRC_B", "USE_PKTLEN"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "COUNT_ON_HW_EXCP_DROP", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "COUNT_ON_RULE_DROP", "ALWAYS_COUNT"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_EGR_EFLEX_ACTION_PROFILE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_EGR_EFLEX_ACTION_PROFILE_ID", 11));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ACTION", 11));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "POOL_ID", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "NUM_COUNTERS", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "MODE", "NORMAL"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_A", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_B", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "VAL_A_CTR_EGR_EFLEX_OPERAND_PROFILE_ID", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "CTR_SRC_B", "USE_PKTLEN"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "COUNT_ON_HW_EXCP_DROP", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "COUNT_ON_RULE_DROP", "ALWAYS_COUNT"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_EGR_EFLEX_ACTION_PROFILE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_EGR_EFLEX_ACTION_PROFILE_ID", 12));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ACTION", 12));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "POOL_ID", 2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "NUM_COUNTERS", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "MODE", "NORMAL"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_A", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_B", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "VAL_A_CTR_EGR_EFLEX_OPERAND_PROFILE_ID", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "CTR_SRC_B", "USE_PKTLEN"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "COUNT_ON_HW_EXCP_DROP", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "COUNT_ON_RULE_DROP", "ALWAYS_COUNT"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_EGR_EFLEX_ACTION_PROFILE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_EGR_EFLEX_ACTION_PROFILE_ID", 13));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ACTION", 13));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "POOL_ID", 3));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "NUM_COUNTERS", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "MODE", "NORMAL"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_A", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_B", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "VAL_A_CTR_EGR_EFLEX_OPERAND_PROFILE_ID", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "CTR_SRC_B", "USE_PKTLEN"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "COUNT_ON_HW_EXCP_DROP", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "COUNT_ON_RULE_DROP", "ALWAYS_COUNT"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_EGR_EFLEX_ACTION_PROFILE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_EGR_EFLEX_ACTION_PROFILE_ID", 14));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ACTION", 14));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "POOL_ID", 4));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "NUM_COUNTERS", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "MODE", "NORMAL"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_A", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_B", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "VAL_A_CTR_EGR_EFLEX_OPERAND_PROFILE_ID", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "CTR_SRC_B", "USE_PKTLEN"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "COUNT_ON_HW_EXCP_DROP", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "COUNT_ON_RULE_DROP", "ALWAYS_COUNT"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_EGR_EFLEX_ACTION_PROFILE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_EGR_EFLEX_ACTION_PROFILE_ID", 15));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ACTION", 15));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "POOL_ID", 5));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "NUM_COUNTERS", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "MODE", "NORMAL"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_A", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_B", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "VAL_A_CTR_EGR_EFLEX_OPERAND_PROFILE_ID", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "CTR_SRC_B", "USE_PKTLEN"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "COUNT_ON_HW_EXCP_DROP", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "COUNT_ON_RULE_DROP", "ALWAYS_COUNT"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_EGR_EFLEX_ACTION_PROFILE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_EGR_EFLEX_ACTION_PROFILE_ID", 16));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ACTION", 16));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "POOL_ID", 6));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "NUM_COUNTERS", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "MODE", "NORMAL"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_A", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_B", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "VAL_A_CTR_EGR_EFLEX_OPERAND_PROFILE_ID", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "CTR_SRC_B", "USE_PKTLEN"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "COUNT_ON_HW_EXCP_DROP", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "COUNT_ON_RULE_DROP", "ALWAYS_COUNT"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_EGR_EFLEX_ACTION_PROFILE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_EGR_EFLEX_ACTION_PROFILE_ID", 17));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "ACTION", 17));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "POOL_ID", 7));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "NUM_COUNTERS", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "MODE", "NORMAL"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_A", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "UPDATE_MODE_B", "INC"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "VAL_A_CTR_EGR_EFLEX_OPERAND_PROFILE_ID", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "CTR_SRC_B", "USE_PKTLEN"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "COUNT_ON_HW_EXCP_DROP", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry, "COUNT_ON_RULE_DROP", "ALWAYS_COUNT"));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_EGR_EFLEX_GROUP_ACTION_PROFILE", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_EGR_EFLEX_GROUP_ACTION_PROFILE_ID", 2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_EGR_EFLEX_ACTION_PROFILE_ID", 10));
    fval = 11;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry, "GROUP_MAP", 0, &fval, 1));
    fval = 12;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry, "GROUP_MAP", 1, &fval, 1));
    fval = 13;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry, "GROUP_MAP", 2, &fval, 1));
    fval = 14;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry, "GROUP_MAP", 3, &fval, 1));
    fval = 15;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry, "GROUP_MAP", 4, &fval, 1));
    fval = 16;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry, "GROUP_MAP", 5, &fval, 1));
    fval = 17;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry, "GROUP_MAP", 6, &fval, 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "GROUP", 1));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_EGR_EFLEX_STATS", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_EGR_EFLEX_ACTION_PROFILE_ID", 2));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry, "CTR_EFLEX_INDEX", 0));
    SHR_IF_ERR_EXIT
    (bcmlt_entry_commit(entry, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
    bcmlt_entry_free(entry);

exit:
    if (entry != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry);
    }

    SHR_FUNC_EXIT();

}


static int
trident4_power_test_setup_ipep_wrap(int unit, void *bp)
{
    traffic_max_power_param_t *tra_param = (traffic_max_power_param_t *)bp;
    int vlan_id, port, j;
    shr_mac_t mac_addr;
    shr_mac_t base_mac_addr = TRAFFIC_MAX_POWER_MAC_BASE, base_invert_mac_addr = TRAFFIC_MAX_POWER_INVERT_MAC_BASE;
    shr_mac_t rmac_addr = TRAFFIC_MAX_POWER_RMAC_ADDR;
    shr_ip6_t ip_addr;
    shr_ip6_t base_ip_addr= TRAFFIC_MAX_POWER_IP_ADDR, base_invert_ip_addr = TRAFFIC_MAX_POWER_INVERT_IP_ADDR;
    int intf_num, next_hop_index, ecmp_grp_idx, ecmp_base_ptr, ecmp_grp_cnt;
    int mirror_port[8] = {0}, mirror_enable = 0, imirror = 0, mirror_index[8] = {0};

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(tra_param, SHR_E_PARAM);

    if (tra_param->config_n_send) {
        BCMDRD_PBMP_ITER(tra_param->pbmp_all, port) {
              if (port%2) {
                  for (j = 0; j < SHR_MAC_ADDR_LEN; j++) {
                      mac_addr[j] = base_mac_addr[j];
                  }
                  for (j = 0; j < SHR_IPV6_ADDR_LEN; j++) {
                      ip_addr[j] = base_ip_addr[j];
                  }
              } else {
                  for (j = 0; j < SHR_IPV6_ADDR_LEN; j++) {
                      ip_addr[j] = base_invert_ip_addr[j];
                  }
                  for (j = 0; j < SHR_MAC_ADDR_LEN; j++) {
                      mac_addr[j] = base_invert_mac_addr[j];
                  }
              }
              mac_addr[5] = port;
              ip_addr[15] = port;
              vlan_id = TRAFFIC_MAX_POWER_VLAN_ID_BASE + port;
              intf_num = TRAFFIC_MAX_POWER_INTF_NUM_BASE + port;
              next_hop_index = TRAFFIC_MAX_POWER_NEXT_HOP_BASE + port;
              ecmp_grp_idx = TRAFFIC_MAX_POWER_ECMP_GRP_IDX_BASE + port;
              if ((port > TRAFFIC_MAX_POWER_MAX_ECMP_GRP_NUM) || (tra_param->enable_dlb != TRUE )) {
                  ecmp_base_ptr = TRAFFIC_MAX_POWER_ECMP_GRP_IDX_BASE + TRAFFIC_MAX_POWER_ECMP_GRP_CNT * port;
              } else {
                  ecmp_base_ptr = TRAFFIC_MAX_POWER_ECMP_GRP_CNT * port;
              }
              ecmp_grp_cnt = TRAFFIC_MAX_POWER_ECMP_GRP_CNT - 1;


            SHR_IF_ERR_EXIT
                (bcm56880_a0_l3_ipv6_unicast_ecmp_add(unit, port, mac_addr,
                                        rmac_addr, vlan_id, next_hop_index,
                                        ip_addr, intf_num, ecmp_grp_idx,
                                        ecmp_base_ptr, ecmp_grp_cnt, tra_param->enable_defip));

            for (j = 0; j < TRAFFIC_MAX_POWER_ECMP_GRP_CNT; j++) {

                SHR_IF_ERR_EXIT
                    (bcm56880_a0_ecmp_member_add(unit, ecmp_grp_idx , port,
                                                vlan_id, next_hop_index, intf_num,
                                                0 , mac_addr, rmac_addr));
            }

        }

        j = 0;
        if (BCMDRD_PBMP_NOT_NULL(tra_param->mirror_pbmp)) {
            BCMDRD_PBMP_ITER(tra_param->mirror_pbmp, port) {
                if (j >= 8) {
                    break;
                }
                mirror_index[j] = j;
                mirror_port[j] = port;
                mirror_enable |= 1 << j;
                imirror = 0;
                j++;
            }
            if (mirror_enable != 0) {
                SHR_IF_ERR_EXIT
                    (bcm56880_a0_mirror_add(unit, mirror_port, mirror_enable, imirror,
                                                    mirror_index));
            }
        }

        if (tra_param->enable_dlb == TRUE) {
            SHR_IF_ERR_EXIT
                (bcm56880_a0_dlb_ecmp_add(unit));
        }

        if (tra_param->enable_ifp) {
            SHR_IF_ERR_EXIT
                (bcm56880_a0_ifp_add(unit));
        }

        if (tra_param->enable_efp) {
            SHR_IF_ERR_EXIT
                (bcm56880_a0_efp_add(unit));
        }

        if (tra_param->enable_flex_ctr) {
            SHR_IF_ERR_EXIT
                (bcm56880_a0_flex_ctr_add(unit));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static void
databuf_print(int unit, const bcmpkt_data_buf_t *buf, shr_pb_t *pb)
{
    uint8_t *pbuf;
    uint32_t idx;

    pbuf = buf->data;
    shr_pb_printf(pb,"packet content:\n");
    for (idx = 0; idx < buf->data_len; idx++) {
        shr_pb_printf(pb,"%02x ", pbuf[idx]);
    }
    shr_pb_printf(pb, "\n----------------------------------------------------------------------------------\n");
    shr_pb_printf(pb,"\n");
}

int
bcm56880_a0_power_txrx(int unit, void *bp)
{
    traffic_max_power_param_t *tra_param = (traffic_max_power_param_t *)bp;
    bcmdrd_dev_type_t pkt_dev_type;
    bcmpkt_packet_t **packet_list[BCMDRD_CONFIG_MAX_PORTS] = {NULL};
    uint64_t *expeceted_rate = NULL;
    bool counter_checker_create = false, counter_checker_start = false;
    bool rxdata_checker_create = false, rxdata_checker_start = false;
    bool test_result = false, first_polling = true;
    int netif_id, pktdev_id, port, pkt_cnt_all;
    bcmdrd_pbmp_t pbmp_temp;
    uint32_t curtime, idx, j;
    int vlan_id, pkt_len = 0, wait_time;
    shr_mac_t mac_addr;
    shr_mac_t base_mac_addr = TRAFFIC_MAX_POWER_MAC_BASE, base_invert_mac_addr = TRAFFIC_MAX_POWER_INVERT_MAC_BASE;
    shr_mac_t rmac_addr = TRAFFIC_MAX_POWER_RMAC_ADDR;
    shr_ip6_t ip_addr, src_ip_addr;
    shr_ip6_t base_ip_addr= TRAFFIC_MAX_POWER_IP_ADDR, base_invert_ip_addr = TRAFFIC_MAX_POWER_INVERT_IP_ADDR;
    shr_pb_t *pb;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(tra_param, SHR_E_PARAM);

    /* setup packet device */
    pkt_cnt_all = 0;
    BCMDRD_PBMP_ITER(tra_param->pbmp_all, port) {
        pkt_cnt_all += tra_param->pkt_cnt[port];
    }

    if (tra_param->need_check_rx == TRUE) {
        /* coverity[dont_call] */
        SHR_IF_ERR_EXIT
            (bcma_testutil_packet_dev_init
                (unit, TRAFFIC_MAX_POWER_PKTDEV_CHAN_TX, TRAFFIC_MAX_POWER_PKTDEV_CHAN_RX,
                TRAFFIC_MAX_POWER_PKT_SIZE_MAX,
                (tra_param->need_check_rx == TRUE) ? pkt_cnt_all * 2 : pkt_cnt_all,
                &netif_id, &pktdev_id));
    }

    /* allocate packet buffer */
    BCMDRD_PBMP_ITER(tra_param->pbmp_all, port) {
        uint32_t pkt_cnt = tra_param->pkt_cnt[port];
        SHR_ALLOC(packet_list[port], pkt_cnt * sizeof(bcmpkt_packet_t *),
                  "bcmaTestCaseTrafficStreamPkt");
        SHR_NULL_CHECK(packet_list[port], SHR_E_MEMORY);
        sal_memset(packet_list[port], 0, pkt_cnt * sizeof(bcmpkt_packet_t *));
        pkt_len = tra_param->pkt_len[port][0];
    }

    /* determine target speed : minimun port speed * 95% */
    SHR_ALLOC(expeceted_rate, BCMDRD_CONFIG_MAX_PORTS * sizeof(uint64_t),
              "bcmaTestCaseTrafficStreamExpRate");
    SHR_NULL_CHECK(expeceted_rate, SHR_E_MEMORY);
    sal_memset(expeceted_rate, 0, BCMDRD_CONFIG_MAX_PORTS * sizeof(uint64_t));
    BCMDRD_PBMP_ASSIGN(pbmp_temp, tra_param->pbmp_all);
    BCMDRD_PBMP_OR(pbmp_temp, tra_param->mirror_pbmp);
    SHR_IF_ERR_EXIT
        (bcma_testutil_drv_traffic_expeceted_rate_get
            (unit, pbmp_temp, tra_param->pbmp_oversub,
             pkt_len + 4, BCMDRD_CONFIG_MAX_PORTS, expeceted_rate));

    BCMDRD_PBMP_ITER(pbmp_temp, port) {
        expeceted_rate[port] =
            (expeceted_rate[port] *
             (100 - TRAFFIC_MAX_POWER_DEFAULT_CHECK_SPEED_MARGIN)) / 100;
    }

    /* print information */
    BCMDRD_PBMP_ITER(tra_param->pbmp_all, port) {
        cli_out("port %d target speed : ", port);
        bcma_testutil_stat_show(expeceted_rate[port], 1, true);
        cli_out("b/s ,");
        cli_out("inject %d packets.\n  (size :", tra_param->pkt_cnt[port]);
        for (idx = 0; idx < tra_param->pkt_cnt[port]; idx++) {
            cli_out(" %d", tra_param->pkt_len[port][idx]);
        }
        cli_out(")\n");
    }

    /* setup counter checker */
    SHR_IF_ERR_EXIT
        (bcma_testutil_traffic_counter_checker_create(unit));

    counter_checker_create = true;

    BCMDRD_PBMP_ITER(tra_param->pbmp_all, port) {

        char show_msg[TRAFFIC_COUNTER_CHECK_SHOWMSG_MAXLEN];

        sal_snprintf(show_msg, TRAFFIC_COUNTER_CHECK_SHOWMSG_MAXLEN,
                     "Port %3"PRId32" :", port);

        SHR_IF_ERR_EXIT
            (bcma_testutil_traffic_counter_checker_add
                (unit, port, true, true, expeceted_rate[port], show_msg));
    }

    /* for mirror port only check tx */
    BCMDRD_PBMP_ITER(tra_param->mirror_pbmp, port) {

        char show_msg[TRAFFIC_COUNTER_CHECK_SHOWMSG_MAXLEN];

        sal_snprintf(show_msg, TRAFFIC_COUNTER_CHECK_SHOWMSG_MAXLEN,
                     "Port %3"PRId32" :", port);

        SHR_IF_ERR_EXIT
            (bcma_testutil_traffic_counter_checker_add
                (unit, port, false, true, expeceted_rate[port], show_msg));
    }

   /* inject packet, and setup rxdata checker if need */
   SHR_IF_ERR_EXIT
        (bcmpkt_dev_type_get(unit, &pkt_dev_type));

    if (tra_param->need_check_rx == TRUE) {
        SHR_IF_ERR_EXIT
            (bcma_testutil_traffic_rxdata_checker_create
                (unit, netif_id, pktdev_id, TRAFFIC_RXDATA_CHECKER_MODE_PORT));
        rxdata_checker_create = true;
    }

    bcma_testutil_packet_init();
    BCMDRD_PBMP_ITER(tra_param->pbmp_all, port) {
        if (tra_param->need_check_rx == FALSE) {
            SHR_IF_ERR_EXIT
                (bcma_testutil_packet_dev_init
                    (unit, TRAFFIC_MAX_POWER_PKTDEV_CHAN_TX, TRAFFIC_MAX_POWER_PKTDEV_CHAN_RX,
                    TRAFFIC_MAX_POWER_PKT_SIZE_MAX,
                    tra_param->pkt_cnt[port] * 2,
                    &netif_id, &pktdev_id));
        }

        for (idx = 0; idx < tra_param->pkt_cnt[port]; idx++) {
           SHR_IF_ERR_EXIT
               (bcmpkt_alloc(pktdev_id, tra_param->pkt_len[port][idx],
                             BCMPKT_BUF_F_TX, &(packet_list[port][idx])));
        }
        for (idx = 0; idx < tra_param->pkt_cnt[port]; idx++) {
            bcmpkt_packet_t *packet = packet_list[port][idx];
            if (port%2) {
                  for (j = 0; j < SHR_MAC_ADDR_LEN; j++) {
                      mac_addr[j] = base_mac_addr[j];
                  }
                  for (j = 0; j < SHR_IPV6_ADDR_LEN; j++) {
                      ip_addr[j] = base_ip_addr[j];
                      src_ip_addr[j] = base_ip_addr[j];
                  }
            } else {
                for (j = 0; j < SHR_MAC_ADDR_LEN; j++) {
                      mac_addr[j] = base_invert_mac_addr[j];
                  }
                for (j = 0; j < SHR_IPV6_ADDR_LEN; j++) {
                      ip_addr[j] = base_invert_ip_addr[j];
                      src_ip_addr[j] = base_invert_ip_addr[j];
                }
            }

            mac_addr[5] = port;
            ip_addr [15] = port;
            src_ip_addr [15] = port-1;
            vlan_id = TRAFFIC_MAX_POWER_VLAN_ID_BASE + port;

            SHR_IF_ERR_EXIT
                (bcma_testutil_l3_packet_fill
                    (packet->data_buf, tra_param->pkt_len[port][idx],
                     rmac_addr, mac_addr, src_ip_addr, ip_addr, vlan_id,
                     255, 0,
                     TRAFFIC_MAX_POWER_DEFAULT_PACKET_PATTERN,
                     TRAFFIC_MAX_POWER_DEFAULT_PACKET_PATTERN_INC, TRUE, seed, 1));

            if (tra_param->need_check_rx == TRUE) {
                pb = shr_pb_create();
                databuf_print(unit, packet->data_buf, pb);
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "%s\n"), shr_pb_str(pb)));
                shr_pb_destroy(pb);
                SHR_IF_ERR_EXIT
                    (bcma_testutil_traffic_rxdata_checker_add
                        (unit, port, packet->data_buf));
            }

            SHR_IF_ERR_EXIT
                (bcmpkt_fwd_port_set(pkt_dev_type, port, packet));

            if (tra_param->config_n_send) {
                SHR_IF_ERR_EXIT
                     (bcmpkt_tx(unit, TRAFFIC_MAX_POWER_DEFAULT_NETIF_ID, packet));
            }
        }

        if (packet_list[port] != NULL) {
            for (idx = 0; idx < tra_param->pkt_cnt[port]; idx++) {
                if (packet_list[port][idx] != NULL) {
                    bcmpkt_free(pktdev_id, packet_list[port][idx]);
                    packet_list[port][idx] = NULL;
                }
            }
            SHR_FREE(packet_list[port]);
        }

    }

    sal_sleep(tra_param->interval);
    LOG_VERBOSE(BSL_LOG_MODULE,
             (BSL_META_U(unit, "traffic run for %0d second\n"),
              tra_param->interval));
    if (tra_param->check_n_stop) {
        /* counter polling */
        SHR_IF_ERR_EXIT
            (bcma_testutil_traffic_counter_checker_start(unit));
        counter_checker_start = true;

        curtime = 0;
        first_polling = true;
        while (curtime < tra_param->runtime) {

            sal_sleep(tra_param->interval);

            curtime += tra_param->interval;
            if (curtime > tra_param->runtime) {
                curtime = tra_param->runtime;
            }
            cli_out("TRAFFIC: running... (%"PRId32"%% complete)\n",
                    (curtime * 100) / tra_param->runtime);

            if (tra_param->need_check_speed == TRUE && first_polling == false) {
                SHR_IF_ERR_EXIT
                    (bcma_testutil_traffic_counter_checker_execute
                        (unit, true, &test_result));
            } else {
                SHR_IF_ERR_EXIT
                    (bcma_testutil_traffic_counter_checker_execute
                        (unit, false, &test_result));
            }

            if (test_result == false && tra_param->run_2_end == FALSE) {
                /* return immediately if user dont want to run to end */
                SHR_ERR_EXIT(SHR_E_FAIL);
            }

            first_polling = false;
        }

        if (test_result == false) {
            SHR_ERR_EXIT(SHR_E_FAIL);
        }

        /* redirect all packet back to CPU and check */
        if (tra_param->need_check_rx == TRUE) {

            SHR_IF_ERR_EXIT
                (bcma_testutil_traffic_rxdata_checker_start(unit));

            rxdata_checker_start = true;
        }
        LOG_VERBOSE(BSL_LOG_MODULE,
             (BSL_META("drop all the packets and copy to cpu\n")));
        /* drop all the packets and copy to cpu */
        SHR_IF_ERR_EXIT
           (bcm56880_a0_update_pkt_integrity_check(unit, true));

        if (tra_param->need_check_rx == TRUE) {
            /* check */
            wait_time = TRAFFIC_MAX_POWER_DEFAULT_RX_CHECK_WAIT;
            LOG_VERBOSE(BSL_LOG_MODULE,
               (BSL_META_U(unit, "wait time is %0dms\n"), wait_time/1000));
            SHR_IF_ERR_EXIT
                (bcma_testutil_traffic_rxdata_checker_execute
                    (unit, wait_time, &test_result));

            if (test_result == false) {
                SHR_ERR_EXIT(SHR_E_FAIL);
            }
        }
    }

exit:
    BCMDRD_PBMP_ITER(tra_param->pbmp_all, port) {
        if (packet_list[port] != NULL) {
            for (idx = 0; idx < tra_param->pkt_cnt[port]; idx++) {
                if (packet_list[port][idx] != NULL) {
                    bcmpkt_free(pktdev_id, packet_list[port][idx]);
                    packet_list[port][idx] = NULL;
                }
            }
            SHR_FREE(packet_list[port]);
        }
    }
    if (expeceted_rate != NULL) {
        SHR_FREE(expeceted_rate);
    }
    if (counter_checker_start == true) {
        bcma_testutil_traffic_counter_checker_stop(unit);
    }
    if (counter_checker_create == true) {
        bcma_testutil_traffic_counter_checker_destroy(unit);
    }
    if (rxdata_checker_start == true) {
        bcma_testutil_traffic_rxdata_checker_stop(unit);
    }
    if (rxdata_checker_create == true) {
        bcma_testutil_traffic_rxdata_checker_destroy(unit);
    }
    (void)bcma_testutil_packet_dev_cleanup(unit);

    SHR_FUNC_EXIT();
}

int
bcm56880_a0_power_init(int unit, void *bp)
{
    traffic_max_power_param_t *tra_param = (traffic_max_power_param_t *)bp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(tra_param, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "bcm56880_a0_power_init() START \n")));

    sal_srand(0x12347);

exit:
    SHR_FUNC_EXIT();
}


#define BCM56880_A0_TRAFFIC_MAX_POWER_PARAM_HELP \
    "  EnableBmp        - Power test flags\n"


int
bcm56880_a0_power_help(int unit)
{
    cli_out("%s", BCM56880_A0_TRAFFIC_MAX_POWER_PARAM_HELP);
    return SHR_E_NONE;
}

int
bcm56880_a0_power_port_set(int unit, void *bp)
{
    traffic_max_power_param_t *tra_param = (traffic_max_power_param_t *)bp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(tra_param, SHR_E_PARAM);

    /* Place holder for now */
    cli_out("bcm56880_a0_power_port_set CALLED\n");

exit:
    SHR_FUNC_EXIT();
}

int
bcm56880_a0_power_config_set(int unit, void *bp)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(trident4_power_test_setup_ipep_wrap(unit, bp));

exit:
    SHR_FUNC_EXIT();
}


