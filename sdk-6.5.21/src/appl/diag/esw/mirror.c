/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Mirror CLI commands
 */

#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <appl/diag/dport.h>
#include <shared/bsl.h>
#include <bcm/error.h>
#include <bcm/mirror.h>
#include <bcm/stack.h>
#include <bcm/debug.h>
#include <bcm_int/control.h>
#include <bcm_int/esw/mirror.h>

#ifdef BCM_TRIUMPH2_SUPPORT
#include <bcm_int/esw/triumph2.h>
#ifdef BCM_TRIDENT_SUPPORT
#include <bcm_int/esw/trident.h>
#endif
#ifdef BCM_TRIUMPH3_SUPPORT
#include <bcm_int/esw/triumph3.h>
#endif
#ifdef BCM_TRIUMPH_SUPPORT
#include <bcm_int/esw/triumph.h>
#endif
#ifdef BCM_MPLS_SUPPORT
#include <bcm_int/esw/mpls.h>
#endif
#endif


static int
show_mirror_destination_cb(int unit, bcm_mirror_destination_t * mirror_dest,
                           void *user_data)
{
    int        *count = (int *) user_data;

    (*count)++;
    cli_out("Mirror Dest %2d: ", *count);
    cli_out("Mirror ID: %2d; ", BCM_GPORT_MIRROR_GET(mirror_dest->mirror_dest_id));
    if (mirror_dest->flags & BCM_MIRROR_DEST_TUNNEL_L2) {
        cli_out("%s ", "L2 Tunnel to");
    }
    if ((mirror_dest->flags & BCM_MIRROR_DEST_TUNNEL_IP_GRE) ||
        (soc_feature(unit, soc_feature_sflow_ing_mirror) &&
         (mirror_dest->flags & BCM_MIRROR_DEST_TUNNEL_SFLOW))) {

        if (mirror_dest->flags & BCM_MIRROR_DEST_TUNNEL_IP_GRE) {
            cli_out("IPv%d GRE Tunnel\n", mirror_dest->version);
        } else {
            cli_out("Sflow IPv%d Tunnel\n", mirror_dest->version);
        }

        if (4 == mirror_dest->version) {
            char        buffer[SAL_IPADDR_STR_LEN];

            format_ipaddr(buffer, mirror_dest->src_addr);
            cli_out("\tSrc IP=%s\n", buffer);
            format_ipaddr(buffer, mirror_dest->dst_addr);
            cli_out("\tDst IP=%s\n", buffer);
        } else if (6 == mirror_dest->version) {
            char        buffer[IP6ADDR_STR_LEN];

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
        cli_out("\tdst_mac=%02x:%02x:%02x:%02x:%02x:%02x\n", mirror_dest->dst_mac[0],
                mirror_dest->dst_mac[1], mirror_dest->dst_mac[2],
                mirror_dest->dst_mac[3], mirror_dest->dst_mac[4],
                mirror_dest->dst_mac[5]);

        cli_out("\tvlan_id=0x%04x\n", mirror_dest->vlan_id);
        cli_out("\ttpid=0x%04x\n", mirror_dest->tpid);
        cli_out("\tttl=%d\n", mirror_dest->ttl);
        cli_out("\ttos=%d\n", mirror_dest->tos);
	cli_out("\tmirrored to ");
    }
    if (mirror_dest->flags & BCM_MIRROR_DEST_PAYLOAD_UNTAGGED) {
        cli_out("%s ", "Payload Untagged");
    }
    if (mirror_dest->flags & BCM_MIRROR_DEST_TUNNEL_TRILL) {
        cli_out("%s ", "Trill Tunnel to");
    }
    if (mirror_dest->flags & BCM_MIRROR_DEST_TUNNEL_NIV) {
        cli_out("%s ", "NIV Tunnel to");
    }
    if (mirror_dest->flags & BCM_MIRROR_DEST_TUNNEL_ETAG) {
        cli_out("%s ", "ETAG Tunnel to");
    }

    if (BCM_GPORT_IS_LOCAL(mirror_dest->gport)) {
        cli_out("local port %s",
                BCM_PORT_NAME(unit, BCM_GPORT_LOCAL_GET(mirror_dest->gport)));
    } else if (BCM_GPORT_IS_MODPORT(mirror_dest->gport)) {
        bcm_port_t  local_port;

        if (bcm_port_local_get(unit, mirror_dest->gport, &local_port) != BCM_E_NONE) {
            cli_out("modport %d/%d", BCM_GPORT_MODPORT_MODID_GET(mirror_dest->gport),
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

cmd_result_t
_if_esw_mirror_dest(int unit, args_t *a)
{
    parse_table_t               pt;
    cmd_result_t                retCode;
    bcm_gport_t                 mirror_dest_id = 0;
    bcm_port_t                  src_port = 0, dest_port = 0;
    bcm_ip_t                    src_ip = 0, dest_ip = 0;
    bcm_ip6_t                   src_ip6,dest_ip6;
    uint32                      flow_lable;
    uint32                      mirror_port_flags = 0;
    bcm_mac_t                   src_mac, dest_mac;
    bcm_vlan_t                  vid = BCM_VLAN_INVALID;
    uint16                      tpid;
    int                         vlan_id = -1, tag_id = -1;
    int                         ttl = 0,tos = 0, version = 0;
    int                         rv, mode = 0, tunnel = 0, strip_vlan = 0;
    bcm_mirror_destination_t    mirror_dest;
    char                        *cmd = NULL;
    char *modeList[] = {"OFF", "Ingress", "Egress", "EgressTrue", "IngressEgress", NULL};
    char *tunnelList[] = {"OFF","L2", "GRE", NULL};

    if ((cmd = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }
    if (0 == sal_strcasecmp("show", cmd)) {
        int         md_count = 0;

        rv = bcm_mirror_destination_traverse(unit, show_mirror_destination_cb,
                                             &md_count);
        if (BCM_FAILURE(rv)) {
            cli_out("%s bcm_mirror_destination_traverse() failed  %s\n",
                    ARG_CMD(a), bcm_errmsg(rv));
            return CMD_FAIL;
        }
	if (md_count == 0) {
	    cli_out("No mirror destinations currently defined\n");
	}
        return CMD_OK;
    }

    sal_memset(src_ip6, 0, sizeof(bcm_ip6_t));
    sal_memset(dest_ip6, 0, sizeof(bcm_ip6_t));
    sal_memset(src_mac, 0, sizeof(bcm_mac_t));
    sal_memset(dest_mac, 0, sizeof(bcm_mac_t));
    
    parse_table_init(unit, &pt);
    
    parse_table_add(&pt, "Id", PQ_DFL | PQ_INT,0, &mirror_dest_id, NULL);
    parse_table_add(&pt, "Mode", PQ_DFL | PQ_MULTI,(void *)(0), &mode, modeList);
    parse_table_add(&pt, "Tunnel", PQ_DFL | PQ_MULTI,(void *)(0), &tunnel, tunnelList);
    parse_table_add(&pt, "SrcPort", PQ_DFL | PQ_PORT, 0, &src_port, NULL);
    parse_table_add(&pt, "DestPort", PQ_DFL | PQ_PORT, 0, &dest_port, NULL);
    parse_table_add(&pt, "SrcIP", PQ_DFL | PQ_IP, 0, (void *)&src_ip, NULL);
    parse_table_add(&pt, "DestIP", PQ_DFL | PQ_IP, 0, (void *)&dest_ip, NULL);
    parse_table_add(&pt, "SrcIP6", PQ_DFL | PQ_IP6, 0, (void *)&src_ip6, NULL);
    parse_table_add(&pt, "DestIP6", PQ_DFL | PQ_IP6, 0, (void *)&dest_ip6, NULL);
    parse_table_add(&pt, "SrcMAC", PQ_DFL|PQ_MAC, 0, (void *)src_mac, NULL);
    parse_table_add(&pt, "DestMAC", PQ_DFL|PQ_MAC, 0, (void *)dest_mac, NULL);
    parse_table_add(&pt, "VLAN", PQ_DFL|PQ_INT, 0, (void *)&vlan_id, NULL);
    parse_table_add(&pt, "tpid", PQ_DFL|PQ_INT, 0, (void *)&tag_id, NULL);
    parse_table_add(&pt, "Version",  PQ_DFL|PQ_INT, 0, (void*)&version, NULL);
    parse_table_add(&pt, "TTL",  PQ_DFL|PQ_INT, 0, (void*)&ttl, NULL);
    parse_table_add(&pt, "TOS",  PQ_DFL|PQ_INT, 0, (void*)&tos, NULL);
    parse_table_add(&pt, "FlowLable",  PQ_DFL|PQ_INT, 0, (void*)&flow_lable, NULL);
    parse_table_add(&pt, "NoVLAN",  PQ_DFL|PQ_BOOL, 0, (void*)&strip_vlan, NULL);

    if (!parseEndOk( a, &pt, &retCode)) {
	return retCode;
    }

    vid = (bcm_vlan_t) (vlan_id & 0xffff);
    tpid = (uint16) (tag_id & 0xffff);
    bcm_mirror_destination_t_init(&mirror_dest);

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
            mirror_dest.flags |= BCM_MIRROR_DEST_TUNNEL_L2;
            sal_memcpy(mirror_dest.src_mac, src_mac, sizeof(src_mac));
            sal_memcpy(mirror_dest.dst_mac, dest_mac, sizeof(dest_mac));
            mirror_dest.vlan_id = vid;
            mirror_dest.tpid = tpid;
            break;
        case 2:
            mirror_dest.flags |= BCM_MIRROR_DEST_TUNNEL_IP_GRE;
            sal_memcpy(mirror_dest.src_mac, src_mac, sizeof(src_mac));
            sal_memcpy(mirror_dest.dst_mac, dest_mac, sizeof(dest_mac));
            mirror_dest.vlan_id = vid;
            mirror_dest.tpid = tpid;
            mirror_dest.src_addr = src_ip;
            mirror_dest.dst_addr = dest_ip;
            mirror_dest.ttl = ttl;
            mirror_dest.tos = tos;
            mirror_dest.flow_label = flow_lable;
            mirror_dest.version = version;
            sal_memcpy(mirror_dest.src6_addr, src_ip6, sizeof(src_ip6));
            sal_memcpy(mirror_dest.dst6_addr, dest_ip6, sizeof(dest_ip6));
            break;
        default:
            break;
    }

    if (strip_vlan) {
        mirror_dest.flags |= BCM_MIRROR_DEST_PAYLOAD_UNTAGGED;
    }

    if (!BCM_GPORT_IS_SET(dest_port)) {
        rv = bcm_port_gport_get(unit, dest_port, &dest_port);
        if (BCM_FAILURE(rv)) {
            cli_out("Illegal port [%d]\n", dest_port);
            return CMD_FAIL;
        }
    }

    /* Dest port can be trunk, api handles this with no flags */
    mirror_dest.gport = dest_port;

    if (mirror_dest_id != 0) {
        mirror_dest.flags |= BCM_MIRROR_DEST_WITH_ID;
        mirror_dest.mirror_dest_id = mirror_dest_id;
    }

    if (0 == sal_strcasecmp("create", cmd)) {
        rv = bcm_mirror_destination_create(unit, &mirror_dest);
        if (BCM_FAILURE(rv)) {
            cli_out("%s bcm_mirror_destination_create() failed  %s\n", 
                    ARG_CMD(a), bcm_errmsg(rv));
            return CMD_FAIL;
        }
        cli_out(" Mirror Id = 0x%x\n", mirror_dest.mirror_dest_id);
        return CMD_OK;
    } else if (0 == sal_strcasecmp("add", cmd)) {
        rv = bcm_mirror_port_dest_add(unit, src_port,
                                      mirror_port_flags, mirror_dest_id);
        if (BCM_FAILURE(rv)) {
            cli_out("%s bcm_mirror_port_dest_add() failed  %s\n", 
                    ARG_CMD(a), bcm_errmsg(rv));
            return CMD_FAIL;
        }
        return CMD_OK;
    } else if (0 == sal_strcasecmp("destroy", cmd)) {
        if (0 == mirror_dest_id) {
            cli_out("%s mirror destination id is missing\n", ARG_CMD(a));
            return CMD_FAIL;
        }
        rv = bcm_mirror_destination_destroy(unit, mirror_dest_id);
        if (BCM_FAILURE(rv)) {
            cli_out("%s bcm_mirror_destination_destroy failed()  %s\n", 
                    ARG_CMD(a), bcm_errmsg(rv));
            return CMD_FAIL;
        }
        return CMD_OK;
    } else if (0 == sal_strcasecmp("delete", cmd)) {
        rv = bcm_mirror_port_dest_delete(unit, src_port,
                                         mirror_port_flags, mirror_dest_id);
        if (BCM_FAILURE(rv)) {
            cli_out("%s bcm_mirror_port_dest_delete failed %s\n", 
                    ARG_CMD(a), bcm_errmsg(rv));
            return CMD_FAIL;
        }
        return CMD_OK;
    }

    return CMD_USAGE;
}

static void _dmirror_show(int unit, int port, int dstmod, int dstport, uint32 flags);
static int
_mirror_show(int unit)
{
    const uint32 flags[3] = { BCM_MIRROR_PORT_INGRESS, BCM_MIRROR_PORT_EGRESS,
                              BCM_MIRROR_PORT_EGRESS_TRUE };
    bcm_gport_t mirror_dest[BCM_MIRROR_MTP_COUNT];
    bcm_port_config_t pcfg;
    int         count = 0;
    int         fc;
    int         mirror_dest_count;
    int         port, dport;
    int         rv;
#if defined(INCLUDE_L3) && defined(BCM_TRIUMPH2_SUPPORT)
    bcm_gport_t gport = 0;
    int         vp = 0;
    int         first = TRUE;
#endif

    if (bcm_port_config_get(unit, &pcfg) != BCM_E_NONE) {
        cli_out("Error: bcm ports not initialized\n");
        return CMD_FAIL;
    }

    /* coverity[overrun-local] */
    DPORT_BCM_PBMP_ITER(unit, pcfg.all, dport, port) {
        for (fc = 0; fc < sizeof(flags) / sizeof(uint32); fc++) {
            if(flags[fc] == BCM_MIRROR_PORT_EGRESS_TRUE) {
                if(!soc_feature(unit, soc_feature_egr_mirror_true))
                    continue;
            }
            rv =
              bcm_mirror_port_dest_get(unit,
                                       port,
                                       flags[fc],
                                       BCM_MIRROR_MTP_COUNT, mirror_dest,
                                       &mirror_dest_count);
            if (rv < 0) {
                cli_out("bcm_mirror_port_dest_get() FAILED: %s\n", bcm_errmsg(rv));
                return CMD_FAIL;
            }
            if (mirror_dest_count != 0) {
                int         i;

                for (i = 0; i < mirror_dest_count; i++) {
                    bcm_mirror_destination_t mirror_dest_desc;

                    bcm_mirror_destination_t_init(&mirror_dest_desc);
                    mirror_dest_desc.flags = flags[fc];
                    rv =
                      bcm_mirror_destination_get(unit, mirror_dest[i],
                                                 &mirror_dest_desc);
                    if (rv < 0) {
                        cli_out("bcm_mirror_destination_get() FAILED: %s\n",
                                bcm_errmsg(rv));
                        return CMD_FAIL;
                    }
                    _dmirror_show(unit, port, 0, mirror_dest_desc.gport, flags[fc]);
                }
                count += mirror_dest_count;
            }
        }
    }
        
#if defined(INCLUDE_L3) && defined(BCM_TRIUMPH2_SUPPORT)
    if (SOC_MEM_IS_VALID(unit,SOURCE_VPm)) {
        for (vp = soc_mem_index_min(unit, SOURCE_VPm);
             vp <= soc_mem_index_max(unit, SOURCE_VPm);
             vp++) {
            gport = 0;
            BCM_GPORT_VP_GROUP_SET(gport, vp);
            
            for (fc = 0; fc < sizeof(flags) / sizeof(uint32); fc++) {
                if(flags[fc] == BCM_MIRROR_PORT_EGRESS_TRUE) {
                    if(!soc_feature(unit, soc_feature_egr_mirror_true))
                        continue;
                }
                rv =
                  bcm_mirror_port_dest_get(unit,
                                           gport,
                                           flags[fc],
                                           BCM_MIRROR_MTP_COUNT, mirror_dest,
                                           &mirror_dest_count);
                if (BCM_E_INTERNAL == rv) {
                    continue;
                } else if (rv < 0) {
                    cli_out("bcm_mirror_port_dest_get() FAILED: %s\n", bcm_errmsg(rv));
                    return CMD_FAIL;
                }
                
                if (first) {
                    cli_out("-----------------------------------------\n");
                    first = FALSE;
                }
                
                if (mirror_dest_count != 0) {
                    int         i;

                    for (i = 0; i < mirror_dest_count; i++) {
                        bcm_mirror_destination_t mirror_dest_desc;

                        bcm_mirror_destination_t_init(&mirror_dest_desc);
                        mirror_dest_desc.flags = flags[fc];
                        rv =
                          bcm_mirror_destination_get(unit, mirror_dest[i],
                                                     &mirror_dest_desc);
                        if (rv < 0) {
                            cli_out("bcm_mirror_destination_get() FAILED: %s\n",
                                    bcm_errmsg(rv));
                            return CMD_FAIL;
                        }
                        _dmirror_show(unit, gport, 0, mirror_dest_desc.gport, flags[fc]);
                    }
                    count += mirror_dest_count;
                }
            }
        }
    }
#endif
    if (count == 0) {
        cli_out("No mirror ports configured\n");
    }
    return CMD_OK;
}

cmd_result_t
if_esw_mirror(int unit, args_t *a)
{
    int		rv;
    soc_port_t	port, dport;
    char	pfmt[SOC_PBMP_FMT_LEN];
    char	bmstr[FORMAT_PBMP_MAX];
    char *modeList[] = {"Off", "L2", "L3", NULL};
    int		argMode = -1, argPort = -1;
    bcm_pbmp_t argIngBmp, argEgrBmp;
    bcm_port_config_t   pcfg;
    parse_table_t	pt;
    cmd_result_t	retCode;
    char            *subcmd = NULL;

    BCM_PBMP_CLEAR(argIngBmp);
    BCM_PBMP_CLEAR(argEgrBmp);

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    if ((subcmd = ARG_GET(a)) != NULL) {
        if (parse_cmp("DEST", subcmd, '\0')) {
            return _if_esw_mirror_dest(unit, a);
        } else if ((ARG_CNT(a) == 0) && parse_cmp("SHow", subcmd, '\0')) {
            /* coverity[stack_use_overflow : FALSE] */
	        return _mirror_show(unit);
	    }
        /* no sub command going down, all arguments are treated as mirror parameter
           decrement processed parameter count by 1 to check it properly */
        ARG_PREV(a);
    }

    if (bcm_port_config_get(unit, &pcfg) != BCM_E_NONE) {
        cli_out("%s: Error: bcm ports not initialized\n", ARG_CMD(a));
        return CMD_FAIL;
    }

    if (!SOC_IS_XGS12_FABRIC(unit) ) {
	rv = bcm_mirror_to_get(unit, &argPort);
	if (rv < 0) {
	    cli_out("%s: bcm_mirror_to_get() failed: %s\n", ARG_CMD(a),
                    bcm_errmsg(rv));
	    return CMD_FAIL;
	}

	rv = bcm_mirror_mode_get(unit, &argMode);
	if (rv < 0) {
	    cli_out("%s: bcm_mirror_mode_get() failed: %s\n", ARG_CMD(a),
                    bcm_errmsg(rv));
	    return CMD_FAIL;
	}

	BCM_PBMP_CLEAR(argIngBmp);
	BCM_PBMP_CLEAR(argEgrBmp);

	/* coverity[overrun-local] */
        DPORT_BCM_PBMP_ITER(unit, pcfg.all, dport, port) {
	    int enable;

	    rv = bcm_mirror_ingress_get(unit, port, &enable);
	    /* coverity[overrun-local] */
            if (rv < 0 && !IS_CPU_PORT(unit, port)) {
		cli_out("%s: bcm_mirror_ingress_get() failed: %s\n", ARG_CMD(a),
                        bcm_errmsg(rv));
		return CMD_FAIL;
	    }

	    if (enable) {
		/* coverity[overrun-local] */
                BCM_PBMP_PORT_ADD(argIngBmp, port);
	    }

	    rv = bcm_mirror_egress_get(unit, port, &enable);
	    if (rv < 0 && !IS_CPU_PORT(unit, port)) {
		cli_out("%s: bcm_mirror_egress_get() failed: %s\n", ARG_CMD(a),
                        bcm_errmsg(rv));
		return CMD_FAIL;
	    }

	    if (enable) {
		/* coverity[overrun-local] */
                BCM_PBMP_PORT_ADD(argEgrBmp, port);
	    }
	}

	parse_table_init(unit, &pt);
	parse_table_add(&pt, "Mode", PQ_DFL|PQ_MULTI,
			(void *)(0), &argMode, modeList);
	parse_table_add(&pt, "Port", PQ_DFL|PQ_PORT|PQ_BCM,
			(void *)(0), &argPort, NULL);
	parse_table_add(&pt, "IngressBitMap", PQ_DFL|PQ_PBMP|PQ_BCM,
			(void *)(0), &argIngBmp, NULL);
	parse_table_add(&pt, "EgressBitMap", PQ_DFL|PQ_PBMP|PQ_BCM,
			(void *)(0), &argEgrBmp, NULL);
	if (!parseEndOk( a, &pt, &retCode)) {
	    return retCode;
	}

	rv = bcm_mirror_to_set(unit, argPort);
	if (rv < 0) {
	    cli_out("%s: bcm_mirror_to_set() failed: %s\n", ARG_CMD(a),
                    bcm_errmsg(rv));
	    return CMD_FAIL;
	}

	switch (argMode) {
	case 0:
	    rv = bcm_mirror_mode_set(unit, BCM_MIRROR_DISABLE);
	    break;
	case 1:
	    rv = bcm_mirror_mode_set(unit, BCM_MIRROR_L2);
	    break;
	case 2:
	    rv = bcm_mirror_mode_set(unit, BCM_MIRROR_L2_L3);
	    break;
	}

	if (rv < 0) {
	    cli_out("%s: bcm_mirror_mode_set() failed: %s\n", ARG_CMD(a),
                    bcm_errmsg(rv));
	    return CMD_FAIL;
	}

	if (argMode == 0) {	/* skip ingress/egress for disable case */
	    return CMD_OK;
	}

	/* coverity[overrun-local] */
        DPORT_BCM_PBMP_ITER(unit, pcfg.all, dport, port) {
	    rv = bcm_mirror_ingress_set(unit, port,
					BCM_PBMP_MEMBER(argIngBmp, port) ? 1 : 0);
	    if (rv < 0 && !BCM_PBMP_MEMBER(pcfg.cpu, port)) {
		cli_out("%s: bcm_mirror_ingress_set() failed: %s\n", ARG_CMD(a),
                        bcm_errmsg(rv));
		return CMD_FAIL;
	    }
	    rv = bcm_mirror_egress_set(unit, port,
				       BCM_PBMP_MEMBER(argEgrBmp, port) ? 1 : 0);
	    if (rv < 0 && !BCM_PBMP_MEMBER(pcfg.cpu, port)) {
		cli_out("%s: bcm_mirror_egress_set failed: %s\n", ARG_CMD(a),
                        bcm_errmsg(rv));
		return CMD_FAIL;
	    }
	}

	return CMD_OK;
    } else {
	/* Hercules chip */

	if (ARG_CNT(a) == 0) {
	    /* Dump mirroring bitmaps */
	    cli_out("Mirroring configuration:\n");
            /* coverity[overrun-local] */
            DPORT_BCM_PBMP_ITER(unit, pcfg.port, dport, port) {
		if ( (rv = bcm_mirror_to_pbmp_get(unit,
						  port, &argIngBmp)) <  0){
		    cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(rv));
		    return CMD_FAIL;
		}
		format_bcm_pbmp(unit, bmstr, sizeof(bmstr), argIngBmp);
		cli_out("\t%s\tmirror bitmap %s, %s\n",
                        BCM_PORT_NAME(unit, port),
                        SOC_PBMP_FMT(argIngBmp, pfmt),
                        bmstr);
	    }
	} else {
	    parse_table_init(unit, &pt);
	    parse_table_add(&pt, "Port", PQ_DFL|PQ_PORT|PQ_BCM,
			    (void *)( 0), &argPort, NULL);
	    parse_table_add(&pt, "BitMap", PQ_DFL|PQ_PBMP|PQ_BCM,
			    (void *)(0), &argIngBmp, NULL);
	    if (!parseEndOk( a, &pt, &retCode)) {
		return retCode;
	    }

	    if ( (rv = bcm_mirror_to_pbmp_set(unit, argPort, argIngBmp)) < 0) {
		cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(rv));
		return CMD_FAIL;
	    }
	}
    }

    return CMD_OK;
}

static void
_dmirror_show(int unit, int port, int dstmod, int dstport, uint32 flags)
{
    int rv, show_dest;
    bcm_port_t local_port;
    bcm_gport_t gport;
    char *mstr;
    char *vptype = "";
    uint16 tpid, vlan;
    int vp = 0;

    show_dest = 1;
    if (flags & BCM_MIRROR_PORT_INGRESS) {
        if (flags & (BCM_MIRROR_PORT_EGRESS|BCM_MIRROR_PORT_EGRESS_TRUE)) {
            mstr = "Mirror all";
        } else {
            mstr = "Mirror ingress";
        }
    } else if (flags & BCM_MIRROR_PORT_EGRESS) {
        mstr = "Mirror egress";
    } else if (flags & BCM_MIRROR_PORT_EGRESS_TRUE) {
        mstr = "Mirror egress true";
    } else if (flags & BCM_MIRROR_PORT_ENABLE) {
        mstr = "Mirror";
    } else {
        mstr = "-";
        show_dest = 0;
    }

    if (BCM_GPORT_IS_SET(port)) {
#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
        bcm_module_t mod_out;
        bcm_trunk_t trunk_id;
        int id;
        bcm_vlan_port_t vlan_vp;

        vp = BCM_GPORT_VP_GROUP_GET(port);

        BCM_GPORT_VLAN_PORT_ID_SET(port, vp);
        bcm_vlan_port_t_init(&vlan_vp);
        vlan_vp.vlan_port_id = port;
        rv = bcm_vlan_port_find(unit, &vlan_vp);
        if (BCM_E_NONE == rv) {
            (void)_bcm_esw_gport_resolve(unit, vlan_vp.port, &mod_out,
                &port, &trunk_id, &id);
            vptype = "Vlan";
        }
#ifdef BCM_TRIDENT_SUPPORT
        if (rv != BCM_E_NONE) {
            bcm_niv_port_t niv_port;
    
            BCM_GPORT_NIV_PORT_ID_SET(port, vp);
            bcm_niv_port_t_init(&niv_port);
            niv_port.niv_port_id = port;
            rv = bcm_niv_port_get(unit, &niv_port);
            if (BCM_E_NONE == rv) {
                (void)_bcm_esw_gport_resolve(unit, niv_port.port, &mod_out,
                    &port, &trunk_id, &id);
                vptype = "Niv";
            }
        }
#endif /* BCM_TRIDENT_SUPPORT */
#ifdef BCM_TRIUMPH3_SUPPORT
        if (rv != BCM_E_NONE) {
            bcm_extender_port_t extender_port;
    
            BCM_GPORT_EXTENDER_PORT_ID_SET(port, vp);
            bcm_extender_port_t_init(&extender_port);
            extender_port.extender_port_id = port;
            rv = bcm_extender_port_get(unit, &extender_port);
            if (BCM_E_NONE == rv) {
                (void)_bcm_esw_gport_resolve(unit, extender_port.port, &mod_out,
                    &port, &trunk_id, &id);
                vptype = "Extender";
            }
        }
#endif /* BCM_TRIUMPH3_SUPPORT */
#ifdef BCM_MPLS_SUPPORT
        if (rv != BCM_E_NONE) {
            bcm_mpls_port_t mpls_port;
            int vpn0;
     
            BCM_GPORT_MPLS_PORT_ID_SET(port, vp);
            bcm_mpls_port_t_init(&mpls_port);
            mpls_port.mpls_port_id = port;
            _BCM_MPLS_VPN_SET(vpn0,_BCM_MPLS_VPN_TYPE_VPWS,0);
            rv = bcm_mpls_port_get(unit, vpn0, &mpls_port);
            if (BCM_E_NONE == rv) {
                (void)_bcm_esw_gport_resolve(unit, mpls_port.port, &mod_out,
                    &port, &trunk_id, &id);
                vptype = "Mpls";
            }
        }
#endif  /* BCM_MPLS_SUPPORT */
#endif
        cli_out("%s vp%d(%4s): %s", vptype, vp, BCM_PORT_NAME(unit, port), mstr);
    } else {
        cli_out("%4s: %s", BCM_PORT_NAME(unit, port), mstr);
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
            cli_out(" to local port %s", BCM_PORT_NAME(unit, local_port));
        } else {
            cli_out(" to module %d, port %d", dstmod, dstport);
        }
        if (local_port >= 0) {
            rv = bcm_mirror_vlan_get(unit, local_port, &tpid, &vlan);
            if (rv == BCM_E_NONE && vlan > 0) {
                cli_out(" (TPID=0x%04X(%d), VLAN=0x%04X(%d))", tpid, tpid, vlan, vlan);
            }
        }
    }
    cli_out("\n");
}

char if_dmirror_usage[] =
    "Parameters: <ports> [Mode=<Off|On|Ingress|Egress|All>]\n"
    "\t[DestPort=<port>] [DestModule=<modid>] [DestTrunk=<tgid>]\n"
    "\t[MirrorTagProtocolID=<tpid>] [MirrorVlanID=<vid>\n"
    "\tDestTrunk overrides DestModule/DestPort.\n"
    "\tTPID and VLAN are set only if DestPort is a local port.\n";

cmd_result_t
if_dmirror(int unit, args_t *a)
{
    int	rv;
    int port, dport, dstmod, dstport;
    int mymodid;
    uint32 flags;
    parse_table_t pt;
    cmd_result_t retCode;
    bcm_pbmp_t pbm, tmp_pbm;
    bcm_port_config_t pcfg;
    char *mstr;
    int argDestPort = -1, argDestMod = -1, argDestTrunk = -1;
    int argMode = -1, argTpid = -1, argVlan = -1;

    char *modeList[] = {"OFF", "ON", "Ingress", "Egress", "All", NULL};

    if (!sh_check_attached(ARG_CMD(a), unit)) {
	return CMD_FAIL;
    }

    if (SOC_IS_XGS12_FABRIC(unit) ) {
        cli_out("%s: command not supported for switch fabric chips\n", ARG_CMD(a));
        return CMD_FAIL;
    }

    if (bcm_port_config_get(unit, &pcfg) != BCM_E_NONE) {
        cli_out("%s: Error: bcm ports not initialized\n", ARG_CMD(a));
        return CMD_FAIL;
    }

    rv = bcm_stk_my_modid_get(unit, &mymodid);
    if (rv < 0) {
        cli_out("%s: bcm_stk_my_modid_get: %s\n", ARG_CMD(a),
                bcm_errmsg(rv));
        return CMD_FAIL;
    }

    if ((mstr = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

    if ((ARG_CNT(a) == 0) && parse_cmp("SHow", mstr, '\0')) {
        int         count = 0;

        /* coverity[overrun-local] */
        DPORT_BCM_PBMP_ITER(unit, pcfg.all, dport, port) {
            rv = bcm_mirror_port_get(unit, port, &dstmod, &dstport, &flags);
            if (rv < 0) {
                cli_out("%s: bcm_mirror_port_get: %s\n", ARG_CMD(a), bcm_errmsg(rv));
                return CMD_FAIL;
            }
            if (flags != 0) {
                count++;
                _dmirror_show(unit, port, dstmod, dstport, flags);
            }
        }
        if (count == 0) {
            cli_out("%s: No mirror ports configured\n", ARG_CMD(a));
        }
        return CMD_OK;
    }
    if (parse_bcm_pbmp(unit, mstr, &pbm) < 0) {
        cli_out("%s: Error: unrecognized port bitmap: %s\n",
                ARG_CMD(a), mstr);
        return CMD_FAIL;
    }

    if (soc_feature(unit, soc_feature_cpuport_mirror)) {
        BCM_PBMP_AND(pbm, pcfg.all);
    } else {
        BCM_PBMP_AND(pbm, pcfg.port);
    }

    if (BCM_PBMP_IS_NULL(pbm)) {
        cli_out("%s: Error: unsupported port bitmap: %s\n",
                ARG_CMD(a), mstr);
        return CMD_FAIL;
    }

    if (ARG_CNT(a) == 0) {
        /* Show FE ports separately (for Tucana) */
        tmp_pbm = pbm;
        BCM_PBMP_AND(tmp_pbm, pcfg.fe);
        /* coverity[overrun-local] */
        DPORT_BCM_PBMP_ITER(unit, tmp_pbm, dport, port) {
            rv = bcm_mirror_port_get(unit, port, &dstmod, &dstport, &flags);
            if (rv < 0) {
                cli_out("%s: bcm_mirror_port_get: %s\n", ARG_CMD(a),
                        bcm_errmsg(rv));
                return CMD_FAIL;
            }
            _dmirror_show(unit, port, dstmod, dstport, flags);
        }
        /* Show non-FE ports */
        tmp_pbm = pbm;
        BCM_PBMP_REMOVE(tmp_pbm, pcfg.fe);
        DPORT_BCM_PBMP_ITER(unit, tmp_pbm, dport, port) {
            rv = bcm_mirror_port_get(unit, port, &dstmod, &dstport, &flags);
            if (rv < 0) {
                cli_out("%s: bcm_mirror_port_get: %s\n", ARG_CMD(a),
                        bcm_errmsg(rv));
                return CMD_FAIL;
            }
            _dmirror_show(unit, port, dstmod, dstport, flags);
        }
        return CMD_OK;
    }

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "DestPort", PQ_DFL|PQ_PORT,
                    (void *)(0), &argDestPort, NULL);
    parse_table_add(&pt, "DestModule", PQ_DFL|PQ_INT,
                    (void *)(0), &argDestMod, NULL);
    parse_table_add(&pt, "DestTrunk", PQ_DFL|PQ_INT,
                    (void *)(0), &argDestTrunk, NULL);
    parse_table_add(&pt, "MirrorTagProtocolID", PQ_DFL|PQ_INT,
                    (void *)(0), &argTpid, NULL);
    parse_table_add(&pt, "MirrorVlanID", PQ_DFL|PQ_INT,
                    (void *)(0), &argVlan, NULL);
    parse_table_add(&pt, "Mode", PQ_DFL|PQ_MULTI,
                    (void *)(0), &argMode, modeList);
    if (!parseEndOk( a, &pt, &retCode)) {
        return retCode;
    }

    flags = 0;

    if (argMode < 0) {
        cli_out("%s: must specify a mode\n", ARG_CMD(a));
        return CMD_USAGE;
    }

    if ((argMode > 0) && (argDestPort < 0) && (argDestTrunk < 0)) {
        cli_out("%s: missing DestPort or DestTrunk\n", ARG_CMD(a));
        return CMD_USAGE;
    }
    if ((argDestPort >= 0) && (argDestTrunk >= 0)) {
        cli_out("%s: cannot specify both DestPort and DestTrunk\n", ARG_CMD(a));
        return CMD_USAGE;
    }

    if (argDestTrunk < 0) {
        dstport = argDestPort;
    } else {
        dstport = argDestTrunk;
        flags |= BCM_MIRROR_PORT_DEST_TRUNK;
    }

    switch (argMode) {
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

    DPORT_BCM_PBMP_ITER(unit, pbm, dport, port) {
        rv = bcm_mirror_port_set(unit, port, argDestMod, dstport, flags);

        if (rv < 0) {
            cli_out("%s: bcm_mirror_port_set: %s\n", ARG_CMD(a),
                    bcm_errmsg(rv));
            return CMD_FAIL;
        }

        if (argDestMod < 0 || argDestMod == mymodid) {
            if (argTpid >= 0 && argVlan >= 0 && argDestTrunk < 0) {
                rv = bcm_mirror_vlan_set(unit, dstport, argTpid, argVlan);

                if (rv < 0) {
                    cli_out("%s: bcm_mirror_vlan_set failed: %s\n", ARG_CMD(a),
                            bcm_errmsg(rv));
                    return CMD_FAIL;
                }
            }
        }
    }

    return CMD_OK;
}
