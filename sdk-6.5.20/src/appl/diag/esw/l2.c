/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * L2 CLI commands
 */

#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <appl/diag/dport.h>
#include <shared/bsl.h>
#include <soc/l2x.h>
#include <soc/l2u.h>
#include <soc/debug.h>
#include <soc/hash.h>

#include <bcm/error.h>
#include <bcm/l2.h>
#include <bcm/stack.h>
#include <bcm/mcast.h>
#include <bcm/debug.h>

#include <bcm_int/esw/trunk.h>
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/hercules.h>
#include <bcm_int/esw/firebolt.h>
#if defined(BCM_TRIUMPH_SUPPORT) /* BCM_TRIUMPH_SUPPORT*/
#include <bcm_int/esw/triumph.h>
#endif /* !BCM_TRIUMPH_SUPPORT */
#if defined(BCM_TOMAHAWK_SUPPORT)
#include <bcm_int/esw/switch.h>
#endif
#if defined(BCM_TOMAHAWK3_SUPPORT)
#include <soc/tomahawk3.h>
#endif

static sal_usecs_t prev_l2x_interval[SOC_MAX_NUM_DEVICES];
#if defined(BCM_TOMAHAWK3_SUPPORT)
static sal_usecs_t prev_l2x_lrn_interval[SOC_MAX_NUM_DEVICES];
#endif

#if defined(BCM_TRIUMPH_SUPPORT) 
/*
 * Macro:
 *     L2_CHECK_RETURN
 * Purpose:
 *     Check the return value from an API call. Output either a failed
 *     message or okay along with the function name.
 */
#define L2_CHECK_RETURN(unit, rv, func_name)                            \
            do {                                                        \
                if (BCM_FAILURE(rv)) {                                  \
                    cli_out("L2(unit %d) Error: %s() failed - %s. \n",  \
                            (unit), (func_name), bcm_errmsg(rv));       \
                    return (CMD_FAIL);                                  \
                } else {                                                \
                    if (LOG_CHECK(BSL_LS_BCM_L2 | BSL_VERBOSE)) {       \
                        cli_out("L2(unit %d) Info: %s() success. \n",   \
                                (unit), (func_name));                   \
                    }                                                   \
                }                                                       \
            } while(0)
#endif


#if defined(BCM_TRIDENT2_SUPPORT)
#define L2_MAX_BANKS  6
#endif

/*
 * Used with the 'l2 watch' command below.
 */
static void
_l2_watch_cb(int unit,
	     bcm_l2_addr_t *l2addr,
	     int insert,
	     void *userdata)
{
    int s;
    sal_thread_t main_thread;

    s = sal_splhi();

    main_thread = sal_thread_main_get();
    sal_thread_main_set(sal_thread_self());

    if(insert) {
	dump_l2_addr(unit, "L2 ADD: ", l2addr);
    }
    else {
	dump_l2_addr(unit, "L2 DEL: ", l2addr);
    }

    sal_thread_main_set(main_thread);

    sal_spl(s);
}


/* CB function to print L2 Entry */
int _l2addr_dump(int unit, bcm_l2_addr_t *l2addr, void *user_data)
{
    /* Don't dump individual entries when just the 'count' is desired, the
       user of this callback will print the counts */
#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_ism_memory)) {
        if(!(_l2_addr_counts.flags & L2_DUMP_COUNT)) {
            dump_l2_addr(unit, "", l2addr);
        }
    } else {
        dump_l2_addr(unit, "", l2addr);
    }
#else
    dump_l2_addr(unit, "", l2addr);
#endif

    return BCM_E_NONE;
}


#if defined(BCM_TRIUMPH_SUPPORT) /* BCM_TRIUMPH_SUPPORT */
/*
 * Function:
 *    _l2_station_add
 * Purpose:
 *    Add L2 station entry.
 * Parmameters:
 *    unit - (IN) BCM device number
 *    args - (IN) input parameters passed as arguments
 * Returns:
 *     CMD_XXX
 */
STATIC int
_l2_station_add(int unit, args_t *args)
{
    int rv;
    int sid = -1;
    int prio = 0;
    bcm_mac_t dst_mac;
    bcm_mac_t dst_mac_mask;
    int vlan = 0;
    int vlan_mask = 0;
    int src_port = 0;
    int src_port_mask = 0;
    int ipv4 = 0;
    int ipv6 = 0;
    int arp_rarp = 0;
    int mpls_term = 0;
    int mim_term = 0;
    int trill_term = 0;
    int fcoe_term = 0;
    int oam_term = 0;
    int replace = 0;
    int copy_to_cpu = 0;
    int discard = 0;
    parse_table_t pt;
    bcm_l2_station_t s_ent;
    int station_id = -1;
    cmd_result_t retCode = CMD_OK;

    sal_memset(dst_mac, 0x00, sizeof(bcm_mac_t));
    sal_memset(dst_mac_mask, 0x00, sizeof(bcm_mac_t));

    bcm_l2_station_t_init(&s_ent);

    parse_table_init(unit, &pt);

    parse_table_add(&pt, "Priority", PQ_DFL|PQ_INT,
                    0, (void *)&prio,  NULL);

    parse_table_add(&pt, "ID", PQ_DFL|PQ_INT,
                    (void *)-1, (void *)&sid,  NULL);

    parse_table_add(&pt, "MACaddress", PQ_DFL|PQ_MAC,
                    0, (void *)&dst_mac, NULL);

    parse_table_add(&pt, "MACaddressMask", PQ_DFL|PQ_MAC,
                    0, (void *)&dst_mac_mask, NULL);

    parse_table_add(&pt, "Vlanid", PQ_DFL|PQ_HEX,
                    0, (void *)&vlan, NULL);

    parse_table_add(&pt, "VlanidMask", PQ_DFL|PQ_HEX,
                    0, (void *)&vlan_mask, NULL);

    parse_table_add(&pt, "SourcePort", PQ_DFL|PQ_PORT|PQ_BCM,
                    0, (void *)&src_port, NULL);

    parse_table_add(&pt, "SourcePortMask", PQ_DFL|PQ_HEX,
                    0, (void *)&src_port_mask, NULL);

    parse_table_add(&pt, "IPv4", PQ_DFL|PQ_BOOL,
                    0, (void *)&ipv4, NULL);

    parse_table_add(&pt, "IPv6", PQ_DFL|PQ_BOOL,
                    0, (void *)&ipv6, NULL);

    parse_table_add(&pt, "ArpRarp", PQ_DFL|PQ_BOOL,
                    0, (void *)&arp_rarp, NULL);

    parse_table_add(&pt, "MPLS", PQ_DFL|PQ_BOOL,
                    0, (void *)&mpls_term, NULL);

    parse_table_add(&pt, "MiM", PQ_DFL|PQ_BOOL,
                    0, (void *)&mim_term, NULL);

    parse_table_add(&pt, "TRILL", PQ_DFL|PQ_BOOL,
                    0, (void *)&trill_term, NULL);

    parse_table_add(&pt, "FCoE", PQ_DFL|PQ_BOOL,
                    0, (void *)&fcoe_term, NULL);

    parse_table_add(&pt, "OAM", PQ_DFL|PQ_BOOL,
                    0, (void *)&oam_term, NULL);

    parse_table_add(&pt, "Replace", PQ_DFL|PQ_BOOL,
                    0, (void *)&replace, NULL);

    parse_table_add(&pt, "CPUmirror", PQ_DFL|PQ_BOOL,
                    0, (void *)&copy_to_cpu, NULL);

    parse_table_add(&pt, "Discard", PQ_DFL|PQ_BOOL,
                    0, (void *)&discard, NULL);

    if (!parseEndOk(args, &pt, &retCode)) {
        return retCode;
    }

    s_ent.priority = prio;

    if (-1 != sid) {
        station_id = sid;
        s_ent.flags |= BCM_L2_STATION_WITH_ID;
    }
    sal_memcpy(s_ent.dst_mac, dst_mac, sizeof(bcm_mac_t));
    sal_memcpy(s_ent.dst_mac_mask, dst_mac_mask, sizeof(bcm_mac_t));

    s_ent.vlan = vlan;
    s_ent.vlan_mask = vlan_mask;

    s_ent.src_port = src_port;
    s_ent.src_port_mask = src_port_mask;

    if (1 == ipv4) {
        s_ent.flags |= BCM_L2_STATION_IPV4;
    }

    if (1 == ipv6) {
        s_ent.flags |= BCM_L2_STATION_IPV6;
    }

    if (1 == arp_rarp) {
        s_ent.flags |= BCM_L2_STATION_ARP_RARP;
    }

    if (1 == mpls_term) {
        s_ent.flags |= BCM_L2_STATION_MPLS;
    }

    if (1 == mim_term) {
        s_ent.flags |= BCM_L2_STATION_MIM;
    }

    if (1 == trill_term) {
        s_ent.flags |= BCM_L2_STATION_TRILL;
    }

    if (1 == oam_term) {
        s_ent.flags |= BCM_L2_STATION_OAM;
    }

    if (1 == fcoe_term) {
        s_ent.flags |= BCM_L2_STATION_FCOE;
    }

    if (1 == replace) {
        s_ent.flags |= BCM_L2_STATION_REPLACE;
    }

    if (1 == copy_to_cpu) {
        s_ent.flags |= BCM_L2_STATION_COPY_TO_CPU;
    }

    if (1 == discard) {
        s_ent.flags |= BCM_L2_STATION_DISCARD;
    }

    rv =  bcm_l2_station_add(unit, &station_id, &s_ent);
    L2_CHECK_RETURN(unit, rv, "bcm_l2_station_add");

    if (-1 == sid) {
        cli_out("Created SID=0x%08x\n", station_id);
    }

    return (retCode);
}

/*
 * Function:
 *    _l2_station_delete
 * Purpose:
 *    Delete L2 station entry.
 * Parmameters:
 *    unit - (IN) BCM device number
 *    args - (IN) input parameters passed as arguments
 * Returns:
 *     CMD_XXX
 */
STATIC int
_l2_station_delete(int unit, args_t *args)
{
    int rv;
    parse_table_t pt;
    int station_id = -1;
    cmd_result_t retCode = CMD_OK;

    parse_table_init(unit, &pt);

    parse_table_add(&pt, "ID", PQ_DFL|PQ_INT,
                    (void *)-1, (void *)&station_id,  NULL);
    if (!parseEndOk(args, &pt, &retCode)) {
        return retCode;
    }

    rv =  bcm_l2_station_delete(unit, station_id);

    L2_CHECK_RETURN(unit, rv, "bcm_l2_station_delete");

    return (retCode);
}

/*
 * Function:
 *    _l2_station_clear
 * Purpose:
 *    Delete all L2 station entries.
 * Parmameters:
 *    unit - (IN) BCM device number
 *    args - (IN) input parameters passed as arguments
 * Returns:
 *     CMD_XXX
 */
STATIC int
_l2_station_clear(int unit, args_t *args)
{
    cmd_result_t retCode = CMD_OK;
    int rv;

    rv =  bcm_l2_station_delete_all(unit);
    L2_CHECK_RETURN(unit, rv, "bcm_l2_station_delete_all");

    return (retCode);
}

/*
 * Function:
 *    _l2_station_show
 * Purpose:
 *    Show L2 station entry value.
 * Parmameters:
 *    unit - (IN) BCM device number
 *    args - (IN) input parameters passed as arguments
 * Returns:
 *     CMD_XXX
 */
