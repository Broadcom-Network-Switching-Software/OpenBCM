/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        tksdiag.c
 * Purpose:     CPU transport, CPU DB, and Stacking functions
 * Requires:
 */

#include <sal/types.h>
#include <sal/core/libc.h>
#include <sal/core/thread.h>
#include <sal/core/sync.h>
#include <sal/core/dpc.h>
#include <sal/appl/sal.h>
#include <sal/appl/io.h>
#include <shared/bsl.h>
#include <soc/drv.h>
#include <bcm/pkt.h>
#include <bcm/tx.h>
#include <bcm/topo.h>
#include <bcm/init.h>
#include <bcm/stg.h>
#include <bcm/vlan.h>
#include <bcm/stack.h>
#ifdef BCM_RPC_SUPPORT
#include <bcm_int/rpc/rpc.h>
#include <bcm_int/rpc/rlink.h>
#endif /* BCM_RPC_SUPPORT */
#include <bcm_int/control.h>
#include <bcm/debug.h>

#include <appl/diag/shell.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <appl/diag/ctrans.h>

#include <appl/cpudb/cpudb.h>
#include <appl/cputrans/cputrans.h>
#include <appl/cputrans/atp.h>
#include <appl/cputrans/next_hop.h>

#include <appl/discover/disc.h>
#include <appl/stktask/stktask.h>
#include <appl/stktask/topology.h>
#include <appl/stktask/topo_pkt.h>
#include <appl/stktask/topo_brd.h>
#include <appl/stktask/attach.h>

#if defined(INCLUDE_BOARD)
#include <board/board.h>
#endif

/* Event Logging */
#include <appl/diag/evlog.h>

#if defined(INCLUDE_SHARED_EVLOG)
#define ST_EVLOG
#endif /* INCLUDE_SHARED_EVLOG */


#if defined(BCM_FIELD_SUPPORT) && defined(BCM_FIREBOLT_SUPPORT)
#include <soc/drv.h>    /* For MH Opcode-0 priority workaround */
#endif

void topo_cpu_dump(topo_cpu_t *topo_cpu, char *prefix);



/* Parsing a CPU KEY is same as parsing a MAC for now */

#if defined(INCLUDE_LIB_CPUDB) && defined(INCLUDE_LIB_CPUTRANS) && \
    defined(INCLUDE_LIB_DISCOVER) && defined(INCLUDE_LIB_STKTASK)

const bcm_mac_t _bcast_mac = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};


/****************************************************************
 *
 * CPUDB control
 */

STATIC INLINE void
db_entry_show(const cpudb_entry_t *entry)
{
    const cpudb_stk_port_t  *sp_p;
    int                     i;
    char                    keybuf[CPUDB_KEY_STRING_LEN];
    const cpudb_unit_port_t *sp_base;
    char                    pr_buf[80];
    cpudb_ref_t             db_ref;
    uint32                  flags;
    char                    *idstr = "unknown";

    db_ref = entry->db_ref;

    cpudb_key_format(entry->base.key, keybuf, sizeof(keybuf));
    cli_out("    Key: %s   MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
            keybuf,
            entry->base.mac[0], entry->base.mac[1], entry->base.mac[2],
            entry->base.mac[3], entry->base.mac[4], entry->base.mac[5]);
    cli_out("    Flags 0x%x:%s%s%s%s%s%s%s%s%s%s\n",
            entry->flags,
            entry->flags & CPUDB_F_IS_MASTER ? " master" : "",
            entry->flags & CPUDB_F_IS_LOCAL ? " local" : "",
            entry->flags & CPUDB_F_TX_KNOWN ? " tx-known" : "",
            entry->flags & CPUDB_F_SYSID_KNOWN ? " sysid-known" : "",
            entry->flags & CPUDB_F_DEST_KNOWN ? " dest-known" : "",
            entry->flags & CPUDB_F_BASE_INIT_DONE ? " base-init" : "",
            entry->flags & CPUDB_F_GLOBAL_COMPLETE ? " gcomp" : "",
            entry->flags & CPUDB_F_INACTIVE_MARKED ? " inact" : "",
            entry->flags & CPUDB_F_CONFIG_IN ? " cfg in" : "",
            entry->flags & CPUDB_F_LOCAL_COMPLETE ? " lcomp" : "");

    /* Base board id and flags */
    cpudb_board_idstr_get(entry->base.board_id, &idstr);
    cli_out("    Board ID: %d (%s)\n", entry->base.board_id, idstr);
    flags = entry->base.flags;
    cli_out("        Flags: %08x %s%s%s%s\n", flags,
            flags & CPUDB_BASE_F_CHASSIS ? " chassis" : "",
            flags & CPUDB_BASE_F_LOAD_BALANCE ? " load-bal" : "",
            flags & CPUDB_BASE_F_CHASSIS_AST ? " ast" : "",
            flags & CPUDB_BASE_F_CHASSIS_10SLOT ? " 10slot" : "");

    cli_out("    units %d. dest unit %d. base dest port %d. "
            "dest mod %d dest port %d.\n",
            entry->base.num_units,
            entry->base.dest_unit,
            entry->base.dest_port,
            entry->dest_mod,
            entry->dest_port);
    for (i = 0; i < entry->base.num_units; i++) {
        cli_out("        unit %d: mod id req %d; mod id pref %d\n",
                i, entry->base.mod_ids_req[i], entry->base.pref_mod_id[i]);
    }
    cli_out("    master prio %d. slot id %d. discovery seq %d.\n",
            entry->base.master_pri,
            entry->base.slot_id,
            entry->base.dseq_num);
    cli_out("    dest mod,port %d,%d. tx unit,port %d,%d. num stk %d\n",
            entry->dest_mod,
            entry->dest_port,
            entry->tx_unit,
            entry->tx_port,
            entry->base.num_stk_ports);
    cli_out("    trans pointers %p.\n", (void *)entry->trans_ptr);
    for (i = 0; i < entry->base.num_units; i++) {
        cli_out("        unit %d: base mod id %d\n", i, entry->mod_ids[i]);
    }

    for (i = 0; i < entry->base.num_stk_ports; i++) {
        sp_p = &entry->sp_info[i];
        sp_base = &entry->base.stk_ports[i];
        cli_out("        StkPort %d:  Unit %d. Port %d. "
                "Flags 0x%x:%s%s%s%s%s%s; \n",
                i, sp_base->unit, sp_base->port,
                sp_p->flags,
                sp_p->flags & CPUDB_SPF_NO_LINK ? " nolink" : "",
                sp_p->flags & CPUDB_SPF_INACTIVE ? " inactive" : "",
                sp_p->flags & CPUDB_SPF_DUPLEX ? " duplex" : "",
                sp_p->flags & CPUDB_SPF_CUT_PORT ? " cut_port" : "",
                sp_p->flags & CPUDB_SPF_TX_RESOLVED ? " txok" : "",
                sp_p->flags & CPUDB_SPF_RX_RESOLVED ? " rxok" : "");
        cli_out("            Weight %u. Base Flags 0x%x:%s.\n",
                sp_base->weight,
                sp_base->bflags,
                sp_base->bflags & CPUDB_UPF_DISABLE_IF_CUT ? " dis_cut" : "");
        cpudb_key_format(sp_p->tx_cpu_key, keybuf, sizeof(keybuf));

        if (CPUDB_KEY_COMPARE(sp_p->tx_cpu_key, sp_p->rx_cpu_key) == 0 &&
            sp_p->tx_stk_idx == sp_p->rx_stk_idx) {
            sal_sprintf(pr_buf, "TX/RX Key: %s", keybuf);
            if (sp_p->tx_stk_idx >= 0) {
                sal_sprintf(pr_buf + strlen(pr_buf), " (rmt-idx %d)",
                            sp_p->tx_stk_idx);
            }
            if (db_ref->local_entry->base.flags & CPUDB_BASE_F_CHASSIS) {
                sal_sprintf(pr_buf + strlen(pr_buf), " to slot %d",
                            cpudb_sp_idx_to_slot(db_ref, entry, i, NULL));
            }
            cli_out("            %s\n", pr_buf);
        } else {
            if (sp_p->tx_stk_idx >= 0) {
                cli_out("            TX Key: %s (rmt-idx %d)\n",
                        keybuf, sp_p->tx_stk_idx);
            } else {
                cli_out("            TX Key: %s\n",
                        keybuf);
            }
            cpudb_key_format(sp_p->rx_cpu_key, keybuf, sizeof(keybuf));
            if (sp_p->rx_stk_idx >= 0) {
                cli_out("            RX Key: %s (rmt-idx %d)\n",
                        keybuf, sp_p->rx_stk_idx);
            } else {
                cli_out("            RX Key: %s\n",
                        keybuf);
            }
        }
    }
}

/* Skip to next digit or end of string (0) */

STATIC char *
_next_digit(char *str)
{
    if (str == NULL) {
        return str;
    }

    while ((*str != 0) && !isdigit((unsigned)*str)) {
        str++;
    }

    return str;
}


/* Parse unit/port pair from string.
 * Returns NULL if fails.
 * Otherwise, returns updated string pointer
 *
 * All non-digits are ignored, so the only requirement is that
 * the numbers are in pairs.  E.g.:
 * "0 1 0 3 0 5"   and    "(0,1) (0,3) (0,5)" are equivalent.
 */

STATIC char *
unit_port_pair_parse(char *str, int *unit, bcm_port_t *port)
{
    char *next;
    char *orig_str;

    orig_str = str;
    str = _next_digit(str);
    *unit = strtoul(str, &next, 0);
    if (next == str) {
        return NULL;
    }
    str = next;
    str = _next_digit(str);
    *port = strtoul(str, &next, 0);
    if (next == str) {
        cli_out("Error parsing unit/port in %s\n", orig_str);
        return NULL;
    }

    return next;
}

STATIC INLINE int
port_str_parse(char *str, cpudb_entry_t *entry)
{
    char *next;
    int i;

    if (str == NULL || *str == 0) {
        return 0;
    }

    entry->base.num_stk_ports = strtoul(str, &next, 0);
    if (next == str) {
        return -1;
    }

    str = next;
    for (i = 0; i < entry->base.num_stk_ports; i++) {
        str = unit_port_pair_parse(str, &(entry->base.stk_ports[i].unit),
                                   &(entry->base.stk_ports[i].port));

        if (str == NULL) {
            return -1;
        }
    }

    return 0;
}

STATIC void
db_dump(cpudb_ref_t db_ref)
{
    int           i, count;
    cpudb_entry_t *entry;
    char          keybuf[CPUDB_KEY_STRING_LEN];

    if (cpudb_valid(db_ref)) {
        count = cpudb_entry_count_get(db_ref);
        cli_out("  DB is valid and contains %d entries\n", count);
        if (db_ref->local_entry != NULL) {
            cpudb_key_format(db_ref->local_entry->base.key,
                             keybuf, sizeof(keybuf));
            cli_out("    Local Key: %s\n", keybuf);
        }
        if (db_ref->master_entry != NULL) {
            cpudb_key_format(db_ref->master_entry->base.key,
                             keybuf, sizeof(keybuf));
            cli_out("    Master Key: %s\n", keybuf);
        }
        cli_out("\n");
        i = 0;
        CPUDB_FOREACH_ENTRY(db_ref, entry) {
            i += 1;
            cli_out("    Entry %d/%d: ", i, count);
            db_entry_show(entry);
            cli_out("\n");
        }
        if (i != count) {
            cli_out("WARNING: entry count is %d but %d entries actually found\n",
                    count, i);
        }
    } else {
        cli_out("DB is not valid\n");
    }
}

char ct_cpudb_usage[] =
    "cpudb <subcmd> [options]\n"
#ifndef COMPILER_STRING_CONST_LIMIT
    "Subcommands:\n"
    "    newdb | create\n"
    "        Create a new DB and indicate DB reference.  By default, DB 0 is\n"
    "        created and used.  Will not change current DB.\n"
    "    current <n>\n"
    "        Set the current DB reference to <n> (returned from new).\n"
    "    destroy | kill <n>\n"
    "        Delete the DB indicated\n"
    "    update KEY=<key> [MAC=<mac>] [TXUnit=<unit>] [TXPort=<port>]\n"
    "            [DMod=<mod>] [DPort=<port>] [ModidsReq=<n>] [MasterPri=<n>]\n"
    "            [NumUnits=<n>]\n"
    "        Update with the CPU entry with the given key using the\n"
    "        information provided.  Negative parameters will be ignored.\n"
    "    find | lookup MAC=<mac>\n"
    "        Look up the given MAC address\n"
    "    show\n"
    "        Show contents of the current DB\n"
    "    remove KEY=<key>\n"
    "        Remove the given KEY\n"
    "    stackports PortString=\"<n> <unit0> <port0> ...\" [DBidx=<idx>]\n"
    "            Indicate stack ports for the local system; local entry\n"
    "            must exist for the given DB\n"
    "    add KEY=<key> Local=<T|F>\n"
    "        Create an entry in the current DB for the given KEY.\n"
    "        Will also add the CPU key to ATP.\n"
#endif
    ;

cpudb_ref_t db_refs[MAX_DBS];
int cur_db;
int num_db = 0;

