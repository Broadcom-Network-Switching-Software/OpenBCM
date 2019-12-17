/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * Network interface configuration routines and support utilities.
 */

#ifndef NO_SAL_APPL


#if defined(VXWORKS)
#include <vxWorks.h>
#include <hostLib.h>
#include <netShow.h>
#include <assert.h>

#ifndef VXWORKS_NETWORK_STACK_6_5
#include <ifLib.h>
#include <routeLib.h>
#endif /* VXWORKS_NETWORK_STACK_6_5 */

#if VX_VERSION == 66 || VX_VERSION == 68
#include <net/utils/ifconfig.h>
#include <net/utils/routeCmd.h>
#endif /* VX_VERSION == 66 || VX_VERSION == 68 */

#include <soc/debug.h>
#include <shared/bsl.h>
#include <appl/diag/shell.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <appl/diag/pp.h>
#include <appl/diag/diag.h>

#include <bcm/error.h>
#include <bcm/link.h>
#include <bcm/stack.h>
#include <bcm/port.h>
#include <bcm/debug.h>

#include <pingLib.h>

#ifndef SOC_END_NAME
#define SOC_END_NAME "sc"
#endif

typedef struct if_soc_s {
    void 		*ifs_sal;	/* SAL Cookie */
    char		*ifs_net_name;	/* Network Name */
    int			ifs_net_interface; /* Network interface # */
    sal_mac_addr_t	ifs_net_mac;	/* MAC Address */
    int			ifs_net_vlan;	/* VLAN ID */
    ip_addr_t		ifs_net_ip;	/* IP Address */
    ip_addr_t		ifs_net_mask;	/* Subnet Mask */
    ip_addr_t           ifs_gateway;    /* Default Gateway */
    char		*ifs_net_host;	/* Host name */
} if_soc_t;

#define	MAX_INTERFACE	32

static if_soc_t	*if_table[SOC_MAX_NUM_DEVICES];
#include <sys/types.h>

#include <soc/types.h>

#include <vxWorks.h>
#if defined(INCLUDE_DRIVERS)
#include <end.h>
#include <endLib.h>
#include <netLib.h>
#include <hostLib.h>
#include <etherMultiLib.h>
#endif

#include <sal/appl/sal.h>
#include <sal/appl/io.h>

#include <soc/debug.h>

IMPORT int 	ipAttach ();		/* Import VxWorks routine */

/*
 * Macro:	SAL_MAP_NETUNIT
 * Purpose:	Takes "normalized" unit number and turns it into 
 *		something VxWorks can deal with
 * Parameters:	_x - unit number passed down.
 */
#define	SAL_MAP_NETUNIT(_x)	(_x) += 2

#if !defined(INCLUDE_DRIVERS)
typedef void *END_OBJ;
#endif