STATIC int
_l2_station_show(int unit, args_t *args)
{
    int rv;
    parse_table_t pt;
    int station_id = -1;
    cmd_result_t retCode = CMD_OK;
    bcm_l2_station_t s_ent;

    parse_table_init(unit, &pt);

    parse_table_add(&pt, "ID", PQ_DFL|PQ_INT,
                    (void *)-1, (void *)&station_id,  NULL);
    if (!parseEndOk(args, &pt, &retCode)) {
        return retCode;
    }

    bcm_l2_station_t_init(&s_ent);
 
    rv = bcm_l2_station_get(unit, station_id, &s_ent);
    L2_CHECK_RETURN(unit, rv, "bcm_l2_station_get");

    cli_out("\tSID=0x%08x, priority=%d\n", station_id, s_ent.priority);
    cli_out("\tMAC=%x:%x:%x:%x:%x:%x\n", s_ent.dst_mac[0], s_ent.dst_mac[1],
            s_ent.dst_mac[2], s_ent.dst_mac[3], s_ent.dst_mac[4],
            s_ent.dst_mac[5]);
    cli_out("\tMAC MASK=%x:%x:%x:%x:%x:%x\n", s_ent.dst_mac_mask[0],
            s_ent.dst_mac_mask[1], s_ent.dst_mac_mask[2], s_ent.dst_mac_mask[3],
            s_ent.dst_mac_mask[4], s_ent.dst_mac_mask[5]);
    cli_out("\tVLAN=0x%x\n", s_ent.vlan);
    cli_out("\tVLAN MASK=0x%x\n", s_ent.vlan_mask);
    cli_out("\tSourcePort=0x%x\n", s_ent.src_port);
    cli_out("\tSourcePort MASK=0x%x\n", s_ent.src_port_mask);
    cli_out("\tFlags=0x%x\n", s_ent.flags);

    return (rv);
}

/*
 * Function:
 *    _l2_station_cmd_process
 * Purpose:
 *    Add/Delete/Clear/Show L2 station entry values.
 * Parmameters:
 *    unit - (IN) BCM device number
 *    args - (IN) input parameters passed as arguments
 * Returns:
 *     CMD_XXX
 */
