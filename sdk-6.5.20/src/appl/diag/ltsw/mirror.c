/*! \file mirror.c
 *
 * LTSW MIRROR command.
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

#include <bcm/mirror.h>
#include <bcm/init.h>
#include <bcm/stack.h>

#include <bcm_int/ltsw/mirror.h>
#include <bcm_int/ltsw/port.h>
#include <bcm_int/ltsw/virtual.h>

/******************************************************************************
 * Private functions
 */
/*!
 * \brief Callback function to show Mirror destination.
 *
 * \param [in] unit Unit number.
 * \param [in] mirror_dest Mirror destination structure.
 * \param [in] user_data User data.
 *
 * \retval CMD_OK Command completed successfully.
 * \retval CMD_FAIL Command failed.
 */
static int
show_mirror_destination_cb(int unit, bcm_mirror_destination_t *mirror_dest,
                           void *user_data)
{
    int *count = (int *)user_data, rv;

    if (mirror_dest == NULL || user_data == NULL) {
        return BCM_E_PARAM;
    }

    (*count)++;
    cli_out("Mirror Dest %2d: ", *count);
    cli_out("Mirror ID: %2d; ",
            BCM_GPORT_MIRROR_GET(mirror_dest->mirror_dest_id));
    if (mirror_dest->flags & BCM_MIRROR_DEST_TUNNEL_L2) {
        cli_out("%s ", "L2 Tunnel to");
    }
    if (mirror_dest->flags & BCM_MIRROR_DEST_TUNNEL_IP_GRE) {
        cli_out("IPv%d GRE Tunnel\n", mirror_dest->version);

        if (4 == mirror_dest->version) {
            char        buffer[16];

            format_ipaddr(buffer, mirror_dest->src_addr);
            cli_out("\tSrc IP=%s\n", buffer);
            format_ipaddr(buffer, mirror_dest->dst_addr);
            cli_out("\tDst IP=%s\n", buffer);
        } else if (6 == mirror_dest->version) {
            char        buffer[64];

            format_ip6addr(buffer, mirror_dest->src6_addr);
            cli_out("\tSrc IP=%s\n", buffer);
            format_ip6addr(buffer, mirror_dest->dst6_addr);
            cli_out("\tDst IP=%s\n", buffer);
            cli_out("\tFlow Label=0x%04x\n", mirror_dest->flow_label);
        }
        cli_out("\tsrc_mac=%02x:%02x:%02x:%02x:%02x:%02x\n",
                mirror_dest->src_mac[0], mirror_dest->src_mac[1],
                mirror_dest->src_mac[2], mirror_dest->src_mac[3],
                mirror_dest->src_mac[4], mirror_dest->src_mac[5]);
        cli_out("\tdst_mac=%02x:%02x:%02x:%02x:%02x:%02x\n",
                mirror_dest->dst_mac[0], mirror_dest->dst_mac[1],
                mirror_dest->dst_mac[2], mirror_dest->dst_mac[3],
                mirror_dest->dst_mac[4], mirror_dest->dst_mac[5]);

        cli_out("\tvlan_id=0x%04x\n", mirror_dest->vlan_id);
        cli_out("\ttpid=0x%04x\n", mirror_dest->tpid);
        cli_out("\tttl=%d\n", mirror_dest->ttl);
        cli_out("\ttos=%d\n", mirror_dest->tos);
        cli_out("\tmirrored to ");
    }
    if (mirror_dest->flags & BCM_MIRROR_DEST_PAYLOAD_UNTAGGED) {
        cli_out("%s ", "Payload Untagged");
    }

    if (BCM_GPORT_IS_LOCAL(mirror_dest->gport)) {
        cli_out("local port %s",
                BCM_PORT_NAME(unit, BCM_GPORT_LOCAL_GET(mirror_dest->gport)));
    } else if (BCM_GPORT_IS_MODPORT(mirror_dest->gport)) {
        bcm_port_t  local_port;

        rv = bcm_port_local_get(unit, mirror_dest->gport, &local_port);
        if (rv != BCM_E_NONE) {
            cli_out("modport %d/%d",
                    BCM_GPORT_MODPORT_MODID_GET(mirror_dest->gport),
                    BCM_GPORT_MODPORT_PORT_GET(mirror_dest->gport));
        } else {
            cli_out("local port %s", BCM_PORT_NAME(unit, local_port));
        }
    } else if (BCM_GPORT_IS_TRUNK(mirror_dest->gport)) {
        cli_out("trunk group %d", BCM_GPORT_TRUNK_GET(mirror_dest->gport));
    } else {
        cli_out("GPORT 0x%08X", mirror_dest->gport);
    }
    cli_out("\n");
    return BCM_E_NONE;
}

