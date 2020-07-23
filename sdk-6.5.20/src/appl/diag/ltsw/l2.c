/*! \file l2.c
 *
 * LTSW L2 command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#if defined(BCM_LTSW_SUPPORT)

#include <appl/diag/system.h>
#include <appl/diag/ltsw/cmdlist.h>
#include <shared/bsl.h>
#include <sal/sal_types.h>

#include <bcm/l2.h>
#include <bcm/init.h>

#include <bcm_int/ltsw/l2.h>

/******************************************************************************
 * Private functions
 */

/*!
 * \brief L2 add cmd handler.
 *
 * \param [in] unit           Unit number
 * \param [in] arg            Arg structure for parsing arguments.
 *
 * \retval CMD_OK             Command completed successfully.
 * \retval CMD_FAIL           Command failed.
 */
STATIC cmd_result_t
cmd_ltsw_l2_add(int unit, args_t *arg)
{
    parse_table_t pt;
    int arg_modid = 0, arg_port = 0, arg_vlan = 0, arg_trunk = 0, arg_tgid = 0,
        arg_static = 0, arg_hit = 0, arg_scp = 0, arg_ds = 0, arg_dd = 0,
        arg_cbit = 0, arg_group = 0;
    uint32_t flags = 0;
    sal_mac_addr_t arg_macaddr = {0, 0, 0, 0, 0, 0X1};
    bcm_l2_addr_t l2addr;
    int rv;

    if (ARG_CNT(arg) == 0) {
        return CMD_USAGE;
    }

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Module", PQ_DFL|PQ_INT, 0, &arg_modid, NULL);
    parse_table_add(&pt, "Port", PQ_DFL|PQ_PORT, 0, &arg_port, NULL);
    parse_table_add(&pt, "MACaddress",PQ_DFL|PQ_MAC,0,&arg_macaddr, NULL);
    parse_table_add(&pt, "Vlanid", PQ_DFL|PQ_HEX, 0, &arg_vlan, NULL);
    parse_table_add(&pt, "Trunk", PQ_DFL|PQ_BOOL, 0, &arg_trunk, NULL);
    parse_table_add(&pt, "TrunkGroupID", PQ_DFL|PQ_INT, 0, &arg_tgid, NULL);
    parse_table_add(&pt, "STatic", PQ_DFL|PQ_BOOL, 0, &arg_static, NULL);
    parse_table_add(&pt, "HIT", PQ_DFL|PQ_BOOL, 0, &arg_hit, NULL);
    parse_table_add(&pt, "SourceCosPriority", PQ_DFL|PQ_BOOL, 0, &arg_scp, NULL);
    parse_table_add(&pt, "DiscardSource", PQ_DFL|PQ_BOOL, 0, &arg_ds, NULL);
    parse_table_add(&pt, "DiscardDest",PQ_DFL|PQ_BOOL, 0, &arg_dd, NULL);
    parse_table_add(&pt, "CPUmirror", PQ_DFL|PQ_BOOL, 0, &arg_cbit, NULL);
    parse_table_add(&pt, "Group", PQ_DFL|PQ_INT, 0, &arg_group, NULL);

    if (parse_arg_eq(arg, &pt) < 0 || ARG_CNT(arg) > 0) {
        cli_out("%s: Invalid option %s\n", ARG_CMD(arg), ARG_CUR(arg));
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }

    parse_arg_eq_done(&pt);
    /* Configure flags for SDK call */
    flags = 0;
    if (arg_static) {
        flags |= BCM_L2_STATIC;
    }
    if (arg_hit) {
        flags |= BCM_L2_HIT;
    }
    if (arg_scp) {
        flags |= BCM_L2_COS_SRC_PRI;
    }
    if (arg_dd) {
        flags |= BCM_L2_DISCARD_DST;
    }
    if (arg_ds) {
        flags |= BCM_L2_DISCARD_SRC;
    }
    if (arg_trunk) {
        flags |= BCM_L2_TRUNK_MEMBER;
    }
    if (arg_cbit) {
        flags |= BCM_L2_COPY_TO_CPU;
    }
    /* Fill l2addr structure */
    bcm_l2_addr_t_init(&l2addr, arg_macaddr, arg_vlan);
    l2addr.tgid = arg_tgid;
    l2addr.group = arg_group;
    l2addr.flags = flags;
    l2addr.port = arg_port;
    l2addr.modid = arg_modid;

    rv = bcm_l2_addr_add(unit, &l2addr);
    if (rv != BCM_E_NONE) {
        cli_out("%s ERROR: %s\n", ARG_CMD(arg), bcm_errmsg(rv));
        return CMD_FAIL;
    }
    dump_l2_addr(unit, "ADD: ", &l2addr);

    return CMD_OK;
}

/*!
 * \brief L2 delete cmd handler.
 *
 * \param [in] unit           Unit number
 * \param [in] arg            Arg structure for parsing arguments.
 *
 * \retval CMD_OK             Command completed successfully.
 * \retval CMD_FAIL           Command failed.
 */