cmd_result_t
ct_cpudb(int unit, args_t *args)
{
    char *subcmd;
    char *int_arg;
    parse_table_t pt;
    static bcm_mac_t mac;
    int a_db;
    int tx_port = -1;
    int tx_unit = -1;
    int dest_mod = -1;
    int dest_port = -1;
    int master_pri = -1;
    int num_units = -1;
    cpudb_entry_t *entry;
    int slot_id = -1;
    int dest_unit = -1;
    int i;
    cpudb_key_t key;

    COMPILER_REFERENCE(unit);
    if ((subcmd = ARG_GET(args)) == NULL) {
        cli_out("Current DB is %d and is%s valid\n",
                cur_db, cpudb_valid(db_refs[cur_db]) ? "" : " NOT");
        return CMD_OK;
    }

    if ((sal_strcasecmp(subcmd, "NEWDB") == 0) ||
        (sal_strcasecmp(subcmd, "CREATE") == 0)) {
        for (i = 0; i < MAX_DBS; i++) {
            if (db_refs[i] == NULL) {
                break;
            }
        }
        if (i >= MAX_DBS) {
            cli_out("Too many DBs\n");
            return CMD_FAIL;
        }
        if ((db_refs[i] = cpudb_create()) == NULL) {
            cli_out("ERROR: cpudb %d create failed\n", i);
        } else {
            cli_out("cpudb %d created\n", i);
            num_db++;
        }
    } else if (sal_strcasecmp(subcmd, "CURRENT") == 0) {
        if ((int_arg = ARG_GET(args)) == NULL) {
            cli_out("Current db is %d\n", cur_db);
            return CMD_OK;
        }
        cur_db = strtoul(int_arg, NULL, 0);
        cli_out("Current DB set to %d%s\n", cur_db,
                cpudb_valid(db_refs[cur_db]) ? "" :
                " (NOTE:  Not currently valid)");

    } else if ((sal_strcasecmp(subcmd, "KILL") == 0) ||
               (sal_strcasecmp(subcmd, "DESTROY") == 0)) {
        if ((int_arg = ARG_GET(args)) == NULL) {
            cli_out("Destroy requires database refence\n");
            return CMD_USAGE;
        }
        a_db = strtoul(int_arg, NULL, 0);
        if (db_refs[a_db] != NULL) {
            cli_out("Destroy returns %d\n", cpudb_destroy(db_refs[a_db]));
            db_refs[a_db] = NULL;
        } else {
            cli_out("DB %d does not exist\n", a_db);
        }
    } else if (sal_strcasecmp(subcmd, "STACKPORTS") == 0) {
        int db_idx = cur_db;
        char *port_str;

        if (ARG_CUR(args) == NULL) {
            cli_out("stack ports required\n");
            return CMD_FAIL;
        }
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "PortString", PQ_STRING, 0, &port_str, 0);
        parse_table_add(&pt, "DBidx", PQ_DFL|PQ_INT, 0, &db_idx, 0);
        if (parse_arg_eq(args, &pt) < 0) {
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }

        if (db_refs[db_idx] == CPUDB_REF_NULL) {
            cli_out("Invalid DB index.  Try cpudb create\n");
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
        }
        if (db_refs[db_idx]->local_entry == NULL) {
            cli_out("No local DB entry in cpudb %d\n", db_idx);
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
        }
        if (port_str_parse(port_str, db_refs[db_idx]->local_entry) < 0) {
            cli_out("Bad port string specified: %s\n", port_str);
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
        }
        parse_arg_eq_done(&pt);
    } else if (sal_strcasecmp(subcmd, "ADD") == 0) {
        int  is_local = FALSE;
        char keybuf[CPUDB_KEY_STRING_LEN];

        parse_table_init(unit, &pt);
        parse_table_add(&pt, "KEY", PQ_DFL|PQ_KEY, 0, key.key, 0);
        parse_table_add(&pt, "Local", PQ_DFL|PQ_BOOL, 0, &is_local, 0);
        if (parse_arg_eq(args, &pt) < 0) {
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        parse_arg_eq_done(&pt);

        cpudb_key_format(key, keybuf, sizeof(keybuf));
        entry = cpudb_entry_create(db_refs[cur_db], key, is_local);
        if (entry == NULL) {
            cli_out("ERROR: cpudb entry %s create failed\n", keybuf);
        } else {
            cli_out("cpudb entry %s created\n", keybuf);
            atp_key_add(key, is_local);
        }
    } else if ((sal_strcasecmp(subcmd, "UPDATE") == 0)) {

        parse_table_init(unit, &pt);
        sal_memcpy(mac, _bcast_mac, sizeof(bcm_mac_t));

        parse_table_add(&pt, "KEY", PQ_DFL|PQ_KEY, 0, key.key, 0);
        parse_table_add(&pt, "MAC", PQ_DFL|PQ_MAC, 0, mac, 0);
        parse_table_add(&pt, "TXUnit", PQ_DFL|PQ_INT, 0, &tx_unit, 0);
        parse_table_add(&pt, "TXPort", PQ_DFL|PQ_INT, 0, &tx_port, 0);
        parse_table_add(&pt, "DMod", PQ_DFL|PQ_INT, 0, &dest_mod, 0);
        parse_table_add(&pt, "DPort", PQ_DFL|PQ_INT, 0, &dest_port, 0);
        parse_table_add(&pt, "MasterPri", PQ_DFL|PQ_INT, 0, &master_pri, 0);
        parse_table_add(&pt, "NumUnits", PQ_DFL|PQ_INT, 0, &num_units, 0);
        parse_table_add(&pt, "SlotID", PQ_DFL|PQ_INT, 0, &slot_id, 0);
        parse_table_add(&pt, "DestUnit", PQ_DFL|PQ_INT, 0,
                        &dest_unit, 0);

        if (parse_arg_eq(args, &pt) < 0) {
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }

        CPUDB_KEY_SEARCH(db_refs[cur_db], key, entry);
        if (entry == NULL) {
            cli_out("Could not find entry for key\n");
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
        }

        if (tx_unit >= 0) {
            entry->tx_unit = tx_unit;
            entry->flags |= CPUDB_F_TX_KNOWN;
        }
        if (tx_port >= 0) {
            entry->tx_port = tx_port;
        }
        if (dest_mod >= 0) {
            entry->dest_mod = dest_mod;
            entry->flags |= CPUDB_F_DEST_KNOWN;
        }
        if (dest_port >= 0) {  /* For now, set both base and entry port */
            entry->base.dest_port = dest_port;
            entry->dest_port = dest_port;
        }
        if (num_units >= 0) {
            entry->base.num_units = num_units;
        }
        if (dest_unit >= 0) {
            entry->base.dest_unit = dest_unit;
        }
        if (slot_id >= 0) {
            entry->base.slot_id = slot_id;
        }
        if (sal_memcmp(mac, _bcast_mac, sizeof(bcm_mac_t))) {
            entry->flags |= CPUDB_F_BASE_INIT_DONE;
            sal_memcpy(entry->base.mac, mac, sizeof(bcm_mac_t));
        }

        if ((tx_unit >= 0 && tx_port < 0) || (tx_port >= 0 && tx_unit < 0)) {
            cli_out("Warning:  Not properly initializing tx_unit (%d) and "
                    "tx_port (%d)\n", tx_unit, tx_port);
        }
        if ((dest_mod >= 0 && dest_port < 0) ||
            (dest_port >= 0 && dest_mod < 0)) {
            cli_out("Warning:  Not properly initializing dest_mode (%d) and "
                    "dest_port (%d)\n", dest_mod, dest_port);
        }

        parse_arg_eq_done(&pt);
    } else if ((sal_strcasecmp(subcmd, "LOOKUP") == 0) ||
               (sal_strcasecmp(subcmd, "FIND") == 0)) {
        const cpudb_entry_t *entry;

        parse_table_init(unit, &pt);
        parse_table_add(&pt, "MAC", PQ_DFL|PQ_MAC, 0, mac, 0);
        if (parse_arg_eq(args, &pt) < 0) {
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        parse_arg_eq_done(&pt);

        if ((entry = cpudb_mac_lookup(db_refs[cur_db], mac)) != NULL) {
            db_entry_show(entry);
        } else {
            cli_out("Entry not found in database %d\n", cur_db);
        }

    } else if (sal_strcasecmp(subcmd, "SHOW") == 0) {
        int valid;
        int db_idx;

        cli_out("Current DB index is %d and is%s valid\n", cur_db,
                cpudb_valid(db_refs[cur_db]) ? "" : " NOT");
        for (db_idx = 0; db_idx < MAX_DBS; db_idx++) {
            valid = cpudb_valid(db_refs[db_idx]);
            if (valid) {
                cli_out("\nDB index %d:\n", db_idx);
                db_dump(db_refs[db_idx]);
            }
        }
    } else if (sal_strcasecmp(subcmd, "REMOVE") == 0) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "KEY", PQ_DFL|PQ_KEY, 0, key.key, 0);
        if (parse_arg_eq(args, &pt) < 0) {
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        parse_arg_eq_done(&pt);
        cli_out("Remove returns %d\n",
                cpudb_entry_remove(db_refs[cur_db], key));
    } else {
        cli_out("Subcommand not found: %s\n", subcmd);
        return CMD_USAGE;
    }

    return CMD_OK;
}

#if 0  
/****************************************************************
 *
 * TOPOLOGY callbacks
 */

static topo_cpu_t *tp_cpu;

#define TP_CPU_INIT \
    if (tp_cpu == NULL) { \
        tp_cpu = sal_alloc(sizeof(*tp_cpu), "tp_cpu"); \
        sal_memset(tp_cpu, 0, sizeof(*tp_cpu)); \
    }

#endif  

/****************************************************************
 *
 * StackTask thread
 */

/*
 * Stack update callback function
 *
 * This function is registered with stk_port_set as a callback.
 * Currently, it interprets the cookie as the COS on which
 * stack (next hop) traffic flows and sets up the protocol priority
 * on enabled, SL stack ports according to that value.
 */

STATIC void
_stk_port_update(int unit, bcm_port_t port, uint32 flags, void *cookie)
{
    int stk_cos;
    int rv;

    if (flags & BCM_STK_ENABLE) {
        if (flags & BCM_STK_SL) {
            stk_cos = PTR_TO_INT(cookie);
            LOG_VERBOSE(BSL_LS_BCM_STK,
                        (BSL_META_U(unit,
                                    "STACK: port update: unit %d, port %d, flags 0x%x,"
                                    "stk_cos %d\n"),
                         unit, port, flags, stk_cos));

            rv = bcm_switch_control_port_set(unit, port,
                                             bcmSwitchCpuProtocolPrio,
                                             stk_cos);
            if ((rv < 0) && (rv != BCM_E_UNAVAIL)) {
                cli_out("STACK: Error setting CPU protocol priority"
                        " on unit %d, port %d: %s\n", unit, port,
                        bcm_errmsg(rv));
            }
        }
    } else {
        /* Do any special processing needed for a port which is
         * removed from stacking.
         */
    }
}


/*
 * Workaround for MH Opcode 0 Priority issue on devices,
 *     5650x A0, A1
 *     5630x A0, B0
 *     5610x A0
 */

#define MH_OPCODE0_INIT    0
#define MH_OPCODE0_SELECT  1
#define MH_OPCODE0_CLEAR   2

#if defined(BCM_FIELD_SUPPORT) && defined(BCM_FIREBOLT_SUPPORT)
/*
 * Function:
 *     mh_opcode0_priority_select
 * Description:
 *     Creates FP rule to redirect incoming HiGig port packets with
 *     MH Opcode 0 to the CPU port.  In addition, it clears the 
 *     'copy-to-cpu' in the icontrol_opcode_bitmap registers for the HiGig.
 *     
 * Parameters:
 *     unit           - BCM device number
 *     group_priority - Group priority within allowable range 
 *                      (see bcm_field_status_get),
 *                      or BCM_FIELD_GROUP_PRIO_ANY to automatically assign a
 *                      priority; each priority value may be used only once
 *     group          - (OUT) New FP Group ID 
 *     entry          - (OUT) New FP Entry ID
 * Returns:
 *     BCM_E_NONE     Success
 *     BCM_E_XXX      Failure,
 *                    - NULL pointers
 *                    - FP subsystem is not initialized
 *                    - No more FP resources available
 * Notes:
 *     FP subsystem must be initialized.
 *     This routine should only be executed on (local)
 *         5650x A0, A1
 *         5630x A0, B0
 *         5610x A0
 */