/*!
 * \brief Mirror destination show subcmd handler.
 *
 * \param [in] unit Unit number.
 * \param [in] arg Arg structure for parsing arguments.
 *
 * \retval CMD_OK Command completed successfully.
 * \retval CMD_FAIL Command failed.
 */
STATIC cmd_result_t
cmd_ltsw_mirror_dest_show(int unit, args_t *arg)
{
    int rv, count = 0;

    rv = bcm_mirror_destination_traverse(unit, show_mirror_destination_cb,
                                         &count);
    if (BCM_FAILURE(rv)) {
        cli_out("%s bcm_mirror_destination_traverse() failed  %s\n",
                ARG_CMD(arg), bcm_errmsg(rv));
        return CMD_FAIL;
    }
    return CMD_OK;
}

/*!
 * \brief Mirror destination cmd handler.
 *
 * \param [in] unit Unit number.
 * \param [in] arg Arg structure for parsing arguments.
 *
 * \retval CMD_OK Command completed successfully.
 * \retval CMD_FAIL Command failed.
 */
STATIC cmd_result_t
cmd_ltsw_mirror_dest(int unit, args_t *arg)
{
    parse_table_t pt;
    char *cmd = NULL;
    bcm_ip_t src_ip = 0, dest_ip = 0;
    bcm_ip6_t src_ip6, dest_ip6;
    bcm_mac_t src_mac, dest_mac;
    bcm_gport_t mirror_dest_id = 0;
    bcm_port_t src_port = 0, dest_port = 0;
    uint32_t flow_lable = 0;
    uint32_t mirror_port_flags = 0;
    int vlan_id = -1, tpid = -1, ttl = 0,tos = 0, version = 0;
    int mode = 0, tunnel = 0, rv;
    bool strip_vlan = false;
    bcm_mirror_destination_t *mirror_dest = NULL;
    char *modeList[] =
        {"OFF", "Ingress", "Egress", "EgressTrue", "IngressEgress", NULL};
    char *tunnelList[] = {"OFF","L2", "GRE", NULL};
    cmd_result_t cmd_rv = CMD_OK;

    if ((cmd = ARG_GET(arg)) == NULL) {
        cmd_rv = CMD_USAGE;
        goto exit;
    }

    if (sal_strcasecmp("show", cmd) == 0) {
        cmd_rv = cmd_ltsw_mirror_dest_show(unit, arg);
        goto exit;
    }

    sal_memset(src_ip6, 0, sizeof(bcm_ip6_t));
    sal_memset(dest_ip6, 0, sizeof(bcm_ip6_t));
    sal_memset(src_mac, 0, sizeof(bcm_mac_t));
    sal_memset(dest_mac, 0, sizeof(bcm_mac_t));

    parse_table_init(unit, &pt);

    parse_table_add(&pt, "Id", PQ_DFL | PQ_INT,0, &mirror_dest_id, NULL);
    parse_table_add(&pt, "Mode", PQ_DFL | PQ_MULTI,(void *)(0),
                    &mode, modeList);
    parse_table_add(&pt, "Tunnel", PQ_DFL | PQ_MULTI,(void *)(0),
                    &tunnel, tunnelList);
    parse_table_add(&pt, "SrcPort", PQ_DFL | PQ_PORT, 0, &src_port, NULL);
    parse_table_add(&pt, "DestPort", PQ_DFL | PQ_PORT, 0, &dest_port, NULL);
    parse_table_add(&pt, "SrcIP", PQ_DFL | PQ_IP, 0, (void *)&src_ip, NULL);
    parse_table_add(&pt, "DestIP", PQ_DFL | PQ_IP, 0, (void *)&dest_ip, NULL);
    parse_table_add(&pt, "SrcIP6", PQ_DFL | PQ_IP6, 0, (void *)&src_ip6, NULL);
    parse_table_add(&pt, "DestIP6", PQ_DFL | PQ_IP6, 0,
                    (void *)&dest_ip6, NULL);
    parse_table_add(&pt, "SrcMAC", PQ_DFL|PQ_MAC, 0, (void *)src_mac, NULL);
    parse_table_add(&pt, "DestMAC", PQ_DFL|PQ_MAC, 0, (void *)dest_mac, NULL);
    parse_table_add(&pt, "VLAN", PQ_DFL|PQ_INT, 0, (void *)&vlan_id, NULL);
    parse_table_add(&pt, "tpid", PQ_DFL|PQ_INT, 0, (void *)&tpid, NULL);
    parse_table_add(&pt, "Version",  PQ_DFL|PQ_INT, 0, (void *)&version, NULL);
    parse_table_add(&pt, "TTL",  PQ_DFL|PQ_INT, 0, (void *)&ttl, NULL);
    parse_table_add(&pt, "TOS",  PQ_DFL|PQ_INT, 0, (void *)&tos, NULL);
    parse_table_add(&pt, "FlowLable",  PQ_DFL|PQ_INT, 0,
                    (void *)&flow_lable, NULL);
    parse_table_add(&pt, "NoVLAN",  PQ_DFL|PQ_BOOL, 0,
                    (void *)&strip_vlan, NULL);

    if (parse_arg_eq(arg, &pt) < 0 || ARG_CNT(arg) > 0) {
        cli_out("%s: Invalid option %s\n", ARG_CMD(arg), ARG_CUR(arg));
        parse_arg_eq_done(&pt);
        cmd_rv = CMD_USAGE;
        goto exit;
    }

    parse_arg_eq_done(&pt);
    /* Allocate memory from heap to avoid stack overflow. */
    mirror_dest = sal_alloc(sizeof(bcm_mirror_destination_t),
                            "CLI Mirror Dest");
    if (mirror_dest == NULL) {
        cmd_rv = CMD_FAIL;
        goto exit;
    }

    bcm_mirror_destination_t_init(mirror_dest);

    switch (mode) {
        case 1:
            mirror_port_flags |= BCM_MIRROR_PORT_INGRESS;
            break;
        case 2:
            mirror_port_flags |= BCM_MIRROR_PORT_EGRESS;
            break;
        case 3:
            mirror_port_flags |= BCM_MIRROR_PORT_EGRESS_TRUE;
            break;
        case 4:
            mirror_port_flags |=
                (BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS);
            break;
        default:
            break;
    }

    switch (tunnel) {
        case 1:
            mirror_dest->flags |= BCM_MIRROR_DEST_TUNNEL_L2;
            sal_memcpy(mirror_dest->src_mac, src_mac, sizeof(src_mac));
            sal_memcpy(mirror_dest->dst_mac, dest_mac, sizeof(dest_mac));
            mirror_dest->vlan_id = (bcm_vlan_t) (vlan_id & 0xffff);
            mirror_dest->tpid = (uint16_t) (tpid & 0xffff);
            break;
        case 2:
            mirror_dest->flags |= BCM_MIRROR_DEST_TUNNEL_IP_GRE;
            sal_memcpy(mirror_dest->src_mac, src_mac, sizeof(src_mac));
            sal_memcpy(mirror_dest->dst_mac, dest_mac, sizeof(dest_mac));
            mirror_dest->vlan_id = (bcm_vlan_t) (vlan_id & 0xffff);
            mirror_dest->tpid = (uint16_t) (tpid & 0xffff);
            mirror_dest->src_addr = src_ip;
            mirror_dest->dst_addr = dest_ip;
            mirror_dest->ttl = ttl;
            mirror_dest->tos = tos;
            mirror_dest->flow_label = flow_lable;
            mirror_dest->version = version;
            sal_memcpy(mirror_dest->src6_addr, src_ip6, sizeof(src_ip6));
            sal_memcpy(mirror_dest->dst6_addr, dest_ip6, sizeof(dest_ip6));
            break;
        default:
            break;
    }

    if (strip_vlan) {
        mirror_dest->flags |= BCM_MIRROR_DEST_PAYLOAD_UNTAGGED;
    }

    if (!BCM_GPORT_IS_SET(dest_port)) {
        rv = bcm_port_gport_get(unit, dest_port, &dest_port);
        if (BCM_FAILURE(rv)) {
            cli_out("Illegal port [%d]\n", dest_port);
            cmd_rv = CMD_FAIL;
            goto exit;
        }
    }

    mirror_dest->gport = dest_port;
    if (mirror_dest_id != 0) {
        mirror_dest->flags |= BCM_MIRROR_DEST_WITH_ID;
        mirror_dest->mirror_dest_id = mirror_dest_id;
    }

    if (sal_strcasecmp("create", cmd) == 0) {
        rv = bcm_mirror_destination_create(unit, mirror_dest);
        if (BCM_FAILURE(rv)) {
            cli_out("%s bcm_mirror_destination_create() failed  %s\n",
                    ARG_CMD(arg), bcm_errmsg(rv));
            cmd_rv = CMD_FAIL;
            goto exit;
        }
        cli_out(" Mirror Id = 0x%x\n", mirror_dest->mirror_dest_id);
        cmd_rv = CMD_OK;
        goto exit;
    } else if (sal_strcasecmp("add", cmd) == 0) {
        rv = bcm_mirror_port_dest_add(unit, src_port,
                                      mirror_port_flags, mirror_dest_id);
        if (BCM_FAILURE(rv)) {
            cli_out("%s bcm_mirror_port_dest_add() failed  %s\n",
                    ARG_CMD(arg), bcm_errmsg(rv));
            cmd_rv = CMD_FAIL;
            goto exit;
        }
        cmd_rv = CMD_OK;
        goto exit;
    } else if (sal_strcasecmp("destroy", cmd) == 0) {
        if (0 == mirror_dest_id) {
            cli_out("%s mirror destination id is missing\n", ARG_CMD(arg));
            cmd_rv = CMD_FAIL;
            goto exit;
        }
        rv = bcm_mirror_destination_destroy(unit, mirror_dest_id);
        if (BCM_FAILURE(rv)) {
            cli_out("%s bcm_mirror_destination_destroy failed()  %s\n",
                    ARG_CMD(arg), bcm_errmsg(rv));
            cmd_rv = CMD_FAIL;
            goto exit;
        }
        cmd_rv = CMD_OK;
        goto exit;
    } else if (sal_strcasecmp("delete", cmd) == 0) {
        rv = bcm_mirror_port_dest_delete(unit, src_port,
                                         mirror_port_flags, mirror_dest_id);
        if (BCM_FAILURE(rv)) {
            cli_out("%s bcm_mirror_port_dest_delete failed %s\n",
                    ARG_CMD(arg), bcm_errmsg(rv));
            cmd_rv = CMD_FAIL;
            goto exit;
        }
        cmd_rv = CMD_OK;
        goto exit;
    }

    cmd_rv = CMD_OK;
    goto exit;

exit:
    if (mirror_dest != NULL) {
        sal_free(mirror_dest);
        mirror_dest = NULL;
    }
    return cmd_rv;
}

