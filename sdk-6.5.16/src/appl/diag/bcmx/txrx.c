/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:        txrx.c
 * Purpose:
 * Requires:    
 */

#include <bcm/error.h>
#include <bcm/pkt.h>
#include <bcm/topo.h>
#include <shared/bsl.h>
#ifdef BCM_RPC_SUPPORT
#include <bcm_int/rpc/rlink.h>
#endif
#include <bcm_int/control.h>

#include <bcmx/tx.h>
#include <bcmx/rx.h>

#include <appl/diag/system.h>
#include <appl/diag/decode.h>

#include <appl/cputrans/ct_tun.h>

#include "bcmx.h"

static volatile int dump;

static volatile int pkt_count;
static volatile int rmt_count;
static volatile int tx_async_count;

char bcmx_cmd_rx_usage[] =
    "    [+|- dump] [source | start | stop | reg | unreg]\n"
    "    Test the bcmx rx subsystem.  \n"
#ifndef COMPILER_STRING_CONST_LIMIT
    "        start   - Make tunnel connect requests and register handler\n"
    "        stop    - Make tunnel disconnect requests\n"
    "        reg     - Register local BCMX handler (dump)\n"
    "        unreg   - Unregister local BCMX handler (dump)\n"
    "        dump    - Set/clear/toggle whether pkts are dumped\n"
    "        rlclear - Call rlink_clear function\n"
    "        txtunnel- Setup tx tunnel registration\n"
    "        notxtun - Tear down tx tunnel\n"
#endif
    ;

STATIC bcm_rx_t _bcmx_rx_handler(int unit,
                                 bcm_pkt_t *pkt,
                                 void *cookie);

cmd_result_t
bcmx_cmd_rx(int unit, args_t *args)
{
    int rv = CMD_OK;
    char *ch;

    if ((ch = ARG_GET(args)) == NULL) {
        cli_out("BCMX RX is %srunning; seen %d packet%s (%d remote); "
                "pkt dump is %s\n",
                bcmx_rx_running() ? "" : "not ", 
                pkt_count, pkt_count == 1 ? "" : "s",
                rmt_count,
                dump ? "on" : "off");
        return CMD_OK;
    }

    while (ch) {
        if (!strcmp("start", ch)) {
            rv = bcmx_rx_start();
            pkt_count = 0;
            cli_out("BCMX RX start returns %s\n", bcm_errmsg(rv));
        } else if (!strcmp("rlclear", ch)) {
#ifdef	BCM_RPC_SUPPORT
            bcm_rlink_clear();
#else
            cli_out("Function rlink_clear unavailable\n");
	    return CMD_FAIL;
#endif	/* BCM_RPC_SUPPORT */
        } else if (!strcmp("stop", ch)) {
            rv = bcmx_rx_stop();
            cli_out("BCMX RX stop returns %s\n", bcm_errmsg(rv));
        } else if (!strcmp("reg", ch)) {
            rv = bcmx_rx_register(
                "bcmx_diag",
                _bcmx_rx_handler,
                100,
                NULL,
                BCM_RCO_F_ALL_COS);
            cli_out("BCMX RX register returns %s\n", bcm_errmsg(rv));
        } else if (!strcmp("unreg", ch)) {
            rv = bcmx_rx_unregister(_bcmx_rx_handler, 100);
            cli_out("BCMX RX unregister returns %s\n", bcm_errmsg(rv));
        } else if (!strcmp("-dump", ch)) {
            dump = FALSE;
        } else if (!strcmp("dump", ch)) {
            dump = !dump;
        } else if (!strcmp("+dump", ch)) {
            dump = TRUE;
        } else if (!strcmp("txtunnel", ch)) {
#ifdef	BCM_RPC_SUPPORT
            rv = ct_tx_tunnel_setup();
            pkt_count = 0;
            cli_out("BCM TX Tunnel Setup returns %s\n", bcm_errmsg(rv));
#else
            cli_out("BCM TX Tunnel Setup unavailable\n");
	    return CMD_FAIL;
#endif	/* BCM_RPC_SUPPORT */
        } else if (!strcmp("notxtun", ch)) {
#ifdef	BCM_RPC_SUPPORT
            rv = ct_tx_tunnel_stop();
            pkt_count = 0;
            cli_out("BCM TX Tunnel Stop returns %s\n", bcm_errmsg(rv));
#else
            cli_out("BCM TX Tunnel Stop unavailable\n");
	    return CMD_FAIL;
#endif	/* BCM_RPC_SUPPORT */
        } else {
            return CMD_USAGE;
        }
        ch = ARG_GET(args);
    }

    return rv < 0 ? CMD_FAIL : CMD_OK;
}