STATIC cmd_result_t
cmd_ltsw_l2_delete(int unit, args_t *arg)
{
    parse_table_t pt;
    int arg_vlan = 0;
    sal_mac_addr_t arg_macaddr = {0, 0, 0, 0, 0, 0X1};
    bcm_l2_addr_t l2addr;
    int rv;

    if (ARG_CNT(arg) == 0) {
        return CMD_USAGE;
    }

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "MACaddress",PQ_DFL|PQ_MAC,0,&arg_macaddr, NULL);
    parse_table_add(&pt, "Vlanid", PQ_DFL|PQ_HEX, 0, &arg_vlan, NULL);

    if (parse_arg_eq(arg, &pt) < 0 || ARG_CNT(arg) > 0) {
        cli_out("%s: Invalid option %s\n", ARG_CMD(arg), ARG_CUR(arg));
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }

    parse_arg_eq_done(&pt);
    /* Fill l2addr structure */
    bcm_l2_addr_t_init(&l2addr, arg_macaddr, arg_vlan);

    rv = bcm_l2_addr_delete(unit, arg_macaddr, arg_vlan);
    if (rv != BCM_E_NONE) {
        cli_out("%s ERROR: %s\n", ARG_CMD(arg), bcm_errmsg(rv));
        return CMD_FAIL;
    }

    return CMD_OK;
}

/*!
 * \brief L2 get cmd handler.
 *
 * \param [in] unit           Unit number
 * \param [in] arg            Arg structure for parsing arguments.
 *
 * \retval CMD_OK             Command completed successfully.
 * \retval CMD_FAIL           Command failed.
 */
STATIC cmd_result_t
cmd_ltsw_l2_get(int unit, args_t *arg)
{
    parse_table_t pt;
    int arg_vlan = 0;
    sal_mac_addr_t arg_macaddr = {0, 0, 0, 0, 0, 0X1};
    bcm_l2_addr_t l2addr;
    int rv;

    if (ARG_CNT(arg) == 0) {
        return CMD_USAGE;
    }

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "MACaddress",PQ_DFL|PQ_MAC,0,&arg_macaddr, NULL);
    parse_table_add(&pt, "Vlanid", PQ_DFL|PQ_HEX, 0, &arg_vlan, NULL);

    if (parse_arg_eq(arg, &pt) < 0 || ARG_CNT(arg) > 0) {
        cli_out("%s: Invalid option %s\n", ARG_CMD(arg), ARG_CUR(arg));
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }

    parse_arg_eq_done(&pt);
    /* Fill l2addr structure */
    bcm_l2_addr_t_init(&l2addr, arg_macaddr, arg_vlan);

    rv = bcm_l2_addr_get(unit, arg_macaddr, arg_vlan, &l2addr);
    if (rv != BCM_E_NONE) {
        cli_out("%s ERROR: %s\n", ARG_CMD(arg), bcm_errmsg(rv));
        return CMD_FAIL;
    }
    dump_l2_addr(unit, "GET: ", &l2addr);

    return CMD_OK;
}

/*!
 * \brief Callback function to print l2 entry.
 *
 * \param [in] unit           Unit number
 * \param [in] l2addr         L2 address structure.
 * \param [in] user_data      User specified cookie.
 *
 * \retval CMD_OK             Command completed successfully.
 */
static int
ltsw_l2addr_dump(int unit, bcm_l2_addr_t *l2addr, void *user_data)
{
    dump_l2_addr(unit, "", l2addr);
    return CMD_OK;
}

/*!
 * \brief L2 show cmd handler.
 *
 * \param [in] unit           Unit number
 * \param [in] arg            Arg structure for parsing arguments.
 *
 * \retval CMD_OK             Command completed successfully.
 * \retval CMD_FAIL           Command failed.
 */
STATIC cmd_result_t
cmd_ltsw_l2_show(int unit, args_t *arg)
{
    int rv;

    if (ARG_CNT(arg) > 0) {
        return CMD_USAGE;
    }

    /* Fill l2addr structure */
    rv = bcm_l2_traverse(unit, ltsw_l2addr_dump, NULL);
    if (rv < 0) {
        cli_out("%s: ERROR: bcm_l2_traverse failed %s\n",
                ARG_CMD(arg), bcm_errmsg(rv));
    }

    return CMD_OK;
}

/*!
 * \brief Delete all L2 entries for given mac/vlan/port/trunk, etc.
 *
 * \param [in] unit           Unit number
 * \param [in] arg            Arg structure for parsing arguments.
 *
 * \retval CMD_OK             Command completed successfully.
 * \retval CMD_FAIL           Command failed.
 */
