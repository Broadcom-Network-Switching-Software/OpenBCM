/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        ctecho.c
 * Purpose:     CPU transport setup and echo routines
 * Requires:    
 */


#include <sal/types.h>
#include <sal/core/libc.h>
#include <sal/core/thread.h>
#include <sal/core/dpc.h>
#include <sal/appl/sal.h>
#include <sal/appl/io.h>
#include <shared/bsl.h>
#include <soc/drv.h>
#include <bcm/stack.h>

#include <appl/diag/shell.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <appl/diag/ctrans.h>

#include <appl/cpudb/cpudb.h>
#include <appl/cputrans/cputrans.h>
#include <appl/cputrans/atp.h>
#include <appl/cputrans/next_hop.h>
#include <appl/cputrans/cpu2cpu.h>
#include <appl/cputrans/atptrans_socket.h>
#if defined(BCM_RPC_SUPPORT)
#include <bcm/pkt.h>
#include <appl/cputrans/ct_tun.h>
#endif

#if defined(BROADCOM_DEBUG)
extern void atp_dump(int);
extern void c2c_dump(void);
#endif /* BROADCOM_DEBUG */


#if defined(INCLUDE_LIB_CPUDB) && defined(INCLUDE_LIB_CPUTRANS) && \
    defined(INCLUDE_LIB_DISCOVER) && defined(INCLUDE_LIB_STKTASK)


/*
 * The following routine is used to register with BCM RX in
 * order to initiate BCM RX packet processing.
 */
#define RX_PKT_CLEAR_USEC       250000  /* Time to wait for RX discard */ 
#define RX_PROCESS_START_PRIO        1  /* Callback routine prio */

STATIC bcm_rx_t
rx_process_start(int unit, bcm_pkt_t *pkt, void *cookie)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(pkt);
    COMPILER_REFERENCE(cookie);
    
    return BCM_RX_NOT_HANDLED;
}

/*
 * Start BCM RX and initiate packet processing
 *
 * Note:
 *   BCM RX does not start packet processing until a callback
 *   routine is registered (other than the internal discard routine).
 */
STATIC int
rx_start(void)
{
    int  i;
    int  rv = BCM_E_NONE;
    int  rx_started = 0;
    /*
     * Start BCM RX
     */
    for (i = 0; i < soc_ndev; i++) {
        if (bcm_rx_active(SOC_NDEV_IDX2DEV(i))) {
            rx_started++;
            continue;
        }
        rv = bcm_rx_init(SOC_NDEV_IDX2DEV(i));
        if (rv == BCM_E_UNAVAIL) {
            cli_out("WARNING: BCM RX init failed, unit %d: %s\n",
                    SOC_NDEV_IDX2DEV(i), bcm_errmsg(rv));
            continue;
        } else if (BCM_FAILURE(rv)) {
            cli_out("ERROR: BCM RX init failed, unit %d: %s\n",
                    SOC_NDEV_IDX2DEV(i), bcm_errmsg(rv));
            return rv;
        }

        rv = bcm_rx_start(SOC_NDEV_IDX2DEV(i), NULL);
        if (BCM_FAILURE(rv)) {
            cli_out("ERROR: BCM RX start failed, unit %d: %s\n",
                    SOC_NDEV_IDX2DEV(i), bcm_errmsg(rv));
            return rv;
        }
        rx_started++;
    }

    /* Error if RX did not get started on any unit */
    if (rx_started == 0) {
        return BCM_E_UNAVAIL;
    }
    
    /*
     * Start BCM RX packet processing
     */
    
    /* RX packet processing has already started if NH is running */
    if (next_hop_running()) {
        return BCM_E_NONE;
    }
     
    /* Register dummy callback to trigger BCM RX packet processing */
    for (i = 0; i < soc_ndev; i++) {

        /* Skip units that did not get started */
        if (!bcm_rx_active(SOC_NDEV_IDX2DEV(i))) {
            cli_out("WARNING: skipping callback registration on unit %d\n", SOC_NDEV_IDX2DEV(i));
            continue;
        }

        rv = bcm_rx_register(SOC_NDEV_IDX2DEV(i), "process_start",
                             rx_process_start, RX_PROCESS_START_PRIO,
                             NULL, BCM_RCO_F_ALL_COS);
        if (BCM_FAILURE(rv)) {
            cli_out("ERROR: Cannot register RX callback to initiate packet "
                    "processing, unit %d: %s\n",
                    SOC_NDEV_IDX2DEV(i), bcm_errmsg(rv));
            /* Clean-up */
            for (; i > 0; i--) {
                bcm_rx_unregister(SOC_NDEV_IDX2DEV(i), rx_process_start,
                                  RX_PROCESS_START_PRIO);
            }
            return rv;
        }
    }
    
    /* Give time for RX handler to process and discard queued packets */
    sal_usleep(RX_PKT_CLEAR_USEC);

    /* Routine no longer needed after RX packet processing has started */
    for (i = 0; i < soc_ndev; i++) {
        bcm_rx_unregister(SOC_NDEV_IDX2DEV(i), rx_process_start, RX_PROCESS_START_PRIO);
    }

    return rv;
}

STATIC int
pkt_preinit(void)
{
    int rv;

    /* Start BCM RX and packet processing */
    if ((rv = rx_start()) < 0) {
        return -1;
    }

    if (!cputrans_tx_setup_done()) {
        
        rv = cputrans_tx_pkt_setup(100, &bcm_trans_ptr);
        if (rv < 0) {
            cli_out("ERROR: cputrans TX setup failed: %s\n",
                    bcm_errmsg(rv));
            return -1;
        }
    }

    if (!cputrans_rx_setup_done()) {
        rv = cputrans_rx_pkt_setup(-1, NULL);  /* Use defaults */
        if (rv < 0) {
            cli_out("ERROR: cputrans RX setup failed: %s\n",
                    bcm_errmsg(rv));
            return -1;
        }
    }

    return 0;
}


/****************************************************************
 *
 * CPUTRANS setup
 */

char ct_setup_usage[] =
    "CTSetup <subcmd> [options]\n"
    "    Modify the CPUTRANS subsystem\n"
#ifndef COMPILER_STRING_CONST_LIMIT
    "    Subcommands:\n"
    "        <none>                  Report if running\n"
    "        BCMREG                  Register BCM transport pointers\n"
    "        TXLIMIT [COS=<n> Limit=<n>]\n"
    "             Set TX packet alloc limit for the given cos\n"
    "        RX START\n"
    "             Start packet RX\n"
    "        NEXTHOP [DBidx=<n>] [THRDpri=<n>] [RXpri=<n>]\n"
    "             [COS=<n>] [VLAN=<n>]\n"
    "             Start next hop with the given DB index\n"
    "        ATP [TXPri=<n>] [RXPri=<n>] [COS=<n>] [VLAN=<n>]\n"
    "             Start ATP with given thread priorities\n"