#if defined(INCLUDE_DRIVERS)
static int
sal_if_do_deconfig(char *pfx, END_OBJ *eo, char *if_name, int if_unit)
/*
 * Function: 
 * Purpose:
 * Parameters:
 * Returns:
 */
{
#if VX_VERSION == 66 || VX_VERSION == 68
    char ifconfigstr[32];
#endif /* VX_VERSION == 66 || VX_VERSION == 68 */

    LOG_INFO(BSL_LS_APPL_END,
             (BSL_META("%s: Stopping %s %d %08x \n"),
              pfx, if_name, if_unit, (uint32)eo));
    if (OK != muxDevStop(eo)) {
	cli_out("%s: Error: muxDevStop failed: %s%d\n", 
                pfx, if_name, if_unit);
	return(-1);
    }

#if VX_VERSION == 66 || VX_VERSION == 68
    sprintf(ifconfigstr, "%s%d down", if_name, if_unit);
    if (ERROR == ifconfig(ifconfigstr)) {
        cli_out("%s: ifconfig down failed: %s%d <-- %s\n", pfx, if_name, if_unit, ifconfigstr);
        return(-1);
    }

    sprintf(ifconfigstr, "%s%d detach", if_name, if_unit);
    if (ERROR == ifconfig(ifconfigstr)) {
        cli_out("%s: ifconfig down failed: %s%d <-- %s\n", pfx, if_name, if_unit, ifconfigstr);
        return(-1);
    }
#else /* VX_VERSION == 66 || VX_VERSION == 68 */

    LOG_INFO(BSL_LS_APPL_END,
             (BSL_META("%s: Clearing Routes\n"), pfx));

#ifdef VXWORKS_NETWORK_STACK_6_5
#else
    ifRouteDelete(if_name, if_unit);
#endif /* VXWORKS_NETWORK_STACK_6_5 */

    LOG_INFO(BSL_LS_APPL_END,
             (BSL_META("%s: Unloading\n"), pfx));
    if (OK != muxDevUnload(if_name, if_unit)) {
	cli_out("%s: Error: muxDevUnload failed: %s%d\n",
                pfx, if_name, if_unit);
	return(-1);
    }
    LOG_INFO(BSL_LS_APPL_END,
             (BSL_META("%s: Stopped and Unloaded: %s%d\n"), 
              pfx, if_name, if_unit));
#endif /* VX_VERSION == 66 || VX_VERSION == 68 */
    return(0);
}
#endif /* defined(INCLUDE_DRIVERS) */

static int
sal_if_deconfig(char *pfx,  END_OBJ     *eo, char *if_name, int if_unit)
/*
 * Function: 
 * Purpose:	
 * Parameters:	pfx - prefix to print on errors/warnings
 *		if_name - name of interface ("sn" for "sn0")
 *		if_unit - unit # (0 for "sn0")
 * Returns:	0 - success
 *		-1 - failed
 */
{
#if defined(INCLUDE_DRIVERS)
    int		rv;

    SAL_MAP_NETUNIT(if_unit);

    if (NULL == endFindByName(if_name, if_unit)) {
	cli_out("%s: Error: can not locate device: %s%d\n", 
                pfx, if_name, if_unit);
	return(-1);
    }

    rv = sal_if_do_deconfig(pfx, eo, if_name, if_unit);
    LOG_INFO(BSL_LS_APPL_END,
             (BSL_META("%s: sal_if_deconfig complete: %s%d (%d)\n"), 
              pfx, if_name, if_unit, rv));
    return(rv);
#else  /* !defined(INCLUDE_DRIVERS) */
    return(-1);
#endif /* !defined(INCLUDE_DRIVERS) */
}

static void *
sal_if_config(char *pfx, int u, char *if_name, int if_unit, 
	      char *if_host, sal_mac_addr_t if_mac, int if_vlan, 
	      sal_ip_addr_t if_ip, sal_ip_addr_t if_netmask)