STATIC int
mh_opcode0_priority_select(int unit, 
                           int group_priority,
                           bcm_field_group_t *group,
                           bcm_field_entry_t *entry,
                           int               *obm_reg_changed)
{
    int                 valid_rev = FALSE;
    bcm_port_t          port;
    uint32              obm_reg;
    bcm_field_qset_t    qset;
    uint8               opcode;
    uint8               opcode_mask;
    uint32              action_param0;
    bcm_pbmp_t          in_ports;
    bcm_pbmp_t          in_ports_mask;
    uint16              dev_id;
    uint8               rev_id;


    /* Check device type and revision */
    if ((!SOC_UNIT_VALID(unit))) {
        return BCM_E_NONE;
    }

    /* Need to check the Revision ID of the device, not the driver */
    soc_cm_get_id(unit, &dev_id, &rev_id);

    if (SOC_IS_FIREBOLT(unit) &&
        ((rev_id == BCM56504_A0_REV_ID) ||
         (rev_id == BCM56504_A1_REV_ID))) {
        valid_rev = TRUE;
    } else if (SOC_IS_HELIX(unit) &&
               ((rev_id == BCM56304_A0_REV_ID) ||
                (rev_id == BCM56304_B0_REV_ID))) {
        valid_rev = TRUE;
    } else if (SOC_IS_FELIX(unit) &&
               (rev_id == BCM56107_A0_REV_ID)) {
        valid_rev = TRUE;
    }
    if (!valid_rev) {  /* No error; not for this device */
        return BCM_E_NONE;
    }

  
    /* Valid devide type, revision */

    if ((group == NULL) || (entry == NULL)) {
        return BCM_E_PARAM;
    }

    /* Set the HG ingress CPU Opcode map to NOT forward to CPU */
    PBMP_ST_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(READ_ICONTROL_OPCODE_BITMAPr(unit, port,
                                                         &obm_reg));
        soc_reg_field_set(unit, ICONTROL_OPCODE_BITMAPr, &obm_reg, BITMAPf,
                          0x0);
        SOC_IF_ERROR_RETURN(WRITE_ICONTROL_OPCODE_BITMAPr(unit, port,
                                                          obm_reg));
        *obm_reg_changed = TRUE;
    }


    /* Select qualifiers: MHOpcode, Ingress Ports */
    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyMHOpcode);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPorts);
    
    /* Create group */
    BCM_IF_ERROR_RETURN(bcm_field_group_create(unit, qset, 
                                               group_priority, group));
    /* Create entry */
    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, *group, entry));

    /* Set qualifiers data, mask */
    /* MHOpcode */
    opcode = BCM_FIELD_MHOPCODE_CONTROL;
    opcode_mask = 0x7;   /* Match all bits */
    BCM_IF_ERROR_RETURN(bcm_field_qualify_MHOpcode(unit, *entry, 
                                                   opcode, opcode_mask));
    /* Ingress Ports */
    BCM_PBMP_ASSIGN(in_ports, PBMP_ST_ALL(unit));
    BCM_PBMP_ASSIGN(in_ports_mask, PBMP_PORT_ALL(unit));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_InPorts(unit, *entry, 
                                                  in_ports, in_ports_mask));

    /* Set action to redirect to CPU port */
    action_param0 = SOC_PBMP_WORD_GET(PBMP_CMIC(unit), 0);
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, *entry, 
                                             bcmFieldActionRedirectPbmp,
                                             action_param0, 0));

    return bcm_field_entry_install(unit, *entry);
}


/*
 * Function:
 *     mh_opcode0_priority_clear
 * Description:
 *     It destroys the FP rule for the given group ID and entry ID.
 *     In addition, it sets the 'copy-to-cpu' flag in the icontrol_opcode_bitmap
 *      registers for the HiGig.
 *
 * Parameters:
 *     unit           - BCM device number
 *     group          - FP Group ID 
 *     entry          - FP Entry ID
 * Returns:
 *     BCM_E_NONE     Success
 *     BCM_E_XXX      Failure
 * Notes:
 *     This routine is used in conjuction with mh_opcode0_priority_select.
 *     This should only be called on those units where the 
 *     mh_opcode0_priority_select was successfully called.  The FP group 
 *     and entry IDs should have been saved earlier and passed in 
 *     to this routine.
 *     
 *     Given the above, the device type and rev will NOT be validated.
 */

STATIC int
mh_opcode0_priority_clear(int unit, 
                          bcm_field_group_t group,
                          bcm_field_entry_t entry,
                          int obm_reg_changed)
{
    bcm_port_t          port;
    uint32              obm_reg;

    /* Destroy entry */
    if (entry != -1) {
        BCM_IF_ERROR_RETURN(bcm_field_entry_remove(unit, entry));
        BCM_IF_ERROR_RETURN(bcm_field_entry_destroy(unit, entry));
    }

    /* Destroy group */
    if (group != -1) {
        BCM_IF_ERROR_RETURN(bcm_field_group_destroy(unit, group));
    }

    /* Set HG ingress CPU Opcode map to COPY to the CPU */
    if (obm_reg_changed) {
        PBMP_ST_ITER(unit, port) {
            SOC_IF_ERROR_RETURN(READ_ICONTROL_OPCODE_BITMAPr(unit, port,
                                                             &obm_reg));
            soc_reg_field_set(unit, ICONTROL_OPCODE_BITMAPr, &obm_reg, BITMAPf,
                              0x10);
            SOC_IF_ERROR_RETURN(WRITE_ICONTROL_OPCODE_BITMAPr(unit, port,
                                                              obm_reg));
        }
    }

    return BCM_E_NONE;
}

typedef struct mh_opcode0_fp_s {
    bcm_field_group_t   group;
    bcm_field_entry_t   entry;
    int                 obm_reg_changed;
} mh_opcode0_fp_t;

/*
 * Function:
 *     mh_opcode0_priority
 * Description:
 *     This routines performs the specified action for the MH Opcode 0
 *     priority.
 *     
 * Parameters:
 *     action         - Action to take,
 *                        MH_OPCODE0_INIT    Initializes static data; must 
 *                                           be called first
 *                        MH_OPCODE0_SELECT  Calls 'priority_select'
 *                        MH_OPCODE0_CLEAR   Calls 'priority_clear'
 * Returns:
 *     BCM_E_NONE     Success
 *     BCM_E_XXX      Failure
 */

STATIC int
mh_opcode0_priority(int action)
{
    int i;
    int rv;
    static int init = FALSE;
    static mh_opcode0_fp_t mh_opcode0_fp[SOC_MAX_NUM_DEVICES];

    if (!init && (action != MH_OPCODE0_INIT)) {
        return BCM_E_INIT;
    }

    switch (action) {
    case MH_OPCODE0_INIT:
        for (i = 0; i < (sizeof(mh_opcode0_fp)/sizeof(mh_opcode0_fp_t)); i++) {
            mh_opcode0_fp[i].group = -1;
            mh_opcode0_fp[i].entry = -1;
            mh_opcode0_fp[i].obm_reg_changed = FALSE;
        }
        init = TRUE;
        break;

    case MH_OPCODE0_SELECT:
        for (i = 0; i < soc_ndev; i++) {

            rv = mh_opcode0_priority_select(SOC_NDEV_IDX2DEV(i),
                                            BCM_FIELD_GROUP_PRIO_ANY,
                                            &mh_opcode0_fp[SOC_NDEV_IDX2DEV(i)].group,
                                            &mh_opcode0_fp[SOC_NDEV_IDX2DEV(i)].entry,
                                            &mh_opcode0_fp[SOC_NDEV_IDX2DEV(i)].obm_reg_changed);
            if (BCM_FAILURE(rv)) {
                cli_out("ERROR: Failed to create FP rule to use HiGig packet "
                        "priority on unit %d.\n",SOC_NDEV_IDX2DEV(i));
                cli_out("       Stacking may not work properly.\n");
                return rv;
            }
        }
        break;

    case MH_OPCODE0_CLEAR:
        /* Destroy created FP rule on units */
        for (i = 0; i < soc_ndev; i++) {
            mh_opcode0_priority_clear(SOC_NDEV_IDX2DEV(i),
                                      mh_opcode0_fp[SOC_NDEV_IDX2DEV(i)].group,
                                      mh_opcode0_fp[SOC_NDEV_IDX2DEV(i)].entry,
                                      mh_opcode0_fp[SOC_NDEV_IDX2DEV(i)].obm_reg_changed);
            mh_opcode0_fp[SOC_NDEV_IDX2DEV(i)].group = -1;
            mh_opcode0_fp[SOC_NDEV_IDX2DEV(i)].entry = -1;
            mh_opcode0_fp[SOC_NDEV_IDX2DEV(i)].obm_reg_changed = FALSE;
        }
        break;
        
    default:
        break;
    }

    return BCM_E_NONE;
}

#else 

STATIC int
mh_opcode0_priority(int action)
{
    COMPILER_REFERENCE(action);
    return BCM_E_NONE;
}

#endif  /* BCM_FIELD_SUPPORT && BCM_FIREBOLT_SUPPORT */


#define TKS_STK_DEFAULT_THREAD_PRIORITY 100
static int st_pri = TKS_STK_DEFAULT_THREAD_PRIORITY;
static sal_thread_t st_tid = SAL_THREAD_ERROR;
static sal_sem_t st_done_sem;

#define TOPO_ATP_FLAGS (ATP_F_NEXT_HOP | ATP_F_REASSEM_BUF)

STATIC void
tks_st_thread(void *cookie)
{
    bcm_st_config_t *cfg;
    int rv, i;
    sal_usecs_t attach_timeout;

    /* The following call is needed as a workaround for
     * the MHOpcode 0 priority issue in FB, HX, and FX devices
     */
    mh_opcode0_priority(MH_OPCODE0_INIT);
    rv = mh_opcode0_priority(MH_OPCODE0_SELECT);
    if (BCM_FAILURE(rv)) {
        mh_opcode0_priority(MH_OPCODE0_CLEAR);
    }

    cfg = (bcm_st_config_t *)cookie;

    if ((rv=topo_pkt_handle_init(TOPO_ATP_FLAGS)) != BCM_E_NONE) {
        cli_out("WARNING: topo_pkt_handle_init returned %s\n", bcm_errmsg(rv));
    }

    if ((rv=bcm_stack_attach_init()) != BCM_E_NONE) {
        cli_out("WARNING: bcm_stack_attach_init returned %s\n", bcm_errmsg(rv));
    }

    rv = bcm_st_start(cfg, TRUE);
    cli_out("ST thread exited with value %d (%s)\n", rv, bcm_errmsg(rv));
    st_tid = SAL_THREAD_ERROR;

    cli_out("ST: Stopping RPC and rlink\n");
    
#define TKS_STK_THREAD_POLL_FRACTION 100
    /* Make sure an attach thread is not running before stopping RPC */
    if (bcm_st_timeout_get(BCM_STS_ATTACH,
                           &attach_timeout) == BCM_E_NONE) {
        /* Wait twice the attach timeout for stktask to stop */
        for (i=0; i<2*TKS_STK_THREAD_POLL_FRACTION; i++) {
            if (!bcm_stack_attach_running()) {
                break;
            }
            sal_usleep(attach_timeout/TKS_STK_THREAD_POLL_FRACTION);
        }

        if (bcm_stack_attach_running()) {
            cli_out("Attach thread failed to signal completion\n");
        }
        
    } else {
        cli_out("Could not get attach timeout\n");
    }

#ifdef BCM_RPC_SUPPORT
    /* Stop RLINK and RPC */
    (void)bcm_rlink_stop();
    (void)bcm_rpc_stop();
#endif

    mh_opcode0_priority(MH_OPCODE0_CLEAR);
    sal_sem_give(st_done_sem);
    sal_thread_exit(rv);
}

typedef struct {
    int timeout;
    int rv;
} tks_st_stop_thread_arg_t;

STATIC void
tks_st_stop_thread(void *cookie)
{
    tks_st_stop_thread_arg_t *arg = (tks_st_stop_thread_arg_t *)cookie;

    arg->rv = bcm_st_stop(arg->timeout);

    sal_thread_exit(0);
}

static int
null_attach(cpudb_ref_t db_ref)
{
    int ismaster = db_ref->local_entry == db_ref->master_entry;
    char master[CPUDB_KEY_STRING_LEN];
    char slave[CPUDB_KEY_STRING_LEN];

    master[0] = 0;
    slave[0] = 0;
    if (db_ref->master_entry) {
        cpudb_key_format(db_ref->master_entry->base.key,
			 master, sizeof(master));
    }
    cpudb_key_format(db_ref->local_entry->base.key, slave, sizeof(slave));
    
    if (ismaster) {
	cli_out("STACK: master on %s (%d cpu%s)\n",
                master,
                db_ref->num_cpus, db_ref->num_cpus == 1 ? "" : "s");
    } else {
	cli_out("STACK: slave on %s (%d cpus, master %s)\n",
                slave, db_ref->num_cpus, master);
    }

    bcm_st_event_send(BCM_STE_ATTACH_SUCCESS);
    return BCM_E_NONE;
}


#if defined(ST_EVLOG)

STATIC int
evlog_start(void)
{
    int   args = 4;
    int   entries = 100;

    /* Create log of size 100 entries with 4 arguments */
    if (st_log == NULL) {
        if ((st_log = diag_evlog_create(args, entries)) == NULL) {
            return BCM_E_MEMORY;
        }
    }
    
    SHARED_EVLOG_ENABLE_ALL(st_log);
    return BCM_E_NONE;
}

STATIC int
evlog_stop(void)
{
    if (st_log == NULL) {
        cli_out("No event log\n");
    } else {
        diag_evlog_destroy(st_log);
        st_log = NULL;
    }

    return BCM_E_NONE;
}