#if defined(BROADCOM_DEBUG)
    "        ATP SHOW\n"
    "             Display internal ATP information\n"
#endif  /* BROADCOM_DEBUG */
#ifdef INCLUDE_ATPTRANS_SOCKET
    "        ATP TRANSport SOCKet INStall DestKey=<key> DestIP=<ip-addr>\n"
    "             Install socket interface for ATP transport\n"
    "        ATP TRANSport SOCKet UNINStall DestKey=<key>\n"
    "             Uninstall socket interface for ATP transport\n"
    "        ATP TRANSport SOCKet SERVer {START | STOP}\n"
    "             Start/stop ATP-over-Sockets Server\n"
    "        ATP TRANSport SOCKet CONFIG PRIority=<pri> \
        ListenPORT=<port-num> ConnectionPORT=<port-num>\n"
    "             Configure socket interface for ATP transport\n"
#ifdef BROADCOM_DEBUG
    "        ATP TRANSport SOCKet SHOW\n"
    "             Display internal ATP-over-Sockets subsystem information\n"
#endif /* BROADCOM_DEBUG */
#endif /* INCLUDE_ATPTRANS_SOCKET */
#if defined(BROADCOM_DEBUG)
    "        C2C SHOW\n"
    "             Display internal C2C information\n"
#endif  /* BROADCOM_DEBUG */
    "        STOP                    Stop ATP\n"
    "        TIMEOUT [RetryTO=<n>] [RETRIES=<n>]\n"
    "             Set/get the timeout (in usecs) and number of retries\n"
    "        SEGLEN [<n>]            Set/get the segment length\n"
    "        POOLSIZE [TX=<n>] [RX=<n>]   "
    "             Set/get the TX/RX pool size\n"
#endif
#if defined(BCM_RPC_SUPPORT)
    "        TUNNEL mode [{RELIABLE | BESTEFFORT}]\n"
    "             Set/get the default rx tunnel mode\n"
    "        TUNNEL set\n"
    "             Create tx tunnel\n"
    "        TUNNEL destroy\n"
    "             Tear down tx tunnel\n"
#endif
    ;


#ifdef INCLUDE_ATPTRANS_SOCKET

STATIC cmd_result_t
atptrans_socket_subcmd(int unit, args_t *args, cpudb_ref_t db_ref)
{
    char           *subcmd;
    parse_table_t  pt;
    cpudb_key_t    local_key;
    cpudb_key_t    dest_key;
    bcm_ip_t       dest_ip;
    int            rv = BCM_E_NONE;

    if ((subcmd = ARG_GET(args)) == NULL) {
        cli_out("Need option\n");
        return CMD_USAGE;
    }
                    
    if (parse_cmp("INStall", subcmd, '\0')) {

        /* Get destination CPU key and IP address */
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "DestKey", PQ_DFL|PQ_KEY, 0, 
                        dest_key.key, 0);
        parse_table_add(&pt, "DestIP", PQ_DFL|PQ_IP, 0,
                        (void *)&dest_ip, 0);
        if (parse_arg_eq(args, &pt) < 2) {
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }

        /* Make sure local CPU is set */
        CPUDB_KEY_COPY(local_key, db_refs[cur_db]->local_entry->base.key);
        atptrans_socket_local_cpu_key_set(local_key);
        
        /* Install socket for given dest CPU */
        rv = atptrans_socket_install(dest_key, dest_ip, 0x0);
        if (BCM_FAILURE(rv)) {
            cli_out("Cannot install socket interface in ATP transport "
                    "for CPU key " CPUDB_KEY_FMT_EOLN, 
                    CPUDB_KEY_DISP(dest_key));
        }
        parse_arg_eq_done(&pt);
        
    } else if (parse_cmp("UNINStall", subcmd, '\0')) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "DestKey", PQ_DFL|PQ_KEY, 0, 
                        dest_key.key, 0);
        if (parse_arg_eq(args, &pt) <= 0) {
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        rv = atptrans_socket_uninstall(dest_key);
        parse_arg_eq_done(&pt);
        
    } else if (parse_cmp("SERVer", subcmd, '\0')) {
        if ((subcmd = ARG_GET(args)) == NULL) {
            cli_out("Need option\n");
            return CMD_USAGE;
        }
        if (sal_strcasecmp(subcmd, "START") == 0) {
            rv = atptrans_socket_server_start();

        } else if (sal_strcasecmp(subcmd, "STOP") == 0) {
            rv = atptrans_socket_server_stop();

        } else {
            cli_out("Invalid argument: %s\n", subcmd);
            return CMD_USAGE;
        }
    } else if (parse_cmp("CONFIG", subcmd, '\0')) {
   /* Should be called after atptrans_socket_server_start() */
        /* Get destination priority and port numbers */
        int         priority;
        int         listen_port;
        int         connect_port;

        parse_table_init(unit, &pt);
        parse_table_add(&pt, "PRIority", PQ_DFL | PQ_INT, 0, &priority, 0);
        parse_table_add(&pt, "ListenPORT", PQ_DFL | PQ_INT, 0,
                        (void *) &listen_port, 0);
        parse_table_add(&pt, "ConnectionPORT", PQ_DFL | PQ_INT, 0,
                        (void *) &connect_port, 0);
        if (parse_arg_eq(args, &pt) <= 0) {
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        rv = atptrans_socket_config_set(priority, listen_port, connect_port);
#ifdef BROADCOM_DEBUG
    } else if (parse_cmp("SHOW", subcmd, '\0')) {
        atptrans_socket_show();
#endif /* BROADCOM_DEBUG */
    } else {
        cli_out("Invalid option: %s\n", subcmd);
        return CMD_USAGE;
    }
    if (BCM_FAILURE(rv)) {
        if (rv == BCM_E_UNAVAIL) {
            cli_out("Socket interface not available for current system\n");
        }
        else {
            cli_out("Command failed\n");
        }
    }

    return rv;
}
#endif /* INCLUDE_ATPTRANS_SOCKET */