STATIC int
_l2_station_cmd_process(int unit, args_t *args)
{
    char *subcmd = NULL;

    if ((subcmd = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }

    /* BCM.0> l2 station add ... */
    if (!sal_strcasecmp(subcmd, "add")) {
        return _l2_station_add(unit, args);
    }

    /* BCM.0> l2 station delete ... */
    if (!sal_strcasecmp(subcmd, "delete")) {
        return _l2_station_delete(unit, args);
    }

    /* BCM.0> l2 station clear ... */
    if (!sal_strcasecmp(subcmd, "clear")) {
        return _l2_station_clear(unit, args);
    }

    /* BCM.0> l2 station show ... */
    if (!sal_strcasecmp(subcmd, "show")) {
        return _l2_station_show(unit, args);
    }

    return CMD_USAGE;
}
#endif /* !BCM_TRIUMPH_SUPPORT */

/*
 * Add an L2 address to the switch, remove an L2 address from the
 * switch, show addresses, and clear addresses on a per port basis.
 */
cmd_result_t
if_esw_l2(int unit, args_t *a)
{
    int                 idx;
    char 		*subcmd = NULL;
    pbmp_t 	arg_pbmp, arg_pbmp_mb;
    int		arg_static = 0, arg_trunk = 0, arg_l3if = 0,
	arg_scp = 0, arg_ds = 0, arg_dd = 0, arg_count = 1, arg_modid = 0,
    arg_newmodid = 0, arg_vlan = VLAN_ID_DEFAULT, arg_tgid = 0, arg_newtgid = 0, 
    arg_cbit = 0, arg_hit = 0, arg_replace = 0, arg_port = 0, arg_newport = 0, 
        arg_group = 0, arg_pending = 0, arg_limitexempt = 0, arg_mirror = 0;
    int arg_mesh = 0;
#ifdef BCM_XGS_SWITCH_SUPPORT
    int          arg_hash = XGS_HASH_COUNT;
#endif
    int          arg_rpe = TRUE;
    soc_port_t		p, dport;
    int			rv = BCM_E_NONE;
    parse_table_t	pt;
    cmd_result_t	retCode;
    bcm_l2_addr_t	l2addr;
    uint32              flags;
    bcm_module_t    mymodid;
    /*
     * Initialize MAC address field for the user to the first real
     * address which does not conflict
     */

    sal_mac_addr_t default_macaddr = {0, 0, 0, 0, 0, 0X1};
    sal_mac_addr_t arg_macaddr = {0, 0, 0, 0, 0, 0X1};

    BCM_PBMP_CLEAR(arg_pbmp);
    BCM_PBMP_CLEAR(arg_pbmp_mb);
    sal_memset(&l2addr, 0, sizeof(bcm_l2_addr_t));

    UNSUPPORTED_COMMAND(unit, SOC_CHIP_BCM5670, a);


    if ((subcmd = ARG_GET(a)) == NULL) {
	return CMD_USAGE;
    }

    /* Check valid device to operation on ...*/
    if (!sh_check_attached(ARG_CMD(a), unit)) {
	return CMD_FAIL;
    }

    if (!sal_strcasecmp(subcmd, "replace")) {
        uint32 flags = 0;

        arg_modid = arg_port = arg_vlan = arg_tgid = arg_newmodid = arg_newport = arg_newtgid = -1;
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "Module",          PQ_DFL|PQ_INT,
                0, &arg_modid,  NULL);
        parse_table_add(&pt, "Port",            PQ_DFL|PQ_PORT,
                0, &arg_port,   NULL);
        parse_table_add(&pt, "MACaddress", 	    PQ_DFL|PQ_MAC,
                0, &arg_macaddr, NULL);
        parse_table_add(&pt, "Vlanid", 	        PQ_DFL|PQ_HEX,
                0, &arg_vlan,	NULL);
        parse_table_add(&pt, "Trunk",           PQ_DFL|PQ_BOOL,
                0, &arg_trunk,	NULL);
        parse_table_add(&pt, "TrunkGroupID",    PQ_DFL|PQ_INT,
                0, &arg_tgid,	NULL);
        parse_table_add(&pt, "STatic", 	        PQ_DFL|PQ_BOOL,
                0, &arg_static,	NULL);
        parse_table_add(&pt, "PENDing",	        PQ_DFL|PQ_BOOL,
                0, &arg_pending, NULL);
        parse_table_add(&pt, "NewModule",       PQ_DFL|PQ_INT,
                0, &arg_newmodid,  NULL);
        parse_table_add(&pt, "NewPort",         PQ_DFL|PQ_PORT,
                0, &arg_newport,   NULL);
        parse_table_add(&pt, "NewTrunkGroupID", PQ_DFL|PQ_INT,
                0, &arg_newtgid,	NULL);
        parse_table_add(&pt, "Mesh", PQ_DFL|PQ_BOOL,
                        0, &arg_mesh, NULL);
        if (!parseEndOk(a, &pt, &retCode)) {
            return retCode;
        }

        bcm_l2_addr_t_init(&l2addr, arg_macaddr, arg_vlan);
        if (arg_static) {
            flags |= BCM_L2_REPLACE_MATCH_STATIC;
        }
        if (arg_pending) {
            flags |= BCM_L2_REPLACE_PENDING;
        }
        if (ENET_CMP_MACADDR(arg_macaddr, default_macaddr)) {
            flags |= BCM_L2_REPLACE_MATCH_MAC;
        }
        if (arg_vlan != -1) {
            flags |= BCM_L2_REPLACE_MATCH_VLAN;
        }
        if (arg_newtgid != - 1) {
            flags |= BCM_L2_REPLACE_NEW_TRUNK; 
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
        if (arg_mesh) {
            l2addr.flags2 |= BCM_L2_FLAGS2_MESH;
        }

        rv = bcm_l2_replace(unit, flags, &l2addr, arg_newmodid, 
                                arg_newport, arg_newtgid); 

        if (BCM_FAILURE(rv)) {
            cli_out("%s ERROR: %s\n", ARG_CMD(a), bcm_errmsg(rv));
            return CMD_FAIL;
        }

        return CMD_OK;
    } else if (!sal_strcasecmp(subcmd, "add") ||
	!sal_strcasecmp(subcmd, "+")) {

	parse_table_init(unit, &pt);
	parse_table_add(&pt, "Module",          PQ_DFL|PQ_INT,
			0, &arg_modid,  NULL);
        parse_table_add(&pt, "Port",          PQ_DFL|PQ_PORT,
                        0, &arg_port,  NULL);
	parse_table_add(&pt, "PortBitMap", 	PQ_DFL|PQ_PBMP,
			(void *)(0), &arg_pbmp, NULL);
    	parse_table_add(&pt, "MACaddress", 	PQ_DFL|PQ_MAC,0, 
                        &arg_macaddr,   NULL);
	parse_table_add(&pt, "Vlanid", 	        PQ_DFL|PQ_HEX,
			0, &arg_vlan,	NULL);
	parse_table_add(&pt, "Trunk",           PQ_DFL|PQ_BOOL,
			0, &arg_trunk,	NULL);
	parse_table_add(&pt, "TrunkGroupID", 	PQ_DFL|PQ_INT,
			0, &arg_tgid,	NULL);
	parse_table_add(&pt, "STatic", 	        PQ_DFL|PQ_BOOL,
			0, &arg_static,	NULL);
	parse_table_add(&pt, "HIT", 	        PQ_DFL|PQ_BOOL,
			0, &arg_hit,	NULL);
	parse_table_add(&pt, "Replace",         PQ_DFL|PQ_BOOL,
                        0, &arg_replace,    NULL);
	parse_table_add(&pt, "SourceCosPriority",	PQ_DFL|PQ_BOOL,
			0, &arg_scp,	NULL);
	parse_table_add(&pt, "DiscardSource", 	PQ_DFL|PQ_BOOL,
			0, &arg_ds,	NULL);
	parse_table_add(&pt, "DiscardDest",PQ_DFL|PQ_BOOL,
			0, &arg_dd,	NULL);
	parse_table_add(&pt, "L3", 			PQ_DFL|PQ_BOOL,
			0, &arg_l3if,	NULL);
	parse_table_add(&pt, "CPUmirror", 	PQ_DFL|PQ_BOOL,
			0, &arg_cbit,	NULL);
        parse_table_add(&pt, "MIRror", 	        PQ_DFL|PQ_BOOL,
                        0, &arg_mirror,	NULL);
	parse_table_add(&pt, "MacBlockPortBitMap", PQ_DFL|PQ_PBMP|PQ_BCM,
			(void *)(0), &arg_pbmp_mb, NULL);
	parse_table_add(&pt, "Group",          PQ_DFL|PQ_INT,
			0, &arg_group,  NULL);
	parse_table_add(&pt, "PENDing",	        PQ_DFL|PQ_BOOL,
			0, &arg_pending, NULL);
	parse_table_add(&pt, "LearnLimitExempt", PQ_DFL|PQ_BOOL,
			0, &arg_limitexempt, NULL);

        if (SOC_IS_XGS3_SWITCH(unit)) {
            parse_table_add(&pt, "ReplacePriority", 	PQ_DFL|PQ_BOOL,
                            0, &arg_rpe,	NULL);
        }
        parse_table_add(&pt, "Mesh", PQ_DFL|PQ_BOOL,
                        0, &arg_mesh, NULL);
	if (!parseEndOk(a, &pt, &retCode)) {
	    return retCode;
	}
    

        /* Configure flags for SDK call */
        flags = 0;
        if (arg_static)
            flags |= BCM_L2_STATIC;
        if (arg_hit)
            flags |= BCM_L2_HIT;
        if (arg_replace)
            flags |= BCM_L2_REPLACE_DYNAMIC;
        if (arg_scp)
            flags |= BCM_L2_COS_SRC_PRI;
        if (arg_dd)
            flags |= BCM_L2_DISCARD_DST;
        if (arg_ds)
            flags |= BCM_L2_DISCARD_SRC;
        if (arg_l3if)
            flags |= BCM_L2_L3LOOKUP;
        if (arg_trunk)
            flags |= BCM_L2_TRUNK_MEMBER;
        if (arg_cbit)
            flags |= BCM_L2_COPY_TO_CPU;
        if (arg_mirror)
            flags |= BCM_L2_MIRROR;
        if (arg_rpe)
            flags |= BCM_L2_SETPRI;
        if (arg_pending)
            flags |= BCM_L2_PENDING;
        if (arg_limitexempt)
            flags |= BCM_L2_LEARN_LIMIT_EXEMPT;
    
        if (SOC_PBMP_IS_NULL(arg_pbmp)) {
            /* Fill l2addr structure */
            bcm_l2_addr_t_init(&l2addr, arg_macaddr, arg_vlan);
            l2addr.tgid = arg_tgid;
            l2addr.group = arg_group;
            l2addr.flags = flags;
            l2addr.port = arg_port;
            if (arg_mesh) {
                l2addr.flags2 |= BCM_L2_FLAGS2_MESH;
            }
            if (!BCM_GPORT_IS_SET(arg_port)) {
                l2addr.modid = arg_modid;
                bcm_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                   l2addr.modid, l2addr.port, 
                                   &l2addr.modid, &l2addr.port);
            }
            BCM_PBMP_ASSIGN(l2addr.block_bitmap, arg_pbmp_mb);
            if ((rv = bcm_l2_addr_add(unit, &l2addr))!= BCM_E_NONE) {
                cli_out("%s ERROR: %s\n", ARG_CMD(a), bcm_errmsg(rv));
                return CMD_FAIL;
            }
            dump_l2_addr(unit, "ADD: ", &l2addr);
        } else {
            /*
             * If we are setting the range, the MAC address is incremented by
             * 1 for each port.
             */
            /* coverity[overrun-local : FALSE] */
            DPORT_SOC_PBMP_ITER(unit, arg_pbmp, dport, p) {
                bcm_error_t rv;
                /* Fill l2addr structure */
                bcm_l2_addr_t_init(&l2addr, arg_macaddr, arg_vlan);
                l2addr.tgid = arg_tgid;
                l2addr.group = arg_group;
                l2addr.flags = flags;
                if (arg_mesh) {
                    l2addr.flags2 |= BCM_L2_FLAGS2_MESH;
                }
                SOC_IF_ERROR_RETURN(bcm_stk_my_modid_get(unit, &mymodid));

                /*
                 * Handle devices that don't support port stacking by
                 * ignoring BCM_E_UNAVAIL status
                 */
                rv = bcm_stk_modmap_map(unit, BCM_STK_MODMAP_GET, mymodid,
                                        p, &mymodid, &p);

                if ((BCM_FAILURE(rv)) && (rv != BCM_E_UNAVAIL)) {
                    cli_out("%s ERROR: bcm_stk_modmap_map returned %d(%s)\n",
                            ARG_CMD(a), rv, bcm_errmsg(rv));
                    return CMD_FAIL;
                }

                l2addr.port = p;
                l2addr.modid = mymodid;
                BCM_PBMP_ASSIGN(l2addr.block_bitmap, arg_pbmp_mb);
        	    
                if ((rv = bcm_l2_addr_add(unit, &l2addr))!= BCM_E_NONE) {
                    cli_out("%s ERROR: %s\n", ARG_CMD(a), bcm_errmsg(rv));
                    return CMD_FAIL;
                }

                dump_l2_addr(unit, "ADD: ", &l2addr);

                /* Set up for next call */
                increment_macaddr(arg_macaddr, 1);
            }
    	} 
    
	return CMD_OK;
    } else if (!sal_strcasecmp(subcmd, "del") ||
	     !sal_strcasecmp(subcmd, "-")) {

	parse_table_init(unit, &pt);
	parse_table_add(&pt, "MACaddress", 	PQ_DFL|PQ_MAC,
			0, &arg_macaddr,NULL);
	parse_table_add(&pt, "Count", 	PQ_DFL|PQ_INT,
			(void *)(1), &arg_count, NULL);
        parse_table_add(&pt, "Vlanid",  PQ_DFL|PQ_HEX,
			0, &arg_vlan,	NULL);
	if (!parseEndOk(a, &pt, &retCode)) {
	    return retCode;
	}

	for (idx = 0; idx < arg_count; idx++) {
            bcm_l2_addr_t_init(&l2addr, arg_macaddr, arg_vlan);
	    rv = bcm_l2_addr_get(unit, arg_macaddr, arg_vlan, &l2addr);
	    if (rv < 0) {
		cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(rv));
		return CMD_FAIL;
	    }

            if (LOG_CHECK(BSL_LS_BCM_L2 | BSL_VERBOSE)) {
                dump_l2_addr(unit, "DEL: ", &l2addr);
            }

	    if ((rv = bcm_l2_addr_delete(unit, arg_macaddr, arg_vlan)) < 0) {
		cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(rv));
		return CMD_FAIL;
	    }

	    increment_macaddr(arg_macaddr, 1);

        }
        arg_count = 1;
	return CMD_OK;
    }

    else if (!sal_strcasecmp(subcmd, "show") ||
             !sal_strcasecmp(subcmd, "-d") ||
             !sal_strcasecmp(subcmd, "count")) {

#ifdef BCM_XGS_SWITCH_SUPPORT
    if (SOC_IS_XGS_SWITCH(unit)) {

#if defined(BCM_TRIUMPH3_SUPPORT)
        if (soc_feature(unit, soc_feature_ism_memory)) {
            sal_memset(&_l2_addr_counts, 0, sizeof(_l2_addr_counts));
            if(!sal_strcasecmp(subcmd, "count")) {
                _l2_addr_counts.flags = L2_DUMP_COUNT;
            }
        } else if(!sal_strcasecmp(subcmd, "count")) {
            cli_out("ERROR: %s %s not implemented on this unit\n",
                    ARG_CMD(a), subcmd);
            return BCM_E_UNAVAIL;
        }
#else
        if(!sal_strcasecmp(subcmd, "count")) {
            cli_out("ERROR: %s %s not implemented on this unit\n",
                    ARG_CMD(a), subcmd);
            return BCM_E_UNAVAIL;
        }
#endif

        rv = bcm_l2_traverse(unit, _l2addr_dump, NULL);

        if (rv < 0) {
            cli_out("%s: ERROR: bcm_l2_traverse failed %s\n",
                    ARG_CMD(a), bcm_errmsg(rv));
        }

#if defined(BCM_TRIUMPH3_SUPPORT)
            if (soc_feature(unit, soc_feature_ism_memory)) {
                if(_l2_addr_counts.flags & L2_DUMP_COUNT) {
                    cli_out("\n L2 Counts:");
                    cli_out("\n  L2_ENTRY_1: %d",
                            _l2_addr_counts.l2_count.l2_entry_1);
                    cli_out("\n  L2_ENTRY_2: %d\n",
                            _l2_addr_counts.l2_count.l2_entry_2);
                    if ((soc_feature(unit, soc_feature_esm_support)) ) {
                        cli_out("\n  EXT_L2_ENTRY_1: %d",
                                _l2_addr_counts.l2_count.ext_l2_entry_1);
                        cli_out("\n  EXT_L2_ENTRY_1: %d\n\n",
                                _l2_addr_counts.l2_count.ext_l2_entry_2);
                    }
                }

                /* Reset the L2 counts structure for subsequent runs */
                sal_memset(&_l2_addr_counts, 0, sizeof(_l2_addr_counts));
            }
#endif

        return CMD_OK;

    }

#endif /* BCM_XGS3_SWITCH_SUPPORT */

	cli_out("ERROR: %s %s not implemented on this unit\n",
                ARG_CMD(a), subcmd);
	return CMD_FAIL;
    }

    else if (!sal_strcasecmp(subcmd, "check")) {
        int i, rv, min, max;
        l2x_entry_t entry;
        char *sscmd;
        soc_mem_t mem;
        soc_field_t fld;

        mem = INVALIDm;
        fld = INVALIDf;
        if ((sscmd = ARG_GET(a)) != NULL) {
            cli_out("Subcommands not supported\n");
            return CMD_USAGE;
        } else {
            if (SOC_IS_TRIUMPH3(unit)) {
                mem = L2_ENTRY_1m; 
            }else {   
               mem = L2Xm;
            } 
            fld = VALIDf;
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)
            if(SOC_MEM_FIELD_VALID(unit, mem, BASE_VALIDf)) {
                fld = BASE_VALIDf;
            }
#endif 
        }
        if (!SOC_IS_XGS_SWITCH(unit)) {
            cli_out("Command only valid for XGS switches\n");
            return CMD_FAIL;
        }

        if (!SOC_MEM_IS_VALID(unit, mem)) {
            cli_out("Error: Memory %s not valid for chip %s.\n",
                    SOC_MEM_UFNAME(unit, mem), SOC_UNIT_NAME(unit));
            return CMD_FAIL;
        }

        min = soc_mem_index_min(unit, mem);
        max = soc_mem_index_max(unit, mem);

        cli_out("Dumping %s entries.  From %d to %d.\n",
                sscmd, min, max);

        cli_out("Progress . marks 100 memory table entries.\n");
        for (i = min; i < max; i++) {
            if (!((i+1)%100)) {
                cli_out(".");
                if (!((i+1)%4000)) {
                    cli_out("\n");
                }
            }
            rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, i, &entry);
            if (SOC_SUCCESS(rv)) {
               if (soc_mem_field32_get(unit, mem, &entry, fld)) {
                  cli_out("Index %d (0x%x) has valid entries (H: %x, E: %d)\n",
                          i, i, i>>3, i&0x3);
                  soc_mem_entry_dump(unit, mem, &entry, BSL_LSS_CLI);
                  cli_out("\n");
               }
            }
        }
        cli_out("Done.\n");
    }

    else if (!sal_strcasecmp(subcmd, "clear") ||
	     !sal_strcasecmp(subcmd, "clr")) {
        char *static_str;
        int old_modid, old_port, old_vlan, old_tgid, old_static, old_pending;
        enum {
            MAC_PRESENT         = 0x00000001,
            MODID_PRESENT       = 0x00000002,
            PORT_PRESENT        = 0x00000004,
            VLANID_PRESENT      = 0x00000008,
            TGID_PRESENT        = 0x00000010
        } arg_combination;
        
	/*
	 * Clear the ARL on a per port or per switch basis. Walk ARL
	 * memory finding matching ports such as the user specified,
	 * issue BCM L2 SDK calls for clearing the entries.
	 */

        /* Save current arguments */
        old_modid = arg_modid;
	old_port = arg_port;
	old_vlan = arg_vlan;
	old_tgid = arg_tgid;
	old_static = arg_static;
	old_pending = arg_pending;

	arg_modid = -1;
	arg_port = -1;
	arg_vlan = -1;
	arg_tgid = -1;
	arg_static = TRUE;
	arg_pending = FALSE;
        ENET_SET_MACADDR(arg_macaddr, _soc_mac_all_zeroes);
        arg_combination = 0;

	parse_table_init(unit, &pt);
	parse_table_add(&pt, "Module",          PQ_DFL|PQ_INT,
			0, &arg_modid,  NULL);
	parse_table_add(&pt, "Port",            PQ_DFL|PQ_PORT,
			0, &arg_port,   NULL);
	parse_table_add(&pt, "Vlanid", 	        PQ_DFL|PQ_HEX,
			0, &arg_vlan,	NULL);
	parse_table_add(&pt, "TrunkGroupID", 	PQ_DFL|PQ_INT,
			0, &arg_tgid,	NULL);
        parse_table_add(&pt, "MACaddress",      PQ_DFL|PQ_MAC,
                        0, &arg_macaddr,NULL);
	parse_table_add(&pt, "STatic", 	        PQ_DFL|PQ_BOOL,
			0, &arg_static,	NULL);
	parse_table_add(&pt, "PENDing",         PQ_DFL|PQ_BOOL,
			0, &arg_pending, NULL);

        retCode = CMD_OK;

        if (!ARG_CNT(a)) {
            /*
             * Restore arguments for parseEndOk below to print
             * correct settings.
             */
            if (arg_modid == -1) arg_modid = old_modid;
            if (arg_port == -1) arg_port = old_port;
            if (arg_vlan == -1) arg_vlan = old_vlan;
            if (arg_tgid == -1) arg_tgid = old_tgid;
            if (arg_static == -1) arg_static = old_static;
            if (arg_pending == -1) arg_pending = old_pending;
        }

	if (!parseEndOk(a, &pt, &retCode)) {
	    goto done;
        }

        /*
         * Notice which arguments were supplied
         */
        if (arg_modid >=0) {
            arg_combination |= MODID_PRESENT;
        }

        if (arg_port >= 0) {
            arg_combination |= PORT_PRESENT;
        }

        if (ENET_CMP_MACADDR(arg_macaddr, _soc_mac_all_zeroes)) {
            arg_combination |= MAC_PRESENT;
        }

        if (arg_tgid >= 0) {
            arg_combination |= TGID_PRESENT;
        }

        if (arg_vlan >= 0) {
            arg_combination |= VLANID_PRESENT;
        }

        arg_static = arg_static ? BCM_L2_DELETE_STATIC : 0;
        if (arg_pending == TRUE) {
            static_str = arg_static ? "static and pending" : "pending";
            arg_static |= BCM_L2_DELETE_PENDING;
        } else {
            static_str = arg_static ? "static and non-static" : "non-static";
        }
        switch ((int)arg_combination) {
        case PORT_PRESENT:
	    cli_out("%s: Deleting %s addresses by port, local module ID\n",
                    ARG_CMD(a), static_str);
	    rv = bcm_l2_addr_delete_by_port(unit,
					    -1, arg_port,
					    arg_static);
            break;

        case PORT_PRESENT | MODID_PRESENT:
	    cli_out("%s: Deleting %s addresses by module/port\n",
                    ARG_CMD(a), static_str);
	    rv = bcm_l2_addr_delete_by_port(unit,
					    arg_modid, arg_port,
					    arg_static);
            break;

        case VLANID_PRESENT:
	    cli_out("%s: Deleting %s addresses by VLAN\n",
                    ARG_CMD(a), static_str);
	    rv = bcm_l2_addr_delete_by_vlan(unit,
					    arg_vlan,
					    arg_static);
            break;

        case TGID_PRESENT:
	    cli_out("%s: Deleting %s addresses by trunk ID\n",
                    ARG_CMD(a), static_str);
	    rv = bcm_l2_addr_delete_by_trunk(unit,
					     arg_tgid,
					     arg_static);

	    break;

        case MAC_PRESENT:
            cli_out("%s: Deleting %s addresses by MAC\n",
                    ARG_CMD(a), static_str);
            rv = bcm_l2_addr_delete_by_mac(unit, 
                                           arg_macaddr, 
                                           arg_static);
            break;

        case MAC_PRESENT | VLANID_PRESENT:
            cli_out("%s: Deleting an address by MAC and VLAN\n",
                    ARG_CMD(a));
            rv = bcm_l2_addr_delete(unit, 
                                    arg_macaddr, 
                                    arg_vlan);

            break;

        case MAC_PRESENT | PORT_PRESENT:
            cli_out("%s: Deleting %s addresses by MAC and port\n",
                    ARG_CMD(a), static_str);
            rv = bcm_l2_addr_delete_by_mac_port(unit, 
                                                arg_macaddr, 
                                                -1, arg_port,
                                                arg_static);
            break;

        case MAC_PRESENT | PORT_PRESENT | MODID_PRESENT:
            cli_out("%s: Deleting %s addresses by MAC and module/port\n",
                    ARG_CMD(a), static_str);
            rv = bcm_l2_addr_delete_by_mac_port(unit, 
                                                arg_macaddr, 
                                                arg_modid, arg_port,
                                                arg_static);
            break;

        case VLANID_PRESENT | PORT_PRESENT:
	    cli_out("%s: Deleting %s addresses by VLAN and port\n",
                    ARG_CMD(a), static_str);
	    rv = bcm_l2_addr_delete_by_vlan_port(unit,
						 arg_vlan,
						 -1, arg_port,
						 arg_static);
            break;

        case VLANID_PRESENT | PORT_PRESENT | MODID_PRESENT:
	    cli_out("%s: Deleting %s addresses by VLAN and module/port\n",
                    ARG_CMD(a), static_str);
	    rv = bcm_l2_addr_delete_by_vlan_port(unit,
						 arg_vlan,
						 arg_modid, arg_port,
						 arg_static);
            break;

        case VLANID_PRESENT | TGID_PRESENT:
            cli_out("%s: Deleting %s addresses by VLAN and trunk ID\n",
                    ARG_CMD(a), static_str);
            rv = bcm_l2_addr_delete_by_vlan_trunk(unit,
                                                  arg_vlan,
                                                  arg_tgid,
                                                  arg_static);
            break;

        default:
	    cli_out("%s: Unknown argument combination\n", ARG_CMD(a));
	    retCode = CMD_USAGE;
            break;
	}

done:
        /* Restore unused arguments */
        if (arg_modid == -1) arg_modid = old_modid;
	if (arg_port == -1) arg_port = old_port;
	if (arg_vlan == -1) arg_vlan = old_vlan;
	if (arg_tgid == -1) arg_tgid = old_tgid;
	if (arg_static == -1) arg_static = old_static;
	if (arg_pending == -1) arg_pending = old_pending;

        if ((retCode == CMD_OK) && (rv < 0)) {
            cli_out("ERROR: %s\n", bcm_errmsg(rv));
            return CMD_FAIL;
        }

	return retCode;
    }

    else if (!sal_strcasecmp(subcmd, "watch")) {
	static int watch = 0;
	char* opt = ARG_GET(a);
	if(opt == NULL) {
	    cli_out("L2 watch is%srunning.\n",
                    (watch) ? " " : " not ");
	    return CMD_OK;
	}
	else if(!sal_strcasecmp(opt, "start")) {
	    watch = 1;
	    bcm_l2_addr_register(unit, _l2_watch_cb, NULL);
	    return CMD_OK;
	}
	else if(!sal_strcasecmp(opt, "stop")) {
	    watch = 0;
	    bcm_l2_addr_unregister(unit, _l2_watch_cb, NULL);
	    return CMD_OK;
	}
	else {
	    return CMD_USAGE;
	}
    }

