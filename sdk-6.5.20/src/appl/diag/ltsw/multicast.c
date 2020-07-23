/*! \file multicast.c
 *
 * LTSW MULTICAST command.
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

#include <bcm/multicast.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/ltsw/multicast.h>

static const char *cmd_multicast_parse_type[] = {
    "NONE",
    "L2",
    "L3",
    "VPLS",
    "SUBPORT",
    "MIM",
    "WLAN",
    "VLAN",
    "TRILL",
    "NIV",
    "EGRESS_OBJECT",
    "L2GRE",
    "VXLAN",
    "PORTS_GROUP",
    "EXTENDER",
    "FLOW",
    NULL
};

static const char *cmd_multicast_parse_encap_type[] = {
    "INVALID",
    "L3",
    "L3TNL",
    "L2TNL",
    "L2TNLACC",
    NULL
};

static int
_cmd_multicast_traverse_cb(int unit, bcm_multicast_t group, uint32 flags,
                           void *user_data)
{
    int rv, i, port_count;
    bcm_gport_t *port_array = NULL;
    bcm_if_t    *encap_id_array = NULL;
    int         port_max = 0;


    /* first, get the port count.
     * port_max = 0, port_array & encap_id_array = NULL
     */
    rv = bcm_multicast_egress_get(unit, group, port_max, port_array,
                                  encap_id_array, &port_count);
    if (rv < 0) {
        cli_out("%s ERROR: egress port count get failed - %s\n",
                ARG_CMD((args_t *)user_data), bcm_errmsg(rv));
        return rv;
    }
    if (port_count) {
        /* allocate proper size port_array & encap_id_array */
        port_array = (bcm_gport_t *)sal_alloc
                     (sizeof(bcm_gport_t) * port_count,
                      "_cmd_multicast_traverse_cb : port_array");
        if (port_array == NULL) {
            cli_out("%s ERROR: port_array mem alloc failed\n",
                   ARG_CMD((args_t *)user_data));
            return BCM_E_MEMORY;
        }
        encap_id_array = (bcm_if_t *)sal_alloc(
                         sizeof(bcm_if_t) *port_count,
                         "_cmd_multicast_traverse_cb : encap_id_array");
        if (encap_id_array == NULL) {
            cli_out("%s ERROR: encap_id_array mem alloc failed\n",
                   ARG_CMD((args_t *)user_data));
            sal_free(port_array);
            return BCM_E_MEMORY;
        }

        rv = bcm_multicast_egress_get(unit, group, port_count,
                                      port_array, encap_id_array,
                                      &port_count);
        if (rv < 0) {
            cli_out("%s ERROR: egress get failure - %s\n",
                    ARG_CMD((args_t *)user_data), bcm_errmsg(rv));
            sal_free(port_array);
            sal_free(encap_id_array);
            return rv;
        }
        if (_BCM_MULTICAST_TYPE_GET(group) >=
            COUNTOF(cmd_multicast_parse_type)) {
            cli_out("Group 0x%x (%s)\n", group, "UNKNOWN");
        } else {
            cli_out("Group 0x%x (%s)\n", group,
                   cmd_multicast_parse_type[_BCM_MULTICAST_TYPE_GET(group)]);
        }
        for (i = 0; i < port_count; i++) {
            cli_out("\tport %s, encap id %d\n",
                   mod_port_name(unit, -1, port_array[i]), encap_id_array[i]);
        }

        sal_free(port_array);
        sal_free(encap_id_array);
    }

    return BCM_E_NONE;
}