/*!
 * \brief Init Mirror module.
 *
 * \param [in] unit Unit number.
 * \param [in] arg Arg structure for parsing arguments.
 *
 * \retval CMD_OK Command completed successfully.
 * \retval CMD_FAIL Command failed.
 */
STATIC cmd_result_t
cmd_ltsw_mirror_init(int unit, args_t *arg)
{
    int rv;

    if (ARG_CNT(arg) > 0) {
        return CMD_USAGE;
    }

    rv = bcm_mirror_init(unit);
    if (rv < 0) {
        cli_out("%s: ERROR: bcm_mirror_init failed %s\n",
                ARG_CMD(arg), bcm_errmsg(rv));
        return CMD_FAIL;
    }

    return CMD_OK;
}

/*!
 * \brief Show Mirror configuration on a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Source port.
 * \param [in] destmod Mirror destination modid.
 * \param [in] destport Mirror destination port.
 * \param [in] flags Mirror flags.
 */
static void
show_mirror_port(int unit, int port, int dstmod, int dstport, uint32 flags)
{
    int show_dest, rv;
    bcm_port_t local_port, port_out;
    bcm_gport_t gport;
    char *mstr;
    bcmi_ltsw_virtual_vp_info_t vp_info;
    char *vptype = "";

    show_dest = 1;
    if (flags & BCM_MIRROR_PORT_INGRESS) {
        if (flags & (BCM_MIRROR_PORT_EGRESS)) {
            mstr = "Mirror all";
        } else {
            mstr = "Mirror ingress";
        }
    } else if (flags & BCM_MIRROR_PORT_EGRESS) {
        mstr = "Mirror egress";
    } else if (flags & BCM_MIRROR_PORT_ENABLE) {
        mstr = "Mirror";
    } else {
        mstr = "-";
        show_dest = 0;
    }

    if (BCM_GPORT_IS_SET(port)) {
        rv = bcmi_ltsw_port_gport_validate(unit, port, &port_out);
        if (rv < 0) {
            cli_out("bcmi_ltsw_port_gport_validate: %s\n",
                    bcm_errmsg(rv));
            return;
        }

        rv = bcmi_ltsw_virtual_vp_info_get(unit, port_out, &vp_info);
        if (rv < 0) {
            cli_out("bcmi_ltsw_virtual_vp_info_get: %s\n",
                    bcm_errmsg(rv));
            return;
        }

        if (vp_info.flags & BCMI_LTSW_VIRTUAL_VP_TYPE_MPLS) {
            vptype = "Mpls";
        } else if (vp_info.flags & BCMI_LTSW_VIRTUAL_VP_TYPE_FLOW) {
            vptype = "Flow";
        }

        cli_out("%s port(%#x): %s", vptype, port, mstr);
    } else {
        cli_out("%4s: %s", bcm_port_name(unit, port), mstr);
    }

    if (show_dest) {
        if (BCM_GPORT_IS_MODPORT(dstport)) {
            gport = dstport;
            dstmod = BCM_GPORT_MODPORT_MODID_GET(gport);
            dstport = BCM_GPORT_MODPORT_PORT_GET(gport);
        } else {
            BCM_GPORT_MODPORT_SET(gport, dstmod, dstport);
        }
        if (bcm_port_local_get(unit, gport, &local_port) != BCM_E_NONE) {
            local_port = -1;
        }
        if (BCM_GPORT_IS_TRUNK(dstport)) {
            cli_out(" to trunk group %d", BCM_GPORT_TRUNK_GET(dstport));
        } else if (flags & BCM_MIRROR_PORT_DEST_TRUNK) {
            cli_out(" to trunk group %d", dstport);
        } else if (local_port >= 0) {
            cli_out(" to local port %s", bcm_port_name(unit, local_port));
        } else {
            cli_out(" to module %d, port %d", dstmod, dstport);
        }
    }
    cli_out("\n");
}