cmd_result_t
ct_setup(int unit, args_t *args)
{
    parse_table_t  pt;
    char *subcmd;
    int rv;
    int db_idx = cur_db;

    COMPILER_REFERENCE(unit);

    if ((subcmd = ARG_GET(args)) == NULL) {
        cli_out("Next Hop is%s running\n",
                next_hop_running() ? "" : " not");
        cli_out("ATP is%s running\n",
                atp_running() ? "" : " not");
        return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "RX") == 0) {
        char *next_arg;

        if ((next_arg = ARG_GET(args)) == NULL) {
            return CMD_USAGE;
        }
        if (sal_strcasecmp(next_arg, "START")) {
            return CMD_USAGE;
        }
        if (pkt_preinit() < 0) {
            cli_out("Failed to start BCM RX\n");
            return CMD_FAIL;
        }
        
    } else if (sal_strcasecmp(subcmd, "NEXTHOP") == 0) {
        int idx;
        const cpudb_key_t *key;
        int s_unit = 0, s_port = 0, duplex = 0;
        int did_print = FALSE;

        if (cputrans_trans_count() == 0) {
            cputrans_trans_add(&bcm_trans_ptr);
        }

        if (ARG_CUR(args) == NULL) {
            for (idx = 0; idx < next_hop_num_ports_get(); idx++) {
                if (next_hop_port_get(idx, &s_unit, &s_port, &duplex)
                    == BCM_E_NONE) {
                    cli_out("NH idx %d:  Unit %d, Port %d: %sarked duplex.\n",
                            idx, s_unit, s_port, duplex ? "M" : "Not m");
                } else {
                    cli_out("NH idx %d:  INVALID?\n", idx);
                }
                did_print = TRUE;
            }
            for (idx = 0; idx < next_hop_max_entries_get(); idx++) {
                key = next_hop_key_get(idx);
                if (key != NULL) {
                    cli_out("KEY %x:%x:%x:%x:%x:%x is in NH DB\n",
                            (*key).key[0], (*key).key[1], (*key).key[2],
                            (*key).key[3], (*key).key[4], (*key).key[5]);
                    did_print = TRUE;
                }
            }
            if (!did_print) {
                cli_out("Next Hop database and ports empty\n");
            }
        } else {
            int thrd_pri = -1;
            int rx_pri = -1;
            int nhvlan = -1, nhcos = -1, src_mod = -1;
            int add_ports = 0;
            const cpudb_base_t *base;

            parse_table_init(unit, &pt);
            parse_table_add(&pt, "DBidx", PQ_DFL|PQ_INT, 0, &db_idx, 0);
            parse_table_add(&pt, "THRDpri", PQ_DFL|PQ_INT, 0, &thrd_pri, 0);
            parse_table_add(&pt, "RXpri", PQ_DFL|PQ_INT, 0, &rx_pri, 0);
            parse_table_add(&pt, "COS", PQ_DFL|PQ_INT, 0, &nhcos, 0);
            parse_table_add(&pt, "VLAN", PQ_DFL|PQ_INT, 0, &nhvlan, 0);
            parse_table_add(&pt, "SrcMOD", PQ_DFL|PQ_INT, 0, &src_mod, 0);
            parse_table_add(&pt, "Add", PQ_DFL|PQ_INT, 0, &add_ports, 0);
            if (parse_arg_eq(args, &pt) < 0) {
                parse_arg_eq_done(&pt);
                return CMD_USAGE;
            }
            parse_arg_eq_done(&pt);
            if (db_idx < 0 || db_idx >= MAX_DBS ||
                    db_refs[db_idx] == CPUDB_REF_NULL) {
                cli_out("Bad db index: %d\n", db_idx);
                return CMD_FAIL;
            }
            if (db_refs[db_idx]->local_entry == NULL) {
                cli_out("No local DB entry in cpudb %d\n", db_idx);
                return CMD_FAIL;
            }

            if (nhcos >= 0) {
                next_hop_cos_set(nhcos);
            }
            if (nhvlan >= 0) {
                next_hop_vlan_set(nhvlan);
                /* Load the next hop mac addr into L2 tables */
                rv = nh_tx_dest_install(TRUE, nhvlan);
                if (rv < 0) {
                    cli_out("NH TX dest install failed: %s\n", bcm_errmsg(rv));
                    return CMD_FAIL;
                }
            }
            if (src_mod >= 0) {
                int mod, port;

                nh_tx_src_get(&mod, &port);
                mod = src_mod;
                nh_tx_src_set(mod, port);
                nh_tx_unknown_modid_set(src_mod);
            }
            
            if (pkt_preinit() < 0) {
                cli_out("NH: Pre-init failed\n");
                return CMD_FAIL;
            }
            
            if (next_hop_running()) {
                cli_out("Next hop is running; stopping: %d\n",
                        next_hop_stop());
            }
            
            if (thrd_pri >= 0 || rx_pri >= 0) {
                if (next_hop_config_set(NULL, thrd_pri, rx_pri) < 0) {
                    cli_out("Warning: config set failed\n");
                }
            }

            base = &db_refs[db_idx]->local_entry->base;
            if (add_ports > 0) {
                int i, u, p;
                for (i = 0; i < base->num_stk_ports; i++) {
                    bcm_port_encap_config_t mode;
                    u = base->stk_ports[i].unit;
                    p = base->stk_ports[i].port;

                    rv = bcm_port_encap_config_get(u, p, &mode);
                    if (BCM_FAILURE(rv)) {
                        continue;
                    }
                    if (mode.encap == BCM_PORT_ENCAP_IEEE) {
                        continue;
                    }
                    rv = next_hop_port_add(u, p, 1);
                    if (rv < 0) {
                        cli_out("nexthop add (%d.%d) failed: %s\n",
                                u, p, bcm_errmsg(rv));
                    }
                }
            }
            rv = next_hop_start(base);
            if (rv < 0) {
                cli_out("ERROR: next_hop_start failed: %s\n", bcm_errmsg(rv));
            }
        }
        return CMD_OK;
#if defined(BROADCOM_DEBUG)
    } else if (sal_strcasecmp(subcmd, "NHSHOW") == 0) {
        next_hop_dump();
        return CMD_OK;
#endif /* BROADCOM_DEBUG */
    } else if (sal_strcasecmp(subcmd, "TXLIMIT") == 0) {
        int cos = -1;
        int limit = 0;
        int rv;

        parse_table_init(unit, &pt);
        parse_table_add(&pt, "COS", PQ_DFL|PQ_INT, 0, &cos, 0);
        parse_table_add(&pt, "Limit", PQ_DFL|PQ_INT, 0, &limit, 0);

        if (parse_arg_eq(args, &pt) < 0) {
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }

        if (cos == -1) {
            for (cos = 0; cos <= BCM_COS_MAX; cos++) {
                cputrans_tx_alloc_limit_get(cos, &limit);
                cli_out("Limit for COS %d currently %d\n", cos, limit);
            }
            parse_arg_eq_done(&pt);
            return CMD_OK;
        }

        rv = cputrans_tx_alloc_limit_set(cos, limit);
        if (rv < 0) {
            cli_out("WARNING: limit set returned %d: %s\n", rv, bcm_errmsg(rv));
        }

        parse_arg_eq_done(&pt);
        return CMD_OK;
        
    } else if (sal_strcasecmp(subcmd, "ATP") == 0) {
        int tx_pri = -1;
        int rx_pri = -1;
        int atp_cos = -1, atp_vlan = -1;

#if defined(BROADCOM_DEBUG)
        if ((subcmd = ARG_CUR(args)) != NULL) {
            if (sal_strcasecmp(subcmd, "SHOW") == 0) {
                ARG_NEXT(args);
                atp_dump(1);
                return CMD_OK;
            }
        }
#endif /* BROADCOM_DEBUG */

#ifdef INCLUDE_ATPTRANS_SOCKET        
        if ((subcmd = ARG_CUR(args)) != NULL) {
            if (parse_cmp("TRANSport", subcmd, '\0')) {
                ARG_NEXT(args);
                if ((subcmd = ARG_GET(args)) == NULL) {
                    return CMD_USAGE;
                }
                if (db_idx < 0 || db_idx >= MAX_DBS ||
                    db_refs[db_idx] == CPUDB_REF_NULL) {
                    cli_out("Bad or empty DB index: %d\n", db_idx);
                    return CMD_FAIL;
                }
                if (db_refs[db_idx]->local_entry == NULL) {
                    cli_out("No local DB entry in cpudb %d\n", db_idx);
                    return CMD_FAIL;
                }
                if (parse_cmp("SOCKet", subcmd, '\0')) {
                    return atptrans_socket_subcmd(unit, args, db_refs[db_idx]);
                }
                return CMD_USAGE;
            }
        }
#endif /* INCLUDE_ATPTRANS_SOCKET */

        if (cputrans_trans_count() == 0) {
            cputrans_trans_add(&bcm_trans_ptr);
        }

        if (pkt_preinit() < 0) {
#ifdef INCLUDE_ATPTRANS_SOCKET
            /* If ATP over sockets is running, then device RX is not
               required for ATP */
            if (!atptrans_socket_server_running()) {
                cli_out("ATP: Pre-init failed\n");
                return CMD_FAIL;
            }
#else
            cli_out("ATP: Pre-init failed\n");
            return CMD_FAIL;
#endif
        }

        parse_table_init(unit, &pt);
        parse_table_add(&pt, "DBidx", PQ_DFL|PQ_INT, 0, &db_idx, 0);
        parse_table_add(&pt, "TXPrio", PQ_DFL|PQ_INT, 0, &tx_pri, 0);
        parse_table_add(&pt, "RXPrio", PQ_DFL|PQ_INT, 0, &rx_pri, 0);
        parse_table_add(&pt, "COS", PQ_DFL|PQ_INT, 0, &atp_cos, 0);
        parse_table_add(&pt, "VLAN", PQ_DFL|PQ_INT, 0, &atp_vlan, 0);
        if (parse_arg_eq(args, &pt) < 0) {
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        if (db_idx < 0 || db_idx >= MAX_DBS ||
                db_refs[db_idx] == CPUDB_REF_NULL) {
            cli_out("Bad db index: %d\n", db_idx);
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
        }
        if (db_refs[db_idx]->local_entry == NULL) {
            cli_out("No local DB entry in cpudb %d\n", db_idx);
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
        }

        atp_cos_vlan_set(atp_cos, atp_vlan);
        rv = atp_config_set(tx_pri, rx_pri, NULL);
        if (rv < 0) {
            cli_out("ERROR: ATP config set failed: %s\n", bcm_errmsg(rv));
        }
        atp_db_update(db_refs[db_idx]);
        rv = atp_start(ATP_F_LEARN_SLF,
                       ATP_UNITS_ALL,
                       BCM_RCO_F_ALL_COS);
        if (rv < 0) {
            cli_out("ERROR: ATP start failed: %s\n", bcm_errmsg(rv));
        }
        parse_arg_eq_done(&pt);
#if defined(BROADCOM_DEBUG)
    } else if (sal_strcasecmp(subcmd, "C2C") == 0) {
        if ((subcmd = ARG_GET(args)) != NULL) {
            if (sal_strcasecmp(subcmd, "SHOW") == 0) {
                c2c_dump();
                return CMD_OK;
            }
        }
#endif /* BROADCOM_DEBUG */
    } else if (sal_strcasecmp(subcmd, "STOP") == 0) {
        rv = atp_stop();
        if (rv < 0) {
            cli_out("ERROR: ATP stop failed: %s\n", bcm_errmsg(rv));
        }
    } else if (sal_strcasecmp(subcmd, "TIMEOUT") == 0) {
        int retry_us = -1, retries = -1;
        
        if (ARG_CUR(args) == NULL) {
            atp_timeout_get(&retry_us, &retries);
            cli_out("Retries occur after %d usecs and will be made %d times\n",
                    retry_us, retries);
        } else {
            parse_table_init(unit, &pt);
            parse_table_add(&pt, "RetryTO", PQ_DFL|PQ_INT, 0, &retry_us, 0);
            parse_table_add(&pt, "RETRIES", PQ_DFL|PQ_INT, 0, &retries, 0);
            if (parse_arg_eq(args, &pt) < 0) {
                parse_arg_eq_done(&pt);
                return CMD_USAGE;
            }
            cli_out("Retry set(%d, %d) returns %d\n", retry_us, retries,
                    atp_timeout_set(retry_us, retries));
            parse_arg_eq_done(&pt);
        }
    } else if (sal_strcasecmp(subcmd, "SEGLEN") == 0) {
        int seg_len = -1;
        char *next_arg;

        if ((next_arg = ARG_GET(args)) == NULL) {
            seg_len = atp_segment_len_get();
            cli_out("Current segmentation length is %d\n", seg_len);
        } else {
            seg_len = strtoul(next_arg, NULL, 0);
            cli_out("Seg_len set(%d) returns %d\n", seg_len,
                    atp_segment_len_set(seg_len));
        }
    } else if (sal_strcasecmp(subcmd, "POOLSIZE") == 0) {
        int tx_pool, rx_pool;
        /* Get current pool sizes in case one is not specified */
        atp_pool_size_get(&tx_pool, &rx_pool);
        if (ARG_CUR(args) == NULL) {
            cli_out("Current TX pool: %d.  RX pool %d.\n", tx_pool, rx_pool);
        } else {
            parse_table_init(unit, &pt);
            parse_table_add(&pt, "TX", PQ_DFL|PQ_INT, 0, &tx_pool, 0);
            parse_table_add(&pt, "RX", PQ_DFL|PQ_INT, 0, &rx_pool, 0);
            if (parse_arg_eq(args, &pt) < 0) {
                parse_arg_eq_done(&pt);
                return CMD_USAGE;
            }
            cli_out("Pool size set returns %d\n",
                    atp_pool_size_set(tx_pool, rx_pool));
            parse_arg_eq_done(&pt);
        }
#if defined(BCM_RPC_SUPPORT)
    } else if (sal_strcasecmp(subcmd, "TUNNEL") == 0) {
        if ((subcmd = ARG_GET(args)) == NULL) {
            return CMD_USAGE;
        }

        if (sal_strcasecmp(subcmd, "mode") == 0) {
            int mode = -1;
            char *mode_str = NULL;

            if ((mode_str = ARG_GET(args)) == NULL) {

                /* Display the current rx tunnel mode */
                mode = ct_rx_tunnel_mode_default_get();
                if (BCM_CPU_TUNNEL_PACKET_RELIABLE == mode) {
                    mode_str = "RELIABLE";
                } else if (BCM_CPU_TUNNEL_PACKET_BEST_EFFORT == mode) {
                    mode_str = "BESTEFFORT";
                }

                if (NULL != mode_str) {
                    cli_out("Default rx tunnel mode is %s.\n", mode_str);
                }
            } else {
                if (sal_strcasecmp(mode_str, "BESTEFFORT") == 0) {
                    mode = BCM_CPU_TUNNEL_PACKET_BEST_EFFORT;
                } else if (sal_strcasecmp(mode_str, "RELIABLE") == 0) {
                    mode = BCM_CPU_TUNNEL_PACKET_RELIABLE;
                } else {
                    return CMD_USAGE;
                }

                rv = ct_rx_tunnel_mode_default_set(mode);
                if (BCM_FAILURE(rv)) {
                    return CMD_FAIL;
                }
            }
        } else if (sal_strcasecmp(subcmd, "set") == 0) {
            rv = ct_tx_tunnel_setup();
            cli_out("BCM TX Tunnel Setup returns %s\n", bcm_errmsg(rv));
        } else if (sal_strcasecmp(subcmd, "destroy") == 0) {
            rv = ct_tx_tunnel_stop();
            cli_out("BCM TX Tunnel Stop returns %s\n", bcm_errmsg(rv));
        } else {
            return CMD_USAGE;
        }
#endif /* BCM_RPC_SUPPORT */
    } else {
        cli_out("Subcommand not found: %s\n", subcmd);
        return CMD_USAGE;
    }

    return CMD_OK;
}