#ifdef BCM_XGS_SWITCH_SUPPORT
    else if (!sal_strcasecmp(subcmd, "conflict")) {
	bcm_l2_addr_t		addr;
	bcm_l2_addr_t		cf[SOC_L2X_BUCKET_SIZE];
	int			cf_count, i;

	parse_table_init(unit, &pt);
	parse_table_add(&pt, "MACaddress", 	PQ_DFL|PQ_MAC,
			0, &arg_macaddr,NULL);
	parse_table_add(&pt, "Vlanid", 	        PQ_DFL|PQ_HEX,
			0, &arg_vlan,	NULL);
	if (!parseEndOk(a, &pt, &retCode))
	    return retCode;

	bcm_l2_addr_t_init(&addr, arg_macaddr, arg_vlan);

	if ((rv = bcm_l2_conflict_get(unit, &addr,
				      cf, sizeof (cf) / sizeof (cf[0]),
				      &cf_count)) < 0) {
	    cli_out("%s: bcm_l2_conflict_get failed: %s\n",
                    ARG_CMD(a), bcm_errmsg(rv));
	    return CMD_FAIL;
	}

	for (i = 0; i < cf_count; i++) {
	    dump_l2_addr(unit, "conflict: ", &cf[i]);
	}

	return CMD_OK;
    }

    else if (!sal_strcasecmp(subcmd, "hash") ||
	     !sal_strcasecmp(subcmd, "-h") ) {
	int hash_select;
	int hash_bucket = 0;
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT) || \
    defined(BCM_TOMAHAWK_SUPPORT)
    int num_l2_banks                  = 0;
    int bank                          = 0;
    int l2_index_array[L2_MAX_BANKS]  = {0};
    int l2_bucket_array[L2_MAX_BANKS] = {0};
#endif
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAVEN_SUPPORT) || \
        defined(BCM_TRX_SUPPORT)
	int hash_select2 = -1;
	int hash_bucket2 = -1;
#endif
	uint32 regval;
	uint8 key[XGS_HASH_KEY_SIZE];

	parse_table_init(unit, &pt);
	parse_table_add(&pt, "MACaddress", 	PQ_DFL|PQ_MAC,
			0, &arg_macaddr,NULL);
	parse_table_add(&pt, "Vlanid", 	        PQ_DFL|PQ_HEX,
			0, &arg_vlan,	NULL);
	parse_table_add(&pt, "Hash", 	PQ_DFL|PQ_INT,
			(void *)(0), &arg_hash, NULL);
	if (!parseEndOk(a, &pt, &retCode))
	    return retCode;

	hash_select = arg_hash;