static
int cmd_ltsw_mc_create(int unit, args_t *arg)
{
    parse_table_t pt;
    int rv, type;
    uint32 flags;
    bcm_multicast_t group;
    char buf[20];

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Type", PQ_MULTI, 0, &type, cmd_multicast_parse_type);
    parse_table_add(&pt, "Group", PQ_INT, (void *)-1, &group, 0);
    if (parse_arg_eq(arg, &pt) < 0) {
        cli_out("%s: Invalid argument: %s\n", ARG_CMD(arg), ARG_CUR(arg));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }
    parse_arg_eq_done(&pt);

    switch (type) {
    case _BCM_MULTICAST_TYPE_L2:
        flags = BCM_MULTICAST_TYPE_L2;
        break;
    case _BCM_MULTICAST_TYPE_L3:
        flags = BCM_MULTICAST_TYPE_L3;
        break;
    case _BCM_MULTICAST_TYPE_VPLS:
        flags = BCM_MULTICAST_TYPE_VPLS;
        break;
    case _BCM_MULTICAST_TYPE_FLOW:
        flags = BCM_MULTICAST_TYPE_FLOW;
        break;
    default:
        return CMD_FAIL;
    }
    if (group != -1) {
        flags |= BCM_MULTICAST_WITH_ID;
        if (!_BCM_MULTICAST_IS_SET(group)) {
            /* Encode the group properly for the selected type */
            _BCM_MULTICAST_GROUP_SET(group, type, group);
        }
    }
    rv = bcm_multicast_create(unit, flags, &group);
    if (rv != BCM_E_NONE) {
        cli_out("%s ERROR: fail to create %s group - %s\n",
                ARG_CMD(arg), cmd_multicast_parse_type[type],
                bcm_errmsg(rv));
        return CMD_FAIL;
    }

    cli_out("group id 0x%x\n", group);
    sal_sprintf(buf, "0x%x", group);
    var_set("multicast_group", buf, TRUE, FALSE);
    return CMD_OK;
}

static
int cmd_ltsw_mc_destroy(int unit, args_t *arg)
{
    parse_table_t pt;
    bcm_multicast_t group;
    int rv;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Group", PQ_INT, (void *)-1, &group, 0);
    if (parse_arg_eq(arg, &pt) < 0 || group == -1) {
        cli_out("%s: Invalid argument: %s\n", ARG_CMD(arg), ARG_CUR(arg));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }
    parse_arg_eq_done(&pt);

    rv = bcm_multicast_destroy(unit, group);
    if (rv != BCM_E_NONE) {
        cli_out("%s ERROR: fail to destroy group %d - %s\n",
                ARG_CMD(arg), group, bcm_errmsg(rv));
        return CMD_FAIL;
    }
    return CMD_OK;
}

static
int cmd_ltsw_mc_encap_create(int unit, args_t *arg)
{
    parse_table_t   pt;
    int             rv, encap_type;
    bcm_gport_t     port_id;
    bcm_if_t        ul_egress_if, l3_intf;
    bcm_if_t        encap_id;
    bcm_multicast_encap_t mc_encap;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "EncapType", PQ_MULTI, 0, &encap_type,
                    cmd_multicast_parse_encap_type);
    parse_table_add(&pt, "UlEgressIf", PQ_INT, 0, &ul_egress_if, 0);
    parse_table_add(&pt, "L3Intf", PQ_INT, 0, &l3_intf, 0);
    parse_table_add(&pt, "PortId", PQ_PORT, 0, &port_id, 0);
    if (parse_arg_eq(arg, &pt) < 0) {
        cli_out("%s: Invalid argument: %s\n", ARG_CMD(arg), ARG_CUR(arg));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }
    parse_arg_eq_done(&pt);

    if ((encap_type < 0) || (encap_type >= bcmMulticastEncapTypeCount)) {
        return CMD_FAIL;
    }
    mc_encap.encap_type   = encap_type;
    mc_encap.ul_egress_if = ul_egress_if;
    mc_encap.l3_intf      = l3_intf;
    mc_encap.port_id      = port_id;

    rv = bcm_multicast_encap_create(unit, &mc_encap,&encap_id);
    if (rv < 0) {
        cli_out("%s ERROR: fail to create encap_id - %s\n",
                ARG_CMD(arg), bcm_errmsg(rv));
        return CMD_FAIL;
    }
    cli_out("Encap ID %d\n", encap_id);
    return CMD_OK;
}