static char *cti_driver_list[] = {
    "BCM",
    "KERNEL",
    "KREMOTE",
    "OOB",
    "NONE",
    NULL};

#define CTI_DRIVER_BCM                 0
#define CTI_DRIVER_KERNEL              1
#define CTI_DRIVER_KREMOTE             2
#define CTI_DRIVER_OOB                 3
#define CTI_DRIVER_NONE                4

static bcm_trans_ptr_t * cti_driver_ptrs[] = {
    &bcm_trans_ptr,   /* Usual BCM */
    NULL,             /* Local via kernel driver */
    NULL,             /* Remote via kernel driver */
    NULL,             /* Out of band driver */
    NULL              /* Install NULL driver */
};

static char *cti_transport_list[] = {"NH", "DBENTRY", "ALL", "DEF", NULL};

#define CTI_TRANS_NH                 0
#define CTI_TRANS_DBENTRY            1
#define CTI_TRANS_ALL                2
#define CTI_TRANS_DEF                3

char ct_install_usage[] =
    "Usage:  CTInstall [Key=<key> Driver=<mode> Unit=<unit> Port=<port>\n"
    "                Transport=<mode> DBidx=<n>]\n"
    "    Install per CPU transport driver for the given CPU key\n"
#ifndef COMPILER_STRING_CONST_LIMIT
    "    Current drivers supported include:\n"
    "        BCM     -  The usual BCM transport pointers\n"
    "        KERNEL  -  Local devices via a user to kernel interface\n"
    "        KREMOTE -  Remote devices via a user to kernel interface\n"
    "        OOB     -  Out of band driver\n"
    "        NONE    -  Install NULL driver (disable per-CPU driver)\n"
    "    Current transports include:\n"
    "        NH      -  Install for next hop transmit\n"
    "        DBENTRY -  Install in DB entry which affects CPU2CPU\n"
    "        ALL     -  Install in all transports above\n"
    "        DEF     -  Install as the default transport for C2C and NH\n"