#ifdef BCM_XGS3_SWITCH_SUPPORT
        if (SOC_IS_XGS3_SWITCH(unit)) {
#ifdef BCM_ISM_SUPPORT
            if (soc_feature(unit, soc_feature_ism_memory)) {
                int b, rc, index;
                l2_entry_1_entry_t entry;
                soc_field_t keyflds[MAX_FIELDS], lsbfld;
                uint32 bucket, sizes[_SOC_ISM_MAX_BANKS];
                uint8 act_zero_lsb, act_offset, zero_lsb = 0, offset = 48;
                uint8 banks[_SOC_ISM_MAX_BANKS], bcount, num_flds;
                
                sal_memset(&entry, 0, sizeof(entry));
                soc_L2_ENTRY_1m_field32_set(unit, &entry, VALIDf, 1);
                soc_L2_ENTRY_1m_field32_set(unit, &entry, VLAN_IDf, arg_vlan);
                soc_L2_ENTRY_1m_mac_addr_set(unit, &entry, MAC_ADDRf, arg_macaddr);
                
                rc = soc_ism_get_banks_for_mem(unit, L2_ENTRY_1m, banks, sizes, &bcount);
                if (rc || !bcount) {
                    cli_out("Error retreiving bank info: Test aborted.\n");
                    return CMD_FAIL;
                }
                rc = soc_generic_get_hash_key(unit, L2_ENTRY_1m, &entry, keyflds, &lsbfld,
                                              &num_flds);
                if (rc) {
                    cli_out("Error constructing key: Test aborted.\n");
                    return CMD_FAIL;
                }
                sal_memset(key, 0, sizeof(key));
                soc_ism_gen_key_from_keyfields(unit, L2_ENTRY_1m, &entry, keyflds, key, num_flds);
                cli_out("Hash[%d] of key "
                        "0x%02x%02x%02x%02x%02x%02x%02x%02x is \n", hash_select,
                        key[7], key[6], key[5], key[4],
                        key[3], key[2], key[1], key[0]);
                for (b = 0; b < bcount; b++) {
                    rc = soc_ism_mem_hash_config_get(unit, L2_ENTRY_1m, &act_zero_lsb);
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
                    case XGS_HASH_CRC16_UPPER: offset = 48 - soc_ism_get_hash_bits(sizes[b]);
                        break;
                    case XGS_HASH_CRC16_LOWER: offset = 32;
                        break;
                    case XGS_HASH_LSB: offset = 48; zero_lsb = 1;
                        break;
                    case XGS_HASH_ZERO:
                        break;
                    case XGS_HASH_CRC32_UPPER: offset = 32 - soc_ism_get_hash_bits(sizes[b]);
                        break;
                    case XGS_HASH_CRC32_LOWER: offset = 0;
                        break;
                    case XGS_HASH_COUNT: offset = act_offset; zero_lsb = act_zero_lsb;
                        break;
                    default: if (hash_select > 63) {
                                 cli_out("Invalid hash offset value !!\n");
                                 return CMD_FAIL;
                             }
                        offset = hash_select; if (hash_select >= 48) { zero_lsb = 1; }
                        break;
                    }
                    /* get config */
                    if (zero_lsb != act_zero_lsb) {
                        rc = soc_ism_mem_hash_config(unit, L2_ENTRY_1m, zero_lsb);
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
                    rc = soc_generic_hash(unit, L2_ENTRY_1m, &entry, 1 << banks[b], 0,
                                          &index, NULL, &bucket, NULL);
                    if (rc) {
                        cli_out("Error calculating hash: Test aborted.\n");
                        return CMD_FAIL;
                    }
                    cli_out("\tBank[%d] num: %d, bucket: %d, index: %d\n", b, 
                            banks[b], bucket, index);
                    /* restore config */
                    if (zero_lsb != act_zero_lsb) {
                        rc = soc_ism_mem_hash_config(unit, L2_ENTRY_1m, act_zero_lsb);
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
	    if (
#if defined(BCM_TOMAHAWK3_SUPPORT)
	       (!SOC_IS_TOMAHAWK3(unit)) &&
#endif /* BCM_TOMAHAWK3_SUPPORT */
	       (hash_select == XGS_HASH_COUNT)) {
	        /* Get the hash selection from hardware */
	        if ( (rv = READ_HASH_CONTROLr(unit, &regval)) < 0) {
		    cli_out("%s: ERROR: %s\n", ARG_CMD(a), soc_errmsg(rv));
		    return CMD_FAIL;
	        }
	        hash_select = soc_reg_field_get(unit, HASH_CONTROLr, regval,
					        L2_AND_VLAN_MAC_HASH_SELECTf);
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAVEN_SUPPORT) || \
        defined(BCM_TRX_SUPPORT)
                if (soc_feature(unit, soc_feature_dual_hash)) {
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT) || \
    defined(BCM_TOMAHAWK_SUPPORT)
                    if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TOMAHAWKX(unit)) {
                        if ( (rv = READ_L2_TABLE_HASH_CONTROLr(unit,
                                                             &regval)) < 0) {
                            cli_out("%s: ERROR: %s\n", ARG_CMD(a),
                                    soc_errmsg(rv));
                            return CMD_FAIL;
                        }
                        hash_select2 =
                            soc_reg_field_get(unit, L2_TABLE_HASH_CONTROLr,
                                              regval, HASH_ZERO_OR_LSBf);
                        if (0 == hash_select2) {
                            hash_select2 = FB_HASH_ZERO;
                        } else if (1 == hash_select2) {
                            hash_select2 = FB_HASH_LSB;
                        }
                    } else 
#endif
                    {
                        if ( (rv = READ_L2_AUX_HASH_CONTROLr(unit,
                                                             &regval)) < 0) {
                            cli_out("%s: ERROR: %s\n", ARG_CMD(a),
                                    soc_errmsg(rv));
                            return CMD_FAIL;
                        }
                        hash_select2 =
                            soc_reg_field_get(unit, L2_AUX_HASH_CONTROLr,
                                              regval, HASH_SELECTf);
                    }
                }
#endif
	    }

#ifdef BCM_TRX_SUPPORT
            if (SOC_IS_TRX(unit)) {
                int bits;
                l2x_entry_t entry;

                sal_memset(&entry, 0, sizeof(entry));
                soc_L2Xm_field32_set(unit, &entry, VLAN_IDf, arg_vlan);
                soc_L2Xm_mac_addr_set(unit, &entry, MAC_ADDRf, arg_macaddr);
            bits = soc_tr_l2x_base_entry_to_key(unit, &entry, key);
#if defined(BCM_TRIDENT2PLUS_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT) ||   \
    defined(BCM_TRIDENT2_SUPPORT)
            if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TOMAHAWKX(unit)) {
                if ((rv = soc_td2x_th_l2x_hash(unit, (uint32 *)&entry,
                                               &num_l2_banks, l2_bucket_array,
                                               l2_index_array, key)) < 0) {
                    cli_out("%s ERROR: %s\n", ARG_CMD(a), soc_errmsg(rv));
                    return CMD_FAIL;
                }

                if (num_l2_banks > L2_MAX_BANKS) {
                    cli_out("%s ERROR: the number of banks overflow\n",
                            ARG_CMD(a));
                    return CMD_FAIL;
                }
            } else
#endif
            {
                hash_bucket = soc_tr_l2x_hash(unit, hash_select, bits, &entry,
                                                  key);
            }
                if (hash_select2 >= 0) {
                    hash_bucket2 =
                        soc_tr_l2x_hash(unit, hash_select2, bits, &entry, key);
                }
            } else
#endif
#ifdef BCM_FIREBOLT_SUPPORT
            if (SOC_IS_FBX(unit)) {
	        soc_draco_l2x_param_to_key(arg_macaddr, arg_vlan, key);
	        hash_bucket = soc_fb_l2_hash(unit, hash_select, key);
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAVEN_SUPPORT)
                if (hash_select2 >= 0) {
                    hash_bucket2 = soc_fb_l2_hash(unit, hash_select2, key);
                }
#endif
            } else
#endif
            {
	        return CMD_FAIL;
            } 
        }
#endif

#if defined(BCM_TRIDENT2PLUS_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT) || \
    defined(BCM_TOMAHAWK_SUPPORT)
    if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TOMAHAWKX(unit)) {
        cli_out("Hash[%d] of key "
                "0x%02x%02x%02x%02x%02x%02x%02x%01x:\n",
                hash_select,
                key[7], key[6], key[5], key[4],
                key[3], key[2], key[1], (key[0] >> 4) & 0xf);

        for (bank = 0; bank < num_l2_banks; bank++) {
            cli_out("bank%d bucket 0x%06x (%06d) index 0x%06x (%06d)\n",
                     bank, l2_bucket_array[bank], l2_bucket_array[bank],
                     l2_index_array[bank], l2_index_array[bank]);
          
        }

    } else
#endif
    {
        cli_out("Hash[%d] of key "
                    "0x%02x%02x%02x%02x%02x%02x%02x%01x "
                    "is bucket 0x%03x (%d)\n", hash_select,
                    key[7], key[6], key[5], key[4],
                    key[3], key[2], key[1], (key[0] >> 4) & 0xf,
                    hash_bucket, hash_bucket);
    }

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAVEN_SUPPORT) || \
        defined(BCM_TRX_SUPPORT)
        if (hash_select2 >= 0) {
            cli_out("Dual hash[%d] of key "
                    "0x%02x%02x%02x%02x%02x%02x%02x%01x "
                    "is bucket 0x%03x (%d)\n", hash_select2,
                    key[7], key[6], key[5], key[4],
                    key[3], key[2], key[1], (key[0] >> 4) & 0xf,
                    hash_bucket2, hash_bucket2);
        }
#endif
    } else if (!sal_strcasecmp(subcmd, "info") ||
               !sal_strcasecmp(subcmd, "-i")) {
        if (SOC_IS_XGS_SWITCH(unit)) {
            int output[2];
            struct resource
            {
                char *name;
                bcm_switch_object_t cur_max[2];
            };

            struct resource l2_resources[] =
            {{"mac entries", {bcmSwitchObjectL2EntryCurrent,
             bcmSwitchObjectL2EntryMax}}};

            rv = bcm_switch_object_count_multi_get(
                    unit, 2, l2_resources[0].cur_max, output);

            if (rv < 0)
            {
                cli_out("%s: ERROR: Get L2 info failed %s\n",
                        ARG_CMD(a), bcm_errmsg(rv));
                return CMD_FAIL;
            } else {
                cli_out("%-16s : %6d / %6d\n", "mac entries",
                        output[0], output[1]);
            }
            return CMD_OK;
        }
        cli_out("ERROR: %s %s not implemented on this unit\n",
                ARG_CMD(a), subcmd);
        return CMD_FAIL;
    }
#endif /* BCM_XGS_SWITCH_SUPPORT */