STATIC cmd_result_t
cmd_ltsw_l2_clear(int unit, args_t *arg)
{
    int rv;
    parse_table_t pt;
    int arg_modid = -1, arg_port = -1, arg_vlan = -1, arg_tgid = -1,
        arg_static = TRUE;
    uint32_t flags = 0;
    bcm_mac_t arg_macaddr_all_zeroes = {0, 0, 0, 0, 0, 0}, arg_macaddr;
    char str[30];
    enum {
        MAC_PRESENT         = 0x00000001,
        MODID_PRESENT       = 0x00000002,
        PORT_PRESENT        = 0x00000004,
        VLANID_PRESENT      = 0x00000008,
        TGID_PRESENT        = 0x00000010
    } arg_combination;

    if (ARG_CNT(arg) == 0) {
        return CMD_USAGE;
    }

    sal_memcpy(arg_macaddr, arg_macaddr_all_zeroes, sizeof(bcm_mac_t));
    arg_combination = 0;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Module", PQ_DFL|PQ_INT, 0, &arg_modid, NULL);
    parse_table_add(&pt, "Port", PQ_DFL|PQ_PORT, 0, &arg_port, NULL);
    parse_table_add(&pt, "MACaddress",PQ_DFL|PQ_MAC,0,&arg_macaddr, NULL);
    parse_table_add(&pt, "Vlanid", PQ_DFL|PQ_HEX, 0, &arg_vlan, NULL);
    parse_table_add(&pt, "TrunkGroupID", PQ_DFL|PQ_INT, 0, &arg_tgid, NULL);
    parse_table_add(&pt, "STatic", PQ_DFL|PQ_BOOL, 0, &arg_static, NULL);

    if (parse_arg_eq(arg, &pt) < 0 || ARG_CNT(arg) > 0) {
        cli_out("%s: Invalid option %s\n", ARG_CMD(arg), ARG_CUR(arg));
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }

    parse_arg_eq_done(&pt);
    /* Configure flags for SDK call */
    flags = 0;
    if (arg_static) {
        flags |= BCM_L2_DELETE_STATIC;
    }
    if (arg_modid >= 0) {
        arg_combination |= MODID_PRESENT;
    }
    if (arg_port >= 0) {
        arg_combination |= PORT_PRESENT;
    }
    if (arg_tgid >= 0) {
        arg_combination |= TGID_PRESENT;
    }
    if (arg_vlan >= 0) {
        arg_combination |= VLANID_PRESENT;
    }
    if (sal_memcmp(arg_macaddr, arg_macaddr_all_zeroes, sizeof(bcm_mac_t))) {
        arg_combination |= MAC_PRESENT;
    }

    switch ((int)arg_combination) {
        case PORT_PRESENT:
            sal_sprintf(str, "port, local module ID");
            rv = bcm_l2_addr_delete_by_port(unit, -1, arg_port, arg_static);
            break;

        case PORT_PRESENT | MODID_PRESENT:
            sal_sprintf(str, "module/port");
            rv = bcm_l2_addr_delete_by_port(unit, arg_modid, arg_port,
                                            arg_static);
            break;
        case VLANID_PRESENT:
            sal_sprintf(str, "VLAN");
            rv = bcm_l2_addr_delete_by_vlan(unit, arg_vlan, arg_static);
            break;
        case TGID_PRESENT:
            sal_sprintf(str, "trunk ID");
            rv = bcm_l2_addr_delete_by_trunk(unit, arg_tgid, arg_static);
            break;
        case MAC_PRESENT:
            sal_sprintf(str, "MAC");
            rv = bcm_l2_addr_delete_by_mac(unit, arg_macaddr, arg_static);
            break;
        case MAC_PRESENT | VLANID_PRESENT:
            sal_sprintf(str, "MAC and VLAN");
            rv = bcm_l2_addr_delete(unit, arg_macaddr, arg_vlan);
            break;
        case MAC_PRESENT | PORT_PRESENT:
            sal_sprintf(str, "MAC and port");
            rv = bcm_l2_addr_delete_by_mac_port(unit, arg_macaddr, -1, arg_port,
                                                arg_static);
            break;
        case MAC_PRESENT | PORT_PRESENT | MODID_PRESENT:
            sal_sprintf(str, "MAC and module/port");
            rv = bcm_l2_addr_delete_by_mac_port(unit, arg_macaddr, arg_modid,
                                                arg_port, arg_static);
            break;
        case VLANID_PRESENT | PORT_PRESENT:
            sal_sprintf(str, "VLAN and port");
            rv = bcm_l2_addr_delete_by_vlan_port(unit, arg_vlan, -1, arg_port,
                                                 arg_static);
            break;
        case VLANID_PRESENT | PORT_PRESENT | MODID_PRESENT:
            sal_sprintf(str, "VLAN and module/port");
            rv = bcm_l2_addr_delete_by_vlan_port(unit, arg_vlan, arg_modid,
                                                 arg_port, arg_static);
            break;
        case VLANID_PRESENT | TGID_PRESENT:
            sal_sprintf(str, "VLAN and trunk ID");
            rv = bcm_l2_addr_delete_by_vlan_trunk(unit, arg_vlan, arg_tgid,
                                                  arg_static);
            break;
        default:
            cli_out("%s: Unknown argument combination\n", ARG_CMD(arg));
            rv = BCM_E_FAIL;
            break;
    }

    if (BCM_FAILURE(rv)) {
        cli_out("%s ERROR: %s\n", ARG_CMD(arg), bcm_errmsg(rv));
        return CMD_FAIL;
    }

    cli_out("%s: Deleting %s addresses by %s\n", ARG_CMD(arg),
            arg_static ? "static and non-static" : "non-static", str);

    return CMD_OK;
}

/*!
 * \brief L2 replace cmd handler.
 *
 * \param [in] unit           Unit number
 * \param [in] arg            Arg structure for parsing arguments.
 *
 * \retval CMD_OK             Command completed successfully.
 * \retval CMD_FAIL           Command failed.
 */