#endif
    ;

cmd_result_t
ct_install(int unit, args_t *args)
{
    parse_table_t pt;
    int rv;
    int drv_mode = CTI_DRIVER_NONE;
    int trans_mode = CTI_TRANS_ALL;
    cpudb_key_t key;
    int stk_unit;
    bcm_port_t stk_port;
    int db_idx = -1;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Driver", PQ_DFL|PQ_MULTI, 0, &drv_mode,
                    cti_driver_list);
    parse_table_add(&pt, "Transport", PQ_DFL|PQ_MULTI, 0, &trans_mode,
                    cti_transport_list);
    parse_table_add(&pt, "Key", PQ_DFL|PQ_KEY, 0, key.key, 0);
    parse_table_add(&pt, "Unit", PQ_DFL|PQ_INT, 0, &stk_unit, 0);
    parse_table_add(&pt, "Port", PQ_DFL|PQ_PORT, 0, &stk_port, 0);
    parse_table_add(&pt, "DBidx", PQ_DFL|PQ_INT, 0, &db_idx, 0);

    if (parse_arg_eq(args, &pt) < 0) {
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }

#ifdef INCLUDE_USER_KERNEL_TRANSPORTS
    {
        static int init = 1; 
        if(init == 1) {
            {
                static bcm_trans_ptr_t* bcm_uk_trans = NULL; 
                extern int bcm_uk_trans_create(char* service, int port,
                                               bcm_trans_ptr_t** p); 
                assert(bcm_uk_trans_create(NULL, stk_port,
                                           &bcm_uk_trans) >= 0); 
                cti_driver_ptrs[CTI_DRIVER_KERNEL] = bcm_uk_trans; 
                cputrans_trans_add(bcm_uk_trans); 
            }
            {
                static bcm_trans_ptr_t* bcm_uk_rtrans = NULL; 
                extern int bcm_uk_rtrans_create(char* service,
                                                bcm_trans_ptr_t** p); 
                assert(bcm_uk_rtrans_create(NULL, &bcm_uk_rtrans) >= 0); 
                if(bcm_uk_rtrans) {
                    cti_driver_ptrs[CTI_DRIVER_KREMOTE] = bcm_uk_rtrans; 
                    cputrans_trans_add(bcm_uk_rtrans); 
                }
            }
        }
        init = 0; 
    }