#ifdef BCM_XGS3_SWITCH_SUPPORT
    else if (!sal_strcasecmp(subcmd, "cache") ||
	     !sal_strcasecmp(subcmd, "c") ) {
        int cidx;
        char *cachecmd = NULL;
        char str[16];
        sal_mac_addr_t arg_macaddr, arg_macaddr_mask;
        int arg_setpri = 0, arg_bpdu = 0;
        int arg_prio = -1, arg_lrndis = FALSE, arg_tunnel = FALSE;
        int arg_vlan_mask, arg_cidx, arg_ccount, arg_reinit, idx_max;
        int arg_sport = -1, arg_sport_mask;
        int arg_lookup_class = 0;
        int arg_protocolPkt = 0;
        bcm_l2_cache_addr_t l2caddr;

        if ((cachecmd = ARG_GET(a)) == NULL) {
            return CMD_USAGE;
        }

        /* Masks always default to full mask */
        arg_vlan_mask = BCM_L2_VID_MASK_ALL;
        arg_sport_mask = BCM_L2_SRCPORT_MASK_ALL;
        sal_memset(arg_macaddr, 0, sizeof (sal_mac_addr_t));
        sal_memset(arg_macaddr_mask, 0xff, sizeof (sal_mac_addr_t));

        arg_cidx = -1;
        arg_ccount = 1;

        if (!sal_strcasecmp(cachecmd, "add") ||
            !sal_strcasecmp(cachecmd, "+") ) {

            parse_table_init(unit, &pt);
            parse_table_add(&pt, "LookupClass",         PQ_DFL|PQ_INT,
                            0, &arg_lookup_class, NULL);
            parse_table_add(&pt, "CacheIndex",          PQ_DFL|PQ_INT,
                            0, &arg_cidx,       NULL);
            parse_table_add(&pt, "MACaddress",          PQ_DFL|PQ_MAC,
                            0, &arg_macaddr,    NULL);
            parse_table_add(&pt, "MACaddressMask",      PQ_DFL|PQ_MAC,
                            0, &arg_macaddr_mask, NULL);
            parse_table_add(&pt, "Vlanid",              PQ_DFL|PQ_HEX,
                            0, &arg_vlan,       NULL);
            parse_table_add(&pt, "VlanidMask",          PQ_DFL|PQ_HEX,
                            0, &arg_vlan_mask,  NULL);
            parse_table_add(&pt, "SourcePort",          PQ_DFL|PQ_PORT|PQ_BCM,
                            0, &arg_sport,      NULL);
            parse_table_add(&pt, "SourcePortMask",      PQ_DFL|PQ_HEX,
                            0, &arg_sport_mask, NULL);
            parse_table_add(&pt, "Module",              PQ_DFL|PQ_INT,
                            0, &arg_modid,      NULL);
            parse_table_add(&pt, "Port",                PQ_DFL|PQ_PORT,
                            (void *)(0), &arg_port, NULL);
            parse_table_add(&pt, "Trunk",               PQ_DFL|PQ_BOOL,
                            0, &arg_trunk,	NULL);
            parse_table_add(&pt, "TrunkGroupID",        PQ_DFL|PQ_INT,
                            0, &arg_tgid,	NULL);
            parse_table_add(&pt, "SetPriority",         PQ_DFL|PQ_BOOL,
                            0, &arg_setpri,	NULL);
            parse_table_add(&pt, "PRIOrity",            PQ_DFL|PQ_INT,
                            0, &arg_prio,       NULL);
            parse_table_add(&pt, "DiscardDest",         PQ_DFL|PQ_BOOL,
                            0, &arg_dd,	NULL);
            parse_table_add(&pt, "L3",                  PQ_DFL|PQ_BOOL,
                            0, &arg_l3if,       NULL);
            parse_table_add(&pt, "CPUmirror",           PQ_DFL|PQ_BOOL,
                            0, &arg_cbit,       NULL);
            parse_table_add(&pt, "MIRror",              PQ_DFL|PQ_BOOL,
                            0, &arg_mirror,     NULL);
            parse_table_add(&pt, "BPDU",                PQ_DFL|PQ_BOOL,
                            0, &arg_bpdu,       NULL);
            parse_table_add(&pt, "ReplacePriority",  PQ_DFL|PQ_BOOL,
                            0, &arg_rpe,	NULL);
            if (SOC_IS_FIREBOLT2(unit) || SOC_IS_TRX(unit)) {
                parse_table_add(&pt, "LearnDisable",   PQ_DFL|PQ_BOOL,
                                0, &arg_lrndis,	NULL);
            }
            parse_table_add(&pt, "PROTocolPacKeT",  PQ_DFL|PQ_BOOL,
                            0, &arg_protocolPkt,	NULL);
            if (!parseEndOk(a, &pt, &retCode)) {
                return retCode;
            }

            if (arg_setpri && arg_prio == -1) {
                cli_out("%s ERROR: no priority specified\n", ARG_CMD(a));
                return CMD_FAIL;
            }
            if (!arg_setpri) {
                arg_prio = -1;
            }

            bcm_l2_cache_addr_t_init(&l2caddr);

            l2caddr.vlan = arg_vlan_mask ? arg_vlan : 0;
            l2caddr.vlan_mask = arg_vlan_mask;

            ENET_COPY_MACADDR(arg_macaddr, l2caddr.mac);
            ENET_COPY_MACADDR(arg_macaddr_mask, l2caddr.mac_mask);

            if (arg_sport >= 0) {
                l2caddr.src_port = arg_sport;
                l2caddr.src_port_mask = arg_sport_mask;
            }

            l2caddr.lookup_class = arg_lookup_class;

            l2caddr.dest_modid = arg_modid;
            l2caddr.dest_port = arg_port;
            l2caddr.prio = arg_prio;
            l2caddr.dest_trunk = arg_tgid;

            /* Configure flags for SDK call */
            if (arg_dd || arg_ds)
                l2caddr.flags |= BCM_L2_CACHE_DISCARD;
            if (arg_l3if)
                l2caddr.flags |= BCM_L2_CACHE_L3;
            if (arg_trunk)
                l2caddr.flags |= BCM_L2_CACHE_TRUNK;
            if (arg_cbit)
                l2caddr.flags |= BCM_L2_CACHE_CPU;
            if (arg_bpdu)
                l2caddr.flags |= BCM_L2_CACHE_BPDU;
            if (arg_mirror)
                l2caddr.flags |= BCM_L2_CACHE_MIRROR;
            if (arg_rpe || arg_setpri)
                l2caddr.flags |= BCM_L2_CACHE_SETPRI;
            if (arg_lrndis)
                l2caddr.flags |= BCM_L2_CACHE_LEARN_DISABLE;
            if (arg_tunnel)
                l2caddr.flags |= BCM_L2_CACHE_TUNNEL;
            if (arg_protocolPkt)
                l2caddr.flags |= BCM_L2_CACHE_PROTO_PKT;

            dump_l2_cache_addr(unit, "ADD CACHE: ", &l2caddr);

            if ((rv = bcm_l2_cache_set(unit, arg_cidx, &l2caddr, 
                                       &cidx)) != BCM_E_NONE) {
                cli_out("%s ERROR: %s\n", ARG_CMD(a), bcm_errmsg(rv));
                return CMD_FAIL;
            }

            if (arg_cidx == -1) {
                cli_out(" => using index %d\n", cidx);
            }

	    /* Set up for next call */
	    increment_macaddr(arg_macaddr, 1);
            if (arg_cidx >= 0) {
                arg_cidx++;
            }

            return CMD_OK;
        }

        else if (!sal_strcasecmp(cachecmd, "del") ||
                 !sal_strcasecmp(cachecmd, "-")) {

            if (ARG_CNT(a)) {
                parse_table_init(unit, &pt);
                parse_table_add(&pt, "CacheIndex",      PQ_DFL|PQ_INT,
                                0, &arg_cidx,   NULL);
                parse_table_add(&pt, "Count",           PQ_DFL|PQ_INT,
                                0, &arg_ccount, NULL);
                if (!parseEndOk(a, &pt, &retCode)) {
                    return retCode;
                }
            }

            if (arg_cidx == -1) {
                cli_out("%s ERROR: no index specified\n", ARG_CMD(a));
                return CMD_FAIL;
            }

            for (idx = 0; idx < arg_ccount; idx++) {
                if ((rv = bcm_l2_cache_get(unit, arg_cidx, &l2caddr)) < 0) {
                    cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(rv));
                    return CMD_FAIL;
                }

                if (LOG_CHECK(BSL_LS_BCM_L2 | BSL_VERBOSE)) {
                    dump_l2_cache_addr(unit, "DEL CACHE: ", &l2caddr);
                }

                if ((rv = bcm_l2_cache_delete(unit, arg_cidx)) != BCM_E_NONE) {
                    cli_out("%s ERROR: %s\n", ARG_CMD(a), bcm_errmsg(rv));
                    return CMD_FAIL;
                }
                arg_cidx++;
            }

            return CMD_OK;
        }

        else if (!sal_strcasecmp(cachecmd, "show") ||
                 !sal_strcasecmp(cachecmd, "-d")) {

	    if ((rv = bcm_l2_cache_size_get(unit, &idx_max)) < 0) {
		cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(rv));
		return CMD_FAIL;
	    }

	    for (idx = 0; idx < idx_max; idx++) {
                if (bcm_l2_cache_get(unit, idx, &l2caddr) == BCM_E_NONE) {
                    sal_sprintf(str, " %4d : ", idx);
                    dump_l2_cache_addr(unit, str, &l2caddr);
                }
            }

            return CMD_OK;
        }

        else if (!sal_strcasecmp(cachecmd, "clear") ||
                 !sal_strcasecmp(cachecmd, "clr")) {

            arg_reinit = 0;

            if (ARG_CNT(a)) {
                parse_table_init(unit, &pt);
                parse_table_add(&pt, "ReInit",              PQ_DFL|PQ_BOOL,
                                0, &arg_reinit,     NULL);
                if (!parseEndOk(a, &pt, &retCode)) {
                    return retCode;
                }
            }

            if ((rv = bcm_l2_cache_delete_all(unit)) != BCM_E_NONE) {
                cli_out("%s ERROR: %s\n", ARG_CMD(a), bcm_errmsg(rv));
                return CMD_FAIL;
            }

            if (arg_reinit && (rv = bcm_l2_cache_init(unit)) != BCM_E_NONE) {
                cli_out("%s ERROR: %s\n", ARG_CMD(a), bcm_errmsg(rv));
                return CMD_FAIL;
            }

            return CMD_OK;
        }

        else {
            return CMD_USAGE;
        }
    }
#if defined(BCM_TRIUMPH_SUPPORT) /* BCM_TRIUMPH_SUPPORT */
    else if (!sal_strcasecmp(subcmd, "station")) {
        /* BCM.0> l2 station ... */
        return _l2_station_cmd_process(unit, a);
    }
#endif /* !BCM_TRIUMPH_SUPPORT */
#endif /* BCM_XGS3_SWITCH_SUPPORT */

    else {
        return CMD_USAGE;
    }

    return CMD_FAIL;
}


/*
 * Add an L2 address to the switch, remove an L2 address from the
 * switch, show addresses, and clear addresses on a per port basis.
 */

cmd_result_t
if_esw_bpdu(int unit, args_t *a)
{
    int                 idx, count;
    char 		*subcmd = NULL;
    sal_mac_addr_t	arg_macaddr, maczero;
    bcm_l2_cache_addr_t addr;
    int		arg_index = 0, idx_used = 0;
    cmd_result_t   retCode;
    int rv;
    parse_table_t	pt;
    char		buf[SAL_MACADDR_STR_LEN];

    /*
     * Initialize MAC address field for the user to the first real
     * address which does not conflict
     */
    maczero[0] = arg_macaddr[0] = 0;
    maczero[1] = arg_macaddr[1] = 0;
    maczero[2] = arg_macaddr[2] = 0;
    maczero[3] = arg_macaddr[3] = 0;
    maczero[4] = arg_macaddr[4] = 0;
    maczero[5] = 0;
    arg_macaddr[5] = 0x1;

    if ((subcmd = ARG_GET(a)) == NULL) {
	return CMD_USAGE;
    }

    /* Check valid device to operation on ...*/
    if (!sh_check_attached(ARG_CMD(a), unit)) {
	return CMD_FAIL;
    }

    if (!sal_strcasecmp(subcmd, "add") ||
	!sal_strcasecmp(subcmd, "+")) {
	parse_table_init(unit, &pt);
	parse_table_add(&pt, "Index", 	PQ_DFL|PQ_INT,
			(void *)(0), &arg_index, NULL);
	parse_table_add(&pt, "MACaddress", 	PQ_DFL|PQ_MAC,
		    0, &arg_macaddr,NULL);

	if (!parseEndOk(a, &pt, &retCode)) {
	    return retCode;
	}

        ENET_COPY_MACADDR(arg_macaddr, addr.mac);
        addr.flags = BCM_L2_CACHE_BPDU;
	rv = bcm_l2_cache_set(unit, arg_index, &addr, &idx_used);
	if (rv != BCM_E_NONE) {
	    cli_out("%s ERROR: %s\n", ARG_CMD(a), bcm_errmsg(rv));
	    return CMD_FAIL;
	}
	return CMD_OK;
    }

    if (!sal_strcasecmp(subcmd, "del") ||
	!sal_strcasecmp(subcmd, "-")) {
	parse_table_init(unit, &pt);
	parse_table_add(&pt, "Index", 	PQ_DFL|PQ_INT,
			(void *)(0), &arg_index, NULL);

	if (!parseEndOk(a, &pt, &retCode)) {
	    return retCode;
	}

        ENET_COPY_MACADDR(maczero, addr.mac);
        addr.flags = BCM_L2_CACHE_BPDU;
	rv = bcm_l2_cache_set(unit, arg_index, &addr, &idx_used);
	if (rv != BCM_E_NONE) {
	    cli_out("%s ERROR: %s\n", ARG_CMD(a), bcm_errmsg(rv));
	    return CMD_FAIL;
	}
	return CMD_OK;
    }

    if (!sal_strcasecmp(subcmd, "show") ||
	!sal_strcasecmp(subcmd, "-d")) {
	rv = bcm_l2_cache_size_get(unit, &count);
	if (rv != BCM_E_NONE) {
	    cli_out("%s ERROR: %s\n", ARG_CMD(a), bcm_errmsg(rv));
	    return CMD_FAIL;
	}   
	cli_out("unit %d has %d BPDU entries\n", unit, count);
	for (idx = 0; idx < count; idx++) {
            rv = bcm_l2_cache_get(unit, idx, &addr);
	    if (rv == BCM_E_NOT_FOUND) {
                continue;
	    } else if (rv != BCM_E_NONE) {
		cli_out("%s ERROR: %s\n", ARG_CMD(a), bcm_errmsg(rv));
		return CMD_FAIL;
	    }

	    format_macaddr(buf, addr.mac);
	    cli_out("\tBPDU %d: %s\n", idx, buf);
	}
	return CMD_OK ;
    }
    return CMD_USAGE;
}