/*!
 * \brief Mirror port cmd handler.
 *
 * \param [in] unit Unit number.
 * \param [in] arg Arg structure for parsing arguments.
 *
 * \retval CMD_OK Command completed successfully.
 * \retval CMD_FAIL Command failed.
 */
STATIC cmd_result_t
cmd_ltsw_mirror_port(int unit, args_t *arg)
{
    parse_table_t pt;
    char *cmd = NULL;
    bcm_pbmp_t pbm;
    bcm_port_t port, destport = -1;
    bcm_port_config_t pcfg;
    int destmod = -1, mode = -1, desttrunk = -1, rv, count = 0;
    char *modeList[] = {"OFF", "ON", "Ingress", "Egress", "All", NULL};
    uint32 flags = 0;

    if ((cmd = ARG_GET(arg)) == NULL) {
        return CMD_USAGE;
    }

    if (bcm_port_config_get(unit, &pcfg) != BCM_E_NONE) {
        cli_out("%s: Error: bcm ports not initialized\n", ARG_CMD(arg));
        return CMD_FAIL;
    }

    if ((ARG_CNT(arg) == 0) && parse_cmp("SHow", cmd, '\0')) {
        BCM_PBMP_ITER(pcfg.all, port) {
            destport = destmod = -1;
            rv = bcm_mirror_port_get(unit, port, &destmod, &destport, &flags);
            if (rv < 0) {
                cli_out("%s: bcm_mirror_port_get: %s\n",
                        ARG_CMD(arg), bcm_errmsg(rv));
                return CMD_FAIL;
            }
            if (flags != 0) {
                count++;
                show_mirror_port(unit, port, destmod, destport, flags);
            }
        }
        if (count == 0) {
            cli_out("%s: No mirror ports configured\n", ARG_CMD(arg));
        }
        return CMD_OK;
    }

    if (parse_bcm_pbmp(unit, cmd, &pbm) < 0) {
        cli_out("%s: Error: unrecognized port bitmap: %s\n",
                ARG_CMD(arg), cmd);
        return CMD_FAIL;
    }
    BCM_PBMP_AND(pbm, pcfg.all);

    if (BCM_PBMP_IS_NULL(pbm)) {
        cli_out("%s: Error: unsupported port bitmap: %s\n",
                ARG_CMD(arg), cmd);
        return CMD_FAIL;
    }

    if (ARG_CNT(arg) == 0) {
        BCM_PBMP_ITER(pbm, port) {
            rv = bcm_mirror_port_get(unit, port, &destmod, &destport, &flags);
            if (rv < 0) {
                cli_out("%s: bcm_mirror_port_get: %s\n", ARG_CMD(arg),
                        bcm_errmsg(rv));
                return CMD_FAIL;
            }
            show_mirror_port(unit, port, destmod, destport, flags);
        }
        return CMD_OK;
    }

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "DestPort", PQ_DFL|PQ_PORT,
                    (void *)(0), &destport, NULL);
    parse_table_add(&pt, "DestModule", PQ_DFL|PQ_INT,
                    (void *)(0), &destmod, NULL);
    parse_table_add(&pt, "DestTrunk", PQ_DFL|PQ_INT,
                    (void *)(0), &desttrunk, NULL);
    parse_table_add(&pt, "Mode", PQ_DFL|PQ_MULTI,
                    (void *)(0), &mode, modeList);

    if (parse_arg_eq(arg, &pt) < 0 || ARG_CNT(arg) > 0) {
        cli_out("%s: Invalid option %s\n", ARG_CMD(arg), ARG_CUR(arg));
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }

    parse_arg_eq_done(&pt);

    if (mode < 0) {
        cli_out("%s: must specify a mode\n", ARG_CMD(arg));
        return CMD_USAGE;
    }

    if ((mode > 0) && (destport < 0) && (desttrunk < 0)) {
        cli_out("%s: missing DestPort or DestTrunk\n", ARG_CMD(arg));
        return CMD_USAGE;
    }

    if (destport > 0 && desttrunk > 0) {
        cli_out("%s: cannot specify both DestPort and DestTrunk\n",
                ARG_CMD(arg));
        return CMD_USAGE;
    }

    switch (mode) {
        case 1:
            flags |= BCM_MIRROR_PORT_ENABLE;
            break;
        case 2:
            flags |= BCM_MIRROR_PORT_INGRESS;
            break;
        case 3:
            flags |= BCM_MIRROR_PORT_EGRESS;
            break;
        case 4:
            flags |= (BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS);
            break;
        default:
            break;
    }

    BCM_PBMP_ITER(pbm, port) {
        rv = bcm_mirror_port_set(unit, port, destmod, destport, flags);

        if (rv < 0) {
            cli_out("%s: bcm_mirror_port_set: %s\n", ARG_CMD(arg),
                    bcm_errmsg(rv));
            return CMD_FAIL;
        }
    }

    return CMD_OK;
}