/*
 * Function: 	sal_if_config
 * Purpose:	Configure a network device (load driver and start)
 * Parameters:	pfx - string printed for error messages.
 *		name - device name ("sn" for "sn0")
 *		unit - unint number (0 for "sn0")
 * Returns:	NULL - failed
 *		!NULL - opaque pointer.
 */
{
#ifdef INCLUDE_DRIVERS
    extern END_OBJ *socend_load(char *is, void *);
    char	if_name_str[END_NAME_MAX];
    char	if_init_str[64];
    char 	if_ip_str[SAL_IPADDR_STR_LEN];
    char	if_mac_str[SAL_MACADDR_STR_LEN];
    END_OBJ	*eo;			/* END object  */
#if VX_VERSION == 64 || VX_VERSION == 65 || VX_VERSION == 66 || VX_VERSION == 68
#else
    M2_INTERFACETBL	m2;
#endif
#if VX_VERSION == 66 || VX_VERSION == 68
    char if_cfg_str[256];
    char if_mask_str[SAL_IPADDR_STR_LEN];
#endif

    SAL_MAP_NETUNIT(if_unit);
    LOG_INFO(BSL_LS_APPL_END,
             (BSL_META_U(u,
                         "%s: sal_if_config *** if_name=%s if_unit=%d\n"), 
              pfx, if_name, if_unit));
    /* Build vxWorks device name */
    sprintf(if_name_str, "%s%d", if_name, if_unit);

    /* Check to see if end device already loaded */

    if (NULL == (eo = endFindByName(if_name, if_unit))) {
	LOG_INFO(BSL_LS_APPL_END,
                 (BSL_META_U(u,
                             "%s: End device not loaded: if %s if_unit %d\n"), 
                  pfx, if_name, if_unit));
	format_macaddr(if_mac_str, if_mac);
	sprintf(if_init_str, "%d:%s:%d", u, if_mac_str, if_vlan);
	if (NULL == (eo = muxDevLoad(if_unit, socend_load, 
				     if_init_str, 0, NULL))) {
	    cli_out("%s: muxDevLoad failed: Unit %d\n", pfx, u);
	    return(NULL);
	}

	LOG_INFO(BSL_LS_APPL_END,
                 (BSL_META_U(u,
                             "%s: muxDevLoad successful: 0x%x\n"), pfx, (int)eo));
	if (ERROR == muxDevStart(eo)) {
	    cli_out("%s: muxDevStart failed: Unit %d\n", pfx, u);
	    (void)muxDevUnload(if_name, if_unit);
	    return(NULL);
	}
	LOG_INFO(BSL_LS_APPL_END,
                 (BSL_META_U(u,
                             "%s: muxDevStart successful: 0x%x\n"), pfx, (int)eo));
    }
#if VX_VERSION == 64 || VX_VERSION == 65 || VX_VERSION == 66 || VX_VERSION == 68
#else
    /*
     * Configure device....
     */
    if (OK != muxIoctl(eo, EIOCGMIB2, (caddr_t)&m2)) {
        cli_out("%s: muxIoctl failed: Unit %d\n", pfx, u);
	(void)sal_if_do_deconfig(pfx, eo, if_name, if_unit);
	return(NULL);
    }
    LOG_INFO(BSL_LS_APPL_END,
             (BSL_META_U(u,
                         "%s: muxIOCTL successful: Unit %d\n"), pfx, u));
#endif /* VX_VERSION */

    /*
     * Setup interface in following order:
     *		[1] Attach TCP/IP to END device
     *		[2] Set Netmask (if non-0)
     *		[3] Set IP address
     *		[4] Set host name associated with interface (if given).
     */
    if (OK != ipAttach(if_unit, if_name)) { /* [1] */
        cli_out("%s: ipAttach failed: Unit %d (interface %s)\n", 
                pfx, u, if_name_str);
	(void)sal_if_do_deconfig(pfx, eo, if_name, if_unit);
	return(NULL);
    }
    LOG_INFO(BSL_LS_APPL_END,
             (BSL_META_U(u,
                         "%s: ipAttach successful: if_name %s if_unit %d\n"), 
              pfx, if_name, if_unit));

#if VX_VERSION == 66 || VX_VERSION == 68

    format_ipaddr(if_ip_str, if_ip);	/* [3] */

    if (0 != if_netmask) {		/* [2] */
        format_ipaddr(if_mask_str, if_netmask);	/* [3] */
    }

    sprintf(if_cfg_str, "%s up inet add %s netmask %s", if_name_str, if_ip_str, if_mask_str);
   
    if (ERROR == ifconfig(if_cfg_str)) {
        cli_out("%s: ifconfig failed: %s%d: %s\n",
                pfx, if_name, if_unit, if_cfg_str);
        (void)sal_if_do_deconfig(pfx, eo, if_name, if_unit);
        return(NULL);
    }
    LOG_INFO(BSL_LS_APPL_END,
             (BSL_META_U(u,
                         "%s: ifconfig: if %s, cfg str %s\n"),
              pfx, if_name_str, if_cfg_str));
#else
    if (0 != if_netmask) {		/* [2] */
#ifdef VXWORKS_NETWORK_STACK_6_5
#else 
	if (ERROR == ifMaskSet(if_name_str, if_netmask)) {
	    cli_out("%s: ifMaskSet failed: %s 0x%x\n", 
                    pfx, if_name_str, if_netmask);
	    (void)sal_if_do_deconfig(pfx, eo, if_name, if_unit);
	    return(NULL);
	}
#endif /* VXWORKS_NETWORK_STACK_6_5 */
    }

    format_ipaddr(if_ip_str, if_ip);	/* [3] */
#ifdef VXWORKS_NETWORK_STACK_6_5
#else
    if (OK != ifAddrSet(if_name_str, (char *)if_ip_str)) {
	cli_out("%s: ifAddrSet failed: %s <-- %s\n", pfx, if_name, if_ip_str);
	(void)sal_if_do_deconfig(pfx, eo, if_name, if_unit);
	return(NULL);
    }
#endif /* VXWORKS_NETWORK_STACK_6_5 */

    LOG_INFO(BSL_LS_APPL_END,
             (BSL_META_U(u,
                         "%s: ifAddrSet successful: if %s\n"), pfx, if_name_str));
#endif

    if (if_host && *if_host) {		/* [4] */
	LOG_INFO(BSL_LS_APPL_END,
                 (BSL_META_U(u,
                             "%s: Setting hostname: %s\n"), pfx, if_host));
	if (OK != hostAdd (if_host, if_ip_str)) {
	    cli_out("%s: Warning: Failed to set hostname %s for device %s\n", 
                    pfx, if_host, if_name_str);
	}
    }

    return((void *)eo);			/* This is our opaque value */
#else /* !defined(INCLUDE_DRIVERS) */
    cli_out("sal_if_config: Interface configuration not compiled in\n");
    return(NULL);
#endif /* !defined(INCLUDE_DRIVERS) */
}


