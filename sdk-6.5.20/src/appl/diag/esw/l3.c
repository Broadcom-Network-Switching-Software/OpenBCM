/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * L3 CLI commands
 */

#include <shared/bsl.h>

#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <appl/diag/dport.h>
#include <appl/diag/shell.h>
#include <appl/diag/bslcons.h>
#include <shared/bsl.h>
#include <soc/debug.h>
#include <soc/hash.h>
#if defined(BCM_TRIDENT2_SUPPORT)
#include <soc/trident2.h>
#endif /* BCM_TRIDENT2_SUPPORT */

#ifdef ALPM_ENABLE
#include <soc/alpm.h>
#endif

#include <bcm/error.h>
#include <bcm/l3.h>
#include <bcm/tunnel.h>
#include <bcm/ipmc.h>
#include <bcm/debug.h>
#include <bcm/nat.h>

#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/nat.h>
#include <bcm_int/esw/katana.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/esw/port.h>

#if defined(BCM_TRX_SUPPORT)
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/virtual.h>
#endif /* BCM_TRX_SUPPORT */
#if defined(BCM_TOMAHAWK3_SUPPORT)
#include <bcm_int/esw/tomahawk3.h>
#endif /* BCM_TOMAHAWK3_SUPPORT */

#ifdef ALPM_ENABLE
#include <bcm_int/esw/alpm.h>
#include <bcm_int/esw/trident2.h>

#endif

#ifdef INCLUDE_L3
#define ENV_EGRESS_OBJECT_ID "egr_object_id"

#define L3X_MAX_BUCKET_SIZE     16
#define L3X_BUCKET_SIZE(_u)     ((SOC_IS_RAVEN(_u) || SOC_IS_TR_VL(_u)) ? 16 : 8)
#define L3X_IP_MULTICAST(_i)    (((uint32)(_i) & 0xf0000000) == 0xe0000000)

/*
 * Function:
 *      _ipmc_cmd_entry_print
 * Description:
 *	Internal function to print out ipmc entry info
 * Parameters:
 *      unit   - device number.
 *	    info   - Pointer to bcm_ipmc_addr_t data structure.
 *      cookie - user cookie
 */
STATIC int
_ipmc_cmd_entry_print(int unit, bcm_ipmc_addr_t *info, void *cookie)
{
    char s_ip_str[IP6ADDR_STR_LEN];
    char mc_ip_str[IP6ADDR_STR_LEN];

    /* Input parameters check. */
    if (NULL == info) {
        return (BCM_E_PARAM);
    }

    /* Print IPMC entry. */
    if (info->flags & BCM_IPMC_IP6) {
        format_ip6addr(s_ip_str, info->s_ip6_addr);
        format_ip6addr(mc_ip_str, info->mc_ip6_addr);
        cli_out("SRC IP ADDRESS: %s\n", s_ip_str);
        cli_out("MC  IP ADDRESS: %s\n", mc_ip_str);
        cli_out("VLAN  MOD  PORT V COS  TS   ");
        cli_out("%s     %s     %s  %s  %s\n", "Group", "GroupL2", "VRF", "CLASS", "HIT");
        cli_out("%3d %4d %5d %2d %3d %3d   0x%x 0x%x %2d,%5d,%4s %6d\n",
                info->vid, info->mod_id, info->port_tgid,
                info->v, info->cos,
                info->ts, info->group, info->group_l2,
                info->vrf, info->lookup_class,
                (info->flags & BCM_IPMC_HIT) ? "y" :"no", info->l3a_intf);
    } else {
        format_ipaddr(s_ip_str, info->s_ip_addr);
        format_ipaddr(mc_ip_str, info->mc_ip_addr);
        cli_out("%-16s%-16s%4d %3d %4d %3d %3d %2d   0x%x %3d\n",
                s_ip_str, mc_ip_str, info->vid,
                info->mod_id, info->port_tgid,
                info->v, info->cos, info->ts, info->group,
                info->vrf);
        cli_out("0x%x %d %6s %6d\n",
                info->group_l2, info->lookup_class, (info->flags & BCM_IPMC_HIT) ? "y" :"no", info->l3a_intf);

    }
    return (BCM_E_NONE);
}
#if defined(BCM_XGS_SWITCH_SUPPORT)
char if_ipmc_usage[] =
    "Usages:\n\t"
#ifdef COMPILER_STRING_CONST_LIMIT
    "  ipmc <options> [arg...]\n"
#else
    "  ipmc init    - Init ipmc function.\n\t"
    "  ipmc detach  - Detach ipmc function.\n\t"
    "  ipmc config [Enable=<bool>] [CheckSrcPort=<bool>] "
                                  "[CheckSrcIp=<bool>]\n\t"
    "  ipmc table add Src_IP=<ipaddr> Mc_IP=<grpaddr> VlanID=<vid>\n\t"
    "         Group=<val> [GroupL2=<val>] [src_Port=<port>] [COS=<cosl>]\n\t"
    "         [TS=<tsl>] [Valid=<bool>] [NoCHECK=<bool>]\n\t"
    "         [Replace=<bool>] [VRF=<vrf>] [LookupClass=<val>]\n\t"
    "              - Add an entry into ipmc table\n\t"
    "  ipmc table del Src_IP=<ipaddr> Mc_IP=<macaddr> VlanID=<vid>\n\t"
    "              - Destroy an entry in ipmc table\n\t"
    "  ipmc table clear\n\t"
    "              - Delete all ipmc table\n\t"
    "  ipmc table show [entry]\n\t"
    "              - Display all ipmc table or a single one\n\t"
    "  ipmc egr set Port=<port> Mac=<macaddr> Vlan=<vid>\n\t"
    "                    Untag=<untag> Ttl_thresh=<ttl threshold>\n\t"
    "              - set egress IP Multicast configuration regs for a port\n\t"
    "  ipmc egr show [<port>]\n\t"
    "              - Display egress IP Multicast configuration regs for\n\t"
    "                all ports or a single port\n\t"
    "  ipmc counter show [<port>]\n\t"
    "              - Display IP Multicast counter information\n\t"
    "  ipmc ip6table add Src_IP=<ipaddr> Mc_IP=<grpaddr> VlanID=<vid>\n\t"
    "         Group=<val> [src_Port=<port>] [COS=<cosl>]\n\t"
    "         [NoCHECK=<bool>] [TS=<tsl>]\n\t"
    "         [Valid=<bool>] [LookupClass=<val>] [VRF=<vrf>]\n\t"
    "              - Add an entry into ipmc table\n\t"
    "  ipmc ip6table del Src_IP=<ipaddr> Mc_IP=<macaddr> VlanID=<vid> [VRF=<vrf>]\n\t"
    "              - Destroy an entry in ipmc table\n\t"
    "  ipmc ip6table clear\n\t"
    "              - Delete all ipmc table\n\t"
    "  ipmc ip6table show [entry]\n\t"
    "              - Display all ipmc table or a single one\n"
#endif
    ;

cmd_result_t
if_ipmc(int unit, args_t *a)
{
    char *table, *subcmd_s, *argstr;
    int subcmd = 0;
    parse_table_t pt;
    cmd_result_t retCode;
    bcm_mac_t mac;
    vlan_id_t vid;
    int vlan = 0;
    int r, i, entry = 0, one_entry_only = 0;
    int dport, port = 0, ttl_th = 1, untag = 1;
    char mac_str[SAL_MACADDR_STR_LEN];
    bcm_ipmc_counters_t counters;
    bcm_ip_t s_ip_addr = 0, mc_ip_addr = 0;
    bcm_ipmc_addr_t ipmc_data;
    bcm_pbmp_t pbmp;
    bcm_port_config_t pcfg;
    int valid = 1, cos = 0, ts = 0, nocheck = 0, replace = 0, group = 0, group_l2 = 0;
    int lookup_class = 0;
    int cfg_enable = 1, cfg_check_src_port = 1, cfg_check_src_ip = 1;
#if defined(BCM_XGS3_SWITCH_SUPPORT)
    int vrf = 0;
    bcm_ip6_t       ip6_addr, sip6_addr;
#endif

#define IPMC_ADD 1
#define IPMC_DEL 2
#define IPMC_CLEAR 3
#define IPMC_SHOW 4
#define IPMC_SET 5

    sal_memset(mac, 0x10, 6);
    sal_memset(&ipmc_data, 0, sizeof(ipmc_data));
#if defined(BCM_XGS3_SWITCH_SUPPORT)
    sal_memset(ip6_addr, 0, sizeof(bcm_ip6_t));
    sal_memset(sip6_addr, 0, sizeof(bcm_ip6_t));
#endif

    /* Check valid device to operation on ...*/
    if (! sh_check_attached(ARG_CMD(a), unit)) {
	return CMD_FAIL;
    }

    if (bcm_port_config_get(unit, &pcfg) != BCM_E_NONE) {
        cli_out("%s: Error: bcm ports not initialized\n", ARG_CMD(a));
        return CMD_FAIL;
    }

    if ((table = ARG_GET(a)) == NULL) {
	return CMD_USAGE;
    }

    if (sal_strcasecmp(table, "init") == 0) {
	if ((r = bcm_ipmc_init(unit)) < 0) {
	    cli_out("%s: error initializing: %s\n", ARG_CMD(a), bcm_errmsg(r));
	}
        return CMD_OK;
    }
    if (sal_strcasecmp(table, "detach") == 0) {
	if ((r = bcm_ipmc_detach(unit)) < 0) {
	    cli_out("%s: error detaching: %s\n", ARG_CMD(a), bcm_errmsg(r));
	}
        return CMD_OK;
    }

    if (sal_strcasecmp(table, "config") == 0) {
	parse_table_init(unit, &pt);
	parse_table_add(&pt, "Enable", PQ_DFL|PQ_BOOL, 0,
			(void *)&cfg_enable, 0);
	parse_table_add(&pt, "CheckSrcPort", PQ_DFL|PQ_BOOL, 0,
			(void *)&cfg_check_src_port, 0);
	parse_table_add(&pt, "CheckSrcIp", PQ_DFL|PQ_BOOL, 0,
			(void *)&cfg_check_src_ip, 0);
	if (parse_arg_eq(a, &pt) < 0) {
	    cli_out("%s: Error: Unknown option: %s\n",
                    ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
	    return CMD_FAIL;
	}
	i = CMD_OK;
	if (pt.pt_entries[0].pq_type & PQ_PARSED) {
	    r = bcm_ipmc_enable(unit, cfg_enable);
	    if (r < 0) {
		cli_out("%s: Error: Enable failed: %s\n",
                        ARG_CMD(a), bcm_errmsg(r));
		i = CMD_FAIL;
	    }
	}
	if (pt.pt_entries[1].pq_type & PQ_PARSED) {
            /* bcm_ipmc_source_port_check is deprecated */
	    r = BCM_E_UNAVAIL;
	    if (r < 0) {
		cli_out("%s: Error: check_source_port failed: %s\n",
                        ARG_CMD(a), bcm_errmsg(r));
		i = CMD_FAIL;
	    }
	}
	if (pt.pt_entries[2].pq_type & PQ_PARSED) {
            /* bcm_ipmc_source_ip_search is deprecated */
	    r = BCM_E_UNAVAIL;
	    if (r < 0) {
		cli_out("%s: Error: check_source_ip failed: %s\n",
                        ARG_CMD(a), bcm_errmsg(r));
		i = CMD_FAIL;
	    }
	}
        parse_arg_eq_done(&pt);
	return (i);
    }

    if ((subcmd_s = ARG_GET(a)) == NULL) {
	return CMD_USAGE;
    }

    if (sal_strcasecmp(subcmd_s, "add") == 0) {
	subcmd = IPMC_ADD;
    }

    if (sal_strcasecmp(subcmd_s, "del") == 0) {
	subcmd = IPMC_DEL;
    }

    if (sal_strcasecmp(subcmd_s, "clear") == 0) {
	subcmd = IPMC_CLEAR;
    }

    if (sal_strcasecmp(subcmd_s, "show") == 0) {
	subcmd = IPMC_SHOW;
    }

    if (sal_strcasecmp(subcmd_s, "set") == 0) {
	subcmd = IPMC_SET;
    }

    /*    coverity[equality_cond : FALSE]    */
    if (!subcmd) {
	return CMD_USAGE;
    }

    if (sal_strcasecmp(table, "table") == 0) {
	switch (subcmd) {
	case IPMC_ADD:
	    parse_table_init(unit, &pt);
	    parse_table_add(&pt, "Src_IP", PQ_DFL|PQ_IP, 0,
			    (void *)&s_ip_addr, 0);
	    parse_table_add(&pt, "Mc_IP", PQ_DFL|PQ_IP, 0,
			    (void *)&mc_ip_addr, 0);
	    parse_table_add(&pt, "VlanID", PQ_DFL|PQ_INT, 0, (void *)&vlan, 0);
        parse_table_add(&pt, "COS", PQ_DFL|PQ_INT, 0, (void *)&cos, 0);
#if defined(BCM_XGS3_SWITCH_SUPPORT)
		parse_table_add(&pt, "VRF", PQ_DFL|PQ_INT, 0,
				(void *)&vrf, 0);
#endif
	    parse_table_add(&pt, "Valid", PQ_DFL|PQ_BOOL, 0,
			    (void *)&valid, 0);
	    parse_table_add(&pt, "src_Port", PQ_INT, 0, (void *)&port, 0);
	    parse_table_add(&pt, "TS", PQ_INT, 0, (void *)&ts, 0);
	    parse_table_add(&pt, "NoCHECK", PQ_DFL|PQ_BOOL, 0,
			    (void *)&nocheck, 0);
	    parse_table_add(&pt, "Replace", PQ_DFL|PQ_BOOL, 0,
			    (void *)&replace, 0);
        parse_table_add(&pt, "LookupClass", PQ_DFL|PQ_INT, 0,
                            (void *)&lookup_class, 0);
        parse_table_add(&pt, "Group", PQ_DFL|PQ_INT, 0,
                            (void *)&group, 0);
        parse_table_add(&pt, "GroupL2", PQ_DFL|PQ_INT, 0,
                            (void *)&group_l2, 0);
        if (parse_arg_eq(a, &pt) < 0) {
	        cli_out("%s: Error: Unknown option: %s\n",
                        ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
	        return CMD_FAIL;
        }
        bcm_ipmc_addr_t_init(&ipmc_data);
        if (pt.pt_entries[3].pq_type & PQ_PARSED) {
            ipmc_data.flags |= BCM_IPMC_SETPRI;
		}
        parse_arg_eq_done(&pt);		
	    vid = vlan;
        ipmc_data.s_ip_addr = s_ip_addr;
        ipmc_data.mc_ip_addr = mc_ip_addr;
        ipmc_data.vid = vid;
#if defined(BCM_XGS3_SWITCH_SUPPORT)
        ipmc_data.vrf = vrf;
#endif
	    ipmc_data.cos = cos;
	    ipmc_data.ts = ts;
	    ipmc_data.port_tgid = port; 
	    ipmc_data.v = valid;
	    if (nocheck) {
		ipmc_data.flags |= BCM_IPMC_SOURCE_PORT_NOCHECK;
	    }
        if (replace) {
            ipmc_data.flags |= BCM_IPMC_REPLACE;
        }
        ipmc_data.lookup_class = lookup_class;
        if (group) {
            ipmc_data.group = group;
        }
        if (group_l2) {
            ipmc_data.group_l2 = group_l2;
        }
	    r = bcm_ipmc_add(unit, &ipmc_data);
	    if (r < 0) {
		cli_out("%s: Error Add to ipmc table %s\n", ARG_CMD(a),
                        bcm_errmsg(r));
		return CMD_FAIL;
	    }
	    return CMD_OK;
	    break;
	case IPMC_DEL:
	    parse_table_init(unit, &pt);
	    parse_table_add(&pt, "Src_IP", PQ_DFL|PQ_IP, 0,
			    (void *)&s_ip_addr, 0);
	    parse_table_add(&pt, "Mc_IP", PQ_DFL|PQ_IP, 0,
			    (void *)&mc_ip_addr, 0);
	    parse_table_add(&pt, "VlanID", PQ_DFL|PQ_INT, 0, (void *)&vlan, 0);
#if defined(BCM_XGS3_SWITCH_SUPPORT)
		parse_table_add(&pt, "VRF", PQ_DFL|PQ_INT, 0,
				(void *)&vrf, 0);
#endif
	    if (!parseEndOk( a, &pt, &retCode))
		return retCode;
        bcm_ipmc_addr_t_init(&ipmc_data);
        ipmc_data.s_ip_addr = s_ip_addr;
        ipmc_data.mc_ip_addr = mc_ip_addr;
        ipmc_data.vid = vlan;
#if defined(BCM_XGS3_SWITCH_SUPPORT)
	        ipmc_data.vrf = vrf;
#endif
	    r = bcm_ipmc_remove(unit, &ipmc_data);
	    if (r < 0) {
		cli_out("%s: Error delete from ipmc table %s\n", ARG_CMD(a),
                        bcm_errmsg(r));
		return CMD_FAIL;
	    }
	    return CMD_OK;
	    break;
	case IPMC_CLEAR:
	    r = bcm_ipmc_remove_all(unit);
	    if (r < 0) {
		cli_out("%s: %s\n",
                        ARG_CMD(a), bcm_errmsg(r));
		return CMD_FAIL;
	    }
	    return (CMD_OK);
	    break;
    case IPMC_SHOW:
        if (ARG_CNT(a)) {
            if (!isint(ARG_CUR(a))) {
                cli_out("%s: Invalid entry number: %s\n", ARG_CMD(a),
                        ARG_CUR(a));
                return (CMD_FAIL);
            }
            entry = parse_integer(ARG_GET(a));
            if (entry == 0)
                return (CMD_OK);
            if (_BCM_MULTICAST_IS_SET(entry) && _BCM_MULTICAST_IS_L3(entry)) {
                entry = _BCM_MULTICAST_ID_GET(entry);
            }
            if (entry < soc_mem_index_min(unit, L3_IPMCm) ||
                entry > soc_mem_index_max(unit, L3_IPMCm)) {
                cli_out("%s: ERROR: Entry # too high, only permitted %d\n",
                        ARG_CMD(a), soc_mem_index_max(unit, L3_IPMCm));
                return(CMD_FAIL);
            }
            one_entry_only = 1;
        }

        cli_out("SRC IP ADDRESS  MC IP ADDRESS     VLAN MOD PORT V COS ");
        cli_out("TS   GROUP     %s\n",
                "VRF");
        cli_out("%s %s %s %s\n",
                "GROUP-L2 ", "CLASS", "HIT", "L3_IIF");
        if (one_entry_only) {
            /* bcm_ipmc_get_by_index is deprecated */
            r = BCM_E_UNAVAIL;
            return CMD_OK;
        }
        bcm_ipmc_traverse(unit, 0, _ipmc_cmd_entry_print, NULL);
        return (CMD_OK);
        break;
	default:
	    return CMD_USAGE;
	    break;
	}
	return CMD_OK;
    }

    if (sal_strcasecmp(table, "egr") == 0) {
	switch (subcmd) {
	case IPMC_SET:
	    parse_table_init(unit, &pt);
	    parse_table_add(&pt, "Port", PQ_PORT, 0, (void *)&port, 0);
	    parse_table_add(&pt, "MAC", PQ_DFL|PQ_MAC, 0, (void *)mac, 0);
	    parse_table_add(&pt, "Vlan", PQ_INT, 0, (void *)&vlan, 0);
	    parse_table_add(&pt, "Untag", PQ_DFL|PQ_BOOL, 0, (void *)&untag,
			    0);
	    parse_table_add(&pt, "Ttl_thresh", PQ_DFL|PQ_INT, 0,
			    (void *)&ttl_th, 0);
	    if (!parseEndOk( a, &pt, &retCode))
		return retCode;
	    vid = vlan;
	    r = bcm_ipmc_egress_port_set(unit, port, mac, untag, vid,
					 ttl_th);
	    if (r < 0) {
		cli_out("%s: Egress IP Multicast Configuration registers: %s\n",
                        ARG_CMD(a), bcm_errmsg(r));
		return CMD_FAIL;
	    }
	    break;

	case IPMC_SHOW:
            if ((argstr  = ARG_GET(a)) == NULL) { 
                pbmp = pcfg.e;
            } else {
		if (parse_bcm_port(unit, argstr, &port) < 0) {
		    cli_out("%s: Invalid port string: %s\n", ARG_CMD(a),
                            argstr);
		    return CMD_FAIL;
		}
		if (!BCM_PBMP_MEMBER(pcfg.e, port)) {
		    cli_out("port %d is not a valid Ethernet port\n", port);
		    return CMD_FAIL;
		}
                BCM_PBMP_CLEAR(pbmp);
                BCM_PBMP_PORT_ADD(pbmp, port);
	    }
	    cli_out("Egress IP Multicast Configuration Register information\n");
	    cli_out("Port      Mac Address    Vlan Untag TTL_THRESH\n");
	    DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
		r = bcm_ipmc_egress_port_get(unit, port, mac, &untag, &vid,
					     &ttl_th);
		if (r == BCM_E_NONE) {
		    format_macaddr(mac_str, mac);
		    cli_out("%-4s  %-18s %4d   %s  %5d\n",
                            BCM_PORT_NAME(unit, port), mac_str,
                            vid, untag ? "Y" : "N", ttl_th);
		}
	    }
	    return CMD_OK;
	    break;
	default:
	    return CMD_USAGE;
	    break;
	}

	return CMD_OK;
    }

    if (sal_strcasecmp(table, "counter") == 0) {
        if ((argstr  = ARG_GET(a)) == NULL) { 
            pbmp = pcfg.e;
        } else {
            if (parse_bcm_port(unit, argstr, &port) < 0) {
                cli_out("%s: Invalid port string: %s\n", ARG_CMD(a),
                        argstr);
                return CMD_FAIL;
            }
            if (!BCM_PBMP_MEMBER(pcfg.e, port)) {
                cli_out("port %d is not a valid Ethernet port\n", port);
                return CMD_FAIL;
            }
            BCM_PBMP_CLEAR(pbmp);
            BCM_PBMP_PORT_ADD(pbmp, port);
        }
	cli_out("PORT      RMCA     TMCA     IMBP     IMRP    RIMDR    TIMDR\n");
        /* coverity[overrun-local] */
        DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
	    r = bcm_ipmc_counters_get(unit, port, &counters);
	    if (r == BCM_E_NONE) {
		cli_out("%4d  %8d %8d %8d %8d %8d %8d\n",
                        port,
                        COMPILER_64_LO(counters.rmca),
                        COMPILER_64_LO(counters.tmca),
                        COMPILER_64_LO(counters.imbp),
                        COMPILER_64_LO(counters.imrp),
                        COMPILER_64_LO(counters.rimdr),
                        COMPILER_64_LO(counters.timdr));
	    }
	}
	return CMD_OK;
    }

#if defined(BCM_XGS3_SWITCH_SUPPORT)
    if (sal_strcasecmp(table, "ip6table") == 0) {
        switch (subcmd) {
        /* ipmc ip6table add */
        case IPMC_ADD:
            parse_table_init(unit, &pt);
            parse_table_add(&pt, "Src_IP", PQ_DFL|PQ_IP6, 0,
                            (void *)&sip6_addr, 0);
            parse_table_add(&pt, "Mc_IP", PQ_DFL|PQ_IP6, 0,
                            (void *)&ip6_addr, 0);
            parse_table_add(&pt, "VlanID", PQ_DFL|PQ_INT, 0, (void *)&vlan, 0);
            parse_table_add(&pt, "Valid", PQ_DFL|PQ_BOOL, 0,
                            (void *)&valid, 0);
            parse_table_add(&pt, "COS", PQ_DFL|PQ_INT, 0, (void *)&cos, 0);
            parse_table_add(&pt, "src_Port", PQ_INT, 0, (void *)&port, 0);
            parse_table_add(&pt, "TS", PQ_INT, 0, (void *)&ts, 0);
            parse_table_add(&pt, "NoCHECK", PQ_DFL|PQ_BOOL, 0,
                            (void *)&nocheck, 0);
            parse_table_add(&pt, "LookupClass", PQ_DFL|PQ_INT, 0,
                            (void *)&lookup_class, 0);
            parse_table_add(&pt, "Group", PQ_DFL|PQ_INT, 0,
                            (void *)&group, 0);
            parse_table_add(&pt, "GroupL2", PQ_DFL|PQ_INT, 0,
                            (void *)&group_l2, 0);
            parse_table_add(&pt, "VRF", PQ_DFL | PQ_INT, 0, (void *)&vrf, 0);
            if (parse_arg_eq(a, &pt) < 0) {
                cli_out("%s: Error: Unknown option: %s\n",
                        ARG_CMD(a), ARG_CUR(a));
                parse_arg_eq_done(&pt);
                return CMD_FAIL;
			}
            bcm_ipmc_addr_t_init(&ipmc_data);
            if (pt.pt_entries[4].pq_type & PQ_PARSED) {
                ipmc_data.flags |= BCM_IPMC_SETPRI;
            }
            parse_arg_eq_done(&pt); 
            sal_memcpy(ipmc_data.mc_ip6_addr, ip6_addr, BCM_IP6_ADDRLEN);
            sal_memcpy(ipmc_data.s_ip6_addr, sip6_addr, BCM_IP6_ADDRLEN);
            ipmc_data.cos = cos;
            ipmc_data.vid = vlan;
            ipmc_data.flags |= BCM_IPMC_IP6;
            ipmc_data.ts = ts;
            ipmc_data.port_tgid = port;
            ipmc_data.vrf = vrf;
            ipmc_data.v = valid;
            if (nocheck) {
                ipmc_data.flags |= BCM_IPMC_SOURCE_PORT_NOCHECK;
            }
            ipmc_data.lookup_class = lookup_class;
            if (group) {
                ipmc_data.group = group;
            }
            if (group_l2) {
                ipmc_data.group_l2 = group_l2;
            }
            r = bcm_ipmc_add(unit, &ipmc_data);
            if (r < 0) {
                cli_out("%s: Error Add to ipmc table %s\n", ARG_CMD(a),
                        bcm_errmsg(r));
                return CMD_FAIL;
            }
            return CMD_OK;
            break;
        case IPMC_DEL:
        /* ipmc ip6table del */
            parse_table_init(unit, &pt);
            parse_table_add(&pt, "Src_IP", PQ_DFL|PQ_IP6, 0,
                            (void *)&sip6_addr, 0);
            parse_table_add(&pt, "Mc_IP", PQ_DFL|PQ_IP6, 0,
                            (void *)&ip6_addr, 0);
            parse_table_add(&pt, "VlanID", PQ_DFL|PQ_INT, 0, (void *)&vlan, 0);
            parse_table_add(&pt, "VRF", PQ_DFL | PQ_INT, 0, (void *)&vrf, 0);
            if (!parseEndOk( a, &pt, &retCode))
                return retCode;
            bcm_ipmc_addr_t_init(&ipmc_data);
            sal_memcpy(ipmc_data.mc_ip6_addr, ip6_addr, BCM_IP6_ADDRLEN);
            sal_memcpy(ipmc_data.s_ip6_addr, sip6_addr, BCM_IP6_ADDRLEN);
            ipmc_data.vid=vlan;
            ipmc_data.flags = BCM_IPMC_IP6;
            ipmc_data.vrf = vrf;
            r = bcm_ipmc_remove(unit, &ipmc_data);
            if (r < 0) {
                cli_out("%s: Error delete from ipmc table %s\n", ARG_CMD(a),
                        bcm_errmsg(r));
                return CMD_FAIL;
            }
            return CMD_OK;
            break;
        case IPMC_CLEAR:
        /* ipmc ip6table clear */
            r = bcm_ipmc_remove_all(unit);
            if (r < 0) {
                cli_out("%s: %s\n",
                        ARG_CMD(a), bcm_errmsg(r));
                return CMD_FAIL;
            }
            return (CMD_OK);
            break;
        case IPMC_SHOW:
        /* ipmc ip6table show */
            if (ARG_CNT(a)) {
                if (!isint(ARG_CUR(a))) {
                    cli_out("%s: Invalid entry number: %s\n", ARG_CMD(a),
                            ARG_CUR(a));
                    return (CMD_FAIL);
                }
                entry = parse_integer(ARG_GET(a));
                if (entry == 0)
                    return (CMD_OK);
                if (entry < soc_mem_index_min(unit, L3_IPMCm) ||
                    entry > soc_mem_index_max(unit, L3_IPMCm)) {
                    cli_out("%s: ERROR: Entry # too high, only permitted %d\n",
                            ARG_CMD(a), soc_mem_index_max(unit, L3_IPMCm));
                    return(CMD_FAIL);
                }
                one_entry_only = 1;
            }

            if (one_entry_only) {
                /* bcm_ipmc_get_by_index is deprecated */
                r = BCM_E_UNAVAIL;
                cli_out("%s: bcm_ipmc_get_by_index is deprecated %s\n",
                        ARG_CMD(a), bcm_errmsg(r));
            }
            bcm_ipmc_traverse(unit, BCM_IPMC_IP6, _ipmc_cmd_entry_print, NULL);

            return (CMD_OK);
            break;
        default:
            return CMD_USAGE;
            break;
        }
        return CMD_OK;
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */

    return CMD_USAGE;
}
#endif /* BCM_XGS_SWITCH_SUPPORT */
#endif /* INCLUDE_L3 */

#ifdef INCLUDE_L3
char if_l3_usage[] =
    "Usages:\n\t"
#ifdef COMPILER_STRING_CONST_LIMIT
    "  l3 <option> [args...]\n"
#else
    "  l3 init    - Init L3 function.\n\t"
    "  l3 detach  - Detach L3 function.\n\t"
    "  l3 info    - Display the capacity and status of the L3 resources.\n\n\t"

    "  l3 intf add <Vlan>=<val> <Mac>=<val> [<INtf>=<val>] [<VRF>=<val>]\n\t"
    "                   [<Mtu>=<val>] [<Group>=<val>]\n\t"
    "                   - Add an entry into l3 interface table\n\t"
    "  l3 intf destroy <INtf>=<val>\n\t"
    "                   - Destroy an entry in l3 interface table\n\t"
    "  l3 intf clear    - Destroy all intf\n\t"
    "  l3 intf show     - [<INtf>=<val>]\n\t"
    "                     Display all Intfs or a single one\n\n\t"
    
    "  l3 egress add <Mac>=<val> <INtf>=<val> [<Trunk>=<val>]\n\t"
    "                  <Port>=<val> [<MOdule>=<val>] [<L2tocpu>=<val>]\n\t" 
    "                  [<L3MC>=<val>] [<MplsLabel>=<val>] [<Drop>=<val>]\n\t"
    "                  [<CopyToCpu>=<val>]\n\t"
    "                  - Create egress object entry\n\t"
    "  l3 egress update <EgrId>=<val> <Mac>=<val> <INtf>=<val>\n\t"
    "                  [<Trunk>=<val>] <Port>=<val> [<MOdule>=<val>]\n\t"
    "                  [<L2tocpu>=<val>] [<L3MC>=<val>] [<MplsLabel>=<val>]\n\t"
    "                  - Update egress object entry\n\t"
    "  l3 egress delete <entry>\n\t"
    "                  - Delete egress object entry\n\t"
    "  l3 egress show [entry]\n\t"
    "                  -Show egress object entry\n\t"
    "  l3 egress find <Mac>=<val> <INtf>=<val> [<Trunk>=<val>]\n\t"
    "                  <Port>=<val> [<MOdule>=<val>] [<L2tocpu>=<val>]\n\t" 
    "                  [<L3MC>=<val>] [<MplsLabel>=<val>]\n\t"
    "                  - Find egress object entry\n\t"
    "  l3 multipath add <Size>=<val> [<INtf0>=<val>] [<INtf1>=<val>]\n\t"
    "                  [<INtf2>=<val>] [<INtf3>=<val>]\n\t"
    "                  - Create multipath egress object entry\n\t"
    "  l3 multipath maxadd <Size>=<val> [<MaxGrpSize>=<val>] [<INtf0>=<val>] [<INtf1>=<val>]\n\t"
    "                  [<INtf2>=<val>] [<INtf3>=<val>]\n\t"
    "                  - Create multipath egress object entry(dynamic grp size)\n\t"
    "  l3 multipath update <EgrId>=<val> <Size>=<val> [<INtf0>=<val>]\n\t"
    "                  [<INtf1>=<val>] [<INtf2>=<val>] [<INtf3>=<val>]\n\t"
    "                  - Update multipath egress object entry\n\t"
    "  l3 multipath delete <entry>\n\t"
    "                  - Delete multipath egress object entry\n\t"
    "  l3 multipath show [entry]\n\t"
    "                  -Show multipath egress object entry\n\t"
    "  l3 multipath insert <EgrId>=<val> <Intf>=<val>\n\t"
    "                  -Add interface to multipath egress object entry\n\t"
    "  l3 multipath remove <EgrId>=<val> <Intf>\n\t"
    "                  -Remove interface from multipath egress object entry\n\t"
    "  l3 multipath find <Size>=<val> [<INtf0>=<val>] [<INtf1>=<val>]\n\t"
    "                  [<INtf2>=<val>] [<INtf3>=<val>]\n\t"
    "                  - Find multipath egress object entry\n\t"
    "  l3 ecmp egress add <Size>=<val> [<INtf0>=<val>] [<INtf1>=<val>]\n\t"
    "                  [<INtf2>=<val>] [<INtf3>=<val>]\n\t"
    "                  - Create ecmp egress object entry\n\t"
    "  l3 ecmp egress maxadd <Size>=<val> [<MaxGrpSize>=<val>] [<INtf0>=<val>] [<INtf1>=<val>]\n\t"
    "                  [<INtf2>=<val>] [<INtf3>=<val>]\n\t"
    "                  - Create ecmp egress object entry(dynamic grp size)\n\t"
    "  l3 ecmp egress update <EgrId>=<val> <Size>=<val> [<INtf0>=<val>]\n\t"
    "                  [<INtf1>=<val>] [<INtf2>=<val>] [<INtf3>=<val>]\n\t"
    "                  - Update ecmp egress object entry\n\t"
    "  l3 ecmp egress delete <entry>\n\t"
    "                  - Delete ecmp egress object entry\n\t"
    "  l3 ecmp egress show [entry]\n\t"
    "                  -Show ecmp egress object entry\n\t"
    "  l3 ecmp egress insert <EgrId>=<val> <Intf>=<val>\n\t"
    "                  -Add interface to ecmp egress object entry\n\t"
    "  l3 ecmp egress remove <EgrId>=<val> <Intf>\n\t"
    "                  -Remove interface from ecmp egress object entry\n\t"
    "  l3 ecmp egress find <Size>=<val> [<INtf0>=<val>] [<INtf1>=<val>]\n\t"
    "                  [<INtf2>=<val>] [<INtf3>=<val>]\n\t"
    "                  - Find ecmp egress object entry\n\t"
    "  l3 l3table add <VRF>=<val> <IP>=<val> <Mac>=<val> <INtf>=<val>\n\t"
    "       [<Hit>=<val>] [<Trunk>=<val>] [MOdule=<val>] <Port>=<val>\n\t"
    "       <PRI>=<val> <RPE>=<val> [<Untag>=<val>] [<L2tocpu>=<val>]\n\t"
    "       [<HOST_AS_ROUTE>=<val>] [Replace=<bool>] <LookupClass>=<val>\n\t"
    "  l3 l3table destroy <VRF>=<val> <IP>=<val>\n\t"
    "                   - Destroy an entry in l3 table\n\t"
    "  l3 l3table clear - Destroy all l3 table\n\t"
    "  l3 l3table show  - Display all l3 table or a single one\n\t"
    "  l3 l3table age   - Age L3 table\n\t"
    "  l3 l3table sanity <Start=<index> End=<index>\n\t"
    "                   - Check l3x tables for (san)ity in the given range\n\t"
    "  l3 l3table hash <VRF>=<val> <IP>=<val> [<SrcIP>=<val>] [<SipKey>=<val>]\n\t"
    "                    [<VID>=<val>] [<Hash>=<hash_type>]\n\t"
    "                   - Calculate l3 hash index\n\t"
    "  l3 l3table ip6hash <IP>=<val> [<SrcIP>=<val>] [<SipKey>=<val>]\n\t"
    "                    [<VID>=<val>] [<Hash>=<hash_type>]\n\t"
    "                   - Calculate l3 hash index for IPv6 entry\n\t"
    "  l3 l3table untag  - Recalculate untagged bits for L3\n\n\t"
    "  l3 l3table conflict <VRF>=<val> <DIP>=<val> <DIP6>=<val>\n\t"
    "                     <SIP>=<val> <SIP6>=<val> <IP6>=<val> <IPMC>=<val>\n\t"
    "                     <VLAN>=<val> \n\t"
    "                     - Get conflicting l3 entries.\n\n\t"

    "  l3 defip add <VRF>=<val> <IP>=<val> <MaSk>=<val> <Mac>=<val>\n\t"
    "       <INtf>=<val> <Gateway>=<next hop Ip address> <Port>=<val>\n\t"
    "       [MOdule=<val>] [<Untag>=<val>] [<Local>=<val>] [<ECMP>=<1/0>]\n\t"
    "       [<Hit>=<val>] [<VLAN>=<val>](BCM5695 def route 0.0.0.0/0 only)\n\t"
    "       [<DstDiscard>=<val>] <PRI>=<val> <RPE>=<val> [Replace=<bool>]\n\t"
    "       [<MplsLabel>=<val>] <TunnelOpt>=<val> <LookupClass>=<val>\n\t"
    "                   - Add an IPv4 entry into DEFIP table\n\t"
    "  l3 defip destroy <VRF>=<val> <IP>=<val> <MaSk>=<val>\n\t"
    "       [<ECMP=<1/0> <Mac>=<val> <Gateway>=<next hop Ip address>\n\t"
    "        <INtf>=<val> <Port>=<val> [MOdule=<val>]\n\t"
    "       [<VLAN>=<val>](BCM5695 def route 0.0.0.0/0 only)\n\t"
    "                   - Destroy an IPv4 entry in def IP table\n\t"
    "  l3 defip clear   - Destroy all IPv4 DEFIP table\n\t"
    "  l3 defip age     - Age DEFIP(LPM) table(XGS Switch only)\n\t"
    "  l3 defip show [<START> [<COUNT>]]\n\t"
    "                   - Display all IPv4 DEFIP table or\n\t"
    "                     the entires in a specified range\n\t"
    "  l3 defip ecmp <max>=<val> - Set MAX number of ECMP paths allowed\n\t"
    "  l3 defip ecmphash <DIP>=<val> <SIP>=<val> <SIP6>=val <DIP6>=val\n\t"
    "                    <COUNT>=<val> <IP6>=val  <L4_SRC_PORT>=val\n\t"
    "                    <L4_DST_PORT>=val [<EHash>=<ECMP hash Sel>] \n\t"
    "                    [<Hash>=<L3 hash type>]\n\t"
    "                   - Calculate ECMP hash index (COUNT is zero based)\n\t"
#ifdef FB_LPM_DEBUG
    "  l3 defip sanity - Check L3 lpm states in scaling mode\n\n\t"
#endif

#ifdef ALPM_ENABLE
    "  l3 alpm show [all] [<VRF>=<val>] [<Global-vrf>=<bool>] \n\t"
    "       [<Pivot>=<bool>] [<Bucket>=<bool>] [<Counter>=<bool>]\n\t"
    "       [BucketuSage=<bool>] [InternelDebug=<bool>\n\t"
    "                   - Show ALPM debug info\n\n\t"

    "  l3 alpm show brief [<Bucket>=<val>] [<Index>=<val>] \n\t"
    "                   - Show ALPM brief debug info\n\n\t"

    "  l3 alpm find [<VRF>=<val>] <IP4>=<val> <IP6>=<val> [<Length>=<val>]\n\t"
    "                   - Find hit longest prefix match route\n\n\t"   

    "  l3 alpm sanity [<memory>=<val>] [<index>=<val>] [<sw>=<0/1>]\n\t"
    "                   - Process sanity check for ALPM tables\n\n\t"

#endif

    "  l3 ip6host add <VRF>=<val> <IP>=<val> <Mac>=<val> [MOdule=<val>]\n\t"
    "       <Port>=<val> [<Trunk>=<val>] <INtf>=<val> [<Hit>=<val>]\n\t"
    "       <PRI>=<val> <RPE>=<val> [<Replace>=<bool>]\n\t"
    "       [<HOST_AS_ROUTE>=<bool>] <LookupClass>=<val>\n\t"
    "                   - Add/Update an IPv6 entry\n\t"
    "  l3 ip6host delete <VRF>=<val> <IP>=<val>\n\t"
    "                   - Delete an IPv6 entry\n\t"
    "  l3 ip6host clear - Delete all IPv6 host entries\n\t"
    "  l3 ip6host age   - Age IPv6 host entries\n\t"
    "  l3 ip6host show  - Show all IPv6 host entries\n\n\t"

    "  l3 ip6route add  <VRF>=<val> <IP>=<val> <MaskLen>=<val> <Mac>=<val>\n\t" 
    "                   [MOdule=<val>] <Port>=<val> [<Trunk>=<val>]\n\t" 
    "                   <INtf>=<val> <VLan>=<val> [<Hit>=<val>]\n\t" 
    "                   [<ECMP>=<1/0>] [<DstDiscard>=<val>] <PRI>=<val>\n\t" 
    "                   <RPE>=<val> [Replace=<bool>] [<MplsLabel>=<val>]\n\t"
    "                   <TunnelOpt>=<val> <LookupClass>=<val>\n\t"
    "                    - Add IPv6 route\n\t"
    "  l3 ip6route delete <VRF>=<val> <IP>=<val> <MaskLen>=<val>\n\t"
    "       [<ECMP=<1/0> <Mac>=<val> <INtf>=<val> <Port>=<val> [MOdule=<val>]\n\t"
    "                    - Delete an IPv6 route\n\t"
    "  l3 ip6route clear - Delete all IPv6 routes\n\t"
    "  l3 ip6route age   - Clear all IPv6 route HIT bits\n\t"
    "  l3 ip6route show [<START> [<COUNT>]]\n\t"
    "                    - Display all IPv6 routes or\n\t"
    "                      the routes in a specified range\n\t"

    "  l3 tunnel_init set <INtf>=<val> <TYpe>=<val> <TTL>=<val>\n\t"
    "       <Mac>=<val> <DIP>=<val> <SIP>=<val> <DSCPSel>=<val> <DSCPV>=<val>\n\t"
    "       <DFSEL4>=<val> <DFSEL6>=<val> <SIP6>=<val> <DIP6>=<val>\n\t"
    "  l3 tunnel_init show <INtf>=<val>\n\t"
    "  l3 tunnel_init clear <INtf>=<val>\n\t"

    "  l3 tunnel_term add <TYPE>=<val> <VRF>=<val> <DIP>=<val> <SIP>=<val>\n\t"
    "       <DipMaskLen>=<val> <SipMaskLen>=<val> <DIP6>=<val> <SIP6>=<val>\n\t"
    "       <L4DstPort>=<val> <L4SrcPort>=<val> <OuterTTL>=<bool> \n\t"
    "       <OuterDSCP>=<val> <InnerDSCP>=<val> <TrustDSCP>=<val> \n\t"
    "       <VLanid>=<val> <PBMP>=<val> <AllowIPv6Payload>=<bool> \n\t"
    "       <AllowIPv4Payload>=<bool> <Replace>=<bool>\n\t"
    "  l3 tunnel_term delete <TYPE>=<val> <VRF>=<val> <DIP>=<val> <SIP>=<val>\n\t"
    "       <DipMaskLen>=<val> <SipMaskLen>=<val> <DIP6>=<val> <SIP6>=<val>\n\t"
    "       <L4DstPort>=<val> <L4SrcPort>=<val> \n\t"
    "  l3 tunnel_term show <TYPE>=<val> <VRF>=<val> <DIP>=<val> <SIP>=<val>\n\t"
    "       <DipMaskLen>=<val> <SipMaskLen>=<val> <DIP6>=<val> <SIP6>=<val>\n\t"
    "       <L4DstPort>=<val> <L4SrcPort>=<val> \n\t"

    "  l3 dscp_map add\n\t"
    "  l3 dscp_map destroy <DscpMapId>=<val>\n\t"
    "  l3 dscp_map set <DscpMapId>=<val> <Priority>=<val> <Color>=<val>\n\t"
    "                  <DSCPValue>=<val>\n\t"
    "  l3 dscp_map show <DscpMapId>=<val>\n\t"
    "  l3 dscp_map_port set <Port>=<val> <Priority>=<val> <Color>=<val>\n\t"
    "                  <DSCPValue>=<val>\n\t"
    "  l3 dscp_map_port show <Port>=<val>\n\t"
    "  l3 prefix_map add <IP>=<val>\n\t"
    "  l3 prefix_map delete <IP>=<val>\n\t"
    "                  - Destroy ip6 prefix map to IPv4 entry\n\t"
    "  l3 prefix_map clear\n\t"
    "                  - Flush ip6 prefix map to IPv4 entry\n\t"
    "  l3 prefix_map show [entry]\n\t"
    "                  - Show ip6 prefix map to IPv4 entry\n\t"
    "  l3 nat_ingress show\n\t"
    "                  - Show all NAT Ingress Rules\n\t"
    "  l3 nat_egress add <DNAT=0/1> <DIP=new_ip> <SIP=new_ip> <DIPMASK=mask>\n\t"
    "       <SIPMASK=mask> <SRCPORT=new_port> <DSTPORT=new_port>\n\t"
    "                  - Add a new Egress Translation Rule\n\t"
    "  l3 nat_egress show\n\t"
    "                  - Show all NAT Egress Translation Rules\n\t"
    "  l3 route test perf\n\t"
    "                  - Run IPv4/IPv6 route performance test\n\t"
    "\n"
#endif
    ;

/*
 * Function:
 * _l3_cmd_info_show
 * Description:
 * Internal function to print out l3_info
 * Parameters:
 * unit - (IN) Device number.
 * Returns:
 * CMD_XXX
 */
STATIC int
_l3_cmd_info_show(int unit)
{
    int rv, i;
    struct resource
    {
        char *name;
        bcm_switch_object_t cur_max[2];
    };

    struct resource l3_resources[] =
    {{"hosts", {bcmSwitchObjectL3HostCurrent, bcmSwitchObjectL3HostV4Max}},
    {"routes", {bcmSwitchObjectL3RouteCurrent, -1}},
    {"nexthops", {bcmSwitchObjectL3EgressCurrent,
                  bcmSwitchObjectL3EgressMax}},
    {"ecmp_groups", {bcmSwitchObjectEcmpCurrent, bcmSwitchObjectEcmpMax}}};

    size_t num_resources = sizeof(l3_resources)/sizeof(l3_resources[0]);

    /* Get V4RoutesMinGuaranteed if ALPM is enabled else get V4RouteMax. */
    if (soc_property_get(unit, spn_L3_ALPM_ENABLE, 0))
    {
        l3_resources[1].cur_max[1] = bcmSwitchObjectL3RouteV4RoutesMinGuaranteed;
    } else {
        l3_resources[1].cur_max[1] = bcmSwitchObjectL3RouteV4RoutesMax;
    }

    for (i = 0; i < num_resources; i++) {
        int output[2];
        rv = bcm_switch_object_count_multi_get(
                unit, 2, l3_resources[i].cur_max, output);

        if (BCM_FAILURE(rv))
        {
            cli_out("Error L3 info: %s\n", bcm_errmsg(rv));
            return CMD_FAIL;
        }

        cli_out("%-16s : %6d / %6d\n", l3_resources[i].name, output[0],
                output[1]);
    }

    return CMD_OK;
}

/*
 * Function:
 *     _nat_ingress_traverse_cb
 * Description:
 *    Internal function to print out ingress nat info.
 * Parameters:
 *    unit      - (IN) Device number.
 *    index    - (IN) Ingress NAT index. 
 *    nat_info - (IN) Ingress NAT structure.
 *    ign       - (IN) Ingress NAT user data.
 */
int
_nat_ingress_traverse_cb(int unit, int index, 
                            bcm_l3_nat_ingress_t *nat_info, void *ign)
{
    char ip_str[IP6ADDR_STR_LEN];
    
    format_ipaddr(ip_str, nat_info->ip_addr);
    cli_out("%-5d %-5d %-16s %-5d %-5d %-5d %-5d\n", 
            index, nat_info->nat_id, ip_str, nat_info->vrf, nat_info->l4_port, 
            nat_info->ip_proto, nat_info->nexthop);
    return BCM_E_NONE;
}

/*
 * Function:
 *     _nat_ingress_traverse_cb
 * Description:
 *    Internal function to print out ingress nat info.
 * Parameters:
 *    unit      - (IN) Device number.
 *    index    - (IN) Engress NAT index. 
 *    nat_info - (IN) Engress NAT structure.
 *    ign       - (IN) Engress NAT user data.
 */
int
_nat_egress_traverse_cb(int unit, int index, 
                            bcm_l3_nat_egress_t *nat_info, void *ign)
{
    char ip_str[IP6ADDR_STR_LEN];
    char ip_mask_str[IP6ADDR_STR_LEN];
    uint32 flags = nat_info->flags;
    uint32 ref_count = 0;

    _bcm_esw_l3_nat_egress_reference_get(unit, nat_info->nat_id,
                                         &ref_count);
    if(flags & BCM_L3_NAT_EGRESS_SNAT)
    {
        format_ipaddr(ip_str, nat_info->sip_addr);
        format_ipaddr(ip_mask_str, nat_info->sip_addr_mask);
        cli_out("%-5d %-5s %-5d %-16s %-16s %-6d %-8d\n",
                index, "SNAT", nat_info->nat_id, ip_str, 
                ip_mask_str, nat_info->src_port, ref_count);
    }
    if(flags & BCM_L3_NAT_EGRESS_DNAT)
    {
        format_ipaddr(ip_str, nat_info->dip_addr);
        format_ipaddr(ip_mask_str, nat_info->dip_addr_mask);
        cli_out("%-5d %-5s %-5d %-16s %-16s %-6d %-8d\n",
                index, "DNAT", nat_info->nat_id, ip_str, 
                ip_mask_str, nat_info->dst_port, ref_count);
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _l3_cmd_nat_ingress_show
 * Description:
 *     Service routine used to show net entries. 
 * Parameters:
 *      unit      - (IN) Device number.
 *      a         - (IN) Command arguments. 
 * Returns: 
 *      CMD_XXX
 */
STATIC int
_l3_cmd_nat_ingress_show(int unit, args_t *a)
{
    int rv;

    uint32 flags = 0;

    /* -1 is special value to say all entries */
    cli_out("%-5s %-5s %-16s %-5s %-6s %-5s %-5s\n", "IDX", 
            "ID", "IP", "VRF", "L4Port", "IPProto", "NextHop");
    cli_out("*****SNAT Entries*****\n");
    rv = bcm_l3_nat_ingress_traverse( unit, flags, 0, -1, _nat_ingress_traverse_cb, NULL );
    if( BCM_FAILURE( rv ) ) {
        cli_out("Failed in ingress_traverse: %s\n", bcm_errmsg(rv));
        return rv;
    }

    flags = BCM_L3_NAT_INGRESS_TYPE_NAPT;
    rv = bcm_l3_nat_ingress_traverse( unit, flags, 0, -1, _nat_ingress_traverse_cb, NULL );
    if( BCM_FAILURE( rv ) ) {
        cli_out("Failed in ingress_traverse: %s\n", bcm_errmsg(rv));
        return rv;
    }

    cli_out("*****DNAT Entries*****\n");
    flags = BCM_L3_NAT_INGRESS_DNAT;
    rv = bcm_l3_nat_ingress_traverse( unit, flags, 0, -1, _nat_ingress_traverse_cb, NULL );
    if( BCM_FAILURE( rv ) ) {
        cli_out("Failed in ingress_traverse: %s\n", bcm_errmsg(rv));
        return rv;
    }
    flags = BCM_L3_NAT_INGRESS_DNAT | BCM_L3_NAT_INGRESS_TYPE_NAPT;
    rv = bcm_l3_nat_ingress_traverse( unit, flags, 0, -1, _nat_ingress_traverse_cb, NULL );
    if( BCM_FAILURE( rv ) ) {
        cli_out("Failed in ingress_traverse: %s\n", bcm_errmsg(rv));
        return rv;
    }

    cli_out("*****DNAT Pool Entries*****\n");
    flags = BCM_L3_NAT_INGRESS_DNAT | BCM_L3_NAT_INGRESS_DNAT_POOL;
    rv = bcm_l3_nat_ingress_traverse( unit, flags, 0, -1, _nat_ingress_traverse_cb, NULL );
    if( BCM_FAILURE( rv ) ) {
        cli_out("Failed in ingress_traverse: %s\n", bcm_errmsg(rv));
        return rv;
    }
    flags = BCM_L3_NAT_INGRESS_DNAT
            | BCM_L3_NAT_INGRESS_DNAT_POOL
            | BCM_L3_NAT_INGRESS_TYPE_NAPT;
    rv = bcm_l3_nat_ingress_traverse( unit, flags, 0, -1, _nat_ingress_traverse_cb, NULL );
    if( BCM_FAILURE( rv ) ) {
        cli_out("Failed in ingress_traverse: %s\n", bcm_errmsg(rv));
        return rv;
    }
        
    return CMD_OK;
}

/*
 * Function:
 *      _l3_cmd_nat_egress_add
 * Description:
 *     Service routine used to add net entry. 
 * Parameters:
 *      unit      - (IN) Device number.
 *      a         - (IN) Command arguments. 
 * Returns: 
 *      CMD_XXX
 */
STATIC int
_l3_cmd_nat_egress_add(int unit, args_t *a)
{
    parse_table_t     pt;     
    cmd_result_t      retCode; 
    bcm_ip_t          sip_addr = 0;
    bcm_ip_t          dip_addr = 0;
    bcm_ip_t          sip_mask = 0;
    bcm_ip_t          dip_mask = 0;
    int               src_port = 0;
    int               dst_port = 0;
    int               dnat     = 0;
    int               rv; 
    bcm_l3_nat_egress_t nat_info;


    parse_table_init(unit, &pt);
    parse_table_add(&pt, "DIP",  PQ_DFL | PQ_IP, 0, (void *)&dip_addr, 0);
    parse_table_add(&pt, "SIP",  PQ_DFL | PQ_IP, 0, (void *)&sip_addr, 0);
    parse_table_add(&pt, "DIPMASK",  PQ_DFL | PQ_IP, 0, (void *)&dip_mask, 0);
    parse_table_add(&pt, "SIPMASK",  PQ_DFL | PQ_IP, 0, (void *)&sip_mask, 0);
    parse_table_add(&pt, "SRCPORT", PQ_DFL | PQ_INT, 0, (void *)&src_port, 0);
    parse_table_add(&pt, "SRCPORT", PQ_DFL | PQ_INT, 0, (void *)&dst_port, 0);
    parse_table_add(&pt, "DNAT", PQ_DFL | PQ_INT, 0, (void *)&dnat, 0);
    if (!parseEndOk(a, &pt, &retCode)) {
        return retCode;
    }

    bcm_l3_nat_egress_t_init( &nat_info );
    if( dnat ) {
        nat_info.flags = BCM_L3_NAT_EGRESS_DNAT;
    }
    else {
        nat_info.flags = BCM_L3_NAT_EGRESS_SNAT;
    }
    nat_info.sip_addr = sip_addr;
    nat_info.dip_addr = dip_addr;
    nat_info.sip_addr_mask = sip_mask;
    nat_info.dip_addr_mask = dip_mask;
    nat_info.src_port = src_port;
    nat_info.dst_port = dst_port;

    rv = bcm_l3_nat_egress_add(unit, &nat_info);
    if (BCM_FAILURE(rv)) {
        cli_out("Egress Add failed: %s\n", bcm_errmsg(rv));
        return rv;
    }
    cli_out("NAT ID: %d\n", nat_info.nat_id);

    return CMD_OK;
}

/*
 * Function:
 *      _l3_cmd_nat_egress_show
 * Description:
 *     Service routine used to show net egress info. 
 * Parameters:
 *      unit      - (IN) Device number.
 *      a         - (IN) Command arguments. 
 * Returns: 
 *      CMD_XXX
 */
STATIC int
_l3_cmd_nat_egress_show(int unit, args_t *a)
{
    int rv;

    uint32 flags = 0;

    cli_out("%-5s %-5s %-5s %-16s %-16s %-6s %-8s\n", "IDX",
            "TYPE", "ID", "IP", "MASK", "L4Port", "RefCount");
    /* -1 is special value to say all entries */
    rv = bcm_l3_nat_egress_traverse( unit, flags, 0, -1, _nat_egress_traverse_cb, NULL );
    if( BCM_FAILURE( rv ) ) {
        cli_out("Failed in egress_traverse: %s\n", bcm_errmsg(rv));
        return rv;
    }

    return CMD_OK;
}
 


/*
 * Function:
 *      _l3_cmd_route_print
 * Description:
 *	Internal function to print out route info
 * Parameters:
 *      unit   - device number.
 *      index  - Traversal index number
 *	info   - Pointer to bcm_l3_route_t data structure.
 *      cookie - user cookie
 */
STATIC int
_l3_cmd_route_print(int unit, int index, bcm_l3_route_t *info, void *cookie)
{
    char ip_str[IP6ADDR_STR_LEN + 3];
    char if_mac_str[SAL_MACADDR_STR_LEN];
    char vrf_str[20];
    char *hit;
    char *trunk;
    int masklen;
    int priority;

    hit = (info->l3a_flags & BCM_L3_HIT) ? "y" : "n";
    trunk = (info->l3a_flags & BCM_L3_TGID) ? "t" : " ";
    priority = info->l3a_pri;

    switch (info->l3a_vrf){
      case BCM_L3_VRF_GLOBAL:
          /* coverity[secure_coding : FALSE] */
          sal_strcpy(vrf_str,"Global"); 
          break;
      case BCM_L3_VRF_OVERRIDE:
          /* coverity[secure_coding : FALSE] */
          sal_strcpy(vrf_str,"Override"); 
          break;
      default:
          sal_sprintf(vrf_str,"%d",info->l3a_vrf);
    }

    format_macaddr(if_mac_str, info->l3a_nexthop_mac);

    if (info->l3a_flags & BCM_L3_IP6) {
        format_ip6addr(ip_str, info->l3a_ip6_net);
        masklen = bcm_ip6_mask_length(info->l3a_ip6_mask);
        cli_out("%-5d %-8s %-s/%d %18s %4d %4d %5d%1s %4d %4d %s", index, vrf_str,
                ip_str, masklen, if_mac_str, info->l3a_intf,
                info->l3a_modid, info->l3a_port_tgid, trunk, priority, 
                info->l3a_lookup_class, hit);
    } else {
        format_ipaddr_mask(ip_str, info->l3a_subnet, info->l3a_ip_mask);
        cli_out("%-5d %-8s %-19s %18s %4d %4d %5d%1s %4d %4d %s", index, vrf_str,
                ip_str, if_mac_str, info->l3a_intf,
                info->l3a_modid, info->l3a_port_tgid, trunk, priority, 
                info->l3a_lookup_class, hit);

        if (SOC_CHIP_GROUP(unit) == SOC_CHIP_BCM5695 &&
            info->l3a_subnet == 0) {
            cli_out("   %d (Per VLAN)", info->l3a_vid);
        }
    }

    if (info->l3a_flags & BCM_L3_MULTIPATH) {
        cli_out("      (ECMP)");
    } else if (info->l3a_flags & BCM_L3_DEFIP_LOCAL) {
        cli_out("      (LOCAL ROUTE)");
    } 

    if (info->l3a_flags & BCM_L3_ROUTE_LABEL) {
        cli_out(" (MPLS)%d", info->l3a_mpls_label);
    } else if (info->l3a_tunnel_option) {
        cli_out(" (UDP tunnel) %d", info->l3a_tunnel_option);
    } 
        cli_out("\n");
    

    return BCM_E_NONE;
}


/*
 * Function:
 *      _l3_cmd_egress_intf_print
 * Description:
 *	Internal function to print out egress l3 interface info.
 * Parameters:
 *      unit      - (IN) Device number.
 *      print_hdr - (IN) Print header indicator. 
 *	    intf      - (IN) Pointer to bcm_l3_intf_t data structure.
 */
STATIC int
_l3_cmd_egress_intf_print(int unit, int print_hdr, bcm_l3_intf_t *intf)
{
    char if_mac_str[SAL_MACADDR_STR_LEN];

    if (NULL == intf) {
        return (BCM_E_PARAM);
    }

    if (print_hdr) {
        cli_out("Unit  Intf  VRF Group VLAN    Source Mac     MTU TTL Tunnel InnerVlan  NATRealm\n");
        cli_out("------------------------------------------------------------------\n");
    }

    format_macaddr(if_mac_str, intf->l3a_mac_addr);
    cli_out("%-5d %-5d %-5d %-5d %-4d %-18s %-4d %-4d %-5d %-5d %-5d\n", unit, 
            intf->l3a_intf_id, intf->l3a_vrf, intf->l3a_group, 
            intf->l3a_vid, if_mac_str, intf->l3a_mtu, intf->l3a_ttl,           
            intf->l3a_tunnel_idx, intf->l3a_inner_vlan, intf->l3a_nat_realm_id);           
    return (CMD_OK);
}


/*
 * Function:
 *      _l3_cmd_tunnel_init_print
 * Description:
 *	Internal function to print out tunnel initiator info.
 * Parameters:
 *      unit      - (IN) Device number.
 *      interface - (IN) L3 egress interface. 
 *	    info      - (IN) Pointer to bcm_tunnel_initiator_t data structure.
 */
STATIC int
_l3_cmd_tunnel_init_print (int unit, bcm_if_t interface, bcm_tunnel_initiator_t *info)
{
    char ip_str[IP6ADDR_STR_LEN + 3];
    char if_mac_str[SAL_MACADDR_STR_LEN];

    cli_out("Tunnel initiator:\n");
    cli_out("\tUnit        = %d\n", unit);
    cli_out("\tInterface   = %d\n", interface);
    cli_out("\tTUNNEL_TYPE = %d\n", info->type);
    cli_out("\tTTL         = %d\n", info->ttl);
    format_macaddr(if_mac_str, info->dmac);
    cli_out("\tMAC_ADDR    = %-18s\n", if_mac_str);
    if (_BCM_TUNNEL_OUTER_HEADER_IPV6(info->type)) { 
        format_ip6addr(ip_str, info->dip6);
        cli_out("\tDIP         = 0x%-s\n", ip_str);
        format_ip6addr(ip_str, info->sip6);
        cli_out("\tSIP         = 0x%-s\n", ip_str);
    } else { 
        cli_out("\tDIP         = 0x%08x\n", info->dip);
        cli_out("\tSIP         = 0x%08x\n", info->sip);
        if (info->flags & BCM_TUNNEL_INIT_USE_INNER_DF) {
            cli_out("\tCopy DF from inner header.\n");
        } else if (info->flags & BCM_TUNNEL_INIT_IPV4_SET_DF) {
            cli_out("\tForce DF to 1 for ipv4 payload.\n");
        }
        if (info->flags & BCM_TUNNEL_INIT_IPV6_SET_DF) {
            cli_out("\tForce DF to 1 for ipv6 payload.\n");
        }
    }
    cli_out("\tDSCP_SEL    = 0x%x\n", info->dscp_sel);
    cli_out("\tDSCP        = 0x%x\n", info->dscp);
    cli_out("\tDSCP_MAP    = 0x%x\n", info->dscp_map);
    return CMD_OK;
}

/*
 * Function:
 *      _l3_cmd_tunnel_term_print
 * Description:
 *	Internal function to print out tunnel terminator info.
 * Parameters:
 *      unit      - (IN) Device number.
 *	    info      - (IN) Pointer to bcm_tunnel_terminator_t data structure.
 */
STATIC int
_l3_cmd_tunnel_term_print (int unit, bcm_tunnel_terminator_t *info)
{
    char ip_str[IP6ADDR_STR_LEN + 3];
    char buf_pbmp[SOC_PBMP_FMT_LEN];

    cli_out("Tunnel terminator:\n");
    cli_out("\tUnit        = %d\n", unit);
    cli_out("\tTUNNEL_TYPE = %d\n", info->type);
    cli_out("\tVRF         = %d\n", info->vrf);
    if (_BCM_TUNNEL_OUTER_HEADER_IPV6(info->type)) { 
        format_ip6addr(ip_str, info->dip6);
        cli_out("\tDIP         = 0x%-s\n", ip_str);
        format_ip6addr(ip_str, info->dip6_mask);
        cli_out("\tDIP MASK    = 0x%-s\n", ip_str);
        format_ip6addr(ip_str, info->sip6);
        cli_out("\tSIP         = 0x%-s\n", ip_str);
        format_ip6addr(ip_str, info->sip6_mask);
        cli_out("\tSIP MASK    = 0x%-s\n", ip_str);
    } else { 
        cli_out("\tDIP         = 0x%08x\n", info->dip);
        cli_out("\tDIP_MASK    = 0x%08x\n", info->dip_mask);
        cli_out("\tSIP         = 0x%08x\n", info->sip);
        cli_out("\tSIP_MASK    = 0x%08x\n", info->sip_mask);
    }


    cli_out("\tL4 Destination Port    = 0x%08x\n", info->udp_dst_port);
    cli_out("\tL4 Source Port         = 0x%08x\n", info->udp_src_port);
    cli_out("\tVlan id                = 0x%08x\n", info->vlan);
    format_pbmp(unit, buf_pbmp, sizeof(buf_pbmp), info->pbmp);
    cli_out("\tPbmp                   = 0x%s\n", buf_pbmp);

    if (info->flags & BCM_TUNNEL_TERM_USE_OUTER_DSCP) {
        cli_out("\tCopy DSCP from outer ip header.\n");
    } else if (info->flags & BCM_TUNNEL_TERM_KEEP_INNER_DSCP) {
        cli_out("\tCopy DSCP from inner ip header.\n");
    } 


    if (info->flags & BCM_TUNNEL_TERM_DSCP_TRUST) {
        cli_out("\tTrust incming DSCP based on inner/outer selection.\n");
    }

    if (info->flags & BCM_TUNNEL_TERM_USE_OUTER_TTL) {
        cli_out("\tCopy TTL from outer ip header.\n");
    } 
    return CMD_OK;
}


/*
 * Function:
 *      _l3_cmd_egress_mpath_obj_print
 * Description:
 *	 Internal function to print out multipath egress object info
 * Parameters:
 *      unit        - device number.
 *      index       - Egress object id.
 *      intf_count  - Interface count.
 *      info        - Pointer to interfaces array.
 *      user_data   - User cookie.
 */
STATIC int
_l3_cmd_egress_mpath_obj_print(int unit, int index, int intf_count, bcm_if_t
                               *info, void *user_data)
{
    int    idx;
    uint32 ref_count;

    cli_out("Multipath Egress Object %d\n", index);

    cli_out("Interfaces:");

    for (idx = 0; idx < intf_count; idx++) {
        cli_out(" %d", info[idx]);
        if (idx && (!(idx % 10))) {
            cli_out("\n           ");
        }
    }

    /*
     * Get the reference count for the multipath object. This ref count
     * indicates the number of route references for this egress object. This is
     * a read only object and used for debugging purposes. Hence, this is not
     * exposed in the form of an API and retrieved internally
     */
    _bcm_esw_l3_egress_reference_get(unit, index, 1, &ref_count);

    cli_out("\nReference count: %d", ref_count);
    cli_out("\n");
    return BCM_E_NONE;
}

/*
 * Function:
 *      _l3_cmd_egress_ecmp_obj_print
 * Description:
 *	 Internal function to print out ecmp egress object info
 * Parameters:
 *      unit        - device number.
 *      ecmp        - ECMP group info.
 *      intf_count  - Interface count. 
 *      intf_array  - Array of Egress forwarding objects.
 *      user_data   - User cookie. 
 */
STATIC int
_l3_cmd_egress_ecmp_obj_print(int unit, bcm_l3_egress_ecmp_t *ecmp,
                               int intf_count, bcm_if_t *intf_array,
                               void *user_data)
{
    int    idx; 
    uint32 ref_count;

    cli_out("ECMP Egress Object %d\n", ecmp->ecmp_intf);

    cli_out("Interfaces:");
 
    for (idx = 0; idx < intf_count; idx++) {
        cli_out(" %d", *intf_array);
        if (idx && (!(idx % 10))) {
            cli_out("\n           ");
        } 
        intf_array++;
    }

    /*
     * Get the reference count for the ecmp object. This ref count
     * indicates the number of route references for this egress object. This is
     * a read only object and used for debugging purposes. Hence, this is not
     * exposed in the form of an API and retrieved internally
     */
    _bcm_esw_l3_egress_reference_get(unit, ecmp->ecmp_intf, 1, &ref_count);

    cli_out("\nReference count: %d", ref_count);
    cli_out("\n");
    return BCM_E_NONE;
}

/*
 * Function:
 *      _l3_cmd_egress_obj_print
 * Description:
 *	 Internal function to print out egress object info
 * Parameters:
 *      unit   - device number.
 *      index  - Egress object id. 
 *   	info   - Pointer to bcm_l3_egres_t data structure.
 *      cookie - user cookie
 */
STATIC int
_l3_cmd_egress_obj_print(int unit, int index, bcm_l3_egress_t *info, void *cookie)
{
    char if_mac_str[SAL_MACADDR_STR_LEN];
    char *trunk;
    int     port, tgid, module, id;
    uint32  ref_count = 0;

    trunk = (info->flags & BCM_L3_TGID) ? "t" : " ";

    format_macaddr(if_mac_str, info->mac_addr);
    if (BCM_GPORT_IS_SET(info->port)) {
        BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit, info->port, &module, &port, &tgid, &id));
        if (id != -1) {
            port = id;
        }
    } else {
        port = info->port;
        module = info->module;
        tgid = info->trunk;
    }

    /*
     * Get the reference count for the egress object. This ref count indicates
     * the number of route references for this egress object. This is a read
     * only object and used for debugging purposes. Hence, this is not exposed
     * in the form of an API and retrieved internally
     */
    _bcm_esw_l3_egress_reference_get(unit, index, 0, &ref_count);

    BCM_IF_ERROR_RETURN(_bcm_esw_l3_egress_ref_dlb_update
                                      (unit, index, &ref_count));

    cli_out("%d %18s %4d %4d %5d%1s %3d %9d %4s %4s %4u %4s\n", index,
            if_mac_str, info->vlan, info->intf, 
            ((info->flags & BCM_L3_TGID) ? tgid : port),
            trunk, module, info->mpls_label, 
            (info->flags & BCM_L3_COPY_TO_CPU) ? "yes" : "no" , 
            (info->flags & BCM_L3_DST_DISCARD) ? "yes" : "no" ,
            ref_count,
            (info->flags & BCM_L3_IPMC) ? "yes" : "no");

    return BCM_E_NONE;
}

/*
 * Function:
 *      _l3_cmd_host_print
 * Description:
 *	Internal function to print out host entry info
 * Parameters:
 *      unit   - device number.
 *      index  - Traversal index number
 *	info   - Pointer to bcm_l3_host_t data structure.
 *      cookie - user cookie
 */
STATIC int
_l3_cmd_host_print(int unit, int index, bcm_l3_host_t *info, void *cookie)
{
    char ip_str[IP6ADDR_STR_LEN];
    char if_mac_str[SAL_MACADDR_STR_LEN];
    char *hit;
    char *trunk = " ";
    int     tgid=0, port, module, id;

    hit = (info->l3a_flags & BCM_L3_HIT) ? "y" : "n";
    
    format_macaddr(if_mac_str, info->l3a_nexthop_mac);
    if (BCM_GPORT_IS_SET(info->l3a_port_tgid)) {
        BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit, info->l3a_port_tgid, &module, 
                               &port, &tgid, &id));
#ifdef BCM_RIOT_SUPPORT
        /* VFI type virtual port */
        if (_bcm_vp_is_vfi_type(unit, info->l3a_port_tgid)) {
            port = id;
        } else
#endif
        {
            if (!BCM_L3_BK_FLAG_GET(unit, BCM_L3_BK_ENABLE_MACDA_OUI_PROFILE)) {
                /* For Tomahawk L3/NIV/PE extended view,ignore id = -1 check
                 * as id > 0 is returned for VPs in extended view
                 */
                if (id != -1) {
                    return CMD_USAGE;
                }
            }
        }
        if (tgid != -1) {
            trunk = "t";
        }
    } else {
        if (info->l3a_flags & BCM_L3_TGID) {
            trunk = "t" ; 
            tgid = info->l3a_port_tgid;
            port = -1;
        } else {
            port = info->l3a_port_tgid;
        }
        module = info->l3a_modid;
    }
    
    if (info->l3a_flags & BCM_L3_IP6) {
        format_ip6addr(ip_str, info->l3a_ip6_addr);
        cli_out("%-5d %-4d %-42s %18s %4d %4d %5d%1s   %6d %s      %-5d\n",
                index, info->l3a_vrf, ip_str, if_mac_str, info->l3a_intf,
                module, (port == -1) ? tgid : port, trunk,
                info->l3a_lookup_class, hit, *(int *)cookie);
    } else {
        format_ipaddr(ip_str, info->l3a_ip_addr);
        cli_out("%-5d %-4d %-16s %-18s %4d %4d %4d%1s   %6d %s      %-5d",
                index, info->l3a_vrf, ip_str, if_mac_str, info->l3a_intf,
                module, (port == -1) ? tgid : port, trunk,
                info->l3a_lookup_class, hit, *(int *)cookie);
    }

    if (info->l3a_flags & BCM_L3_MULTIPATH) {
        cli_out("      (ECMP)\n");
    } else if (info->l3a_flags & BCM_L3_HOST_LOCAL) {
        cli_out("      (LOCAL ROUTE)\n");    
    } else { 
        cli_out("\n");
    }
     
    return BCM_E_NONE;
}


/*
 * Function:
 *      _l3_cmd_l3_key_print
 * Description:
 *	Internal function to print out host entry info
 * Parameters:
 *      unit   - device number.
 *      index  - Traversal index number
 */
STATIC int
_l3_cmd_l3_key_print(int unit, int index, bcm_l3_key_t *info)
{
    char dip_str[IP6ADDR_STR_LEN];
    char sip_str[IP6ADDR_STR_LEN];

    if (info->l3k_flags & BCM_L3_IPMC) {
        if (info->l3k_flags & BCM_L3_IP6) {
            format_ip6addr(dip_str, info->l3k_ip6_addr);
            format_ip6addr(sip_str, info->l3k_sip6_addr);
            cli_out("%-5d Group %-42s\n", index, dip_str);
            cli_out("      Src %-42s\n", sip_str);
            cli_out("      Vrf %-4d VLAN %-4d\n", info->l3k_vrf, info->l3k_vid);
        } else {
            format_ipaddr(dip_str, info->l3k_ip_addr);
            format_ipaddr(sip_str, info->l3k_sip_addr);
            cli_out("%-5d Group %-18s  Src %-18s Vrf %-4d Vlan %-4d\n", 
                    index, dip_str, sip_str, info->l3k_vrf, info->l3k_vid);
        }
    } else {
        if (info->l3k_flags & BCM_L3_IP6) {
            format_ip6addr(dip_str, info->l3k_ip6_addr);
            cli_out("%-5d VRF %-4d DIP %-18s\n",
                    index, info->l3k_vrf, dip_str);
        } else {
            format_ipaddr(dip_str, info->l3k_ip_addr);
            cli_out("%-5d VRF %-4d DIP %-18s\n", 
                    index, info->l3k_vrf, dip_str);
        }
    }
    return CMD_OK;
}


/*
 * Function:
 *      _l3_cmd_host_age_callback
 * Description:
 *	Internal function to print out host entry being aged out
 * Parameters:
 *      unit   - device number.
 *      index  - Traversal index number
 *	info   - Pointer to bcm_l3_host_t data structure.
 *      cookie - user cookie
 */
STATIC int
_l3_cmd_host_age_callback(int unit, int index, bcm_l3_host_t *info, void *cookie)
{
    if (info->l3a_flags & BCM_L3_IP6) {
        char ip_str[IP6ADDR_STR_LEN];

        format_ip6addr(ip_str, info->l3a_ip6_addr);
        cli_out("IPv6 Address %s age out\n", ip_str);
    } else {
        cli_out("IP Address 0x%x age out\n", info->l3a_ip_addr);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _l3_cmd_route_age_callback
 * Description:
 *	Internal function to print out route being aged (HIT bit cleared)
 * Parameters:
 *      unit   - device number.
 *      index  - Traversal index number
 *	info   - Pointer to bcm_l3_host_t data structure.
 *      cookie - user cookie
 */
STATIC int
_l3_cmd_route_age_callback(int unit, int index, bcm_l3_route_t *net, void *cookie)
{
    if (net->l3a_flags & BCM_L3_IP6) {
        char ip_str[IP6ADDR_STR_LEN];
        int masklen;

        format_ip6addr(ip_str, net->l3a_ip6_net);
        masklen = bcm_ip6_mask_length(net->l3a_ip6_mask);
        cli_out("Deleted unused route %s/%d)\n", ip_str, masklen);
    } else {
        cli_out("Deleted unused route 0x%x(mask 0x%x)\n",
                net->l3a_subnet, net->l3a_ip_mask);
    }

    return BCM_E_NONE;
}

/*
 * L3 command catagory
 */
enum _l3_cmd_t {
    L3_ADD = 1,
    L3_DESTROY,
    L3_SET,
    L3_CLEAR,
    L3_SHOW,
    L3_AGE,
    L3_UPDATE,
    L3_HASH,
    L3_IP6HASH,
    L3_SANITY,
    L3_ECMP,
    L3_ECMP_HASH, 
    L3_INSERT, 
    L3_REMOVE, 
    L3_FIND,
    L3_CONFLICT,
    L3_TEST,
    L3_MAXADD
};

const char *cmd_tunnel_term_parse_type[] = {
    "6to4",
    "6to4-unchecked",
    "isatap",
    "ipip",
    NULL
};


#if defined(BCM_XGS_SWITCH_SUPPORT)
/*
 * Function:
 *      _l3_cmd_l3_conflict_show
 * Description:
 * 	    Service routine used to show l3 table conflicting entries. 
 * Parameters:
 *      unit      - (IN) Device number.
 *      a         - (IN) Command arguments. 
 * Returns: 
 *      CMD_XXX
 */

STATIC cmd_result_t
_l3_cmd_l3_conflict_show(int unit, args_t *a) 
{
    parse_table_t     pt;     
    cmd_result_t      retCode; 
    bcm_ip6_t         sip6_addr;
    bcm_ip6_t         ip6_addr;
    bcm_ip_t          sip_addr = 0;
    bcm_ip_t          ip_addr = 0;
    int               vlan = 0;
    int               ipmc = 0;
    int               ip6 = 0;
    bcm_vrf_t         vrf = BCM_L3_VRF_DEFAULT;
    bcm_l3_key_t      ipkey;
    bcm_l3_key_t      result[L3X_MAX_BUCKET_SIZE];
    int               count;  
    int               idx;
    int               rv; 

    /* Stack variables initialization. */
    sal_memset(sip6_addr, 0, sizeof(bcm_ip6_t));
    sal_memset(ip6_addr, 0, sizeof(bcm_ip6_t));
    sal_memset(result, 0, L3X_MAX_BUCKET_SIZE * sizeof(bcm_l3_key_t));

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "DIP",  PQ_DFL | PQ_IP, 0, (void *)&ip_addr, 0);
    parse_table_add(&pt, "SIP",  PQ_DFL | PQ_IP, 0, (void *)&sip_addr, 0);
    parse_table_add(&pt, "DIP6", PQ_DFL | PQ_IP6, 0, (void *)&ip6_addr, 0);
    parse_table_add(&pt, "SIP6", PQ_DFL | PQ_IP6, 0, (void *)&sip6_addr, 0);
    parse_table_add(&pt, "VRF",  PQ_DFL | PQ_INT, 0, (void *)&vrf, 0);
    parse_table_add(&pt, "VLAN", PQ_DFL | PQ_INT, 0, (void *)&vlan, 0);
    parse_table_add(&pt, "IPMC", PQ_DFL | PQ_INT, 0, (void *)&ipmc, 0);
    parse_table_add(&pt, "IP6",  PQ_DFL | PQ_INT, 0, (void *)&ip6, 0);
    if (!parseEndOk(a, &pt, &retCode)) {
        return retCode;
    }

    /* Initialize data structures. */
    sal_memset(&ipkey, 0, sizeof(bcm_l3_key_t));

    /* Fill tunnel info. */
    if (ip6) { 
        ipkey.l3k_flags |= BCM_L3_IP6; 
        sal_memcpy(ipkey.l3k_ip6_addr, ip6_addr, sizeof(bcm_ip6_t));
        sal_memcpy(ipkey.l3k_sip6_addr, sip6_addr, sizeof(bcm_ip6_t));
    } else {
        ipkey.l3k_ip_addr = ip_addr;
        ipkey.l3k_sip_addr = sip_addr;
    }

    if (ipmc) { 
       ipkey.l3k_flags |= BCM_L3_IPMC; 
    }
    ipkey.l3k_vrf = vrf;
    ipkey.l3k_vid = vlan & 0xfff;

    if ((rv = bcm_l3_host_conflict_get(unit, &ipkey, result, 
                                       L3X_MAX_BUCKET_SIZE, &count)) < 0) {
        cli_out("ERROR %s: getting l3 conflicts %s\n", 
                ARG_CMD(a), bcm_errmsg(rv));
        return CMD_FAIL;
    }

    for (idx = 0; idx < count; idx++) {
        _l3_cmd_l3_key_print(unit, idx, result + idx);
    }
    return CMD_OK;
}

/*
 * Function:
 *      _l3_cmd_defip_ecmp_hash
 * Description:
 * 	    Service routine used to calculate route ecmp hash 
 * Parameters:
 *      unit      - (IN) Device number.
 *      a         - (IN) Command arguments. 
 * Returns: 
 *      CMD_XXX
 */
STATIC cmd_result_t
_l3_cmd_defip_ecmp_hash(int unit, args_t *a) 
{
    uint32                 l4_dst_port = 0;
    uint32                 l4_src_port = 0;
    int                    ecmp_count = 0;
    bcm_ip6_t              dip6_addr;
    bcm_ip6_t              sip6_addr;
    bcm_ip_t               dip_addr = 0;
    bcm_ip_t               sip_addr = 0;
    cmd_result_t           retCode;
    parse_table_t          pt;     
    int                    rv;
    int                    ip6 = 0;
    int                    hash_select = XGS_HASH_COUNT;
    int                    ecmp_hash_sel = 2;
#ifdef BCM_XGS3_SWITCH_SUPPORT
    soc_xgs3_ecmp_hash_t   data; 
#endif /* BCM_XGS3_SWITCH_SUPPORT */    

    if (!SOC_IS_XGS_SWITCH(unit)) {
        cli_out("Command only valid for XGS switches\n");
        return CMD_FAIL;
    }

    sal_memset(dip6_addr, 0, sizeof (bcm_ip6_t));
    sal_memset(sip6_addr, 0, sizeof (bcm_ip6_t));

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "DIP", PQ_DFL | PQ_IP, 0, (void *)&dip_addr, 0);
    parse_table_add(&pt, "SIP", PQ_DFL | PQ_IP, 0, (void *)&sip_addr, 0);
    parse_table_add(&pt, "DIP6", PQ_DFL | PQ_IP6, 0, (void *)dip6_addr, 0);
    parse_table_add(&pt, "SIP6", PQ_DFL | PQ_IP6, 0, (void *)sip6_addr, 0);
    parse_table_add(&pt, "IP6", PQ_DFL | PQ_INT, 0, (void *)&ip6, 0);
    parse_table_add(&pt, "L4_SRC_PORT", PQ_DFL | PQ_INT, 0,
                    (void *)&l4_dst_port, 0);
    parse_table_add(&pt, "L4_DST_PORT", PQ_DFL | PQ_INT, 0, 
                    (void *)&l4_src_port, 0);
    parse_table_add(&pt, "COUNT", PQ_DFL | PQ_INT, 0,
                    (void *)&ecmp_count, 0);
    parse_table_add(&pt, "EHash", PQ_DFL | PQ_INT, 0,
                    (void *)&ecmp_hash_sel, 0);
    parse_table_add(&pt, "Hash", PQ_DFL | PQ_INT, 0,
                    (void *)&hash_select, 0);
    if (!parseEndOk(a, &pt, &retCode)) {
        return retCode;
    }

#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit)) {   /* FBX || ER */
        sal_memset(&data, 0, sizeof (soc_xgs3_ecmp_hash_t));
        /* Fill source/destination address. . */
        if (ip6) { 
            sal_memcpy(data.dip6, dip6_addr, sizeof(bcm_ip6_t));
            sal_memcpy(data.sip6, sip6_addr, sizeof(bcm_ip6_t));
            data.v6 = TRUE;
        } else {
            data.dip = dip_addr;
            data.sip = sip_addr;
        }
        data.l4_src_port = l4_src_port & 0xffff;  /* TCP/UDP src port. */
        data.l4_dst_port = l4_dst_port & 0xffff;  /* TCP/UDP dst port. */
        data.ecmp_count = ecmp_count;      /* Zero based ecmp count. */

        rv = soc_xgs3_l3_ecmp_hash(unit, &data);
        if (rv < 0){
            cli_out("Internal error\n");
            return CMD_FAIL;
        } 
        cli_out("ECMP hash is %d \n", rv);
        return CMD_OK; 
    } else 
#endif /* BCM_XGS3_SWITCH_SUPPORT */
    if (ip6) {
        cli_out("IPv6 is not supported.\n");
        return CMD_OK; 
    }

    cli_out("Command is not supported.\n"); 
    return CMD_FAIL;
}
#endif /* BCM_XGS_SWITCH_SUPPORT*/

/*
 * Function:
 *      _l3_cmd_tunnel_init_add
 * Description:
 * 	    Service routine used to add tunnel initiator to an interface.
 * Parameters:
 *      unit      - (IN) Device number.
 *      a         - (IN) Command arguments. 
 * Returns: 
 *      CMD_XXX
 */

STATIC cmd_result_t
_l3_cmd_tunnel_init_add(int unit, args_t *a) 
{
    bcm_tunnel_initiator_t tunnel_init;
    bcm_l3_intf_t     intf;
    bcm_tunnel_type_t type = 0;
    parse_table_t     pt;     
    cmd_result_t      retCode; 
    bcm_ip6_t         sip6_addr;
    bcm_ip6_t         ip6_addr;
    bcm_mac_t         mac;
    bcm_if_t          interface = 0;
    bcm_ip_t          sip_addr = 0;
    bcm_ip_t          ip_addr = 0;
    int               ip4_df_sel = 0;
    int               ip6_df_sel = 0;
    int               dscp_val = 0;
    int               dscp_sel = 0;
    int               dscp_map = 0;
    int               ttl = 0;
    int               rv;


    /* Stack variables initialization. */
    sal_memset(&intf, 0, sizeof(bcm_l3_intf_t));
    sal_memset(sip6_addr, 0, sizeof(bcm_ip6_t));
    sal_memset(ip6_addr, 0, sizeof(bcm_ip6_t));
    sal_memset(mac, 0, sizeof(bcm_mac_t));

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "INtf", PQ_DFL|PQ_INT, 0, (void*)&interface, 0);
    parse_table_add(&pt, "TYpe", PQ_DFL|PQ_INT, 0, (void*)&type, 0);
    parse_table_add(&pt, "TTL",  PQ_DFL|PQ_INT, 0, (void*)&ttl, 0);
    parse_table_add(&pt, "Mac",  PQ_DFL|PQ_MAC, 0, (void *)mac, 0);
    parse_table_add(&pt, "DIP",  PQ_DFL|PQ_IP, 0, (void *)&ip_addr, 0);
    parse_table_add(&pt, "SIP",  PQ_DFL|PQ_IP, 0, (void *)&sip_addr, 0);
    parse_table_add(&pt, "DIP6", PQ_DFL | PQ_IP6, 0, (void *)&ip6_addr, 0);
    parse_table_add(&pt, "SIP6", PQ_DFL | PQ_IP6, 0, (void *)&sip6_addr, 0);
    parse_table_add(&pt, "DSCPSel", PQ_DFL|PQ_INT, 0, (void *)&dscp_sel, 0);
    parse_table_add(&pt, "DSCPV", PQ_DFL|PQ_INT, 0, (void *)&dscp_val, 0);
    parse_table_add(&pt, "DFSEL4", PQ_DFL|PQ_INT, 0, (void *)&ip4_df_sel, 0);
    parse_table_add(&pt, "DFSEL6", PQ_DFL|PQ_BOOL, 0, (void *)&ip6_df_sel, 0);
    parse_table_add(&pt, "DSCPMap", PQ_DFL|PQ_INT, 0, (void *)&dscp_map, 0);
    if (!parseEndOk(a, &pt, &retCode)) {
        return retCode;
    }

    /* Initialize data structures. */
    bcm_l3_intf_t_init(&intf);
    bcm_tunnel_initiator_t_init(&tunnel_init);

    /* Fill tunnel info. */
    intf.l3a_intf_id = interface;
    tunnel_init.type = type;
    tunnel_init.ttl = ttl;

    sal_memcpy(tunnel_init.dmac, mac, sizeof(mac));

    if (ip4_df_sel) { 
        tunnel_init.flags |= (ip4_df_sel > 1) ?
            BCM_TUNNEL_INIT_USE_INNER_DF : BCM_TUNNEL_INIT_IPV4_SET_DF;
    }
    if (ip6_df_sel) { 
        tunnel_init.flags |= BCM_TUNNEL_INIT_IPV6_SET_DF;
    }

    sal_memcpy(tunnel_init.dip6, ip6_addr, sizeof(bcm_ip6_t));
    sal_memcpy(tunnel_init.sip6, sip6_addr, sizeof(bcm_ip6_t));
    tunnel_init.dip = ip_addr;
    tunnel_init.sip = sip_addr;
    tunnel_init.dscp_sel = dscp_sel;
    tunnel_init.dscp = dscp_val;
    tunnel_init.dscp_map = dscp_map;

    if ((rv = bcm_tunnel_initiator_set(unit, &intf, &tunnel_init)) < 0) {
        cli_out("ERROR %s: setting tunnel initiator for %d %s\n",
                ARG_CMD(a), interface, bcm_errmsg(rv));
        return CMD_FAIL;
    }
    return CMD_OK;
}

/*
 * Function:
 *      _l3_cmd_tunnel_init_show
 * Description:
 * 	    Service routine used to get & show tunnel initiator info. 
 * Parameters:
 *      unit      - (IN) Device number.
 *      a         - (IN) Command arguments. 
 * Returns: 
 *      CMD_XXX
 */
STATIC cmd_result_t
_l3_cmd_tunnel_init_show(int unit, args_t *a) 
{
    bcm_tunnel_initiator_t tunnel_init;
    bcm_l3_intf_t          intf;
    parse_table_t          pt;     
    cmd_result_t           retCode; 
    bcm_if_t               interface;
    int                    rv;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "INtf", PQ_DFL|PQ_INT, 0, (void*)&interface, 0);
    if (!parseEndOk(a, &pt, &retCode)) {
        return retCode;
    }

    bcm_tunnel_initiator_t_init(&tunnel_init);

    bcm_l3_intf_t_init(&intf);
    intf.l3a_intf_id = interface;

    if ((rv = bcm_tunnel_initiator_get(unit, &intf, &tunnel_init)) < 0) {
        cli_out("ERROR %s: getting tunnel initiator for %d %s\n",
                ARG_CMD(a), interface, bcm_errmsg(rv));
        return CMD_FAIL;
    }

    _l3_cmd_tunnel_init_print(unit, interface, &tunnel_init);
    return CMD_OK;
}

/*
 * Function:
 *      _l3_cmd_tunnel_init_clear
 * Description:
 * 	    Service routine used to remove tunnel initiator from an interface.
 * Parameters:
 *      unit      - (IN) Device number.
 *      a         - (IN) Command arguments. 
 * Returns: 
 *      CMD_XXX
 */
STATIC cmd_result_t
_l3_cmd_tunnel_init_clear(int unit, args_t *a) 
{
    bcm_if_t               interface;
    cmd_result_t           retCode; 
    bcm_l3_intf_t          intf;
    parse_table_t          pt;     
    int                    rv;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "INtf", PQ_DFL|PQ_INT, 0, (void*)&interface, 0);
    if (!parseEndOk(a, &pt, &retCode)) {
        return retCode;
    }

    bcm_l3_intf_t_init(&intf);
    intf.l3a_intf_id = interface;

    if ((rv = bcm_tunnel_initiator_clear(unit, &intf)) < 0) {
        cli_out("ERROR %s: clearing tunnel initiator for %d %s\n",
                ARG_CMD(a), interface, bcm_errmsg(rv));
        return CMD_FAIL;
    }
    return CMD_OK;
}

/*
 * Function:
 *      _l3_cmd_tunnel_term_add
 * Description:
 * 	    Service routine used to add tunnel terminator entry.
 * Parameters:
 *      unit      - (IN) Device number.
 *      a         - (IN) Command arguments. 
 * Returns: 
 *      CMD_XXX
 */
STATIC cmd_result_t
_l3_cmd_tunnel_term_add(int unit, args_t *a) 
{
    bcm_tunnel_terminator_t tunnel_term;
    cmd_result_t      retCode; 
    parse_table_t     pt;     
    int               rv;
    bcm_pbmp_t        pbmap;
    bcm_ip6_t         sip6;
    bcm_ip6_t         dip6;
    int               vlan = 0;
    int               dip = 0;
    int               sip = 0;
    int               sip_mask_len = 0; 
    int               dip_mask_len = 0;
    int               dport = 0; 
    int               sport = 0; 
    int               tnl_type = 0;
    int               outer_ttl = 0;
    int               outer_dscp = 0;
    int               inner_dscp = 0;
    int               trust_dscp = 0; 
    int               allow_v4 = 0; 
    int               allow_v6 = 0;
    int               vrf = 0;
    int               replace=0;
    bcm_gport_t       remote_port = BCM_GPORT_INVALID;
    bcm_gport_t       tunnel_id = 0;

    parse_table_init(unit, &pt);
    sal_memset(sip6, 0, sizeof(bcm_ip6_t));
    sal_memset(dip6, 0, sizeof(bcm_ip6_t));
    sal_memset(&pbmap, 0, sizeof(bcm_pbmp_t));

    parse_table_add(&pt, "VRF",  PQ_DFL | PQ_INT, 0, (void *)&vrf, 0);
    parse_table_add(&pt, "DIP",  PQ_DFL|PQ_IP, 0, (void*)&dip, 0);
    parse_table_add(&pt, "SIP",  PQ_DFL|PQ_IP, 0, (void*)&sip, 0);
    parse_table_add(&pt, "DIP6",  PQ_DFL|PQ_IP6, 0, (void*)&dip6, 0);
    parse_table_add(&pt, "SIP6",  PQ_DFL|PQ_IP6, 0, (void*)&sip6, 0);
    parse_table_add(&pt, "DipMaskLen", PQ_DFL|PQ_INT, 0, (void*)&dip_mask_len, 0);
    parse_table_add(&pt, "SipMaskLen", PQ_DFL|PQ_INT, 0, (void*)&sip_mask_len, 0);
    parse_table_add(&pt, "L4DstPort", PQ_DFL|PQ_INT, 0, (void*)&dport, 0);
    parse_table_add(&pt, "L4SrcPort", PQ_DFL|PQ_INT, 0, (void*)&sport, 0);
    parse_table_add(&pt, "TYpe", PQ_DFL|PQ_INT, 0, (void*)&tnl_type, 0);
    parse_table_add(&pt, "OuterDSCP", PQ_DFL|PQ_BOOL, 0,(void *)&outer_dscp, 0);
    parse_table_add(&pt, "OuterTTL", PQ_DFL|PQ_BOOL, 0, (void *)&outer_ttl, 0);
    parse_table_add(&pt, "InnerDSCP", PQ_DFL|PQ_BOOL, 0,(void *)&inner_dscp, 0);
    parse_table_add(&pt, "TrustDSCP", PQ_DFL|PQ_BOOL, 0,(void *)&trust_dscp, 0);
    parse_table_add(&pt, "VLanid", PQ_DFL|PQ_INT, 0, (void*)&vlan, 0);
    parse_table_add(&pt, "AllowIPv6Payload", PQ_DFL|PQ_BOOL, 0, (void*)&allow_v4, 0);
    parse_table_add(&pt, "AllowIPv4Payload", PQ_DFL|PQ_BOOL, 0, (void*)&allow_v6, 0);
    parse_table_add(&pt, "PBMP", PQ_DFL|PQ_PBMP|PQ_BCM, 0, &pbmap, 0);
    parse_table_add(&pt, "Replace", PQ_DFL|PQ_BOOL, 0, &replace, 0);
    parse_table_add(&pt, "RemoteTerminate", PQ_DFL|PQ_INT, 0, &remote_port, 0);
    parse_table_add(&pt, "TunnelID", PQ_DFL|PQ_INT, 0, (void*)&tunnel_id, 0);

    if (!parseEndOk(a, &pt, &retCode)) {
        return retCode;
    }

    sal_memset(&tunnel_term, 0, sizeof (tunnel_term));

    if ((tnl_type == bcmTunnelTypeWlanWtpToAc) || 
        (tnl_type == bcmTunnelTypeWlanAcToAc) ||
        (tnl_type == bcmTunnelTypeWlanWtpToAc6) || 
        (tnl_type == bcmTunnelTypeWlanAcToAc6)) { 
        tunnel_term.flags |= BCM_TUNNEL_TERM_TUNNEL_WITH_ID;
        tunnel_term.tunnel_id = tunnel_id;
        if (remote_port != BCM_GPORT_INVALID) {
            tunnel_term.flags |= BCM_TUNNEL_TERM_WLAN_REMOTE_TERMINATE;
            tunnel_term.remote_port = remote_port;
        }
    }

    if (_BCM_TUNNEL_OUTER_HEADER_IPV6(tnl_type)) {
        sal_memcpy(tunnel_term.sip6, sip6, sizeof(bcm_ip6_t));
        sal_memcpy(tunnel_term.dip6, dip6, sizeof(bcm_ip6_t));
        bcm_ip6_mask_create(tunnel_term.sip6_mask, sip_mask_len);
        bcm_ip6_mask_create(tunnel_term.dip6_mask, dip_mask_len);
    } else {
        tunnel_term.sip = sip;
        tunnel_term.dip = dip;
        tunnel_term.sip_mask = bcm_ip_mask_create(sip_mask_len);
        tunnel_term.dip_mask = bcm_ip_mask_create(dip_mask_len);
    }

    tunnel_term.udp_dst_port = dport;
    tunnel_term.udp_src_port = sport;
    tunnel_term.type = tnl_type;
    tunnel_term.vrf = vrf;

    if (outer_dscp) {
        tunnel_term.flags |= BCM_TUNNEL_TERM_USE_OUTER_DSCP;
    }

    if (outer_ttl) {
        tunnel_term.flags |= BCM_TUNNEL_TERM_USE_OUTER_TTL;
    }
    if (inner_dscp) {
        tunnel_term.flags |= BCM_TUNNEL_TERM_KEEP_INNER_DSCP;
    }

    if (trust_dscp) {
        tunnel_term.flags |= BCM_TUNNEL_TERM_DSCP_TRUST;
    }

    if (replace) {
        tunnel_term.flags |= BCM_TUNNEL_REPLACE;
    }

    tunnel_term.vlan = (bcm_vlan_t)vlan;
    tunnel_term.pbmp = pbmap;

    if ((rv = bcm_tunnel_terminator_add(unit, &tunnel_term)) < 0) {
        cli_out("ERROR %s: adding tunnel term %s\n",
                ARG_CMD(a), bcm_errmsg(rv));
        return CMD_FAIL;
    }
    return CMD_OK;
}

/*
 * Function:
 *      _l3_cmd_tunnel_term_delete
 * Description:
 * 	    Service routine used to delete tunnel terminator entry.
 * Parameters:
 *      unit      - (IN) Device number.
 *      a         - (IN) Command arguments. 
 * Returns: 
 *      CMD_XXX
 */
STATIC cmd_result_t
_l3_cmd_tunnel_term_delete(int unit, args_t *a) 
{
    bcm_tunnel_terminator_t tunnel_term;
    cmd_result_t      retCode; 
    parse_table_t     pt;     
    int               rv;
    bcm_ip6_t         sip6;
    bcm_ip6_t         dip6;
    int               dip = 0;
    int               sip = 0;
    int               sip_mask_len = 0; 
    int               dip_mask_len = 0;
    int               tnl_type = 0;
    int               vrf = 0;
    int               dport = 0;
    int               sport = 0;

    parse_table_init(unit, &pt);
    sal_memset(sip6, 0, sizeof(bcm_ip6_t));
    sal_memset(dip6, 0, sizeof(bcm_ip6_t));

    parse_table_add(&pt, "VRF",  PQ_DFL | PQ_INT, 0, (void *)&vrf, 0);
    parse_table_add(&pt, "DIP",  PQ_DFL|PQ_IP, 0, (void*)&dip, 0);
    parse_table_add(&pt, "SIP",  PQ_DFL|PQ_IP, 0, (void*)&sip, 0);
    parse_table_add(&pt, "DIP6",  PQ_DFL|PQ_IP6, 0, (void*)&dip6, 0);
    parse_table_add(&pt, "SIP6",  PQ_DFL|PQ_IP6, 0, (void*)&sip6, 0);
    parse_table_add(&pt, "DipMaskLen", PQ_DFL|PQ_INT, 0, (void*)&dip_mask_len, 0);
    parse_table_add(&pt, "SipMaskLen", PQ_DFL|PQ_INT, 0, (void*)&sip_mask_len, 0);
    parse_table_add(&pt, "TYpe", PQ_DFL|PQ_INT, 0, (void*)&tnl_type, 0);
    parse_table_add(&pt, "L4DstPort", PQ_DFL|PQ_INT, 0, (void*)&dport, 0);
    parse_table_add(&pt, "L4SrcPort", PQ_DFL|PQ_INT, 0, (void*)&sport, 0);

    if (!parseEndOk(a, &pt, &retCode)) {
        return retCode;
    }

    sal_memset(&tunnel_term, 0, sizeof (tunnel_term));


    if (_BCM_TUNNEL_OUTER_HEADER_IPV6(tnl_type)) {
        sal_memcpy(tunnel_term.sip6, sip6, sizeof(bcm_ip6_t));
        sal_memcpy(tunnel_term.dip6, dip6, sizeof(bcm_ip6_t));
        bcm_ip6_mask_create(tunnel_term.sip6_mask, sip_mask_len);
        bcm_ip6_mask_create(tunnel_term.dip6_mask, dip_mask_len);
    } else {
        tunnel_term.sip = sip;
        tunnel_term.dip = dip;
        tunnel_term.sip_mask = bcm_ip_mask_create(sip_mask_len);
        tunnel_term.dip_mask = bcm_ip_mask_create(dip_mask_len);
    }

    tunnel_term.udp_dst_port = dport;
    tunnel_term.udp_src_port = sport;
    tunnel_term.type = tnl_type;
    tunnel_term.vrf = vrf;

    if ((rv = bcm_tunnel_terminator_delete(unit, &tunnel_term)) < 0) {
        cli_out("ERROR %s: adding tunnel term %s\n",
                ARG_CMD(a), bcm_errmsg(rv));
        return CMD_FAIL;
    }
    return CMD_OK;
}

/*
 * Function:
 *      _l3_cmd_tunnel_term_get
 * Description:
 * 	    Service routine used to read tunnel terminator entry.
 * Parameters:
 *      unit      - (IN) Device number.
 *      a         - (IN) Command arguments. 
 * Returns: 
 *      CMD_XXX
 */
STATIC cmd_result_t
_l3_cmd_tunnel_term_get(int unit, args_t *a) 
{
    bcm_tunnel_terminator_t tunnel_term;
    cmd_result_t      retCode; 
    parse_table_t     pt;     
    int               rv;
    bcm_ip6_t         sip6;
    bcm_ip6_t         dip6;
    int               vlan = 0;
    int               dip = 0;
    int               sip = 0;
    int               sip_mask_len = 0; 
    int               dip_mask_len = 0;
    int               tnl_type = 0;
    int               vrf = 0;
    int               dport = 0;
    int               sport = 0;

    parse_table_init(unit, &pt);
    sal_memset(sip6, 0, sizeof(bcm_ip6_t));
    sal_memset(dip6, 0, sizeof(bcm_ip6_t));
    sal_memset(&tunnel_term, 0, sizeof(bcm_tunnel_terminator_t));

    parse_table_add(&pt, "VRF",  PQ_DFL | PQ_INT, 0, (void *)&vrf, 0);
    parse_table_add(&pt, "DIP",  PQ_DFL|PQ_IP, 0, (void*)&dip, 0);
    parse_table_add(&pt, "SIP",  PQ_DFL|PQ_IP, 0, (void*)&sip, 0);
    parse_table_add(&pt, "DIP6",  PQ_DFL|PQ_IP6, 0, (void*)&dip6, 0);
    parse_table_add(&pt, "SIP6",  PQ_DFL|PQ_IP6, 0, (void*)&sip6, 0);
    parse_table_add(&pt, "DipMaskLen", PQ_DFL|PQ_INT, 0, (void*)&dip_mask_len, 0);
    parse_table_add(&pt, "SipMaskLen", PQ_DFL|PQ_INT, 0, (void*)&sip_mask_len, 0);
    parse_table_add(&pt, "TYpe", PQ_DFL|PQ_INT, 0, (void*)&tnl_type, 0);
    parse_table_add(&pt, "L4DstPort", PQ_DFL|PQ_INT, 0, (void*)&dport, 0);
    parse_table_add(&pt, "L4SrcPort", PQ_DFL|PQ_INT, 0, (void*)&sport, 0);
    parse_table_add(&pt, "VLAN", PQ_DFL|PQ_INT, 0, (void*)&vlan, 0);

    if (!parseEndOk(a, &pt, &retCode)) {
        return retCode;
    }

    sal_memset(&tunnel_term, 0, sizeof (tunnel_term));


    if (_BCM_TUNNEL_OUTER_HEADER_IPV6(tnl_type)) {
        sal_memcpy(tunnel_term.sip6, sip6, sizeof(bcm_ip6_t));
        sal_memcpy(tunnel_term.dip6, dip6, sizeof(bcm_ip6_t));
        bcm_ip6_mask_create(tunnel_term.sip6_mask, sip_mask_len);
        bcm_ip6_mask_create(tunnel_term.dip6_mask, dip_mask_len);
    } else {
        tunnel_term.sip = sip;
        tunnel_term.dip = dip;
        tunnel_term.sip_mask = bcm_ip_mask_create(sip_mask_len);
        tunnel_term.dip_mask = bcm_ip_mask_create(dip_mask_len);
    }

    tunnel_term.udp_dst_port = dport;
    tunnel_term.udp_src_port = sport;
    tunnel_term.type = tnl_type;
    tunnel_term.vrf = vrf;
    tunnel_term.vlan = (bcm_vlan_t)vlan;

    if ((rv = bcm_tunnel_terminator_get(unit, &tunnel_term)) < 0) {
        cli_out("ERROR %s: adding tunnel term %s\n",
                ARG_CMD(a), bcm_errmsg(rv));
        return CMD_FAIL;
    }

    _l3_cmd_tunnel_term_print(unit, &tunnel_term);
    return CMD_OK;
}

/*
 * Function:
 *      _l3_cmd_dscp_map_add
 * Description:
 * 	    Service routine used to add dscp map object.
 * Parameters:
 *      unit      - (IN) Device number.
 *      a         - (IN) Command arguments. 
 * Returns: 
 *      CMD_XXX
 */
STATIC cmd_result_t
_l3_cmd_dscp_map_add(int unit, args_t *a) 
{
    int dscp_map_id = -1;
    int               rv;

    rv = bcm_tunnel_dscp_map_create(unit, 0, &dscp_map_id);
    if (BCM_FAILURE(rv)) {
        cli_out("%s: Failed to create egress dscp map id: %s\n",
                ARG_CMD(a), bcm_errmsg(rv));
        return (CMD_FAIL);
    } else {
        cli_out("%s: Returned DSCP Map ID %d\n", ARG_CMD(a), dscp_map_id);
    }
    return (CMD_OK);
}

/*
 * Function:
 *      _l3_cmd_dscp_map_delete
 * Description:
 * 	    Service routine used to delete dscp map object.
 * Parameters:
 *      unit      - (IN) Device number.
 *      a         - (IN) Command arguments. 
 * Returns: 
 *      CMD_XXX
 */
STATIC cmd_result_t
_l3_cmd_dscp_map_delete(int unit, args_t *a) 
{
    cmd_result_t      retCode; 
    parse_table_t     pt;     
    int               dscp_map_id = -1;
    int               rv;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "DscpMapId", PQ_DFL|PQ_INT, 0, (void*)&dscp_map_id, 0);
    if (!parseEndOk(a, &pt, &retCode)) {
        return retCode;
    }

    if (-1 == dscp_map_id) {
        return(CMD_USAGE);
    }

    rv = bcm_tunnel_dscp_map_destroy(unit, dscp_map_id);
    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error deleting entry from egress : %s\n",
                ARG_CMD(a), bcm_errmsg(rv));
        return (CMD_FAIL);
    } 
    return (CMD_OK);
}
/*
 * Function:
 *      _l3_cmd_dscp_map_port_set
 * Description:
 * 	    Service routine used to set dscp map priority, color -> dscp mapping.
 * Parameters:
 *      unit      - (IN) Device number.
 *      a         - (IN) Command arguments. 
 * Returns: 
 *      CMD_XXX
 */
STATIC cmd_result_t
_l3_cmd_dscp_map_port_set(int unit, args_t *a) 
{
    parse_table_t     pt;     
    bcm_tunnel_dscp_map_t dscp_map;
    int prio, color, dscp_val;
    cmd_result_t      retCode; 
    bcm_port_t        port = -1;
    int rv;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Port", PQ_DFL|PQ_PORT, 0, (void*)&port, 0);
    parse_table_add(&pt, "Priority", PQ_DFL|PQ_INT, 0, (void*)&prio, 0);
    parse_table_add(&pt, "Color",  PQ_DFL|PQ_INT, 0, (void*)&color, 0);
    parse_table_add(&pt, "DSCPValue", PQ_DFL|PQ_INT, 0, (void *)&dscp_val, 0);
    if (!parseEndOk(a, &pt, &retCode)) {
        return retCode;
    }

    if (-1 == port) {
        return (CMD_USAGE);
    }

    dscp_map.priority = prio;
    dscp_map.color = color;
    dscp_map.dscp = dscp_val;

    rv = bcm_tunnel_dscp_map_port_set(unit, port, &dscp_map);

    if (BCM_FAILURE(rv)) {
        cli_out("ERROR %s: setting tunnel DSCP Map for port %d %s\n",
                ARG_CMD(a), port, bcm_errmsg(rv));
        return (CMD_FAIL);
    }
    return (CMD_OK);
}

/*
 * Function:
 *      _l3_cmd_dscp_map_port_show
 * Description:
 * 	    Service routine used to show dscp map object.
 * Parameters:
 *      unit      - (IN) Device number.
 *      a         - (IN) Command arguments. 
 * Returns: 
 *      CMD_XXX
 */
STATIC cmd_result_t
_l3_cmd_dscp_map_port_show(int unit, args_t *a) 
{
    parse_table_t     pt;     
    bcm_tunnel_dscp_map_t dscp_map;
    int prio;
    cmd_result_t      retCode; 
    bcm_port_t port = -1;
    int rv;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Port", PQ_DFL | PQ_PORT, 0,
                    (void *)&port, 0);
    if (!parseEndOk(a, &pt, &retCode))
        return retCode;

    if (-1 == port) {
        return(CMD_USAGE);
    }

    cli_out("\nDSCP Map for port %d:\n", port);
    cli_out("\n-----------\n");
    for (prio = 0; prio < 8; prio++) {
        dscp_map.priority = prio;
        dscp_map.color = bcmColorGreen;
        rv = bcm_tunnel_dscp_map_port_get(unit, port, &dscp_map);
        if (BCM_FAILURE(rv)) {
            cli_out("%s: Error L3 accessing: %s\n", ARG_CMD(a), bcm_errmsg(rv));
            return CMD_FAIL;
        }
        cli_out(" Priority = %d, Color = Green,  DSCP = %d\n",
                prio, dscp_map.dscp);

        dscp_map.priority = prio;
        dscp_map.color = bcmColorYellow;
        rv = bcm_tunnel_dscp_map_port_get(unit, port, &dscp_map);
        if (BCM_FAILURE(rv)) {
            cli_out("%s: Error L3 accessing: %s\n", ARG_CMD(a), bcm_errmsg(rv));
            return CMD_FAIL;
        }
        cli_out(" Priority = %d, Color = Yellow, DSCP = %d\n",
                prio, dscp_map.dscp);

        dscp_map.priority = prio;
        dscp_map.color = bcmColorRed;
        rv = bcm_tunnel_dscp_map_port_get(unit, port, &dscp_map);
        if (BCM_FAILURE(rv)) {
            cli_out("%s: Error L3 accessing: %s\n", ARG_CMD(a), bcm_errmsg(rv));
            return CMD_FAIL;
        }
        cli_out(" Priority = %d, Color = Red,    DSCP = %d\n",
                prio, dscp_map.dscp);
    }
    return CMD_OK;
}


/*
 * Function:
 *      _l3_cmd_dscp_map_show
 * Description:
 * 	    Service routine used to set dscp map priority, color -> dscp mapping.
 * Parameters:
 *      unit      - (IN) Device number.
 *      a         - (IN) Command arguments. 
 * Returns: 
 *      CMD_XXX
 */
STATIC cmd_result_t
_l3_cmd_dscp_map_set(int unit, args_t *a) 
{
    parse_table_t     pt;     
    bcm_tunnel_dscp_map_t dscp_map;
    int prio, color, dscp_val;
    cmd_result_t      retCode; 
    int dscp_map_id = -1;
    int rv;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "DscpMapId", PQ_DFL|PQ_INT, 0, 
                    (void*)&dscp_map_id, 0);
    parse_table_add(&pt, "Priority", PQ_DFL|PQ_INT, 0, (void*)&prio, 0);
    parse_table_add(&pt, "Color",  PQ_DFL|PQ_INT, 0, (void*)&color, 0);
    parse_table_add(&pt, "DSCPValue", PQ_DFL|PQ_INT, 0, (void *)&dscp_val, 0);
    if (!parseEndOk(a, &pt, &retCode)) {
        return retCode;
    }

    if (-1 == dscp_map_id) {
        return(CMD_USAGE);
    }

    dscp_map.priority = prio;
    dscp_map.color = color;
    dscp_map.dscp = dscp_val;

    rv = bcm_tunnel_dscp_map_set(unit, dscp_map_id, &dscp_map);

    if (BCM_FAILURE(rv)) {
        cli_out("ERROR %s: setting tunnel DSCP Map for %d %s\n",
                ARG_CMD(a), dscp_map_id, bcm_errmsg(rv));
        return (CMD_FAIL);
    }
    return (CMD_OK);
}

/*
 * Function:
 *      _l3_cmd_dscp_map_show
 * Description:
 * 	    Service routine used to show dscp map object.
 * Parameters:
 *      unit      - (IN) Device number.
 *      a         - (IN) Command arguments. 
 * Returns: 
 *      CMD_XXX
 */
STATIC cmd_result_t
_l3_cmd_dscp_map_show(int unit, args_t *a) 
{
    parse_table_t     pt;     
    bcm_tunnel_dscp_map_t dscp_map;
    int prio;
    cmd_result_t      retCode; 
    int dscp_map_id = -1;
    int rv;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "DscpMapId", PQ_DFL | PQ_INT, 0,
                    (void *)&dscp_map_id, 0);
    if (!parseEndOk(a, &pt, &retCode))
        return retCode;

    if (-1 == dscp_map_id) {
        return(CMD_USAGE);
    }

    cli_out("\nDSCP Map %d:\n", dscp_map_id);
    cli_out("\n-----------\n");
    for (prio = 0; prio < 8; prio++) {
        dscp_map.priority = prio;
        dscp_map.color = bcmColorGreen;
        rv = bcm_tunnel_dscp_map_get(unit, dscp_map_id, &dscp_map);
        if (BCM_FAILURE(rv)) {
            cli_out("%s: Error L3 accessing: %s\n", ARG_CMD(a), bcm_errmsg(rv));
            return CMD_FAIL;
        }
        cli_out(" Priority = %d, Color = Green,  DSCP = %d\n",
                prio, dscp_map.dscp);

        dscp_map.priority = prio;
        dscp_map.color = bcmColorYellow;
        rv = bcm_tunnel_dscp_map_get(unit, dscp_map_id, &dscp_map);
        if (BCM_FAILURE(rv)) {
            cli_out("%s: Error L3 accessing: %s\n", ARG_CMD(a), bcm_errmsg(rv));
            return CMD_FAIL;
        }
        cli_out(" Priority = %d, Color = Yellow, DSCP = %d\n",
                prio, dscp_map.dscp);

        dscp_map.priority = prio;
        dscp_map.color = bcmColorRed;
        rv = bcm_tunnel_dscp_map_get(unit, dscp_map_id, &dscp_map);
        if (BCM_FAILURE(rv)) {
            cli_out("%s: Error L3 accessing: %s\n", ARG_CMD(a), bcm_errmsg(rv));
            return CMD_FAIL;
        }
        cli_out(" Priority = %d, Color = Red,    DSCP = %d\n",
                prio, dscp_map.dscp);
    }
    return CMD_OK;
}

/*
 * Function:
 *      _l3_cmd_egress_intf_create
 * Description:
 * 	    Service routine used to create/add egress l3 interface.
 * Parameters:
 *      unit      - (IN) Device number.
 *      a         - (IN) Command arguments. 
 * Returns: 
 *      CMD_XXX
 */
STATIC cmd_result_t
_l3_cmd_egress_intf_create(int unit, args_t *a) 
{
    cmd_result_t      retCode; 
    parse_table_t     pt;     
    int               rv;
    bcm_mac_t         mac;
    bcm_l3_intf_t     intf;
    bcm_if_t          interface = -1;
    int               vid = 0;
#if defined(BCM_XGS3_SWITCH_SUPPORT)
    int               inner_vlan = 0;
    bcm_vrf_t         vrf = 0;
    bcm_if_group_t    group = 0;
    int               mtu = 0;    
    int               nat_realm = 0;
#endif /* BCM_XGS_SWITCH_SUPPORT */

    parse_table_init(unit, &pt);
    sal_memset(mac, 0, sizeof(bcm_mac_t));
    bcm_l3_intf_t_init(&intf);

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Vlan", PQ_DFL|PQ_INT, 0, &vid, NULL);
    parse_table_add(&pt, "Mac",  PQ_DFL|PQ_MAC, 0, mac, NULL);
    parse_table_add(&pt, "INtf", PQ_DFL|PQ_INT, 0, (void*)&interface, 0);
#if defined(BCM_XGS3_SWITCH_SUPPORT)
    parse_table_add(&pt, "InnerVlan", PQ_DFL|PQ_INT, 0, &inner_vlan, NULL);
    parse_table_add(&pt, "VRF", PQ_DFL | PQ_INT, 0, (void *)&vrf, 0);
    parse_table_add(&pt, "Group", PQ_DFL | PQ_INT, 0, (void *)&group, 0);
    parse_table_add(&pt, "MTU", PQ_DFL | PQ_INT, 0, (void *)&mtu, 0);
    parse_table_add(&pt, "NATrealm", PQ_DFL | PQ_INT, 0, (void *)&nat_realm, 0);
#endif
    if (!parseEndOk(a, &pt, &retCode)) {
        return retCode;
    }

    sal_memcpy(intf.l3a_mac_addr, mac, sizeof(bcm_mac_t));
    intf.l3a_vid = (bcm_vlan_t) vid;
    if (!soc_property_get(unit, spn_L3_DISABLE_ADD_TO_ARL, 0)) {
        intf.l3a_flags |= BCM_L3_ADD_TO_ARL;
    }
    if (interface != -1) {
        intf.l3a_intf_id = interface;
        intf.l3a_flags |= BCM_L3_WITH_ID;
    }
#if defined(BCM_XGS3_SWITCH_SUPPORT)
    intf.l3a_vrf = vrf;
    intf.l3a_group = group;
    intf.l3a_mtu = mtu;
    intf.l3a_inner_vlan = inner_vlan;
    intf.l3a_nat_realm_id = nat_realm;
#endif
    rv = bcm_l3_intf_create(unit, &intf);
    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error creating entry to L3 Intf table: %s\n",
                ARG_CMD(a), bcm_errmsg(rv));
        return (CMD_FAIL);
    }
    return (CMD_OK);
}
/*
 * Function:
 *      _l3_cmd_egress_intf_delete
 * Description:
 * 	    Service routine used to remove egress l3 interface.
 * Parameters:
 *      unit      - (IN) Device number.
 *      a         - (IN) Command arguments. 
 * Returns: 
 *      CMD_XXX
 */
STATIC cmd_result_t
_l3_cmd_egress_intf_delete(int unit, args_t *a) 
{
    bcm_l3_intf_t     intf;
    cmd_result_t      retCode; 
    parse_table_t     pt;     
    int               rv;
    bcm_if_t          interface = -1;

    if (!ARG_CNT(a)) {
        cli_out("Expected command parameters: Intf=id\n");
        return (CMD_FAIL);
    }

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "INtf", PQ_DFL|PQ_INT, 0, (void*)&interface, 0);
    if (!parseEndOk(a, &pt, &retCode)) {
        return retCode;
    }

    bcm_l3_intf_t_init(&intf);
    intf.l3a_intf_id = interface;
    if (-1 == interface) {
        cli_out("Expected command parameters: Intf=id\n");
        return (CMD_FAIL);
    }

    if ((rv = bcm_l3_intf_delete(unit, &intf)) < 0) {
        cli_out("%s: Error (%s) destroy interface (%d)\n",
                ARG_CMD(a), bcm_errmsg(rv), interface);
        return (CMD_FAIL);
    }

    return (CMD_OK);
}


/*
 * Function:
 *      _l3_cmd_egress_intf_delete_all
 * Description:
 * 	    Service routine used to flush all egress l3 interfaces.
 * Parameters:
 *      unit      - (IN) Device number.
 *      a         - (IN) Command arguments. 
 * Returns: 
 *      CMD_XXX
 */
STATIC cmd_result_t
_l3_cmd_egress_intf_delete_all(int unit, args_t *a) 
{
    int             rv;
    int             idx;
    bcm_l3_intf_t   intf;
    bcm_l3_info_t   l3_hw_status;

    if ((rv = bcm_l3_info(unit, &l3_hw_status)) < 0) {
        cli_out("%s: Error L3 accessing: %s\n", ARG_CMD(a), bcm_errmsg(rv));
        return CMD_FAIL;
    }

    bcm_l3_intf_t_init(&intf);
    for (idx = 0; idx < l3_hw_status.l3info_max_intf; idx++) {
        intf.l3a_intf_id = idx;
        rv = bcm_l3_intf_delete(unit, &intf);

        if (BCM_FAILURE(rv) && rv != BCM_E_NOT_FOUND) {
            cli_out("%s: Error destroy interface %d from L3 Intf"
                    "table: %s\n", ARG_CMD(a), idx, bcm_errmsg(rv));
            return (CMD_FAIL);
        }
    }
    return (CMD_OK);
}

/*
 * Function:
 *      _l3_cmd_egress_intf_show
 * Description:
 * 	    Service routine used to show l3 egress interfaces.
 * Parameters:
 *      unit      - (IN) Device number.
 *      a         - (IN) Command arguments. 
 * Returns: 
 *      CMD_XXX
 */
STATIC cmd_result_t
_l3_cmd_egress_intf_show(int unit, args_t *a) 
{
    int             rv;
    parse_table_t   pt;     
    bcm_l3_intf_t   intf;
    cmd_result_t    retCode;
    bcm_l3_info_t   l3_hw_status;
    int             free_l3intf;
    int             print_hdr = TRUE;
    bcm_if_t        interface = -1;
    int             min_idx, max_idx;

    if ((rv = bcm_l3_info(unit, &l3_hw_status)) < 0) {
        cli_out("Error in L3 info access: %s\n",  bcm_errmsg(rv));
        return CMD_FAIL;
    }

    free_l3intf  = l3_hw_status.l3info_max_intf -
        l3_hw_status.l3info_occupied_intf;


    if (ARG_CNT(a)) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "INtf", PQ_DFL|PQ_INT, 0, (void*)&interface, 0);
        if (!parseEndOk(a, &pt, &retCode)) {
            return retCode;
        }
    }

    /* Min index and max interfaces number depend on this soc feature. */
    if (soc_feature(unit, soc_feature_l3_egr_intf_zero_invalid)) {
        min_idx = 1;
        } else {
        min_idx = 0;
    }
    max_idx = min_idx + (l3_hw_status.l3info_max_intf - 1);

    if (interface != -1) {
        if ((interface < 0)  || (interface > max_idx)) {
            cli_out("Invalid interface index: %d\n", interface);
            return (CMD_FAIL);
        }  
    }

    cli_out("Free L3INTF entries: %d\n", free_l3intf);

    if (interface != -1) {
        sal_memset(&intf, 0, sizeof (bcm_l3_intf_t));
        intf.l3a_intf_id = interface;
        rv = bcm_l3_intf_get(unit, &intf);
        if (BCM_SUCCESS(rv)) {
            _l3_cmd_egress_intf_print(unit, TRUE, &intf);
            return (CMD_OK);
        } else {
            cli_out("Error L3 interface %d: %s\n", interface, bcm_errmsg(rv));
            return (CMD_FAIL);
        }
    }

    /* Note: last interface id is reserved for Copy To Cpu purposes. */
    for (interface = min_idx; interface < max_idx; interface++) {
        sal_memset(&intf, 0, sizeof (bcm_l3_intf_t));
        intf.l3a_intf_id = interface;
        rv = bcm_l3_intf_get(unit, &intf);
        if (BCM_SUCCESS(rv)) {
            _l3_cmd_egress_intf_print(unit, print_hdr, &intf);
            print_hdr = FALSE;
        } else if (rv == BCM_E_NOT_FOUND) {
            continue;
        } else if (BCM_FAILURE(rv)) {
            cli_out("Error traverse l3 interfaces: %s\n", bcm_errmsg(rv));
        }
    }
    return CMD_OK;
}

#if defined(BCM_XGS3_SWITCH_SUPPORT)
/*
 * Function:
 *      _l3_cmd_egress_object_create
 * Description:
 * 	    Service routine used to create/update l3 egress object.
 * Parameters:
 *      unit      - (IN) Device number.
 *      a         - (IN) Command arguments. 
 * Returns: 
 *      CMD_XXX
 */
STATIC cmd_result_t
_l3_cmd_egress_object_create(int unit, args_t *a) 
{
    cmd_result_t      retCode; 
    parse_table_t     pt;     
    int               rv;
    bcm_mac_t         mac;
    bcm_l3_egress_t   egress_object;
    bcm_if_t          interface = -1;
    bcm_port_t        port = 0;
    bcm_module_t      module = 0;
    bcm_trunk_t       trunk = -1;
    int               l2tocpu = 0;
    int               copytocpu = 0;
    int               drop = 0;
    int               l3mc = 0;
    int               mpls_label = BCM_MPLS_LABEL_INVALID;
    int               object_id = -1;
    uint32            flags = 0;

    bcm_l3_egress_t_init(&egress_object);
    sal_memset(mac, 0, sizeof(bcm_mac_t));

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Mac",  PQ_DFL | PQ_MAC, 0, (void *)mac, 0);
    parse_table_add(&pt, "Port", PQ_DFL | PQ_PORT, 0, (void *)&port, 0);
    parse_table_add(&pt, "INtf", PQ_DFL | PQ_INT, 0, (void *)&interface, 0);
    parse_table_add(&pt, "MOdule", PQ_DFL | PQ_INT, 0, (void *)&module, 0);
    parse_table_add(&pt, "Trunk", PQ_DFL | PQ_INT, 0, (void *)&trunk, 0);
    parse_table_add(&pt, "L2tocpu", PQ_DFL | PQ_BOOL, 0, (void *)&l2tocpu, 0);
    parse_table_add(&pt, "CopyToCpu", PQ_DFL | PQ_BOOL, 0, (void *)&copytocpu, 0);
    parse_table_add(&pt, "Drop", PQ_DFL | PQ_BOOL, 0, (void *)&drop, 0);
    parse_table_add(&pt, "L3MC", PQ_DFL | PQ_BOOL, 0, (void *)&l3mc, 0);
    parse_table_add(&pt, "MplsLabel", PQ_DFL | PQ_INT, 0,
                    (void *)&mpls_label, 0);
    parse_table_add(&pt, "EgrId", PQ_DFL | PQ_INT, 0, (void *)&object_id, 0);
    if (!parseEndOk(a, &pt, &retCode))
        return retCode;

    egress_object.intf   = interface;
    egress_object.mpls_label = mpls_label;
    sal_memcpy(egress_object.mac_addr, mac, sizeof(mac));

    if (BCM_GPORT_IS_SET(port)) {
        egress_object.port = port;
    } else {
        egress_object.module = module;
    if (trunk >= 0) {
        egress_object.flags |= BCM_L3_TGID;
        egress_object.trunk = trunk;
    } else {
        egress_object.port = port;
    }
    }

    if (l2tocpu) {
        egress_object.flags |= BCM_L3_L2TOCPU;
    }

    if (copytocpu) {
        egress_object.flags |= (BCM_L3_COPY_TO_CPU);
    }

    if (drop) {
        egress_object.flags |= (BCM_L3_DST_DISCARD);
    }

    if (l3mc) {
        egress_object.flags |= (BCM_L3_IPMC);
        if (MAC_ADDR_EQUAL(mac, _soc_mac_all_zeroes)){
            /* Assert BCM_L3_KEEP_DSTMAC if no valid mac-da is assigned. */
            egress_object.flags |= (BCM_L3_KEEP_DSTMAC);
        }
    }

    if (object_id >= 0) {
        flags = (BCM_L3_WITH_ID | BCM_L3_REPLACE);
    }


    rv = bcm_l3_egress_create(unit, flags, &egress_object, &object_id);
    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error creating egress object entry: %s\n",
                ARG_CMD(a), bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    var_set_integer(ENV_EGRESS_OBJECT_ID, object_id, TRUE, FALSE);
    cli_out("New egress object index: %d\n", object_id);
    cli_out("Environment variable (%s) was set\n", ENV_EGRESS_OBJECT_ID);
    return (CMD_OK);
}

/*
 * Function:
 *      _l3_cmd_egress_object_find
 * Description:
 * 	    Service routine used to locate id of l3 egress object.
 * Parameters:
 *      unit      - (IN) Device number.
 *      a         - (IN) Command arguments. 
 * Returns: 
 *      CMD_XXX
 */
STATIC cmd_result_t
_l3_cmd_egress_object_find(int unit, args_t *a) 
{
    cmd_result_t      retCode; 
    parse_table_t     pt;     
    int               rv;
    bcm_mac_t         mac;
    bcm_l3_egress_t   egress_object;
    bcm_if_t          interface = -1;
    bcm_port_t        port = 0;
    bcm_module_t      module = 0;
    bcm_trunk_t       trunk = -1;
    int               l2tocpu = 0;
    int               l3mc = 0;
    int               mpls_label = BCM_MPLS_LABEL_INVALID;
    int               object_id = -1;

    bcm_l3_egress_t_init(&egress_object);
    sal_memset(mac, 0, sizeof(bcm_mac_t));

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Mac",  PQ_DFL | PQ_MAC, 0, (void *)mac, 0);
    parse_table_add(&pt, "Port", PQ_DFL | PQ_PORT, 0, (void *)&port, 0);
    parse_table_add(&pt, "INtf", PQ_DFL | PQ_INT, 0, (void *)&interface, 0);
    parse_table_add(&pt, "MOdule", PQ_DFL | PQ_INT, 0, (void *)&module, 0);
    parse_table_add(&pt, "Trunk", PQ_DFL | PQ_INT, 0, (void *)&trunk, 0);
    parse_table_add(&pt, "L2tocpu", PQ_DFL | PQ_BOOL, 0, (void *)&l2tocpu, 0);
    parse_table_add(&pt, "L3MC", PQ_DFL | PQ_BOOL, 0, (void *)&l3mc, 0);
    parse_table_add(&pt, "MplsLabel", PQ_DFL | PQ_INT, 0,
                    (void *)&mpls_label, 0);
    if (!parseEndOk(a, &pt, &retCode))
        return retCode;

    egress_object.intf   = interface;
    egress_object.mpls_label = mpls_label;
    sal_memcpy(egress_object.mac_addr, mac, sizeof(mac));

    if (BCM_GPORT_IS_SET(port)) {
        egress_object.port = port;
    } else {
        egress_object.module = module;
    if (trunk >= 0) {
        egress_object.flags |= BCM_L3_TGID;
        egress_object.trunk = trunk;
    } else {
        egress_object.port = port;
    }
    }

    if (l2tocpu) {
        egress_object.flags |= BCM_L3_L2TOCPU;
    }

    if (l3mc) {
        egress_object.flags |= BCM_L3_IPMC;
        if (MAC_ADDR_EQUAL(mac, _soc_mac_all_zeroes)){
            /* Assert BCM_L3_KEEP_DSTMAC if no valid mac-da is assigned. */
            egress_object.flags |= (BCM_L3_KEEP_DSTMAC);
        }
    }

    rv = bcm_l3_egress_find(unit, &egress_object, &object_id);
    if (BCM_FAILURE(rv)) {
        if (rv != BCM_E_NOT_FOUND) {
            cli_out("%s: Error finding egress object entry: %s\n",
                    ARG_CMD(a), bcm_errmsg(rv));
            return (CMD_FAIL);
        }
        cli_out("%s: Couldn't find entry\n",ARG_CMD(a));
        return (CMD_OK);
    }
    var_set_integer(ENV_EGRESS_OBJECT_ID, object_id, TRUE, FALSE);
    cli_out("Egress object interface id: %d\n", object_id);
    cli_out("Environment variable (%s) was set\n", ENV_EGRESS_OBJECT_ID);
    return (CMD_OK);
}


/*
 * Function:
 *      _l3_cmd_egress_object_destroy
 * Description:
 * 	    Service routine used to destroy l3 egress object by id.
 * Parameters:
 *      unit      - (IN) Device number.
 *      a         - (IN) Command arguments. 
 * Returns: 
 *      CMD_XXX
 */
STATIC cmd_result_t
_l3_cmd_egress_object_destroy(int unit, args_t *a) 
{
    int               rv;
    int               object_id = -1;

    if (ARG_CNT(a)) {
        if (!isint(ARG_CUR(a))) {
            cli_out("%s: Invalid number: %s\n", ARG_CMD(a), ARG_CUR(a));
            return (CMD_FAIL);
        }
        object_id = parse_integer(ARG_GET(a));
    } else {
        cli_out("%s: Invalid arguments: %s\n", ARG_CMD(a), ARG_CUR(a));
        return (CMD_FAIL);
    }

    rv = bcm_l3_egress_destroy(unit, object_id);
    if (BCM_FAILURE(rv))
    { 
        cli_out("%s: Error deleting egress object entry: %s\n", ARG_CMD(a),
                bcm_errmsg(rv));
        return (CMD_FAIL);
    }
    return (CMD_OK);
}

/*
 * Function:
 *      _l3_cmd_egress_object_show
 * Description:
 * 	    Service routine used to display l3 egress object/s.
 *      specific one or all of them. 
 * Parameters:
 *      unit      - (IN) Device number.
 *      a         - (IN) Command arguments. 
 * Returns: 
 *      CMD_XXX
 */
STATIC cmd_result_t
_l3_cmd_egress_object_show(int unit, args_t *a) 
{
    int               rv;
    int               object_id = -1;
    int               one_entry_only = 0;
    bcm_l3_egress_t   egress_object;

    bcm_l3_egress_t_init(&egress_object);

    if (ARG_CNT(a)) {
        if (!isint(ARG_CUR(a))) {
            cli_out("%s: Invalid number: %s\n", ARG_CMD(a), ARG_CUR(a));
            return (CMD_FAIL);
        }
        object_id = parse_integer(ARG_GET(a));
        one_entry_only = 1;
    }

    cli_out("Entry  Mac                 Vlan INTF PORT MOD MPLS_LABEL ToCpu Drop"
            " RefCount L3MC\n");

    if (one_entry_only) {
        rv = bcm_l3_egress_get(unit, object_id, &egress_object);
        if (BCM_FAILURE(rv)) {
            cli_out("%s: Error reading egress object entry: %s\n",
                    ARG_CMD(a), bcm_errmsg(rv));
            return (CMD_FAIL);
        }
        _l3_cmd_egress_obj_print(unit, object_id, &egress_object, NULL);
    } else {
        rv = bcm_l3_egress_traverse(unit, _l3_cmd_egress_obj_print, NULL);
        if (BCM_FAILURE(rv)) {
            cli_out("%s: Error reading egress object entry: %s\n",
                    ARG_CMD(a), bcm_errmsg(rv));
            return (CMD_FAIL);
        }
    }
    return (CMD_OK);
}


/*
 * Function:
 *      _l3_cmd_egress_mpath_object_create
 * Description:
 * 	    Service routine used to create/update l3 multipath egress object.
 * Parameters:
 *      unit      - (IN) Device number.
 *      a         - (IN) Command arguments. 
 * Returns: 
 *      CMD_XXX
 */
STATIC cmd_result_t
_l3_cmd_egress_mpath_object_create(int unit, args_t *a) 
{
    int               rv;
    parse_table_t     pt;     
    cmd_result_t      retCode; 
    bcm_if_t          mpath_egr[32];
    int               intf_count = 0;
    int               object_id = -1;
    int               flags = 0;


    parse_table_init(unit, &pt);
    parse_table_add(&pt, "EgrId", PQ_DFL | PQ_INT, 0, (void *)&object_id, 0);
    parse_table_add(&pt, "Size",  PQ_DFL | PQ_INT, 0, (void *)&intf_count, 0);
    parse_table_add(&pt, "Intf0", PQ_DFL | PQ_INT, 0, (void *)&mpath_egr[0], 0);
    parse_table_add(&pt, "Intf1", PQ_DFL | PQ_INT, 0, (void *)&mpath_egr[1], 0);
    parse_table_add(&pt, "Intf2", PQ_DFL | PQ_INT, 0, (void *)&mpath_egr[2], 0);
    parse_table_add(&pt, "Intf3", PQ_DFL | PQ_INT, 0, (void *)&mpath_egr[3], 0);

    if (!parseEndOk(a, &pt, &retCode))  {
        return retCode;
    }

    if (object_id >= 0) {
        flags = (BCM_L3_WITH_ID | BCM_L3_REPLACE);
    }

    rv = bcm_l3_egress_multipath_create(unit, flags, intf_count,
                                        mpath_egr, &object_id);
    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error creating mpath egress object entry: %s\n",
                ARG_CMD(a), bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    var_set_integer(ENV_EGRESS_OBJECT_ID, object_id, TRUE, FALSE);
    cli_out("New multipath egress object index: %d\n", object_id);
    cli_out("Environment variable (%s) was set\n", ENV_EGRESS_OBJECT_ID);
    return (CMD_OK);
}

/*
 * Function:
 *      _l3_cmd_egress_mpath_object_max_create
 * Description:
 * 	    Service routine used to create l3 multipath egress object
 * 	    with dynamic ecmp group sizing.
 * Parameters:
 *      unit      - (IN) Device number.
 *      a         - (IN) Command arguments.
 * Returns:
 *      CMD_XXX
 */
STATIC cmd_result_t
_l3_cmd_egress_mpath_object_max_create(int unit, args_t *a)
{
    int               rv;
    parse_table_t     pt;
    cmd_result_t      retCode;
    bcm_if_t          mpath_egr[32];
    int               max_paths = 0;
    int               intf_count = 0;
    int               object_id = -1;
    int               flags = 0;


    parse_table_init(unit, &pt);
    parse_table_add(&pt, "EgrId", PQ_DFL | PQ_INT, 0, (void *)&object_id, 0);
    parse_table_add(&pt, "Size",  PQ_DFL | PQ_INT, 0, (void *)&intf_count, 0);
    parse_table_add(&pt, "MaxGrpSize",  PQ_DFL | PQ_INT, 0, (void *)&max_paths, 0);
    parse_table_add(&pt, "Intf0", PQ_DFL | PQ_INT, 0, (void *)&mpath_egr[0], 0);
    parse_table_add(&pt, "Intf1", PQ_DFL | PQ_INT, 0, (void *)&mpath_egr[1], 0);
    parse_table_add(&pt, "Intf2", PQ_DFL | PQ_INT, 0, (void *)&mpath_egr[2], 0);
    parse_table_add(&pt, "Intf3", PQ_DFL | PQ_INT, 0, (void *)&mpath_egr[3], 0);

    if (!parseEndOk(a, &pt, &retCode))  {
        return retCode;
    }

    if (object_id >= 0) {
        flags = (BCM_L3_WITH_ID | BCM_L3_REPLACE);
    }

    rv = bcm_l3_egress_multipath_max_create(unit, flags, max_paths, intf_count,
                                        mpath_egr, &object_id);
    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error creating mpath egress object entry: %s\n",
                ARG_CMD(a), bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    var_set_integer(ENV_EGRESS_OBJECT_ID, object_id, TRUE, FALSE);
    cli_out("New multipath egress object index: %d\n", object_id);
    cli_out("Environment variable (%s) was set\n", ENV_EGRESS_OBJECT_ID);
    return (CMD_OK);
}

/*
 * Function:
 *      _l3_cmd_egress_mpath_object_lookup
 * Description:
 * 	    Service routine used to search for matching l3
 *      multipath egress object.
 * Parameters:
 *      unit      - (IN) Device number.
 *      a         - (IN) Command arguments.
 * Returns:
 *      CMD_XXX
 */
STATIC cmd_result_t
_l3_cmd_egress_mpath_object_lookup(int unit, args_t *a)
{
    int               rv;
    parse_table_t     pt;
    cmd_result_t      retCode;
    bcm_if_t          mpath_egr[32];
    int               intf_count = 0;
    int               object_id = -1;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Size",  PQ_DFL | PQ_INT, 0, (void *)&intf_count, 0);
    parse_table_add(&pt, "Intf0", PQ_DFL | PQ_INT, 0, (void *)&mpath_egr[0], 0);
    parse_table_add(&pt, "Intf1", PQ_DFL | PQ_INT, 0, (void *)&mpath_egr[1], 0);
    parse_table_add(&pt, "Intf2", PQ_DFL | PQ_INT, 0, (void *)&mpath_egr[2], 0);
    parse_table_add(&pt, "Intf3", PQ_DFL | PQ_INT, 0, (void *)&mpath_egr[3], 0);
    if (!parseEndOk(a, &pt, &retCode)) {
        return retCode;
    }
    rv = bcm_l3_egress_multipath_find(unit, intf_count, mpath_egr, &object_id);

    if (BCM_FAILURE(rv)) {
        if (rv != BCM_E_NOT_FOUND) {
            cli_out("%s: Error finding mpath egress object entry: %s\n",
                    ARG_CMD(a), bcm_errmsg(rv));
            return CMD_FAIL;
        }
        cli_out("%s: Couldn't find the entry\n",ARG_CMD(a));
        return CMD_OK;
    }

    var_set_integer(ENV_EGRESS_OBJECT_ID, object_id, TRUE, FALSE);
    cli_out("Egress mpath object interface id: %d\n", object_id);
    cli_out("Environment variable (%s) was set\n", ENV_EGRESS_OBJECT_ID);
    return CMD_OK;
}

/*
 * Function:
 *      _l3_cmd_egress_mpath_object_destroy
 * Description:
 * 	    Service routine used to destroy l3 multipath egress object.
 * Parameters:
 *      unit      - (IN) Device number.
 *      a         - (IN) Command arguments.
 * Returns:
 *      CMD_XXX
 */
STATIC cmd_result_t
_l3_cmd_egress_mpath_object_destroy(int unit, args_t *a)
{
    int               rv;
    int               object_id = -1;

    if (ARG_CNT(a)) {
        if (!isint(ARG_CUR(a))) {
            cli_out("%s: Invalid object id: %s\n", ARG_CMD(a), ARG_CUR(a));
            return (CMD_FAIL);
        }
        object_id = parse_integer(ARG_GET(a));
    } else {
        cli_out("%s: Invalid arguments: %s\n", ARG_CMD(a), ARG_CUR(a));
        return (CMD_FAIL);
    }

    rv = bcm_l3_egress_multipath_destroy(unit, object_id);
    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error deleting mpath egress object entry: %s\n",
                ARG_CMD(a), bcm_errmsg(rv));
        return (CMD_FAIL);
    }
    return (CMD_OK);
}

/*
 * Function:
 *      _l3_cmd_egress_mpath_object_show
 * Description:
 * 	    Service routine used to display l3 multipath egress object.
 * Parameters:
 *      unit      - (IN) Device number.
 *      a         - (IN) Command arguments.
 * Returns:
 *      CMD_XXX
 */
STATIC cmd_result_t
_l3_cmd_egress_mpath_object_show(int unit, args_t *a)
{
    int               rv;
    bcm_if_t          *mpath_egr = NULL;
    int               one_entry_only = 0;
    int               intf_count = 0;
    int               object_id = -1;

    if (ARG_CNT(a)) {
        if (!isint(ARG_CUR(a))) {
            cli_out("%s: Invalid number: %s\n", ARG_CMD(a), ARG_CUR(a));
            return (CMD_FAIL);
        }
        object_id = parse_integer(ARG_GET(a));
        one_entry_only = 1;
    }

    if (one_entry_only) {
        rv = bcm_l3_egress_multipath_get(unit, object_id, 0, NULL,
                                         &intf_count);
        mpath_egr = sal_alloc((intf_count * sizeof(int)), "ecmp group");
        if (mpath_egr == NULL) {
            cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(BCM_E_MEMORY));
            return (CMD_FAIL);
        }
        rv = bcm_l3_egress_multipath_get(unit, object_id, intf_count,
                                         mpath_egr, &intf_count);
        if (BCM_FAILURE(rv)) {
            sal_free(mpath_egr);
            cli_out("%s: Error reading egress object entry: %s\n",
                    ARG_CMD(a), bcm_errmsg(rv));
            return (CMD_FAIL);
        }
        _l3_cmd_egress_mpath_obj_print(unit, object_id, intf_count,
                                       mpath_egr, NULL);
        sal_free(mpath_egr);
    } else {
        rv = bcm_l3_egress_multipath_traverse(unit,
                                              _l3_cmd_egress_mpath_obj_print,
                                              NULL);
    }
    if (BCM_FAILURE(rv)) {
        return (CMD_FAIL);
    }
    return (CMD_OK);
}

/*
 * Function:
 *      _l3_cmd_egress_mpath_object_intf_insert
 * Description:
 * 	    Service routine used to add an egress interface to multipath
 *      egress object.
 * Parameters:
 *      unit      - (IN) Device number.
 *      a         - (IN) Command arguments.
 * Returns:
 *      CMD_XXX
 */
STATIC cmd_result_t
_l3_cmd_egress_mpath_object_intf_insert(int unit, args_t *a)
{
    int               rv;
    parse_table_t     pt;
    cmd_result_t      retCode;
    int               object_id = -1;
    int               interface = 0;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "EgrId", PQ_DFL | PQ_INT, 0, (void *)&object_id, 0);
    parse_table_add(&pt, "INtf", PQ_DFL | PQ_INT, 0, (void *)&interface, 0);

    if (!parseEndOk(a, &pt, &retCode))
        return retCode;

    rv = bcm_l3_egress_multipath_add(unit, object_id, interface);
    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error adding interface to egress obj: %s\n",
                ARG_CMD(a), bcm_errmsg(rv));
        return (CMD_FAIL);
    }
    return (CMD_OK);
}


/*
 * Function:
 *      _l3_cmd_egress_mpath_object_intf_remove
 * Description:
 * 	    Service routine used to remove an egress interface
 *      from multipath  egress object.
 * Parameters:
 *      unit      - (IN) Device number.
 *      a         - (IN) Command arguments.
 * Returns:
 *      CMD_XXX
 */
STATIC cmd_result_t
_l3_cmd_egress_mpath_object_intf_remove(int unit, args_t *a)
{
    int               rv;
    parse_table_t     pt;
    cmd_result_t      retCode;
    int               object_id = -1;
    int               interface = 0;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "EgrId", PQ_DFL | PQ_INT, 0, (void *)&object_id, 0);
    parse_table_add(&pt, "INtf", PQ_DFL | PQ_INT, 0, (void *)&interface, 0);

    if (!parseEndOk(a, &pt, &retCode))
        return retCode;

    rv = bcm_l3_egress_multipath_delete(unit, object_id, interface);
    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error deleting interface from egress obj: %s\n",
                ARG_CMD(a), bcm_errmsg(rv));
        return (CMD_FAIL);
    }
    return (CMD_OK);
}

/*
 * Function:
 *      _l3_cmd_egress_ecmp_object_create
 * Description:
 * 	    Service routine used to create/update l3 ecmp egress object.
 * Parameters:
 *      unit      - (IN) Device number.
 *      a         - (IN) Command arguments.
 * Returns:
 *      CMD_XXX
 */
STATIC cmd_result_t
_l3_cmd_egress_ecmp_object_create(int unit, args_t *a)
{
    int               rv;
    parse_table_t     pt;
    cmd_result_t      retCode;
    bcm_if_t          ecmp_egr[32];
    int               intf_count = 0;
    int               object_id = -1;
    bcm_l3_egress_ecmp_t ecmp;


    parse_table_init(unit, &pt);
    parse_table_add(&pt, "EgrId", PQ_DFL | PQ_INT, 0, (void *)&object_id, 0);
    parse_table_add(&pt, "Size",  PQ_DFL | PQ_INT, 0, (void *)&intf_count, 0);
    parse_table_add(&pt, "Intf0", PQ_DFL | PQ_INT, 0, (void *)&ecmp_egr[0], 0);
    parse_table_add(&pt, "Intf1", PQ_DFL | PQ_INT, 0, (void *)&ecmp_egr[1], 0);
    parse_table_add(&pt, "Intf2", PQ_DFL | PQ_INT, 0, (void *)&ecmp_egr[2], 0);
    parse_table_add(&pt, "Intf3", PQ_DFL | PQ_INT, 0, (void *)&ecmp_egr[3], 0);

    if (!parseEndOk(a, &pt, &retCode))  { 
        return retCode;
    }

    bcm_l3_egress_ecmp_t_init(&ecmp);
    if (object_id >= 0) {
        ecmp.flags = (BCM_L3_WITH_ID | BCM_L3_REPLACE);
        ecmp.ecmp_intf = object_id;
    }

    rv = bcm_l3_egress_ecmp_create(unit, &ecmp, intf_count, ecmp_egr);
    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error creating ecmp egress object entry: %s\n",
                ARG_CMD(a), bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    var_set_integer(ENV_EGRESS_OBJECT_ID, ecmp.ecmp_intf, TRUE, FALSE);
    cli_out("New ecmp egress object index: %d\n", ecmp.ecmp_intf);
    cli_out("Environment variable (%s) was set\n", ENV_EGRESS_OBJECT_ID);
    return (CMD_OK);
}

/*
 * Function:
 *      _l3_cmd_egress_ecmp_object_max_create
 * Description:
 * 	    Service routine used to create l3 ecmp egress object
 * 	    with dynamic ecmp group sizing.
 * Parameters:
 *      unit      - (IN) Device number.
 *      a         - (IN) Command arguments. 
 * Returns: 
 *      CMD_XXX
 */
STATIC cmd_result_t
_l3_cmd_egress_ecmp_object_max_create(int unit, args_t *a)
{
    int               rv;
    parse_table_t     pt;     
    cmd_result_t      retCode; 
    bcm_if_t          ecmp_egr[32];
    int               max_paths = 0;
    int               intf_count = 0;
    int               object_id = -1;
    bcm_l3_egress_ecmp_t ecmp;


    parse_table_init(unit, &pt);
    parse_table_add(&pt, "EgrId", PQ_DFL | PQ_INT, 0, (void *)&object_id, 0);
    parse_table_add(&pt, "Size",  PQ_DFL | PQ_INT, 0, (void *)&intf_count, 0);
    parse_table_add(&pt, "MaxGrpSize",  PQ_DFL | PQ_INT, 0, (void *)&max_paths, 0);
    parse_table_add(&pt, "Intf0", PQ_DFL | PQ_INT, 0, (void *)&ecmp_egr[0], 0);
    parse_table_add(&pt, "Intf1", PQ_DFL | PQ_INT, 0, (void *)&ecmp_egr[1], 0);
    parse_table_add(&pt, "Intf2", PQ_DFL | PQ_INT, 0, (void *)&ecmp_egr[2], 0);
    parse_table_add(&pt, "Intf3", PQ_DFL | PQ_INT, 0, (void *)&ecmp_egr[3], 0);

    if (!parseEndOk(a, &pt, &retCode))  { 
        return retCode;
    }

    bcm_l3_egress_ecmp_t_init(&ecmp);
    if (object_id >= 0) {
        ecmp.flags = (BCM_L3_WITH_ID | BCM_L3_REPLACE);
        ecmp.ecmp_intf = object_id;
    }

    ecmp.max_paths = max_paths;
    rv = bcm_l3_egress_ecmp_create(unit, &ecmp, intf_count, ecmp_egr);
    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error creating ecmp egress object entry: %s\n",
                ARG_CMD(a), bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    var_set_integer(ENV_EGRESS_OBJECT_ID, ecmp.ecmp_intf, TRUE, FALSE);
    cli_out("New ecmp egress object index: %d\n", ecmp.ecmp_intf);
    cli_out("Environment variable (%s) was set\n", ENV_EGRESS_OBJECT_ID);
    return (CMD_OK);
}

/*
 * Function:
 *      _l3_cmd_egress_ecmp_object_lookup
 * Description:
 * 	    Service routine used to search for matching l3 
 *      ecmp egress object.
 * Parameters:
 *      unit      - (IN) Device number.
 *      a         - (IN) Command arguments. 
 * Returns: 
 *      CMD_XXX
 */
STATIC cmd_result_t
_l3_cmd_egress_ecmp_object_lookup(int unit, args_t *a)
{
    int               rv;
    parse_table_t     pt;     
    cmd_result_t      retCode; 
    bcm_if_t          ecmp_egr[32];
    int               intf_count = 0;
    bcm_l3_egress_ecmp_t ecmp;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Size",  PQ_DFL | PQ_INT, 0, (void *)&intf_count, 0);
    parse_table_add(&pt, "Intf0", PQ_DFL | PQ_INT, 0, (void *)&ecmp_egr[0], 0);
    parse_table_add(&pt, "Intf1", PQ_DFL | PQ_INT, 0, (void *)&ecmp_egr[1], 0);
    parse_table_add(&pt, "Intf2", PQ_DFL | PQ_INT, 0, (void *)&ecmp_egr[2], 0);
    parse_table_add(&pt, "Intf3", PQ_DFL | PQ_INT, 0, (void *)&ecmp_egr[3], 0);
    if (!parseEndOk(a, &pt, &retCode)) {
        return retCode;
    }

    bcm_l3_egress_ecmp_t_init(&ecmp);
    rv = bcm_l3_egress_ecmp_find(unit, intf_count, ecmp_egr, &ecmp);

    if (BCM_FAILURE(rv)) {
        if (rv != BCM_E_NOT_FOUND) {
            cli_out("%s: Error finding ecmp egress object entry: %s\n",
                    ARG_CMD(a), bcm_errmsg(rv));
            return CMD_FAIL;
        } 
        cli_out("%s: Couldn't find the entry\n",ARG_CMD(a));
        return CMD_OK;
    }

    var_set_integer(ENV_EGRESS_OBJECT_ID, ecmp.ecmp_intf, TRUE, FALSE);
    cli_out("Egress ecmp object interface id: %d\n", ecmp.ecmp_intf);
    cli_out("Environment variable (%s) was set\n", ENV_EGRESS_OBJECT_ID);
    return CMD_OK;
} 

/*
 * Function:
 *      _l3_cmd_egress_ecmp_object_destroy
 * Description:
 * 	    Service routine used to destroy l3 ecmp egress object.
 * Parameters:
 *      unit      - (IN) Device number.
 *      a         - (IN) Command arguments. 
 * Returns: 
 *      CMD_XXX
 */
STATIC cmd_result_t
_l3_cmd_egress_ecmp_object_destroy(int unit, args_t *a)
{
    int               rv;
    int               object_id = -1;
    bcm_l3_egress_ecmp_t ecmp;

    if (ARG_CNT(a)) {
        if (!isint(ARG_CUR(a))) {
            cli_out("%s: Invalid object id: %s\n", ARG_CMD(a), ARG_CUR(a));
            return (CMD_FAIL);
        }
        object_id = parse_integer(ARG_GET(a));
    } else {
        cli_out("%s: Invalid arguments: %s\n", ARG_CMD(a), ARG_CUR(a));
        return (CMD_FAIL);
    }

    bcm_l3_egress_ecmp_t_init(&ecmp);
    ecmp.ecmp_intf = object_id;
    rv = bcm_l3_egress_ecmp_destroy(unit, &ecmp);
    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error deleting ecmp egress object entry: %s\n",
                ARG_CMD(a), bcm_errmsg(rv));
        return (CMD_FAIL);
    }
    return (CMD_OK);
}

/*
 * Function:
 *      _l3_cmd_egress_ecmp_object_show
 * Description:
 * 	    Service routine used to display l3 ecmp egress object.
 * Parameters:
 *      unit      - (IN) Device number.
 *      a         - (IN) Command arguments. 
 * Returns: 
 *      CMD_XXX
 */
STATIC cmd_result_t
_l3_cmd_egress_ecmp_object_show(int unit, args_t *a)
{
    int               rv;
    bcm_if_t          *ecmp_egr = NULL;
    int               one_entry_only = 0;
    int               intf_count = 0;
    int               object_id = -1;
    bcm_l3_egress_ecmp_t ecmp;

    if (ARG_CNT(a)) {
        if (!isint(ARG_CUR(a))) {
            cli_out("%s: Invalid number: %s\n", ARG_CMD(a), ARG_CUR(a));
            return (CMD_FAIL);
        }
        object_id = parse_integer(ARG_GET(a));
        one_entry_only = 1;
    }

    bcm_l3_egress_ecmp_t_init(&ecmp);
    if (one_entry_only) {
        ecmp.ecmp_intf = object_id;
        rv = bcm_l3_egress_ecmp_get(unit, &ecmp, 0, NULL, &intf_count);
        ecmp_egr = sal_alloc((intf_count * sizeof(int)), "ecmp group");
        if (ecmp_egr == NULL) {
            cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(BCM_E_MEMORY));
            return (CMD_FAIL);
        }
        rv = bcm_l3_egress_ecmp_get(unit, &ecmp, intf_count, ecmp_egr,
                                    &intf_count);
        if (BCM_FAILURE(rv)) {
            sal_free(ecmp_egr);
            cli_out("%s: Error reading egress object entry: %s\n",
                    ARG_CMD(a), bcm_errmsg(rv));
            return (CMD_FAIL);
        } 
        _l3_cmd_egress_ecmp_obj_print(unit, &ecmp, intf_count, ecmp_egr,
                                       NULL);
        sal_free(ecmp_egr);
    } else {
        rv = bcm_l3_egress_ecmp_traverse(unit, _l3_cmd_egress_ecmp_obj_print,
                                         NULL);
    }
    if (BCM_FAILURE(rv)) {
        return (CMD_FAIL);
    }
    return (CMD_OK);
}

/*
 * Function:
 *      _l3_cmd_egress_ecmp_object_intf_insert
 * Description:
 * 	    Service routine used to add an egress interface to ecmp
 *      egress object. 
 * Parameters:
 *      unit      - (IN) Device number.
 *      a         - (IN) Command arguments. 
 * Returns: 
 *      CMD_XXX
 */
STATIC cmd_result_t
_l3_cmd_egress_ecmp_object_intf_insert(int unit, args_t *a)
{
    int               rv;
    parse_table_t     pt;     
    cmd_result_t      retCode; 
    int               object_id = -1;
    int               interface = 0;
    bcm_l3_egress_ecmp_t ecmp;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "EgrId", PQ_DFL | PQ_INT, 0, (void *)&object_id, 0);
    parse_table_add(&pt, "INtf", PQ_DFL | PQ_INT, 0, (void *)&interface, 0);

    if (!parseEndOk(a, &pt, &retCode))
        return retCode;

    bcm_l3_egress_ecmp_t_init(&ecmp);
    ecmp.ecmp_intf = object_id;
    rv = bcm_l3_egress_ecmp_add(unit, &ecmp, interface);
    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error adding interface to egress obj: %s\n", 
                ARG_CMD(a), bcm_errmsg(rv));
        return (CMD_FAIL);
    }
    return (CMD_OK);
}


/*
 * Function:
 *      _l3_cmd_egress_ecmp_object_intf_remove
 * Description:
 * 	    Service routine used to remove an egress interface 
 *      from ecmp  egress object.
 * Parameters:
 *      unit      - (IN) Device number.
 *      a         - (IN) Command arguments. 
 * Returns: 
 *      CMD_XXX
 */
STATIC cmd_result_t
_l3_cmd_egress_ecmp_object_intf_remove(int unit, args_t *a)
{
    int               rv;
    parse_table_t     pt;     
    cmd_result_t      retCode; 
    int               object_id = -1;
    int               interface = 0;
    bcm_l3_egress_ecmp_t ecmp;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "EgrId", PQ_DFL | PQ_INT, 0, (void *)&object_id, 0);
    parse_table_add(&pt, "INtf", PQ_DFL | PQ_INT, 0, (void *)&interface, 0);

    if (!parseEndOk(a, &pt, &retCode))
        return retCode;

    bcm_l3_egress_ecmp_t_init(&ecmp);
    ecmp.ecmp_intf = object_id;
    rv = bcm_l3_egress_ecmp_delete(unit, &ecmp, interface);
    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error deleting interface from egress obj: %s\n",
                ARG_CMD(a), bcm_errmsg(rv));
        return (CMD_FAIL);
    }
    return (CMD_OK);
}

/*
 * Function:
 *      _l3_cmd_prefix_map_add
 * Description:
 * 	    Service routine used to add prefix map.
 * Parameters:
 *      unit      - (IN) Device number.
 *      a         - (IN) Command arguments. 
 * Returns: 
 *      CMD_XXX
 */
STATIC cmd_result_t
_l3_cmd_prefix_map_add(int unit, args_t *a) 
{
    bcm_ip6_t         ip6_addr;
    cmd_result_t      retCode;
    parse_table_t     pt;
    int               rv;

    sal_memset(&ip6_addr, 0, sizeof(bcm_ip6_t));
    parse_table_init(unit, &pt);

    parse_table_add(&pt, "IP", PQ_DFL | PQ_IP6, 0, (void *)&ip6_addr, 0);
    if (!parseEndOk(a, &pt, &retCode))
        return retCode;

    rv = bcm_l3_ip6_prefix_map_add(unit, ip6_addr);
    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error adding ip6 prefix map: %s\n", ARG_CMD(a),
                bcm_errmsg(rv));
        return (CMD_FAIL);
    }
    return (CMD_OK);
}

/*
 * Function:
 *      _l3_cmd_prefix_map_delete
 * Description:
 * 	    Service routine used to delete prefix map.
 * Parameters:
 *      unit      - (IN) Device number.
 *      a         - (IN) Command arguments. 
 * Returns: 
 *      CMD_XXX
 */
STATIC cmd_result_t
_l3_cmd_prefix_map_delete(int unit, args_t *a) 
{
    bcm_ip6_t         ip6_addr;
    cmd_result_t      retCode;
    parse_table_t     pt;
    int               rv;

    sal_memset(&ip6_addr, 0, sizeof(bcm_ip6_t));
    parse_table_init(unit, &pt);

    parse_table_add(&pt, "IP", PQ_DFL | PQ_IP6, 0, (void *)&ip6_addr, 0);
    if (!parseEndOk(a, &pt, &retCode))
        return retCode;

    rv = bcm_l3_ip6_prefix_map_delete(unit, ip6_addr);
    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error adding ip6 prefix map: %s\n",
                ARG_CMD(a), bcm_errmsg(rv));
        return (CMD_FAIL);
    }
    return (CMD_OK);
}

/*
 * Function:
 *      _l3_cmd_prefix_map_delete_all
 * Description:
 * 	    Service routine used to flush all prefix maps.
 * Parameters:
 *      unit      - (IN) Device number.
 *      a         - (IN) Command arguments. 
 * Returns: 
 *      CMD_XXX
 */
STATIC cmd_result_t
_l3_cmd_prefix_map_delete_all(int unit, args_t *a) 
{
    int               rv;

    if ((rv = bcm_l3_ip6_prefix_map_delete_all(unit)) < 0) {
        cli_out("%s: Error flushing prefix map : %s\n",
                ARG_CMD(a), bcm_errmsg(rv));
        return (CMD_FAIL);
    } 
    return (CMD_OK);
}

#define BCM_DIAG_CMD_PREFIX_MAP_SIZE  (10)
/*
 * Function:
 *      _l3_cmd_prefix_map_show
 * Description:
 * 	    Service routine used display prefix maps.
 * Parameters:
 *      unit      - (IN) Device number.
 *      a         - (IN) Command arguments. 
 * Returns: 
 *      CMD_XXX
 */
STATIC cmd_result_t
_l3_cmd_prefix_map_show(int unit, args_t *a) 
{
    int        rv;
    int        idx;
    bcm_ip6_t  prefix_map[BCM_DIAG_CMD_PREFIX_MAP_SIZE];
    char       ip_str[IP6ADDR_STR_LEN + 3];
    int        intf_count = 0;     

    rv = bcm_l3_ip6_prefix_map_get(unit,  BCM_DIAG_CMD_PREFIX_MAP_SIZE, 
                                   prefix_map, &intf_count);
    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error reading prefix map entry: %s\n",
                ARG_CMD(a), bcm_errmsg(rv));
        return (CMD_FAIL);
    } 

    if (intf_count > 0) {
        cli_out("Entry  IP6_ADDR\n");
    }

    for (idx = 0; idx < intf_count; idx++) {
        format_ip6addr(ip_str, prefix_map[idx]);
         cli_out("%-5d %s\n", idx, ip_str);
    }

    return (CMD_OK);
}
#undef BCM_DIAG_CMD_PREFIX_MAP_SIZE 
#endif /* BCM_XGS3_SWITCH_SUPPORT */

/*
 * Function:
 *      _l3_cmd_route_add
 * Description:
 * 	    Service routine used to add route.
 * Parameters:
 *      unit      - (IN) Device number.
 *      a         - (IN) Command arguments. 
 *      v6        - (IN) IPv6 route indication.
 * Returns:
 *      CMD_XXX
 */
STATIC cmd_result_t
_l3_cmd_route_add(int unit, args_t *a, int v6) 
{
    bcm_l3_route_t  route_info;
    cmd_result_t    retCode;
    bcm_mac_t       mac;
    parse_table_t   pt;
    int             rv;
    int             interface = 0;
    bcm_ip_t        ip_addr = 0;
    bcm_ip_t        ip_mask = 0;
    int		        module = 0;
    int             trunk = -1;
    int             untag = 0;
    int             port = 0;
    int             hits = 0;
    int             vlan = 0;
    int             ipmc = 0;
#if defined(BCM_XGS_SWITCH_SUPPORT)
    int             ecmp = 0;
    int             local_route = 0;
#endif /* BCM_XGS_SWITCH_SUPPORT */
#if defined(BCM_XGS3_SWITCH_SUPPORT)
    int             lookup_class = 0;
    int             tunnel_opt = 0;
    int             mpls_label = 0;
    int             replace = 0;
    int             rpe = 0;
    int             pri = 0;
    int             dst_discard = 0;
    int             vrf = 0;
    int             plen = 0;
    bcm_ip6_t       ip6_addr;
#endif

    bcm_l3_route_t_init(&route_info);
    sal_memset(mac, 0, sizeof(bcm_mac_t));
    parse_table_init(unit, &pt);

    parse_table_add(&pt, "Mac",     PQ_DFL | PQ_MAC,  0, (void *)mac, 0);
    parse_table_add(&pt, "Port",    PQ_DFL | PQ_PORT, 0, (void *)&port, 0);
    parse_table_add(&pt, "INtf",    PQ_DFL | PQ_INT,  0, (void *)&interface, 0);
    parse_table_add(&pt, "MOdule",  PQ_DFL | PQ_INT,  0, (void *)&module, 0);
    parse_table_add(&pt, "Trunk",   PQ_DFL | PQ_INT,  0, (void *)&trunk, 0);
    parse_table_add(&pt, "Untag",   PQ_DFL | PQ_BOOL, 0, (void *)&untag, 0);
    parse_table_add(&pt, "Hit",     PQ_DFL | PQ_BOOL, 0, (void *)&hits, 0);
#if defined(BCM_XGS3_SWITCH_SUPPORT)
    parse_table_add(&pt, "VRF",     PQ_DFL | PQ_INT,  0, (void *)&vrf, 0);
    parse_table_add(&pt, "DstDiscard", PQ_DFL | PQ_INT, 0, 
                    (void *)&dst_discard, 0);
    parse_table_add(&pt, "PRI",     PQ_DFL|PQ_INT,    0, (void *)&pri, 0);
    parse_table_add(&pt, "RPE",     PQ_DFL|PQ_BOOL,   0, (void *)&rpe, 0);
    parse_table_add(&pt, "Replace", PQ_DFL | PQ_BOOL, 0, (void *)&replace, 0);
    parse_table_add(&pt, "MplsLabel", PQ_DFL | PQ_INT, 0,
                    (void *)&mpls_label, 0);
    parse_table_add(&pt, "TunnelOpt", PQ_DFL | PQ_INT, 0,
                    (void *)&tunnel_opt, 0);
    parse_table_add(&pt, "LookupClass", PQ_DFL | PQ_INT, 0, (void *)&lookup_class, 0);
    parse_table_add(&pt, "VLAN",    PQ_DFL | PQ_INT,  0, (void *)&vlan, 0);
    parse_table_add(&pt, "IPMC",    PQ_DFL | PQ_INT,  0, (void *)&ipmc, 0);
    if(v6) {
        sal_memset(ip6_addr, 0, sizeof(bcm_ip6_t));
        parse_table_add(&pt, "IP", PQ_DFL | PQ_IP6, 0, (void *)&ip6_addr, 0);
        parse_table_add(&pt, "MaskLen", PQ_DFL | PQ_INT, 0, (void *)&plen, 0);
    } else  
#endif /* BCM_XGS3_SWITCH_SUPPORT */
    {
        parse_table_add(&pt, "IP",   PQ_DFL | PQ_IP,   0, (void *)&ip_addr, 0);
        parse_table_add(&pt, "MaSk", PQ_DFL | PQ_IP,   0, (void *)&ip_mask, 0);
    }

#if defined(BCM_XGS_SWITCH_SUPPORT)
    parse_table_add(&pt, "ECMP",    PQ_DFL | PQ_BOOL, 0, (void *)&ecmp, 0);
    parse_table_add(&pt, "Local",   PQ_DFL | PQ_BOOL, 0, (void *)&local_route, 0);
#endif /* BCM_XGS_SWITCH_SUPPORT */

    if (!parseEndOk(a, &pt, &retCode)) {
        return retCode;
    }

#if defined(BCM_XGS3_SWITCH_SUPPORT)
    route_info.l3a_vrf     = vrf;
    if (dst_discard) {
        route_info.l3a_flags |= BCM_L3_DST_DISCARD;
    }
    if (rpe) {
        route_info.l3a_flags |= BCM_L3_RPE;
    }
    if (replace) {
        route_info.l3a_flags |= BCM_L3_REPLACE;
    }
    if (ipmc) {
        route_info.l3a_flags |= BCM_L3_IPMC;
        BCM_MULTICAST_L3_SET(route_info.l3a_mc_group, interface);
    }
    route_info.l3a_pri = pri;

    if (lookup_class) {
        route_info.l3a_lookup_class= lookup_class;
    }

    if (mpls_label) { 
        route_info.l3a_mpls_label = mpls_label;
        route_info.l3a_flags |= BCM_L3_ROUTE_LABEL;
    } else if (tunnel_opt) {
        route_info.l3a_tunnel_option = tunnel_opt;
    }
    if (v6) {
        sal_memcpy(route_info.l3a_ip6_net, ip6_addr, BCM_IP6_ADDRLEN);
        bcm_ip6_mask_create(route_info.l3a_ip6_mask, plen);
        route_info.l3a_flags |= BCM_L3_IP6;
    } else 
#endif /* BCM_XGS3_SWITCH_SUPPORT */
    { 
        route_info.l3a_subnet  = ip_addr;
        route_info.l3a_ip_mask = ip_mask;
    }

    route_info.l3a_intf = interface;
    route_info.l3a_vid = vlan;
    sal_memcpy(route_info.l3a_nexthop_mac, mac, sizeof(bcm_mac_t));

    if (BCM_GPORT_IS_SET(port)) {
        route_info.l3a_port_tgid = port;
    } else {
    if (trunk >= 0) {
        route_info.l3a_flags |= BCM_L3_TGID;
        route_info.l3a_port_tgid = trunk;
    } else {
        route_info.l3a_port_tgid = port;
    }
        route_info.l3a_modid = module;
    }

    if (untag) {
        route_info.l3a_flags |= BCM_L3_UNTAG;
    }

    if (hits) {
        route_info.l3a_flags |= BCM_L3_HIT;
    }

#ifdef BCM_XGS_SWITCH_SUPPORT
    if (ecmp) {
        route_info.l3a_flags |= BCM_L3_MULTIPATH;
    }

    if (local_route) {
        route_info.l3a_flags |= BCM_L3_DEFIP_LOCAL;
    }
#endif
    rv = bcm_l3_route_add(unit, &route_info);
    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error adding route table: %s\n", 
                ARG_CMD(a), bcm_errmsg(rv));
        return (CMD_FAIL);
    }
    return (CMD_OK);
}

/*
 * Function:
 *      _l3_cmd_host_add
 * Description:
 * 	    Service routine used to add host entry.
 * Parameters:
 *      unit      - (IN) Device number.
 *      a         - (IN) Command arguments. 
 *      v6        - (IN) IPv6 route indication.
 * Returns:
 *      CMD_XXX
 */
STATIC cmd_result_t
_l3_cmd_host_add(int unit, args_t *a, int v6) 
{
    bcm_l3_host_t   host_info;
    cmd_result_t    retCode;
    bcm_mac_t       mac;
    parse_table_t   pt;
    int             rv;
    int             host_as_route = 0;
    int             interface = 0;
    bcm_ip_t        ip_addr = 0;
    int		        module = 0;
    int             trunk = -1;
    int             untag = 0;
    int             port = 0;
    int             hits = 0;
    int             replace = 0;
#ifdef BCM_XGS_SWITCH_SUPPORT
    int             l2tocpu = 0;
#endif /* BCM_XGS_SWITCH_SUPPORT */ 
#if defined(BCM_XGS3_SWITCH_SUPPORT)
    int             rpe = 0;
    int             pri = 0;
    int             vrf = 0;
    int             lookup_class = 0;
    bcm_ip6_t       ip6_addr;
#endif

    bcm_l3_host_t_init(&host_info);
    sal_memset(mac, 0, sizeof(bcm_mac_t));
    parse_table_init(unit, &pt);

    parse_table_add(&pt, "Mac", PQ_DFL | PQ_MAC, 0, (void *)mac, 0);
#if defined(BCM_XGS3_SWITCH_SUPPORT)
    parse_table_add(&pt, "VRF", PQ_DFL | PQ_INT, 0, (void *)&vrf, 0);
    parse_table_add(&pt, "PRI", PQ_DFL|PQ_INT, 0, (void *)&pri, 0);
    parse_table_add(&pt, "RPE", PQ_DFL|PQ_BOOL, 0, (void *)&rpe, 0);
    parse_table_add(&pt, "Group", PQ_DFL|PQ_INT, 0, (void *)&lookup_class, 0);
    if (v6) {
	    parse_table_add(&pt, "IP", PQ_DFL | PQ_IP6, 0, (void *)&ip6_addr, 0);
    } else 
#endif /* BCM_XGS3_SWITCH_SUPPORT */
    {
        parse_table_add(&pt, "IP", PQ_DFL | PQ_IP, 0, (void *)&ip_addr, 0);
    }
    parse_table_add(&pt, "Replace", PQ_DFL | PQ_BOOL, 0, (void *)&replace, 0);
    parse_table_add(&pt, "Port", PQ_DFL | PQ_PORT, 0, (void *)&port, 0);
    parse_table_add(&pt, "INtf", PQ_DFL | PQ_INT, 0, (void *)&interface, 0);
    parse_table_add(&pt, "Hit", PQ_DFL | PQ_BOOL, 0, (void *)&hits, 0);
    parse_table_add(&pt, "MOdule", PQ_DFL | PQ_INT, 0, (void *)&module, 0);
    parse_table_add(&pt, "Trunk", PQ_DFL | PQ_INT, 0, (void *)&trunk, 0);
    parse_table_add(&pt, "Untag", PQ_DFL | PQ_BOOL, 0, (void *)&untag, 0);
    parse_table_add(&pt, "HOST_AS_ROUTE", PQ_DFL | PQ_BOOL, 0, 
                    (void *)&host_as_route, 0);
#ifdef BCM_XGS_SWITCH_SUPPORT
    parse_table_add(&pt, "L2tocpu", PQ_DFL | PQ_BOOL, 0, (void *)&l2tocpu, 0);
#endif
    if (!parseEndOk(a, &pt, &retCode))
        return retCode;

#if defined(BCM_XGS3_SWITCH_SUPPORT)
    host_info.l3a_vrf     = vrf;
    host_info.l3a_pri     = pri;
    host_info.l3a_lookup_class = lookup_class;

    if (rpe) {
        host_info.l3a_flags |= BCM_L3_RPE;
    }

    if (v6) {
        sal_memcpy(host_info.l3a_ip6_addr, ip6_addr, BCM_IP6_ADDRLEN);
        host_info.l3a_flags |= BCM_L3_IP6;
    } else 
#endif
    {
        host_info.l3a_ip_addr = ip_addr;
    } 

    if (host_as_route) {
        host_info.l3a_flags |= BCM_L3_HOST_AS_ROUTE;
    } 
    host_info.l3a_intf = interface;
    sal_memcpy(host_info.l3a_nexthop_mac, mac, sizeof(mac));

    if (BCM_GPORT_IS_SET(port)) {
        host_info.l3a_port_tgid = port;
    } else {
    host_info.l3a_modid = module;
    if (trunk >= 0) {
        host_info.l3a_flags |= BCM_L3_TGID;
        host_info.l3a_port_tgid = trunk;
    } else {
        host_info.l3a_port_tgid = port;
    }
    }

    if (hits) {
        host_info.l3a_flags |= BCM_L3_HIT;
    }
    if (untag) {
        host_info.l3a_flags |= BCM_L3_UNTAG;
    }

    if (replace) {
        host_info.l3a_flags |= BCM_L3_REPLACE;
    }

#ifdef BCM_XGS_SWITCH_SUPPORT
    if (l2tocpu) {
        host_info.l3a_flags |= BCM_L3_L2TOCPU;
    }
#endif
    rv = bcm_l3_host_add(unit, &host_info);
    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error creating entry in L3 host table: %s\n",
                ARG_CMD(a), bcm_errmsg(rv));
        return (CMD_FAIL);
    }
    return (CMD_OK);
} 

/* Struct to track performance data of a collection of API
   calls (or table operations).  Recommended units: micro seconds(us) */
typedef struct _l3_route_perf_data_s {
    int     count;  /* Number of times the functions is called. */
    int     max;    /* Maximum call time. */
    int     min;    /* Minimum call time. */
    int     total;  /* Total duration time. */
} _l3_route_perf_data_t;

/* Initalizes a _l3_route_perf_data_t structure */
void
_l3_route_perf_data_init(_l3_route_perf_data_t *perf)
{
    perf->count = 0;
    perf->min = 0;
    perf->max = 0;
    perf->total = 0;
}

/* Adds a time measurement to a perf_data_t instance.   */
void
_l3_route_perf_data_add(_l3_route_perf_data_t *perf, int duration)
{
    if (perf->count == 0) {
        perf->count = 1;
        perf->min = duration;
        perf->max = duration;
        perf->total = duration;
    } else {
        perf->count++;
        if (duration > perf->max) {
            perf->max = duration;
        }
        if (duration < perf->min) {
            perf->min = duration;
        }
        perf->total += duration;
    }
}

/* Dumps perf stats */
void
_l3_route_perf_data_print(_l3_route_perf_data_t *perf, int route_cnt, const char *label)
{
    if( perf->count > 0) {
        /*int u_avg = perf->total / perf->count;*/
        uint32 temp;

        /*cli_out("Performance Dump: %s :\n", label);
        cli_out("  Min: %d uSec\n", perf->min);
        cli_out("  Max: %d uSec\n", perf->max);
        cli_out("  Avg: %d uSec\n", u_avg);
        cli_out("  Total: %d.%3d Sec\n", (perf->total / 1000000), (perf->total%1000000/1000));
        cli_out("  Count: %d\n", perf->count);*/

        temp = route_cnt * perf->count;
        if (temp > 4294) {
            uint32 temp2 = 0;
            uint32 rem = 0, divn;
            while ((int)temp > 0) {
                divn = (temp > 2147) ? 2147 : temp;
                divn = divn * 1000000;
                temp2 += (divn + rem) / perf->total;
                rem = (divn + rem) % perf->total;
                temp -= 2147;
            }
            temp = temp2;
        } else {
            temp = temp * 1000000 / perf->total;
        }
        cli_out(" %7d ", temp);
    } else {
        cli_out("    -    ");
    }
}

/*
 * Performance test to add IPv4 routes
 * Same as arch_ipv4_route_add_001 but also measure route update times
 * and runs the while test 10 times to get 10 different data points.
 */
int _l3_route_perf_egr_mode[SOC_MAX_NUM_DEVICES];
int _l3_route_perf_obj_id[SOC_MAX_NUM_DEVICES];
int _l3_route_perf_obj2_id[SOC_MAX_NUM_DEVICES];
int _l3_route_perf_vrf = 100;


STATIC void
_l3_route_perf_test_setup(int unit)
{
    /* Setup */
    int rv;

    bcm_l3_intf_t l3_intf;

    bcm_l3_egress_t   egress_object;
    bcm_l3_egress_t   egress_object2;
    bcm_mac_t         mac;

    /* Turn off back ground threads */
    sh_process_command(unit, "l2mode");
    sh_process_command(unit, "l2mode off");
    sh_process_command(unit, "linkscan");
    sh_process_command(unit, "linkscan off");
    sh_process_command(unit, "counter");
    sh_process_command(unit, "counter off");
    sh_process_command(unit, "memscan");
    sh_process_command(unit, "memscan off");
    sh_process_command(unit, "sramscan");
    sh_process_command(unit, "sramscan off");

    /* L3 Egress Mode */
    rv = bcm_switch_control_get(unit, bcmSwitchL3EgressMode,
                                &_l3_route_perf_egr_mode[unit]);
    if (BCM_FAILURE(rv)) {
        cli_out("Perf: Get L3 Egress Mode failed: %s\n", bcm_errmsg(rv));
        return;
    }
    rv = bcm_switch_control_set(unit, bcmSwitchL3EgressMode, 1);
    if (BCM_FAILURE(rv)) {
        cli_out("Perf: Set L3 Egress Mode failed: %s\n", bcm_errmsg(rv));
        return;
    }

    /* L3 Interface */
    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_flags = BCM_L3_WITH_ID;
    l3_intf.l3a_intf_id = 1;
    l3_intf.l3a_mac_addr[5] = 0x1;
    l3_intf.l3a_vid = 1;
    l3_intf.l3a_vrf = _l3_route_perf_vrf;
    rv = bcm_l3_intf_create(unit, &l3_intf);
    if (BCM_FAILURE(rv)) {
        cli_out("Perf: Create L3 intf failed: %s\n", bcm_errmsg(rv));
        return;
    }

    /* L3 Egress */
    bcm_l3_egress_t_init(&egress_object);
    sal_memset(mac, 0, sizeof(bcm_mac_t));
    mac[5] = 0x1;
    egress_object.intf = 1;
    egress_object.module = 0;
    egress_object.port = 1;
    sal_memcpy(egress_object.mac_addr, mac, sizeof(mac));

    rv = bcm_l3_egress_create(unit, 0, &egress_object,
                              &_l3_route_perf_obj_id[unit]);
    if (BCM_FAILURE(rv)) {
        cli_out("Perf: Error creating egress object entry: %s\n",
                bcm_errmsg(rv));
        return ;
    }

    bcm_l3_egress_t_init(&egress_object2);
    sal_memset(mac, 0, sizeof(bcm_mac_t));
    mac[5] = 0x2;
    egress_object2.intf = 1;
    egress_object2.module = 0;
    egress_object2.port = 1;
    sal_memcpy(egress_object2.mac_addr, mac, sizeof(mac));

    rv = bcm_l3_egress_create(unit, 0, &egress_object2,
                              &_l3_route_perf_obj2_id[unit]);
    if (BCM_FAILURE(rv)) {
        cli_out("Perf: Error creating egress object2 entry: %s\n",
                bcm_errmsg(rv));
        return ;
    }

    return ;
}

STATIC void
_l3_route_perf_test_clean(int unit)
{
    int rv;
    bcm_l3_intf_t intf;

    rv = bcm_l3_egress_destroy(unit, _l3_route_perf_obj2_id[unit]);
    if (BCM_FAILURE(rv) && rv != BCM_E_BUSY) {
        cli_out("Perf: Error deleting egress object2 entry: %s\n",
                bcm_errmsg(rv));
        return ;
    }

    rv = bcm_l3_egress_destroy(unit, _l3_route_perf_obj_id[unit]);
    if (BCM_FAILURE(rv) && rv != BCM_E_BUSY) {
        cli_out("Perf: Error deleting egress object entry: %s\n",
                bcm_errmsg(rv));
        return ;
    }

    bcm_l3_intf_t_init(&intf);
    intf.l3a_intf_id = 1;

    if ((rv = bcm_l3_intf_delete(unit, &intf)) < 0) {
        cli_out("Perf: Error (%s) destroy interface (%d)\n",
                bcm_errmsg(rv), 1);
        return ;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchL3EgressMode,
                                _l3_route_perf_egr_mode[unit]);
    if (BCM_FAILURE(rv)) {
        cli_out("Perf: Set L3 Egress Mode failed: %s\n", bcm_errmsg(rv));
        return;
    }

    /* Turn on back ground threads */
    sh_process_command(unit, "l2mode on");
    sh_process_command(unit, "linkscan on");
    sh_process_command(unit, "counter on");
    sh_process_command(unit, "memscan on");
    sh_process_command(unit, "sramscan on");

    return ;

}

#define _L3_ROUTE_PERF_OP_ADD   1
#define _L3_ROUTE_PERF_OP_UPD   2
#define _L3_ROUTE_PERF_OP_GET   3
#define _L3_ROUTE_PERF_OP_DEL   4

char *_l3_route_perf_op_str[] = {
    "Invalid",
    "Insert",
    "Update",
    "Lookup",
    "Delete"
};

STATIC int
_l3_route_perf_v4_op_test(int unit, int op, int use_api, int set_idx,
                          _l3_route_perf_data_t *route_perf, int *route_cnt, int vrf)
{
    int         rv = 0;
    int         inx, idx;
    int         start, end, duration = 0;
    bcm_l3_route_t base_route;
    _bcm_defip_cfg_t base_defip;

    int (*op_func[5])(int, bcm_l3_route_t *) = {
        NULL,
        bcm_l3_route_add,
        bcm_l3_route_add,
        bcm_l3_route_get,
        bcm_l3_route_delete
    };

#if defined(BCM_TOMAHAWK_SUPPORT) && defined(ALPM_ENABLE)
    int (*op_func_defip[5])(int, _bcm_defip_cfg_t *) = {
        NULL,
        bcm_esw_alpm_insert,
        bcm_esw_alpm_update,
        bcm_esw_alpm_lookup,
        bcm_esw_alpm_delete
    };
#endif
    /* 4K Entries */
    uint32 ip, start_ip = 0x01010101;
    uint32 ip_mask = 0xffffffff;
    int loop = 4 * 1024;

    /* 1K Entries */
    int loop_ip = 1 * 1024;
    int j;
    uint32 prfx[4] = {22, 23, 24, 20};
    uint32 mask[4] = {0xfffffc00, 0xfffffe00, 0xffffff00, 0xfffff000};
    uint32 ipv4[4] = {0x02020202, 0x29020202, 0x61020202, 0x8C010101};
    uint32 ipAddr;
    uint32 ipMask;
    uint32 base_ip;

    *route_cnt = 0;

    bcm_l3_route_t_init(&base_route);
    sal_memset(&base_defip, 0, sizeof(base_defip));

    /* Insert entry */
    /*cli_out("%s 4K IPv4 routes with mask - 0x%08x\n",
            _l3_route_perf_op_str[op], ip_mask);*/
    start = sal_time_usecs();
    ip = start_ip + set_idx * loop;

    for (inx = 0; inx < loop; inx++) {
        if (use_api) {
            base_route.l3a_vrf = vrf;
            base_route.l3a_subnet = ip;
            base_route.l3a_ip_mask = ip_mask;
            base_route.l3a_intf = _l3_route_perf_obj_id[unit];
            if (op == _L3_ROUTE_PERF_OP_UPD) {
                base_route.l3a_flags = BCM_L3_REPLACE;
                base_route.l3a_intf = _l3_route_perf_obj2_id[unit];
            }

            rv = op_func[op](unit, &base_route);
        } else {
#if defined(BCM_TOMAHAWK_SUPPORT) && defined(ALPM_ENABLE)
            sal_memset(&base_defip, 0, sizeof(base_defip));
            base_defip.defip_vrf = vrf;
            base_defip.defip_ip_addr = ip;
            base_defip.defip_sub_len = bcm_ip_mask_length(ip_mask);
            base_defip.defip_intf = _l3_route_perf_obj_id[unit];
            if (op == _L3_ROUTE_PERF_OP_UPD) {
                base_defip.defip_flags = BCM_L3_REPLACE;
                base_defip.defip_intf = _l3_route_perf_obj2_id[unit];
            }

            rv = op_func_defip[op](unit, &base_defip);
#endif
        }
        if (rv < 0) {
            end = sal_time_usecs();
            duration = end - start;
            /*cli_out("%s %dth route returned msg: %s\n",
                    _l3_route_perf_op_str[op], inx,
                    bcm_errmsg(rv));*/
            goto End4;
        } else {
            *route_cnt += 1;
        }

        ip++;
    }
    end = sal_time_usecs();
    duration = end - start;

    bcm_l3_route_t_init(&base_route);
    sal_memset(&base_defip, 0, sizeof(base_defip));

    for (j = 0; j < 4; j++) {
        /*cli_out("%s 1K IPv4 routes with mask - 0x%08x\n",
                _l3_route_perf_op_str[op], mask[j]);*/
        start = sal_time_usecs();
        for (idx = 0; idx < loop_ip; idx++) {
            base_ip = ((loop_ip + idx + loop_ip * set_idx) << (32 - prfx[j]));
            ipAddr = base_ip + ipv4[j];
            ipMask = mask[j];

            if (use_api) {
                base_route.l3a_vrf = vrf;
                base_route.l3a_subnet = ipAddr;
                base_route.l3a_ip_mask = ipMask;
                base_route.l3a_intf = _l3_route_perf_obj_id[unit];
                if (op == _L3_ROUTE_PERF_OP_UPD) {
                    base_route.l3a_flags = BCM_L3_REPLACE;
                    base_route.l3a_intf = _l3_route_perf_obj2_id[unit];
                }

                rv = op_func[op](unit, &base_route);
            } else {
#if defined(BCM_TOMAHAWK_SUPPORT) && defined(ALPM_ENABLE)
                sal_memset(&base_defip, 0, sizeof(base_defip));
                base_defip.defip_vrf = vrf;
                base_defip.defip_ip_addr = ipAddr;
                base_defip.defip_sub_len = bcm_ip_mask_length(ipMask);
                base_defip.defip_intf = _l3_route_perf_obj_id[unit];
                if (op == _L3_ROUTE_PERF_OP_UPD) {
                    base_defip.defip_flags = BCM_L3_REPLACE;
                    base_defip.defip_intf = _l3_route_perf_obj2_id[unit];
                }

                rv = op_func_defip[op](unit, &base_defip);
#endif
            }
            if (rv < 0) {
                end = sal_time_usecs();
                duration += end - start;
                /*cli_out("%s %dth route returned msg: %s\n",
                        _l3_route_perf_op_str[op], idx,
                        bcm_errmsg(rv));*/
                goto End4;
                /*return;*/
            } else {
                *route_cnt += 1;
            }
        }
        end = sal_time_usecs();
        duration += end - start;
    }

End4:
    _l3_route_perf_data_add(route_perf, duration);
    return rv;
}

STATIC void
_l3_route_perf_v4_test(int unit, int loops, int nsets, int use_api,
                       int op_bitmap, int vrf, uint8 alpm_mode)
{
    int run, i;
    int route_added = 0, set_size = 0, route_cnt = 0, tot_route = 0;
    bcm_l3_route_t base_route;
    _bcm_defip_cfg_t base_defip;
    void *pb = NULL;

    _l3_route_perf_data_t *route_add_perf = NULL;
    _l3_route_perf_data_t *route_get_perf = NULL;
    _l3_route_perf_data_t *route_del_perf = NULL;
    _l3_route_perf_data_t *route_upd_perf = NULL;

    route_add_perf = sal_alloc(sizeof(_l3_route_perf_data_t) * nsets, "add_perf");
    route_get_perf = sal_alloc(sizeof(_l3_route_perf_data_t) * nsets, "get_perf");
    route_del_perf = sal_alloc(sizeof(_l3_route_perf_data_t) * nsets, "del_perf");
    route_upd_perf = sal_alloc(sizeof(_l3_route_perf_data_t) * nsets, "upd_perf");

    for (i = 0; i < nsets; i++) {
        _l3_route_perf_data_init(&route_add_perf[i]);
        _l3_route_perf_data_init(&route_get_perf[i]);
        _l3_route_perf_data_init(&route_del_perf[i]);
        _l3_route_perf_data_init(&route_upd_perf[i]);
    }

    for (run = 0; run < loops; run ++) {
        int     rv = 0;

        route_added = 0;

        /* Default route */
        if (use_api) {
            bcm_l3_route_t_init(&base_route);
            base_route.l3a_intf = _l3_route_perf_obj_id[unit];
            base_route.l3a_vrf = vrf;
            base_route.l3a_subnet = 0;
            base_route.l3a_ip_mask = 0;
            rv = bcm_l3_route_add(unit, &base_route);
        } else {
            sal_memset(&base_defip, 0, sizeof(base_defip));
#if defined(BCM_TOMAHAWK_SUPPORT) && defined(ALPM_ENABLE)
            base_defip.defip_intf = _l3_route_perf_obj_id[unit];
            base_defip.defip_vrf  = vrf;
            rv = bcm_esw_alpm_insert(unit, &base_defip);
#endif
        }

        if (BCM_FAILURE(rv)) {
            cli_out("Unable to add IPv4 default route (return %d)\n", rv);
            goto done;
        }

        for (i = 0; i < nsets; i++) {
            rv = _l3_route_perf_v4_op_test(unit, _L3_ROUTE_PERF_OP_ADD, use_api, i,
                                           &route_add_perf[i], &route_cnt, vrf);
            route_added += route_cnt;
            if (BCM_FAILURE(rv)) {
                nsets = i + 1;
                if (i == 0) {
                    set_size = route_cnt;
                }
                break;
            } else {
                set_size = route_cnt;
            }
        }

        if (op_bitmap & (1 << (_L3_ROUTE_PERF_OP_UPD - 1))) {
            for (i = 0; i < nsets; i++) {
                rv = _l3_route_perf_v4_op_test(unit, _L3_ROUTE_PERF_OP_UPD, use_api, i,
                                               &route_upd_perf[i], &route_cnt, vrf);
                if (BCM_FAILURE(rv)) {
                    break;
                }
            }
        }

        if (op_bitmap & (1 << (_L3_ROUTE_PERF_OP_GET - 1))) {
            for (i = 0; i < nsets; i++) {
                rv = _l3_route_perf_v4_op_test(unit, _L3_ROUTE_PERF_OP_GET, use_api, i,
                                               &route_get_perf[i], &route_cnt, vrf);
                if (BCM_FAILURE(rv)) {
                    break;
                }
            }
        }

        if (op_bitmap & (1 << (_L3_ROUTE_PERF_OP_DEL - 1))) {
            for (i = 0; i < nsets; i++) {
                rv = _l3_route_perf_v4_op_test(unit, _L3_ROUTE_PERF_OP_DEL, use_api, i,
                                               &route_del_perf[i], &route_cnt, vrf);
                if (BCM_FAILURE(rv)) {
                    break;
                }
            }

            /* Cleanup */
            /* Delete the default route */
            if (use_api) {
                rv = bcm_l3_route_delete(unit, &base_route);
            } else {
#if defined(BCM_TOMAHAWK_SUPPORT) && defined(ALPM_ENABLE)
                rv = bcm_esw_alpm_delete(unit, &base_defip);
#endif
            }

            if (BCM_FAILURE(rv)) {
                cli_out("Failed to delete IPv4 default route (return %d)\n", rv);
                goto done;
            }

            /* For test purpose, flush heap memory by allocate 16MB and free,
               in order to trigger garbage defragmentation before next run */
            pb = NULL;
            pb = sal_alloc(1024*1024*16, "alpm_mem_flush");
            if (pb) {
                sal_free(pb);
            }
	}
    }

    cli_out("IPv4 (%s) performance test: Added routes = %d\n",
            (alpm_mode == 2 ? "Combined" : (vrf < 0 ? "Global" : "Private")),
            route_added);
    cli_out(" Set RouteCnt TotRoute  RPS:Add  Update   Lookup   Delete \n");

    tot_route = 0;
    for (i = 0; i < nsets; i++) {
        if ((tot_route + set_size) > route_added) {
            route_cnt = route_added - tot_route;
        } else {
            route_cnt = set_size;
        }
        tot_route += route_cnt;

        cli_out("%3d  %7d  %7d ", i, route_cnt, tot_route);

        _l3_route_perf_data_print(&route_add_perf[i], route_cnt, "");
        _l3_route_perf_data_print(&route_upd_perf[i], route_cnt, "");
        _l3_route_perf_data_print(&route_get_perf[i], route_cnt, "");
        _l3_route_perf_data_print(&route_del_perf[i], route_cnt, "");
        cli_out("\n");
    }
    cli_out("\n");

done:
    sal_free(route_add_perf);
    sal_free(route_upd_perf);
    sal_free(route_get_perf);
    sal_free(route_del_perf);
}

STATIC int
_l3_route_perf_v6_op_test(int unit, int op, int use_api, int set_idx,
                          _l3_route_perf_data_t *route_perf, int *route_cnt, int vrf)
{
    int         i, rv = 0;
    int         start, end, duration = 0;
    bcm_l3_route_t base_route;
    _bcm_defip_cfg_t base_defip;

    int (*op_func[5])(int, bcm_l3_route_t *) = {
        NULL,
        bcm_l3_route_add,
        bcm_l3_route_add,
        bcm_l3_route_get,
        bcm_l3_route_delete
    };

#if defined(BCM_TOMAHAWK_SUPPORT) && defined(ALPM_ENABLE)
    int (*op_func_defip[5])(int, _bcm_defip_cfg_t *) = {
        NULL,
        bcm_esw_alpm_insert,
        bcm_esw_alpm_update,
        bcm_esw_alpm_lookup,
        bcm_esw_alpm_delete
    };
#endif


    /* 1022 Entries */
    int    count = 1022;
    bcm_ip6_t v6_mask = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
    bcm_ip6_t v6_ip   = {0x70,0x00,0x00,0x01,0x00,0x01,0x00,0x01,0x01,0x01,0x02,0x02,0x03,0x03,0x04,0x04};
    int pfx_bit_loc = 32;
    int pfx = 0x110 + count * set_idx;

#ifdef ALPM_ENABLE
    if (soc_feature(unit, soc_feature_alpm)) {
        if (!soc_property_get(unit, spn_IPV6_LPM_128B_ENABLE, 1)) {
            sal_memset(v6_mask+8, 0, 8); /* IPv6-64 */
        }
    }
#endif

    *route_cnt = 0;

    bcm_l3_route_t_init(&base_route);
    sal_memset(&base_defip, 0, sizeof(base_defip));

    /* Insert entry */
    increment_ip6addr(v6_ip, pfx_bit_loc/8 - 1, pfx);
    start = sal_time_usecs();
    for (i = 0; i < count; i++) {
        increment_ip6addr(v6_ip, pfx_bit_loc/8 - 1, 1);

        if (use_api) {
            base_route.l3a_vrf = vrf;
            sal_memcpy(base_route.l3a_ip6_net, v6_ip, sizeof(v6_ip));
            sal_memcpy(base_route.l3a_ip6_mask, v6_mask, sizeof(v6_mask));
            base_route.l3a_intf = _l3_route_perf_obj_id[unit];
            if (op == _L3_ROUTE_PERF_OP_UPD) {
                base_route.l3a_flags = BCM_L3_REPLACE;
                base_route.l3a_intf = _l3_route_perf_obj2_id[unit];
            }
            base_route.l3a_flags |= BCM_L3_IP6;

            rv = op_func[op](unit, &base_route);
        } else {
#if defined(BCM_TOMAHAWK_SUPPORT) && defined(ALPM_ENABLE)
            sal_memset(&base_defip, 0, sizeof(base_defip));
            base_defip.defip_vrf = vrf;
            sal_memcpy(base_defip.defip_ip6_addr, v6_ip, sizeof(v6_ip));
            base_defip.defip_sub_len = bcm_ip6_mask_length(v6_mask);
            base_defip.defip_intf = _l3_route_perf_obj_id[unit];
            if (op == _L3_ROUTE_PERF_OP_UPD) {
                base_defip.defip_flags = BCM_L3_REPLACE;
                base_defip.defip_intf = _l3_route_perf_obj2_id[unit];
            }
            base_defip.defip_flags |= BCM_L3_IP6;

            rv = op_func_defip[op](unit, &base_defip);
#endif
        }
        if (rv < 0) {
            end = sal_time_usecs();
            duration = end - start;
            /*cli_out("%s %dth route returned msg: %s\n",
                    _l3_route_perf_op_str[op], i,
                    bcm_errmsg(rv));*/
            goto End6;
        } else {
            *route_cnt += 1;
        }
    }
    end = sal_time_usecs();
    duration = end - start;

End6:
    _l3_route_perf_data_add(route_perf, duration);
    return rv;
}

STATIC void
_l3_route_perf_v6_test(int unit, int loops, int nsets, int use_api,
                       int op_bitmap, int vrf, uint8 alpm_mode)
{
    int run, i;
    int route_added = 0, set_size = 0, route_cnt = 0, tot_route = 0;
    bcm_l3_route_t base_route;
    _bcm_defip_cfg_t base_defip;
    void *pb = NULL;

    _l3_route_perf_data_t *route_add_perf = NULL;
    _l3_route_perf_data_t *route_get_perf = NULL;
    _l3_route_perf_data_t *route_del_perf = NULL;
    _l3_route_perf_data_t *route_upd_perf = NULL;

    route_add_perf = sal_alloc(sizeof(_l3_route_perf_data_t) * nsets, "add_perf");
    route_get_perf = sal_alloc(sizeof(_l3_route_perf_data_t) * nsets, "get_perf");
    route_del_perf = sal_alloc(sizeof(_l3_route_perf_data_t) * nsets, "del_perf");
    route_upd_perf = sal_alloc(sizeof(_l3_route_perf_data_t) * nsets, "upd_perf");

    for (i = 0; i < nsets; i++) {
        _l3_route_perf_data_init(&route_add_perf[i]);
        _l3_route_perf_data_init(&route_get_perf[i]);
        _l3_route_perf_data_init(&route_del_perf[i]);
        _l3_route_perf_data_init(&route_upd_perf[i]);
    }

    for (run = 0; run < loops; run ++) {
        int     rv = 0;

        route_added = 0;

        /* Default route */
        if (use_api) {
            bcm_l3_route_t_init(&base_route);
            base_route.l3a_intf = _l3_route_perf_obj_id[unit];
            base_route.l3a_vrf = vrf;
            base_route.l3a_flags = BCM_L3_IP6;
            rv = bcm_l3_route_add(unit, &base_route);
        } else {
            sal_memset(&base_defip, 0, sizeof(base_defip));
#if defined(BCM_TOMAHAWK_SUPPORT) && defined(ALPM_ENABLE)
            base_defip.defip_intf = _l3_route_perf_obj_id[unit];
            base_defip.defip_vrf  = vrf;
            base_defip.defip_flags = BCM_L3_IP6;
            rv = bcm_esw_alpm_insert(unit, &base_defip);
#endif
        }

        if (BCM_FAILURE(rv)) {
            cli_out("Unable to add IPv6 default route (return %d)\n", rv);
            goto done;
        }

        for (i = 0; i < nsets; i++) {
            rv = _l3_route_perf_v6_op_test(unit, _L3_ROUTE_PERF_OP_ADD, use_api, i,
                                           &route_add_perf[i], &route_cnt, vrf);
            route_added += route_cnt;
            if (BCM_FAILURE(rv)) {
                nsets = i + 1;
                if (i == 0) {
                    set_size = route_cnt;
                }
                break;
            } else {
                set_size = route_cnt;
            }
        }

        if (op_bitmap & (1 << (_L3_ROUTE_PERF_OP_UPD - 1))) {
            for (i = 0; i < nsets; i++) {
                rv = _l3_route_perf_v6_op_test(unit, _L3_ROUTE_PERF_OP_UPD, use_api, i,
                                               &route_upd_perf[i], &route_cnt, vrf);
                if (BCM_FAILURE(rv)) {
                    break;
                }
            }
        }

        if (op_bitmap & (1 << (_L3_ROUTE_PERF_OP_GET - 1))) {
            for (i = 0; i < nsets; i++) {
                rv = _l3_route_perf_v6_op_test(unit, _L3_ROUTE_PERF_OP_GET, use_api, i,
                                               &route_get_perf[i], &route_cnt, vrf);
                if (BCM_FAILURE(rv)) {
                    break;
                }
            }
        }

        if (op_bitmap & (1 << (_L3_ROUTE_PERF_OP_DEL - 1))) {
            for (i = 0; i < nsets; i++) {
                rv = _l3_route_perf_v6_op_test(unit, _L3_ROUTE_PERF_OP_DEL, use_api, i,
                                               &route_del_perf[i], &route_cnt, vrf);
                if (BCM_FAILURE(rv)) {
                    break;
                }
            }

            /* Cleanup */
            /* Delete the default route */
            if (use_api) {
                rv = bcm_l3_route_delete(unit, &base_route);
            } else {
#if defined(BCM_TOMAHAWK_SUPPORT) && defined(ALPM_ENABLE)
                rv = bcm_esw_alpm_delete(unit, &base_defip);
#endif
            }

            if (BCM_FAILURE(rv)) {
                cli_out("Failed to delete IPv6 default route (return %d)\n", rv);
                goto done;
            }

            /* For test purpose, flush heap memory by allocate 16MB and free,
               in order to trigger garbage defragmentation before next run */
            pb = NULL;
            pb = sal_alloc(1024*1024*16, "alpm_mem_flush");
            if (pb) {
                sal_free(pb);
            }
	}
    }

    cli_out("IPv6 (%s) performance test: Added routes = %d\n",
            (alpm_mode == 2 ? "Combined" : (vrf < 0 ? "Global" : "Private")),
            route_added);
    cli_out(" Set RouteCnt TotRoute  RPS:Add  Update   Lookup   Delete \n");

    tot_route = 0;
    for (i = 0; i < nsets; i++) {
        if ((tot_route + set_size) > route_added) {
            route_cnt = route_added - tot_route;
        } else {
            route_cnt = set_size;
        }
        tot_route += route_cnt;

        cli_out("%3d  %7d  %7d ", i, route_cnt, tot_route);

        _l3_route_perf_data_print(&route_add_perf[i], route_cnt, "");
        _l3_route_perf_data_print(&route_upd_perf[i], route_cnt, "");
        _l3_route_perf_data_print(&route_get_perf[i], route_cnt, "");
        _l3_route_perf_data_print(&route_del_perf[i], route_cnt, "");
        cli_out("\n");
    }
    cli_out("\n");

done:
    sal_free(route_add_perf);
    sal_free(route_upd_perf);
    sal_free(route_get_perf);
    sal_free(route_del_perf);

}

STATIC cmd_result_t
_l3_cmd_route_perf_test(int unit, args_t *a)
{
    int cons_st;
    int ip_type = 3;
    int loops = 3;
    int nonset, nsets = -1;
    int op_bitmap = 0xf;
    int use_api = 1;

    cmd_result_t cmd_rv = CMD_OK;
    parse_table_t pt;
    uint8 alpm_mode = 0;

    /* v4v6: 1 (IPv4 only), 2 (IPv6 only), 3 (IPv4 first then IPv6), 4 (IPv6 first then IPv4)
       OPs: bit0 (Add), bit1 (Update), bit2 (Lookup), bit3 (Delete)
       For capacity test: bit3 (Delete) must be disabled from OPs, e.g.
         l3 route test perf v4v6=3 Ops=7
       When not Delete, loops will be forced to 1.
       Global routes will be included automatically for Parallel or TCAM mixed modes.
    */

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "v4v6", PQ_DFL | PQ_INT, (void *)3,
                    (void *)&ip_type, 0);
    parse_table_add(&pt, "loops", PQ_DFL | PQ_INT, (void *)3,
                    (void *)&loops, 0);
    parse_table_add(&pt, "nsets", PQ_DFL | PQ_INT, (void *)-1,
                    (void *)&nsets, 0);
    parse_table_add(&pt, "UseApi", PQ_DFL | PQ_INT, (void *)1,
                    (void *)&use_api, 0);
    parse_table_add(&pt, "OPs", PQ_DFL | PQ_INT, (void *)0xf,
                    (void *)&op_bitmap, 0);

    if (!parseEndOk(a, &pt, &cmd_rv)) {
        return cmd_rv;
    }
    /* OP_ADD is must */
    if ((op_bitmap & (1 << (_L3_ROUTE_PERF_OP_ADD - 1))) == 0) {
        return CMD_USAGE;
    }

    /* Force loops=1 if not OP_DEL */
    if ((op_bitmap & (1 << (_L3_ROUTE_PERF_OP_DEL - 1))) == 0) {
        loops = 1;
    }

    cons_st = bslcons_is_enabled();
    if (cons_st) {
        bslcons_enable(0);
    }
    _l3_route_perf_test_setup(unit);
    bslcons_enable(cons_st);

#ifdef ALPM_ENABLE
    if (soc_feature(unit, soc_feature_alpm)) {
        alpm_mode = soc_property_get(unit, spn_L3_ALPM_ENABLE, 0);
    }
#endif
    cli_out("Test runs: %d\n", loops);
#ifdef ALPM_ENABLE
    cli_out("ALPM mode: %s\n",
            alpm_mode == 1 ? "Parallel" :
            alpm_mode == 2 ? "Combined" :
            alpm_mode == 3 ? "ALPM/TCAM Mixed" : "Disabled");
#endif
    nonset = (nsets == -1);
     /* nsets > 0 is only useful for first test, after that nsets will be unlimited (until FULL) */

    if ((ip_type == 1) || (ip_type == 3)) { /* IPv4 first */
        if (nonset) {
            nsets = 130;
        }
        /* Private VRF 100 */
        _l3_route_perf_v4_test(unit, loops, nsets, use_api, op_bitmap,
                               _l3_route_perf_vrf, alpm_mode);
#ifdef ALPM_ENABLE
         /* Global low route test for Paralle & ALPM/TCAM mixed modes */
        if ((alpm_mode == 1) || (alpm_mode == 3)) {
            _l3_route_perf_v4_test(unit, loops, nsets, use_api, op_bitmap,
                                   BCM_L3_VRF_GLOBAL, alpm_mode);
        }
#endif
        nonset = 1;
    }

    if (ip_type != 1) { /* IPv6 */
        if (nonset) {
            nsets = 600;
        }
        /* Private VRF 100 */
        _l3_route_perf_v6_test(unit, loops, nsets, use_api, op_bitmap,
                               _l3_route_perf_vrf, alpm_mode);
#ifdef ALPM_ENABLE
         /* Global low route test for Paralle & ALPM/TCAM mixed modes */
        if ((alpm_mode == 1) || (alpm_mode == 3)) {
            _l3_route_perf_v6_test(unit, loops, nsets, use_api, op_bitmap,
                                   BCM_L3_VRF_GLOBAL, alpm_mode);
        }
#endif
        nonset = 1;
    }

    if (ip_type == 4) { /* then IPv4 */
        if (nonset) {
            nsets = 130;
        }
        /* Private VRF 100 */
        _l3_route_perf_v4_test(unit, loops, nsets, use_api, op_bitmap,
                               _l3_route_perf_vrf, alpm_mode);
#ifdef ALPM_ENABLE
         /* Global low route test for Paralle & ALPM/TCAM mixed modes */
        if ((alpm_mode == 1) || (alpm_mode == 3)) {
            _l3_route_perf_v4_test(unit, loops, nsets, use_api, op_bitmap,
                                   BCM_L3_VRF_GLOBAL, alpm_mode);
        }
#endif
    }

    cons_st = bslcons_is_enabled();
    if (cons_st) {
        bslcons_enable(0);
    }
    _l3_route_perf_test_clean(unit);
    bslcons_enable(cons_st);

    return CMD_OK;
}

#ifdef ALPM_ENABLE
/* Usage: 
 *   l3 alpm show [all] [VRF=<val>] [global-vrf=<bool>]
 *                [Pivot=<bool>] [Bucket=<bool> [Counter=<bool] 
 *                [BucketuSage=<bool>] [InternalDebug=<bool>
 *  l3 alpm show brief [<Bucket>=<val>] [<Index>=<val>]
 *                  - Show ALPM brief debug info
 */

STATIC cmd_result_t
_l3_cmd_alpm_debug_show(int unit, args_t *a)
{
    int rv = CMD_OK;
    cmd_result_t cmd_rv;
    uint32 flags = 0;
    char *c = NULL;
    
    int vrf = -1;
    int global_vrf = 0;
    int pivot = 0;
    int bucket = 0;
    int counter = 0;
    int bkt_usage = 0;
    int internal_debug = 0;
    int trie_tree = 0;
    int index = -1;
#if defined(BCM_TOMAHAWK_SUPPORT)
    int app = 0;
#endif
    
    parse_table_t   pt;

    c = ARG_CUR(a);

    if (c != NULL && sal_strcasecmp(c, "brief") == 0) {
        ARG_NEXT(a);
        
        bucket = -1;
        index = -1;

        if (ARG_CNT(a)) {
            parse_table_init(unit, &pt);
            parse_table_add(&pt, "Bucket", PQ_DFL | PQ_INT, (void *)-1, 
                            (void *)&bucket, 0);
            parse_table_add(&pt, "Index", PQ_DFL | PQ_INT, (void *)-1, 
                            (void *)&index, 0);
            if (!parseEndOk(a, &pt, &cmd_rv)) {
                return cmd_rv;
            }
        }

#if defined(BCM_TOMAHAWK_SUPPORT)
        if (soc_feature(unit, soc_feature_alpm2)) {
            rv = bcm_esw_alpm_debug_brief_show(unit, app, bucket, index);
        } else
#endif
#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_APACHE_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TOMAHAWKX(unit) ||
            SOC_IS_APACHE(unit) ||
            SOC_IS_TRIDENT3X(unit)) {
            rv = soc_th_alpm_debug_brief_show(unit, bucket, index);
        } else 
#endif
        {
            rv = soc_alpm_debug_brief_show(unit, bucket, index);
        }
        if (rv < 0) {
            return CMD_USAGE;
        }
        return CMD_OK;
        
    } else if (c != NULL && sal_strcasecmp(c, "all") == 0) {
        ARG_NEXT(a);
    } else {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "VRF", PQ_DFL | PQ_INT, (void *)-1, 
                        (void *)&vrf, 0);
        parse_table_add(&pt, "Global-VRF", PQ_DFL | PQ_BOOL, 0, 
                        (void *)&global_vrf, 0);
        parse_table_add(&pt, "Pivot", PQ_DFL | PQ_BOOL, 0, 
                        (void *)&pivot, 0);
        parse_table_add(&pt, "Bucket", PQ_DFL | PQ_BOOL, 0, 
                        (void *)&bucket, 0);
        parse_table_add(&pt, "Counter", PQ_DFL | PQ_BOOL, 0, 
                        (void *)&counter, 0);   
        parse_table_add(&pt, "BucketuSage", PQ_DFL | PQ_BOOL, 0,
                        (void *)&bkt_usage, 0); 
        parse_table_add(&pt, "InternalDebug", PQ_DFL | PQ_BOOL, 0,
                        (void *)&internal_debug, 0);     
        parse_table_add(&pt, "TrieTree", PQ_DFL | PQ_BOOL, 0,
                        (void *)&trie_tree, 0);

        if (!parseEndOk(a, &pt, &cmd_rv)) {
            return cmd_rv;
        }

        if (global_vrf == TRUE) {
            vrf = SOC_VRF_MAX(unit) + 1;
        }

        if (pivot == TRUE) {
            flags |= SOC_ALPM_DEBUG_SHOW_FLAG_PVT;
        }

        if (bucket == TRUE) {
            flags |= SOC_ALPM_DEBUG_SHOW_FLAG_BKT;
        }

        if (counter == TRUE) {
            flags |= SOC_ALPM_DEBUG_SHOW_FLAG_CNT;
        }

        if (internal_debug == TRUE) {
            flags |= SOC_ALPM_DEBUG_SHOW_FLAG_INTDBG;
        }

        if (trie_tree == TRUE) {
            flags |= SOC_ALPM_DEBUG_SHOW_FLAG_TRIE_TREE;
        }

        if (bkt_usage == TRUE) {
            flags |= SOC_ALPM_DEBUG_SHOW_FLAG_BKTUSG;
        }
    }

    if (flags == 0) {
        flags = SOC_ALPM_DEBUG_SHOW_FLAG_ALL;
    }

#ifdef BCM_TOMAHAWK_SUPPORT
    if (soc_feature(unit, soc_feature_alpm2)) {
        rv = bcm_esw_alpm_debug_show(unit, app, vrf, flags);
    } else if (SOC_IS_TOMAHAWKX(unit) ||
               SOC_IS_APACHE(unit) ||
               SOC_IS_TRIDENT3X(unit)) {
        rv = soc_th_alpm_debug_show(unit, vrf, flags);
    } else
#endif
    {
        rv = soc_alpm_debug_show(unit, vrf, flags);
    }
    if (rv < 0) {
        return CMD_USAGE;
    }

    return CMD_OK;
}

/* Usage: 
 *   l3 alpm find [<VRF>=<val>] <IP4>=<val> <IP6>=<val> [<Length>=<val>]
 */
STATIC cmd_result_t
_l3_cmd_alpm_find(int unit, args_t *a)
{
    int             rv = BCM_E_NONE;
    cmd_result_t    cmd_rv = CMD_OK;
    bcm_ip6_t       ip6addr = {0};
    bcm_ip_t        ip4addr = 0;
    int             ip_len = BCM_L3_IP4_MAX_NETLEN;
    bcm_vrf_t       vrf = 1;
    parse_table_t   pt; 
    int             for_host = 0;
    int             index;
    char            ip_str[IP6ADDR_STR_LEN];
    int             is_ip4 = 0;
    int             is_ip6 = 0;

    bcm_l3_host_t   host;
    bcm_l3_route_t  route;
    bcm_l3_route_t  input;
    
    char            *subcmd = NULL;

    int arg_index_placeholder = ARG_CUR_INDEX(a);
    while ((subcmd = ARG_CUR(a)) != NULL) {
        if (parse_cmp("ip4", subcmd, '=')) {
            is_ip4 = 1;
        }

        if (parse_cmp("ip6", subcmd, '=')) {
            is_ip6 = 1;
        }

        ARG_NEXT(a);
    }

    while (arg_index_placeholder < ARG_CUR_INDEX(a)) {
        ARG_PREV(a);
    }


    if ((is_ip4 && is_ip6) || (!is_ip4 && !is_ip6)) {
        return CMD_USAGE;
    }


    parse_table_init(unit, &pt);
    parse_table_add(&pt, "VRF", PQ_DFL | PQ_INT, (void *)1, 
                    (void *)&vrf, 0);    
    parse_table_add(&pt, "IP4", PQ_DFL | PQ_IP, 0,
                    (void *)&ip4addr, 0);    
    parse_table_add(&pt, "IP6", PQ_DFL | PQ_IP6, 0,
                    (void *)&ip6addr, 0); 
    parse_table_add(&pt, "Length", PQ_DFL | PQ_INT, 
                    (void *)BCM_L3_IP4_MAX_NETLEN, 
                    (void *)&ip_len, 0);
    
    if (!parseEndOk(a, &pt, &cmd_rv)) {
        return cmd_rv;
    }      

    if (is_ip4 != 0) {
        if (ip_len == BCM_L3_IP4_MAX_NETLEN) {
            for_host = 1;
        }
    } else {
        if (ip_len == BCM_L3_IP6_MAX_NETLEN) {
            for_host = 1;
        }
    }

    bcm_l3_host_t_init(&host);
    bcm_l3_route_t_init(&route);
    bcm_l3_route_t_init(&input);
    if (for_host) {
        host.l3a_vrf = vrf;
        if (is_ip4 != 0) {
            host.l3a_ip_addr = ip4addr;
            format_ipaddr(ip_str, ip4addr);
        } else {
            host.l3a_flags |= BCM_L3_IP6;
            sal_memcpy(host.l3a_ip6_addr, ip6addr, BCM_IP6_ADDRLEN);
            format_ip6addr(ip_str, ip6addr);
        }

        rv = bcm_l3_route_find(unit, &host, &route);
    } else {
        input.l3a_vrf = vrf;
        if (is_ip4 != 0) {
            input.l3a_subnet = ip4addr;
            input.l3a_ip_mask = bcm_ip_mask_create(ip_len);
            format_ipaddr(ip_str, ip4addr);
        } else {
            input.l3a_flags |= BCM_L3_IP6;
            bcm_ip6_mask_create(input.l3a_ip6_mask, ip_len);
            sal_memcpy(input.l3a_ip6_net, ip6addr, BCM_IP6_ADDRLEN);
            format_ip6addr(ip_str, ip6addr);
        }

        rv = bcm_l3_subnet_route_find(unit, &input, &route);
    }
    cli_out("Matching <%d, %s/%d> ==> %s\n", vrf, ip_str, ip_len, 
            bcm_errmsg(rv));
    
    if (rv >= 0) {
        if (route.l3a_flags & BCM_L3_IP6) {
            cli_out("%-7s %-6s %-41s %-18s %-4s %4s %5s %1s %4s %4s %s\n", "#", 
                    "VRF", "Net-addr", "Next-Hop-Mac", "INTF", "MODID", "PORT", 
                    "PRI", "CLASS", "HIT", "VLAN");
        } else {
            cli_out("%-7s %-6s %-19s %-18s %-4s %4s %5s %1s %4s %4s %s\n", "#", 
                    "VRF", "Net-addr", "Next-Hop-Mac", "INTF", "MODID", "PORT", 
                    "PRI", "CLASS", "HIT", "VLAN");
        }
        
        index = route.l3a_tunnel_option;
        route.l3a_tunnel_option = 0;
        _l3_cmd_route_print(unit, index, &route, NULL);
    } 

    return CMD_OK;
}

/* Process L3 ALPM Sanity Check 
 * l3 alpm sanity [<memory>=<val>] [<index>=<val>] 
 */
STATIC cmd_result_t
_l3_cmd_alpm_sanity_check(int unit, args_t *a)
{
    cmd_result_t    cmd_rv = CMD_OK;
    int             rv = BCM_E_NONE;
    static char     *mem_name = NULL;
    int             index = -1;
    int             check_sw = 1;
    int             copyno;
    parse_table_t   pt;
    soc_mem_t       mem;
    int             def_type = 0;
    char *def_mem[] = {"L3_DEFIP", "L3_DEFIP_LEVEL1"};
#if defined(BCM_TOMAHAWK_SUPPORT)
    int             app = 0;
#endif

    if (SOC_MEM_IS_VALID(unit, L3_DEFIP_LEVEL1m)) {
        def_type = 1;
    }

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Memory", PQ_STRING|PQ_STATIC,
                    def_mem[def_type], &mem_name, 0);
    parse_table_add(&pt, "Index",  PQ_INT, (void *)-1, &index, 0);
    parse_table_add(&pt, "Sw",  PQ_INT, (void *)1, &check_sw, 0);
    /* alpm2 OK without args */
    if (!soc_feature(unit, soc_feature_alpm2) || ARG_CNT(a)) {
        if (!parseEndOk(a, &pt, &cmd_rv)) {
        return cmd_rv;
    }

    if (parse_memory_name(unit, &mem, mem_name, &copyno, 0) < 0) {
        cli_out("Parse memory name failed\n");
        return CMD_USAGE;
        }
    }
#if defined(BCM_TOMAHAWK_SUPPORT)
    if (soc_feature(unit, soc_feature_alpm2)) {
        rv = bcm_esw_alpm_sanity_check(unit, app, mem, index, check_sw);
    } else
#endif
#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_APACHE_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_APACHE(unit) || SOC_IS_TRIDENT3X(unit)) {
        rv = soc_th_alpm_sanity_check(unit, mem, index, check_sw);
    } else 
#endif
    {
        rv = soc_alpm_sanity_check(unit, mem, index, check_sw);
    }
    if (BCM_FAILURE(rv)) {
        cli_out("%s: ALPM sanity check failed: %s\n",
                ARG_CMD(a), bcm_errmsg(rv));
        return (CMD_FAIL);
    } else {
        cli_out("L3 ALPM sanity check: passed!\n");
    }
    return (CMD_OK);
}


char cmd_esw_alpm_usage[] =
    "Parameters: <init|cust1|v4|v6|pvt|bkt|fmt> <show|add>\n\t"
    "alpm init\n\t"
    "alpm hitbit <enable|disable>\n\t"
    "alpm trace <enable|disable|clear>\n\t"
    "alpm trace sanity [enable=<val>] [Start=<val>] [End=<val>]\n\t"
    "alpm trace wrap <enable|disable>\n\t"
    "alpm trace dump [Showflags=<yes|no>] [File=<filename>]\n\t"
    "alpm table dump [raw|chg|all|nocache] <mem> <index> <fmt> [<ent> <vrf_id>]\n\t"
    "\tFor Trident3, only accept L3_DEFIP_ALPM_RAW in <mem>\n\t"
    "\tFor Tomahawk3, other usage can be:\n\t"
    "\talpm table dump [nocache] <LEVEL1 mem> <index or -1(all)> <levels to print(1|2|3)> <entry 0|1>\n\t"
    "\t<index> is used to index <mem>\n\t"
    "\t<fmt> is used to parse <mem>\n\t"
    "\t<ent> is used to index entry in <mem>\n\t"
    "alpm debug info on [pvt|rte|cnt|hit|all|none]\n\t"
    "alpm data <save|restore> [FileName=<fn>]\n\t"
    "alpm defrag [vrf=<-2|x|-4 for all>] [ipt=<0|1|3 for all>] [time=<msec>] [l2merge=<cnt>] [l3merge=<cnt>]\n\t"
    "alpm bkt show [acb_idx]\n\t"
    "alpm fmt show [acb_idx]\n\t"
    "alpm pvtlen show [acb_idx]\n\t"
    "alpm stat show [acb_idx]\n\t"
    "alpm pvttrie show [acb_idx] [vrf] [ipt]\n\t"
    "alpm bkttrie show [acb_idx] [vrf] [ipt]\n\t"
    "alpm usage show [-1 for dump usage log]\n\t"
    "alpm projection show [v4v6_ratio x100]\n\t"
    "alpm tcam show\n\t"
    "alpm config show [acb_idx]\n\t"
    "alpm pvt show [acb_idx] [bkt]\n";

bcm_ip_t    base_ipv4 = 0x1000001;
bcm_ip6_t   base_ipv6 = {
    0x2a,0x01,0x01,0x11,0xf4,0x00,0xe0,0x87,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT)
#endif

#define ALPM_TRACE_DUMP_SIZE    10000

int _cmd_esw_alpm_trace_dump_cb(int u, void *user_data, char *log_data)
{
#ifndef NO_FILEIO
    if (user_data) {
        sal_fprintf((FILE *)user_data, "%s", log_data);
    } else
#endif
    {
        cli_out("%s", log_data);
    }

    return 0;
}

/* Dump ALPM trace log */
STATIC int
_cmd_esw_alpm_trace_dump(int u, int showflags, char *filename)
{
    int rv = BCM_E_NONE;
#ifndef NO_FILEIO
    int trace_cnt;
    FILE *fp = NULL;

#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT)
    if (soc_feature(u, soc_feature_alpm2)) {
        trace_cnt = bcm_esw_alpm_trace_cnt(u);
    } else
#endif
    {
        trace_cnt = _bcm_td2_alpm_trace_cnt(u);
    }
    if (filename[0] != 0) {
        fp = sal_fopen(filename, "w");
        if (fp == NULL) {
            cli_out("Open file %s failed\n", filename);
            rv = BCM_E_FAIL;
            return rv;
        }
    } else if (trace_cnt > ALPM_TRACE_DUMP_SIZE) {
        cli_out("Too big to dump! Suggest to use: alpm trace dump file=<name>\n");
        return rv;
    }

#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT)
    if (soc_feature(u, soc_feature_alpm2)) {
        rv = bcm_esw_alpm_trace_dump(u, showflags, _cmd_esw_alpm_trace_dump_cb, fp);
    } else
#endif
    {
        rv = _bcm_td2_alpm_trace_dump(u, showflags, _cmd_esw_alpm_trace_dump_cb, fp);
    }

    if (fp) {
        sal_fclose(fp);
    }
#else

    if (filename[0] != 0) {
        cli_out("File IO is not supported\n");
        return BCM_E_UNAVAIL;
    }

#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT)
    if (soc_feature(u, soc_feature_alpm2)) {
        rv = bcm_esw_alpm_trace_dump(u, showflags, _cmd_esw_alpm_trace_dump_cb, NULL);
    } else
#endif
    {
        rv = _bcm_td2_alpm_trace_dump(u, showflags, _cmd_esw_alpm_trace_dump_cb, NULL);
    }

#endif /* FILE_IO */

    return rv;
}

/* TH3/TD3 ALPM data save and restore */
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT)

STATIC int
_alpm_data_write_cb(int u, void *fp, char *data, int num)
{
    int rv = 0;
#ifndef NO_FILEIO
    if (fp) {
        rv = sal_fwrite(data, 1, num, (FILE *)fp);
    }
#endif
    return rv;
}

STATIC int
_alpm_data_read_cb(int u, void *fp, char *data, int num)
{
    int rv = 0;
#ifndef NO_FILEIO
    if (fp) {
        rv = sal_fread(data, 1, num, (FILE *)fp);
    }
#endif
    return rv;
}

int
_alpm_data_save(int u, char *filename)
{
    int rv = BCM_E_NONE;
#ifndef NO_FILEIO
    FILE *fp = NULL;

    fp = sal_fopen(filename, "wb");
    if (fp == NULL) {
        cli_out("Open binary file %s failed\n", filename);
        return BCM_E_FAIL;
    }

    rv = bcm_esw_alpm_data_save(u, _alpm_data_write_cb, fp);

    sal_fclose(fp);
    if (BCM_SUCCESS(rv)) {
        cli_out("ALPM data saved successfully to file: %s\n", filename);
    } else {
        cli_out("ALPM data failed to save to file: %s\n", filename);
    }
    return rv;
#else
    cli_out("File IO is not supported\n");
    return BCM_E_UNAVAIL;
#endif /* FILE_IO */
}

STATIC int
_alpm_data_restore(int u, char *filename)
{
    int rv = BCM_E_NONE;
#ifndef NO_FILEIO
    FILE *fp = NULL;

    if (BCM_XGS3_L3_DEFIP_IP4_CNT(u) || BCM_XGS3_L3_DEFIP_IP6_CNT(u)) {
        cli_out("ALPM DB not empty! Clear DB before restore (rc)\n");
        return BCM_E_FAIL;
    }
    fp = sal_fopen(filename, "rb");
    if (fp == NULL) {
        cli_out("Open binary file %s failed\n", filename);
        return BCM_E_FAIL;
    }

    rv = bcm_esw_alpm_data_restore(u, _alpm_data_read_cb, fp);

    sal_fclose(fp);

    if (BCM_SUCCESS(rv)) {
        cli_out("ALPM data restored successfully from file: %s\n", filename);
    } else {
        cli_out("ALPM data failed to restore from file: %s\n", filename);
    }
    return rv;
#else
    cli_out("File IO is not supported\n");
    return BCM_E_UNAVAIL;
#endif /* FILE_IO */
}

#endif /* TH3/TD3 ALPM data save and restore */

cmd_result_t
cmd_esw_alpm(int unit, args_t *a)
{
    char  *table, *subcmd_s;
    int   flags = 0;
    int (* _alpm_trace_set)(int, int) = _bcm_td2_alpm_trace_set;
    int (* _alpm_trace_clear)(int) = _bcm_td2_alpm_trace_clear;
    int (* _alpm_trace_sanity)(int, int, int, int) = _bcm_td2_alpm_trace_sanity;
    int (* _alpm_trace_wrap_set)(int, int) = _bcm_td2_alpm_trace_wrap_set;
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT)
    int   subcmd = 0;
    int   acb_bmp = 3;
    int   index = 0;
    int   fmt = 1;
    int   ent_idx = 0;
    int   vrf_id = 1;
    int   copyno;
    int   app = 0;
    soc_mem_t mem;
#endif
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT)
    if (soc_feature(unit, soc_feature_alpm2)) {
        _alpm_trace_set = bcm_esw_alpm_trace_set;
        _alpm_trace_clear = bcm_esw_alpm_trace_clear;
        _alpm_trace_sanity = bcm_esw_alpm_trace_sanity;
        _alpm_trace_wrap_set = bcm_esw_alpm_trace_wrap_set;
    }
#endif

    if ((table = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

    if (!soc_property_get(unit, spn_L3_ALPM_ENABLE, 0)) {
        cli_out("ALPM is disabled!\n" );
        ARG_DISCARD(a);
        return CMD_OK;
    }

    if (sal_strcasecmp(table, "trace") == 0) {
        if ((subcmd_s = ARG_GET(a)) == NULL) {
            return CMD_USAGE;
        }
        if (sal_strcasecmp(subcmd_s, "enable") == 0) {
            (void)_alpm_trace_set(unit, TRUE);
        } else if (sal_strcasecmp(subcmd_s, "disable") == 0) {
            (void)_alpm_trace_set(unit, FALSE);
        } else if (sal_strcasecmp(subcmd_s, "clear") == 0) {
            (void)_alpm_trace_clear(unit);
        } else if (sal_strcasecmp(subcmd_s, "sanity") == 0) {
            parse_table_t pt;
            int enable = -1, start = 0, end = -1;

            parse_table_init(unit, &pt);
            /* -1 for show, 0 for disable, >0 for freq */
            parse_table_add(&pt, "enable", PQ_DFL | PQ_INT, (void *)-1,
                            (void *)&enable, 0);
            parse_table_add(&pt, "Start", PQ_DFL | PQ_INT, (void *)0,
                            (void *)&start, 0);
            parse_table_add(&pt, "End", PQ_DFL | PQ_INT, (void *)-1,
                            (void *)&end, 0); /* end=-1 means forever */
            if (parse_arg_eq(a, &pt) < 0) {
                parse_arg_eq_done(&pt);
                return CMD_USAGE;
            }

            (void)_alpm_trace_sanity(unit, enable, start, end);
            parse_arg_eq_done(&pt);

        } else if (sal_strcasecmp(subcmd_s, "dump") == 0) {
            parse_table_t pt;
            char *filename = NULL;

            parse_table_init(unit, &pt);
            parse_table_add(&pt, "Showflags", PQ_BOOL, (void *)0,
                            (void *)&flags, 0);
            parse_table_add(&pt, "File", PQ_STRING, NULL,
                                    (void *)&filename, NULL);
            if (parse_arg_eq(a, &pt) < 0) {
                parse_arg_eq_done(&pt);
                return CMD_USAGE;
            }

            (void) _cmd_esw_alpm_trace_dump(unit, flags, filename);
            parse_arg_eq_done(&pt);

        } else if (sal_strcasecmp(subcmd_s, "wrap") == 0) {
            int wrap = TRUE;
            if ((subcmd_s = ARG_GET(a)) == NULL) {
                return CMD_USAGE;
            }
            if (sal_strcasecmp(subcmd_s, "enable") == 0) {
                wrap = TRUE;
            } else if (sal_strcasecmp(subcmd_s, "disable") == 0) {
                wrap = FALSE;
            } else {
                return CMD_USAGE;
            }

            (void)_alpm_trace_wrap_set(unit, wrap);
        } else {
            return CMD_USAGE;
        }
        return CMD_OK;
    }

    if (sal_strcasecmp(table, "defrag") == 0) {
        parse_table_t pt;
        int rv;
        bcm_vrf_t vrf = BCM_L3_VRF_ALL;
        bcm_l3_alpm_ipt_t ipt = bcmL3AlpmIptAll;
        int max_time = -1, maxl2cnt = -1, maxl3cnt = -1, l2cnt = 0, l3cnt = 0;

        parse_table_init(unit, &pt);
        parse_table_add(&pt, "Vrf", PQ_DFL | PQ_INT, (void *)BCM_L3_VRF_ALL,
                        (void *)&vrf, 0);
        parse_table_add(&pt, "Ipt", PQ_DFL | PQ_INT, (void *)bcmL3AlpmIptAll,
                        (void *)&ipt, 0);
        parse_table_add(&pt, "Time", PQ_DFL | PQ_INT, (void *)-1,
                        (void *)&max_time, 0);
        parse_table_add(&pt, "l2merge", PQ_DFL | PQ_INT, (void *)-1,
                        (void *)&maxl2cnt, 0);
        parse_table_add(&pt, "l3merge", PQ_DFL | PQ_INT, (void *)-1,
                        (void *)&maxl3cnt, 0);

        rv = parse_arg_eq(a, &pt);
        if (rv < 0) {
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        } else if (rv == 0) {
            cli_out("\n  Usage: alpm defrag [Vrf=<-2|x|-4>] [Ipt=<0|1|3>] [Time=<msec>] [l2merge=<cnt>] [l3merge=<cnt>]\n");
            cli_out("          Vrf: >=0 for a specific private VRF, -2 for Global low, -4 for all VRFs\n");
            cli_out("          Ipt: 0 IPv4, 1 IPv6, 3 for all IP types\n");
            cli_out("         Time: Maximum time in msec, -1 for no time restriction\n");
            cli_out("      l2merge: Maximum L2 bucket merges, 0 for no L2 merge, -1 for all possible merges\n");
            cli_out("      l3merge: Maximum L3 bucket merges, 0 for no L3 merge, -1 for all possible merges\n\n");
        }

        if (ipt != bcmL3AlpmIptV4 && ipt != bcmL3AlpmIptV6 && ipt != bcmL3AlpmIptAll) {
            ipt = bcmL3AlpmIptAll;
        }

        if (SOC_IS_TOMAHAWK3(unit)) {
            cli_out("alpm defrag vrf=%d ipt=%d time=%d l2merge=%d l3merge=%d\n", vrf, ipt, max_time, maxl2cnt, maxl3cnt);

            rv = bcm_esw_alpm_defrag(unit, vrf, ipt, max_time, maxl2cnt, maxl3cnt, &l2cnt, &l3cnt);

            cli_out("  L2 bucket merge cnt: %d\n", l2cnt);
            cli_out("  L3 bucket merge cnt: %d\n", l3cnt);
            cli_out("  Defrag %s (rv=%d).\n", BCM_SUCCESS(rv) ? "OK" : "failed", rv);
        } else {
            cli_out("Currently unavailable for this chip\n");
        }
        parse_arg_eq_done(&pt);
        return CMD_OK;
    }
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT)
    if (!soc_feature(unit, soc_feature_alpm2)) {
        cli_out("Feature alpm2 is unavailable\n");
        return CMD_OK;
    }
    if (sal_strcasecmp(table, "init") == 0) {
        bcm_esw_alpm_init(unit);
        return CMD_OK;
    }
    if (sal_strcasecmp(table, "hitbit") == 0) {
        int rv;
        if ((subcmd_s = ARG_GET(a)) == NULL) {
            cli_out("Current settings:\n");
            cli_out("\tInterval=%d\n", alpm_dist_hitbit_interval_get(unit));
            return CMD_OK;
        }
        if (sal_strcasecmp(subcmd_s, "disable") == 0 ||
            sal_strcasecmp(subcmd_s, "off") == 0) {
            if (bcm_esw_alpm_hit_enabled(unit)) {
                rv = bcm_esw_alpm_hit_deinit(unit);
                (void)bcm_esw_alpm_hit_enable(unit, 0);
                if (BCM_SUCCESS(rv)) {
                    cli_out("ALPM hitbit disable OK!\n");
                } else {
                    cli_out("ALPM hitbit disable failed!\n");
                }
            }
        } else if (sal_strcasecmp(subcmd_s, "enable") == 0 ||
                   sal_strcasecmp(subcmd_s, "on") == 0) {
            if (!bcm_esw_alpm_hit_enabled(unit)) {
                (void)bcm_esw_alpm_hit_enable(unit, 1);
                rv = bcm_esw_alpm_hit_init(unit);
                if (BCM_SUCCESS(rv)) {
                    cli_out("ALPM hitbit enable OK!\n");
                } else {
                    cli_out("ALPM hitbit enable failed!\n");
                }
            }
        } else {
            return CMD_USAGE;
        }
        return CMD_OK;
    }

    if (sal_strcasecmp(table, "data") == 0) {
        parse_table_t pt;
        char *filename = NULL;
        int cmd_save = 0;

        if ((subcmd_s = ARG_GET(a)) == NULL) {
            return CMD_USAGE;
        }
        if (sal_strcasecmp(subcmd_s, "save") == 0) {
            cmd_save = 1;
        } else if (sal_strcasecmp(subcmd_s, "restore") == 0) {
            cmd_save = 0;
        } else {
            return CMD_USAGE;
        }

        parse_table_init(unit, &pt);
        parse_table_add(&pt, "FileName", PQ_STRING, "alpm_data.bin",
                                (void *)&filename, NULL);
        if (parse_arg_eq(a, &pt) < 0) {
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        if (cmd_save) {
            (void)_alpm_data_save(unit, filename);
        } else {
            (void)_alpm_data_restore(unit, filename);
        }
        parse_arg_eq_done(&pt);
        return CMD_OK;
    }

    if (!sal_strcasecmp(table, "table")) {
        if ((subcmd_s = ARG_GET(a)) == NULL) {
            return CMD_USAGE;
        }
        if (!sal_strcasecmp(subcmd_s, "dump")) {
            if ((subcmd_s = ARG_GET(a)) == NULL) {
                return CMD_USAGE;
            }
            for (;;) {
                if (subcmd_s != NULL && !sal_strcasecmp(subcmd_s, "raw")) {
                    flags |= ALPM_DUMP_TABLE_RAW;
                    subcmd_s = ARG_GET(a);
                } else if (subcmd_s != NULL && !sal_strcasecmp(subcmd_s, "all")) {
                    flags |= ALPM_DUMP_TABLE_ALL;
                    subcmd_s = ARG_GET(a);
                } else if (subcmd_s != NULL && !sal_strcasecmp(subcmd_s, "chg")) {
                    flags |= ALPM_DUMP_TABLE_CHANGED;
                    subcmd_s = ARG_GET(a);
                } else if (subcmd_s != NULL && !sal_strcasecmp(subcmd_s, "nocache")) {
                    flags |= ALPM_DUMP_TABLE_NOCACHE;
                    subcmd_s = ARG_GET(a);
                } else {
                    break;
                }
            } /* for */

            if (subcmd_s == NULL) {
                return CMD_USAGE;
            }

            if (parse_memory_name(unit, &mem, subcmd_s, &copyno, 0) >= 0) {
                if (SOC_MEM_IS_VALID(unit, L3_DEFIP_ALPM_LEVEL2m) &&
                    mem != L3_DEFIP_ALPM_LEVEL2m &&
                    mem != L3_DEFIP_ALPM_LEVEL3m &&
                    mem != L3_DEFIP_PAIR_LEVEL1m &&
                    mem != L3_DEFIP_LEVEL1m) {
                    return CMD_USAGE;
                }
                if (SOC_MEM_IS_VALID(unit, L3_DEFIP_ALPM_RAWm) &&
                    mem != L3_DEFIP_ALPM_RAWm) {
                    return CMD_USAGE;
                }

                if (copyno == COPYNO_ALL) {
                    copyno = SOC_MEM_BLOCK_ANY(unit, mem);
                }
            } else {
                return CMD_USAGE;
            }

            if ((subcmd_s = ARG_GET(a)) == NULL) {
                return CMD_USAGE;
            }

            if (mem == L3_DEFIP_LEVEL1m || mem == L3_DEFIP_PAIR_LEVEL1m) {
                index = parse_integer(subcmd_s);
            } else {
                index = parse_memory_index(unit, mem, subcmd_s);
            }

            if ((subcmd_s = ARG_GET(a)) == NULL || !isint(subcmd_s)) {
                return CMD_USAGE;
            }
            fmt = parse_integer(subcmd_s);
            if ((subcmd_s = ARG_GET(a)) == NULL) {
                ent_idx = -1; /* all entries in the bank */
            } else {
                if (!isint(subcmd_s)) {
                    return CMD_USAGE;
                }
                ent_idx = parse_integer(subcmd_s);
            }
            if ((subcmd_s = ARG_GET(a)) == NULL) {
                vrf_id = 1; /* all entries in the bank */
            } else {
                if (!isint(subcmd_s)) {
                    return CMD_USAGE;
                }
                vrf_id = parse_integer(subcmd_s);
            }
            if (soc_feature(unit, soc_feature_alpm2)) {
                (void)alpm_table_dump(unit, 0, vrf_id, mem, copyno, index,
                                      fmt, ent_idx, flags);
            } else {
                cli_out("cmd is not supported\n");
            }
            return CMD_OK;
        }
        return CMD_USAGE;
    }


    if (!sal_strcasecmp(table, "debug")) {
        if ((subcmd_s = ARG_GET(a)) == NULL) {
            return CMD_USAGE;
        }
        if (!sal_strcasecmp(subcmd_s, "info")) {
            if ((subcmd_s = ARG_GET(a)) == NULL) {
                return CMD_USAGE;
            }
            if (!sal_strcasecmp(subcmd_s, "on")) {
                if ((subcmd_s = ARG_GET(a)) == NULL) {
                    (void)alpm_dbg_info_bmp_show(unit);
                    return CMD_OK;
                }
                for (;;) {
                    if (subcmd_s != NULL && !sal_strcasecmp(subcmd_s, "pvt")) {
                        flags |= _ALPM_DBG_INFO_PVT;
                        subcmd_s = ARG_GET(a);
                    } else if (subcmd_s != NULL && !sal_strcasecmp(subcmd_s, "rte")) {
                        flags |= _ALPM_DBG_INFO_RTE;
                        subcmd_s = ARG_GET(a);
                    } else if (subcmd_s != NULL && !sal_strcasecmp(subcmd_s, "cnt")) {
                        flags |= _ALPM_DBG_INFO_CNT;
                        subcmd_s = ARG_GET(a);
                    } else if (subcmd_s != NULL && !sal_strcasecmp(subcmd_s, "hit")) {
                        flags |= _ALPM_DBG_INFO_HIT;
                        subcmd_s = ARG_GET(a);
                    } else if (subcmd_s != NULL && !sal_strcasecmp(subcmd_s, "all")) {
                        flags |= _ALPM_DBG_INFO_ALL;
                        subcmd_s = ARG_GET(a);
                    } else if (subcmd_s != NULL && !sal_strcasecmp(subcmd_s, "none")) {
                        flags = 0;
                        subcmd_s = ARG_GET(a);
                    } else {
                        break;
                    }
                } /* for */

                (void)alpm_dbg_info_bmp_set(unit, flags);
                return CMD_OK;
            }
        }
        if (!sal_strcasecmp(subcmd_s, "bmp")) {
            uint32 dbg_bmp = 0;
            if ((subcmd_s = ARG_GET(a)) == NULL) {
                return CMD_USAGE;
            }

            dbg_bmp = parse_integer(subcmd_s);
            (void)alpm_dbg_bmp_set(unit, dbg_bmp);
            return CMD_OK;
        }

        return CMD_USAGE;
    }

    if (sal_strcasecmp(table, "util") == 0) {
        int i;
        ip6_addr_t ip6;
        char buf[IP6ADDR_STR_LEN];
#define IP6_STR_CNT 6
        char *ip6_str[IP6_STR_CNT] = {
            "ffff::ffff:ffff:ffff:ffff:ffff:ffff",
            "eeee::eeee:eeee:eeee:eeee:eeee",
            "dddd:dddd::dddd",
            "cccc:cccc::",
            "bbbb::",
            "::aaaa"
        };

        for (i = 0; i < IP6_STR_CNT; i++) {
            if (ip6_str[i] != NULL) {
                parse_ip6addr(ip6_str[i], ip6);
                sal_memset(buf, 0, sizeof(buf));
                format_ip6addr(buf, ip6);
                cli_out(" Original: %s\n", ip6_str[i]);
                cli_out("Converted: %s\n", buf);
            }
        }
        return CMD_OK;
    }


    if (sal_strcasecmp(table, "load") == 0) {
        if ((subcmd_s = ARG_CUR(a)) == NULL) {
    	    return CMD_USAGE;
        }
        return CMD_OK;
    }

    if ((subcmd_s = ARG_GET(a)) == NULL) {
	    return CMD_USAGE;
    }

    if (sal_strcasecmp(subcmd_s, "show") == 0) {
	    subcmd = L3_SHOW;
    }
    if (sal_strcasecmp(subcmd_s, "add") == 0) {
	    subcmd = L3_ADD;
    }

    if ((subcmd == L3_SHOW) &&
        (sal_strcasecmp(table, "bkt") == 0 ||
         sal_strcasecmp(table, "pvt") == 0 ||
         sal_strcasecmp(table, "fmt") == 0 ||
         sal_strcasecmp(table, "pvtlen") == 0 ||
         sal_strcasecmp(table, "stat") == 0 ||
         sal_strcasecmp(table, "pvttrie") == 0 ||
         sal_strcasecmp(table, "bkttrie") == 0 ||
         sal_strcasecmp(table, "usage") == 0 ||
         sal_strcasecmp(table, "config") == 0
        )
       ) {
        subcmd_s = ARG_GET(a);
        if (subcmd_s != NULL) {
            acb_bmp = parse_integer(subcmd_s);
        } else {
            acb_bmp = 3;
        }
    }

    if (sal_strcasecmp(table, "bkt") == 0) {
        switch (subcmd) {
            /* alpm bkt show */
            case L3_SHOW:
                (void)alpm_cb_bkt_dump(unit, app, acb_bmp);
                return CMD_OK;
            default:
                return (CMD_USAGE);
        }

        return CMD_OK;
    }

    if (sal_strcasecmp(table, "pvt") == 0) {
        int bkt = -1;
        switch (subcmd) {
            /* alpm pvt show */
            case L3_SHOW:
                subcmd_s = ARG_GET(a);
                if (subcmd_s != NULL) {
                    bkt = parse_integer(subcmd_s);
                } else {
                    bkt = -1;
                }

                (void)alpm_cb_pvt_dump(unit, app, acb_bmp, bkt);
                return CMD_OK;
            default:
                return (CMD_USAGE);
        }

        return CMD_OK;
    }

    if (sal_strcasecmp(table, "fmt") == 0) {
        switch (subcmd) {
            /* alpm fmt show */
            case L3_SHOW:
                (void)alpm_cb_fmt_dump(unit, app, acb_bmp);
                return CMD_OK;
            default:
                return (CMD_USAGE);
        }

        return CMD_OK;
    }

    if (sal_strcasecmp(table, "pvtlen") == 0) {
        switch (subcmd) {
            /* alpm pvtlen show */
            case L3_SHOW:
                (void)alpm_cb_pvtlen_dump(unit, app, acb_bmp);
                return CMD_OK;
            default:
                return (CMD_USAGE);
        }

        return CMD_OK;
    }

    if (sal_strcasecmp(table, "stat") == 0) {
        switch (subcmd) {
            /* alpm stat show */
            case L3_SHOW:
                (void)alpm_cb_stat_dump(unit, app, acb_bmp);
                return CMD_OK;
            default:
                return (CMD_USAGE);
        }

        return CMD_OK;
    }

    if (sal_strcasecmp(table, "tcam") == 0) {
        switch (subcmd) {
            /* alpm tcam show */
            case L3_SHOW:
                bcm_esw_alpm_tcam_zone_state_dump(unit, app);
                return CMD_OK;
            default:
                return (CMD_USAGE);
        }

        return CMD_OK;
    }

    if (sal_strcasecmp(table, "config") == 0) {
        switch (subcmd) {
            /* alpm config show */
            case L3_SHOW:
                (void)alpm_cb_conf_dump(unit, app, acb_bmp);
                return CMD_OK;
            default:
                return (CMD_USAGE);
        }

        return CMD_OK;
    }

    if (sal_strcasecmp(table, "pvttrie") == 0) {
        bcm_vrf_t vrf = BCM_L3_VRF_ALL;
        bcm_l3_alpm_ipt_t ipt = bcmL3AlpmIptAll;
        switch (subcmd) {
            /* alpm pvt trie show */
            case L3_SHOW:
                subcmd_s = ARG_GET(a);
                if (subcmd_s != NULL) {
                    vrf = parse_integer(subcmd_s);
                }
                subcmd_s = ARG_GET(a);
                if (subcmd_s != NULL) {
                    ipt = parse_integer(subcmd_s);
                }

                (void)alpm_trie_show(unit, app, vrf, ipt, 0, acb_bmp);
                return CMD_OK;
            default:
                return (CMD_USAGE);
        }

        return CMD_OK;
    }

    if (sal_strcasecmp(table, "bkttrie") == 0) {
        bcm_vrf_t vrf = BCM_L3_VRF_ALL;
        bcm_l3_alpm_ipt_t ipt = bcmL3AlpmIptAll;
        switch (subcmd) {
            /* alpm pvt_bkt trie show */
            case L3_SHOW:
                subcmd_s = ARG_GET(a);
                if (subcmd_s != NULL) {
                    vrf = parse_integer(subcmd_s);
                }
                subcmd_s = ARG_GET(a);
                if (subcmd_s != NULL) {
                    ipt = parse_integer(subcmd_s);
                }

                (void)alpm_trie_show(unit, app, vrf, ipt, 1, acb_bmp);
                return CMD_OK;
            default:
                return (CMD_USAGE);
        }

        return CMD_OK;
    }

    if (sal_strcasecmp(table, "usage") == 0) {
        switch (subcmd) {
            /* alpm resource usage log show */
            case L3_SHOW:
                if (acb_bmp == -1) {
                    (void)alpm_resource_usage_dump(unit);
                } else {
                    (void)alpm_resource_usage_show(unit);
                }
                return CMD_OK;
            default:
                return (CMD_USAGE);
        }

        return CMD_OK;
    }
    if (sal_strcasecmp(table, "projection") == 0) {
        int v4v6_ratio = 0; /* default to uni-dimensional */
        switch (subcmd) {
            /* alpm route capacity projection show */
            case L3_SHOW:
                subcmd_s = ARG_GET(a);
                if (subcmd_s != NULL) {
                    v4v6_ratio = parse_integer(subcmd_s); /* v4v6_ratio x100 */
                }

                if (SOC_IS_TOMAHAWK3(unit)) {
                    (void)alpm_route_projection(unit, v4v6_ratio);
                } else {
                    cli_out("Currently unavailable for this chip\n");
                }
                return CMD_OK;
            default:
                return (CMD_USAGE);
        }

        return CMD_OK;
    }

#endif
    return CMD_USAGE;
}
#endif /* BCM_TRIDENT3_SUPPORT or BCM_TOMAHAWK_SUPPORT */

cmd_result_t
if_l3(int unit, args_t *a)
{
    char           *table, *subcmd_s;
    int             subcmd = 0;
    parse_table_t   pt;
    cmd_result_t    retCode;
    bcm_mac_t      mac;
    int             id = 0;
    int             r, first_entry, last_entry; 
    int             num_entries = 0;
    int             free_l3, free_defip;
#if defined(BCM_XGS_SWITCH12_SUPPORT)
    int             free_lpm_blk;
#endif
    bcm_l3_info_t   l3_hw_status;
    char            ip_str[IP6ADDR_STR_LEN + 3];
    bcm_ip_t       ip_addr = 0, mask = 0;
#ifdef BCM_XGS_SWITCH_SUPPORT
    int             interface = 0, port = 0;
    int		    module = 0;
    bcm_vlan_t      vlan = 0;
    bcm_ip_t	    gateway = 0;
    int             sip_key = 2, hash_select = XGS_HASH_COUNT;
    uint8           key[SOC_MAX_MEM_WORDS * 4];
    int             ecmp = 0, max_ecmp = 0;
#endif
    uint32          flags;
    bcm_l3_route_t  route_info;
    bcm_l3_host_t   ip_info;
#if defined(BCM_XGS3_SWITCH_SUPPORT)
    bcm_ip6_t       ip6_addr, sip6_addr;
    int             urpf = 0, vrf = 0;
    int egress_flag = FALSE;
#endif
#if defined(BCM_FIREBOLT_SUPPORT)
    int i;
#endif  /* BCM_FIREBOLT_SUPPORT */
    soc_mem_t uc_mem = L3_ENTRY_IPV4_UNICASTm;

#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    int ext_tcam_flag = 0;
    int defip_128_max = 0;    
    int defip_128_free = 0;
    int defip_64_max = 0;
    int defip_64_free  = 0;
#endif
    void *user_data = NULL;

    sal_memset(mac, 0, sizeof(bcm_mac_t));

    /* Check valid device to operation on ... */
    if (!sh_check_attached(ARG_CMD(a), unit)) {
	return CMD_FAIL;
    }

    if ((table = ARG_GET(a)) == NULL) {
	return CMD_USAGE;
    }

    /* L3 init */
    if (sal_strcasecmp(table, "init") == 0) {
	if ((r = bcm_l3_init(unit)) < 0) {
	    cli_out("%s: error initializing: %s\n", ARG_CMD(a), bcm_errmsg(r));
	}
	return CMD_OK;
    }

    /* L3 detach */
    if (sal_strcasecmp(table, "detach") == 0) {
	if ((r = bcm_l3_cleanup(unit)) < 0) {
	    cli_out("%s: error detaching: %s\n", ARG_CMD(a), bcm_errmsg(r));
	}
	return CMD_OK;
    }

    /* L3 info */
    if (sal_strcasecmp(table, "info") == 0) {
        return _l3_cmd_info_show(unit);
    }

    if (sal_strcasecmp(table, "defipinfo") == 0) {
#if defined(BCM_KATANA_SUPPORT)
        if (SOC_IS_KATANAX(unit)) {
            bcm_l3_info_t l3_info;
            bcm_l3_info_t_init(&l3_info);
            if ((r = bcm_l3_info(unit, &l3_info)) < 0) {
                cli_out("%s: Error L3 info: %s\n", ARG_CMD(a), bcm_errmsg(r));
                return CMD_FAIL;
            }
            _bcm_kt_l3_info_dump(unit);
        }
#endif
        return CMD_OK;
    }

    if ((subcmd_s = ARG_GET(a)) == NULL) {
	return CMD_USAGE;
    }
#if defined(BCM_XGS3_SWITCH_SUPPORT)
    if ((sal_strcasecmp(table, "ecmp") == 0) &&
        (sal_strcasecmp(subcmd_s, "egress") == 0)) {
        egress_flag = TRUE;
        if ((subcmd_s = ARG_GET(a)) == NULL) {
            return CMD_USAGE;
        }
    }
#endif
    if (sal_strcasecmp(subcmd_s, "add") == 0) {
	subcmd = L3_ADD;
    }
    if (sal_strcasecmp(subcmd_s, "update") == 0) {
	subcmd = L3_UPDATE;
    }
    if (sal_strcasecmp(subcmd_s, "destroy") == 0 ||
        sal_strcasecmp(subcmd_s, "delete") == 0) {
	subcmd = L3_DESTROY;
    }
    if (sal_strcasecmp(subcmd_s, "set") == 0) {
	subcmd = L3_SET;
    }
    if (sal_strcasecmp(subcmd_s, "clear") == 0) {
	subcmd = L3_CLEAR;
    }
    if (sal_strcasecmp(subcmd_s, "show") == 0) {
	subcmd = L3_SHOW;
    }
    if (sal_strcasecmp(subcmd_s, "age") == 0) {
	subcmd = L3_AGE;
    }
#ifdef BCM_XGS_SWITCH_SUPPORT
    if (sal_strcasecmp(subcmd_s, "conflict") == 0) {
	subcmd = L3_CONFLICT;
    }
    if (sal_strcasecmp(subcmd_s, "hash") == 0) {
	subcmd = L3_HASH;
    }
    if (sal_strcasecmp(subcmd_s, "ip6hash") == 0) {
	subcmd = L3_IP6HASH;
    }
    if (sal_strcasecmp(subcmd_s, "sanity") == 0) {
	subcmd = L3_SANITY;
    }
    if (sal_strcasecmp(subcmd_s, "ecmp") == 0) {
	subcmd = L3_ECMP;
    }
    if (sal_strcasecmp(subcmd_s, "ecmphash") == 0) {
	subcmd = L3_ECMP_HASH;
    }
    if (sal_strcasecmp(subcmd_s, "maxadd") == 0) {
	subcmd = L3_MAXADD;
    }
#endif
    if (sal_strcasecmp(subcmd_s, "insert") == 0) {
	subcmd = L3_INSERT;
    }
    if (sal_strcasecmp(subcmd_s, "remove") == 0) {
	subcmd = L3_REMOVE;
    }
    if (sal_strcasecmp(subcmd_s, "find") == 0) {
	subcmd = L3_FIND;
    }
    if (sal_strcasecmp(subcmd_s, "test") == 0) {
	subcmd = L3_TEST;
    }
    if (!subcmd) {
	return CMD_USAGE;
    }

    if (sal_strcasecmp(table, "intf") == 0) {
        switch (subcmd) {
            /* l3 intf add */
          case L3_ADD:
              return _l3_cmd_egress_intf_create(unit, a); 
              /* l3 intf destroy */
          case L3_DESTROY:
              return  _l3_cmd_egress_intf_delete(unit, a);
              /* l3 intf clear */
          case L3_CLEAR:
              return _l3_cmd_egress_intf_delete_all(unit, a); 
              /* l3 intf show */
          case L3_SHOW:
              return _l3_cmd_egress_intf_show(unit, a);
          default:
              return (CMD_USAGE);
        }
    }

    if (sal_strcasecmp(table, "egress") == 0) {
#if defined(BCM_XGS3_SWITCH_SUPPORT)
        switch (subcmd) {
            /* l3 egress add/update */
          case L3_ADD:
          case L3_UPDATE:
              return _l3_cmd_egress_object_create(unit, a);
              /* l3 egress destroy */
          case L3_DESTROY:
              return _l3_cmd_egress_object_destroy(unit, a);
              /* l3 egress show */
          case L3_SHOW:
              return _l3_cmd_egress_object_show(unit, a); 
              /* l3 egress find */
          case L3_FIND:
              return _l3_cmd_egress_object_find(unit, a); 
          default:
              return (CMD_USAGE);
        }
    /*    coverity[unreachable]    */
#endif /* BCM_XGS3_SWITCH_SUPPORT */
    /*    coverity[unreachable]    */
        cli_out("Unsupported command. \n");
        return (CMD_OK);
    }
    if (sal_strcasecmp(table, "prefix_map") == 0) {
#if defined(BCM_XGS3_SWITCH_SUPPORT)
        switch (subcmd) {
            /* l3 prefix_map add */
          case L3_ADD:
              return _l3_cmd_prefix_map_add(unit, a);
              /* l3 prefix_map delete */
          case L3_DESTROY:
              return _l3_cmd_prefix_map_delete(unit, a);
              /* l3 prefix_map show */
          case L3_SHOW:
              return _l3_cmd_prefix_map_show(unit, a);
              /* l3 egress clear */
          case L3_CLEAR: 
              return _l3_cmd_prefix_map_delete_all(unit, a);
          default:
              cli_out("Unsupported command. \n");
              return CMD_OK;
    /*    coverity[unreachable]    */
        }
    /** coverity[unreachable]    **/
        return CMD_OK;
    /*    coverity[unreachable]    */
#else
        /* coverity[unreachable]    */
        cli_out("Unsupported command. \n");
        return CMD_OK;
#endif /* BCM_XGS3_SWITCH_SUPPORT */
    }
    if (sal_strcasecmp(table, "multipath") == 0) {
#if defined(BCM_XGS3_SWITCH_SUPPORT)
        switch (subcmd) {
            /* l3 multipath add */
          case L3_ADD:
          case L3_UPDATE:
              return _l3_cmd_egress_mpath_object_create(unit, a); 
              /* l3 multipath max add */
          case L3_MAXADD:
              return _l3_cmd_egress_mpath_object_max_create(unit, a); 
              /* l3 multipath destroy */
          case L3_DESTROY:
              return _l3_cmd_egress_mpath_object_destroy(unit, a); 
              /* l3 multipath show */
          case L3_SHOW:
              return _l3_cmd_egress_mpath_object_show(unit, a); 
          case L3_INSERT:
              return _l3_cmd_egress_mpath_object_intf_insert(unit, a);
          case L3_REMOVE:
              return _l3_cmd_egress_mpath_object_intf_remove(unit, a);
          case L3_FIND:
    /** coverity[unreachable]    **/
              return _l3_cmd_egress_mpath_object_lookup(unit, a);
    /** coverity[unreachable]    **/
          default:
              cli_out("Unsupported command. \n");
    /** coverity[unreachable]    **/
              return CMD_OK;
    /** coverity[unreachable]    **/
         }
    /** coverity[unreachable]    **/
         return CMD_OK;
#else
        /* coverity[unreachable] */
        cli_out("Unsupported command. \n");
        return CMD_OK;
#endif /* BCM_XGS3_SWITCH_SUPPORT */
    }
    if (sal_strcasecmp(table, "ecmp") == 0) {
#if defined(BCM_XGS3_SWITCH_SUPPORT)
        if (!egress_flag) {
            cli_out("Unsupported command. \n");
            return CMD_OK;
        }
        switch (subcmd) {
          /* l3 ecmp add */
          case L3_ADD:
          case L3_UPDATE:
              return _l3_cmd_egress_ecmp_object_create(unit, a);
          /* l3 ecmp max add */
          case L3_MAXADD:
              return _l3_cmd_egress_ecmp_object_max_create(unit, a);
          /* l3 ecmp destroy */
          case L3_DESTROY:
              return _l3_cmd_egress_ecmp_object_destroy(unit, a);
          /* l3 ecmp show */
          case L3_SHOW:
              return _l3_cmd_egress_ecmp_object_show(unit, a);
          case L3_INSERT:
              return _l3_cmd_egress_ecmp_object_intf_insert(unit, a);
          case L3_REMOVE:
              return _l3_cmd_egress_ecmp_object_intf_remove(unit, a);
          case L3_FIND:
              /** coverity[unreachable]    **/
              return _l3_cmd_egress_ecmp_object_lookup(unit, a);
          /** coverity[unreachable]    **/
          default:
              cli_out("Unsupported command. \n");
              /** coverity[unreachable]    **/
              return CMD_OK;
         /** coverity[unreachable]    **/
         }
         /** coverity[unreachable]    **/
         return CMD_OK;
#else
        /* coverity[unreachable] */
        cli_out("Unsupported command. \n");
        return CMD_OK;
#endif /* BCM_XGS3_SWITCH_SUPPORT */
    }

    if (sal_strcasecmp(table, "l3table") == 0) {
        switch (subcmd) {

            /* l3 l3table add */
          case L3_ADD:
              return _l3_cmd_host_add(unit, a, FALSE);
              /* l3 l3table destroy */
          case L3_DESTROY:
              parse_table_init(unit, &pt);
#if defined(BCM_XGS3_SWITCH_SUPPORT)
              parse_table_add(&pt, "VRF", PQ_DFL | PQ_INT, 0, (void *)&vrf, 0);
#endif
              parse_table_add(&pt, "IP", PQ_DFL | PQ_IP, 0, (void *)&ip_addr, 0);
              if (!parseEndOk(a, &pt, &retCode))
                  return retCode;

              bcm_l3_host_t_init(&ip_info);
#if defined(BCM_XGS3_SWITCH_SUPPORT)
              ip_info.l3a_vrf     = vrf;
#endif
              ip_info.l3a_ip_addr = ip_addr;

              if ((r = bcm_l3_host_delete(unit, &ip_info)) < 0) {
                  format_ipaddr(ip_str, ip_addr);
                  cli_out("%s: Error destroy %s from L3 "
                          "table: %s\n", ARG_CMD(a), ip_str, bcm_errmsg(r));
                  return CMD_FAIL;
              }
              break;

              /* l3 l3table clear */
          case L3_CLEAR:
              bcm_l3_host_delete_all(unit, NULL);
              break;

              /* l3 l3table show */
          case L3_SHOW:
              if ((r = bcm_l3_info(unit, &l3_hw_status)) < 0) {
                  cli_out("%s: Error L3 accessing: %s\n", ARG_CMD(a), bcm_errmsg(r));
                  return CMD_FAIL;
              }
              free_l3 = l3_hw_status.l3info_max_host -
                  l3_hw_status.l3info_occupied_host;
              last_entry = l3_hw_status.l3info_max_host;
              if (ARG_CNT(a)) {
                  if (!isint(ARG_CUR(a))) {
                      cli_out("%s: Invalid number: %s\n", ARG_CMD(a), ARG_CUR(a));
                      return (CMD_FAIL);
                  }
                  first_entry = parse_integer(ARG_GET(a));
                  if (first_entry > last_entry) {
                      cli_out("%s: Error: Illegal entry number (0 to %d)\n",
                              ARG_CMD(a), last_entry);
                      return (CMD_FAIL);
                  }
                  last_entry = first_entry;
              } else {
                  first_entry = 0;
              }

              cli_out("Unit %d, free L3 table entries: %d\n", unit, free_l3);
              cli_out("Entry VRF IP address       Mac Address           "
                      "INTF MOD PORT    CLASS HIT    H/W Index\n");
              user_data = soc_cm_salloc(unit, sizeof(int), "l3host hw index");
              if (NULL == user_data) {
                  cli_out("Failed to allocate memory\n");
                  return CMD_FAIL;
              }
              bcm_l3_host_traverse(unit, _BCM_L3_SHOW_INDEX, first_entry,
                                   last_entry, _l3_cmd_host_print, user_data);
              if (user_data) {
                  soc_cm_sfree(unit, user_data);
              }
              return CMD_OK;

              /* l3 l3table age */
          case L3_AGE:
              flags = BCM_L3_S_HIT;

              bcm_l3_host_age(unit, flags, _l3_cmd_host_age_callback, NULL);
              return CMD_OK;

#ifdef BCM_XGS_SWITCH_SUPPORT
          case L3_HASH:

              parse_table_init(unit, &pt);
#if defined(BCM_TRX_SUPPORT)
              if (SOC_IS_TRX(unit)) {
                  parse_table_add(&pt, "VRF", PQ_DFL|PQ_INT, 0,
                                  (void *)&vrf, 0);
              }
#endif
              sal_memset(&ip_addr, 0, sizeof(ip_addr));
              sal_memset(&gateway, 0, sizeof(gateway));
              parse_table_add(&pt, "IP", PQ_DFL | PQ_IP, 0, (void *)&ip_addr, 0);
              parse_table_add(&pt, "SrcIP", PQ_DFL | PQ_IP, 0,
                              (void *)&gateway, 0);
              parse_table_add(&pt, "VID", PQ_DFL | PQ_INT, 0,
                              (void *)&id, 0);
              parse_table_add(&pt, "Hash", PQ_DFL | PQ_INT, 0,
                              (void *)&hash_select, 0);
              parse_table_add(&pt, "SIPKey", PQ_DFL | PQ_INT, 0,
                              (void *)&sip_key, 0);
              if (!parseEndOk(a, &pt, &retCode))
                  return retCode;
              vlan = id;

#ifdef BCM_ISM_SUPPORT
              if (soc_feature(unit, soc_feature_ism_memory)) {
                  int b, rc, index, key_nbits, loop;
                  l3_entry_1_entry_t l3_ipv4_uentry;
                  l3_entry_2_entry_t l3_ipv4_mentry;
                  soc_field_t keyflds[MAX_FIELDS], lsbfld;
                  uint32 bucket, sizes[_SOC_ISM_MAX_BANKS];
                  uint8 act_zero_lsb, act_offset, zero_lsb = 0, offset = 48;
                  uint8 banks[_SOC_ISM_MAX_BANKS], bcount, num_flds;
                  soc_mem_t l3_entry_mem;
                  int mcast_flag =0;
                  void *entry;

                  if (L3X_IP_MULTICAST(ip_addr)) {
                      mcast_flag = 1;
                      l3_entry_mem = L3_ENTRY_2m;
                      entry = &l3_ipv4_mentry;
                      sal_memset(&l3_ipv4_mentry, 0, sizeof(l3_ipv4_mentry)); 
                      soc_L3_ENTRY_2m_field32_set(unit, &l3_ipv4_mentry, 
                              KEY_TYPE_0f, SOC_MEM_KEY_L3_ENTRY_2_IPV4MC_IPV4_MULTICAST);
                      soc_L3_ENTRY_2m_field32_set(unit, &l3_ipv4_mentry, 
                              KEY_TYPE_1f, SOC_MEM_KEY_L3_ENTRY_2_IPV4MC_IPV4_MULTICAST);
                      soc_L3_ENTRY_2m_field32_set(unit, &l3_ipv4_mentry,
                              VALID_0f, 1);
                      soc_L3_ENTRY_2m_field32_set(unit, &l3_ipv4_mentry,
                              IPV4MC__GROUP_IP_ADDRf, ip_addr);
                      soc_L3_ENTRY_2m_field32_set(unit, &l3_ipv4_mentry,
                              IPV4MC__SOURCE_IP_ADDRf, gateway);
                      soc_L3_ENTRY_2m_field32_set(unit, &l3_ipv4_mentry,
                              VRF_IDf, vrf);
                      soc_L3_ENTRY_2m_field32_set(unit, &l3_ipv4_mentry,
                              IPV4MC__VLAN_IDf, vlan);
                  } else {
                      l3_entry_mem = L3_ENTRY_1m;
                      entry = &l3_ipv4_uentry;
                      sal_memset(&l3_ipv4_uentry, 0, sizeof(l3_ipv4_uentry)); 
                      soc_L3_ENTRY_1m_field32_set(unit, &l3_ipv4_uentry, VALIDf, 1);
                      soc_L3_ENTRY_1m_field32_set(unit, &l3_ipv4_uentry,
                              IP_ADDRf, ip_addr);
                      soc_L3_ENTRY_1m_field32_set(unit, &l3_ipv4_uentry,
                              VRF_IDf, vrf);
                      soc_L3_ENTRY_1m_field32_set(unit, &l3_ipv4_uentry,
                              KEY_TYPEf, SOC_MEM_KEY_L3_ENTRY_1_IPV4UC_IPV4_UNICAST);
                  }

                  rc = soc_ism_get_banks_for_mem(unit, l3_entry_mem, banks, sizes,
                          &bcount);
                  if (rc || !bcount) {
                      cli_out("Error retreiving bank info: Test aborted.\n");
                      return CMD_FAIL;
                  }
                  rc = soc_generic_get_hash_key(unit, l3_entry_mem, entry,
                          keyflds, &lsbfld, &num_flds);
                  if (rc) {
                      cli_out("Error constructing key: Test aborted.\n");
                      return CMD_FAIL;
                  }
                  sal_memset(key, 0, sizeof(key));
                  soc_ism_gen_key_from_keyfields(unit, l3_entry_mem, 
                          entry, keyflds, key, num_flds);

                  key_nbits = soc_ism_mem_max_key_bits_get(unit, l3_entry_mem);
                  cli_out("Hash(%s)[%d] of key 0x",
                          (mcast_flag) ? "Ipv4MC" : "IPv4UC", hash_select);
                  for (loop = BITS2BYTES(key_nbits); loop > 0; loop--) {
                      cli_out("%02x", key[loop - 1]);
                  }
                  cli_out("\n");
                  for (b = 0; b < bcount; b++) {
                      rc = soc_ism_mem_hash_config_get(unit, l3_entry_mem, &act_zero_lsb);
                      if (rc) {
                          cli_out("Error retreiving hash config: Test aborted.\n");
                          return CMD_FAIL;
                      }
                      rc = soc_ism_hash_offset_config_get(unit, banks[b], &act_offset);
                      if (rc) {
                          cli_out("Error retreiving hash offset: Test aborted.\n");
                          return CMD_FAIL;
                      }
                      switch(hash_select) {
                          case XGS_HASH_CRC16_UPPER: 
                              offset = 48 - soc_ism_get_hash_bits(sizes[b]);
                              break;
                          case XGS_HASH_CRC16_LOWER:
                              offset = 32;
                              break;
                          case XGS_HASH_LSB:
                              offset = 48; zero_lsb = 1;
                              break;
                          case XGS_HASH_ZERO:
                              break;
                          case XGS_HASH_CRC32_UPPER: 
                              offset = 32 - soc_ism_get_hash_bits(sizes[b]);
                              break;
                          case XGS_HASH_CRC32_LOWER: 
                              offset = 0;
                              break;
                          case XGS_HASH_COUNT: 
                              offset = act_offset; zero_lsb = act_zero_lsb;
                              break;
                          default: 
                              if (hash_select > 63) {
                                  cli_out("Invalid hash offset value !!\n");
                                  return CMD_FAIL;
                              }
                              offset = hash_select; if (hash_select >= 48) { zero_lsb = 1; }
                              break;
                      }
                      /* get config */
                      if (zero_lsb != act_zero_lsb) {
                          rc = soc_ism_mem_hash_config(unit, l3_entry_mem, zero_lsb);
                          if (rc) {
                              cli_out("Error setting hash config: Test aborted.\n");
                              return CMD_FAIL;
                          }
                      }
                      if (offset != act_offset) {
                          rc = soc_ism_hash_offset_config(unit, banks[b], offset);
                          if (rc) {
                              cli_out("Error setting hash offset: Test aborted.\n");
                              return CMD_FAIL;
                          }
                      }
                      rc = soc_generic_hash(unit, l3_entry_mem, entry, 1 << banks[b], 0,
                              &index, NULL, &bucket, NULL);
                      if (rc) {
                          cli_out("Error calculating hash: Test aborted.\n");
                          return CMD_FAIL;
                      }
                      cli_out("\tBank[%d] num: %d, bucket: %d, index: %d\n", b, 
                              banks[b], bucket, index);
                      /* restore config */
                      if (zero_lsb != act_zero_lsb) {
                          rc = soc_ism_mem_hash_config(unit, l3_entry_mem, act_zero_lsb);
                          if (rc) {
                              cli_out("Error setting hash config: Test aborted.\n");
                              return CMD_FAIL;
                          }
                      }
                      if (offset != act_offset) {
                          rc = soc_ism_hash_offset_config(unit, banks[b], act_offset);
                          if (rc) {
                              cli_out("Error setting hash offset: Test aborted.\n");
                              return CMD_FAIL;
                          }
                      }
                  }
                  return CMD_OK;
              }
#endif /* BCM_ISM_SUPPORT */

#ifdef BCM_TRX_SUPPORT
              if (SOC_MEM_FIELD_VALID(unit, L3_ENTRY_ONLYm, KEY_TYPEf)) {

                  int mcast_flag = 0;
                  int hash_bucket;
                  int key_nbits;
                  l3_entry_ipv4_multicast_entry_t  l3x_ip4_mentry;
                  l3_entry_ipv4_unicast_entry_t    l3x_ip4_uentry;
                  soc_field_t key_type_field;
                  int key_type_value;

                  if (hash_select == FB_HASH_COUNT) {
                      /* Get the hash selection from hardware */
                      uint32          regval;
                      int             rv;
                      if ((rv = READ_HASH_CONTROLr(unit, &regval)) < 0) {
                          cli_out("%s: ERROR: %s\n", ARG_CMD(a), soc_errmsg(rv));
                          return CMD_FAIL;
                      }
                      hash_select = soc_reg_field_get(unit, HASH_CONTROLr, regval,
                                                      L3_HASH_SELECTf);
                  }

                  if (L3X_IP_MULTICAST(ip_addr)) {
                      /* Given IP address is multicast IP address */
                      mcast_flag = 1;
                      key_type_field = (SOC_MEM_FIELD_VALID(unit, 
                                        L3_ENTRY_IPV4_MULTICASTm, KEY_TYPEf))?
                                        KEY_TYPEf : KEY_TYPE_0f;
#if defined(BCM_TRIDENT2_SUPPORT)
                      if (SOC_IS_TRIDENT2X(unit)) {
                          key_type_value = TD2_L3_HASH_KEY_TYPE_V4MC;
                      } else
#endif /* BCM_TRIDENT2_SUPPORT */
                      {
                          key_type_value = TR_L3_HASH_KEY_TYPE_V4MC;
                      }

                      sal_memset(&l3x_ip4_mentry, 0,
                                 sizeof(l3_entry_ipv4_multicast_entry_t));

                           
                      soc_mem_field32_set(unit, L3_ENTRY_IPV4_MULTICASTm,
                                          (uint32 *) &l3x_ip4_mentry, 
                                          key_type_field, key_type_value);

                      soc_mem_field32_set(unit, L3_ENTRY_IPV4_MULTICASTm,
                                          (uint32 *) &l3x_ip4_mentry,
                                          GROUP_IP_ADDRf, ip_addr);

                      soc_mem_field32_set(unit, L3_ENTRY_IPV4_MULTICASTm,
                                          (uint32 *) &l3x_ip4_mentry,
                                          SOURCE_IP_ADDRf, gateway);

                      soc_mem_field32_set(unit, L3_ENTRY_IPV4_MULTICASTm,
                                          (uint32 *) &l3x_ip4_mentry, VLAN_IDf,
                                          vlan);

                      /* prevent the case once VRF_ID field is not existed */
                      if (soc_mem_field_valid(unit, L3_ENTRY_IPV4_MULTICASTm, VRF_IDf)) {
                          soc_mem_field32_set(unit, L3_ENTRY_IPV4_MULTICASTm,
                                              (uint32 *) &l3x_ip4_mentry, 
                                              VRF_IDf, vrf);
                      }
                      /* coverity[overrun-buffer-val : FALSE]    */
                      key_nbits = soc_fb_l3x_base_entry_to_key(unit,
                                                               (uint32 *) &l3x_ip4_mentry, key);
                  } else {
                      /* Given IP address is Unicast IP address */
#if defined(BCM_TRIDENT2_SUPPORT)
                      if (SOC_IS_TRIDENT2X(unit)) {
                          key_type_value = TD2_L3_HASH_KEY_TYPE_V4UC;
                      } else
#endif /* BCM_TRIDENT2_SUPPORT */
                      {
                          key_type_value = TR_L3_HASH_KEY_TYPE_V4UC;
                      }
                      sal_memset(&l3x_ip4_uentry, 0,
                                 sizeof(l3_entry_ipv4_unicast_entry_t));

                      soc_mem_field32_set(unit, L3_ENTRY_IPV4_UNICASTm,
                                          (uint32 *) &l3x_ip4_mentry, KEY_TYPEf, 
                                          key_type_value);

                      soc_mem_field32_set(unit, L3_ENTRY_IPV4_UNICASTm,
                                          (uint32 *) &l3x_ip4_uentry,
                                          IP_ADDRf, ip_addr);

                      /* prevent the case once VRF_ID field is not existed */
                      if (soc_mem_field_valid(unit, L3_ENTRY_IPV4_UNICASTm, VRF_IDf)) {
                          soc_mem_field32_set(unit, L3_ENTRY_IPV4_UNICASTm,
                                              (uint32 *) &l3x_ip4_uentry, 
                                              VRF_IDf, vrf);
                      }
                      /* coverity[overrun-buffer-val : FALSE]    */
                      key_nbits = soc_fb_l3x_base_entry_to_key(unit,
                                                               (uint32 *) &l3x_ip4_uentry,
                                                               key);
                  }

                  hash_bucket = soc_fb_l3_hash(unit, hash_select, key_nbits, key);

                  if (mcast_flag) {
                      cli_out("Hash(IPMC)[%d] of key "
                              "0x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x "
                              "is bucket 0x%03x (%d)\n", hash_select,
                              key[9], key[8], key[7], key[6], key[5],
                              key[4], key[3], key[2], key[1], key[0],
                              hash_bucket, hash_bucket);
                  } else {
                      cli_out("Hash(UC)[%d] of key "
                              "0x%02x%02x%02x%02x "
                              "is bucket 0x%03x (%d)\n", hash_select,
                              key[3], key[2], key[1], key[0],
                              hash_bucket, hash_bucket);
                  }
                  break;
              }
#endif /* end of BCM_TRX_SUPPORT*/
#ifdef BCM_FIREBOLT_SUPPORT
              if (SOC_IS_FBX(unit)) {

                  int mcast_flag = 0;
                  int hash_bucket;
                  int key_nbits;
                  l3_entry_ipv4_multicast_entry_t  l3x_ip4_mentry;
                  l3_entry_ipv4_unicast_entry_t    l3x_ip4_uentry;

                  if (hash_select == FB_HASH_COUNT) {
                      /* Get the hash selection from hardware */
                      uint32          regval;
                      int             rv;
                      if ((rv = READ_HASH_CONTROLr(unit, &regval)) < 0) {
                          cli_out("%s: ERROR: %s\n", ARG_CMD(a), soc_errmsg(rv));
                          return CMD_FAIL;
                      }
                      hash_select = soc_reg_field_get(unit, HASH_CONTROLr, regval,
                                                      L3_HASH_SELECTf);
                  }

                  if (L3X_IP_MULTICAST(ip_addr)) {
                      /* Given IP address is multicast IP address */
                      mcast_flag = 1;
                      sal_memset(&l3x_ip4_mentry, 0,
                                 sizeof(l3_entry_ipv4_multicast_entry_t));

                      soc_mem_field32_set(unit, L3_ENTRY_IPV4_MULTICASTm,
                                          (uint32 *) &l3x_ip4_mentry, IPMCf, 1);

                      soc_mem_field32_set(unit, L3_ENTRY_IPV4_MULTICASTm,
                                          (uint32 *) &l3x_ip4_mentry,
                                          GROUP_IP_ADDRf, ip_addr);

                      soc_mem_field32_set(unit, L3_ENTRY_IPV4_MULTICASTm,
                                          (uint32 *) &l3x_ip4_mentry,
                                          SOURCE_IP_ADDRf, gateway);

                      soc_mem_field32_set(unit, L3_ENTRY_IPV4_MULTICASTm,
                                          (uint32 *) &l3x_ip4_mentry, VLAN_IDf,
                                          vlan);
                      /* coverity[overrun-buffer-val : FALSE]    */
                      key_nbits = soc_fb_l3x_base_entry_to_key(unit,
                                                               (uint32 *) &l3x_ip4_mentry, key);
                  } else {
                      /* Given IP address is Unicast IP address */
                      sal_memset(&l3x_ip4_uentry, 0,
                                 sizeof(l3_entry_ipv4_unicast_entry_t));

#ifdef BCM_TRIDENT3_SUPPORT
                      if (SOC_IS_TRIDENT3X(unit)) {
                          uc_mem = L3_ENTRY_SINGLEm;
                      }
#endif
                      if (L3X_IP_MULTICAST(ip_addr)) {
                          soc_mem_field32_set(unit, uc_mem,
                                              (uint32 *) &l3x_ip4_uentry, IPMCf, 1);
                      }

                      soc_mem_field32_set(unit, uc_mem,
                                          (uint32 *) &l3x_ip4_uentry,
                                          IP_ADDRf, ip_addr);
                      /* coverity[overrun-buffer-val : FALSE]    */
                      key_nbits = soc_fb_l3x_base_entry_to_key(unit,
                                                               (uint32 *) &l3x_ip4_uentry,
                                                               key);
                  }

                  hash_bucket = soc_fb_l3_hash(unit, hash_select, key_nbits, key);

                  if (mcast_flag) {
                      cli_out("Hash(IPMC)[%d] of key "
                              "0x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x "
                              "is bucket 0x%03x (%d)\n", hash_select,
                              key[9], key[8], key[7], key[6], key[5],
                              key[4], key[3], key[2], key[1], key[0],
                              hash_bucket, hash_bucket);
                  } else {
                      cli_out("Hash(UC)[%d] of key "
                              "0x%02x%02x%02x%02x "
                              "is bucket 0x%03x (%d)\n", hash_select,
                              key[3], key[2], key[1], key[0],
                              hash_bucket, hash_bucket);
                  }
              }
#endif /* end of BCM_FIREBOLT_SUPPORT */
              break;

              /* l3 l3table ip6hash */
          case L3_IP6HASH:
              if (!SOC_IS_FBX(unit)) {
                  cli_out("Command valid only for FB and ER switches\n");
                  return CMD_FAIL;
              }
#ifdef BCM_XGS3_SWITCH_SUPPORT
              parse_table_init(unit, &pt);
#if defined(BCM_TRX_SUPPORT)
              if (SOC_IS_TRX(unit)) {
                  parse_table_add(&pt, "VRF", PQ_DFL|PQ_INT, 0,
                                  (void *)&vrf, 0);
              }
#endif
              sal_memset(&ip6_addr, 0, sizeof(ip6_addr));
              sal_memset(&sip6_addr, 0, sizeof(sip6_addr));
              parse_table_add(&pt, "IP", PQ_DFL | PQ_IP6, 0,
                              (void *)&ip6_addr, 0);
              parse_table_add(&pt, "SrcIP", PQ_DFL | PQ_IP6, 0,
                              (void *)&sip6_addr, 0);
              parse_table_add(&pt, "VID", PQ_DFL | PQ_INT, 0,
                              (void *)&id, 0);
              parse_table_add(&pt, "Hash", PQ_DFL | PQ_INT, 0,
                              (void *)&hash_select, 0);
              parse_table_add(&pt, "SIPKey", PQ_DFL | PQ_INT, 0,
                              (void *)&sip_key, 0);
              if (!parseEndOk(a, &pt, &retCode)) {
                  return retCode;
              }
              vlan = id;

#ifdef BCM_ISM_SUPPORT
              if (soc_feature(unit, soc_feature_ism_memory)) {
                  int b, rc, index, loop, key_nbits;
                  l3_entry_2_entry_t l3_ipv6_uentry;
                  l3_entry_4_entry_t l3_ipv6_mentry;
                  soc_field_t keyflds[MAX_FIELDS], lsbfld;
                  uint32 bucket, sizes[_SOC_ISM_MAX_BANKS];
                  uint8 act_zero_lsb, act_offset, zero_lsb = 0, offset = 48;
                  uint8 banks[_SOC_ISM_MAX_BANKS], bcount, num_flds;
                  soc_mem_t l3_entry_mem;
                  int mcast_flag = 0;
                  void *entry;

                  if (BCM_IP6_MULTICAST(ip6_addr)) {
                      soc_field_t typef[] = {KEY_TYPE_0f, KEY_TYPE_1f, 
                                             KEY_TYPE_2f, KEY_TYPE_3f};
                      mcast_flag = 1;
                      l3_entry_mem = L3_ENTRY_4m;
                      entry = &l3_ipv6_mentry;
                      sal_memset(&l3_ipv6_mentry, 0, sizeof(l3_ipv6_mentry)); 
                      soc_L3_ENTRY_4m_field32_set(unit, &l3_ipv6_mentry,
                              VALID_0f, 1);
                      soc_mem_ip6_addr_set(unit, L3_ENTRY_4m, &l3_ipv6_mentry,
                              IPV6MC__GROUP_IP_ADDR_LWR_96f, ip6_addr,
                              SOC_MEM_IP6_LOWER_96BIT);
                      soc_mem_ip6_addr_set(unit, L3_ENTRY_4m, &l3_ipv6_mentry,
                              IPV6MC__GROUP_IP_ADDR_UPR_24f, ip6_addr,
                              SOC_MEM_IP6_BITS_119_96);
                      soc_mem_ip6_addr_set(unit, L3_ENTRY_4m, &l3_ipv6_mentry,
                              IPV6MC__SOURCE_IP_ADDR_LWR_64f, sip6_addr,
                              SOC_MEM_IP6_LOWER_ONLY);
                      soc_mem_ip6_addr_set(unit, L3_ENTRY_4m, &l3_ipv6_mentry,
                              IPV6MC__SOURCE_IP_ADDR_UPR_64f, sip6_addr,
                              SOC_MEM_IP6_UPPER_ONLY);
                      soc_L3_ENTRY_4m_field32_set(unit, &l3_ipv6_mentry,
                              IPV6MC__VRF_IDf, vrf);
                      soc_L3_ENTRY_4m_field32_set(unit, &l3_ipv6_mentry,
                              IPV6MC__VLAN_IDf, id);
                      for (i = 0; i < 4; i++) {
                          soc_L3_ENTRY_4m_field32_set(unit, &l3_ipv6_mentry, 
                                  typef[i],
                                  SOC_MEM_KEY_L3_ENTRY_4_IPV6MC_IPV6_MULTICAST);
                      }
                  } else {
                      l3_entry_mem = L3_ENTRY_2m;
                      entry = &l3_ipv6_uentry;
                      sal_memset(&l3_ipv6_uentry, 0, sizeof(l3_ipv6_uentry)); 
                      soc_L3_ENTRY_2m_field32_set(unit, &l3_ipv6_uentry,
                              VALID_0f, 1);
                      soc_mem_ip6_addr_set(unit, L3_ENTRY_2m, &l3_ipv6_uentry,
                              IPV6UC__IP_ADDR_LWR_64f, ip6_addr,
                              SOC_MEM_IP6_LOWER_ONLY);
                      soc_mem_ip6_addr_set(unit, L3_ENTRY_2m, &l3_ipv6_uentry,
                              IPV6UC__IP_ADDR_UPR_64f, ip6_addr,
                              SOC_MEM_IP6_UPPER_ONLY);
                      soc_L3_ENTRY_2m_field32_set(unit, &l3_ipv6_uentry,
                              VRF_IDf, vrf);
                      soc_L3_ENTRY_2m_field32_set(unit, &l3_ipv6_uentry, 
                              KEY_TYPE_0f,
                              SOC_MEM_KEY_L3_ENTRY_2_IPV6UC_IPV6_UNICAST);
                      soc_L3_ENTRY_2m_field32_set(unit, &l3_ipv6_uentry, 
                              KEY_TYPE_1f,
                              SOC_MEM_KEY_L3_ENTRY_2_IPV6UC_IPV6_UNICAST);
                  }

                  rc = soc_ism_get_banks_for_mem(unit, l3_entry_mem, banks, sizes,
                          &bcount);
                  if (rc || !bcount) {
                      cli_out("Error retreiving bank info: Test aborted.\n");
                      return CMD_FAIL;
                  }
                  rc = soc_generic_get_hash_key(unit, l3_entry_mem, entry,
                          keyflds, &lsbfld, &num_flds);
                  if (rc) {
                      cli_out("Error constructing key: Test aborted.\n");
                      return CMD_FAIL;
                  }
                  sal_memset(key, 0, sizeof(key));
                  soc_ism_gen_key_from_keyfields(unit, l3_entry_mem, 
                          entry, keyflds, key, num_flds);

                  key_nbits = soc_ism_mem_max_key_bits_get(unit, l3_entry_mem);
                  cli_out("Hash(%s)[%d] of key 0x",
                          (mcast_flag) ? "Ipv6MC" : "IPv6UC", hash_select);
                  for (loop = BITS2BYTES(key_nbits); loop > 0; loop--) {
                      cli_out("%02x", key[loop - 1]);
                  }
                  cli_out("\n");
                  for (b = 0; b < bcount; b++) {
                      rc = soc_ism_mem_hash_config_get(unit, l3_entry_mem, &act_zero_lsb);
                      if (rc) {
                          cli_out("Error retreiving hash config: Test aborted.\n");
                          return CMD_FAIL;
                      }
                      rc = soc_ism_hash_offset_config_get(unit, banks[b], &act_offset);
                      if (rc) {
                          cli_out("Error retreiving hash offset: Test aborted.\n");
                          return CMD_FAIL;
                      }
                      switch(hash_select) {
                          case XGS_HASH_CRC16_UPPER: 
                              offset = 48 - soc_ism_get_hash_bits(sizes[b]);
                              break;
                          case XGS_HASH_CRC16_LOWER: 
                              offset = 32;
                              break;
                          case XGS_HASH_LSB: 
                              offset = 48; zero_lsb = 1;
                              break;
                          case XGS_HASH_ZERO:
                              break;
                          case XGS_HASH_CRC32_UPPER: 
                              offset = 32 - soc_ism_get_hash_bits(sizes[b]);
                              break;
                          case XGS_HASH_CRC32_LOWER: 
                              offset = 0;
                              break;
                          case XGS_HASH_COUNT: 
                              offset = act_offset; zero_lsb = act_zero_lsb;
                              break;
                          default:
                              if (hash_select > 63) {
                                  cli_out("Invalid hash offset value !!\n");
                                  return CMD_FAIL;
                              }
                              offset = hash_select; if (hash_select >= 48) { zero_lsb = 1; }
                              break;
                      }
                      /* get config */
                      if (zero_lsb != act_zero_lsb) {
                          rc = soc_ism_mem_hash_config(unit, l3_entry_mem, zero_lsb);
                          if (rc) {
                              cli_out("Error setting hash config: Test aborted.\n");
                              return CMD_FAIL;
                          }
                      }
                      if (offset != act_offset) {
                          rc = soc_ism_hash_offset_config(unit, banks[b], offset);
                          if (rc) {
                              cli_out("Error setting hash offset: Test aborted.\n");
                              return CMD_FAIL;
                          }
                      }
                      rc = soc_generic_hash(unit, l3_entry_mem, entry, 1 << banks[b], 0,
                              &index, NULL, &bucket, NULL);
                      if (rc) {
                          cli_out("Error calculating hash: Test aborted.\n");
                          return CMD_FAIL;
                      }
                      cli_out("\tBank[%d] num: %d, bucket: %d, index: %d\n", b, 
                              banks[b], bucket, index);
                      /* restore config */
                      if (zero_lsb != act_zero_lsb) {
                          rc = soc_ism_mem_hash_config(unit, l3_entry_mem, act_zero_lsb);
                          if (rc) {
                              cli_out("Error setting hash config: Test aborted.\n");
                              return CMD_FAIL;
                          }
                      }
                      if (offset != act_offset) {
                          rc = soc_ism_hash_offset_config(unit, banks[b], act_offset);
                          if (rc) {
                              cli_out("Error setting hash offset: Test aborted.\n");
                              return CMD_FAIL;
                          }
                      }
                  }
                  return CMD_OK;
              }
#endif /* BCM_ISM_SUPPORT */

              if (hash_select == FB_HASH_COUNT) {
                  /* Get the hash selection from hardware */
                  uint32 regval;
                  int rv;

                  if ((rv = READ_HASH_CONTROLr(unit, &regval)) < 0) {
                      cli_out("%s: ERROR: %s\n", ARG_CMD(a), soc_errmsg(rv));
                      return CMD_FAIL;
                  }
                  hash_select = soc_reg_field_get(unit, HASH_CONTROLr, regval,
                                                  L3_HASH_SELECTf);
              }
#endif
#ifdef BCM_TRX_SUPPORT
              if (SOC_MEM_FIELD_VALID(unit, L3_ENTRY_ONLYm, KEY_TYPEf)) {
                  int key_nbits;
                  int hash_bucket = 0;
                  int mcast_flag =0;
                  int loop;
                  l3_entry_ipv6_unicast_entry_t    l3x_ip6_uentry;
                  l3_entry_ipv6_multicast_entry_t  l3x_ip6_mentry;
                  soc_field_t vrf_field, vlan_field;
                  int key_type_value;

                  if (BCM_IP6_MULTICAST(ip6_addr)) {
                      soc_field_t typef[] = {KEY_TYPE_0f, KEY_TYPE_1f, 
                                             KEY_TYPE_2f, KEY_TYPE_3f};
                      mcast_flag  = 1;

#if defined(BCM_TRIDENT2_SUPPORT)
                      if (SOC_IS_TRIDENT2X(unit)) {
                          key_type_value = TD2_L3_HASH_KEY_TYPE_V6MC;
                      } else
#endif /* BCM_TRIDENT2_SUPPORT */
                      {
                          key_type_value = TR_L3_HASH_KEY_TYPE_V6MC;
                      }
                      sal_memset(&l3x_ip6_mentry, 0,
                                 sizeof(l3_entry_ipv6_multicast_entry_t));

                      soc_mem_ip6_addr_set(unit, L3_ENTRY_IPV6_MULTICASTm,
                                           &l3x_ip6_mentry,
                                           GROUP_IP_ADDR_LWR_64f,
                                           (uint8*) &ip6_addr,
                                           SOC_MEM_IP6_LOWER_ONLY);

                      ip6_addr[0] = 0x0;
                      soc_mem_ip6_addr_set(unit, L3_ENTRY_IPV6_MULTICASTm,
                                           &l3x_ip6_mentry, GROUP_IP_ADDR_UPR_56f,
                                           (uint8*) &ip6_addr,
                                           SOC_MEM_IP6_UPPER_ONLY);

                      soc_mem_ip6_addr_set(unit, L3_ENTRY_IPV6_MULTICASTm,
                                           &l3x_ip6_mentry, SOURCE_IP_ADDR_LWR_64f,
                                           (uint8*) &sip6_addr,
                                           SOC_MEM_IP6_LOWER_ONLY);

                      soc_mem_ip6_addr_set(unit, L3_ENTRY_IPV6_MULTICASTm,
                                           &l3x_ip6_mentry, SOURCE_IP_ADDR_UPR_64f,
                                           (uint8*) &sip6_addr,
                                           SOC_MEM_IP6_UPPER_ONLY);

                       vlan_field = (SOC_MEM_FIELD_VALID(unit, 
                                        L3_ENTRY_IPV6_MULTICASTm, VLAN_IDf))? \
                                        VLAN_IDf: VLAN_ID_0f;

                       soc_L3_ENTRY_IPV6_MULTICASTm_field32_set(unit,
                                           &l3x_ip6_mentry, vlan_field, vlan);

                       vrf_field = (SOC_MEM_FIELD_VALID(unit, 
                                        L3_ENTRY_IPV6_MULTICASTm, VRF_IDf))? \
                                        VRF_IDf: VRF_ID_0f;

                      /* prevent the case once VRF_ID field is not existed */
                      if (soc_mem_field_valid(unit, 
                                        L3_ENTRY_IPV6_MULTICASTm, vrf_field)) {
                          soc_L3_ENTRY_IPV6_MULTICASTm_field32_set(unit,
                                           &l3x_ip6_mentry, vrf_field, vrf);
                      }

                      for (i=0; i < 4; i++) {
                          soc_L3_ENTRY_IPV6_MULTICASTm_field32_set
                              (unit, &l3x_ip6_mentry, typef[i], 
                               key_type_value);
                      }
                      /* coverity[overrun-buffer-val : FALSE]    */
                      key_nbits = soc_fb_l3x_base_entry_to_key(unit,
                                                               (uint32 *) &l3x_ip6_mentry, key);
                  } else {
#if defined(BCM_TRIDENT2_SUPPORT)
                      if (SOC_IS_TRIDENT2X(unit)) {
                          key_type_value = TD2_L3_HASH_KEY_TYPE_V6UC;
                      } else
#endif /* BCM_TRIDENT2_SUPPORT */
                      {
                          key_type_value = TR_L3_HASH_KEY_TYPE_V6UC;
                      }
                      sal_memset(&l3x_ip6_uentry, 0,
                                 sizeof(l3_entry_ipv6_unicast_entry_t));

                      soc_mem_ip6_addr_set(unit, L3_ENTRY_IPV6_UNICASTm,
                                           &l3x_ip6_uentry, IP_ADDR_LWR_64f,
                                           ip6_addr, SOC_MEM_IP6_LOWER_ONLY);

                      soc_mem_ip6_addr_set(unit, L3_ENTRY_IPV6_UNICASTm,
                                           &l3x_ip6_uentry, IP_ADDR_UPR_64f,
                                           ip6_addr, SOC_MEM_IP6_UPPER_ONLY);

                      soc_L3_ENTRY_IPV6_UNICASTm_field32_set
                          (unit, &l3x_ip6_uentry, KEY_TYPE_1f, key_type_value);

                      soc_L3_ENTRY_IPV6_UNICASTm_field32_set
                          (unit, &l3x_ip6_uentry, KEY_TYPE_0f, key_type_value);

                       vrf_field = (SOC_MEM_FIELD_VALID(unit, 
                                        L3_ENTRY_IPV6_UNICASTm, VRF_IDf))? \
                                        VRF_IDf: VRF_ID_0f;

                      /* prevent the case once VRF_ID field is not existed */
                      if (soc_mem_field_valid(unit, 
                                        L3_ENTRY_IPV6_UNICASTm, vrf_field)) {
                          soc_L3_ENTRY_IPV6_UNICASTm_field32_set(unit,
                                               &l3x_ip6_mentry, vrf_field, vrf);
                      }
                      /* coverity[overrun-buffer-val : FALSE]    */
                      key_nbits = soc_fb_l3x_base_entry_to_key(unit,
                                                               (uint32 *) &l3x_ip6_uentry, key);
                  }
                  hash_bucket = soc_fb_l3_hash(unit, hash_select, key_nbits, key);
                  if (mcast_flag) {
                      cli_out("Hash(IPMC)[%d] of key 0x",hash_select);
                      for (loop = BITS2BYTES(key_nbits); loop > 0; loop--) {
                          cli_out("%02x", key[loop - 1]);
                      }
                      cli_out(" ");
                      cli_out("is bucket 0x%3x (%d)\n", hash_bucket,
                              hash_bucket);
                  } else {
                      cli_out("Hash(UC)[%d] of key 0x",hash_select);
                      for (loop = BITS2BYTES(key_nbits); loop > 0; loop--) {
                          cli_out("%02x", key[loop - 1]);
                      }
                      cli_out(" ");
                      cli_out("is bucket 0x%3x (%d)\n", hash_bucket,
                              hash_bucket);
                  }
                  break;
              }
#endif /* BCM_TRX_SUPPORT */
#ifdef BCM_FIREBOLT_SUPPORT
              if (SOC_IS_FBX(unit)) {
                  int key_nbits;
                  int hash_bucket = 0;
                  int mcast_flag =0;
                  int loop;
                  l3_entry_ipv6_unicast_entry_t    l3x_ip6_uentry;
                  l3_entry_ipv6_multicast_entry_t  l3x_ip6_mentry;

                  if (BCM_IP6_MULTICAST(ip6_addr)) {
                      soc_field_t v6f[] = {V6_0f, V6_1f, V6_2f, V6_3f};
                      soc_field_t mcf[] = {IPMC_0f, IPMC_1f, IPMC_2f, IPMC_3f};
                      soc_field_t vidf[] = {VLAN_ID_0f, VLAN_ID_1f, VLAN_ID_2f,
                          VLAN_ID_3f};
                          mcast_flag  = 1;

                          sal_memset(&l3x_ip6_mentry, 0,
                                     sizeof(l3_entry_ipv6_multicast_entry_t));

                          soc_mem_ip6_addr_set(unit, L3_ENTRY_IPV6_MULTICASTm,
                                               &l3x_ip6_mentry,
                                               GROUP_IP_ADDR_LWR_64f,
                                               (uint8*) &ip6_addr,
                                               SOC_MEM_IP6_LOWER_ONLY);

                          ip6_addr[0] = 0x0;
                          soc_mem_ip6_addr_set(unit, L3_ENTRY_IPV6_MULTICASTm,
                                               &l3x_ip6_mentry, GROUP_IP_ADDR_UPR_56f,
                                               (uint8*) &ip6_addr,
                                               SOC_MEM_IP6_UPPER_ONLY);

                          soc_mem_ip6_addr_set(unit, L3_ENTRY_IPV6_MULTICASTm,
                                               &l3x_ip6_mentry, SOURCE_IP_ADDR_LWR_64f,
                                               (uint8*) &sip6_addr,
                                               SOC_MEM_IP6_LOWER_ONLY);

                          soc_mem_ip6_addr_set(unit, L3_ENTRY_IPV6_MULTICASTm,
                                               &l3x_ip6_mentry, SOURCE_IP_ADDR_UPR_64f,
                                               (uint8*) &sip6_addr,
                                               SOC_MEM_IP6_UPPER_ONLY);

                          for (i=0; i < 4; i++) {
                              soc_L3_ENTRY_IPV6_MULTICASTm_field32_set(unit,
                                                                       &l3x_ip6_mentry, v6f[i], 1);
                              soc_L3_ENTRY_IPV6_MULTICASTm_field32_set(unit,
                                                                       &l3x_ip6_mentry, mcf[i], 1);
                              soc_L3_ENTRY_IPV6_MULTICASTm_field32_set(unit,
                                                                       &l3x_ip6_mentry, vidf[i], vlan);
                          }
                          /* coverity[overrun-buffer-val : FALSE]    */
                          key_nbits = soc_fb_l3x_base_entry_to_key(unit,
                                                                   (uint32 *) &l3x_ip6_mentry, key);
                  } else {
                      sal_memset(&l3x_ip6_uentry, 0,
                                 sizeof(l3_entry_ipv6_unicast_entry_t));

                      soc_mem_ip6_addr_set(unit, L3_ENTRY_IPV6_UNICASTm,
                                           &l3x_ip6_uentry, IP_ADDR_LWR_64f,
                                           ip6_addr, SOC_MEM_IP6_LOWER_ONLY);

                      soc_mem_ip6_addr_set(unit, L3_ENTRY_IPV6_UNICASTm,
                                           &l3x_ip6_uentry, IP_ADDR_UPR_64f,
                                           ip6_addr, SOC_MEM_IP6_UPPER_ONLY);

                      soc_L3_ENTRY_IPV6_UNICASTm_field32_set(unit,
                                                             &l3x_ip6_uentry,
                                                             V6_1f, 1);

                      soc_L3_ENTRY_IPV6_UNICASTm_field32_set(unit,
                                                             &l3x_ip6_uentry,
                                                             V6_0f, 1);
                      /* coverity[overrun-buffer-val : FALSE]    */
                      key_nbits = soc_fb_l3x_base_entry_to_key(unit,
                                                               (uint32 *) &l3x_ip6_uentry, key);
                  }
                  hash_bucket = soc_fb_l3_hash(unit, hash_select,
                                               key_nbits, key);
                  if (mcast_flag) {
                      cli_out("Hash(IPMC)[%d] of key 0x",hash_select);
                      for (loop = BITS2BYTES(key_nbits); loop > 0; loop--) {
                          cli_out("%02x", key[loop - 1]);
                      }
                      cli_out(" ");
                      cli_out("is bucket 0x%3x (%d)\n", hash_bucket,
                              hash_bucket);
                  } else {
                      cli_out("Hash(UC)[%d] of key 0x",hash_select);
                      for (loop = BITS2BYTES(key_nbits); loop > 0; loop--) {
                          cli_out("%02x", key[loop - 1]);
                      }
                      cli_out(" ");
                      cli_out("is bucket 0x%3x (%d)\n", hash_bucket,
                              hash_bucket);
                  }
              }
#endif /* BCM_FIREBOLT_SUPPORT */
              break;

              /* l3 l3table sanity */
          case L3_SANITY:
              cli_out("Command only valid for XGS2 switches\n");
              return CMD_FAIL;
          case L3_CONFLICT:
            return _l3_cmd_l3_conflict_show(unit, a);
#endif /* BCM_XGS_SWITCH_SUPPORT*/
          default:
              return CMD_USAGE;
              break;
        }

        return CMD_OK;
    }

    if (sal_strcasecmp(table, "defip") == 0) {
	switch (subcmd) {
        /* l3 defip add */
	case L3_ADD:
        return _l3_cmd_route_add(unit, a, FALSE); 
        /* l3 defip destroy */
	case L3_DESTROY:
	    parse_table_init(unit, &pt);
#if defined(BCM_XGS3_SWITCH_SUPPORT)
	    parse_table_add(&pt, "VRF", PQ_DFL | PQ_INT, 0, (void *)&vrf, 0);
#endif
	    parse_table_add(&pt, "IP", PQ_DFL | PQ_IP, 0, (void *)&ip_addr, 0);
	    parse_table_add(&pt, "MaSk", PQ_DFL | PQ_IP, 0, (void *)&mask, 0);
#ifdef BCM_XGS_SWITCH_SUPPORT
	    parse_table_add(&pt, "ECMP", PQ_DFL | PQ_BOOL, 0, (void *)&ecmp, 0);
	    parse_table_add(&pt, "Mac", PQ_DFL | PQ_MAC, 0, (void *)mac, 0);
	    parse_table_add(&pt, "Gateway", PQ_DFL | PQ_IP, 0, (void *)&gateway, 0);
	    parse_table_add(&pt, "Port", PQ_DFL | PQ_PORT, 0, (void *)&port, 0);
	    parse_table_add(&pt, "MOdule", PQ_DFL | PQ_INT, 0, (void*)&module, 0);
	    parse_table_add(&pt, "INtf", PQ_DFL | PQ_INT, 0, (void *)&interface, 0);
	    parse_table_add(&pt, "VLAN", PQ_DFL | PQ_INT, 0, (void *)&id, 0);
#endif
	    if (!parseEndOk(a, &pt, &retCode)) {
	        return retCode;
	    }

            bcm_l3_route_t_init(&route_info);

#if defined(BCM_XGS3_SWITCH_SUPPORT)
	    route_info.l3a_vrf     = vrf;
#endif
	    route_info.l3a_subnet = ip_addr;
	    route_info.l3a_ip_mask = mask;
	    route_info.l3a_vid = id; 
	    if (((mask & 0x80000000) == 0) && ip_addr != 0) {
	        cli_out("%s: Invalid mask\n", ARG_CMD(a));
	        return CMD_FAIL;
	    }
#ifdef BCM_XGS_SWITCH_SUPPORT

	    if (ecmp) {
	        route_info.l3a_flags     |= BCM_L3_MULTIPATH;
	        sal_memcpy(route_info.l3a_nexthop_mac, mac, sizeof(mac));
	        route_info.l3a_nexthop_ip = gateway;
	        route_info.l3a_intf       = interface;
	        route_info.l3a_modid      = module;
	        route_info.l3a_port_tgid  = port;
	    }
#endif

        if ((r = bcm_l3_route_delete(unit, &route_info)) < 0) {
            format_ipaddr(ip_str, ip_addr);
            cli_out("%s: Error deleting %d.%d.%d.%d from DEF IP "
                    "table: %s\n", ARG_CMD(a),
                    (route_info.l3a_subnet >> 24) & 0xff,
                    (route_info.l3a_subnet >> 16) & 0xff,
                    (route_info.l3a_subnet >> 8) & 0xff,
                    route_info.l3a_subnet & 0xff,
                    bcm_errmsg(r));
            return CMD_FAIL;
        }
	    break;

        /* l3 defip clear */
	case L3_CLEAR:
	    bcm_l3_route_delete_all(unit, NULL);
	    break;

        /* l3 defip age */
	case L3_AGE:
	    bcm_l3_route_age(unit, 0, _l3_cmd_route_age_callback, NULL);
	    return CMD_OK;

        /* l3 defip show */
        case L3_SHOW:
            if ((r = bcm_l3_info(unit, &l3_hw_status)) < 0) {
                cli_out("%s: Error L3 accessing: %s\n",
                        ARG_CMD(a), bcm_errmsg(r));
                return CMD_FAIL;
            }
            free_defip = l3_hw_status.l3info_max_route -
                         l3_hw_status.l3info_occupied_route;
#if defined(BCM_XGS_SWITCH12_SUPPORT)
            free_lpm_blk = l3_hw_status.l3info_max_lpm_block -
                           l3_hw_status.l3info_used_lpm_block;
#endif
            last_entry = l3_hw_status.l3info_max_route;
            num_entries = last_entry;
    
            if (ARG_CNT(a)) {
                if (!isint(ARG_CUR(a))) {
                    cli_out("%s: Invalid entry number: %s\n", ARG_CMD(a),
                            ARG_CUR(a));
                    return (CMD_FAIL);
                }
                first_entry = parse_integer(ARG_GET(a));
                if (first_entry > last_entry) {
                    cli_out("%s: Error: Illegal entry number (0 to %d)\n",
                            ARG_CMD(a), last_entry);
                    return (CMD_FAIL);
                }
                if (ARG_CNT(a)) {
                    int tmp_last_entry = 0;
                    if (!isint(ARG_CUR(a))) {
                        cli_out("%s: Invalid count: %s\n", ARG_CMD(a),
                                ARG_CUR(a));
                        return (CMD_FAIL);
                    }
                    tmp_last_entry =
                        parse_integer(ARG_GET(a)) + first_entry - 1;
                    if ((tmp_last_entry > last_entry) ||
                        (tmp_last_entry < first_entry)) {
                        cli_out("%s: Error: Illegal entry count (1 to %d)\n",
                                ARG_CMD(a), last_entry - first_entry + 1);
                        return (CMD_FAIL);
                    }
                    last_entry = tmp_last_entry;
                } else {
                    last_entry = first_entry;
                }
            } else {
                first_entry = 0;
            }

            cli_out("Unit %d, Total Number of DEFIP entries: %d\n",
                    unit, num_entries);
            cli_out("#     VRF     Net addr             Next Hop Mac      "
                    "  INTF MODID PORT PRIO CLASS HIT VLAN\n");
            bcm_l3_route_traverse(unit, 0, first_entry, last_entry,
                                      _l3_cmd_route_print, NULL);
            return CMD_OK;

#ifdef BCM_XGS_SWITCH_SUPPORT
        /* l3 defip ecmp <MAX>=<val> */
        case L3_ECMP:
            parse_table_init(unit, &pt);
            parse_table_add(&pt, "MAX", PQ_DFL | PQ_INT, 0, (void *)&max_ecmp, 0);
            if (!parseEndOk(a, &pt, &retCode)) {
                return retCode;
            }

            bcm_l3_route_max_ecmp_set(unit, max_ecmp);
            break;

        /* l3 defip ecmphash */
        case L3_ECMP_HASH:
            return _l3_cmd_defip_ecmp_hash(unit, a);
#endif /* BCM_XGS_SWITCH_SUPPORT */
#ifdef FB_LPM_DEBUG
        /* l3 defip state check */
        case L3_SANITY:
            if (soc_feature(unit, soc_feature_l3_lpm_scaling_enable)) {
                if ((r = lpm128_state_verify(unit)) < 0) {
                    return (CMD_FAIL);
                }
                cli_out("Successful: Unit %d, Checking lpm states passed!\n",
                        unit);
            } else {
                cli_out("Error: Command only valid for LPM scaling mode\n");
            }
        break;
#endif /* FB_LPM_DEBUG */
	default:
	    return CMD_USAGE;
	    break;
	}

	return CMD_OK;
    }

#if defined(BCM_XGS3_SWITCH_SUPPORT)

#ifdef ALPM_ENABLE   
    if (sal_strcasecmp(table, "alpm") == 0) {
        if (SOC_IS_TD2_TT2(unit) &&
            soc_feature(unit, soc_feature_alpm) &&
            soc_property_get(unit, spn_L3_ALPM_ENABLE, 0)) {
            switch (subcmd) {
            case L3_SHOW:
                return _l3_cmd_alpm_debug_show(unit, a);
            case L3_FIND:
                return _l3_cmd_alpm_find(unit, a);
            case L3_SANITY:
                return _l3_cmd_alpm_sanity_check(unit, a);
            default:
               return CMD_USAGE;
            }
            return CMD_OK;
            
        } else {
            cli_out("ALPM is unsupported or disabled!\n" );
            ARG_DISCARD(a);
            return CMD_OK; 
        } 
    }
#endif

    if (sal_strcasecmp(table, "ip6host") == 0) {
	switch (subcmd) {
        /* l3 ip6host add */
	case L3_ADD:
        return _l3_cmd_host_add(unit, a, TRUE);
        /* l3 ip6host delete */
	case L3_DESTROY:
	    parse_table_init(unit, &pt);
	    parse_table_add(&pt, "VRF", PQ_DFL | PQ_INT, 0, (void *)&vrf, 0);
	    parse_table_add(&pt, "IP", PQ_DFL | PQ_IP6, 0, (void *)&ip6_addr, 0);
	    if (!parseEndOk(a, &pt, &retCode)) {
		return retCode;
            }

            bcm_l3_host_t_init(&ip_info);
            sal_memcpy(ip_info.l3a_ip6_addr, ip6_addr, BCM_IP6_ADDRLEN);
            ip_info.l3a_vrf     = vrf;
            ip_info.l3a_flags   = BCM_L3_IP6;
	    if ((r = bcm_l3_host_delete(unit, &ip_info)) < 0) {
		cli_out("%s: Error deleting from L3 table %s\n",
                        ARG_CMD(a), bcm_errmsg(r));
		return CMD_FAIL;
	    }
	    break;

        /* l3 ip6host clear */
	case L3_CLEAR:
            ip_info.l3a_flags   = BCM_L3_IP6;
	    bcm_l3_host_delete_all(unit, &ip_info);
	    break;

        /* l3 ip6host age */
	case L3_AGE:
	    bcm_l3_host_age(unit, BCM_L3_HIT | BCM_L3_IP6,
                            _l3_cmd_host_age_callback, NULL);
	    return CMD_OK;

        /* l3 ip6host show */
	case L3_SHOW:
            if ((r = bcm_l3_info(unit, &l3_hw_status)) < 0) {
                cli_out("%s: Error L3 accessing: %s\n", ARG_CMD(a), bcm_errmsg(r));
                return CMD_FAIL;
            }
            free_l3 = (l3_hw_status.l3info_max_host -
                       l3_hw_status.l3info_occupied_host) / 2;
            last_entry = l3_hw_status.l3info_max_host / 2;
            if (ARG_CNT(a)) {
                if (!isint(ARG_CUR(a))) {
                    cli_out("%s: Invalid number: %s\n", ARG_CMD(a), ARG_CUR(a));
                    return (CMD_FAIL);
                }
                first_entry = parse_integer(ARG_GET(a));
                if (first_entry > last_entry) {
                    cli_out("%s: Error: Illegal entry number (0 to %d)\n",
                            ARG_CMD(a), last_entry);
                    return (CMD_FAIL);
                }
                last_entry = first_entry;
	    } else {
                first_entry = 0;
            }

            cli_out("Unit %d, free IPv6 table entries: %d\n", unit, free_l3);
            cli_out("Entry VRF IP address                         "
                    "          Mac Address        INTF MOD PORT     CLASS HIT"
                    "     H/W Index\n");
            user_data = soc_cm_salloc(unit, sizeof(int), "l3host hw index");
            if (NULL == user_data) {
                cli_out("Failed to allocate memory\n");
                return CMD_FAIL;
            }
            bcm_l3_host_traverse(unit, BCM_L3_IP6 | _BCM_L3_SHOW_INDEX,
                                 first_entry, last_entry,
                                 _l3_cmd_host_print, user_data);
            cli_out("\n");
            if (user_data) {
                soc_cm_sfree(unit, user_data);
            }
	    return CMD_OK;
        default:
            return CMD_USAGE;
            break;
        }
        return CMD_OK;
    }

    if (sal_strcasecmp(table, "ip6route") == 0) {
        bcm_ip6_t ip6_mask;
        int masklen = -1;
        int mask_len_max = 
            soc_feature(unit, soc_feature_lpm_prefix_length_max_128) ? 128 : 64;

	switch (subcmd) {
        /* l3 ip6route add */
	case L3_ADD:
        return _l3_cmd_route_add(unit, a, TRUE); 
        
        /* l3 ip6route delete */
	case L3_DESTROY:
	    parse_table_init(unit, &pt);
	    parse_table_add(&pt, "VRF", PQ_DFL | PQ_INT, 0, (void *)&vrf, 0);
	    parse_table_add(&pt, "IP", PQ_DFL | PQ_IP6, 0, (void *)&ip6_addr, 0);
	    parse_table_add(&pt, "MaskLen", PQ_DFL | PQ_INT, 0,
                            (void *)&masklen, 0);
            parse_table_add(&pt, "ECMP", PQ_DFL | PQ_BOOL, 0, (void *)&ecmp, 0);
            parse_table_add(&pt, "Mac", PQ_DFL | PQ_MAC, 0, (void *)mac, 0);
            parse_table_add(&pt, "Port", PQ_DFL | PQ_PORT, 0, (void *)&port, 0);
            parse_table_add(&pt, "MOdule", PQ_DFL | PQ_INT, 0, (void*)&module, 0);
            parse_table_add(&pt, "INtf", PQ_DFL | PQ_INT, 0, (void *)&interface, 0);
            if (!parseEndOk(a, &pt, &retCode)) {
                return retCode;
            }

        if ((masklen < 0) || (masklen > mask_len_max)) {
            cli_out("%s: Invalid prefix length.\n", ARG_CMD(a));
            return CMD_FAIL;
        }
            bcm_l3_route_t_init(&route_info);

        route_info.l3a_vrf = vrf;
	    sal_memcpy(route_info.l3a_ip6_net, ip6_addr, BCM_IP6_ADDRLEN);
            bcm_ip6_mask_create(ip6_mask, masklen);
            sal_memcpy(route_info.l3a_ip6_mask, ip6_mask, BCM_IP6_ADDRLEN);
	    sal_memcpy(route_info.l3a_nexthop_mac, mac, sizeof(mac));
            route_info.l3a_flags = BCM_L3_IP6;
            if (ecmp) {
                route_info.l3a_flags     |= BCM_L3_MULTIPATH;
                sal_memcpy(route_info.l3a_nexthop_mac, mac, sizeof(mac));
                route_info.l3a_intf       = interface;
                route_info.l3a_modid      = module;
                route_info.l3a_port_tgid  = port;
            }

            if ((r = bcm_l3_route_delete(unit, &route_info)) < 0) {
                format_ip6addr(ip_str, ip6_addr);
                cli_out("ERROR %s: delete %s/%d from DEF IP "
                        "table: %s\n", ARG_CMD(a), ip_str, masklen, bcm_errmsg(r));
                return CMD_FAIL;
            }
            break;

        /* l3 ip6route clear */
	case L3_CLEAR:
            route_info.l3a_flags   = BCM_L3_IP6;
	    bcm_l3_route_delete_all(unit, &route_info);
	    break;

        /* l3 defip age */
	case L3_AGE:
	    bcm_l3_route_age(unit, BCM_L3_IP6, _l3_cmd_route_age_callback, NULL);
	    break;

        /* l3 ip6route show */
    case L3_SHOW:
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
#ifdef BCM_TRX_SUPPORT
        ext_tcam_flag = EXT_TCAM_IS_L3_ENABLED(unit);
#endif
        if ((SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIUMPH3(unit)) &&
            soc_feature(unit, soc_feature_l3_shared_defip_table) &&
            !(soc_property_get(unit, spn_L3_ALPM_ENABLE, 0) &&
              soc_feature(unit, soc_feature_alpm)) &&
            !ext_tcam_flag) {
            if ((r = bcm_switch_object_count_get(unit,
                     bcmSwitchObjectL3RouteV6Routes64bMax,
                     &defip_64_max)) < 0) {
                cli_out("%s: Error getting max number of 64bv6: %s\n",
                        ARG_CMD(a), bcm_errmsg(r));
                return CMD_FAIL;         
            }
            if ((r = bcm_switch_object_count_get(unit, 
                     bcmSwitchObjectL3RouteV6Routes128bMax, 
                     &defip_128_max)) < 0) {
                cli_out("%s: Error getting max number of 128bv6: %s\n", 
                        ARG_CMD(a), bcm_errmsg(r));
                return CMD_FAIL;         
            }
            num_entries = defip_64_max + defip_128_max;
            last_entry = num_entries - 1 ;
            if ((r = bcm_switch_object_count_get(unit, 
                     bcmSwitchObjectL3RouteV6Routes64bFree, 
                     &defip_64_free)) < 0) {
                cli_out("%s: Error getting free number of 64bv6: %s\n", 
                        ARG_CMD(a), bcm_errmsg(r));
                return CMD_FAIL;         
            }
            if ((r = bcm_switch_object_count_get(unit, 
                     bcmSwitchObjectL3RouteV6Routes128bFree, 
                     &defip_128_free)) < 0) {
                cli_out("%s: Error getting free number of 128bv6: %s\n", 
                        ARG_CMD(a), bcm_errmsg(r));
                return CMD_FAIL;         
            }
            free_defip = defip_64_free + defip_128_free;  
        } else
#endif
        {
            if ((r = bcm_l3_info(unit, &l3_hw_status)) < 0) {
                cli_out("%s: Error L3 accessing: %s\n", ARG_CMD(a), bcm_errmsg(r));
                return CMD_FAIL;
            }
            if (soc_feature(unit, soc_feature_urpf)) {
                if ((r = bcm_switch_control_get(unit,
                          bcmSwitchL3UrpfRouteEnable, &urpf)) < 0) {
                    cli_out("%s: Error L3 accessing: %s\n", ARG_CMD(a), bcm_errmsg(r));
                    return CMD_FAIL;
                }
            }
            free_defip = (l3_hw_status.l3info_max_route -
                          l3_hw_status.l3info_occupied_route) / 2;
            last_entry = l3_hw_status.l3info_max_route / 2;
            num_entries = last_entry;
            if (urpf) {
                free_defip = (l3_hw_status.l3info_max_route -
                              (l3_hw_status.l3info_occupied_route * 2)) / 4;
                last_entry = last_entry / 2;
                num_entries = last_entry + 1;
            }

        }
        if (ARG_CNT(a)) {
            if (!isint(ARG_CUR(a))) {
                cli_out("%s: Invalid number: %s\n", ARG_CMD(a), ARG_CUR(a));
                return (CMD_FAIL);
            }
            first_entry = parse_integer(ARG_GET(a));
            if (first_entry > last_entry) {
                cli_out("%s: Error: Illegal entry number (0 to %d)\n",
                        ARG_CMD(a), last_entry);
                return (CMD_FAIL);
            }
            if (ARG_CNT(a)) {
                int tmp_last_entry = 0;
                if (!isint(ARG_CUR(a))) {
                    cli_out("%s: Invalid count: %s\n", ARG_CMD(a), ARG_CUR(a));
                    return (CMD_FAIL);
                }
                tmp_last_entry = parse_integer(ARG_GET(a)) + first_entry - 1;
                if ((tmp_last_entry > last_entry) ||
                    (tmp_last_entry < first_entry)) {
                    cli_out("%s: Error: Illegal entry count (1 to %d)\n",
                            ARG_CMD(a), last_entry - first_entry + 1);
                    return (CMD_FAIL);
                }
                last_entry = tmp_last_entry;
            } else {
                last_entry = first_entry;
            }
        } else {
            first_entry = 0;
        }
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
        if ((SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIUMPH3(unit)) &&
            soc_feature(unit, soc_feature_l3_shared_defip_table) &&
            !(soc_property_get(unit, spn_L3_ALPM_ENABLE, 0) &&
              soc_feature(unit, soc_feature_alpm))) {
            cli_out("Unit %d, Total Number of IPv6 entries: %d "
                    "(IPv6/64 %d, IPv6/128 %d)\n", unit, num_entries,
                    defip_64_max, defip_128_max);
        } else 
#endif
        cli_out("Unit %d, Total Number of IPv6 entries: %d\n",
                unit, num_entries);
        bcm_l3_route_max_ecmp_get(unit, &max_ecmp);
        if (max_ecmp) {
            cli_out("Max number of ECMP paths %d\n", max_ecmp);
        }
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
        if ((SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIUMPH3(unit)) &&
            soc_feature(unit, soc_feature_l3_shared_defip_table) &&
            !(soc_property_get(unit, spn_L3_ALPM_ENABLE, 0) &&
              soc_feature(unit, soc_feature_alpm))) {
            cli_out("Free IPv6 entries available: %d "
                    "(IPv6/64 %d, IPv6/128 %d)\n", free_defip,
                    defip_64_free, defip_128_free);
        } else 
#endif
        cli_out("Free IPv6 entries available: %d\n", free_defip);

        cli_out("#     VRF     Net addr                                "
                "     Next Hop Mac      "
                "  INTF MODID PORT PRIO CLASS HIT VLAN\n");
        bcm_l3_route_traverse(unit, BCM_L3_IP6, first_entry, last_entry,
                              _l3_cmd_route_print, NULL);
        return CMD_OK;

        default:
            return CMD_USAGE;
            break;
        }
        return CMD_OK;
    }

    if (sal_strcasecmp(table, "tunnel_init") == 0) {
        switch (subcmd) {

        /* l3 tunnel_init set */
        case L3_SET:
            return _l3_cmd_tunnel_init_add(unit, a);
        /* l3 tunnel_init show */
        case L3_SHOW:
            return _l3_cmd_tunnel_init_show(unit, a);
        /* l3 tunnel_init clear */
        case L3_CLEAR:
            return _l3_cmd_tunnel_init_clear(unit, a);
        default:
            return CMD_USAGE;
            break;
        }
        return CMD_OK;
    }

    if (sal_strcasecmp(table, "tunnel_term") == 0) {
        switch (subcmd) {
        /* l3 tunnel_term add */
        case L3_ADD:
            return _l3_cmd_tunnel_term_add(unit, a); 
        /* l3 tunnel_term delete */
        case L3_DESTROY:
            return _l3_cmd_tunnel_term_delete(unit, a); 
        /* l3 tunnel_term show */
        case L3_SHOW:
            return _l3_cmd_tunnel_term_get(unit, a); 
        default:
            return CMD_USAGE;
            break;
        }
        return CMD_OK;
    }

    if (sal_strcasecmp(table, "dscp_map_port") == 0) {
        switch (subcmd) {
            /* l3 dscp_map_port show */
          case L3_SHOW:
              return _l3_cmd_dscp_map_port_show(unit, a); 
              /* l3 dscp_map_port set */
          case L3_ADD:
          case L3_SET:
              return _l3_cmd_dscp_map_port_set(unit, a); 
          default:
              return (CMD_USAGE);
        }
        return CMD_OK;
    }
    if (sal_strcasecmp(table, "dscp_map") == 0) {
        switch (subcmd) {
            /* l3 dscp_map add */
          case L3_ADD:
              return _l3_cmd_dscp_map_add(unit, a); 
              /* l3 dscp_map destroy */
          case L3_DESTROY:
              return _l3_cmd_dscp_map_delete(unit, a); 

              /* l3 dscp_map show */
          case L3_SHOW:
              return _l3_cmd_dscp_map_show(unit, a); 
              /* l3 dscp_map set */
          case L3_SET:
              return _l3_cmd_dscp_map_set(unit, a); 

          default:
              return CMD_USAGE;
        }
        return CMD_OK;
    }
    if (soc_feature(unit, soc_feature_nat)) {
        if (sal_strcasecmp(table, "nat_ingress") == 0) {
            switch (subcmd) {
                /* l3 nat_ingress show */
            case L3_SHOW:
                return _l3_cmd_nat_ingress_show(unit, a);
            default:
                return CMD_USAGE;
            }
            return CMD_OK;
        }
        if (sal_strcasecmp(table, "nat_egress") == 0) {
            switch (subcmd) {
                /* l3 nat_egress show */
            case L3_SHOW:
                return _l3_cmd_nat_egress_show(unit, a);
            case L3_ADD:
                return _l3_cmd_nat_egress_add(unit, a);
            default:
                return CMD_USAGE;
            }
            return CMD_OK;
        }
    }

    if (sal_strcasecmp(table, "route") == 0) {
        char *subcmd2;
        switch (subcmd) {
            /* l3 route test */
            case L3_TEST:
                subcmd2 = ARG_GET(a);
                if ((subcmd2 != NULL) && sal_strcasecmp(subcmd2, "perf") == 0) {
                    return _l3_cmd_route_perf_test(unit, a);
                } else {
                    return CMD_USAGE;
                }
            default:
                return CMD_USAGE;
        }
        return CMD_OK;
    }

#endif /* BCM_XGS3_SWITCH_SUPPORT */

    return CMD_USAGE;
}
#endif /* INCLUDE_L3 */