STATIC cmd_result_t
cmd_ltsw_l2_replace(int unit, args_t *arg)
{
    parse_table_t pt;
    int arg_modid = -1, arg_port = -1, arg_vlan = -1, arg_tgid = -1,
        arg_newmodid = -1, arg_newport = -1, arg_newtgid = -1, arg_trunk = 0,
        arg_static = 0;
    uint32_t flags = 0;
    bcm_mac_t default_macaddr = {0, 0, 0, 0, 0, 0X1};
    bcm_mac_t arg_macaddr = {0, 0, 0, 0, 0, 0X1};
    bcm_l2_addr_t l2addr;
    int rv;

    if (ARG_CNT(arg) == 0) {
        return CMD_USAGE;
    }

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Module", PQ_DFL|PQ_INT, 0, &arg_modid, NULL);
    parse_table_add(&pt, "Port", PQ_DFL|PQ_PORT, 0, &arg_port, NULL);
    parse_table_add(&pt, "MACaddress",PQ_DFL|PQ_MAC,0,&arg_macaddr, NULL);
    parse_table_add(&pt, "Vlanid", PQ_DFL|PQ_HEX, 0, &arg_vlan, NULL);
    parse_table_add(&pt, "Trunk", PQ_DFL|PQ_BOOL, 0, &arg_trunk, NULL);
    parse_table_add(&pt, "TrunkGroupID", PQ_DFL|PQ_INT, 0, &arg_tgid, NULL);
    parse_table_add(&pt, "STatic", PQ_DFL|PQ_BOOL, 0, &arg_static, NULL);
    parse_table_add(&pt, "NewModule", PQ_DFL|PQ_INT, 0, &arg_newmodid, NULL);
    parse_table_add(&pt, "NewPort", PQ_DFL|PQ_INT, 0, &arg_newport, NULL);
    parse_table_add(&pt, "NewTrunkGroupID", PQ_DFL|PQ_INT, 0, &arg_newtgid,
                    NULL);

    if (parse_arg_eq(arg, &pt) < 0 || ARG_CNT(arg) > 0) {
        cli_out("%s: Invalid option %s\n", ARG_CMD(arg), ARG_CUR(arg));
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }

    parse_arg_eq_done(&pt);
    /* Configure flags for SDK call */
    flags = 0;
    /* Fill l2addr structure */
    bcm_l2_addr_t_init(&l2addr, arg_macaddr, arg_vlan);
    if (arg_static) {
        flags |= BCM_L2_REPLACE_MATCH_STATIC;
    }
    if (arg_vlan != -1) {
        flags |= BCM_L2_REPLACE_MATCH_VLAN;
    }
    if (arg_newtgid != - 1) {
        flags |= BCM_L2_REPLACE_NEW_TRUNK;
    }
    if (sal_memcmp(arg_macaddr, default_macaddr, sizeof(bcm_mac_t))) {
        flags |= BCM_L2_REPLACE_MATCH_MAC;
    }
    if (BCM_GPORT_IS_SET(arg_port)) {
        flags |= BCM_L2_REPLACE_MATCH_DEST;
        l2addr.port = arg_port;
    } else {
        if ((arg_port != - 1) && (arg_modid != - 1)) {
            flags |= BCM_L2_REPLACE_MATCH_DEST;
            l2addr.modid = arg_modid;
            l2addr.port = arg_port;
        }
        if (arg_trunk && (arg_tgid != -1)) {
            flags |= BCM_L2_REPLACE_MATCH_DEST;
            l2addr.tgid = arg_tgid;
        }
    }

    rv = bcm_l2_replace(unit, flags, &l2addr, arg_newmodid,
                        arg_newport, arg_newtgid);
    if (BCM_FAILURE(rv)) {
        cli_out("%s ERROR: %s\n", ARG_CMD(arg), bcm_errmsg(rv));
        return CMD_FAIL;
    }

    return CMD_OK;
}

/*!
 * \brief Set L2 age timer.
 *
 * \param [in] unit           Unit number
 * \param [in] arg            Arg structure for parsing arguments.
 *
 * \retval CMD_OK             Command completed successfully.
 * \retval CMD_FAIL           Command failed.
 */
STATIC cmd_result_t
cmd_ltsw_l2_age(int unit, args_t *arg)
{
    parse_table_t pt;
    int timer = 0;
    int rv;

    /* Display settings */
    if (ARG_CNT(arg) == 0) {
        rv = bcm_l2_age_timer_get(unit, &timer);
        if (BCM_FAILURE(rv)) {
            cli_out("%s ERROR: could not get timer: %s\n",
                    ARG_CMD(arg), bcm_errmsg(rv));
            return CMD_FAIL;
        }

        cli_out("Current L2 age timer is %d\n", timer);
        return CMD_OK;
    } else if (sal_strcasecmp(_ARG_CUR(arg), "on") == 0) {
        (void)ARG_GET(arg);
        rv = bcmi_ltsw_l2_age_resume(unit);
        if (BCM_FAILURE(rv)) {
            cli_out("%s ERROR: Resume L2 age failed: %s\n",
                    ARG_CMD(arg), bcm_errmsg(rv));
            return CMD_FAIL;
        }
        return CMD_OK;
    } else if (sal_strcasecmp(_ARG_CUR(arg), "off") == 0) {
        (void)ARG_GET(arg);
        rv = bcmi_ltsw_l2_age_stop(unit);
        if (rv == BCM_E_INIT) {
            /*
             * Workaround that allows to turn off L2 age
             * at the beginning of ltsw.soc
             */
            return CMD_OK;
        } else if (BCM_FAILURE(rv)) {
            cli_out("%s ERROR: Stop L2 age failed: %s\n",
                    ARG_CMD(arg), bcm_errmsg(rv));
            return CMD_FAIL;
        }
        return CMD_OK;
    }

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Timer", PQ_DFL|PQ_HEX, 0, &timer, NULL);

    if (parse_arg_eq(arg, &pt) < 0 || ARG_CNT(arg) > 0) {
        cli_out("%s: Invalid option %s\n", ARG_CMD(arg), ARG_CUR(arg));
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }

    parse_arg_eq_done(&pt);

    rv = bcm_l2_age_timer_set(unit, timer);
    if (rv != BCM_E_NONE) {
        cli_out("%s ERROR: %s\n", ARG_CMD(arg), bcm_errmsg(rv));
        return CMD_FAIL;
    }

    return CMD_OK;
}