/*
 * Function: 	if_dump_table
 * Purpose:	Dump the interface table.
 * Parameters:	Unit - Strataswitch Unit #.
 *		interface - interface #
 * Returns:	Nothing
 */
static void
if_dump_table(int u, int interface, int all)
{
    int 	us, ue;
    int		is, ie;
    char	ip_str[SAL_IPADDR_STR_LEN+3];
    char	mac_str[SAL_MACADDR_STR_LEN];
    if_soc_t	*ifs;

    if (SOC_UNIT_VALID(u)) {
	us = ue = u;
    } else {
	us = 0;
	ue = soc_ndev;
    }
    if (interface < 0) {
	is = 0;
	ie = MAX_INTERFACE-1;
    } else {
	is = interface;
	ie = interface;
    }
    for (u = us; u <= ue; u++) {
	if (if_table[u] == NULL) {
	    continue;
	}
	for (interface = is; interface <= ie; interface++) {
	    ifs = &if_table[u][interface];
	    if (!all && !ifs->ifs_sal) {
		continue;;
	    }
	    if (ifs->ifs_sal) {
		format_ipaddr_mask(ip_str, ifs->ifs_net_ip, ifs->ifs_net_mask);
		format_macaddr(mac_str, ifs->ifs_net_mac);
		cli_out("unit %d interface %s%d: mac %s ip %s\n", u,
                        ifs->ifs_net_name, ifs->ifs_net_interface,
                        mac_str, ip_str);
	    } else {
            cli_out("unit %d interface %d: down\n", u, interface);
	    }
	}
    }
}