STATIC bcm_rx_t
_bcmx_rx_handler(int unit, bcm_pkt_t *pkt, void *cookie)
{

    pkt_count++;
    if (dump) {
        cli_out("BCMX RX %d: %p from: (%d, %d), len %d.\n", unit,
                (void *)pkt, pkt->src_mod, pkt->src_port, pkt->pkt_len);
#ifdef	BROADCOM_DEBUG
        bcm_pkt_dump(-1, pkt, dump);
#endif	/* BROADCOM_DEBUG */
    }

    if (BCM_IS_REMOTE(unit)) {
        rmt_count++;
    }
    /* Don't want to interfere with other stuff that's going on */
    return BCM_RX_NOT_HANDLED;
}

#define TX_UNIT 0

STATIC void _bcmx_tx_handler(int unit, bcm_pkt_t *pkt, void *cookie)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(pkt);
    COMPILER_REFERENCE(cookie);
    tx_async_count++;
}

char bcmx_cmd_tx_usage[] =
    "    tx <user-port-list> [<opts>]\n"
#ifndef COMPILER_STRING_CONST_LIMIT
    "    Options include:\n"
    "        Length=<n>       Of packet in bytes\n"
    "        Count=<n>        Number of packets to send\n"
    "        Delay=<n>        Delay between packets (us)\n"
    "        VLantag=<n>      VLAN ID to use\n"
    "        VlanPrio=<n>     VLAN priority to use\n"
    "        L2Lookup=(T|F)   Should L2 lookup be done\n"
    "        DestMac=<mac>    Destination MAC address\n"
    "        SrcMac=<mac>     Source MAC address\n"
    "        SrcMod=<n>       Source module ID\n"
    "        SrcPort=<n>      Source port\n"
    "        COSqueue=<n>     COS queue on which to send\n"
    "        Pattern=<n>      Packet data pattern\n"
    "        PatternInc=<n>   Increment for packet data pattern\n"
    "        UnTagged=(T|F)   All pkts to be sent untagged\n"
    "        BestEffort=(T|F) If tunnelled, use best effort\n"
    "        Reliable=(T|F)   If tunnelled, use reliable transport\n"
    "        Flood=(T|F)      Flood the packet\n"
#endif
    "    Send out a packet to the indicated user ports\n"
    "    If L2 Lookup is true, user-port-list is ignored.\n";