/*!
 * \brief Add a L2 station entry.
 *
 * \param [in] unit           Unit number
 * \param [in] arg            Arg structure for parsing arguments.
 *
 * \retval CMD_OK             Command completed successfully.
 * \retval CMD_FAIL           Command failed.
 */
STATIC cmd_result_t
cmd_ltsw_l2_station_add(int unit, args_t *arg)
{
    parse_table_t pt;
    int rv, sid = -1, pri = 0, vlan = 0, vlan_mask = 0;
    int src_port = 0, src_port_mask = 0, ipv4 = 0, ipv6 = 0;
    int arp = 0, mpls = 0, mim = 0, trill = 0, fcoe = 0, oam = 0;
    int replace = 0, copy_to_cpu = 0, discard = 0;
    int ivid = 0, ivid_mask = 0, vfi = 0, vfi_mask = 0;
    bcm_l2_station_t station;
    bcm_mac_t dst_mac;
    bcm_mac_t dst_mac_mask;

    if (ARG_CNT(arg) == 0) {
        return CMD_USAGE;
    }

    sal_memset(dst_mac, 0x00, sizeof(bcm_mac_t));
    sal_memset(dst_mac_mask, 0x00, sizeof(bcm_mac_t));

    bcm_l2_station_t_init(&station);
    parse_table_init(unit, &pt);

    parse_table_add(&pt, "Priority", PQ_DFL|PQ_INT, 0, (void *)&pri,  NULL);
    parse_table_add(&pt, "ID", PQ_DFL|PQ_INT, (void *)-1, (void *)&sid,  NULL);
    parse_table_add(&pt, "MACaddress", PQ_DFL|PQ_MAC,0, (void *)&dst_mac, NULL);
    parse_table_add(&pt, "MACaddressMask", PQ_DFL|PQ_MAC, 0,
                    (void *)&dst_mac_mask, NULL);
    parse_table_add(&pt, "Vlanid", PQ_DFL|PQ_HEX, 0, (void *)&vlan, NULL);
    parse_table_add(&pt, "VlanidMask", PQ_DFL|PQ_HEX, 0,
                    (void *)&vlan_mask, NULL);
    parse_table_add(&pt, "Ivid", PQ_DFL|PQ_HEX, 0, (void *)&ivid, NULL);
    parse_table_add(&pt, "IvidMask", PQ_DFL|PQ_HEX, 0,
                    (void *)&ivid_mask, NULL);
    parse_table_add(&pt, "Vfi", PQ_DFL|PQ_HEX, 0, (void *)&vfi, NULL);
    parse_table_add(&pt, "VfiMask", PQ_DFL|PQ_HEX, 0,
                    (void *)&vfi_mask, NULL);
    parse_table_add(&pt, "SourcePort", PQ_DFL|PQ_INT, 0,
                    (void *)&src_port, NULL);
    parse_table_add(&pt, "SourcePortMask", PQ_DFL|PQ_HEX, 0,
                    (void *)&src_port_mask, NULL);
    parse_table_add(&pt, "IPv4", PQ_DFL|PQ_BOOL, 0, (void *)&ipv4, NULL);
    parse_table_add(&pt, "IPv6", PQ_DFL|PQ_BOOL, 0, (void *)&ipv6, NULL);
    parse_table_add(&pt, "ArpRarp", PQ_DFL|PQ_BOOL, 0, (void *)&arp, NULL);
    parse_table_add(&pt, "MPLS", PQ_DFL|PQ_BOOL, 0, (void *)&mpls, NULL);
    parse_table_add(&pt, "MiM", PQ_DFL|PQ_BOOL, 0, (void *)&mim, NULL);
    parse_table_add(&pt, "TRILL", PQ_DFL|PQ_BOOL, 0, (void *)&trill, NULL);
    parse_table_add(&pt, "FCoE", PQ_DFL|PQ_BOOL, 0, (void *)&fcoe, NULL);
    parse_table_add(&pt, "OAM", PQ_DFL|PQ_BOOL, 0, (void *)&oam, NULL);
    parse_table_add(&pt, "Replace", PQ_DFL|PQ_BOOL, 0, (void *)&replace, NULL);
    parse_table_add(&pt, "CPUmirror", PQ_DFL|PQ_BOOL, 0,
                    (void *)&copy_to_cpu, NULL);
    parse_table_add(&pt, "Discard", PQ_DFL|PQ_BOOL, 0, (void *)&discard, NULL);

    if (parse_arg_eq(arg, &pt) < 0 || ARG_CNT(arg) > 0) {
        cli_out("%s: Invalid option %s\n", ARG_CMD(arg), ARG_CUR(arg));
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }

    parse_arg_eq_done(&pt);
    station.priority = pri;

    if (sid != -1) {
        station.flags |= BCM_L2_STATION_WITH_ID;
    }
    sal_memcpy(station.dst_mac, dst_mac, sizeof(bcm_mac_t));
    sal_memcpy(station.dst_mac_mask, dst_mac_mask, sizeof(bcm_mac_t));

    station.vlan = vlan;
    station.vlan_mask = vlan_mask;

    station.inner_vlan = ivid;
    station.inner_vlan_mask = ivid_mask;

    station.vfi = vfi;
    station.vfi_mask = vfi_mask;

    station.src_port = src_port;
    station.src_port_mask = src_port_mask;

    if (ipv4 == 1) {
        station.flags |= BCM_L2_STATION_IPV4;
    }
    if (ipv6 == 1) {
        station.flags |= BCM_L2_STATION_IPV6;
    }
    if (arp == 1) {
        station.flags |= BCM_L2_STATION_ARP_RARP;
    }
    if (mpls == 1) {
        station.flags |= BCM_L2_STATION_MPLS;
    }
    if (mim == 1) {
        station.flags |= BCM_L2_STATION_MIM;
    }
    if (trill == 1) {
        station.flags |= BCM_L2_STATION_TRILL;
    }
    if (oam == 1) {
        station.flags |= BCM_L2_STATION_OAM;
    }
    if (fcoe == 1) {
        station.flags |= BCM_L2_STATION_FCOE;
    }
    if (replace == 1) {
        station.flags |= BCM_L2_STATION_REPLACE;
    }
    if (copy_to_cpu == 1) {
        station.flags |= BCM_L2_STATION_COPY_TO_CPU;
    }
    if (discard == 1) {
        station.flags |= BCM_L2_STATION_DISCARD;
    }

    rv =  bcm_l2_station_add(unit, &sid, &station);
    if (rv != BCM_E_NONE) {
        cli_out("%s ERROR: %s\n", ARG_CMD(arg), bcm_errmsg(rv));
        return CMD_FAIL;
    }

    if (!(station.flags & BCM_L2_STATION_WITH_ID)) {
        cli_out("Created SID=%d\n", sid);
    }
    return CMD_OK;
}