static	cmd_result_t
if_config_parse(int unit, if_soc_t *ifs, args_t *a)
/*
 * Function: 	if_config_parse
 * Purpose:	Parse the input parameters for the specified unit/port.
 * Parameters:	u - SOC unit number
 *		p - SOC port number
 *		a - remaining arguments
 * Returns:	CMD_OK/CMD_FAIL/CMD_USAGE
 */
{
    parse_table_t pt;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "MACaddress", PQ_DFL|PQ_MAC, 0, &ifs->ifs_net_mac, 0);
    parse_table_add(&pt, "VLan", PQ_DFL|PQ_INT, 0, &ifs->ifs_net_vlan, 0);
    parse_table_add(&pt, "IPaddress", PQ_DFL|PQ_IP, 0, &ifs->ifs_net_ip, 0);
    parse_table_add(&pt, "GateWay", PQ_DFL|PQ_IP, 0, &ifs->ifs_gateway, 0);
    parse_table_add(&pt, "SubNetMask", PQ_DFL|PQ_IP, 0, &ifs->ifs_net_mask,0);
    parse_table_add(&pt, "HostName", PQ_DFL|PQ_STRING,0,&ifs->ifs_net_host,0);

    if (0 > parse_arg_eq(a, &pt)) {
	parse_arg_eq_done(&pt);
	return(CMD_USAGE);
    }
    return(CMD_OK);
}

char cmd_if_config_usage[] =
    "Parameters: [<ifnum> [up|down [<option>=<value> ...]]]\n\t"
    "With no parameters, all current interfaces are listed. If only\n\t"
    "an interface name is specified, information on that interface is\n\t"
    "listed. If \"up\" or \"down\" is specified, the interface is\n\t"
    "activated or de-activated.\n"
#ifndef COMPILER_STRING_CONST_LIMIT
    "\tValue options are:\n\t"
    "  MACaddress=<address>  - MAC address for interface\n\t"
    "  VLan=<vlanid>         - VLAN id for interface\n\t"
    "  IPaddress=<address>   - IP address associated with interface\n\t"
    "  SubNetMask=<value>    - Subnet mask associated with interface\n\t"
    "  HostName=<name>       - Host name associated with interface\n\t"
    "  GateWay=<adress>      - Default gateway\n";
#endif
    ;