/*
 * Multicast command
 */

cmd_result_t
if_esw_mcast(int unit, args_t *a)
{
    char                *subcmd;
    int			r;
    sal_mac_addr_t	mac_address;
    int		vid=1;
    int		cos_dst=0;
    int		mcindex = -1;
    bcm_pbmp_t	pbmp;
    bcm_pbmp_t	ubmp;
    parse_table_t	pt;
    cmd_result_t	retCode;
    bcm_mcast_addr_t	mcaddr;
    pbmp_t		rtrpbmp;
    int			port, dport;

    mac_address[0] = 0x1;
    mac_address[1] = 0;
    mac_address[2] = 0;
    mac_address[3] = 0;
    mac_address[4] = 0;
    mac_address[5] = 0x1;

    BCM_PBMP_CLEAR(pbmp);

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    if ((subcmd = ARG_GET(a)) == NULL) {
	return CMD_USAGE;
    }

    if (!sal_strcasecmp(subcmd, "init")) {
	if ((r = bcm_mcast_init(unit)) < 0) {
            cli_out("ERROR: %s\n", bcm_errmsg(r));
            return CMD_FAIL;
	}
        return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "bitmap") == 0) {
        if ((subcmd = ARG_GET(a)) == NULL) {
            return CMD_USAGE;
        }

        if (!SOC_IS_XGS_FABRIC(unit)) {
	    cli_out("Mcast bitmap operations only valid on XGS fabric devices\n");
	    return CMD_FAIL;
        }

        if (sal_strcasecmp(subcmd, "max") == 0) {
            if ((r = bcm_mcast_bitmap_max_get(unit, &mcindex)) < 0) {
                cli_out("bcm_mcast_bitmap_max_set failed ERROR: %s\n",
                        bcm_errmsg(r));
                return CMD_FAIL;
            }
            cli_out("Unit %d: Maximum multicast index of %d\n",
                    unit, mcindex);
            return CMD_OK;
        }

        if (sal_strcasecmp(subcmd, "set") == 0) {
            parse_table_init(unit, &pt);
            parse_table_add(&pt, "Port", PQ_DFL|PQ_PORT|PQ_BCM,
                            (void *)(0), &port, NULL);
            parse_table_add(&pt, "PortBitMap", 	PQ_DFL|PQ_PBMP|PQ_BCM,
                            (void *)(0), &pbmp, NULL);
            parse_table_add(&pt, "Index", 	PQ_DFL|PQ_INT,
                            0, &mcindex,	NULL);
            if (!parseEndOk(a, &pt, &retCode))
                return retCode;

            if ((r = bcm_mcast_bitmap_set(unit, mcindex, port, pbmp)) < 0) {
                cli_out("bcm_mcast_bitmap_set failed ERROR: %s\n",
                        bcm_errmsg(r));
                return CMD_FAIL;
            }
            return CMD_OK;
        }

        if (sal_strcasecmp(subcmd, "del") == 0) {
            parse_table_init(unit, &pt);
            parse_table_add(&pt, "Port", PQ_DFL|PQ_PORT|PQ_BCM,
                            (void *)(0), &port, NULL);
            parse_table_add(&pt, "PortBitMap", 	PQ_DFL|PQ_PBMP|PQ_BCM,
                            (void *)(0), &pbmp, NULL);
            parse_table_add(&pt, "Index", 	PQ_DFL|PQ_INT,
                            0, &mcindex,	NULL);
            if (!parseEndOk(a, &pt, &retCode))
                return retCode;

            if ((r = bcm_mcast_bitmap_del(unit, mcindex, port, pbmp)) < 0) {
                cli_out("bcm_mcast_bitmap_del failed ERROR: %s\n",
                        bcm_errmsg(r));
                return CMD_FAIL;
            }
            return CMD_OK;
        }

        if (sal_strcasecmp(subcmd, "get") == 0) {
            char	buf[FORMAT_PBMP_MAX];
            parse_table_init(unit, &pt);
            parse_table_add(&pt, "Port", PQ_DFL|PQ_PORT|PQ_BCM,
                            (void *)(0), &port, NULL);
            parse_table_add(&pt, "Index", 	PQ_DFL|PQ_INT,
                            0, &mcindex,	NULL);
            if (!parseEndOk(a, &pt, &retCode))
                return retCode;

            if ((r = bcm_mcast_bitmap_get(unit, mcindex, port, &pbmp)) < 0) {
                cli_out("bcm_mcast_bitmap_get failed ERROR: %s\n",
                        bcm_errmsg(r));
                return CMD_FAIL;
            }
            format_pbmp(unit, buf, sizeof (buf), pbmp);
            cli_out("Unit %d: MC index=%d, Port=%d, Bitmap=%s\n",
                    unit, mcindex, port, buf);
            return CMD_OK;
        }
        return CMD_USAGE;
    }

    if (SOC_IS_XGS_FABRIC(unit)) {
        cli_out("Non-bitmap mcast operations only valid on XGS switch devices\n\n");
        return CMD_USAGE;
    }

    if (sal_strcasecmp(subcmd, "add") == 0) {
	parse_table_init(unit, &pt);
	parse_table_add(&pt, "MACaddress", 	PQ_DFL|PQ_MAC,
			0, &mac_address,NULL);
	parse_table_add(&pt, "Vlanid", 	        PQ_DFL|PQ_HEX,
			0, &vid,	NULL);
	parse_table_add(&pt, "Cos", 	PQ_DFL|PQ_INT,
			0, &cos_dst,	NULL);
	parse_table_add(&pt, "PortBitMap", 	PQ_DFL|PQ_PBMP|PQ_BCM,
			(void *)(0), &pbmp, NULL);
	parse_table_add(&pt, "UntagBitMap", 	PQ_DFL|PQ_PBMP|PQ_BCM,
			(void *)(0), &ubmp, NULL);
	parse_table_add(&pt, "Index", 	PQ_DFL|PQ_INT,
			0, &mcindex,	NULL);
	if (!parseEndOk(a, &pt, &retCode))
	    return retCode;

	bcm_mcast_addr_t_init(&mcaddr, mac_address, vid);
	mcaddr.cos_dst = cos_dst;
	BCM_PBMP_ASSIGN(mcaddr.pbmp, pbmp);
	BCM_PBMP_ASSIGN(mcaddr.ubmp, ubmp);
	mcaddr.l2mc_index = mcindex;
	if (mcindex < 0) {
	    r = bcm_mcast_addr_add(unit, &mcaddr);
	} else {
	    r = bcm_mcast_addr_add_w_l2mcindex(unit, &mcaddr);
	}
	if (r < 0) {
	    cli_out("bcm_mcast_addr_add failed ERROR: %s\n", bcm_errmsg(r));
	    return CMD_FAIL;
	}
	return CMD_OK;
    }

    if (sal_strncasecmp(subcmd, "del", 3) == 0) {
	parse_table_init(unit, &pt);
	parse_table_add(&pt, "MACaddress", 	PQ_DFL|PQ_MAC,
			0, &mac_address,NULL);
	parse_table_add(&pt, "Vlanid", 	        PQ_DFL|PQ_HEX,
			0, &vid,	NULL);
	if (!parseEndOk(a, &pt, &retCode))
	    return retCode;
        if ((r = bcm_mcast_addr_remove(unit, mac_address, vid)) < 0) {
            cli_out("bcm_mcast_addr_remove failed ERROR: %s\n",
                    bcm_errmsg(r));
            return CMD_FAIL;
        }
	return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "join") == 0) {
	parse_table_init(unit, &pt);
	parse_table_add(&pt, "MACaddress", 	PQ_DFL|PQ_MAC,
			0, &mac_address,NULL);
	parse_table_add(&pt, "Vlanid", 	        PQ_DFL|PQ_HEX,
			0, &vid,	NULL);
	parse_table_add(&pt, "PortBitMap", 	PQ_DFL|PQ_PBMP|PQ_BCM,
			(void *)(0), &pbmp, NULL);
	if (!parseEndOk(a, &pt, &retCode))
	    return retCode;
        /* coverity[overrun-local : FALSE] */
        DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
	    r = bcm_mcast_join(unit, mac_address, vid, port,
			       &mcaddr, &rtrpbmp);
	    if (r < 0) {
		cli_out("ERROR: %s %s port %s failed: %s\n",
                        ARG_CMD(a), subcmd, BCM_PORT_NAME(unit, port),
                        bcm_errmsg(r));
		return CMD_FAIL;
	    }
	    switch (r) {
	    case BCM_MCAST_JOIN_ADDED:
		cli_out("port %s added\n", BCM_PORT_NAME(unit, port));
		break;
	    case BCM_MCAST_JOIN_UPDATED:
		cli_out("port %s updated\n", BCM_PORT_NAME(unit, port));
		break;
	    default:
		cli_out("port %s complete, return %d\n",
                        BCM_PORT_NAME(unit, port), r);
		break;
	    }
	}
	return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "leave") == 0) {
	parse_table_init(unit, &pt);
	parse_table_add(&pt, "MACaddress", 	PQ_DFL|PQ_MAC,
			0, &mac_address,NULL);
	parse_table_add(&pt, "Vlanid", 	        PQ_DFL|PQ_HEX,
			0, &vid,	NULL);
	parse_table_add(&pt, "PortBitMap", 	PQ_DFL|PQ_PBMP|PQ_BCM,
			(void *)(0), &pbmp, NULL);
	if (!parseEndOk(a, &pt, &retCode))
	    return retCode;
        DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
	    r = bcm_mcast_leave(unit, mac_address, vid, port);
	    if (r < 0) {
		cli_out("ERROR: %s %s port %s failed: %s\n",
                        ARG_CMD(a), subcmd, BCM_PORT_NAME(unit, port),
                        bcm_errmsg(r));
		return CMD_FAIL;
	    }
	    switch (r) {
	    case BCM_MCAST_LEAVE_NOTFOUND:
		cli_out("port %s not found\n", BCM_PORT_NAME(unit, port));
		break;
	    case BCM_MCAST_LEAVE_DELETED:
		cli_out("port %s deleted\n", BCM_PORT_NAME(unit, port));
		break;
	    case BCM_MCAST_LEAVE_UPDATED:
		cli_out("port %s updated\n", BCM_PORT_NAME(unit, port));
		break;
	    default:
		cli_out("port %s complete, return %d\n",
                        BCM_PORT_NAME(unit, port), r);
		break;
	    }
	}
	return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "padd") == 0) {
	parse_table_init(unit, &pt);
	parse_table_add(&pt, "MACaddress", 	PQ_DFL|PQ_MAC,
			0, &mac_address, NULL);
	parse_table_add(&pt, "Vlanid", 	        PQ_DFL|PQ_HEX,
			0, &vid,	NULL);
	parse_table_add(&pt, "PortBitMap", 	PQ_DFL|PQ_PBMP|PQ_BCM,
			(void *)(0), &pbmp, NULL);
	parse_table_add(&pt, "UntagBitMap", 	PQ_DFL|PQ_PBMP|PQ_BCM,
			(void *)(0), &ubmp, NULL);
	if (!parseEndOk(a, &pt, &retCode))
	    return retCode;
	bcm_mcast_addr_t_init(&mcaddr, mac_address, vid);
	BCM_PBMP_ASSIGN(mcaddr.pbmp, pbmp);
	BCM_PBMP_ASSIGN(mcaddr.ubmp, ubmp);
	r = bcm_mcast_port_add(unit, &mcaddr);
	if (r < 0) {
	    cli_out("ERROR: %s %s ports failed: %s\n",
                    ARG_CMD(a), subcmd, bcm_errmsg(r));
	    return CMD_FAIL;
	}
	return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "premove") == 0) {
	parse_table_init(unit, &pt);
	parse_table_add(&pt, "MACaddress", 	PQ_DFL|PQ_MAC,
			0, &mac_address, NULL);
	parse_table_add(&pt, "Vlanid", 	        PQ_DFL|PQ_HEX,
			0, &vid,	NULL);
	parse_table_add(&pt, "PortBitMap", 	PQ_DFL|PQ_PBMP|PQ_BCM,
			(void *)(0), &pbmp, NULL);
	if (!parseEndOk(a, &pt, &retCode))
	    return retCode;
	bcm_mcast_addr_t_init(&mcaddr, mac_address, vid);
	BCM_PBMP_ASSIGN(mcaddr.pbmp, pbmp);
	r = bcm_mcast_port_remove(unit, &mcaddr);
	if (r < 0) {
	    cli_out("ERROR: %s %s ports failed: %s\n",
                    ARG_CMD(a), subcmd, bcm_errmsg(r));
	    return CMD_FAIL;
	}
	return CMD_OK;
    }

    return CMD_USAGE;
}