/*!
 * \brief Delete a L2 station entry.
 *
 * \param [in] unit           Unit number
 * \param [in] arg            Arg structure for parsing arguments.
 *
 * \retval CMD_OK             Command completed successfully.
 * \retval CMD_FAIL           Command failed.
 */
STATIC cmd_result_t
cmd_ltsw_l2_station_delete(int unit, args_t *arg)
{
    parse_table_t pt;
    int rv, sid = -1;

    if (ARG_CNT(arg) == 0) {
        return CMD_USAGE;
    }

    parse_table_init(unit, &pt);

    parse_table_add(&pt, "ID", PQ_DFL|PQ_INT, (void *)-1, (void *)&sid,  NULL);
    if (parse_arg_eq(arg, &pt) < 0 || ARG_CNT(arg) > 0) {
        cli_out("%s: Invalid option %s\n", ARG_CMD(arg), ARG_CUR(arg));
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }

    parse_arg_eq_done(&pt);

    rv =  bcm_l2_station_delete(unit, sid);
    if (rv != BCM_E_NONE) {
        cli_out("%s ERROR: %s\n", ARG_CMD(arg), bcm_errmsg(rv));
        return CMD_FAIL;
    }
    return CMD_OK;
}

/*!
 * \brief Clear all L2 station entries.
 *
 * \param [in] unit           Unit number
 * \param [in] arg            Arg structure for parsing arguments.
 *
 * \retval CMD_OK             Command completed successfully.
 * \retval CMD_FAIL           Command failed.
 */
STATIC cmd_result_t
cmd_ltsw_l2_station_clear(int unit, args_t *arg)
{
    int rv;

    if (ARG_CNT(arg) > 0) {
        return CMD_USAGE;
    }

    rv =  bcm_l2_station_delete_all(unit);
    if (rv != BCM_E_NONE) {
        cli_out("%s ERROR: %s\n", ARG_CMD(arg), bcm_errmsg(rv));
        return CMD_FAIL;
    }
    return CMD_OK;
}

/*!
 * \brief Callback function to print l2 station entry.
 *
 * \param [in] unit           Unit number
 * \param [in] sid            Station id.
 * \param [in] l2addr         L2 address structure.
 */
static void
ltsw_l2_station_dump(int unit, int sid, bcm_l2_station_t *station)
{
    bcm_l2_station_t l2station;

    l2station = *station;
    cli_out("\tSID=%d\n", sid);
    cli_out("\tPriority=%d\n", l2station.priority);
    cli_out("\tMAC=%x:%x:%x:%x:%x:%x\n", l2station.dst_mac[0],
            l2station.dst_mac[1], l2station.dst_mac[2], l2station.dst_mac[3],
            l2station.dst_mac[4], l2station.dst_mac[5]);
    cli_out("\tMAC MASK=%x:%x:%x:%x:%x:%x\n", l2station.dst_mac_mask[0],
            l2station.dst_mac_mask[1], l2station.dst_mac_mask[2],
            l2station.dst_mac_mask[3], l2station.dst_mac_mask[4],
            l2station.dst_mac_mask[5]);
    cli_out("\tVLAN=0x%x\n", l2station.vlan);
    cli_out("\tVLAN MASK=0x%x\n", l2station.vlan_mask);
    cli_out("\tInner VLAN=0x%x\n", l2station.inner_vlan);
    cli_out("\tInner VLAN MASK=0x%x\n", l2station.inner_vlan_mask);
    cli_out("\tVFI=0x%x\n", l2station.vfi);
    cli_out("\tVFI MASK=0x%x\n", l2station.vfi_mask);
    cli_out("\tSourcePort=0x%x\n", l2station.src_port);
    cli_out("\tSourcePort MASK=0x%x\n", l2station.src_port_mask);
    if (l2station.flags & BCM_L2_STATION_IPV4) {
        cli_out("\tIPv4\n");
    }
    if (l2station.flags & BCM_L2_STATION_IPV6) {
        cli_out("\tIPv6\n");
    }
    if (l2station.flags & BCM_L2_STATION_ARP_RARP) {
        cli_out("\tArpRarp\n");
    }
    if (l2station.flags & BCM_L2_STATION_MPLS) {
        cli_out("\tMpls\n");
    }
    if (l2station.flags & BCM_L2_STATION_MIM) {
        cli_out("\tMim\n");
    }
    if (l2station.flags & BCM_L2_STATION_TRILL) {
        cli_out("\tTrill\n");
    }
    if (l2station.flags & BCM_L2_STATION_OAM) {
        cli_out("\tOam\n");
    }
    if (l2station.flags & BCM_L2_STATION_FCOE) {
        cli_out("\tFcoe\n");
    }
    if (l2station.flags & BCM_L2_STATION_COPY_TO_CPU) {
        cli_out("\tCpu\n");
    }
    if (l2station.flags & BCM_L2_STATION_DISCARD) {
        cli_out("\tDrop\n");
    }
    /* Easy to differentiate multi entries */
    cli_out("\n");
    return;
}