static
int cmd_ltsw_mc_encap_destroy(int unit, args_t *arg)
{
    parse_table_t   pt;
    int             rv;
    bcm_if_t        encap_id;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "EncapId", PQ_INT, 0, &encap_id, 0);
    if (parse_arg_eq(arg, &pt) < 0) {
        cli_out("%s: Invalid argument: %s\n", ARG_CMD(arg), ARG_CUR(arg));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }
    parse_arg_eq_done(&pt);

    rv = bcm_multicast_encap_destroy(unit, encap_id);
    if (rv < 0) {
        cli_out("%s ERROR: fail to create encap_id - %s\n",
                ARG_CMD(arg), bcm_errmsg(rv));
        return CMD_FAIL;
    }
    cli_out("Encap ID %d\n", encap_id);
    return CMD_OK;
}

static
int cmd_ltsw_mc_encap_show(int unit, args_t *arg)
{
    parse_table_t   pt;
    int             rv;
    bcm_if_t        encap_id;
    bcm_multicast_encap_t encap;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "EncapId", PQ_INT, 0, &encap_id, 0);
    if (parse_arg_eq(arg, &pt) < 0) {
        cli_out("%s: Invalid argument: %s\n", ARG_CMD(arg), ARG_CUR(arg));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }
    parse_arg_eq_done(&pt);

    bcm_multicast_encap_t_init(&encap);
    rv = bcm_multicast_encap_get(unit, encap_id, &encap);
    if (rv < 0) {
        cli_out("%s ERROR: fail to get encap_id - %s\n",
                ARG_CMD(arg), bcm_errmsg(rv));
        return CMD_FAIL;
    }

    cli_out("EncapType %s\n", cmd_multicast_parse_encap_type[encap.encap_type]);
    cli_out("UlEgressIf %d\n", encap.ul_egress_if);
    cli_out("L3Intf %d\n", encap.l3_intf);
    cli_out("PortId 0x%x\n", encap.port_id);

    return CMD_OK;
}


static
int cmd_ltsw_mc_egress_add(int unit, args_t *arg)
{
    parse_table_t pt;
    int rv;
    bcm_multicast_t group;
    bcm_gport_t port;
    bcm_if_t encap_id;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Group", PQ_INT, (void *)-1, &group, 0);
    parse_table_add(&pt, "Port", PQ_PORT, (void *)BCM_GPORT_INVALID, &port, 0);
    parse_table_add(&pt, "EncapId", PQ_INT, (void *)BCM_IF_INVALID,
                    &encap_id, 0);

    if (parse_arg_eq(arg, &pt) < 0 || group < 0 ||
        port == BCM_GPORT_INVALID) {
        cli_out("%s: Invalid argument: %s\n", ARG_CMD(arg), ARG_CUR(arg));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }
    parse_arg_eq_done(&pt);

    if (!BCM_GPORT_IS_SET(port)) {
        BCM_GPORT_LOCAL_SET(port, port);
    }
    rv = bcm_multicast_egress_add(unit, group, port, encap_id);
    if (rv != BCM_E_NONE) {
        cli_out("%s ERROR: egress add failure - %s\n",
                ARG_CMD(arg), bcm_errmsg(rv));
        return CMD_FAIL;
    }
    return CMD_OK;
}