/*
 * AGE timer
 */

cmd_result_t
cmd_esw_age(int unit, args_t *a)
{
    int seconds;
    int r;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    UNSUPPORTED_COMMAND(unit, SOC_CHIP_BCM5670, a);
    UNSUPPORTED_COMMAND(unit, SOC_CHIP_BCM5675, a);

    if (!ARG_CNT(a)) {			/* Display settings */
        if ((r = bcm_l2_age_timer_get(unit, &seconds)) != BCM_E_NONE) {
            cli_out("%s ERROR: could not get age time: %s\n",
                    ARG_CMD(a), bcm_errmsg(r));
            return CMD_FAIL;
        }

    	cli_out("Current age timer is %d.\n", seconds);

	return CMD_OK;
    }

    seconds = sal_ctoi(ARG_GET(a), 0);

    if ((r = bcm_l2_age_timer_set(unit, seconds)) != BCM_E_NONE) {
        cli_out("%s ERROR: could not set age time: %s\n",
                ARG_CMD(a), bcm_errmsg(r));
        return CMD_FAIL;
    }

    cli_out("Set age timer to %d. %s\n", seconds, seconds ? "":"(disabled)");

    return CMD_OK;
}

#ifdef BCM_XGS_SWITCH_SUPPORT

/*
 * L2 Mode
 */

char l2mode_usage[] =
    "Parameters: [on] [off] [Interval=<interval>]\n\t"
    "Starts the L2 shadow table update thread running every <interval>\n\t"
    "microseconds.  The task reads the entire L2 table and issues updates\n\t"
    "to the L2 shadow table and associated callbacks.  If <interval> is 0,\n\t"
    "stops the task.  If <interval> is omitted, prints current setting.\n";

cmd_result_t
cmd_l2mode(int unit, args_t *a)
{
    soc_control_t	*soc = SOC_CONTROL(unit);
    sal_usecs_t		usec;
    parse_table_t	pt;
    int			r;
    uint32		flags = 0;	/* no flags yet */

    if (!sh_check_attached(ARG_CMD(a), unit)) {
	return CMD_FAIL;
    }

    UNSUPPORTED_COMMAND(unit, SOC_CHIP_BCM5670, a);
    UNSUPPORTED_COMMAND(unit, SOC_CHIP_BCM5675, a);

    if (!soc_feature(unit, soc_feature_arl_hashed)) {
	cli_out("%s: No L2X on this chip\n", ARG_CMD(a));
	return CMD_FAIL;
    }

    if (soc == NULL) {
        return CMD_FAIL;
    }
    usec = soc->l2x_interval;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Interval", PQ_DFL|PQ_INT,
		    (void *) 0, &usec, NULL);

    if (!ARG_CNT(a)) {			/* Display settings */
	cli_out("Current settings:\n");
	parse_eq_format(&pt);
	parse_arg_eq_done(&pt);
	return CMD_OK;
    }

    if (parse_arg_eq(a, &pt) < 0) {
	cli_out("%s: Error: Unknown option: %s\n", ARG_CMD(a), ARG_CUR(a));
	parse_arg_eq_done(&pt);
	return CMD_FAIL;
    }
    parse_arg_eq_done(&pt);

    if (ARG_CNT(a) > 0 && !sal_strcasecmp(_ARG_CUR(a), "on")) {
        usec = prev_l2x_interval[unit];
        ARG_NEXT(a);
    }
    if (ARG_CNT(a) > 0 && !sal_strcasecmp(_ARG_CUR(a), "off")) {
	ARG_NEXT(a);
	usec = 0;
    }

    if (ARG_CNT(a) > 0) {
	return CMD_USAGE;
    }

    if (usec > 0) {
        r = soc_l2x_start(unit, flags, usec);
    } else {
        sal_usecs_t prev_interval;

        r = soc_l2x_running(unit, NULL, &prev_interval);
        if (r < 0 || prev_interval <= 0) {
            prev_interval = SOC_L2X_INTERVAL_DEFAULT;
        }
        prev_l2x_interval[unit] = prev_interval;
        r = soc_l2x_stop(unit);
    }

    if (r < 0) {
	cli_out("%s: Error: Could not set L2X mode: %s\n",
                ARG_CMD(a), soc_errmsg(r));
	return CMD_FAIL;
    }

    return CMD_OK;
}

#if defined(BCM_TOMAHAWK3_SUPPORT)
char l2learn_usage[] =
    "Parameters: [on] [off] [Interval=<interval>]\n\t"
    "Starts L2 learning thread running every <interval>\n\t"
    "microseconds. If <interval> is 0, stops the task.\n\t"
    "If <interval> is omitted, prints current setting.\n\t"
    "Note: If this thread is stopped, L2 learning is stopped\n";

cmd_result_t
cmd_l2learn(int unit, args_t *a)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    sal_usecs_t   usec;
    parse_table_t pt;
    int           r;

    if (!(SOC_IS_TOMAHAWK3(unit))) {
        cli_out("%s: Command supported on Tomahawk3 chip only\n", ARG_CMD(a));
        return CMD_FAIL;
    }

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    if (!soc_feature(unit, soc_feature_arl_hashed)) {
        cli_out("%s: No L2X on this chip\n", ARG_CMD(a));
        return CMD_FAIL;
    }

    if (soc == NULL) {
        return CMD_FAIL;
    }

    usec = soc->l2x_learn_interval;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Interval", PQ_DFL|PQ_INT,
                    (void *) 0, &usec, NULL);

    if (!ARG_CNT(a)) { /* Display settings */
        cli_out("Current settings:\n");
        parse_eq_format(&pt);
        parse_arg_eq_done(&pt);
        cli_out("Note: If this thread is stopped, L2 learning is stopped\n");
        return CMD_OK;
    }

    if (parse_arg_eq(a, &pt) < 0) {
        cli_out("%s: Error: Unknown option: %s\n", ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }

    parse_arg_eq_done(&pt);

    if (ARG_CNT(a) > 0 && !sal_strcasecmp(_ARG_CUR(a), "on")) {
        usec = prev_l2x_lrn_interval[unit];
        ARG_NEXT(a);
    }

    if (ARG_CNT(a) > 0 && !sal_strcasecmp(_ARG_CUR(a), "off")) {
        ARG_NEXT(a);
        usec = 0;
    }

    if (ARG_CNT(a) > 0) {
        return CMD_USAGE;
    }

    if (usec > 0) {
        r = soc_th3_l2_learn_thread_start(unit, usec);
    } else {
        sal_usecs_t prev_interval;

        cli_out("Stopping L2 Learn thread\n");
        cli_out("Note this command can take longer to complete depending"
                " on configured interval\n");
        r = soc_th3_l2_learn_thread_running(unit, &prev_interval);
        /* Thread is not running */
        if (r == 0 || prev_interval <= 0) {
            /* Set to default value */
            prev_interval = BCMSIM_L2XLRN_INTERVAL;
        }
        prev_l2x_lrn_interval[unit] = prev_interval;
        r = soc_th3_l2_learn_thread_stop(unit);
    }

    if (r < 0) {
        cli_out("%s: Error: Could not set L2X mode: %s\n",
            ARG_CMD(a), soc_errmsg(r));
        return CMD_FAIL;
    } else {
        cli_out("L2 Learn thread %s\n", usec ? "started" : "stopped");
    }

    return CMD_OK;
}
#endif /* BCM_TOMAHAWK3_SUPPORT */
#endif /* BCM_XGS_SWITCH_SUPPORT */

#if defined(BCM_TOMAHAWK_SUPPORT)
cmd_result_t
if_esw_latency(int unit, args_t *a)
{
    int rv = 0, support;
    char *arg, **list = NULL;
    uint32 resource, count = 0, idx = 0;
    char *rstr[3] = {"feature", "memory", "register"};
    char *sstr[2] = {"not ", ""};

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }
    if ((arg = ARG_GET(a)) == NULL) {
        return (CMD_USAGE);
    }

    /* latency show */
    if (arg && !sal_strcasecmp(arg, "show")) {
        rv = bcmi_esw_switch_latency_show(unit);
        if ((arg = ARG_GET(a)) == NULL) {
            return (CMD_OK);
        }

#ifdef SOC_NO_NAMES
        return CMD_OK;
#else
        if (!sal_strcasecmp(arg, "feature")) {
            count = soc_feature_count;
            list = soc_feature_name;
            idx = 0;
        } else if (!sal_strcasecmp(arg, "mem")) {
            count = NUM_SOC_MEM;
            list = soc_mem_name;
            idx = 1;
        } else if (!sal_strcasecmp(arg, "reg")) {
            count = NUM_SOC_REG;
            list = soc_reg_name;
            idx = 2;
        }
#endif

        if ((arg = ARG_GET(a)) == NULL) {
            return (CMD_USAGE);
        }

        /* generic resource iterator */
        for (resource = 0; resource < count; resource++) {
            if (!sal_strcasecmp(arg, list[resource])) {
                break;
            }
        }
        if (!(resource < count)) {
            cli_out("invalid %s: %s\n", rstr[idx], arg);
            return CMD_FAIL;
        }

        switch(idx)
        {
            case 0:   /* latency show feature */
                support = soc_feature(unit, resource) ? 1 : 0;
                break;
            case 1:   /* latency show mem */
                support = bcmi_esw_switch_latency_mem_avail(unit, resource) ? 1 : 0;
                break;
            case 2:   /* latency show reg */
                support = bcmi_esw_switch_latency_reg_avail(unit, resource) ? 1 : 0;
                break;
            default:
                return CMD_USAGE;
        }

        cli_out("%s '%s' is %savailable in this latency mode\n",
                 rstr[idx], arg, sstr[support]);
        return CMD_OK;
    } else if (arg && !sal_strcasecmp(arg, "diag")) {   /* latency diag */
        rv = bcmi_esw_switch_latency_diag(unit);
    }
    else {
        cli_out("error: incorrect argument\n");
        return CMD_USAGE;
    }

    if (BCM_FAILURE(rv)) {
        return CMD_FAIL;
    } else {
        return CMD_OK;
    }
}
#endif