/*!
 * \brief Show L2 station entries.
 *
 * \param [in] unit           Unit number
 * \param [in] arg            Arg structure for parsing arguments.
 *
 * \retval CMD_OK             Command completed successfully.
 * \retval CMD_FAIL           Command failed.
 */
STATIC cmd_result_t
cmd_ltsw_l2_station_show(int unit, args_t *arg)
{
    parse_table_t pt;
    int rv, sid = -1, tbl_sz = 0;
    bcm_l2_station_t station;

    /* Display all */
    if (ARG_CNT(arg) == 0) {
        rv = bcm_l2_station_size_get(unit, &tbl_sz);
        if (rv != BCM_E_NONE) {
            cli_out("%s ERROR: %s\n", ARG_CMD(arg), bcm_errmsg(rv));
            return CMD_FAIL;
        }

        for (sid = 0; sid < tbl_sz; sid++) {
            rv = bcm_l2_station_get(unit, sid, &station);
            if (rv == BCM_E_NOT_FOUND) {
                continue;
            } else if (rv != BCM_E_NONE) {
                cli_out("%s ERROR: %s\n", ARG_CMD(arg), bcm_errmsg(rv));
                return CMD_FAIL;
            }
            ltsw_l2_station_dump(unit, sid, &station);
        }
        return CMD_OK;
    }

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "ID", PQ_DFL|PQ_INT, (void *)-1, (void *)&sid,  NULL);

    if (parse_arg_eq(arg, &pt) < 0 || ARG_CNT(arg) > 0) {
        cli_out("%s: Invalid option %s\n", ARG_CMD(arg), ARG_CUR(arg));
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }

    parse_arg_eq_done(&pt);

    bcm_l2_station_t_init(&station);
    rv = bcm_l2_station_get(unit, sid, &station);
    if (rv != BCM_E_NONE) {
        cli_out("%s ERROR: %s\n", ARG_CMD(arg), bcm_errmsg(rv));
        return CMD_FAIL;
    }
    ltsw_l2_station_dump(unit, sid, &station);

    return CMD_OK;
}

/*!
 * \brief L2 station cmd handler.
 *
 * \param [in] unit           Unit number
 * \param [in] arg            Arg structure for parsing arguments.
 *
 * \retval CMD_OK             Command completed successfully.
 * \retval CMD_FAIL           Command failed.
 */
STATIC cmd_result_t
cmd_ltsw_l2_station(int unit, args_t *arg)
{
    char *subcmd = NULL;

    if ((subcmd = ARG_GET(arg)) == NULL) {
        return CMD_USAGE;
    }

    if (!sal_strcasecmp(subcmd, "add")) {
        return cmd_ltsw_l2_station_add(unit, arg);
    } else if (!sal_strcasecmp(subcmd, "delete")) {
        return cmd_ltsw_l2_station_delete(unit, arg);
    } else if (!sal_strcasecmp(subcmd, "clear")) {
        return cmd_ltsw_l2_station_clear(unit, arg);
    } else if (!sal_strcasecmp(subcmd, "show")) {
        return cmd_ltsw_l2_station_show(unit, arg);
    } else {
        return CMD_USAGE;
    }
    return CMD_OK;
}

/*!
 * \brief Init L2 module.
 *
 * \param [in] unit           Unit number
 * \param [in] arg            Arg structure for parsing arguments.
 *
 * \retval CMD_OK             Command completed successfully.
 * \retval CMD_FAIL           Command failed.
 */
STATIC cmd_result_t
cmd_ltsw_l2_init(int unit, args_t *arg)
{
    int rv;

    if (ARG_CNT(arg) > 0) {
        return CMD_USAGE;
    }

    rv = bcm_l2_init(unit);
    if (rv < 0) {
        cli_out("%s: ERROR: bcm_l2_init failed %s\n",
                ARG_CMD(arg), bcm_errmsg(rv));
        return CMD_FAIL;
    }

    return CMD_OK;
}

/*!
 * \brief Detach L2 module.
 *
 * \param [in] unit           Unit number
 * \param [in] arg            Arg structure for parsing arguments.
 *
 * \retval CMD_OK             Command completed successfully.
 * \retval CMD_FAIL           Command failed.
 */