static
int cmd_ltsw_mc_egress_del(int unit, args_t *arg)
{
    parse_table_t pt;
    int rv;
    bcm_multicast_t group;
    bcm_gport_t port;
    bcm_if_t encap_id;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Group", PQ_INT, (void *)-1, &group, 0);
    parse_table_add(&pt, "Port", PQ_PORT, (void *)BCM_GPORT_INVALID, &port,
                    0);
    parse_table_add(&pt, "EncapId", PQ_INT, (void *)BCM_IF_INVALID,
                    &encap_id, 0);
    if (parse_arg_eq(arg, &pt) < 0 || group < 0) {
        cli_out("%s: Invalid argument: %s\n", ARG_CMD(arg), ARG_CUR(arg));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }
    parse_arg_eq_done(&pt);

    if (port == BCM_GPORT_INVALID && encap_id == BCM_IF_INVALID) {
        rv = bcm_multicast_egress_delete_all(unit, group);
        if (rv < 0) {
            cli_out("%s ERROR: egress delete all failure - %s\n",
                    ARG_CMD(arg), bcm_errmsg(rv));
            return CMD_FAIL;
        }
    } else {
        if (port == BCM_GPORT_INVALID) {
            return CMD_FAIL;
        }
        if (!BCM_GPORT_IS_SET(port)) {
            BCM_GPORT_LOCAL_SET(port, port);
        }
        rv = bcm_multicast_egress_delete(unit, group, port, encap_id);
        if (rv < 0) {
            cli_out("%s ERROR: egress delete failure - %s\n",
                    ARG_CMD(arg), bcm_errmsg(rv));
            return CMD_FAIL;
        }
    }
    return CMD_OK;
}

static
int cmd_ltsw_mc_show(int unit, args_t *arg)
{
    parse_table_t pt;
    int rv;
    bcm_multicast_t group;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Group", PQ_INT, (void *)-1, &group, 0);
    if (parse_arg_eq(arg, &pt) < 0) {
        cli_out("%s: Invalid argument: %s\n", ARG_CMD(arg), ARG_CUR(arg));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }
    parse_arg_eq_done(&pt);


    if (group < 0) {
        rv = bcm_multicast_group_traverse(unit,
                                          &_cmd_multicast_traverse_cb,
                                          BCM_MULTICAST_TYPE_MASK, arg);
    } else {
        rv = _cmd_multicast_traverse_cb(unit, group, 0, arg);
    }

    if (BCM_FAILURE(rv)) {
        return CMD_FAIL;
    }

    return CMD_OK;
}

static int
cmd_ltsw_mc_encap(int unit, args_t *arg)
{
    char *subcmd = NULL;

    subcmd = ARG_GET(arg);
    if (subcmd == NULL) {
        return CMD_USAGE;
    }

    if (!sal_strcasecmp(subcmd, "create")) {
        return cmd_ltsw_mc_encap_create(unit, arg);
    } else if (!sal_strcasecmp(subcmd, "destroy")) {
        return cmd_ltsw_mc_encap_destroy(unit, arg);
    } else if (!sal_strcasecmp(subcmd, "show")) {
        return cmd_ltsw_mc_encap_show(unit, arg);
    } else {
        return CMD_USAGE;
    }

    return CMD_OK;
}

cmd_result_t
cmd_ltsw_multicast(int unit, args_t *arg)
{
    char *subcmd = NULL;

    subcmd = ARG_GET(arg);
    if (subcmd == NULL) {
        return CMD_USAGE;
    }

    if (!sal_strcasecmp(subcmd, "create")) {
        return cmd_ltsw_mc_create(unit, arg);
    } else if (!sal_strcasecmp(subcmd, "destroy")) {
        return cmd_ltsw_mc_destroy(unit, arg);
    } else if (!sal_strcasecmp(subcmd, "encap")) {
        return cmd_ltsw_mc_encap(unit, arg);
    } else if (!sal_strcasecmp(subcmd, "add")) {
        return cmd_ltsw_mc_egress_add(unit, arg);
    } else if (!sal_strcasecmp(subcmd, "delete")) {
        return cmd_ltsw_mc_egress_del(unit, arg);
    } else if (!sal_strcasecmp(subcmd, "show")) {
        return cmd_ltsw_mc_show(unit, arg);
    } else {
        return CMD_USAGE;
    }

    return CMD_OK;
}

#endif /* BCM_LTSW_SUPPORT */