#endif /* INCLUDE_KERNEL_USER_TRANSPORTS */
    
    if (drv_mode == CTI_DRIVER_NONE) {
        cli_out("Note:  Clearing per CPU transport with NULL driver\n");
    } else if (cti_driver_ptrs[drv_mode] == NULL) {
        cli_out("Sorry, driver for %s is not yet implemented\n",
                cti_driver_list[drv_mode]);
        return CMD_FAIL;
    }


    /* Handle default transport request */
    if(trans_mode == CTI_TRANS_DEF) {
        /* Install this transport as the default for both C2C and NH */
        bcm_trans_ptr_t* tp;
        bcm_mac_t l; 
        uint32 flags; 
        
        c2c_config_get(&tp, l, &flags); 
        tp = cti_driver_ptrs[drv_mode];
        /* Enable checking for destination MAC in C2C packets */
        flags |= C2C_F_DEST_MAC_CHECK_ENABLE;
        c2c_config_set(tp, l, flags); 
        nh_tx_setup(tp); 
    }

    /* Handle next hop transport request */
    if (trans_mode == CTI_TRANS_ALL || trans_mode == CTI_TRANS_NH) {
        rv = nh_tx_trans_ptr_set(stk_unit, stk_port,
                                 cti_driver_ptrs[drv_mode]);

        if (rv < 0) {
            cli_out("WARNING:  nh_tx pointer set returned %d: %s\nContinuing\n",
                    rv, bcm_errmsg(rv));
        }
    }

    /* Handle C2C transport request */
    if (trans_mode == CTI_TRANS_ALL || trans_mode == CTI_TRANS_DBENTRY) {
        cpudb_entry_t *entry;

        if (db_idx == -1) {
            db_idx = cur_db;
        }
        if (db_refs[db_idx] == CPUDB_REF_NULL) {
            cli_out("DB %d is not setup.  Try 'CPUDB create'.\n", db_idx);
            return CMD_FAIL;
        }
        CPUDB_KEY_SEARCH(db_refs[db_idx], key, entry);
        if (entry == NULL) {
            cli_out("ERROR: DB entry not found\n");
            return CMD_FAIL;
        }
        entry->trans_ptr = cti_driver_ptrs[drv_mode];
    }

    return CMD_OK;
}


bcm_rx_t ab_echo_cb(cpudb_key_t src_key,
                    int client_id,
                    bcm_pkt_t *pkt,
                    uint8 *payload,
                    int payload_len,
                    void *cookie);

char echo_setup_usage[] =
    "CTEchoSetup <subcmd>\n"
;

char ct_echo_usage[] =
    "CTEcho <subcmd> [opts]\n"
    "    REG [Mode=<mode>] [DBidx=<n>] [Reassem=<T|F>]\n"
    "        Register for the given mode, with the given CPUDB\n"
    "    UNREG [Mode=<mode>]"
    "        Unregister for the given mode\n"
    "    ECHO String=<string> [Mode=<ATP|BET|ATPNH|BETNH|BETBcast>] \n"
    "        [DEPTH=<n>] [DestKey=<key>] [ALLOChdr=<T|F>] [ASYNC=<T|F>]\n"
    "        [MinLen=<n>] [Regen=<T|F>]\n"
    "    Echo a string, with recursive calls to the given depth.\n";


#define PACK_SHORT(buf, val) \
    do {                                               \
        uint16 v2;                                     \
        v2 = bcm_htons(val);                           \
        sal_memcpy(buf, &v2, sizeof(uint16));          \
    } while (0)

#define UNPACK_SHORT(buf, val) \
    do {                                               \
        sal_memcpy(&(val), buf, sizeof(uint16));       \
        val = bcm_ntohs(val);                          \
    } while (0)

#define PACK_LONG(buf, val) \
    do {                                               \
        uint32 v2;                                     \
        v2 = bcm_htonl(val);                           \
        sal_memcpy(buf, &v2, sizeof(uint32));           \
    } while (0)

#define UNPACK_LONG(buf, val) \
    do {                                               \
        sal_memcpy(&(val), buf, sizeof(uint32));         \
        val = bcm_ntohl(val);                          \
    } while (0)


/* First, a handler for echo packets */

typedef struct {
    int verbose;
} CallbackOptions;


/* Cookie data for echo async callback */
typedef struct {
    int    unit;
} _echo_cb_async_t;

#define ECHO_BASE_CLI_ID   1000
#define CTE_MODE_ATP                 0
#define CTE_MODE_BET                 1
#define CTE_MODE_ATP_NH              2
#define CTE_MODE_BET_NH              3
#define CTE_MODE_BET_BCAST           4
#define CTE_MODE_COUNT               5

#define CTE_MODE_MASK                0xf
#define CTE_FLAGS_ASYNC_FREE         0x100
#define CTE_FLAGS_ALLOC_HDR_HERE     0x200
#define CTE_FLAGS_IS_BCAST           0x400
#define CTE_FLAGS_VERBOSE            0x800
#define CTE_FLAGS_CRC_REGEN          0x1000

static char *cte_mode_list[] = {"ATP", "BET", "ATPNH", "BETNH", "BETBcast",
                                NULL};

static CallbackOptions cb_options[CTE_MODE_COUNT];

STATIC void echo_free_buf(void *payload_ptr, void *pkt_ptr, void *p3,
                          void *p4, void *p5)
{
    atp_rx_free(payload_ptr, pkt_ptr);
}

STATIC void
_send_echo_pkt_cb(uint8 *pkt, void *cookie, int rv)
{
    _echo_cb_async_t *async_data;

    if (cookie == NULL) {
        cli_out("Error in async callback routine; missing data\n");
        return;
    }
    
    async_data = (_echo_cb_async_t *)cookie;
    cli_out("Async callback routine, unit %d\n", async_data->unit);

    /* Free buffer allocated earlier in _ct_echo() */
    sal_dma_free(pkt);
    sal_free(async_data);
}

STATIC int
_send_echo_pkt(int client_id,
               uint8 *payload,
               int payload_len,
               int depth,
               uint32 pkt_flags,
               int mode,
               int verbose,
               cpudb_key_t dest_key,
               void *cookie)
{
    int payload_offset = 0;
    uint32 ct_flags = 0;
    atp_tx_cb_f cb = NULL;
    int crc_payload_len;
    int rv;

    if (verbose) {
        cli_out("echo pkt: %d bytes to " CPUDB_KEY_FMT_EOLN, payload_len,
                CPUDB_KEY_DISP(dest_key));
    }
    PACK_SHORT(payload, depth);
    PACK_LONG(&payload[sizeof(uint16)], pkt_flags);

    /* There are potentially two CRCs involved here: one is the CRC
       that protects the ECHO protocol payload, which is always
       present. The other is the Ethernet FCS, which is calculated by
       the Ethernet transmitter, space for which is only allocated if
       CTE_FLAGS_CRC_REGEN is set. */
    crc_payload_len = payload_len - sizeof(uint32);
    if (pkt_flags & CTE_FLAGS_CRC_REGEN) {
        crc_payload_len -= sizeof(uint32);
    }

    PACK_LONG(payload+crc_payload_len,
              ~_shr_crc32(~0, payload, crc_payload_len));
    
    if (mode == CTE_MODE_BET_BCAST) {
        ct_flags = CPUTRANS_BCAST;
    }

    if (pkt_flags & CTE_FLAGS_ALLOC_HDR_HERE) {
        payload_offset = CPUTRANS_HEADER_BYTES;
        ct_flags |= CPUTRANS_NO_HEADER_ALLOC;
    }

    if (pkt_flags & CTE_FLAGS_CRC_REGEN) {
        ct_flags |= CPUTRANS_CRC_REGEN;
    }

    if (pkt_flags & CTE_FLAGS_ASYNC_FREE) {
        cb = _send_echo_pkt_cb;
    }

    rv = atp_tx(dest_key,
                    client_id,
                    payload - payload_offset,
                    payload_len + payload_offset,
                    ct_flags,
                    cb,
                    cookie);

    return rv;
}