cmd_result_t
bcmx_cmd_tx(int unit, args_t *args)
{
    char *ch;
    static int len = 68;
    bcm_pkt_t *pkt = NULL;
    int rv;
    int xfer;
    bcmx_lplist_t ports;
    static int src_port = 0, src_mod = 0;
    static int vlan_id = 1;
    static int vlan_prio = 0;
    static int l2_lookup = FALSE;
    static int cos = 0;
    static uint32 pattern = 0x12345678;
    static uint32 pat_inc = 0;
    static bcm_mac_t src_mac, dest_mac;
    static int count = 1;
    static int delay = 0;
    uint16 vtag;
    parse_table_t pt;
    int i;
    int untagged = FALSE;
    int best_effort = FALSE;
    int reliable = FALSE;
    int flood = FALSE;
    int async = FALSE;
    int bcmx_flags = 0;

    ch = ARG_GET(args);
    if (ch == NULL) {
        cli_out("BCMX TX async count=%d\n",tx_async_count);
        return CMD_OK;
    }

    bcmx_lplist_init(&ports, 0, 0);
    if (bcmx_lplist_parse(&ports, ch) < 0) {
        cli_out("%s: Error: could not parse portlist: %s\n",
                ARG_CMD(args), ch);
        rv = CMD_FAIL;
        goto done;
    }

    if (ports.lp_last < 0) {
        cli_out("No ports specified.\n");
        rv = CMD_FAIL;
        goto done;
    }

    parse_table_init(unit, &pt);

    parse_table_add(&pt, "Length", 	PQ_DFL|PQ_INT, 	0,
		    &len,    NULL);
    parse_table_add(&pt, "DeLay", 	PQ_DFL|PQ_INT, 	0,
		    &delay, NULL);
    parse_table_add(&pt, "Count", 	PQ_DFL|PQ_INT, 	0,
		    &count, NULL);
    parse_table_add(&pt, "VLantag", 	PQ_DFL|PQ_HEX, 	0,
		    &vlan_id,	    NULL);
    parse_table_add(&pt, "VlanPrio", 	PQ_DFL|PQ_INT, 	0,
		    &vlan_prio,	    NULL);
    parse_table_add(&pt, "L2Lookup", 	PQ_DFL|PQ_BOOL,	0,
		    &l2_lookup, NULL);
    parse_table_add(&pt, "DestMac", 	PQ_DFL|PQ_MAC, 	0,
		    &dest_mac,    NULL);
    parse_table_add(&pt, "SrcMac", 	PQ_DFL|PQ_MAC, 	0,
		    &src_mac,    NULL);
    parse_table_add(&pt, "SrcMod", 	PQ_DFL|PQ_INT, 	0,
		    &src_mod, NULL);
    parse_table_add(&pt, "SrcPort", 	PQ_DFL|PQ_INT, 	0,
		    &src_port, NULL);
    parse_table_add(&pt, "COSqueue", 	PQ_DFL|PQ_INT, 	0,
		    &cos, NULL);
    parse_table_add(&pt, "Pattern", 	PQ_DFL|PQ_HEX,	0,
		    &pattern,	    NULL);
    parse_table_add(&pt, "PatternInc", 	PQ_DFL|PQ_INT,	0,
		    &pat_inc,    NULL);
    parse_table_add(&pt, "UnTagged", 	PQ_DFL|PQ_BOOL,	0,
		    &untagged, NULL);
    parse_table_add(&pt, "BestEffort", 	PQ_DFL|PQ_BOOL,	0,
		    &best_effort, NULL);
    parse_table_add(&pt, "Reliable", 	PQ_DFL|PQ_BOOL,	0,
		    &reliable, NULL);
    parse_table_add(&pt, "Flood", 	PQ_DFL|PQ_BOOL,	0,
		    &flood, NULL);
    parse_table_add(&pt, "Async", 	PQ_DFL|PQ_BOOL,	0,
		    &async, NULL);

    /* Parse remaining arguments */
    if (0 > parse_arg_eq(args, &pt)) {
	cli_out("%s: Error: Invalid option or malformed expression: %s\n",
                ARG_CMD(args), ARG_CUR(args));
	parse_arg_eq_done(&pt);
	rv = CMD_FAIL;
        goto done;
    }
    parse_arg_eq_done(&pt);

    if (BCM_FAILURE(bcm_pkt_alloc(TX_UNIT, len, BCM_TX_CRC_APPEND, &pkt))) {
	rv = CMD_FAIL;
        goto done;
    }

    for (i = 0; i < len/4; i++) {
        sal_memcpy(&pkt->_pkt_data.data[i * 4], &pattern, 4);
        pattern += pat_inc;
    }
    sal_memcpy(pkt->_pkt_data.data, dest_mac, sizeof(bcm_mac_t));
    sal_memcpy(&pkt->_pkt_data.data[6], src_mac, sizeof(bcm_mac_t));

    vtag = (vlan_prio << 13) | vlan_id;
    pkt->_pkt_data.data[12] = 0x81; /* vlan 1 */
    pkt->_pkt_data.data[13] = 0;
    pkt->_pkt_data.data[14] = vtag >> 8;
    pkt->_pkt_data.data[15] = vtag & 0xff;

    /* Packet flags indicate tunnel mode */
    if (best_effort) {
        pkt->flags |= BCM_TX_BEST_EFFORT;
        bcmx_flags |= BCMX_TX_F_CPU_TUNNEL;
    }
    if (reliable) {
        pkt->flags |= BCM_TX_RELIABLE;
        bcmx_flags |= BCMX_TX_F_CPU_TUNNEL;
    }

    if (async) {
        pkt->call_back = _bcmx_tx_handler;
    }

    for ( xfer = 0; xfer < count; xfer++ ) {
        if (l2_lookup) { /* Don't need to look for dest port */
            rv = bcmx_tx(pkt, BCMX_TX_F_L2_LOOKUP|bcmx_flags);
        } else if (flood) {
            rv = bcmx_tx_flood(pkt, bcmx_flags);
        } else if (BCMX_LPLIST_COUNT(&ports) == 1
                   && !untagged) {  /* Single port (tagged) transmit */
            rv = bcmx_tx_uc(pkt, bcmx_lplist_first(&ports), bcmx_flags);
        } else {
            if (untagged) {
                rv = bcmx_tx_lplist(pkt, NULL, &ports, bcmx_flags); 
            } else {
                rv = bcmx_tx_lplist(pkt, &ports, NULL, bcmx_flags); 
            }
        }
        if (rv < 0) {
            cli_out("BCMX TX ERROR:  return value is %d (%s)\n",
                    rv, bcm_errmsg(rv));
            break;
        }
        sal_usleep(delay);
    }

done:
    if (pkt != NULL) {
        bcm_pkt_free(TX_UNIT, pkt);
    }
    bcmx_lplist_free(&ports);

    return CMD_OK;
}