STATIC cmd_result_t
cmd_ltsw_l2_detach(int unit, args_t *arg)
{
    int rv;

    if (ARG_CNT(arg) > 0) {
        return CMD_USAGE;
    }

    rv = bcm_l2_detach(unit);
    if (rv < 0) {
        cli_out("%s: ERROR: bcm_l2_init failed %s\n",
                ARG_CMD(arg), bcm_errmsg(rv));
        return CMD_FAIL;
    }

    return CMD_OK;
}

/*!
 * \brief Enable/Disable global L2 learning.
 *
 * \param [in] unit           Unit number
 * \param [in] arg            Arg structure for parsing arguments.
 *
 * \retval CMD_OK             Command completed successfully.
 * \retval CMD_FAIL           Command failed.
 */
STATIC cmd_result_t
cmd_ltsw_l2_learn(int unit, args_t *arg)
{
    int rv;
    char *subcmd = NULL;
    int enable = 0;

    if (ARG_CNT(arg) == 0) {
        rv = bcmi_ltsw_l2_learn_get(unit, &enable);
        if (rv < 0) {
            cli_out("%s: ERROR: Get global L2 learning setting failed %s\n",
                    ARG_CMD(arg), bcm_errmsg(rv));
            return CMD_FAIL;
        }
        cli_out("L2 learning is %s\n", enable ? "on" : "off");
        return CMD_OK;
    } else if (ARG_CNT(arg) > 1) {
        rv = BCM_E_PARAM;
        cli_out("%s: ERROR: %s\n", ARG_CMD(arg), bcm_errmsg(rv));
        return CMD_USAGE;
    }

    subcmd = ARG_GET(arg);
    if (subcmd == NULL) {
        return CMD_USAGE;
    }

    if (sal_strcasecmp(subcmd, "on") == 0) {
        enable = TRUE;
    } else if (sal_strcasecmp(subcmd, "off") == 0) {
        enable = FALSE;
    } else {
        rv = BCM_E_PARAM;
        cli_out("%s: ERROR: %s\n", ARG_CMD(arg), bcm_errmsg(rv));
        return CMD_FAIL;
    }

    rv = bcmi_ltsw_l2_learn_set(unit, enable);
    if (rv < 0) {
        cli_out("%s: ERROR: Set global L2 learning failed %s\n",
                ARG_CMD(arg), bcm_errmsg(rv));
        return CMD_FAIL;
    }
    return CMD_OK;
}

/*!
 * \brief Show L2 count.
 *
 * \param [in] unit           Unit number
 * \param [in] arg            Arg structure for parsing arguments.
 *
 * \retval CMD_OK             Command completed successfully.
 * \retval CMD_FAIL           Command failed.
 */
STATIC cmd_result_t
cmd_ltsw_l2_count(int unit, args_t *arg)
{
    int rv, count = 0;

    if (ARG_CNT(arg) > 0) {
        return CMD_USAGE;
    }

    rv = bcmi_ltsw_l2_count_get(unit, &count);
    if (rv < 0) {
        cli_out("%s: ERROR: bcmi_ltsw_l2_count_get failed %s\n",
                ARG_CMD(arg), bcm_errmsg(rv));
        return CMD_FAIL;
    }
    cli_out("L2 count is %d\n", count);

    return CMD_OK;
}

/******************************************************************************
 * Public Functions
 */

/*!
 * \brief L2 cmd handler.
 *
 * \param [in] unit           Unit number
 * \param [in] arg            Arg structure for parsing arguments.
 *
 * \retval CMD_OK             Command completed successfully.
 * \retval CMD_FAIL           Command failed.
 */
cmd_result_t
cmd_ltsw_l2(int unit, args_t *arg)
{
    char *subcmd = NULL;

    if ((subcmd = ARG_GET(arg)) == NULL) {
        return CMD_USAGE;
    }

    if (!sal_strcasecmp(subcmd, "add")) {
        return cmd_ltsw_l2_add(unit, arg);
    } else if (!sal_strcasecmp(subcmd, "del")) {
        return cmd_ltsw_l2_delete(unit, arg);
    } else if (!sal_strcasecmp(subcmd, "get")) {
        return cmd_ltsw_l2_get(unit, arg);
    } else if (!sal_strcasecmp(subcmd, "show")) {
        return cmd_ltsw_l2_show(unit, arg);
    } else if (!sal_strcasecmp(subcmd, "clear")) {
        return cmd_ltsw_l2_clear(unit, arg);
    } else if (!sal_strcasecmp(subcmd, "replace")) {
        return cmd_ltsw_l2_replace(unit, arg);
    } else if (!sal_strcasecmp(subcmd, "age")) {
        return cmd_ltsw_l2_age(unit, arg);
    } else if (!sal_strcasecmp(subcmd, "station")) {
        return cmd_ltsw_l2_station(unit, arg);
    } else if (!sal_strcasecmp(subcmd, "init")) {
        return cmd_ltsw_l2_init(unit, arg);
    } else if (!sal_strcasecmp(subcmd, "detach")) {
        return cmd_ltsw_l2_detach(unit, arg);
    } else if (!sal_strcasecmp(subcmd, "learn")) {
        return cmd_ltsw_l2_learn(unit, arg);
    } else if (!sal_strcasecmp(subcmd, "count")) {
        return cmd_ltsw_l2_count(unit, arg);
    } else {
        return CMD_NOTIMPL;
    }
    return CMD_OK;
}

#endif /* BCM_LTSW_SUPPORT */