STATIC int
evlog_clear(void)
{
    if (st_log == NULL) {
        cli_out("No event log\n");
    } else {
        evlog_stop();
        evlog_start();
    }

    return BCM_E_NONE;
}

STATIC void
evlog_show(args_t *args)
{
    int           i;
    int           count;
    uint32        flags;
    char          *option;
    int           summary = FALSE;
    int           start, end;
    sal_usecs_t   elapsed_t;
    char          *from = NULL;  /* Previous Stack task state */
    char          *to = NULL;    /* New Stack task state */
    char          *event = NULL; /* Event that triggered state transition */
      
    
    if (st_log == NULL) {
        cli_out("No event log\n");
        return;
    }

    if ((option = ARG_GET(args)) != NULL) {
        if (parse_cmp("Summary", option, '\0')) {
            summary = TRUE;
        }
    }
       
    /* Disable logging */
    flags = SHARED_EVLOG_ENABLE_GET(st_log);
    SHARED_EVLOG_ENABLE_SET(st_log, 0);

    /* Get log count and check for rollover */
    count = SHARED_EVLOG_COUNT(st_log);
    if (st_log->cur_idx < count) {
        count = st_log->max_entries;
    }

    cli_out("Stack Task State Change Event Log");
    if (summary) {
        cli_out(" - Summary");
    }
    cli_out("\n");
    cli_out("  Flags :  0x%x\n", flags);
    cli_out("  Count :  %d\n", count);
    cli_out("\n");
    if (count <= 0) {
        SHARED_EVLOG_ENABLE_SET(st_log, flags);    /* Restore flags */
        return;
    }
    
    
    /*
     * Summary:  Display total time elapsed time for each state
     */
    if (summary) {
        cli_out("  Entry       Elapsed Time   Stack Task State    On\n");
        cli_out("  Start End        (usecs)    From  ==> To       Event\n");
        cli_out("  ----------------------------------------------------\n");

        /*
         * The display for the summary option assumes the following
         * log sequence for getting the elapsed time between two log entries:
         *   Stack starts         -> start of Stack state
         *   Start of Stack state -> End of Stack state
         */

        start = end = -1;
        for (i = 0; i < count; i++) {
            /* Look for start of stack task */
            if (sal_strcasecmp(SHARED_EVLOG_STR(st_log, i), "stack start")
                == 0) {
                start = i++;
                end   = start;
                /* Look for the start of a state */
                for (; i < count; i++) {
                    if (sal_strcasecmp(SHARED_EVLOG_STR(st_log, i), "start")) {
                        continue;
                    }
                    end = i;
                    
                    from  = "(Start)";
                    to    = bcm_st_state_strings[SHARED_EVLOG_ARG(st_log,
                                                                  i, 0)];
                    event = "---";
                    break;
                }
            } else {
            
                /* Look for the start of a state */
                if (start < 0) {
                    if (sal_strcasecmp(SHARED_EVLOG_STR(st_log, i), "start")) {
                        continue;
                    }
                    start = i++;
                }
            
                /* Look for the end of a state change */
                if (sal_strcasecmp(SHARED_EVLOG_STR(st_log, i), "end")) {
                    continue;
                }
                
                end   = i;
                from  = bcm_st_state_strings[SHARED_EVLOG_ARG(st_log, i, 0)];
                to    = bcm_st_state_strings[SHARED_EVLOG_ARG(st_log, i, 1)];
                event = bcm_st_event_strings[SHARED_EVLOG_ARG(st_log, i, 2)];
            }

            if (start == end) {
                continue;
            }
            
            elapsed_t = SHARED_EVLOG_TIMESTAMP(st_log, end) - 
                        SHARED_EVLOG_TIMESTAMP(st_log, start);
            cli_out("   %3d %3d   %12d    %-8s  %-8s  %-15s\n",
                    start, end,
                    elapsed_t,
                    from, to, event);

            start = end;
        }
        cli_out("\n");
        
    } else {
        /*
         * Detail:  Display time elapsed time for each state broken into,
         *          1) Start of state to Event notification to transition
         *             to new state
         *          2) Transitition to new state
         */

        cli_out("  Entry  Time(usecs)  Description   Stack Task State    "
                "On\n");
        cli_out("                                     From  ==> To       "
                "Event\n");
        cli_out("  -------------------------------------------------------"
                "----\n");
        for (i = 0; i < count; i++) {
            if (SHARED_EVLOG_ARG(st_log, i, 0) == BCM_STS_INVALID) {
                from = to = event = "---";
            } else {
                from  = bcm_st_state_strings[SHARED_EVLOG_ARG(st_log, i, 0)];
                to    = bcm_st_state_strings[SHARED_EVLOG_ARG(st_log, i, 1)];
                event = bcm_st_event_strings[SHARED_EVLOG_ARG(st_log, i, 2)];
            }

            cli_out("  %3d  %12d   %-11s   %-8s  %-8s  %-15s\n",
                    i,
                    SHARED_EVLOG_TIMESTAMP(st_log, i),
                    SHARED_EVLOG_STR(st_log, i),
                    from, to, event);
        }
        cli_out("\n");
    }
    
    SHARED_EVLOG_ENABLE_SET(st_log, flags);    /* Restore flags */

    return;
}
#endif /* ST_EVLOG */

/* Multiblock packet allocator */

#define MAX_NUM_BLKS 50

STATIC int
tks_pkt_alloc(int unit, int num_blks, int length, uint32 flags, bcm_pkt_t **pkt)
{
    int i;
    int rv;
    int alength;
    bcm_pkt_t *pkta[MAX_NUM_BLKS];
    bcm_pkt_blk_t *blk = NULL;

    if ((num_blks <= 0) || (num_blks >= MAX_NUM_BLKS)) {
        return BCM_E_PARAM;
    }

    if (num_blks > 1) {
        blk = sal_alloc(num_blks * sizeof(bcm_pkt_blk_t), "tks_pkt_alloc");
    }

    alength = length/num_blks;

    for (i=0; i<num_blks; i++) {

        /* calculate length needed */
        if (num_blks != 1 && i <= num_blks-2) {
            /* not the last block, so use alength for allocation, and
             subtract from length, so lenggth will be how much is
             needed for the final allocation. */
            length = length - alength;
        } else {
            /* last block, which could also be the first and only block */
            alength = length;
        }

        /* Get a pkt */
        rv = bcm_pkt_alloc(unit, alength, flags, &pkta[i]);

        if (i == 0) {
            if (num_blks > 1) {
                blk[i] = *pkta[i]->pkt_data;
                pkta[i]->pkt_data = blk;
            }
        } else {
            /* not the first packet */
            
            /* Accumulate data segments into first packet */
            pkta[0]->pkt_data[i] = pkta[i]->pkt_data[0];
            pkta[0]->blk_count += 1;

            /* Just needed the packet for its data */
            pkta[i]->blk_count -= 1;
            rv = bcm_pkt_free(unit, pkta[i]);
            if (BCM_FAILURE(rv)) {
                return rv;
            }
        }
    }

    *pkt = pkta[0];

    return BCM_E_NONE;
}

STATIC int
tks_pkt_free(int unit, bcm_pkt_t *pkt)
{
    int rv, num_blks;
    bcm_pkt_blk_t    *blk;

    num_blks = pkt->blk_count;
    blk = pkt->pkt_data;
    rv = bcm_pkt_free(unit, pkt);
    if (num_blks > 1) {
        sal_free(blk);
    }
    return rv;
}

STATIC void
tks_list_callback(int unit, bcm_pkt_t *pkt, void *cookie)
{
    bcm_pkt_t *next;
    int rv;

    cli_out("tks_list_callback(%d,%p,%p)\n",unit,(void *)pkt,cookie);
    while(pkt) {
        next = pkt->next;
        rv = tks_pkt_free(pkt->unit, pkt);
        if (BCM_FAILURE(rv)) {
            cli_out("tks_list_callback: tks_pkt_free()=%d\n",rv);
        }
        pkt = next;
    }
}

STATIC void
tks_array_callback(int unit, bcm_pkt_t *pkt, void *cookie)
{
    tks_list_callback(unit,pkt, (void *)3);
    sal_free(cookie);
}

STATIC void
tks_pkt_callback(int unit, bcm_pkt_t *pkt, void *cookie)
{
    int rv;
    sal_vaddr_t ctl = PTR_TO_UINTPTR(cookie);
    void *pktp = (void *)pkt;

    if (ctl == 1) {
        rv = tks_pkt_free(unit, pkt);
        /* Coverity will complain if 'pkt' is printed because a function
           is passed a de-allocated resource */
        cli_out("tks_pkt_callback(%d,%p,%p)=%d\n",unit,pktp,cookie,rv);
    } else {
        cli_out("tks_pkt_callback [list]\n");
    }
}

static bcm_st_config_t st_cfg = {
    disc_start,
    disc_abort,
    NULL,              /* Use default master election routine */

    NULL,              /* Use default topology update function */
    NULL,              /* Use default attach update function */
    NULL,              /* Use default transition notify function */

    /* Base is filled in from current db */
    {{{0}}},
};

static char *event_list[] = { EVENT_LIST_NAMES };

char tks_stk_task_usage[] =
    "StkTask START [RETRX=<n>] [CfgTimeout=<n>] [TimeOut=<n>] [DBidx=<n>]\n"
    "         [COS=<n>] [VLAN=<n>] [DiscFallBack=<T|F>]\n"
#ifndef COMPILER_STRING_CONST_LIMIT
    "    Start the stack task (if not running).\n"
    "StkTask EVENT Event=<UNBlock | BLock | LINKUP | LINKDOWN | PKT |\n"
    "         DRestart | DSuccess | DFailure | TSuccess | TFailure |\n"
    "         ASuccess | AFailure | TimeOut >\n"
    "    Send the indicated event to the stack task (to be implemented)\n"
    "StkTask UPDATE DBidx=<n> [REStart=<T|F>] [MasterPri=n]\n"
    "    Update the stack task base configuration with that of the\n"
    "    given CPU database\n"
    "StkTask TIMEOUT State=<state> TimeOut=<usec>\n"
    "    Set the timeout for the given state\n"
    "StkTask INIT\n"
    "    Init stack task without starting it\n"
    "StkTask DISC [Version=<n>] [FallBack=<T|F>]\n"
    "    Set or display the current discovery version and fallback setting\n"
    "StkTask STOP\n"
    "    Stop the stack task (if running).\n"
    "StkTask TOPO\n"
    "    Dump the topology database\n"
#if defined(ST_EVLOG)
    "StkTask EVLOG {CLEAR | SHOW [summary]}\n"
    "    Stack task event logging\n"
#endif /* ST_EVLOG */
#endif
    ;

extern bcm_st_state_t st_state;
extern volatile uint32 st_blocked_events;
extern volatile uint32 st_pending_events;
extern volatile sal_usecs_t st_transition_time;
extern uint32 st_stk_port_flags[CPUDB_CXN_MAX];
extern sal_usecs_t stk_port_last_event_us[CPUDB_CXN_MAX];
extern uint32 bcm_st_flags;
extern int _topo_hw_linkscan;