bcm_rx_t
ab_echo_cb(cpudb_key_t src_key,
           int client_id,
           bcm_pkt_t *pkt,
           uint8 *payload,
           int payload_len,
           void *cookie)
{
    uint16 depth;
    int async_free;
    uint32 pkt_flags;
    bcm_rx_t rv = BCM_RX_HANDLED;
    int offset = 0;
    int mode;
    int len;
    CallbackOptions *options = (CallbackOptions *)cookie;

    if (payload == NULL) {  /* Just use first segment for string */
        if (pkt == NULL) {
            cli_out("CT echo error: payload and pkt both NULL, cli %d\n",
                    client_id);
            return BCM_RX_NOT_HANDLED;
        }
        payload = pkt->pkt_data[1].data;
        len = pkt->pkt_data[1].len;
        cli_out("CT echo warning: \n");
        cli_out("   Segmented packet; segmentation not maintained on echo\n");
    } else {
        len = payload_len;
    }

    UNPACK_SHORT(payload, depth);
    offset += sizeof(uint16);
    UNPACK_LONG(payload + offset, pkt_flags);
    offset += sizeof(uint32);
    async_free = pkt_flags & CTE_FLAGS_ASYNC_FREE;
    mode = client_id - ECHO_BASE_CLI_ID;

    if (mode < 0 || mode > 4) {
        cli_out("CT echo error: Bad client id: %d\n", client_id);
        return BCM_RX_NOT_HANDLED;
    }

    if (sal_strlen((char *)&payload[offset]) > len) {
        cli_out("CT echo error: Unterminated string in first segment\n");
        return BCM_RX_HANDLED;
    }

    if (_shr_crc32(~0, payload, len) != _SHR_CRC32_CORRECT) {
        cli_out("CT echo error: Echo payload CRC failure.\n");
    }
    
    if (options->verbose) {
        cli_out("%s echo request from " CPUDB_KEY_FMT ": %s\n",
                cte_mode_list[mode],
                CPUDB_KEY_DISP(src_key),
                &payload[offset]);
        cli_out("       Depth %d, flags 0x%x, len %d. (total %d)\n",
                depth, pkt_flags, len - offset, payload_len);
    }

    if (depth > 0) {
        /* Return echo: one less depth, clear async flag */
        pkt_flags &= ~CTE_FLAGS_ASYNC_FREE;
        pkt_flags &= ~CTE_FLAGS_CRC_REGEN;
        _send_echo_pkt(client_id, payload, len, depth - 1,
                       pkt_flags, mode, options->verbose, src_key, NULL);
    }

    if (async_free) {
        rv = BCM_RX_HANDLED_OWNED;
        sal_dpc(echo_free_buf, payload, pkt, NULL, NULL, NULL);
    } else {
        rv = BCM_RX_HANDLED;
    }

    return rv;
}

#define ECHO_STR_OFFSET (sizeof(uint16) + sizeof(uint32))

typedef struct {
    uint8 *pkt_buf;
    uint32 pkt_flags;
    uint8 *payload_buf;
    int payload_len;
} _echo_payload_t;

int
_ct_echo_pkt_create(_echo_payload_t *ep,
                    char *str, int mode, int cte_flags, int minlen)
{
    int payload_len, len, len_str;
    uint32 pkt_flags = 0;
    uint8 *pkt_buf, *payload_buf;
    int alloc_hdr_here = cte_flags & CTE_FLAGS_ALLOC_HDR_HERE;

    sal_memset(ep, 0, sizeof(*ep));

    if (str == NULL) {
        cli_out("Null string\n");
        return 0;
    }

    pkt_flags = mode;
    len_str = sal_strlen(str);
    len = len_str + 1;
    len += ECHO_STR_OFFSET;      /* To store depth, flags */
    len += sizeof(uint32);       /* CRC at end of payload */
    
    if (cte_flags & CTE_FLAGS_CRC_REGEN) {
        pkt_flags |= CTE_FLAGS_CRC_REGEN;
        len += sizeof(uint32);       /* CRC at end of Ethernet frame */
    }
    
    len = minlen > len ? minlen : len;  /* Take min length if set */
    payload_len = len;
    if (alloc_hdr_here) {
        len += CPUTRANS_HEADER_BYTES;
        pkt_flags |= CTE_FLAGS_ALLOC_HDR_HERE;
    }

    /* Create packet */
    pkt_buf = sal_dma_alloc(len, "ct_echo");
    if (pkt_buf == NULL) {
        cli_out("Could not alloc packet buffer\n");
        return 0;
    }

    /* Pack the string into the payload past depth, flags */
    payload_buf = pkt_buf;
    if (alloc_hdr_here) {
        payload_buf += CPUTRANS_HEADER_BYTES;
    }

    sal_strncpy((char *)&payload_buf[ECHO_STR_OFFSET], str, len_str);
    if (len_str)
        *(char*)&payload_buf[ECHO_STR_OFFSET+len_str] = '\0';

    ep->pkt_buf = pkt_buf;
    ep->pkt_flags = pkt_flags;
    ep->payload_buf = payload_buf;
    ep->payload_len = payload_len;

    return 1;
}


void
_ct_echo_async(int unit, cpudb_key_t dest_key,
               char *str, int depth, int mode,
               int cte_flags, int minlen)
{
    void *cookie = NULL;
    _echo_cb_async_t  *async_data = NULL;
    _echo_payload_t payload;
    int rv = BCM_E_NONE;
    int verbose = cte_flags & CTE_FLAGS_VERBOSE;

    if (!_ct_echo_pkt_create(&payload, str, mode, cte_flags, minlen)) {
        return;
    }
    payload.pkt_flags |= CTE_FLAGS_ASYNC_FREE;

    if ((async_data = sal_alloc(sizeof(*async_data), "ct-echo")) == NULL) {
        cli_out("Could not alloc memory for async data\n");
        return;
    }
    async_data->unit = unit;
    cookie = (void *)async_data;

    if (verbose) {
        cli_out("CT echo:  Sending out %s echo request to CPU key "
                CPUDB_KEY_FMT ", depth %d, flags 0x%x\n",
                cte_mode_list[mode], CPUDB_KEY_DISP(dest_key),
                depth, payload.pkt_flags);
    }

    rv = _send_echo_pkt(ECHO_BASE_CLI_ID + mode,
                        payload.payload_buf, payload.payload_len,
                        depth, payload.pkt_flags, mode, verbose, dest_key,
                        cookie);
    
    /*
     * Free buffers if atp tx was not successful.  When atp tx is
     * successful , buffers are freed by the callback routine.
     */
    if (BCM_FAILURE(rv)) {
        cli_out("%s tx returns %d (%s)\n", cte_mode_list[mode],
                rv, bcm_errmsg(rv));
        sal_dma_free(payload.pkt_buf);
        sal_free(async_data);
    } else {
        cli_out("Echo sent successfully\n");
    }
}