cmd_result_t
cmd_if_config(int u, args_t *a)
/*
 * Function: 	if_config
 * Purpose:	Perform a variety of configuration functions on an interface.
 * Parameters:	u - unit number to act on.
 *		a - Parameters.
 * Returns:	CMD_USAGE/CMD_FAIL/CMD_OK.
 */
{
    if_soc_t	*ifs;
    int		interface;
    cmd_result_t rv;
    if_soc_t	newif;
#if VX_VERSION == 66 || VX_VERSION == 68
    char gw_cfg_str[32];
#endif

    /*
     * Just dump all of our information.
     */
    if (0 == ARG_CNT(a)) {
	if_dump_table(u, -1, FALSE);
	return(CMD_OK);
    }

    /* Pick up interface number */

    if (!isint(ARG_CUR(a))) {
        cli_out("%s: Invalid interface number: %s\n",
                ARG_CMD(a), ARG_CUR(a));
        return(CMD_FAIL);
    }

    interface = parse_integer(ARG_GET(a));
    if (interface >= MAX_INTERFACE) {
        cli_out("%s: ERROR: Interface # too high, only permitted %d\n",
                ARG_CMD(a), MAX_INTERFACE);
        return(CMD_FAIL);
    }

    /* If no parameters left now, display information on the port */

    if (0 == ARG_CNT(a)) {
	if_dump_table(u, interface, TRUE);
	return(CMD_OK);
    }

    if (if_table[u] == NULL) {
	sal_memset(&newif, 0, sizeof(newif));
	ifs = &newif;
    } else {
	ifs = &if_table[u][interface];
    }
    ifs->ifs_net_name = SOC_END_NAME;
    ifs->ifs_net_interface = interface;

    /* Check for UP/DOWN */

    if (sal_strcasecmp("up", _ARG_CUR(a)) == 0) {
	ARG_NEXT(a);			/* Passed UP */

	if (ifs->ifs_sal) {
	    cli_out("%s: ERROR: Interface %d already running\n",
                    ARG_CMD(a), interface);
	    return(CMD_FAIL);
	}

	/* Parse the Parameters and try to configure the device */

	if (CMD_OK != (rv = if_config_parse(u, ifs, a))) {
	    return(rv);
	}
	if (ARG_CNT(a)) {
	    return(CMD_USAGE);
	}

	ifs->ifs_sal = sal_if_config(ARG_CMD(a), u,
				     ifs->ifs_net_name,
				     ifs->ifs_net_interface,
				     ifs->ifs_net_host,
				     ifs->ifs_net_mac,
				     ifs->ifs_net_vlan,
				     ifs->ifs_net_ip,
				     ifs->ifs_net_mask);

	if (ifs->ifs_sal == NULL) {
	    return CMD_FAIL;
	}
	if (if_table[u] == NULL) {
	    if_table[u] = sal_alloc(sizeof(*ifs) * MAX_INTERFACE, "if_table");
	    if (if_table[u] == NULL) {
		cli_out("%s: ERROR: cannot allocate interface table\n",
                        ARG_CMD(a));
		return CMD_FAIL;
	    }
	    sal_memset(if_table[u], 0, sizeof(*ifs) * MAX_INTERFACE);
	    if_table[u][interface] = *ifs;
	}

        /* Add default gateway */
        if (ifs->ifs_gateway != 0) {
            char          gateway_str[SAL_IPADDR_STR_LEN];

           format_ipaddr(gateway_str, ifs->ifs_gateway);
#if VX_VERSION == 66 || VX_VERSION == 68
            sprintf(gw_cfg_str, "add default %s", gateway_str);
            LOG_INFO(BSL_LS_APPL_END,
                     (BSL_META_U(u,
                                 "Default gateway: %s\n"), gw_cfg_str));
            if (ERROR == routec(gw_cfg_str)) {
                cli_out("Warning: Failed to add default route gatway = %s\n",
                        gateway_str);
            }
#else        
#ifdef VXWORKS_NETWORK_STACK_6_5
#else
           if (OK != routeAdd("0.0.0.0", gateway_str)) {
                cli_out("Warning: Failed to add default route gatway = %s\n",
                        gateway_str);
           }
#endif /* VXWORKS_NETWORK_STACK_6_5 */
#endif
        }
	return CMD_OK;
    } else if (sal_strcasecmp("down", _ARG_CUR(a)) == 0) {
	ARG_NEXT(a);

#if VX_VERSION == 66 || VX_VERSION == 68
        if (ifs->ifs_gateway != 0) {
           char          gateway_str[SAL_IPADDR_STR_LEN];

           format_ipaddr(gateway_str, ifs->ifs_gateway);
           sprintf(gw_cfg_str, "delete default %s", gateway_str);
           if (OK != routec(gw_cfg_str)) {
                cli_out("Warning: Failed to delete default route gatway = %s\n", 
                        gateway_str);
           }
        }
#endif /* VX_VERSION == 66 || VX_VERSION == 68 */

	if (!ifs->ifs_sal) {
	    cli_out("%s: Interface %d not running\n",
                    ARG_CMD(a), interface);
	    return(CMD_FAIL);
	}
	if (sal_if_deconfig(ARG_CMD(a), ifs->ifs_sal, ifs->ifs_net_name,
			    ifs->ifs_net_interface)) {
	    ifs->ifs_sal = NULL;
	    return CMD_FAIL;
	}
	ifs->ifs_sal = NULL;
	return CMD_OK;
    }
    cli_out("%s: ERROR: Invalid option %s: [up|down] expected\n",
            ARG_CMD(a), ARG_CUR(a));
    return(CMD_USAGE);
}

#else
int  _appl_diag_if_c_not_empty;
#endif


#endif /*  NO_SAL_APPL */