cmd_result_t
tks_stk_task(int unit, args_t *args)
{
    parse_table_t pt;
    int rv;
    char *subcmd;
    cpudb_ref_t db_ref;

    if ((subcmd = ARG_GET(args)) == NULL) {
        int i;

        cli_out("Stack Task thread is%s running, state %s, flags 0x%x\n",
                st_tid == SAL_THREAD_ERROR ? " not" : "",
                bcm_st_state_strings[st_state],
                bcm_st_flags);
        for (i = 0; i < 32; i++) {
            if (bcm_st_flags & (1 << i)) {
                if (i < BCM_STF_MAX) {
                    cli_out("    %s\n", bcm_st_flags_strings[i]);
                } else {
                    cli_out("    Unknown flag 0x%x\n", bcm_st_flags & (1 << i));
                }
            }
        }
        cli_out("ST pending ev 0x%x; blocked 0x%x; last trans %u\n",
                st_blocked_events, st_pending_events,
                st_transition_time);
        cli_out("Stk ports with link:\n");
        for (i = 0; i < CPUDB_CXN_MAX; i++) {
            if (st_stk_port_flags[i] != 0) {
                cli_out("    %d%s: Last event %u. Link %s; Last link %s\n",
                        i, st_stk_port_flags[i] & ST_SPF_DISABLED ?
                        " (disabled)" : "",
                        stk_port_last_event_us[i],
                        st_stk_port_flags[i] & ST_SPF_LINK_UP ?
                        "up" : "down",
                        st_stk_port_flags[i] & ST_SPF_LAST_EVENT_UP ?
                        "up" : "down");
            }
        }

        if (st_state == BCM_STS_BLOCKED || st_state == BCM_STS_READY) {
            cpudb_ref_t disc = NULL;
            cpudb_ref_t cur = NULL;
            
            if (!cpudb_valid((disc = bcm_st_discovery_db_get()))) {
                cli_out("Stack task discovery DB is not valid\n");
            } else {
                cli_out("Stack task discovery DB:\n");
                db_dump(disc);
            }
            if (!cpudb_valid((cur = bcm_st_current_db_get()))) {
                cli_out("Stack task current DB is not valid\n");
            } else {
                cli_out("Stack task current DB:\n");
                db_dump(cur);
            }
            (void)cpudb_destroy(disc);
            (void)cpudb_destroy(cur);
        } else {
            cli_out("Disc DB %p; Cur DB %p\n", 
                    (void *)bcm_st_disc_db, (void *)bcm_st_cur_db);
            cli_out("Not dumping DBs due to current state\n");
        }
        return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "START") == 0) {
        static int retrx_us = -1;
        static int cfg_to_us = -1;
        static int timeout_us = -1;
        int disc_vlan = -1;
        int disc_cos = -1;
        int disc_fallback = FALSE;
        int auto_trunk = -1;
        int load_balance = -1;
        char *board_id_str = NULL;
        int reserved_modid = -1;
        int db_idx = cur_db;
        uint32 modid_init = 0;
        bcm_st_config_t *cfg = &st_cfg;
        int i;
        bcm_pbmp_t vports, vports_untag;
        int hw_linkscan = TRUE;
        int topo_dly = -1;
        int topo_master_dly = -1;
        int need_board_handler = TRUE;

        if (st_tid != SAL_THREAD_ERROR) {
            cli_out("Stack Task is already running\n");
            return CMD_OK;
        }

        parse_table_init(unit, &pt);
        parse_table_add(&pt, "DBidx", PQ_DFL|PQ_INT, 0, &db_idx, 0);
        parse_table_add(&pt, "RETRX", PQ_DFL|PQ_INT, 0, &retrx_us, 0);
        parse_table_add(&pt, "TimeOut", PQ_DFL|PQ_INT, 0, &timeout_us, 0);
        parse_table_add(&pt, "CfgTimeout", PQ_DFL|PQ_INT, 0, &cfg_to_us, 0);
        parse_table_add(&pt, "Priority", PQ_DFL|PQ_INT, 0, &st_pri, 0);
        parse_table_add(&pt, "COS", PQ_DFL|PQ_INT, 0, &disc_cos, 0);
        parse_table_add(&pt, "VLAN", PQ_DFL|PQ_INT, 0, &disc_vlan, 0);
        parse_table_add(&pt, "rsvdmodidbmp", PQ_DFL|PQ_INT, 0, &modid_init, 0); 
        parse_table_add(&pt, "AutoTrunk", PQ_DFL|PQ_INT, 0, &auto_trunk, 0);
        parse_table_add(&pt, "ReservedModId", PQ_DFL|PQ_INT, 0,
                        &reserved_modid, 0);
        parse_table_add(&pt, "LoadBalance", PQ_DFL|PQ_INT, 0,
                        &load_balance, 0);
        parse_table_add(&pt, "Board", PQ_DFL|PQ_STRING, 0,
                        &board_id_str, 0);
        parse_table_add(&pt, "DiscFallBack", PQ_DFL|PQ_BOOL, 0,
                        &disc_fallback, 0);
        parse_table_add(&pt, "HWLINKscan", PQ_DFL|PQ_BOOL, 0,
                        &hw_linkscan, 0);
        parse_table_add(&pt, "TopoDelay", PQ_DFL|PQ_INT, 0,
                        &topo_dly, 0);
        parse_table_add(&pt, "TopoMasterDelay", PQ_DFL|PQ_INT, 0,
                        &topo_master_dly, 0);
        
        if (parse_arg_eq(args, &pt) < 0) {
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        disc_timeout_set(timeout_us, cfg_to_us, retrx_us);
        if (disc_cos >= 0) {
            disc_cos_set(disc_cos);
            rv = bcm_stk_update_callback_register(0, _stk_port_update,
                                                  INT_TO_PTR(disc_cos));
            if (rv < 0) {
                cli_out("ST: Could not set stack callback: %s\n",
                        bcm_errmsg(rv));
            }
        }
        if (disc_vlan >= 0) {
            disc_vlan_set(disc_vlan);
        }
        if (disc_fallback) {
            disc_fallback_set(TRUE);
        }

        if (modid_init != 0) {
            topo_reserved_modid_set(modid_init, TRUE);
        }

        /* Do not allow HW linkscan mode to be set by Stack task */
        if (hw_linkscan == 0) {
            _topo_hw_linkscan = 0;
        }
        
        if (auto_trunk >= 0) {
            bcm_board_auto_trunk_set(auto_trunk);
        }

        if (reserved_modid >= 0) {
            bcm_st_reserved_modid_enable_set(reserved_modid);
        }

        if (topo_dly >= 0) {
            topo_delay_set(topo_dly);
        }

        if (topo_master_dly >= 0) {
            topo_master_delay_set(topo_master_dly);
        }

        if (!cpudb_valid(db_refs[db_idx]) ||
            db_refs[db_idx]->local_entry == NULL) {
            cfg = NULL;
        } else {

            db_ref = db_refs[db_idx];
            disc_vlan_get(&disc_vlan);
            BCM_PBMP_CLEAR(vports_untag);

            /* Set application flags */
            if (load_balance > 0) {
                db_ref->local_entry->base.flags |= CPUDB_BASE_F_LOAD_BALANCE;
            }
            if (load_balance > 1) {
                db_ref->local_entry->base.flags |= CPUDB_BASE_F_CHASSIS_AST;
            }
#if defined(INCLUDE_BOARD)
            if (board_name() != NULL) {
                need_board_handler = FALSE;
            }
#endif
            if (need_board_handler) {
                rv = topo_board_register(topo_board_default_board_program,
                                         NULL);
                if (rv < 0) {
                    cli_out("Could not register board programming\n");
                    return CMD_FAIL;
                }
            }

            if (board_id_str != NULL) {
                rv = cpudb_board_id_get(board_id_str,
                                        &db_ref->local_entry->base.board_id);
                if (BCM_FAILURE(rv)) {
                    cli_out("ERROR: Could not find board id for '%s'\n",
                            board_id_str);
                    return CMD_FAIL;
                } else {
                    /* Initialize board */
                    rv = topo_board_initialize(db_ref);
                    if (BCM_FAILURE(rv)) {
                        cli_out("ERROR: Could not initialize board '%s'\n",
                                board_id_str);
                        return CMD_FAIL;
                    } else {
                        cli_out("Initialized board %s\n", board_id_str);
                    }
                }
            } else {
                cli_out("ERROR: No board id\n");
                return CMD_FAIL;
            }
            /* Put CPU in VLAN for all devices */
            for (i = 0; i < db_ref->local_entry->base.num_units; i++) {
                rv = bcm_vlan_create(i, disc_vlan);
                if (rv < 0 && rv != BCM_E_EXISTS) {
                    cli_out("ERROR: vlan %d create failed: %s\n",
                            disc_vlan, bcm_errmsg(rv));
                    return CMD_FAIL;
                }
                BCM_PBMP_CLEAR(vports);
                BCM_PBMP_PORT_ADD(vports, CMIC_PORT(i)); 
                rv = bcm_vlan_port_add(i, disc_vlan,
                                       vports, vports_untag);
                if (rv < 0) {
                    cli_out("ERROR: vlan %d CPU port add unit %d failed: %s\n",
                            disc_vlan, unit, bcm_errmsg(rv));
                    return CMD_FAIL;
                }
            }
            /*
             * For each local stack port, if SL, set DISABLE_IF_CUT;
             * Set the weight inversely to the speed
             */
            for (i = 0; i < db_ref->local_entry->base.num_stk_ports; i++) {
                int speed;
                cpudb_unit_port_t *sp =
                    &db_ref->local_entry->base.stk_ports[i];
                
                if (bcm_port_speed_get(sp->unit, sp->port, &speed) < 0) {
                    cli_out("Error getting port speed unit %d, port %d\n",
                            sp->unit, sp->port);
                    return CMD_FAIL;
                }
                if (speed > 0) {
                    if (speed < 2500) {  /* Assume SL port */
                        sp->bflags |= CPUDB_UPF_DISABLE_IF_CUT;
                    }
                    sp->weight = 1000000/speed;
                } else {
                    /* The port speed is less than or equal to zero,
                       so set to the highest weight. */
                    sp->weight = 1000000;
                }
            }
            sal_memcpy(&st_cfg.base, &db_ref->local_entry->base,
                       sizeof(cpudb_base_t));
        }

        if (st_done_sem == NULL) {

            st_done_sem = sal_sem_create("STDoneSem", sal_sem_BINARY, 0);

            if (st_done_sem == NULL) {
                cli_out("Could not create semaphore\n");
                return CMD_FAIL;
            }
            
        }

        st_tid = sal_thread_create("bcmSTACK",
                                   SAL_THREAD_STKSZ,
                                   st_pri,
                                   tks_st_thread,
                                   cfg);
#if defined(ST_EVLOG)
        /* Start event logging for Stack task */
        if (st_log != NULL) {
            evlog_stop();
        }
        rv = evlog_start();
        if (rv < 0) {
            cli_out("Cannot start event logging\n");
        } else {
            SHARED_EVENT_LOG(st_log, "STACK START",
                             BCM_STS_INVALID, BCM_STS_INVALID, 0, 0);
        }
#endif /* ST_EVLOG */
    
        parse_arg_eq_done(&pt);

        return CMD_OK;
    } else if (sal_strcasecmp(subcmd, "UPDATE") == 0) {
        int db_idx = cur_db;
        int restart = FALSE;
        int master_pri = -1;

        parse_table_init(unit, &pt);
        parse_table_add(&pt, "DBidx", PQ_DFL|PQ_INT, 0, &db_idx, 0);
        parse_table_add(&pt, "REStart", PQ_DFL|PQ_BOOL, 0, &restart, 0);
        parse_table_add(&pt, "MasterPri", PQ_DFL|PQ_INT, 0, &master_pri, 0);
        if (parse_arg_eq(args, &pt) < 0) {
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        parse_arg_eq_done(&pt);
        if (!cpudb_valid(db_refs[db_idx]) ||
            db_refs[db_idx]->local_entry == NULL) {
            cli_out("Invalid DB index.  Try cpudb create\n");
            return CMD_FAIL;
        }
        
        if (master_pri >= 0) {
            db_refs[db_idx]->local_entry->base.master_pri = master_pri;
        }
        
        rv = bcm_st_base_update(&db_refs[db_idx]->local_entry->base,
                                restart);
        if (rv < 0) {
            cli_out("ST base update ERROR %d: %s\n", rv, bcm_errmsg(rv));
            return CMD_FAIL;
        }

    } else if (sal_strcasecmp(subcmd, "DISC") == 0) {
        int ver = -1;
        int disc_fallback = -1;
        int rv;

        if (ARG_CNT(args) == 0) {
            if ((rv = disc_version_get(&ver)) < 0) {
                cli_out("Failed to get current discovery version: %s\n",
                        bcm_errmsg(rv));
                return CMD_FAIL;
            } else {
                cli_out("Current discovery version: %d\n", ver);
            }
            if ((rv = disc_fallback_get(&disc_fallback)) < 0) {
                cli_out("Failed to get discovery fallback setting: %s\n",
                        bcm_errmsg(rv));
                return CMD_FAIL;
            } else {
                cli_out("Fallback is %s\n",
                        disc_fallback ? "enable" : "disable");
            }
            return CMD_OK;
        }

        parse_table_init(unit, &pt);
        parse_table_add(&pt, "Version", PQ_DFL|PQ_INT, 0, &ver, 0);
        parse_table_add(&pt, "FallBack", PQ_DFL|PQ_BOOL, 0, &disc_fallback, 0);
        if (parse_arg_eq(args, &pt) < 0) {
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        parse_arg_eq_done(&pt);

        if (ver != -1) {
            if ((rv = disc_version_set(ver)) < 0) {
                cli_out("Failed to set discovery version %d: %s\n", ver,
                        bcm_errmsg(rv));
                return CMD_FAIL;
            }
        }
        if (disc_fallback != -1) {
            if ((rv = disc_fallback_set(disc_fallback)) < 0) {
                cli_out("Failed to %s discovery version fallback: %s\n",
                        disc_fallback ? "enable" : "disable",
                        bcm_errmsg(rv));
                return CMD_FAIL;
            }
        }     

        return CMD_OK;
    } else if (sal_strcasecmp(subcmd, "INIT") == 0) {
        int rv;
        int db_idx = cur_db;

        parse_table_init(unit, &pt);
        parse_table_add(&pt, "DBidx", PQ_DFL|PQ_INT, 0, &db_idx, 0);
        if (parse_arg_eq(args, &pt) < 0) {
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        if (!cpudb_valid(db_refs[db_idx]) ||
            db_refs[db_idx]->local_entry == NULL) {
            cli_out("Invalid DB index.  Try cpudb create\n");
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
        }
        db_ref = db_refs[db_idx];
        sal_memcpy(&st_cfg.base, &db_ref->local_entry->base,
                   sizeof(cpudb_base_t));
        rv = bcm_st_config_load(&st_cfg);
        cli_out("bcm_st_config_load returned %d\n", rv);
        parse_arg_eq_done(&pt);
        return CMD_OK;
    } else if (sal_strcasecmp(subcmd, "TOPO") == 0) {
        topo_cpu_t *topo;

        if (bcm_board_topo_get(&topo) == BCM_E_NONE && topo != NULL) {
            topo_cpu_dump(topo, "");
        } else {
            sal_printf("No Topology\n");
        }
    } else if (sal_strcasecmp(subcmd, "TIMEOUT") == 0) {
        int state = -1;
        sal_usecs_t timeout = 0;
        int rv;

        assert(sizeof (timeout) == sizeof (int));

        parse_table_init(unit, &pt);
        parse_table_add(&pt, "State", PQ_DFL|PQ_MULTI, 0, &state,
                        bcm_st_state_strings);
        parse_table_add(&pt, "TimeOut", PQ_DFL|PQ_INT, 0, &timeout, 0);
        if (parse_arg_eq(args, &pt) < 0) {
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        parse_arg_eq_done(&pt);
        if (state < 0) {
            for (state = BCM_STS_BLOCKED; state < BCM_STS_MAX; state++) {
                bcm_st_timeout_get((bcm_st_state_t)state, &timeout);
                cli_out("Timeout for %s is %d\n", bcm_st_state_strings[state],
                        timeout);
            }
            return CMD_OK;
        }
        rv = bcm_st_timeout_set((bcm_st_state_t)state, timeout);
        cli_out("Set timeout for %s to %d; returns %d\n",
                bcm_st_state_strings[state], timeout, rv);
        return CMD_OK;
    } else if (sal_strcasecmp(subcmd, "STOP") == 0) {
        /* Use stktask attach timeout as a proxy for stop timeouts.
           The should make this work in simulation and real modes. */
        tks_st_stop_thread_arg_t stop_arg;
        sal_thread_t st_stop_tid = SAL_THREAD_ERROR;
        sal_usecs_t attach_timeout; 
        int i;

        if (bcm_st_timeout_get(BCM_STS_ATTACH,
                               &attach_timeout) != BCM_E_NONE) {
            cli_out("Could not get attach timeout\n");
            return CMD_FAIL;
        }

        stop_arg.timeout = attach_timeout/50;
        stop_arg.rv      = BCM_E_INTERNAL;

        if (st_tid == SAL_THREAD_ERROR) {
            cli_out("Stack Task is not running\n");
            return CMD_OK;
        }
        
        st_stop_tid = sal_thread_create("STSTOP",
                                   SAL_THREAD_STKSZ,
                                   st_pri,
                                   tks_st_stop_thread,
                                   (void *)&stop_arg);

        if (st_stop_tid == SAL_THREAD_ERROR) {
            cli_out("Could not create stack stop thread\n");
            return CMD_FAIL;
        }
        
        /* Wait for the stack thread to signal completion */
        if (sal_sem_take(st_done_sem, attach_timeout*3) < 0) {
            cli_out("Stack thread failed to signal completion\n");
            return CMD_FAIL;
        } else {
            /* Wait for twice the bcm_st_stop timeout for the stop
               thread to complete */

#define TKS_STOP_THREAD_POLL_FRACTION 10
            
            for ( i=0; i<2*TKS_STOP_THREAD_POLL_FRACTION; i++ ) {
                if (stop_arg.rv == BCM_E_INTERNAL) {
                    sal_usleep(stop_arg.timeout/TKS_STOP_THREAD_POLL_FRACTION);
                } else {
                    break;
                }
            }
        }

        if (stop_arg.rv == BCM_E_INTERNAL) {
            cli_out("Stack stop thread failed to signal completion\n");
            return CMD_FAIL;
        }
        
        cli_out("bcm_st_stop returns %d: %s\n",
                stop_arg.rv, bcm_errmsg(stop_arg.rv));


    } else if (sal_strcasecmp(subcmd, "EVENT") == 0) {
        bcm_st_event_t event = BCM_STE_DISC_RESTART;
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "Event", PQ_DFL | PQ_MULTI, 0, &event,
                        event_list);
        if (parse_arg_eq(args, &pt) < 0) {
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        rv = bcm_st_event_send(event);
        if (rv < 0) {
            cli_out("bcm_st_send_event ERROR: %s\n", bcm_errmsg(rv));
        }
        parse_arg_eq_done(&pt);
    } else if (sal_strcasecmp(subcmd, "NOATTACH") == 0) {
        st_cfg.st_attach = null_attach;
#if defined(INCLUDE_ATPTRANS_SOCKET) && defined(INCLUDE_CHASSIS)
    } else if (sal_strcasecmp(subcmd, "CHASSIS_ATPTRANS_SOCKET") == 0) {
        st_cfg.st_transition = bcm_board_chassis_atptrans_socket_transition;
#endif
#if defined(ST_EVLOG)
    } else if (sal_strcasecmp(subcmd, "EVLOG") == 0) {
        if ((subcmd = ARG_GET(args)) == NULL) {
            return CMD_USAGE;
        }
        if (sal_strcasecmp(subcmd, "CLEAR") == 0) {
            evlog_clear();
        } else if (sal_strcasecmp(subcmd, "SHOW") == 0) {
            evlog_show(args);
        }
#endif /* ST_EVLOG */
    } else if (sal_strcasecmp(subcmd, "TX") == 0) {
        /* params */
        int op = BCM_HG_OPCODE_CPU;
        int blk = 1;
        bcm_pbmp_t dest;
        int dmod = 0;
        int dport = 0;
        int tagged = 1;
        int length = 68;
        int reply=0;
        int async=0; /* 0=sync 1=async per list 2=async per packet*/
        int list=0;
        int array=0;
        int ether=0;
        int crc=1;
        int txunit=-1;
        /* vars */
        uint32 flags = 0;
        bcm_pkt_t *pkt = NULL;
        bcm_pkt_t *cur_pkt = NULL;
        bcm_pkt_t *prev_pkt = NULL;
        bcm_pkt_t **pkt_array = NULL;
        int i;
        uint8 *buf = NULL;
        int packets;
        int cmd_rv = CMD_FAIL;

        BCM_PBMP_CLEAR(dest);
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "Op", PQ_DFL|PQ_INT, 0, &op, 0);
        parse_table_add(&pt, "Blk", PQ_DFL|PQ_INT, 0, &blk, 0);
        parse_table_add(&pt, "PortBitMap", PQ_DFL|PQ_PBMP, 0, &dest, 0);
        parse_table_add(&pt, "DMod", PQ_DFL|PQ_INT, 0, &dmod, 0);
        parse_table_add(&pt, "DPort", PQ_DFL|PQ_INT, 0, &dport, 0);
        parse_table_add(&pt, "Tagged", PQ_DFL|PQ_INT, 0, &tagged, 0);
        parse_table_add(&pt, "Length", PQ_DFL|PQ_INT, 0, &length, 0);
        parse_table_add(&pt, "Reply", PQ_DFL|PQ_INT, 0, &reply, 0);
        parse_table_add(&pt, "Async", PQ_DFL|PQ_INT, 0, &async, 0);
        parse_table_add(&pt, "LIst", PQ_DFL|PQ_INT, 0, &list, 0);
        parse_table_add(&pt, "ARray", PQ_DFL|PQ_INT, 0, &array, 0);
        parse_table_add(&pt, "Ether", PQ_DFL|PQ_INT, 0, &ether, 0);
        parse_table_add(&pt, "Crc", PQ_DFL|PQ_INT, 0, &crc, 0);
        parse_table_add(&pt, "TXUnit", PQ_DFL|PQ_INT, 0, &txunit, 0);

        if (parse_arg_eq(args, &pt) < 0) {
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }

        parse_arg_eq_done(&pt);

        if (blk < 1) {
            cli_out("blk must be greater than 0\n");
            return CMD_USAGE;
        }

        if (ether) {
            flags |= BCM_TX_ETHER;
        }

        if (crc) {
            flags |= BCM_TX_CRC_APPEND;
        }

        if (tagged) {
            length += 4;
            buf = sal_alloc(length, "sttx");
            if (!buf) {
                cli_out("no memory\n");
                goto done;
            }
            for (i=0; i<12; i++) {
                buf[i] = i;
            }
            buf[12] = 0x81;
            buf[13] = 0x00;
            buf[14] = 0x00;
            buf[15] = 0x01;
            for (i=16; i<length; i++) {
                buf[i] = i-4;
            }
        } else {
            buf = sal_alloc(length, "sttx");
            if (!buf) {
                cli_out("no memory\n");
                cmd_rv = CMD_FAIL;
                goto done;
            }
            for (i=0; i<length; i++) {
                buf[i] = i;
            }
        }

        if (reply) {
            int tmp;
            /* swap SA/DA */
            if (length < 12) {
                cli_out("length %d too short\n", length);
                cmd_rv = CMD_FAIL;
                goto done;
            }
            for (i=0; i<6; i++) {
                tmp = buf[i];
                buf[i] = buf[i+6];
                buf[i+6] = tmp;
            }
        }

        if (array > 0 && list > 0) {
            cli_out("Cannot specify array and list at the same time\n");
            cmd_rv = CMD_FAIL;
            goto done;
        }

        packets = array ? array : (list ? list : 1);

        if (array) {
            pkt_array = sal_alloc(packets * sizeof(bcm_pkt_t *), "sttx");
        }

        if (txunit < 0) {
            txunit = unit;
        }

        for (i=0; i < packets; i++) {
            rv = tks_pkt_alloc(txunit, blk, length, flags, &cur_pkt);
            if (BCM_FAILURE(rv)) {
                cli_out("tks_pkt_alloc: %s %d\n", bcm_errmsg(rv),rv);
                cmd_rv = CMD_FAIL;
                goto done;
            }
            cur_pkt->dest_mod = dmod;
            cur_pkt->dest_port = dport;
            cur_pkt->opcode = op;
            BCM_PBMP_ASSIGN(cur_pkt->tx_pbmp, dest);
            if (tagged) {
                BCM_PBMP_CLEAR(cur_pkt->tx_upbmp);
            } else {
                cur_pkt->_vtag[0] = 0x81;
                cur_pkt->_vtag[1] = 0x00;
                cur_pkt->_vtag[2] = 0x00;
                cur_pkt->_vtag[3] = 0x01;
                BCM_PBMP_ASSIGN(cur_pkt->tx_upbmp, dest);
                BCM_PKT_NO_VTAG_REQUIRE(cur_pkt);
            }
            rv = bcm_pkt_memcpy(cur_pkt, 0, buf, length);
            if (BCM_FAILURE(rv)) {
                cli_out("bcm_pkt_memcpy: %s %d\n", bcm_errmsg(rv),rv);
                cmd_rv = CMD_FAIL;
                goto done;
            }
            if ((array > 0 || list > 0) && async > 1) {
                /* For tx_list() and async > 1, use per packet callback */
                cur_pkt->call_back = tks_pkt_callback;
            }

            /* Keep linked list for packet free */
            if (pkt == NULL) {
                pkt = cur_pkt;
            }
            if (prev_pkt != NULL) {
                prev_pkt->next = cur_pkt;
            }
            prev_pkt = cur_pkt;

            if (array) {
                pkt_array[i] = cur_pkt;
            }
        }

        if (async) {
            if (list) {
                cli_out("async tx list:\n");
                for (cur_pkt = pkt; cur_pkt; cur_pkt=cur_pkt->next) {
                    cli_out("  %p\n", (void *)cur_pkt);
                }
                rv = bcm_tx_list(txunit, pkt, tks_list_callback, (void *)2);
            } else if (array) {
                cli_out("async tx array:\n");
                for (i = 0; i < packets; i++) {
                    cli_out("  %p\n", (void *)pkt_array[i]);
                }
                rv = bcm_tx_array(txunit, pkt_array, packets,
                                  tks_array_callback, (void *)pkt_array);
                if (BCM_SUCCESS(rv)) {
                    pkt_array = NULL; /* Freed by callback */
                }
            } else {
                cli_out("async tx pkt %p\n", (void *)pkt);
                pkt->call_back = tks_pkt_callback;
                rv = bcm_tx(txunit, pkt, (void *)1);
            }
        } else {
            if (list) {
                rv = bcm_tx_list(txunit, pkt, NULL, NULL);
            } else if (array) {
                rv = bcm_tx_array(txunit, pkt_array, packets, NULL, NULL);
            } else {
                rv = bcm_tx(txunit, pkt, NULL);
            }
        }
        if (BCM_FAILURE(rv)) {
            cli_out("bcm tx: %s %d\n", bcm_errmsg(rv),rv);
            cmd_rv = CMD_FAIL;
            goto done;
        }
        if (!async) {
            bcm_pkt_t *next_pkt;
            for (cur_pkt = pkt; cur_pkt != NULL; cur_pkt=next_pkt) {
                next_pkt = cur_pkt->next;
                rv = tks_pkt_free(txunit, cur_pkt);
                if (BCM_FAILURE(rv)) {
                    cli_out("tks_pkt_free: %s %d\n", bcm_errmsg(rv),rv);
                    cmd_rv = CMD_FAIL;
                    goto done;
                }
            }
        }
        cmd_rv = CMD_OK;

    done:
        if (buf) {
            sal_free(buf);
        }
        if (pkt_array) {
            sal_free(pkt_array);
        }
        
        return cmd_rv;
    } else if (sal_strcasecmp(subcmd, "MODPORT") == 0) {
        /* params */
        int modid = 0;
        int dest;
        
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "Modid", PQ_DFL|PQ_INT, 0, &modid, 0);
        parse_table_add(&pt, "Port", PQ_DFL|PQ_INT, 0, &dest, 0);
        if (parse_arg_eq(args, &pt) < 0) {
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        parse_arg_eq_done(&pt);
        rv = bcm_stk_modport_set(unit, modid, dest);
        if (BCM_FAILURE(rv)) {
            cli_out("bcm_stk_modport_set: %s %d\n", bcm_errmsg(rv),rv);
            return CMD_FAIL;
        }
        return CMD_OK;
    } else if (sal_strcasecmp(subcmd, "MODID") == 0) {
        /* params */
        int modid = 0;
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "Modid", PQ_DFL|PQ_INT, 0, &modid, 0);
        if (parse_arg_eq(args, &pt) < 0) {
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        parse_arg_eq_done(&pt);
        rv = bcm_stk_my_modid_set(unit, modid);
        if (BCM_FAILURE(rv)) {
            cli_out("bcm_stk_my_modid_set: %s %d\n", bcm_errmsg(rv),rv);
            return CMD_FAIL;
        }
        return CMD_OK;
    } else {
        cli_out("Subcommand not found: %s\n", subcmd);
        return CMD_USAGE;
    }

    return CMD_OK;
}


/****************************************************************
 *
 * CPUDB DUMP AND PARSE ROUTINES
 *
 * DB dump and parse routines work in tandem.  After configuration,
 * the current stack task DB can be dumped.  This creates a script
 * (a list of dbparse commands) that can be read in on a different
 * system to recreate the DB and topology information.
 *
 * The parsing is stateful and the order of the dbparse commands
 * is very important.  For example, 'dbparse object=cpudb_stk_port...'
 * gives info about a stack port; but which stack port is implied
 * in the current state of the parsing (what was the last DB
 * entry created and which stack ports were added previously).
 *
 * The DB parse routines will install the data into the current
 * CPU DB (from cpudb create) and into parse_tp_cpu.
 */

static topo_cpu_t *parse_tp_cpu;

#define PARSE_TP_CPU_INIT \
    if (parse_tp_cpu == NULL) { \
        parse_tp_cpu = sal_alloc(sizeof(*parse_tp_cpu), "parse_tp_cpu"); \
        sal_memset(parse_tp_cpu, 0, sizeof(*parse_tp_cpu)); \
    }

/* Dump a CPUDB entry's stack port */
void
stk_ports_dump(cpudb_entry_t *entry, char *prefix)
{
    cpudb_stk_port_t *sp;
    int i;

    for (i = 0; i < entry->base.num_stk_ports; i++) {
        sp = &entry->sp_info[i];
        sal_printf("%s object=cpudb_stk_port \\\n"
                   "    weight=0x%x \\\n"
                   "    base_flags=0x%x \\\n"
                   "    flags=0x%x \\\n"
                   "    unit=%d \\\n"
                   "    port=%d \\\n"
                   "    tx_cpu_key=%x:%x:%x:%x:%x:%x \\\n"
                   "    tx_stk_idx=%d \\\n"
                   "    rx_cpu_key=%x:%x:%x:%x:%x:%x \\\n"
                   "    rx_stk_idx=%d\n",
                   prefix,
                   entry->base.stk_ports[i].weight,
                   entry->base.stk_ports[i].bflags,
                   sp->flags,
                   entry->base.stk_ports[i].unit,
                   entry->base.stk_ports[i].port,
                   MAC_ADDR_LIST(sp->tx_cpu_key.key),
                   sp->tx_stk_idx,
                   MAC_ADDR_LIST(sp->rx_cpu_key.key),
                   sp->rx_stk_idx);
    }
}

/* Dump a CPUDB entry's stack port */
void
cpudb_units_dump(cpudb_entry_t *entry, char *prefix)
{
    int i;

    for (i = 0; i < entry->base.num_units; i++) {
        sal_printf("%s object=cpudb_unit_mod_ids \\\n"
                   "    mod_ids_req=%d \\\n"
                   "    pref_mod_id=%d \\\n"
                   "    mod_id=%d\n",
                   prefix,
                   entry->base.mod_ids_req[i],
                   entry->base.pref_mod_id[i],
                   entry->mod_ids[i]);
    }
}

/* Dump a CPUDB entry */

void
cpudb_entry_dump(cpudb_entry_t *entry, char *prefix)
{
    cpudb_base_t *base;

    base = &entry->base;
    sal_printf("%s object=cpudb_entry key=%x:%x:%x:%x:%x:%x \\\n"
               "    mac=%x:%x:%x:%x:%x:%x \\\n"
               "    master_pri=%d \\\n"
               "    slot_id=%d \\\n"
               "    dest_unit=%d \\\n"
               "    base_dest_port=%d \\\n"
               "    flags=0x%x \\\n"
               "    tx_unit=%d \\\n"
               "    tx_port=%d \\\n"
               "    dest_mod=%d \\\n"
               "    dest_port=%d \\\n"
               "    topo_idx=%d\n",
               prefix,
               MAC_ADDR_LIST(base->key.key),
               MAC_ADDR_LIST(base->mac),
               base->master_pri,
               base->slot_id,
               base->dest_unit,
               base->dest_port,
               entry->flags,
               entry->tx_unit,
               entry->tx_port,
               entry->dest_mod,
               entry->dest_port,
               entry->topo_idx);
    stk_ports_dump(entry, prefix);
    cpudb_units_dump(entry, prefix);
}

/* Dump topology mod id lists for a stack port */

void
topo_stk_port_dump(topo_stk_port_t *sp, char *prefix)
{
    int i;

    sal_printf("%s object=topo_stk_port flags=0x%x\n",
               prefix,
               sp->flags);
    for (i = 0; i < sp->tx_mod_num; i++) {
        sal_printf("%s object=topo_tx_mod mod_id=%d\n",
                   prefix,
                   sp->tx_mods[i]);
    }
    for (i = 0; i < sp->rx_mod_num; i++) {
        sal_printf("%s object=topo_rx_mod mod_id=%d\n",
                   prefix,
                   sp->rx_mods[i]);
    }
}

/* Dump a topology CPU structure */

void
topo_cpu_dump(topo_cpu_t *topo_cpu, char *prefix)
{
    int i;

    sal_printf("%s object=topo_cpu \\\n", prefix);
    sal_printf("    key=%x:%x:%x:%x:%x:%x \\\n",
               MAC_ADDR_LIST(topo_cpu->local_entry.base.key.key));
    sal_printf("    version=%d \\\n"
               "    master_seq_num=%d \\\n"
               "    flags=0x%x\n",
               topo_cpu->version,
               topo_cpu->master_seq_num,
               topo_cpu->flags);
    for (i = 0; i < topo_cpu->local_entry.base.num_stk_ports; i++) {
        topo_stk_port_dump(&(topo_cpu->tp_stk_port[i]), prefix);
    }
    sal_printf("\n# End of topology dump.\n\n");
}

/* Dump a CPUDB including topology info */

void
cpudb_dump(cpudb_ref_t db, char *prefix)
{
    cpudb_entry_t *entry;

    if (!cpudb_valid(db)) {
        cli_out("DB is not valid\n");
        return;
    }

    sal_printf("# Dump CPU DB\n");
    sal_printf("# WARNING:  DB parsing is stateful and order specific.\n");
    sal_printf("# WARNING:  Changing the order of dbparse commands\n");
    sal_printf("# WARNING:  will change or break the resulting parse\n\n");
    CPUDB_FOREACH_ENTRY(db, entry) {
        cpudb_entry_dump(entry, prefix);
    }

    if (db->master_entry != NULL) {
        sal_printf("%s object=master_key key=%x:%x:%x:%x:%x:%x\n",
                   prefix,
                   MAC_ADDR_LIST(db->master_entry->base.key.key));
    }

    if (db->local_entry != NULL) {
        sal_printf("%s object=local_key key=%x:%x:%x:%x:%x:%x\n",
                   prefix,
                   MAC_ADDR_LIST(db->local_entry->base.key.key));
    }
    sal_printf("\n# End of CPUDB dump.\n");
}

char cmd_st_db_dump_usage[] =
    "Dump the current stack task CPUDB in a parsable format\n";

cmd_result_t
cmd_st_db_dump(int ignored_unit, args_t *args)
{
    if (!cpudb_valid(bcm_st_cur_db)) {
        cli_out("Stack task current DB is not valid\n");
        return CMD_OK;
    }

    sal_printf("\n# Dumping current stack task CPUDB\n\n");
    cpudb_dump(bcm_st_cur_db, "dbparse");
    sal_printf("\n\n# Clear topology info before parsing\n");
    sal_printf("dbparse object=topo_clear\n");

    return CMD_OK;
}

static char *parse_object_list[] = { PARSE_OBJECT_NAMES };

char cmd_cpudb_parse_usage[] =
    "Parse a CPUDB object command generated by CPUDB dump\n";

static cpudb_key_t current_key;
static int current_topo_sp_idx;

#define CHECK_SET(dest, val) if ((val) >= 0) (dest) = (val)

/* Add stack port to the given entry */
STATIC int _stk_port_add(cpudb_entry_t *entry, int unit, bcm_port_t port,
                         int weight, int base_flags,
                         int flags, int tx_stk_idx, int rx_stk_idx,
                         cpudb_key_t tx_cpu_key, cpudb_key_t rx_cpu_key)
{
    int cur_sp;
    cpudb_stk_port_t *sp;
    
    if (entry->base.num_stk_ports >= CPUDB_STK_PORTS_MAX) {
        cli_out("ERROR: failed to add stack port unit=%d port=%d to cpudb "
                "(max=%d)\n", unit, port, CPUDB_STK_PORTS_MAX);
        return BCM_E_FAIL;
    }
    
    cur_sp = entry->base.num_stk_ports++;
    entry->base.stk_ports[cur_sp].weight = weight;
    entry->base.stk_ports[cur_sp].bflags = base_flags;
    entry->base.stk_ports[cur_sp].unit = unit;
    entry->base.stk_ports[cur_sp].port = port;
    sp = &entry->sp_info[cur_sp];
    sp->flags = flags;
    CPUDB_KEY_COPY(sp->tx_cpu_key, tx_cpu_key);
    sp->tx_stk_idx = tx_stk_idx;
    CPUDB_KEY_COPY(sp->rx_cpu_key, rx_cpu_key);
    sp->rx_stk_idx = rx_stk_idx;

    return BCM_E_NONE;
}

/*
 * Parse a port specification string of unit, port pairs, adding each
 * as a stack port to the given entry.  See notes at unit_port_pair_parse.
 */

STATIC cmd_result_t
_port_string_parse(cpudb_entry_t *entry, char *port_str)
{
    int unit;
    bcm_port_t port;
    static cpudb_key_t empty_key;  /* Initialized to 0 in BSS */

    if (port_str == NULL) {
        return CMD_FAIL;
    }

    port_str = _next_digit(port_str);
    do {
        port_str = unit_port_pair_parse(port_str, &unit, &port);
        if (port_str != NULL) {
            if (BCM_FAILURE(_stk_port_add(entry, unit, port, 0, 0, 0,
                                          -1, -1, empty_key, empty_key))) {
                return CMD_FAIL;
            }
            port_str = _next_digit(port_str);
        } else {
            cli_out("Error parsing port string: %s\n", port_str);
        }
    } while ((port_str != NULL) && (*port_str != 0));

    return port_str == NULL ? CMD_FAIL : CMD_OK;
}

/*
 * Requires the notion of a "current DB" (fixed for entire parsing)
 * and "current DB entry" (updated by parsing routine)
 */

cmd_result_t
cmd_cpudb_parse(int ignored_unit, args_t *args)
{
    parse_table_t pt;
    int object;
    int base_dest_port = -1,
        dest_mod = -1,
        dest_port = -1,
        dest_unit = -1,
        flags = 0,
        weight = 0,
        base_flags = 0,
        master_pri = -1,
        master_seq_num = -1,
        mod_id = -1,
        mod_ids_req = -1,
        num_stk_ports = -1,
        port = -1,
        pref_mod_id = -1,
        rx_stk_idx = -1,
        slot_id = -1,
        topo_idx = -1,
        tx_port = -1,
        tx_stk_idx = -1,
        tx_unit = -1,
        unit = -1,
        version = -1;
    cpudb_key_t key, rx_cpu_key, tx_cpu_key;
    bcm_mac_t mac;
    cpudb_ref_t db_ref;
    cpudb_entry_t *entry;
    cpudb_base_t *base;
    topo_stk_port_t *topo_sp;
    cmd_result_t cmd_rv = CMD_OK;
    int cur_unit;
    int db_idx = cur_db;
    char *sp_string;
    int rv;

    sal_memset(&key, 0, sizeof(cpudb_key_t));
    sal_memset(&rx_cpu_key, 0, sizeof(cpudb_key_t));
    sal_memset(&tx_cpu_key, 0, sizeof(cpudb_key_t));
    sal_memset(mac, 0, sizeof(bcm_mac_t));

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "OBJECT", PQ_DFL | PQ_MULTI, 0, &object,
                    parse_object_list);
    parse_table_add(&pt, "DB_IDX", PQ_DFL | PQ_INT, 0, &db_idx, 0);
    parse_table_add(&pt, "BASE_DEST_PORT", PQ_DFL | PQ_INT, 0, &base_dest_port, 0);
    parse_table_add(&pt, "BASE_FLAGS", PQ_DFL | PQ_INT, 0, &base_flags, 0);
    parse_table_add(&pt, "DEST_MOD", PQ_DFL | PQ_INT, 0, &dest_mod, 0);
    parse_table_add(&pt, "DEST_PORT", PQ_DFL | PQ_INT, 0, &dest_port, 0);
    parse_table_add(&pt, "DEST_UNIT", PQ_DFL | PQ_INT, 0, &dest_unit, 0);
    parse_table_add(&pt, "FLAGS", PQ_DFL | PQ_INT, 0, &flags, 0);
    parse_table_add(&pt, "KEY", PQ_DFL | PQ_KEY, 0, key.key, 0);
    parse_table_add(&pt, "MAC", PQ_DFL | PQ_MAC, 0, mac, 0);
    parse_table_add(&pt, "MASTER_PRI", PQ_DFL | PQ_INT, 0, &master_pri, 0);
    parse_table_add(&pt, "MASTER_SEQ_NUM", PQ_DFL | PQ_INT, 0, &master_seq_num, 0);
    parse_table_add(&pt, "MOD_ID", PQ_DFL | PQ_INT, 0, &mod_id, 0);
    parse_table_add(&pt, "MOD_IDS_REQ", PQ_DFL | PQ_INT, 0, &mod_ids_req, 0);
    parse_table_add(&pt, "NUM_STK_PORTS", PQ_DFL | PQ_INT, 0, &num_stk_ports, 0);
    parse_table_add(&pt, "PORT", PQ_DFL | PQ_INT, 0, &port, 0);
    parse_table_add(&pt, "PREF_MOD_ID", PQ_DFL | PQ_INT, 0, &pref_mod_id, 0);
    parse_table_add(&pt, "RX_CPU_KEY", PQ_DFL | PQ_KEY, 0, rx_cpu_key.key, 0);
    parse_table_add(&pt, "RX_STK_IDX", PQ_DFL | PQ_INT, 0, &rx_stk_idx, 0);
    parse_table_add(&pt, "SLOT_ID", PQ_DFL | PQ_INT, 0, &slot_id, 0);
    parse_table_add(&pt, "SP_STRING", PQ_STRING, 0, &sp_string, 0);
    parse_table_add(&pt, "TOPO_IDX", PQ_DFL | PQ_INT, 0, &topo_idx, 0);
    parse_table_add(&pt, "TX_CPU_KEY", PQ_DFL | PQ_KEY, 0, tx_cpu_key.key, 0);
    parse_table_add(&pt, "TX_PORT", PQ_DFL | PQ_INT, 0, &tx_port, 0);
    parse_table_add(&pt, "TX_STK_IDX", PQ_DFL | PQ_INT, 0, &tx_stk_idx, 0);
    parse_table_add(&pt, "TX_UNIT", PQ_DFL | PQ_INT, 0, &tx_unit, 0);
    parse_table_add(&pt, "UNIT", PQ_DFL | PQ_INT, 0, &unit, 0);
    parse_table_add(&pt, "VERSION", PQ_DFL | PQ_INT, 0, &version, 0);
    parse_table_add(&pt, "WEIGHT", PQ_DFL | PQ_INT, 0, &weight, 0);

    if (parse_arg_eq(args, &pt) < 0) {
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }

    if (db_idx < 0 || db_idx >= MAX_DBS) {
        parse_arg_eq_done(&pt);
        sal_printf("Bad db index %d\n", db_idx);
        return CMD_USAGE;
    }
    db_ref = db_refs[db_idx];
    if (!cpudb_valid(db_ref)) {
        parse_arg_eq_done(&pt);
        sal_printf("DB reference %d is not valid\n", db_idx);
        return CMD_USAGE;
    }

    switch (object) {
    case PARSE_OBJ_CPUDB_ENTRY    :  /* Sets current key */
        CPUDB_KEY_SEARCH(db_ref, key, entry);
        if (entry != NULL) {
            sal_printf("CPUDB PARSE ENTRY:  WARNING key already in DB "
                       CPUDB_KEY_FMT_EOLN, CPUDB_KEY_DISP(key));
        }
        CPUDB_KEY_COPY(current_key, key);
        entry = cpudb_entry_create(db_ref, key, FALSE);
        if (entry == NULL) {
            sal_printf("CPUDB PARSE ENTRY:  ERROR failed to create entry for "
                       CPUDB_KEY_FMT_EOLN, CPUDB_KEY_DISP(key));
            cmd_rv = CMD_FAIL;
            break;
        }
        base = &entry->base;
        sal_memcpy(entry->base.mac, mac, sizeof(bcm_mac_t));

        CHECK_SET(entry->flags, flags);
        CHECK_SET(base->master_pri, master_pri);
        CHECK_SET(base->slot_id, slot_id);
        CHECK_SET(base->dest_unit, dest_unit);
        CHECK_SET(base->dest_port, base_dest_port);
        CHECK_SET(entry->tx_unit, tx_unit);
        CHECK_SET(entry->tx_port, tx_port);
        CHECK_SET(entry->dest_mod, dest_mod);
        CHECK_SET(entry->dest_port, dest_port);
        CHECK_SET(entry->topo_idx, topo_idx);
        entry->base.num_units = 0;
        entry->base.num_stk_ports = 0;
        break;

    case PARSE_OBJ_CPUDB_UNIT_MOD_IDS :
        CPUDB_KEY_SEARCH(db_ref, current_key, entry);
        if (entry == NULL) {
            sal_printf("CPUDB PARSE UNIT_MOD_IDS:  Current DB key not found\n");
            cmd_rv = CMD_FAIL;
            break;
        }
        if (entry->base.num_units >= CPUDB_UNITS_MAX) {
            sal_printf("CPUDB PARSE UNIT_MOD_IDS: Cannot exceed %d units\n",
                       CPUDB_UNITS_MAX);
            cmd_rv = CMD_FAIL;
            break;
        }
        cur_unit = entry->base.num_units++;
        entry->mod_ids[cur_unit] = mod_id;
        entry->base.pref_mod_id[cur_unit] = pref_mod_id;
        entry->base.mod_ids_req[cur_unit] = mod_ids_req;
        break;

    case PARSE_OBJ_CPUDB_STK_PORT :
        CPUDB_KEY_SEARCH(db_ref, current_key, entry);
        if (entry == NULL) {
            sal_printf("CPUDB PARSE STK_PORT:  Current DB key not found\n");
            cmd_rv = CMD_FAIL;
            break;
        }
        if (BCM_FAILURE(_stk_port_add(entry, unit, port, weight, base_flags,
                                      flags, tx_stk_idx, rx_stk_idx,
                                      tx_cpu_key, rx_cpu_key))) {
            cmd_rv = CMD_FAIL;
        }
        break;

    case PARSE_OBJ_CPUDB_STK_PORT_STR :
        CPUDB_KEY_SEARCH(db_ref, current_key, entry);
        if (entry == NULL) {
            sal_printf("CPUDB PARSE STK_PORT_STR:  "
                       "Current DB key not found\n");
            cmd_rv = CMD_FAIL;
            break;
        }
        cmd_rv = _port_string_parse(entry, sp_string);
        break;

    case PARSE_OBJ_MASTER_KEY     :
        CPUDB_KEY_SEARCH(db_ref, key, entry);
        if (entry == NULL) {
            sal_printf("CPUDB PARSE MASTER KEY:  Entry not found: "
                       CPUDB_KEY_FMT_EOLN, CPUDB_KEY_DISP(key));
            cmd_rv = CMD_FAIL;
            break;
        }
        db_ref->master_entry = entry;
        break;

    case PARSE_OBJ_LOCAL_KEY      :
        CPUDB_KEY_SEARCH(db_ref, key, entry);
        if (entry == NULL) {
            sal_printf("CPUDB PARSE LOCAL KEY:  Entry not found: "
                       CPUDB_KEY_FMT_EOLN, CPUDB_KEY_DISP(key));
            cmd_rv = CMD_FAIL;
            break;
        }
        db_ref->local_entry = entry;
        break;

    case PARSE_OBJ_TOPO_CPU       :
        CPUDB_KEY_SEARCH(db_ref, key, entry);
        if (entry == NULL) {
            sal_printf("CPUDB PARSE LOCAL KEY:  Entry not found: "
                       CPUDB_KEY_FMT_EOLN, CPUDB_KEY_DISP(key));
            cmd_rv = CMD_FAIL;
            break;
        }
        PARSE_TP_CPU_INIT;
        cpudb_entry_copy(&parse_tp_cpu->local_entry, entry);
        CHECK_SET(parse_tp_cpu->version, version);
        CHECK_SET(parse_tp_cpu->master_seq_num, master_seq_num);
        CHECK_SET(parse_tp_cpu->flags, flags);
        current_topo_sp_idx = -1;
        break;

    case PARSE_OBJ_TOPO_STK_PORT  :
        current_topo_sp_idx++;
        if (current_topo_sp_idx >= CPUDB_STK_PORTS_MAX) {
            sal_printf("CPUDB PARSE TOPO_STK_PORT: Cannot exceed %d stack "
                       "ports\n", CPUDB_STK_PORTS_MAX);
            cmd_rv = CMD_FAIL;
            break;
        }
        PARSE_TP_CPU_INIT;
        topo_sp = &parse_tp_cpu->tp_stk_port[current_topo_sp_idx];
        CHECK_SET(topo_sp->flags, flags);
        break;
    case PARSE_OBJ_TOPO_TX_MOD    :
        PARSE_TP_CPU_INIT;
        topo_sp = &parse_tp_cpu->tp_stk_port[current_topo_sp_idx];
        if (topo_sp->tx_mod_num >= TOPO_MODIDS_MAX) {
            sal_printf("CPUDB PARSE TOPO_TX_MOD: Cannot exceed %d mod ids\n",
                       TOPO_MODIDS_MAX);
            cmd_rv = CMD_FAIL;
            break;
        } 
        topo_sp->tx_mods[(topo_sp->tx_mod_num)++] = mod_id;
        break;
    case PARSE_OBJ_TOPO_RX_MOD    :
        PARSE_TP_CPU_INIT;
        topo_sp = &parse_tp_cpu->tp_stk_port[current_topo_sp_idx];
        if (topo_sp->rx_mod_num >= TOPO_MODIDS_MAX) {
            sal_printf("CPUDB PARSE TOPO_RX_MOD: Cannot exceed %d mod ids\n",
                       TOPO_MODIDS_MAX);
            cmd_rv = CMD_FAIL;
            break;
        } 
        topo_sp->rx_mods[(topo_sp->rx_mod_num)++] = mod_id;
        break;

    case PARSE_OBJ_TOPO_CLEAR     :   /* Clear parse topology */
        PARSE_TP_CPU_INIT;
        sal_memset(parse_tp_cpu, 0, sizeof(*parse_tp_cpu));
        break;

    case PARSE_OBJ_TOPO_INSTALL  :
        /* not supported */
        break;

    case PARSE_OBJ_STACK_INSTALL  :
        rv = atp_db_update(db_ref);
        if (rv < 0) {
            cli_out("WARNING:  ATP DB update failed %d: %s\n",
                    rv, bcm_errmsg(rv));
        }
        rv = bcm_stack_attach_update(db_ref);
        if (rv < 0) {
            cli_out("ERROR:  stack attach returned %d: %s\n",
                    rv, bcm_errmsg(rv));
        }
        break;
    case PARSE_OBJ_LOCAL_ENTRY  :
        /* Make a few parameters settable for local entry */
        if (db_ref->local_entry == NULL) {
            cli_out("ERROR:  DB parse.  Local entry not set\n");
            cmd_rv = CMD_FAIL;
            break;
        }
        entry = db_ref->local_entry;
        base = &entry->base;
        CHECK_SET(entry->flags, flags);
        CHECK_SET(base->master_pri, master_pri);
        CHECK_SET(base->slot_id, slot_id);
        CHECK_SET(base->dest_unit, dest_unit);
        CHECK_SET(base->dest_port, base_dest_port);
        CHECK_SET(entry->tx_unit, tx_unit);
        CHECK_SET(entry->tx_port, tx_port);
        CHECK_SET(entry->dest_mod, dest_mod);
        CHECK_SET(entry->dest_port, dest_port);
        CHECK_SET(entry->topo_idx, topo_idx);
        break;
    default:
        sal_printf("Unknown object parse\n");
        cmd_rv = CMD_USAGE;
        break;
    }

    parse_arg_eq_done(&pt);
    return cmd_rv;
}        
        

#endif  /* INCLUDE_LIB_CPUDB */