void
_ct_echo_sync(int unit, cpudb_key_t dest_key,
              char *str, int depth, int mode,
              int cte_flags, int minlen, int count)
{
    void *cookie = NULL;
    _echo_payload_t payload;
    int rv = BCM_E_NONE;
    int actual = 0;
    int verbose = cte_flags & CTE_FLAGS_VERBOSE;

    if (count == 0) {
        cli_out("Nothing to send\n");
        return;
    }

    if (!_ct_echo_pkt_create(&payload, str, mode, cte_flags, minlen)) {
        return;
    }

    if (verbose) {
        cli_out("CT echo:  Sending out %s echo request to CPU key "
                CPUDB_KEY_FMT ", depth %d, flags 0x%x\n",
                cte_mode_list[mode], CPUDB_KEY_DISP(dest_key),
                depth, payload.pkt_flags);
    }

    while (count-- > 0) {
        rv = _send_echo_pkt(ECHO_BASE_CLI_ID + mode,
                            payload.payload_buf, payload.payload_len,
                            depth, payload.pkt_flags, mode, verbose, dest_key,
                            cookie);

        if (BCM_FAILURE(rv)) {
            cli_out("%s tx returns %d (%s)\n", cte_mode_list[mode],
                    rv, bcm_errmsg(rv));
            break;
        } else {
            actual++;
        }
    }

    if (actual > 0) {
        cli_out("Echo sent successfully\n");
    }
    
    /*
     * Free buffers.
     */

    sal_dma_free(payload.pkt_buf);

}

cmd_result_t
ct_echo(int unit, args_t *args)
{
    parse_table_t  pt;
    int depth = 1;
    char *str = NULL;
    static cpudb_key_t dest_key;
    int alloc_hdr = FALSE;
    int async = FALSE;
    int mode = 0;
    char *subcmd;
    static int db_idx = -1;
    int rv;
    int flags = 0;
    int minlen = 0;
    int reassem = TRUE;
    int verbose = TRUE;
    int count = 1;
    int crc_regen = FALSE;
    int cte_flags = 0;

    if ((subcmd = ARG_GET(args)) == NULL) {
        cli_out("Requires string argument\n");
        return CMD_USAGE;
    }
    if (sal_strcasecmp(subcmd, "REG") == 0) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "Mode", PQ_DFL|PQ_MULTI,0, &mode, cte_mode_list);
        parse_table_add(&pt, "DBidx", PQ_DFL|PQ_INT, 0, &db_idx, 0);
        parse_table_add(&pt, "Reassem", PQ_DFL|PQ_BOOL, 0, &reassem, 0);
        parse_table_add(&pt, "Verbose", PQ_DFL|PQ_BOOL, 0, &verbose, 0);
        if (parse_arg_eq(args, &pt) < 0) {
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        if (db_idx == -1) {
            db_idx = cur_db;
        }
        if (db_refs[db_idx] == CPUDB_REF_NULL) {
            cli_out("DB %d is not setup.  Try 'CPUDB create'.\n", db_idx);
            return CMD_FAIL;
        }

        if (reassem) {
            flags |= ATP_F_REASSEM_BUF;
        }
        /* Set next-hop and BET flags appropriately */
        if (mode == CTE_MODE_ATP_NH ||
            mode == CTE_MODE_BET_NH ||
            mode == CTE_MODE_BET_BCAST) {
            flags |= ATP_F_NEXT_HOP;
        }
        if (mode == CTE_MODE_BET_NH ||
            mode == CTE_MODE_BET ||
            mode == CTE_MODE_BET_BCAST) {
            flags |= ATP_F_NO_ACK;
        }

        cb_options[mode].verbose = verbose;
        rv = atp_register(ECHO_BASE_CLI_ID + mode, flags, ab_echo_cb,
                              (void *)(cb_options + mode), -1, -1);
        parse_arg_eq_done(&pt);
        if (rv < 0) {
            cli_out("Could not register echo as %s (%d) client (%d): %s\n",
                    cte_mode_list[mode], mode, rv, bcm_errmsg(rv));
            return CMD_FAIL;
        }
    } else if (sal_strcasecmp(subcmd, "UNREG") == 0) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "Mode", PQ_DFL|PQ_MULTI,0, &mode, cte_mode_list);
        if (parse_arg_eq(args, &pt) < 0) {
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        atp_unregister(ECHO_BASE_CLI_ID + mode);
        parse_arg_eq_done(&pt);
    } else if (sal_strcasecmp(subcmd, "ECHO") == 0) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "String", PQ_DFL|PQ_STRING, 0, &str, 0);
        parse_table_add(&pt, "MinLen", PQ_DFL|PQ_INT, 0, &minlen, 0);
        parse_table_add(&pt, "DEPTH", PQ_DFL|PQ_INT, 0, &depth, 0);
        parse_table_add(&pt, "Mode", PQ_DFL|PQ_MULTI,0, &mode, cte_mode_list);
        parse_table_add(&pt, "DestKey", PQ_DFL|PQ_KEY, 0, dest_key.key, 0);
        parse_table_add(&pt, "ALLOChdr", PQ_DFL|PQ_BOOL, 0, &alloc_hdr, 0);
        parse_table_add(&pt, "ASYNC", PQ_DFL|PQ_BOOL, 0, &async, 0);
        parse_table_add(&pt, "Verbose", PQ_DFL|PQ_BOOL, 0, &verbose, 0);
        parse_table_add(&pt, "Regen", PQ_DFL|PQ_BOOL, 0, &crc_regen, 0);
        parse_table_add(&pt, "Count", PQ_DFL|PQ_INT, 0, &count, 0);
        if (parse_arg_eq(args, &pt) < 0) {
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }

        if (alloc_hdr) {
            cte_flags |= CTE_FLAGS_ALLOC_HDR_HERE;
        }

        if (verbose) {
            cte_flags |= CTE_FLAGS_VERBOSE;
        }

        if (crc_regen) {
            cte_flags |= CTE_FLAGS_CRC_REGEN;
        }

        if (async) {
            _ct_echo_async(unit, dest_key, str, depth,
                          mode, cte_flags, minlen);
        } else {
            _ct_echo_sync(unit, dest_key, str, depth,
                          mode, cte_flags, minlen, count);
        }
        parse_arg_eq_done(&pt);
    } else if (sal_strcasecmp(subcmd, "SPIN") == 0) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "Count", PQ_DFL|PQ_INT, 0, &count, 0);
        if (parse_arg_eq(args, &pt) < 0) {
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        while(count-->0);
        parse_arg_eq_done(&pt);
    } else {
        cli_out("Subcommand not found: %s\n", subcmd);
        return CMD_USAGE;
    }



    return CMD_OK;
}

#endif /* TKS libs included */