/*!
 * \brief Show mirror configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] arg Arg structure for parsing arguments.
 *
 * \retval CMD_OK Command completed successfully.
 * \retval CMD_FAIL Command failed.
 */
STATIC cmd_result_t
cmd_ltsw_mirror_show(int unit, args_t *arg)
{
    int rv, dest_size, dest_count = 0;
    bcm_port_t port;
    bcm_pbmp_t pbmp;
    bcm_gport_t gport, mirror_dest[8];
    int vp, i, fc, count = 0, first = TRUE;
    uint32 flags[2] = {BCM_MIRROR_PORT_INGRESS, BCM_MIRROR_PORT_EGRESS};
    uint32 vp_num;
    bcm_mirror_destination_t destination;
    bcmi_ltsw_virtual_vp_info_t vp_info;

    if (ARG_CNT(arg) > 0) {
        return CMD_USAGE;
    }

    rv = bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_ALL, &pbmp);
    if (rv < 0) {
        cli_out("%s: bcmi_ltsw_port_bitmap: %s\n", ARG_CMD(arg),
                bcm_errmsg(rv));
        return CMD_FAIL;
    }

    dest_size = 8;

    BCM_PBMP_ITER(pbmp, port) {
        for (fc = 0; fc < sizeof(flags) / sizeof(uint32); fc++) {
            rv = bcm_mirror_port_dest_get(unit, port, flags[fc], dest_size,
                                          mirror_dest, &dest_count);
            if (rv < 0) {
                cli_out("%s: bcm_mirror_port_dest_get: %s\n",
                        ARG_CMD(arg), bcm_errmsg(rv));
                return CMD_FAIL;
            }

            if (dest_count != 0) {
                for (i = 0; i < dest_count; i++) {
                    bcm_mirror_destination_t_init(&destination);
                    rv = bcm_mirror_destination_get(unit, mirror_dest[i],
                                                    &destination);
                    if (rv < 0) {
                        cli_out("%s: bcm_mirror_destination_get: %s\n",
                                ARG_CMD(arg), bcm_errmsg(rv));
                        return CMD_FAIL;
                    }
                    show_mirror_port(unit, port, 0, destination.gport,
                                     flags[fc]);
                }
                count += dest_count;
            }
        }
    }

    rv = bcmi_ltsw_virtual_vp_num_get(unit, &vp_num);
    if (rv < 0) {
        cli_out("%s: bcmi_ltsw_virtual_vp_num_get: %s\n",
                ARG_CMD(arg), bcm_errmsg(rv));
        return CMD_FAIL;
    }

    for (vp = 0; vp < vp_num; vp++) {
        for (fc = 0; fc < sizeof(flags) / sizeof(uint32); fc++) {
            rv = bcmi_ltsw_virtual_vp_used_get(unit, vp, BCMI_LTSW_VP_TYPE_ANY);
            if (rv == BCM_E_NOT_FOUND) {
                /* Skip unused vp. */
                continue;
            }

            rv = bcmi_ltsw_virtual_vp_info_get(unit, vp, &vp_info);
            if (rv < 0) {
                continue;
            }

            if (vp_info.flags & BCMI_LTSW_VIRTUAL_VP_TYPE_MPLS) {
                BCM_GPORT_MPLS_PORT_ID_SET(gport, vp);
            } else if (vp_info.flags & BCMI_LTSW_VIRTUAL_VP_TYPE_FLOW) {
                BCM_GPORT_FLOW_PORT_ID_SET(gport, vp);
            } else if (vp_info.flags & BCMI_LTSW_VIRTUAL_VP_TYPE_VPLAG) {
                BCM_GPORT_VPLAG_PORT_ID_SET(gport, vp);
            } else {
                continue;
            }

            rv = bcm_mirror_port_dest_get(unit, gport, flags[fc], dest_size,
                                          mirror_dest, &dest_count);
            if (rv < 0) {
                continue;
            }

            if (first) {
                cli_out("-----------------------------------------\n");
                first = FALSE;
            }

            if (dest_count != 0) {
                for (i = 0; i < dest_count; i++) {
                    bcm_mirror_destination_t_init(&destination);
                    rv = bcm_mirror_destination_get(unit, mirror_dest[i],
                                                    &destination);
                    if (rv < 0) {
                        cli_out("%s: bcm_mirror_destination_get: %s\n",
                                ARG_CMD(arg), bcm_errmsg(rv));
                        return CMD_FAIL;
                    }
                    show_mirror_port(unit, gport, 0, destination.gport,
                                     flags[fc]);
                }
                count += dest_count;
            }
        }
    }

    if (count == 0) {
        cli_out("No mirror ports configured\n");
    }
    return CMD_OK;
}

/******************************************************************************
 * Public Functions
 */

/*!
 * \brief Mirror cmd handler.
 *
 * \param [in] unit Unit number.
 * \param [in] arg Arg structure for parsing arguments.
 *
 * \retval CMD_OK Command completed successfully.
 * \retval CMD_FAIL Command failed.
 */
cmd_result_t
cmd_ltsw_mirror(int unit, args_t *arg)
{
    char *subcmd = NULL;

    if ((subcmd = ARG_GET(arg)) == NULL) {
        return CMD_USAGE;
    }

    if (!sal_strcasecmp(subcmd, "DEST")) {
        return cmd_ltsw_mirror_dest(unit, arg);
    } else if (!sal_strcasecmp(subcmd, "init")) {
        return cmd_ltsw_mirror_init(unit, arg);
    } else if (!sal_strcasecmp(subcmd, "Port")) {
        return cmd_ltsw_mirror_port(unit, arg);
    } else if (!sal_strcasecmp(subcmd, "SHow")) {
        return cmd_ltsw_mirror_show(unit, arg);
    } else {
        return CMD_NOTIMPL;
    }
    return CMD_OK;
}

#endif /* BCM_LTSW_SUPPORT */
